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

#include "context/standard_uri_resolvers.h"

#include <istream>
#include <fstream>
#include <sstream>

#include <zorba/external_module.h>
#include <zorba/module_import_checker.h>
#include <zorba/util/file.h>
#include <zorba/zorbastring.h>

#include "util/string_util.h"
#include "util/uri_util.h"

#include "store/api/item.h"
#include "store/api/item_factory.h"
#include "store/api/collection.h"

#include "store/api/store.h"
#include "system/globalenv.h"
#include "zorbatypes/URI.h"
#include "zorbaerrors/error_manager.h"
#include "zorbamisc/ns_consts.h"
#include "context/static_context.h"
#include "context/dynamic_loader.h"
#include "context/get_current_lib_suffix.h"

#include "zorbaerrors/assert.h"

namespace zorba
{

/*******************************************************************************

********************************************************************************/
store::Item_t StandardDocumentURIResolver::resolve(
    const store::Item_t& aURI,
    static_context* aStaticContext,
    bool validateUri,
    bool replaceDoc)
{
  store::LoadProperties loadProperties;
  loadProperties.setStoreDocument(true);

  store::Item_t lResultDoc;

  if (aURI == NULL)
    return NULL;

  zstring lUriString;
  aURI->getStringValue2(lUriString);

  store::Store& lStore = GENV.getStore();

  zstring lEnableDtdOptionValue;
  store::Item_t lEnDtdQName;
  lStore.getItemFactory()->createQName(lEnDtdQName, ZORBA_OPTIONS_NS, "", ZORBA_OPTION_ENABLE_DTD);
  aStaticContext->lookup_option(lEnDtdQName, lEnableDtdOptionValue);
  loadProperties.setEnableDtd( lEnableDtdOptionValue.compare("true")==0 );

  zstring baseUri = aStaticContext->get_base_uri();

  URI lURI(lUriString, validateUri);

  if (replaceDoc)
    lStore.deleteDocument(lURI.toString());
  else
    lResultDoc = lStore.getDocument(lURI.toString());

  if (lResultDoc != NULL)
    return lResultDoc;

  if (ZSTREQ(lURI.get_scheme(), "file"))
  {
#ifdef ZORBA_WITH_FILE_ACCESS // maybe we don't want to allow file access for security reasons (e.g. in a webapp)
    std::ifstream lInStream;
    zstring decodedURI;
    URI::decode_file_URI(lURI.toString(), decodedURI);

#ifndef WIN32
    lInStream.open(decodedURI.c_str(), std::ios::in);
#else
    WCHAR *wdecodedURI;
    wdecodedURI = new WCHAR[decodedURI.length() + 10];
    if(MultiByteToWideChar(CP_UTF8, 0, decodedURI.c_str(), -1, wdecodedURI, decodedURI.length()+10) == 0)
    {
      MultiByteToWideChar(CP_ACP, 0, decodedURI.c_str(), -1, wdecodedURI, decodedURI.length()+10);
    }
    lInStream.open(wdecodedURI, std::ios::in);
    delete[] wdecodedURI;
#endif
    if (lInStream.is_open() == false)
    {
      throw XQUERY_EXCEPTION(
        FODC0002, ERROR_PARAMS( decodedURI, ZED( FileNotFoundOrReadable ) )
      );
    }

    // parse exception must be caught by the caller
    lResultDoc = lStore.loadDocument(baseUri, lURI.toString(), lInStream, loadProperties);

    // result can't be null, because we already asked the store if he has it
    ZORBA_ASSERT(lResultDoc != NULL);
#else
    throw XQUERY_EXCEPTION( FODC0002, ERROR_PARAMS( lURI.toString() ) );
#endif
  }
  else if (ZSTREQ(lURI.get_scheme(), "http") ||
           ZSTREQ(lURI.get_scheme(), "https"))
  {
#ifdef ZORBA_WITH_REST
    // retrieve web file
    std::stringstream iss;
    try {
      uri::fetch(lURI.toString(), iss);
    }
    catch ( uri::fetch_exception const &e ) {
      throw XQUERY_EXCEPTION(
        FODC0002, ERROR_PARAMS( lURI.toString(), e.what() )
      );
    }

    // parse exception must be caught by the caller
    lResultDoc = lStore.loadDocument(baseUri, lURI.toString(), iss, loadProperties);

    // result can't be null, because we already asked the store if he has it
    ZORBA_ASSERT(lResultDoc != NULL);
#else
    throw XQUERY_EXCEPTION( FODC0002, ERROR_PARAMS( lURI ) );
#endif
  }
  else
  {
    throw XQUERY_EXCEPTION(
      FODC0002, ERROR_PARAMS( lURI, ZED( BadURIScheme ), lURI.get_scheme() )
    );
  }

  if (lResultDoc == NULL)
  {
    throw XQUERY_EXCEPTION( FODC0002, ERROR_PARAMS( lURI.toString() ) );
  }

  return lResultDoc;
}


/*******************************************************************************

********************************************************************************/
store::Collection_t
StandardCollectionURIResolver::resolve(
    const store::Item_t& aURI,
    static_context* aStaticContext)
{
  store::Collection_t lResultCol;

  zstring lUriString = aURI->getStringValue();

  store::Store& lStore = GENV.getStore();

  // check and eventually resolve URI
  // throw FODC0004 if the URI is not valid
  URI lURI;

  try
  {
    lURI = URI(lUriString);

    if (!lURI.is_absolute())
    {
      URI lBaseURI(aStaticContext->get_base_uri());
      lURI = URI(lBaseURI, lUriString);
    }
  }
  catch (ZorbaException const& e)
  {
    throw XQUERY_EXCEPTION( FODC0004, ERROR_PARAMS( lUriString, e.what() ) );
  }

  // try to get it from the store again
  lResultCol = lStore.getUriCollection(lURI.toString());
  return lResultCol;
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  StandardSchemaURIResolver                                                  //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
std::string
StandardSchemaURIResolver::resolve(
    const store::Item_t& aURI,
    static_context* aStaticContext,
    std::vector<store::Item_t>& aAtList,
    zstring& aFileUri)
{
  // 1. check using module paths => return if good stream is found
  std::vector<std::string> lModulePaths;
  aStaticContext->get_full_module_paths(lModulePaths);

  if (lModulePaths.size() != 0)
  {
    zstring filepath = checkSchemaPath(lModulePaths, aURI);
    if (!filepath.empty())
    {
      zstring url;
      URI::encode_file_URI(filepath, url);
      return url.str();
    }
  }

  // 2. check the user's resolvers
  std::vector<InternalSchemaURIResolver*> lResolvers;
  aStaticContext->get_schema_uri_resolvers(lResolvers);
  for (std::vector<InternalSchemaURIResolver*>::const_iterator lIter =
      lResolvers.begin(); lIter != lResolvers.end(); ++lIter)
  {
    std::string lResult = (*lIter)->resolve(aURI,
                                            aStaticContext,
                                            aAtList,
                                            aFileUri);
    if (lResult != "")
    {
      return lResult;
    }
  }

  // 3. treat the URI as URL and check if a file is in the
  // filesystem or on the web
  // TODO register other interal resolvers for each of the tasks
  zstring lResolvedURI;
  if (aAtList.size() > 0)
  {
    aAtList[0]->getStringValue2(lResolvedURI);
  }
  else
  {
    aURI->getStringValue2(lResolvedURI);
  }

  if (lResolvedURI.compare(0, 7, "file://") == 0)
  {
    // maybe we don't want to allow file access for security reasons (e.g. in a webapp)
#ifdef ZORBA_WITH_FILE_ACCESS
    zstring filepath;
    URI::decode_file_URI(lResolvedURI, filepath);

    std::auto_ptr<std::ifstream> lSchemaFile(new std::ifstream(filepath.c_str()));

    if (lSchemaFile->good())
    {
      return lResolvedURI.str();
    }
    else
    {
      ZORBA_ERROR_PARAM(XQST0059, lResolvedURI, aURI->getStringValue().c_str());
    }
#endif
  }

  return lResolvedURI.str();
}


/*******************************************************************************

********************************************************************************/
zstring StandardSchemaURIResolver::checkSchemaPath(
    const std::vector<std::string>& aSchemaPath,
    const store::Item_t& aUri)
{
  URI lURI(aUri->getStringValue());

  // compute path notation of the uri with a .xq at the end
  // TODO: we might extend that to other file exentsions
  zstring lPathNotation = lURI.toPathNotation();
  if (!ascii::ends_with(lPathNotation, ".xsd", 4))
  {
    lPathNotation = lPathNotation.append(".xsd");
  }

  // check all module path in the according order
  // the higher in the hirarchy the static context is
  // the higher the priority of its module paths
  for (std::vector<std::string>::const_iterator lIter = aSchemaPath.begin();
       lIter != aSchemaPath.end(); ++lIter)
  {
    zstring lPotentialModuleFile = (*lIter) + lPathNotation;
    std::auto_ptr<std::istream> modfile(
                                        new std::ifstream(lPotentialModuleFile.c_str()));
    if (modfile->good())
    {
      return lPotentialModuleFile;
    }
  }

  return "";
}


#ifndef ZORBA_NO_FULL_TEXT
/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  StandardFullTextURIResolver                                               //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
zstring
StandardFullTextURIResolver::resolve(
    const store::Item_t& aURI,
    static_context* aStaticContext)
{
  // 2. check the user's resolvers
  std::vector<InternalFullTextURIResolver*> lResolvers;
  aStaticContext->get_thesaurus_uri_resolvers(lResolvers);
  for (std::vector<InternalFullTextURIResolver*>::const_iterator lIter =
      lResolvers.begin(); lIter != lResolvers.end(); ++lIter)
  {
    zstring lResult = (*lIter)->resolve(aURI, aStaticContext);
    if (lResult != "")
    {
      return lResult;
    }
  }

  // 3. treat the URI as URL and check if a file is in the
  // filesystem or on the web
  zstring lResolvedURI;
  aURI->getStringValue2(lResolvedURI);

  if (lResolvedURI.compare(0, 7, "file://") == 0)
  {
    // maybe we don't want to allow file access for security reasons (e.g. in a webapp)
#ifdef ZORBA_WITH_FILE_ACCESS
    zstring filepath;
    URI::decode_file_URI(lResolvedURI, filepath);

    File_t lFile = File::createFile(filepath.c_str());
    if (lFile->exists()) {
      return lResolvedURI.str();
    } else {
      ZORBA_ERROR_PARAM(XQST0059, lResolvedURI, aURI->getStringValue().c_str());
    }
#endif /* ZORBA_WITH_FILE_ACCESS */
  }

  return aURI->getStringValue().str();
}
#endif /* ZORBA_NO_FULL_TEXT */

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  StandardModuleURIResolver                                                  //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************
  Given the target namespace URI of a module, return the URIs of this module's
  components.
********************************************************************************/
void StandardModuleURIResolver::resolveTargetNamespace(
    const std::string& nsURI,
    static_context& sctx,
    std::vector<std::string>& compURIs)
{
  // Gather the resolvers registered by the application within the given sctx
  // obj and its ancestors.
  std::vector<InternalModuleURIResolver*> resolvers;
  sctx.get_module_uri_resolvers(resolvers);

  std::vector<InternalModuleURIResolver*>::const_iterator ite;
  for (ite = resolvers.begin(); ite != resolvers.end(); ++ite)
  {
    (*ite)->resolveTargetNamespace(nsURI, sctx, compURIs);

    if (!compURIs.empty())
      return;
  }

  // If there were no user resolvers, or if none of them knew anything about
  // the given target namespace, assume that the module consists of a single
  // component whose URI is the same as the target namespace.
  compURIs.push_back(nsURI);
}


/*******************************************************************************
  Resolve the URI of a module component to an input stream. Resolving is done
  according to the following rules:

  1. All module paths that are registered in the path from the zorba root sctx
     to the given sctx are searched for a file with pathname that matches the
     path notation of the uri. The module paths are searched starting with the
     ones registered in the zorba root sctx and moving down towards the given
     sctx until a match is found.
  2. A (potential) user's resolver is asked
  3. the URI is treated as an URL

  Besides returning an input stream, the function also returns the URL of the
  file that the given URI was resolved to.
********************************************************************************/
std::istream* StandardModuleURIResolver::resolve(
    const std::string& uri,
    static_context& sctx,
    std::string& url)
{
  std::vector<ModuleImportChecker*> lCheckers = sctx.getAllModuleImportCheckers();
  std::vector<ModuleImportChecker*>::iterator lIter = lCheckers.begin();
  for (; lIter != lCheckers.end(); ++lIter) 
  {
    if (!(*lIter)->checkModuleUri(uri.c_str()))
    {
      throw ZORBA_EXCEPTION(
        ZXQP0029_MODULE_IMPORT_NOT_ALLOWED, ERROR_PARAMS( uri )
      );
    }
  }
  std::auto_ptr<std::istream> modfile(0); // result file

  // 1. check using registered module paths
  std::vector<std::string> lModulePaths;
  sctx.get_full_module_paths(lModulePaths);

  if (lModulePaths.size() != 0)
  {
    zstring tmp;
    modfile.reset(checkModulePath(lModulePaths, uri, tmp));
    url = tmp.str();

    if (modfile.get() != 0)
    {
      assert(modfile->good());
      return modfile.release();
    }
  }

  // 2. check the user's resolvers
  std::vector<InternalModuleURIResolver*> lResolvers;
  sctx.get_module_uri_resolvers(lResolvers);

  std::vector<InternalModuleURIResolver*>::const_iterator ite;
  for (ite = lResolvers.begin(); ite != lResolvers.end(); ++ite)
  {
    modfile.reset((*ite)->resolve(uri, sctx, url));
    if (modfile.get())
    {
      assert(modfile->good());
      return modfile.release();
    }
  }

  // 3. treat the URI as URL and check if a file is in the filesystem or on the web
  // TODO register other interal resolvers for each of the tasks
  url = uri;

  if (url.compare(0, 7, "file://") == 0)
  {
    // maybe we don't want to allow file access for security reasons (e.g. in a webapp)
#ifdef ZORBA_WITH_FILE_ACCESS
    zstring filepath;
    URI::decode_file_URI(uri, filepath);
    modfile.reset(new std::ifstream(filepath.c_str()));
#endif
  }
  else
  {
    std::auto_ptr<std::stringstream> code(new std::stringstream());

    try {
      uri::fetch(url, *code);
    }
    catch ( uri::fetch_exception const& ) {
      return NULL;
    }

    modfile.reset(code.release());
  }

  // we transfer ownership to the caller
  return modfile.release();
}


/*******************************************************************************

********************************************************************************/
std::istream* StandardModuleURIResolver::checkModulePath(
    const std::vector<std::string>& modulePaths,
    const std::string& uri,
    zstring& fileURL)
{
  URI lURI(uri);

  // compute path notation of the uri with a .xq at the end
  // TODO: we might extend that to other file exentsions
  zstring lPathNotation = lURI.toPathNotation();
  if (!ascii::ends_with(lPathNotation, ".xq", 3))
  {
    if(!ascii::ends_with(lPathNotation, "/", 1))
      lPathNotation = lPathNotation.append(".xq");
    else
      lPathNotation = lPathNotation.append("index.xq");
  }

  for (std::vector<std::string>::const_iterator ite = modulePaths.begin();
       ite != modulePaths.end();
       ++ite)
  {
    fileURL = (*ite) + lPathNotation;
    std::auto_ptr<std::istream> modfile(new std::ifstream(fileURL.c_str()));
    if (modfile->good())
    {
      return modfile.release();
    }
  }

  return 0;
}


/*******************************************************************************
  Find and load the external module with the given target namespace.
********************************************************************************/
ExternalModule* StandardModuleURIResolver::getExternalModule(
    const zstring& fileURL,
    static_context& sctx)
{
  std::vector<std::string> lModulePaths;
  sctx.get_full_module_paths(lModulePaths);

  std::auto_ptr<std::istream> modfile(0); // result file

  if (lModulePaths.size() != 0)
  {
    URI lURI(fileURL);

    std::string lLibraryName = computeLibraryName(lURI);
    std::string lLibraryNameDebug = computeLibraryName(lURI, true);

    // check all module path in the according order
    // the higher in the hirarchy the static context is
    // the higher the priority of its module paths
    for (std::vector<std::string>::const_iterator ite = lModulePaths.begin();
        ite != lModulePaths.end();
         ++ite)
    {
      std::string potentialModuleFile = (*ite);
      std::string potentialModuleFile2 = potentialModuleFile;
      potentialModuleFile += lLibraryName;
      potentialModuleFile2 += lLibraryNameDebug;

      std::auto_ptr<std::istream> modfile(new std::ifstream(potentialModuleFile.c_str()));
      
      if (!modfile->good()) {
        modfile.reset(new std::ifstream(potentialModuleFile2.c_str()));
        potentialModuleFile = potentialModuleFile2;
      }

      if (modfile->good())
      {
        ExternalModule* lModule = DynamicLoader::getInstance()->
                                  getModule(potentialModuleFile);
        if (lModule)
        {
          if (lModule->getURI().c_str() != fileURL)
          {
            throw ZORBA_EXCEPTION(
              ZXQP0028_TARGET_NAMESPACE_NOT_PROVIDED,
              ERROR_PARAMS( lURI.toString(), potentialModuleFile )
            );
          }
        }

        return lModule;
      }
    }
  }

  return 0;
}


/*******************************************************************************

********************************************************************************/
std::string StandardModuleURIResolver::computeLibraryName(const URI& aURI, bool aUseDebugDir /* = false */)
{
  zstring lPathNotation = aURI.toPathNotation();

  // get the module file name
  size_t lIndexOfLastSlash = lPathNotation.find_last_of("/");

  zstring lFileName;
  zstring lBranchPath;

  // is the URI ends in '/'
  if (lIndexOfLastSlash == lPathNotation.size())
  {
    lBranchPath = lPathNotation;
  }
  else
  {
    // is '/' is not found
    if (lIndexOfLastSlash == std::string::npos)
    {
      lFileName = lPathNotation;
    }
    else
    {
      lFileName = lPathNotation.substr(lIndexOfLastSlash + 1);
      lBranchPath = lPathNotation.substr(0, lIndexOfLastSlash + 1);
    }

    // remove .xq from the end of the file if present
    // bugfix: find_last_of didn't do the right thing
    size_t lIndexOfXQ = lFileName.find(".xq");
    if (lIndexOfXQ != std::string::npos && lIndexOfXQ == lFileName.size() - 3)
    {
      lFileName.erase(lIndexOfXQ );
    }
  }

  // create the name of the file
  // win32: module.dll
  // apple: libmodule.dylib
  // other unix: libmodule.so
  std::ostringstream lLibraryName;
  lLibraryName << lBranchPath;
#ifdef WIN32
  if (aUseDebugDir) {
    lLibraryName << "Debug\\";
  }
  lLibraryName << lFileName << get_current_lib_suffix() << ".dll";
#else
  if (aUseDebugDir) {
    lLibraryName << "Debug/";
  }
#ifdef APPLE
  lLibraryName << "lib" << lFileName << get_current_lib_suffix() << ".dylib";
#else
  lLibraryName << "lib" << lFileName << get_current_lib_suffix() << ".so";
#endif
#endif

  return lLibraryName.str();
}



/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  StandardLibraryModuleURIResolver                                           //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
void StandardLibraryModuleURIResolver::resolveTargetNamespace(
    const std::string& nsURI,
    static_context& sctx,
    std::vector<std::string>& compURIs)
{
  compURIs.push_back(nsURI);
}


/*******************************************************************************

********************************************************************************/
std::istream*
StandardLibraryModuleURIResolver::resolve(
    const std::string& uri,
    static_context& sctx,
    std::string& url)
{
  assert(uri == theLibraryModuleNamespace);
  assert(theStream.good());

  url = theFileName;

  std::auto_ptr<std::istream> modfile(new std::istream(theStream.rdbuf()));

  // we transfer ownership to the caller
  return modfile.release();
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */
