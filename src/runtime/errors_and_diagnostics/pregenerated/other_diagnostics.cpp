/*
 * Copyright 2006-2012 The FLWOR Foundation.
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
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************


#include "stdafx.h"
#include "zorbatypes/rchandle.h"
#include "zorbatypes/zstring.h"
#include "runtime/visitors/planiter_visitor.h"
#include "runtime/errors_and_diagnostics/other_diagnostics.h"
#include "system/globalenv.h"


#include "store/api/item.h"

namespace zorba {

// <ReadLineIterator>
SERIALIZABLE_CLASS_VERSIONS(ReadLineIterator)

void ReadLineIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<ReadLineIterator, PlanIteratorState>*)this);
}


void ReadLineIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

ReadLineIterator::~ReadLineIterator() {}


zstring ReadLineIterator::getNameAsString() const {
  return "op-zorba:read-line";
}
// </ReadLineIterator>


// <PrintIterator>
SERIALIZABLE_CLASS_VERSIONS(PrintIterator)

void PrintIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<PrintIterator, PlanIteratorState>*)this);

    ar & thePrintToConsole;
}


void PrintIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

PrintIterator::~PrintIterator() {}


zstring PrintIterator::getNameAsString() const {
  return "fn-zorba-util:print";
}
// </PrintIterator>


// <FunctionTraceIterator>
SERIALIZABLE_CLASS_VERSIONS(FunctionTraceIterator)

void FunctionTraceIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<FunctionTraceIterator, PlanIteratorState>*)this);

    ar & theFunctionName;
    ar & theFunctionLocation;
    ar & theFunctionCallLocation;
    ar & theFunctionArity;
}


void FunctionTraceIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

FunctionTraceIterator::~FunctionTraceIterator() {}


zstring FunctionTraceIterator::getNameAsString() const {
  return "FunctionTraceIterator";
}
// </FunctionTraceIterator>



}


