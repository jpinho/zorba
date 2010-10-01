/*
 * Copyright 2006-2008 The FLWOR Foundation.
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

#include "imap_client.h"
#include <stdio.h>
#include <iostream>
#include <sstream>

namespace zorba { namespace emailmodule {


  void
  ImapClient::setUserName(const std::string& usr) {
    theUserName = usr; 
  }

  void
  ImapClient::setPassword(const std::string& pwd) {
    thePassword = pwd;
  }

  std::string 
  ImapClient::getUserName() {
    return theUserName;
  }
  
  std::string
  ImapClient::getPassword() {
    return thePassword;
  }
 
  MAILSTREAM* 
  ImapClient::getMailStream(const std::string& aHost,
                            const std::string& aUsername,
                            const std::string& aPassword,                                
                            const std::string& aMailbox,
                            const bool aFullOpen) {
      
      std::string lHostAndMailbox = "{" + aHost + "}" + aMailbox;
      // check if theMailstream is open already and exaclty the same as the old one
      if ((theMailstream) && 
         (theUserName.compare(aUsername) == 0) && 
         (thePassword.compare(aPassword) == 0) && 
         (lHostAndMailbox.compare(theMailstream->original_mailbox) == 0) && 
         ((aFullOpen ? NIL : T) == theMailstream->halfopen))  {
         // do nothing, theMailstream will be returned at the end of the function
      } else if ((theMailstream) && (theHost.compare(aHost) == 0)) {
        // use old stream to same host, but open new
        setUserName(aUsername);
        setPassword(aPassword);
        mail_open(theMailstream, const_cast<char*>(lHostAndMailbox.c_str()), (aFullOpen ? NIL : OP_HALFOPEN));
      } else {
        // make a new mailstream according to the passed values
        setUserName(aUsername);
        setPassword(aPassword);
        theHost = aHost;
        theMailstream = mail_open(NIL, const_cast<char*>(lHostAndMailbox.c_str()), (aFullOpen ? NIL : OP_HALFOPEN));
      }
      return theMailstream;

  }


  void 
  ImapClient::status (const std::string& aHost, 
                      const std::string& aUsername, 
                      const std::string& aPassword, 
                      const std::string& aMailbox) 
  {
    #include "linkage.c"
    std::string lHostAndMailbox = "{" + aHost + "}" + aMailbox;
    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, aMailbox, false);
    mail_status (lSource, const_cast<char*>(lHostAndMailbox.c_str()), SA_MESSAGES | SA_RECENT | SA_UNSEEN | SA_UIDNEXT | SA_UIDVALIDITY);
  }

  bool
  ImapClient::create (const std::string& aHost, 
                      const std::string& aUsername, 
                      const std::string& aPassword, 
                      const std::string& aMailbox) {
   
    #include "linkage.c" 
    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, aMailbox, false); 
    std::string lFullName = "{" + aHost + "}" + aMailbox; 
    return (mail_create (lSource, const_cast<char*>(lFullName.c_str())) == T);
  }


  bool
  ImapClient::delete_mailbox (const std::string& aHost, 
                              const std::string& aUsername, 
                              const std::string& aPassword, 
                              const  std::string& aMailbox) {

     #include "linkage.c"                                 
    MAILSTREAM *lSource = getMailStream(aHost, aUsername, aPassword, aMailbox, false);
    std::string lFullName = "{" + aHost + "}" + aMailbox;
    return (mail_delete(lSource, const_cast<char*>(lFullName.c_str())) == T);
  }

  bool 
  ImapClient::rename (const std::string& aHost, 
                      const std::string& aUsername, 
                      const  std::string& aPassword, 
                      const std::string& aFromMailbox, 
                      const std::string& aToMailbox) {

    #include "linkage.c"
    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, "", false);
    std::string lHost = "{" + aHost + "}"; 
    std::string lFullNameOld = lHost + aFromMailbox;
    std::string lFullNameNew = lHost + aToMailbox;
    return (mail_rename(lSource, const_cast<char*>(lFullNameOld.c_str()), const_cast<char*>( lFullNameNew.c_str())) == T);
  } 

  bool 
  ImapClient::subscription(const std::string& aHost,  
                           const std::string& aUsername, 
                           const std::string& aPassword, 
                           const std::string& aMailbox, 
                           bool subscribe) {

    #include "linkage.c"
    std::string lHost = "{" + aHost + "}";
    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, aMailbox, false); 
    std::string lFullName = lHost + aMailbox;
    if (subscribe) {
      return (mail_subscribe(lSource, const_cast<char*>(lFullName.c_str())) == T);
    } else {
      return (mail_unsubscribe(lSource, const_cast<char*>(lFullName.c_str())) == T);
    }
  }

    
  bool
  ImapClient::expunge(const std::string& aHost, 
                      const std::string& aUsername, 
                      const std::string& aPassword, 
                      const std::string& aMailbox) {

    #include "linkage.c"
    std::string lHost = "{" + aHost + "}" + aMailbox;
    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, aMailbox, true); 
    if (lSource) {  mail_expunge(lSource); }
    return true;
  }


  std::vector<std::string> 
  ImapClient::list(const std::string& aHost, 
                   const std::string& aUsername, 
                   const std::string& aPassword, 
                   const std::string& aReferencePath, 
                   const std::string& aPattern, 
                   bool aOnlySuscribed) {
    
    #include "linkage.c"
    /* IMPORTANT: make sure that the vector of listed mailboxes is empty! */
    theListedMailboxes.clear();

    std::string lHost = "{" + aHost + "}";
    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, "", false); 
    std::string lFullPath = lHost + aReferencePath;
    if (aOnlySuscribed) {
      mail_lsub(lSource, const_cast<char*>(lFullPath.c_str()), const_cast<char*>(aPattern.c_str()));
    } else {
      mail_list(lSource,  const_cast<char*>(lFullPath.c_str()), const_cast<char*>(aPattern.c_str()));
    }
    // theListedMailbox is filled through the mm_list or mm_listsub callback functions
    return theListedMailboxes;
  }  

  bool 
  ImapClient::copy(const std::string& aHost, 
                   const std::string& aUserName, 
                   const std::string& aPassword, 
                   const std::string& aMailboxFrom, 
                   const std::string& aMailboxTo, 
                   const std::string& aMessageNumbers, 
                   bool aUid,   
                   bool aCopy) {
    
    #include "linkage.c"
    
    std::string lHost = "{" + aHost + "}" + aMailboxFrom;
    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailboxFrom, true); 
    long  lLongResult =  mail_copy_full(lSource,  const_cast<char*>(aMessageNumbers.c_str()),  const_cast<char*>(aMailboxTo.c_str()), (aUid ? SE_UID : NIL) | (aCopy ? CP_MOVE : NIL));
    return (lLongResult == T);
  }

  std::vector<long>
  ImapClient::search(const std::string& aHost, 
                     const std::string& aUsername, 
                     const std::string& aPassword, 
                     const std::string& aMailbox, 
                     const std::string& aCriteria, 
                     bool aUid) {

    #include "linkage.c"
  
    /* IMPORTANT: make sure that the vector of found sequence numbers is empty! */
    theFoundSequenceNumbers.clear();

    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, "", true); 
    /* First, tokenize the criteria so that we can work on it */
    std::stringstream lToTokenize(aCriteria);
    std::string lBuffer;
    std::vector<std::string> lTokens;

    SEARCHPGM * lSearchProgram;
    while(lToTokenize >> lBuffer) {
        lTokens.push_back(lBuffer);
    }
    lSearchProgram = search_criteria(lTokens);
    
    mail_search_full(lSource, NIL,  lSearchProgram, (aUid ? SE_UID : NIL) | SE_NOPREFETCH); 
    /* clean up, don't leave a mess */
    mail_free_searchpgm(&lSearchProgram); 
    return theFoundSequenceNumbers;
  }

  ENVELOPE * 
  ImapClient::fetchEnvelope(const std::string& aHost, 
                            const std::string& aUsername, 
                            const std::string& aPassword, 
                            const std::string& aMailbox, 
                            unsigned long aMessageNumber) 
  {
    #include "linkage.c"

    MAILSTREAM* lSource = getMailStream(aHost, aUsername, aPassword, aMailbox, true); 
    
    return mail_fetchenvelope(lSource, aMessageNumber);
  }

  std::string                    
  ImapClient::fetchSubject(const std::string& aHost,
                           const std::string& aUserName,
                           const std::string& aPassword,
                           const std::string& aMailbox,
                           const unsigned long aMessageNumber) {

    #include "linkage.c"

    std::string lHost = "{" + aHost + "}" + aMailbox; 
    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailbox, true); 
    
    char lResult[30];
    mail_fetchsubject(lResult, lSource, aMessageNumber, (unsigned long) 30); 
    return std::string(lResult);
  }



  std::string
  ImapClient::fetchFrom(const std::string& aHost,
                        const std::string& aUserName,
                        const std::string& aPassword,
                        const std::string& aMailbox,
                        const unsigned long aMessageNumber) {

    #include "linkage.c"

    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailbox, true); 

    char lResult[30];
    mail_fetchfrom(lResult, lSource, aMessageNumber, (unsigned long) 30);
    return std::string(lResult);
  }

  long                            
  ImapClient::convertNumber(const std::string& aHost,
                            const std::string& aUserName,
                            const std::string& aPassword,
                            const std::string& aMailbox,
                            const unsigned long aMessageNumber,
                            const bool aUid) {
    
    #include "linkage.c"

    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailbox, true); 

    if (aUid) {
      return mail_uid(lSource, aMessageNumber);
    } else {
      return mail_msgno(lSource, aMessageNumber);
    }


  }

  ENVELOPE* 
  ImapClient::fetchStructure(const std::string& aHost, 
                             const std::string& aUserName, 
                             const std::string& aPassword, 
                             const std::string& aMailbox, 
                             BODY** aBody,  
                             unsigned long aMessageNumber, 
                             bool aUid) 
  {
    #include "linkage.c"
    *aBody = mail_newbody(); 
    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailbox, true);    
 
    ENVELOPE * lResult = mail_fetchstructure_full (lSource, aMessageNumber, aBody, (aUid ? FT_UID : NIL));
  
    return lResult;

  }

  std::string 
  ImapClient::fetchText(const std::string& aHost, 
                        const std::string& aUserName,
                        const std::string& aPassword,
                        const std::string& aMailbox,
                        unsigned long aMessageNumber,
                        bool aUid)
  {
    #include "linkage.c"
    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailbox, true); 
    unsigned long lLenght;
    return std::string(mail_fetchtext_full(lSource, aMessageNumber, &lLenght, (aUid ? FT_UID : NIL))); 
  } 


  std::string 
  ImapClient::fetchFlags(const std::string& aHost,
                       const std::string& aUserName,
                       const std::string& aPassword,
                       const std::string& aMailbox,
                       unsigned long aMessageNumber,
                       bool aUid) {

    #include "linkage.c"
    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailbox, true);
    char* lSequenceNumber;
    std::stringstream lConverter;
    lConverter << aMessageNumber;
    lConverter >> lSequenceNumber; 
    mail_fetchflags_full(lSource, lSequenceNumber, (aUid ? FT_UID : NIL));  
    return "nnn";

  }


 


  std::string
  ImapClient::fetchBodyFull(const std::string& aHost,
                            const std::string& aUserName,
                            const std::string& aPassword,
                            const std::string& aMailbox,
                            const unsigned long aMessageNumber,
                            const std::string& aSection,
                            const bool aUid) 
  {
    #include "linkage.c"
    MAILSTREAM* lSource = getMailStream(aHost, aUserName, aPassword, aMailbox, true); 
    unsigned long lLenght;
    // convert section int into char
    return std::string(mail_fetchbody_full(lSource, aMessageNumber, const_cast<char*>(aSection.c_str()), &lLenght, (aUid ? FT_UID : NIL)));
 
  } 

  SEARCHPGM*
  ImapClient::search_criteria(std::vector<std::string> aCriteria) {
    SEARCHPGM *lSearchProgram = NIL;

    /* initializing the search program */
    lSearchProgram= mail_newsearchpgm ();

    /* Now, we iterate through the whole vector containing the search critera, building our search program */
    for (std::vector<std::string>::iterator lIterator = aCriteria.begin(); lIterator != aCriteria.end(); ++lIterator) {
      if (*lIterator == "ALL") {
        /* do nothing ... */
      } else if (*lIterator == "ANSWERED") {
        lSearchProgram->answered = T;
      } else if (*lIterator == "BCC") {
        char * lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string (&lSearchProgram->bcc, &lNext);
      } else if (*lIterator == "BEFORE") {
        char* lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_date(&lSearchProgram->before, &lNext);
      } else if (*lIterator == "BODY") {
        char* lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string(&lSearchProgram->body, &lNext);
      } else if (*lIterator == "CC") {
        char* lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string(&lSearchProgram->cc, &lNext);
      } else if (*lIterator == "DELETED") {
        lSearchProgram->deleted = T;
      } else if (*lIterator == "DRAFT") {
        lSearchProgram->draft = T;
      } else if (*lIterator == "FLAGGED") {
        lSearchProgram->flagged = T;
      } else if (*lIterator == "FROM") {
        char* lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string(&lSearchProgram->from, &lNext);
      } else if (*lIterator == "HEADER") {
        char *lHeaderField = getNextStringAsChar(++lIterator);
        char *lHeaderContent = getNextStringAsChar(++lIterator);  
        SEARCHHEADER* lSearchHeader = mail_newsearchheader(lHeaderField, lHeaderContent);
        if (lSearchProgram->header) {
          lSearchProgram->header->next = lSearchHeader;
        } else {
          lSearchProgram->header = lSearchHeader;
        }  
      } else if (*lIterator == "KEYWORD") {
        char * lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string(&lSearchProgram->keyword, &lNext);
      } else if (*lIterator == "LARGER") {
        lIterator++;
        // get the unsigned long value that should follow the Keyword LARGER
        std::stringstream lLargerValueStream;
        lLargerValueStream << (*lIterator).c_str();
        unsigned long lLargerValue; 
        lLargerValueStream >> lLargerValue;
        lSearchProgram->larger = lLargerValue;
      } else if (*lIterator == "NEW") {
        // according to the specification, this is equivalent to recent unseen
        lSearchProgram->recent = T;
        lSearchProgram->unseen = T;
      }  else if (*lIterator == "OLD") {
        lSearchProgram->old = T;
      } else if (*lIterator == "ON") {
        char * lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_date(&lSearchProgram->on, &lNext);
      } else if (*lIterator == "RECENT") {
        lSearchProgram->recent = T;
      } else if (*lIterator == "SEEN") {
        lSearchProgram->seen = T;
      } else if (*lIterator == "SENTBEFORE") {
        char * lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_date(&lSearchProgram->sentbefore, &lNext);
      } else if (*lIterator == "SENTON") {
        char *lNext = getNextStringAsChar(lIterator);
        mail_criteria_date(&lSearchProgram->senton, &lNext);
      } else if (*lIterator == "SENTSINCE") {
        char *lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_date(&lSearchProgram->sentsince, &lNext);
      } else if (*lIterator == "SMALLER") {
        lIterator++;
        std::stringstream lSmallerValueStream;
        lSmallerValueStream << (*lIterator).c_str();
        unsigned long lSmallerValue;
        lSmallerValueStream >> lSmallerValue;
        lSearchProgram->smaller = lSmallerValue;
      } else if (*lIterator == "SUBJECT") {
        char *lNext = getNextStringAsChar(++lIterator);
        mail_criteria_string(&lSearchProgram->subject, &lNext);
      } else if (*lIterator == "TEXT") {
        char *lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string(&lSearchProgram->text, &lNext);
      } else if (*lIterator == "TO") {
        char *lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string(&lSearchProgram->to, &lNext);
      } else if (*lIterator == "UNANSWERED") {
        lSearchProgram->unanswered = T;
      } else if (*lIterator == "UNDELETED") {
        lSearchProgram->undeleted = T;
      } else if (*lIterator == "UNDRAFT") {
        lSearchProgram->undraft = T;
      } else if (*lIterator == "UNFLAGGED") {
        lSearchProgram->unflagged = T;
      } else if (*lIterator == "UNKEYWORD") {
        char *lNext = getNextStringAsChar(++lIterator); 
        mail_criteria_string(&lSearchProgram->unkeyword, &lNext);
      } else if (*lIterator == "UNSEEN") {
        lSearchProgram->unseen = T;
      } else if (*lIterator == "NOT") {
        std::vector<std::string> lNotVector;
        std::string lNextWord = *(++lIterator);
        // if next word is a keyword that needs a parameter
        lNotVector.push_back(lNextWord);
        if (!isSingleKeyword(lNextWord)) {
          // pass parameter
          lNotVector.push_back(*(++lIterator));
        }
        SEARCHPGM* lNotted = search_criteria(lNotVector);        
        SEARCHPGMLIST* lNotList = mail_newsearchpgmlist (); 
        lNotList->pgm = lNotted;
        lSearchProgram->not = lNotList;
    } else if (*lIterator == "OR") {
      // Make all up to the or be the first search_program, then or the rest ...
      
      std::vector<std::string> lOrVector(++lIterator, aCriteria.end());
      SEARCHPGM* lOredProgram = search_criteria(lOrVector);
      
      // or the search_program we have constructed so far with the search_program constructed from the rest of the strings 
      SEARCHOR* lOr = mail_newsearchor(); 
      lOr->first = lOredProgram;     
      lOr->second = lSearchProgram;
      
      // Make a new search program that only contains the OR'ed search_programs and assign the pointer we are going to return to it 
      SEARCHPGM* lNewSearchProgramm = mail_newsearchpgm();
      lNewSearchProgramm->or = lOr;
      lSearchProgram = lNewSearchProgramm; 
      // stop here, at least for the left-hand part of the OR. 
      break; 
 
    } else {
        std::string lError = "Unknown keyword in IMAP search: " + *lIterator; 
         // if the word is not known ... throw an error through mm_log 
        mm_log((char *) lError.c_str(), ERROR);  
      }  
    }
   return lSearchProgram;
  }
  
  char * ImapClient::getNextStringAsChar(std::vector<std::string>::iterator aIterator) {
      return (char*) (*aIterator).c_str();
  }  

  bool 
  ImapClient::isSingleKeyword(const std::string& aKey) {
    if (aKey == "UNSEEN" || aKey == "UNANSWERED" || aKey == "UNDELETED" || aKey == "UNDRAFT" || aKey == "UNFLAGGED" || aKey == "DELETED" || aKey == "DRAFT" ||
      aKey == "FLAGGED" || aKey == "NEW" || aKey == "OLD" || aKey == "RECENT" || aKey == "SEEN" || aKey == "ANSWERED") {
      return true;
    }
      return false;
  }

  void 
  ImapClient::setStatus(unsigned long aMessages, unsigned long aRecent, unsigned long aUnseen, unsigned long aUIDNext, unsigned long aUIDValidity) {
    theMessages = aMessages;
    theRecent = aRecent;
    theUnseen = aUnseen;
    theUIDNext = aUIDNext;
    theUIDValidity = aUIDValidity;
  }


  void
  ImapClient::getStatus(unsigned long* aMessages, unsigned long* aRecent, unsigned long* aUnseen, unsigned long* aUIDNext, unsigned long* aUIDValidity) {
    
    *aMessages = theMessages;
    *aRecent = theRecent;
    *aUnseen = theUnseen;
    *aUIDNext = theUIDNext;
    *aUIDValidity = theUIDValidity;

  
  }

  void
  ImapClient::addListedMailbox(const std::string& aListedMail) {
    theListedMailboxes.push_back(aListedMail);
  } 


  std::string 
  ImapClient::getError() {
      // will not see an old error  
    
      std::string lTempError(theErrorMessage);
      theErrorMessage.clear();
      return lTempError;
  }
  
  
    
  void 
  ImapClient::addError(const std::string& aMessage) {
    if (theErrorMessage.size() == 0 ) {
      theErrorMessage = aMessage + "\n";
    } else {
      theErrorMessage = theErrorMessage + aMessage + "\n"; 
    }  
  }

  void 
  ImapClient::addFoundSequenceNumber(long aSequenceNumber) {
    theFoundSequenceNumbers.push_back(aSequenceNumber);
  }

} /* emailmodule */ } /* zorba */



  void mm_searched (MAILSTREAM *stream,unsigned long msgno) {
    zorba::emailmodule::ImapClient::Instance().addFoundSequenceNumber(msgno);
  }
  void mm_exists (MAILSTREAM *stream,unsigned long number) {}
  void mm_expunged (MAILSTREAM *stream,unsigned long number) {}
  void mm_flags (MAILSTREAM *stream,unsigned long number) {}

  /* Callback for when mail_list or mail_listsub are called ... */
  void mm_list (MAILSTREAM *stream,int delimiter,char *mailbox, long attributes) {
    if (!(attributes & LATT_NOSELECT) && *mailbox)  {
        zorba::emailmodule::ImapClient::Instance().addListedMailbox(std::string(mailbox)); 
    }  
  }
  void mm_lsub (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes) {
    if (!(attributes & LATT_NOSELECT) && *mailbox)  {
        zorba::emailmodule::ImapClient::Instance().addListedMailbox(std::string(mailbox));
    }
  }
  
  void mm_critical (MAILSTREAM *stream) {}
  void mm_nocritical (MAILSTREAM *stream) {}
  long mm_diskerror (MAILSTREAM *stream,long errcode,long serious) {return NIL;}


  void mm_notify (MAILSTREAM *stream,char *string,long errflg) {}



  void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status) {
    zorba::emailmodule::ImapClient::Instance().setStatus(status->messages, status->recent, status->unseen, status->uidnext, status->uidvalidity);
  }   

  /* Log an event for the user to see
   * Accepts: string to log
   *          error flag
   */
  void mm_log (char *string,long errflg)
  {

    switch ((short) errflg) {
    case NIL:
       
    case PARSE:
    case WARN:
            break;
    case ERROR:
        zorba::emailmodule::ImapClient::Instance().addError(std::string(string));    
        break;
    }
}

  /* Log an event to debugging telemetry
   * Accepts: string to log
   */
  void mm_dlog (char *string)
  { 
    std::cout << string << '\n';
  }

  /* Get user name and password for this host
   * Accepts: parse of network mailbox name
   *          where to return user name
   *          where to return password
   *          trial count
   */
  void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
  {
    const char* lUser = zorba::emailmodule::ImapClient::Instance().getUserName().c_str();
    const char* lPassword = zorba::emailmodule::ImapClient::Instance().getPassword().c_str();

    strcpy(user, lUser);
    strcpy(pwd, lPassword);

  }

  /* Log a fatal error event
   * Accepts: string to log
   */
  void mm_fatal (char *string)
  {
    printf ("?%s\n",string);
  }



