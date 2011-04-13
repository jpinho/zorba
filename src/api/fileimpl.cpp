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

#include "fileimpl.h"

#include <fstream>

#include <zorba/default_error_handler.h>
#include <zorba/util/file.h>

#include <runtime/util/flowctl_exception.h>

#include <zorbaerrors/error_manager.h>

#ifdef WIN32
#include <util/ascii_util.h>
#endif
#include <util/dir.h>

#include "zorbaimpl.h"

namespace zorba {

DirectoryIteratorImpl::DirectoryIteratorImpl(std::string const& aPath)
{
  theInternalDirIter = new dir_iterator(aPath);
}

DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
  delete theInternalDirIter;
}

bool
DirectoryIteratorImpl::next(std::string& aPathStr) const
{
  if (theInternalDirIter->end()) {
    return false;
  }

  // get the current pointed entry
  aPathStr = *(*theInternalDirIter);

  // advance to the next entry
  ++(*theInternalDirIter);

  return true;
}

void DirectoryIteratorImpl::reset()
{
  std::string aPath = theInternalDirIter->dirpath;
  delete theInternalDirIter;
  theInternalDirIter = new dir_iterator(aPath);
}

FileImpl::FileImpl(std::string const& path)
  : theErrorHandler(new DefaultErrorHandler())
{
  theInternalFile = new file(path
#ifdef WIN32
      , filesystem_path::CONVERT_SLASHES
#endif
    );
}

FileImpl::~FileImpl()
{
  delete theInternalFile;
  delete theErrorHandler;
}

File_t
File::createFile(const std::string& path)
{
  return new FileImpl(path);
}

const char*
File::getDirectorySeparator()
{
  return filesystem_path::get_directory_separator();
}

const char*
File::getPathSeparator()
{
  return filesystem_path::get_path_separator();
}

const std::string
FileImpl::getFilePath() const
{
  std::string lResult;

  ZORBA_TRY
    lResult = theInternalFile->get_path();
  ZORBA_CATCH

  return lResult;
};

const std::string
FileImpl::getFileUri() const
{
  std::string lPath;

  ZORBA_TRY
    lPath = theInternalFile->get_path();
#ifdef WIN32
    ascii::replace_all( lPath, '\\', '/' );
#endif

    std::stringstream lEncodedResult;
    lEncodedResult << "file:///";

    size_t lCurrentPos = 0;
    size_t lNextSlashPos = lPath.find_first_of("/");
    while (lNextSlashPos != std::string::npos) {
      String lEncodedSegment(lPath.substr(lCurrentPos, lNextSlashPos - lCurrentPos));
      lEncodedResult << lEncodedSegment.encodeForUri() << "/";
      lCurrentPos = lNextSlashPos + 1;
      lNextSlashPos = lPath.find_first_of("/", lNextSlashPos + 1);
    }

    String lEncodedSegment(lPath.substr(lCurrentPos));
    lEncodedResult << lEncodedSegment.encodeForUri();

    lPath = lEncodedResult.str();
  ZORBA_CATCH

  return lPath;
};

bool
FileImpl::isDirectory() const
{
  bool lResult = false;

  ZORBA_TRY
    lResult = theInternalFile->is_directory() || theInternalFile->is_volume() || theInternalFile->is_root();
  ZORBA_CATCH

  return lResult;
};

bool
FileImpl::isFile() const
{
  bool lResult = false;

  ZORBA_TRY
    lResult = theInternalFile->is_file() || theInternalFile->is_link();
  ZORBA_CATCH

  return lResult;
}

bool
FileImpl::isLink() const
{
  bool lResult = false;

  ZORBA_TRY
    lResult = theInternalFile->is_link();
  ZORBA_CATCH

  return lResult;
}

bool
FileImpl::isVolume() const
{
  bool lResult = false;

  ZORBA_TRY
    lResult = theInternalFile->is_volume();
  ZORBA_CATCH

  return lResult;
}

bool
FileImpl::isInvalid() const
{
  bool lResult = false;

  ZORBA_TRY
    lResult = theInternalFile->is_invalid();
  ZORBA_CATCH

  return lResult;
}

bool
FileImpl::exists() const
{
  bool lResult = false;

  ZORBA_TRY
    lResult = theInternalFile->exists();
  ZORBA_CATCH

  return lResult;
}

void
FileImpl::remove()
{
  ZORBA_TRY
    //TODO: Throw proper errors

    // precondition
    if (!theInternalFile->exists()) {
      throw "A the file or directory does not exist at this path.";
    }

    if (theInternalFile->is_directory()) {
      theInternalFile->rmdir(false);
    } else {
      theInternalFile->remove(false);
    }

    // postcondition
    // if the file/dir still exists
    if (theInternalFile->exists()) {
      throw "The file or directory at this path could not be deleted.";
    }
  ZORBA_CATCH
}

bool
FileImpl::create()
{
  bool lResult = false;

  ZORBA_TRY
    // if the file/dir exist, return false
    if (theInternalFile->exists()) {
      return false;
    }

    theInternalFile->create();

    // if the file does not exist, return false
    if (!theInternalFile->exists() || !theInternalFile->is_file()) {
      return false;
    }

    lResult = true;
  ZORBA_CATCH

  return lResult;
}

