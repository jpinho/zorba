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
#include "runtime/full_text/ft_module.h"
#include "system/globalenv.h"


#include "store/api/iterator.h"

namespace zorba {

// <CurrentLangIterator>
CurrentLangIterator::class_factory<CurrentLangIterator>
CurrentLangIterator::g_class_factory;


void CurrentLangIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

CurrentLangIterator::~CurrentLangIterator() {}

// </CurrentLangIterator>


// <HostLangIterator>
HostLangIterator::class_factory<HostLangIterator>
HostLangIterator::g_class_factory;


void HostLangIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

HostLangIterator::~HostLangIterator() {}

// </HostLangIterator>


// <IsStemLangSupportedIterator>
IsStemLangSupportedIterator::class_factory<IsStemLangSupportedIterator>
IsStemLangSupportedIterator::g_class_factory;


void IsStemLangSupportedIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

IsStemLangSupportedIterator::~IsStemLangSupportedIterator() {}

// </IsStemLangSupportedIterator>


// <IsStopWordIterator>
IsStopWordIterator::class_factory<IsStopWordIterator>
IsStopWordIterator::g_class_factory;


void IsStopWordIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

IsStopWordIterator::~IsStopWordIterator() {}

// </IsStopWordIterator>


// <IsStopWordLangSupportedIterator>
IsStopWordLangSupportedIterator::class_factory<IsStopWordLangSupportedIterator>
IsStopWordLangSupportedIterator::g_class_factory;


void IsStopWordLangSupportedIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

IsStopWordLangSupportedIterator::~IsStopWordLangSupportedIterator() {}

// </IsStopWordLangSupportedIterator>


// <IsThesaurusLangSupportedIterator>
IsThesaurusLangSupportedIterator::class_factory<IsThesaurusLangSupportedIterator>
IsThesaurusLangSupportedIterator::g_class_factory;


void IsThesaurusLangSupportedIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

IsThesaurusLangSupportedIterator::~IsThesaurusLangSupportedIterator() {}

// </IsThesaurusLangSupportedIterator>


// <IsTokenizerLangSupportedIterator>
IsTokenizerLangSupportedIterator::class_factory<IsTokenizerLangSupportedIterator>
IsTokenizerLangSupportedIterator::g_class_factory;


void IsTokenizerLangSupportedIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

IsTokenizerLangSupportedIterator::~IsTokenizerLangSupportedIterator() {}

// </IsTokenizerLangSupportedIterator>


// <StemIterator>
StemIterator::class_factory<StemIterator>
StemIterator::g_class_factory;


void StemIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

StemIterator::~StemIterator() {}

// </StemIterator>


// <StripDiacriticsIterator>
StripDiacriticsIterator::class_factory<StripDiacriticsIterator>
StripDiacriticsIterator::g_class_factory;


void StripDiacriticsIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

StripDiacriticsIterator::~StripDiacriticsIterator() {}

// </StripDiacriticsIterator>


// <ThesaurusLookupIterator>
ThesaurusLookupIterator::class_factory<ThesaurusLookupIterator>
ThesaurusLookupIterator::g_class_factory;


void ThesaurusLookupIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

ThesaurusLookupIterator::~ThesaurusLookupIterator() {}

ThesaurusLookupIteratorState::ThesaurusLookupIteratorState() {}

ThesaurusLookupIteratorState::~ThesaurusLookupIteratorState() {}


void ThesaurusLookupIteratorState::reset(PlanState& planState) {
  PlanIteratorState::reset(planState);
}
// </ThesaurusLookupIterator>


// <TokenizeIterator>
TokenizeIterator::class_factory<TokenizeIterator>
TokenizeIterator::g_class_factory;


void TokenizeIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

TokenizeIterator::~TokenizeIterator() {}

TokenizeIteratorState::TokenizeIteratorState() {}

TokenizeIteratorState::~TokenizeIteratorState() {}


void TokenizeIteratorState::reset(PlanState& planState) {
  PlanIteratorState::reset(planState);
}
// </TokenizeIterator>


// <TokenizerPropertiesIterator>
TokenizerPropertiesIterator::class_factory<TokenizerPropertiesIterator>
TokenizerPropertiesIterator::g_class_factory;


void TokenizerPropertiesIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

TokenizerPropertiesIterator::~TokenizerPropertiesIterator() {}

// </TokenizerPropertiesIterator>


// <TokenizeStringIterator>
TokenizeStringIterator::class_factory<TokenizeStringIterator>
TokenizeStringIterator::g_class_factory;


void TokenizeStringIterator::accept(PlanIterVisitor& v) const {
  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

TokenizeStringIterator::~TokenizeStringIterator() {}

TokenizeStringIteratorState::TokenizeStringIteratorState() {}

TokenizeStringIteratorState::~TokenizeStringIteratorState() {}


void TokenizeStringIteratorState::reset(PlanState& planState) {
  PlanIteratorState::reset(planState);
}
// </TokenizeStringIterator>



}


