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
#include "stdafx.h"

#include "functions/func_collections.h"
#include "runtime/collections/collections.h"

#include "compiler/expression/expr_consts.h"
#include "compiler/expression/expr_base.h"
#include "compiler/expression/fo_expr.h"

#include "compiler/expression/pragma.h"
#include "compiler/api/compilercb.h"

#include "zorbamisc/ns_consts.h"


namespace zorba
{

/*******************************************************************************

********************************************************************************/
bool
hasNoCopyPragma(expr& e)
{
  pragma* p;
  return e.get_ccb()->lookup_pragma(&e, "no-copy", p);
}


/*******************************************************************************

********************************************************************************/
void
processPragmaInternal(expr* e, const std::vector<pragma*>& p)
{
  for (std::vector<pragma*>::const_iterator lIter = p.begin();
       lIter != p.end();
       ++lIter)
  {
    pragma* p = *lIter;
    if (p->theQName->getNamespace() == ZORBA_EXTENSIONS_NS)
    {
      if (p->theQName->getLocalName() == "no-copy")
      {
        e->get_ccb()->add_pragma(e, p);
        e->setContainsPragma(ANNOTATION_TRUE);
      }
    }
  }
}


/*******************************************************************************

********************************************************************************/
xqtref_t zorba_store_static_collections_dml_collection::getReturnType(const fo_expr* caller) const
{
  if (getName()->getNamespace() ==
      static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS)
  {
    return theSignature.returnType();
  }

  static_context* sctx = caller->get_sctx();

  const store::Item* qname = caller->get_arg(0)->getQName();

  if (qname != NULL)
  {
    const StaticallyKnownCollection* collection = sctx->lookup_collection(qname);

    if (collection != NULL)
    {
      return collection->getCollectionType();
    }
    else
    {
      return theSignature.returnType();

      //RAISE_ERROR(zerr::ZDDY0001_COLLECTION_NOT_DECLARED, caller->get_loc(),
      //ERROR_PARAMS(qname->getStringValue()));
    }
  }
  else
  {
    return theSignature.returnType();
  }
}


PlanIter_t zorba_store_static_collections_dml_collection::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  return new ZorbaCollectionIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_dml_index_of::codegen(
    CompilerCB*,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  return new ZorbaIndexOfIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_ddl_create::codegen(
    CompilerCB*,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DDL_FN_NS;

  return new ZorbaCreateCollectionIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_ddl_delete::codegen(
    CompilerCB*,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DDL_FN_NS;

  return new ZorbaDeleteCollectionIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
void zorba_store_static_collections_dml_insert::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}


PlanIter_t zorba_store_static_collections_dml_insert::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaInsertIterator(sctx, loc, argv, dynamic, copy);
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_insert_before::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_insert_before::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaInsertBeforeIterator(sctx, loc, argv, dynamic, copy);
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_insert_after::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_insert_after::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaInsertAfterIterator(sctx, loc, argv, dynamic, copy);
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_insert_first::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_insert_first::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaInsertFirstIterator(sctx, loc, argv, dynamic, copy);
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_insert_last::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_insert_last::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaInsertLastIterator(sctx, loc, argv, dynamic, copy);
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_apply_insert::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_apply_insert::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaApplyInsertIterator(sctx, loc, argv, dynamic, copy);
}


bool zorba_store_static_collections_dml_apply_insert::propagatesInputNodes(
    expr* fo,
    csize input) const
{
  return false;
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_apply_insert_first::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_apply_insert_first::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaApplyInsertFirstIterator(sctx, loc, argv, dynamic, copy);
}


bool zorba_store_static_collections_dml_apply_insert_first::propagatesInputNodes(
    expr* fo,
    csize input) const
{
  return false;
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_apply_insert_last::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}


PlanIter_t zorba_store_static_collections_dml_apply_insert_last::codegen(
    CompilerCB* cb,
    static_context* sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic = 
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaApplyInsertLastIterator(sctx, loc, argv, dynamic, copy);
}


bool zorba_store_static_collections_dml_apply_insert_last::propagatesInputNodes(
    expr* fo,
    csize input) const
{
  return false;
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_apply_insert_before::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_apply_insert_before::codegen(
  CompilerCB* cb,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic = 
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaApplyInsertBeforeIterator(sctx, loc, argv, dynamic, copy);
}


bool 
zorba_store_static_collections_dml_apply_insert_before::propagatesInputNodes(
    expr* fo,
    csize input) const
{
  return false;
}


/*******************************************************************************

********************************************************************************/
void
zorba_store_static_collections_dml_apply_insert_after::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

PlanIter_t zorba_store_static_collections_dml_apply_insert_after::codegen(
  CompilerCB* cb,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const copy = !hasNoCopyPragma(ann);

  return new ZorbaApplyInsertAfterIterator(sctx, loc, argv, dynamic, copy);
}


bool 
zorba_store_static_collections_dml_apply_insert_after::propagatesInputNodes(
    expr* fo,
    csize input) const
{
  return false;
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_dml_delete::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  return new ZorbaDeleteIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_dml_delete_first::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  return new ZorbaDeleteFirstIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_dml_delete_last::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  return new ZorbaDeleteLastIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
BoolAnnotationValue zorba_store_static_collections_dml_delete::ignoresSortedNodes(
    expr* fo,
    csize input) const 
{
  return ANNOTATION_TRUE;
}


BoolAnnotationValue zorba_store_static_collections_dml_delete::ignoresDuplicateNodes(
    expr* fo, 
    csize input) const 
{
  return ANNOTATION_TRUE;
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_dml_edit::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const lDynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  bool const lCopy = !hasNoCopyPragma(ann);

  return new ZorbaEditIterator(sctx, loc, argv, lDynamic, lCopy);
}


void
zorba_store_static_collections_dml_edit::processPragma(
    expr* e,
    const std::vector<pragma*>& p) const
{
  processPragmaInternal(e, p);
}

/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_dml_truncate::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DML_FN_NS;

  return new ZorbaTruncateCollectionIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_ddl_is_available_collection::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DDL_FN_NS;

  return new IsAvailableCollectionIterator(sctx, loc, argv, dynamic);
}


/*******************************************************************************

********************************************************************************/
PlanIter_t zorba_store_static_collections_ddl_available_collections::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  const zstring& ns = getName()->getNamespace();

  bool const dynamic =
    ns == static_context::ZORBA_STORE_DYNAMIC_COLLECTIONS_DDL_FN_NS;

  return new AvailableCollectionsIterator(sctx, loc, argv, dynamic);
}


} // namespace zorbs
/* vim:set et sw=2 ts=2: */
