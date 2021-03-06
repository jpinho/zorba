/*
 * Copyright 2006-2013 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>

#include <zorba/util/base64_util.h>
#include <zorba/config.h>
#include <zorba/diagnostic_list.h>
#include <zorba/error.h>
#include <zorba/item.h>
#include <zorba/item_factory.h>
#include <zorba/util/transcode_stream.h>
#include <zorba/xmldatamanager.h>
#include <zorba/xquery_exception.h>
#include <zorba/xquery_exception.h>
#include <zorba/xquery_functions.h>
#include <zorba/internal/unique_ptr.h>

#include "util/curl_streambuf.h"

#include "http_response_parser.h"
#include "http_request_handler.h"

namespace zorba {

namespace http_client {

void parse_content_type( std::string const &media_type, std::string *mime_type,
                                std::string *charset )
{
  std::string::size_type start = 0;
  std::string::size_type end = media_type.find( ';' );
  *mime_type = media_type.substr( start, end - start );

  if ( std::strncmp( mime_type->c_str(), "text/", 5 ) == 0 ) {
    //
    // RFC 2616: "Hypertext Transfer Protocol -- HTTP/1.1," section 3.7.1,
    // "Canonicalization and Text Defaults":
    //
    //    The "charset" parameter is used with some media types to define the
    //    character set (section 3.4) of the data. When no explicit charset
    //    parameter is provided by the sender, media subtypes of the "text"
    //    type are defined to have a default charset value of "ISO-8859-1" when
    //    received via HTTP.
    //
    *charset = "ISO-8859-1";
  } else
    charset->clear();

  //media-type = type "/" subtype *( ";" parameter )
  //type = token
  //subtype = token
  //parameter = attribute "=" value
  //attribute = token
  //value = token | quoted-string
  //quoted-string  = ( <"> *(qdtext) <"> )
  //qdtext = <any TEXT except <">>
  std::vector<std::string> fields;
  while ((end = media_type.find(';',start)) != std::string::npos)
  {
    fields.push_back(media_type.substr(start,end-start));
    start = end+1;
  }
  fields.push_back(media_type.substr(start));

  std::vector<std::string>::iterator it = fields.begin();
  for (;it!=fields.end();++it)
  {
    std::string& field =*it;
    std::transform(field.begin(), field.end(), field.begin(), ::tolower);
    field.erase(remove_if(field.begin(), field.end(), ::isspace), field.end());
    if ((start = field.find("charset=")) != std::string::npos)
    {
      std::string t = field.substr(start + 8);
      if (!t.empty())
      {
        if (t[0] == '"' && t[t.length()-1] == '"')
        {
          t.erase( 0, 1 );
          t.erase(t.length() -1, 1);    
        }
        *charset = t;
      }
    }
  }
}

  HttpResponseParser::HttpResponseParser(HttpResponseHandler& aHandler, CURL* aCurl,
                                         ErrorThrower& aErrorThrower,
                                         std::string aOverridenContentType,
                                         bool aStatusOnly) : 
  theHandler(aHandler),
  theCurl(aCurl),
  theErrorThrower(aErrorThrower),
  theStatus(-1),
  theStreamBuffer(0),
  theInsideRead(false),
  theOverridenContentType(aOverridenContentType),
  theStatusOnly(aStatusOnly),
  theSelfContained(true)
  {
    registerHandler();
    theStreamBuffer = new zorba::curl::streambuf(theCurl);
  }

  HttpResponseParser::~HttpResponseParser()
  {
    delete theStreamBuffer;
  }

  CURLcode HttpResponseParser::parse()
  {
    theStreamBuffer->set_listener(this);
    theHandler.begin();
    bool lStatusAndMesssageParsed = false;
    CURLcode lCurlCode = theStreamBuffer->curl_multi_info_read(false);
    if (lCurlCode)
      return lCurlCode;

    if (!theStatusOnly)
    {
      if (!theOverridenContentType.empty())
      {
        parse_content_type(
          theOverridenContentType, &theCurrentContentType, &theCurrentCharset
        );
      }

      std::unique_ptr<std::istream> lStream;
      try
      {
        if ( !theCurrentCharset.empty() &&
             transcode::is_necessary( theCurrentCharset.c_str() ) )
        {
          lStream.reset(
            new transcode::stream<std::istream>(
              theCurrentCharset.c_str(), theStreamBuffer
            )
          );
        }
        else
          lStream.reset(new std::istream(theStreamBuffer));
      }
      catch ( std::invalid_argument const &e )
      {
        theErrorThrower.raiseException("http://www.zorba-xquery.com/errors", "ZXQP0006", e.what());
      }

      Item lItem;
      if (theCurrentContentType == "application/xml" ||
          theCurrentContentType == "application/xml-external-parsed-entity" ||
          theCurrentContentType == "application/json" ||
          theCurrentContentType == "application/x-javascript" ||
          theCurrentContentType == "text/javascript" ||
          theCurrentContentType == "text/x-javascript" ||
          theCurrentContentType == "text/x-json" ||
          (
            theCurrentContentType.length() > 5 &&
              (theCurrentContentType.find("+xml") == theCurrentContentType.size()-4 ||
              theCurrentContentType.find("+json") == theCurrentContentType.size()-5)
          ) ||
          theCurrentContentType.find("text/") == 0)
      {
        lItem = createTextItem(lStream.release());
      }
      else
      {
        lItem = createBase64Item(*lStream.get());
      }

      if (!lItem.isNull())
      {
        std::string empty;
        theHandler.any(lItem, empty);
      }

      if (!theInsideRead)
      {
        theHandler.beginResponse(theStatus, theMessage);
        lStatusAndMesssageParsed = true;
      }
      else
        theHandler.endBody();
    }

    if (!theInsideRead)
    {
      if (!lStatusAndMesssageParsed)
        theHandler.beginResponse(theStatus, theMessage);

      for (std::vector<std::pair<std::string, std::string> >::iterator i = theHeaders.begin();
          i != theHeaders.end(); ++i) {
        theHandler.header(i->first, i->second);
      }
    }

    theHandler.endResponse();
    theHandler.end();
    return lCurlCode;
  }

  void HttpResponseParser::curl_read(void*,size_t)
  {
    if (theInsideRead) {
      return;
    }
    theInsideRead = true;
    theHandler.beginResponse(theStatus, theMessage);
    for ( headers_type::const_iterator
          lIter = theHeaders.begin(); lIter != theHeaders.end(); ++lIter) {
      theHandler.header(lIter->first, lIter->second);
    }
    if (!theStatusOnly)
      theHandler.beginBody(theCurrentContentType, "", NULL);
  }

  void HttpResponseParser::registerHandler()
  {
    curl_easy_setopt(theCurl, CURLOPT_HEADERFUNCTION, &curl_headerfunction);
    curl_easy_setopt(theCurl, CURLOPT_HEADERDATA, this);
  }

  size_t HttpResponseParser::curl_headerfunction( void *ptr, size_t size,
                                                  size_t nmemb, void *data )
  {
    size_t lSize = size*nmemb;
    size_t lResult = lSize;
    HttpResponseParser* lParser = static_cast<HttpResponseParser*>(data);
    if (lParser->theInsideRead) {
      lParser->theHandler.endBody();
      lParser->theInsideRead = false;
    }
    const char* lDataChar = (const char*) ptr;
    while (lSize != 0 && (lDataChar[lSize - 1] == 10
          || lDataChar[lSize - 1] == 13)) {
      lSize--;
    }
    if (lSize == 0) {
      return lResult;
    }
    std::string lData(lDataChar, lSize);

    if (lData.find("HTTP") == 0) {
      lParser->parseStatusAndMessage(lData);
      return lResult;
    }
    std::string::size_type lPos = lData.find(':');
    if (lPos == std::string::npos) {
      return lResult;
    }
    std::string lName = lData.substr(0, lPos);
    std::string lValue = lData.substr(lPos + 2);
    {
      std::string::size_type lPosition = lValue.size() - 1;
      while (true) {
        if (lPosition != std::string::npos) {
          break;
        }
        if (lValue[lPosition] == '\n' || lValue[lPosition] == '\r') {
          --lPosition;
        } else {
          break;
        }
      }
      lValue = lValue.substr(0, lPosition + 1);
    }
    String lNameS = fn::lower_case( lName );
    if (lNameS == "content-type") {
      parse_content_type(
        lValue, &lParser->theCurrentContentType, &lParser->theCurrentCharset
      );
    } else if (lNameS == "content-id") {
      lParser->theId = lValue;
    } else if (lNameS == "content-description") {
      lParser->theDescription = lValue;
    }
    lParser->theHeaders.push_back(
      std::pair<std::string, std::string>(lName, lValue));
    return lResult;
  }

  void HttpResponseParser::parseStatusAndMessage(std::string const &aHeader)
  {
    std::string::size_type lPos = aHeader.find(' ');
    assert(lPos != std::string::npos);
    std::string lStatus = aHeader.substr(lPos, aHeader.find(' ', lPos + 1));
    theMessage = aHeader.substr(aHeader.find(' ', lPos + 1) + 1);
    {
      std::string::size_type lPosition = theMessage.size() - 1;
      while (true) {
        if (lPosition != std::string::npos) {
          break;
        }
        if (theMessage[lPosition] == '\n' || theMessage[lPosition] == '\r') {
          --lPosition;
        } else {
          break;
        }
      }
      theMessage = theMessage.substr(0, lPosition + 1);
    }
    std::stringstream lStream(lStatus);
    lStream >> theStatus;
    // everything that is not a valid http status is an error
    if (theStatus < 100) {
      theErrorThrower.raiseException("HTTP", "An HTTP error occurred. The returned status is: " + lStatus);
    }
  }

  static void streamReleaser(std::istream* aStream)
  {
    if (!aStream)
      return;

    // This istream contains our curl stream buffer, so we have to delete it too
    std::streambuf *const sbuf = aStream->rdbuf();
    if ( transcode::streambuf *tbuf =
          dynamic_cast<transcode::streambuf*>( sbuf ) )
      delete tbuf->orig_streambuf();
    else
      delete sbuf;
    delete aStream;
  }

  zorba::Item HttpResponseParser::createTextItem(std::istream* aStream)
  {
    ItemFactory* lFactory = Zorba::getInstance(0)->getItemFactory();

    // When we create a StreamableString, memory ownership gets very convoluted
    // because the StreamableString object has a longer lifecycle than the
    // iterator which creates it. The StreamableString object depends on its
    // istream, which in turn depends on its read buffer. For us, the read
    // buffer in turn depends on the HttpResponseParser (this object) because
    // it is registered as the "informer" (callback object) for
    // theStreamBuffer. Therefore, this HttpResponseParser object is no longer
    // "self-contained". We delegate ownership of ourself to theStreamBuffer
    // and mark ourselves as no longer being self-contained.
    theStreamBuffer->set_listener(this, true);
    theSelfContained = false;

    // The ownership of theStreamBuffer, in turn, is delegated to the
    // StreamableString object (via streamReleaser, which will free the
    // istream's rdbuf).
    theStreamBuffer = NULL;
    return lFactory->createStreamableString(*aStream, &streamReleaser, false);
  }

  zorba::Item HttpResponseParser::createBase64Item( std::istream& aStream )
  {
    ItemFactory* lFactory = Zorba::getInstance(0)->getItemFactory();
    // TODO: once a proper streaming implementation is in place this can be
    // changed. This required a Base64 encoding stream since the item factory
    // work only builds base64binary and assumes the data is already encoded.
    String lEncoded;
    zorba::base64::encode(aStream, &lEncoded);
    return lFactory->createBase64Binary(lEncoded.data(), lEncoded.size(), true);
  }

} // namespace http_client
} // namespace zorba
/* vim:set et sw=2 ts=2: */
