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
#include "runtime/random/random.h"
#include "functions/func_random.h"


namespace zorba{



PlanIter_t fn_zorba_random_seeded_random::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new SeededRandomIterator(sctx, loc, argv);
}

PlanIter_t fn_zorba_random_random::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new RandomIterator(sctx, loc, argv);
}

PlanIter_t fn_zorba_util_uuid::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new UuidIterator(sctx, loc, argv);
}

void populate_context_random(static_context* sctx)
{


      {
    DECL_WITH_KIND(sctx, fn_zorba_random_seeded_random,
        (createQName("http://zorba.io/modules/random","","seeded-random"), 
        GENV_TYPESYSTEM.INTEGER_TYPE_ONE, 
        GENV_TYPESYSTEM.INTEGER_TYPE_ONE, 
        GENV_TYPESYSTEM.INTEGER_TYPE_STAR),
        FunctionConsts::FN_ZORBA_RANDOM_SEEDED_RANDOM_2);

  }




      {
    DECL_WITH_KIND(sctx, fn_zorba_random_random,
        (createQName("http://zorba.io/modules/random","","random"), 
        GENV_TYPESYSTEM.INTEGER_TYPE_ONE, 
        GENV_TYPESYSTEM.INTEGER_TYPE_STAR),
        FunctionConsts::FN_ZORBA_RANDOM_RANDOM_1);

  }




      {
    DECL_WITH_KIND(sctx, fn_zorba_util_uuid,
        (createQName("http://zorba.io/util-functions","","uuid"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE),
        FunctionConsts::FN_ZORBA_UTIL_UUID_0);

  }




      {
    DECL_WITH_KIND(sctx, fn_zorba_util_uuid,
        (createQName("http://zorba.io/modules/random","","uuid"), 
        GENV_TYPESYSTEM.STRING_TYPE_ONE),
        FunctionConsts::FN_ZORBA_RANDOM_UUID_0);

  }

}


}