bool
FileImpl::rename(std::string const& newpath)
{
  bool lResult = false;

  ZORBA_TRY
    // if the file/dir does not exist, return false
    if (!theInternalFile->exists()) {
      return false;
    }
    // if the new path is already used, return false
    file lNewFile(newpath);
    if (lNewFile.exists()) {
      return false;
    }

    theInternalFile->rename(newpath);

    // if the file does not exist, return false
    if (!lNewFile.exists()) {
      return false;
    }

    lResult = true;
  ZORBA_CATCH

  return lResult;
}

File::FileSize_t
FileImpl::getSize() const
{
  ZORBA_TRY
    return theInternalFile->get_size();
  ZORBA_CATCH
  assert(false);
  return -1;
}

void
FileImpl::mkdir(bool aRecursive)
{
  ZORBA_TRY
    // precondition
    std::string lPath(theInternalFile->get_path());

    if (theInternalFile->exists() && !theInternalFile->is_directory()) {
      throw "FOFL0002"; // "A file already exist at this path: " << lPath
    }

    if (!aRecursive && theInternalFile->exists()) {
      throw "FOFL0004"; // "The directory already exists: " << lPath
    }

    if (aRecursive) {
      theInternalFile->deep_mkdir();
    } else {
      theInternalFile->mkdir();
    }

    // postcondition
    if (!(theInternalFile->is_directory())) {
      throw "FOFL0000"; // "Can not create directory: " << lPath
    }

  ZORBA_CATCH
}

DirectoryIterator_t
FileImpl::files() const
{
  return new DirectoryIteratorImpl(theInternalFile->get_path());
}

void
FileImpl::openInputStream(std::ifstream& aInStream, bool binary, bool trimByteOrderMark) const
{
  ZORBA_TRY
    std::string lPath(theInternalFile->get_path());

    if (!theInternalFile->exists())
      throw XQUERY_EXCEPTION(
        FODC0002, ERROR_PARAMS( lPath, ZED( FileNotFoundOrReadable ) )
      );
    if (!theInternalFile->is_file())
      throw XQUERY_EXCEPTION(
        FODC0002, ERROR_PARAMS( lPath, ZED( NotPlainFile ) )
      );

    std::ios_base::openmode lMode = std::ifstream::in;
    if (binary) {
      lMode |= std::ios_base::binary;
    }
#ifndef WIN32
    aInStream.open(lPath.c_str(), lMode);
#else
    WCHAR wpath_str[1024];
    wpath_str[0] = 0;
    if(MultiByteToWideChar(CP_UTF8,
                        0, lPath.c_str(), -1,
                        wpath_str, sizeof(wpath_str)/sizeof(WCHAR)) == 0)
    {//probably there is some invalid utf8 char, try the Windows ACP
      MultiByteToWideChar(CP_ACP,
                        0, lPath.c_str(), -1,
                        wpath_str, sizeof(wpath_str)/sizeof(WCHAR));
    }
    aInStream.open(wpath_str, lMode);
#endif

    if (aInStream.is_open() == false)
      throw XQUERY_EXCEPTION(
        FODC0002, ERROR_PARAMS( lPath, ZED( FileNotFoundOrReadable ) )
      );

    if (trimByteOrderMark) {
      char lBuf[3];
      aInStream.read(lBuf, 3);
      if (!aInStream.good() || lBuf[0] != (char)0xEF || lBuf[1] != (char)0xBB || lBuf[2] != (char)0xBF ) {
        aInStream.seekg(0);
      }
    }
  ZORBA_CATCH
}

void
FileImpl::openOutputStream(std::ofstream& aOutStream, bool binary, bool append) const
{
  ZORBA_TRY
    std::string lPath(theInternalFile->get_path());

    if (theInternalFile->exists() && !theInternalFile->is_file())
      throw XQUERY_EXCEPTION(
        FODC0002, ERROR_PARAMS( lPath, ZED( NotPlainFile ) )
      );

    std::ios_base::openmode lMode = std::ifstream::out;
    lMode |= append ? std::ios_base::app : std::ios_base::trunc;
    if (binary) {
      lMode |= std::ios_base::binary;
    }
#ifndef WIN32
    aOutStream.open(lPath.c_str(), lMode);
#else
    WCHAR wpath_str[1024];
    wpath_str[0] = 0;
    if(MultiByteToWideChar(CP_UTF8,
                        0, lPath.c_str(), -1,
                        wpath_str, sizeof(wpath_str)/sizeof(WCHAR)) == 0)
    {//probably there is some invalid utf8 char, try the Windows ACP
      MultiByteToWideChar(CP_ACP,
                        0, lPath.c_str(), -1,
                        wpath_str, sizeof(wpath_str)/sizeof(WCHAR));
    }
    aOutStream.open(wpath_str, lMode);
#endif

    if (aOutStream.is_open() == false)
      throw XQUERY_EXCEPTION(
        FODC0002, ERROR_PARAMS( lPath, ZED( FileNotFoundOrReadable ) )
      );
  ZORBA_CATCH
}

time_t
FileImpl::lastModified() const
{
  return theInternalFile->lastModified();
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */
