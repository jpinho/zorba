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
#include "runtime/maths/maths.h"
#include "system/globalenv.h"



namespace zorba {

// <SqrtIterator>
SERIALIZABLE_CLASS_VERSIONS(SqrtIterator)

void SqrtIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<SqrtIterator, PlanIteratorState>*)this);
}


void SqrtIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

SqrtIterator::~SqrtIterator() {}


zstring SqrtIterator::getNameAsString() const {
  return "math:sqrt";
}
// </SqrtIterator>


// <ExpIterator>
SERIALIZABLE_CLASS_VERSIONS(ExpIterator)

void ExpIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<ExpIterator, PlanIteratorState>*)this);
}


void ExpIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

ExpIterator::~ExpIterator() {}


zstring ExpIterator::getNameAsString() const {
  return "math:exp";
}
// </ExpIterator>


// <Exp10Iterator>
SERIALIZABLE_CLASS_VERSIONS(Exp10Iterator)

void Exp10Iterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<Exp10Iterator, PlanIteratorState>*)this);
}


void Exp10Iterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

Exp10Iterator::~Exp10Iterator() {}


zstring Exp10Iterator::getNameAsString() const {
  return "math:exp10";
}
// </Exp10Iterator>


// <LogIterator>
SERIALIZABLE_CLASS_VERSIONS(LogIterator)

void LogIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<LogIterator, PlanIteratorState>*)this);
}


void LogIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

LogIterator::~LogIterator() {}


zstring LogIterator::getNameAsString() const {
  return "math:log";
}
// </LogIterator>


// <Log10Iterator>
SERIALIZABLE_CLASS_VERSIONS(Log10Iterator)

void Log10Iterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<Log10Iterator, PlanIteratorState>*)this);
}


void Log10Iterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

Log10Iterator::~Log10Iterator() {}


zstring Log10Iterator::getNameAsString() const {
  return "math:log10";
}
// </Log10Iterator>


// <SinIterator>
SERIALIZABLE_CLASS_VERSIONS(SinIterator)

void SinIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<SinIterator, PlanIteratorState>*)this);
}


void SinIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

SinIterator::~SinIterator() {}


zstring SinIterator::getNameAsString() const {
  return "math:sin";
}
// </SinIterator>


// <CosIterator>
SERIALIZABLE_CLASS_VERSIONS(CosIterator)

void CosIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<CosIterator, PlanIteratorState>*)this);
}


void CosIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

CosIterator::~CosIterator() {}


zstring CosIterator::getNameAsString() const {
  return "math:cos";
}
// </CosIterator>


// <TanIterator>
SERIALIZABLE_CLASS_VERSIONS(TanIterator)

void TanIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<TanIterator, PlanIteratorState>*)this);
}


void TanIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

TanIterator::~TanIterator() {}


zstring TanIterator::getNameAsString() const {
  return "math:tan";
}
// </TanIterator>


// <ArcSinIterator>
SERIALIZABLE_CLASS_VERSIONS(ArcSinIterator)

void ArcSinIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<ArcSinIterator, PlanIteratorState>*)this);
}


void ArcSinIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

ArcSinIterator::~ArcSinIterator() {}


zstring ArcSinIterator::getNameAsString() const {
  return "math:asin";
}
// </ArcSinIterator>


// <ArcCosIterator>
SERIALIZABLE_CLASS_VERSIONS(ArcCosIterator)

void ArcCosIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<ArcCosIterator, PlanIteratorState>*)this);
}


void ArcCosIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

ArcCosIterator::~ArcCosIterator() {}


zstring ArcCosIterator::getNameAsString() const {
  return "math:acos";
}
// </ArcCosIterator>


// <ArcTanIterator>
SERIALIZABLE_CLASS_VERSIONS(ArcTanIterator)

void ArcTanIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<ArcTanIterator, PlanIteratorState>*)this);
}


void ArcTanIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

ArcTanIterator::~ArcTanIterator() {}


zstring ArcTanIterator::getNameAsString() const {
  return "math:atan";
}
// </ArcTanIterator>


// <Atan2Iterator>
SERIALIZABLE_CLASS_VERSIONS(Atan2Iterator)

void Atan2Iterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (BinaryBaseIterator<Atan2Iterator, PlanIteratorState>*)this);
}


void Atan2Iterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild0->accept(v);
theChild1->accept(v);

  v.endVisit(*this);
}

Atan2Iterator::~Atan2Iterator() {}


zstring Atan2Iterator::getNameAsString() const {
  return "math:atan2";
}
// </Atan2Iterator>


// <CoshIterator>
SERIALIZABLE_CLASS_VERSIONS(CoshIterator)

void CoshIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<CoshIterator, PlanIteratorState>*)this);
}


void CoshIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

CoshIterator::~CoshIterator() {}


zstring CoshIterator::getNameAsString() const {
  return "fn-zorba-math:cosh";
}
// </CoshIterator>


// <AcoshIterator>
SERIALIZABLE_CLASS_VERSIONS(AcoshIterator)

void AcoshIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<AcoshIterator, PlanIteratorState>*)this);
}


void AcoshIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

AcoshIterator::~AcoshIterator() {}


zstring AcoshIterator::getNameAsString() const {
  return "fn-zorba-math:acosh";
}
// </AcoshIterator>


// <FmodIterator>
SERIALIZABLE_CLASS_VERSIONS(FmodIterator)

void FmodIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (BinaryBaseIterator<FmodIterator, PlanIteratorState>*)this);
}


void FmodIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild0->accept(v);
theChild1->accept(v);

  v.endVisit(*this);
}

FmodIterator::~FmodIterator() {}


zstring FmodIterator::getNameAsString() const {
  return "fn-zorba-math:fmod";
}
// </FmodIterator>


// <LdexpIterator>
SERIALIZABLE_CLASS_VERSIONS(LdexpIterator)

void LdexpIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (BinaryBaseIterator<LdexpIterator, PlanIteratorState>*)this);
}


void LdexpIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild0->accept(v);
theChild1->accept(v);

  v.endVisit(*this);
}

LdexpIterator::~LdexpIterator() {}


zstring LdexpIterator::getNameAsString() const {
  return "fn-zorba-math:ldexp";
}
// </LdexpIterator>


// <PowIterator>
SERIALIZABLE_CLASS_VERSIONS(PowIterator)

void PowIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (BinaryBaseIterator<PowIterator, PlanIteratorState>*)this);
}


void PowIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild0->accept(v);
theChild1->accept(v);

  v.endVisit(*this);
}

PowIterator::~PowIterator() {}


zstring PowIterator::getNameAsString() const {
  return "math:pow";
}
// </PowIterator>


// <SinhIterator>
SERIALIZABLE_CLASS_VERSIONS(SinhIterator)

void SinhIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<SinhIterator, PlanIteratorState>*)this);
}


void SinhIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

SinhIterator::~SinhIterator() {}


zstring SinhIterator::getNameAsString() const {
  return "fn-zorba-math:sinh";
}
// </SinhIterator>


// <AsinhIterator>
SERIALIZABLE_CLASS_VERSIONS(AsinhIterator)

void AsinhIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<AsinhIterator, PlanIteratorState>*)this);
}


void AsinhIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

AsinhIterator::~AsinhIterator() {}


zstring AsinhIterator::getNameAsString() const {
  return "fn-zorba-math:asinh";
}
// </AsinhIterator>


// <TanhIterator>
SERIALIZABLE_CLASS_VERSIONS(TanhIterator)

void TanhIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<TanhIterator, PlanIteratorState>*)this);
}


void TanhIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

TanhIterator::~TanhIterator() {}


zstring TanhIterator::getNameAsString() const {
  return "fn-zorba-math:tanh";
}
// </TanhIterator>


// <AtanhIterator>
SERIALIZABLE_CLASS_VERSIONS(AtanhIterator)

void AtanhIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<AtanhIterator, PlanIteratorState>*)this);
}


void AtanhIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

AtanhIterator::~AtanhIterator() {}


zstring AtanhIterator::getNameAsString() const {
  return "fn-zorba-math:atanh";
}
// </AtanhIterator>


// <PiNumberIterator>
SERIALIZABLE_CLASS_VERSIONS(PiNumberIterator)

void PiNumberIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NoaryBaseIterator<PiNumberIterator, PlanIteratorState>*)this);
}


void PiNumberIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  

  v.endVisit(*this);
}

PiNumberIterator::~PiNumberIterator() {}


zstring PiNumberIterator::getNameAsString() const {
  return "math:pi";
}
// </PiNumberIterator>


// <IsInfIterator>
SERIALIZABLE_CLASS_VERSIONS(IsInfIterator)

void IsInfIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<IsInfIterator, PlanIteratorState>*)this);
}


void IsInfIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

IsInfIterator::~IsInfIterator() {}


zstring IsInfIterator::getNameAsString() const {
  return "fn-zorba-math:is_inf";
}
// </IsInfIterator>


// <IsNaNIterator>
SERIALIZABLE_CLASS_VERSIONS(IsNaNIterator)

void IsNaNIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<IsNaNIterator, PlanIteratorState>*)this);
}


void IsNaNIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

IsNaNIterator::~IsNaNIterator() {}


zstring IsNaNIterator::getNameAsString() const {
  return "fn-zorba-math:is_nan";
}
// </IsNaNIterator>


// <ModfIterator>
SERIALIZABLE_CLASS_VERSIONS(ModfIterator)

void ModfIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<ModfIterator, ModfIteratorState>*)this);
}


void ModfIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

ModfIterator::~ModfIterator() {}

ModfIteratorState::ModfIteratorState() {}

ModfIteratorState::~ModfIteratorState() {}


void ModfIteratorState::init(PlanState& planState) {
  PlanIteratorState::init(planState);
  theDoubInteger = numeric_consts<xs_double>::zero();
}

void ModfIteratorState::reset(PlanState& planState) {
  PlanIteratorState::reset(planState);
  theDoubInteger = numeric_consts<xs_double>::zero();
}

zstring ModfIterator::getNameAsString() const {
  return "fn-zorba-math:modf";
}
// </ModfIterator>


// <FrexpIterator>
SERIALIZABLE_CLASS_VERSIONS(FrexpIterator)

void FrexpIterator::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (UnaryBaseIterator<FrexpIterator, FrexpIteratorState>*)this);
}


void FrexpIterator::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  theChild->accept(v);

  v.endVisit(*this);
}

FrexpIterator::~FrexpIterator() {}

FrexpIteratorState::FrexpIteratorState() {}

FrexpIteratorState::~FrexpIteratorState() {}


void FrexpIteratorState::init(PlanState& planState) {
  PlanIteratorState::init(planState);
  theIntExponent = numeric_consts<xs_integer>::zero();
}

void FrexpIteratorState::reset(PlanState& planState) {
  PlanIteratorState::reset(planState);
  theIntExponent = numeric_consts<xs_integer>::zero();
}

zstring FrexpIterator::getNameAsString() const {
  return "fn-zorba-math:frexp";
}
// </FrexpIterator>



}


