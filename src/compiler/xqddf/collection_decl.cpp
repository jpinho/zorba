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

#include "compiler/xqddf/collection_decl.h"

#include "annotations/annotations.h"
#include "store/api/item.h"

#include "types/typeimpl.h"

#include "zorbaserialization/serialize_template_types.h"
#include "zorbaserialization/serialize_zorba_types.h"

namespace zorba {

SERIALIZABLE_CLASS_VERSIONS(StaticallyKnownCollection)


/*******************************************************************************

********************************************************************************/
StaticallyKnownCollection::StaticallyKnownCollection(
    store::Item_t& aName,
    AnnotationList* aAnnotations,
    xqtref_t& aNodeType,
    xqtref_t& aCollectionType,
    StaticContextConsts::declaration_property_t aUpdateProperty,
    StaticContextConsts::declaration_property_t aOrderProperty,
    StaticContextConsts::node_modifier_t        aNodeModifier
  )
  :
  theName(aName),
  theAnnotations(aAnnotations),
  theNodeType(aNodeType),
  theCollectionType(aCollectionType),
  theUpdateProperty(aUpdateProperty),
  theOrderProperty(aOrderProperty),
  theNodeModifier(aNodeModifier)
{
}


/*******************************************************************************

********************************************************************************/
StaticallyKnownCollection::~StaticallyKnownCollection() 
{
  delete theAnnotations;
}


/*******************************************************************************

********************************************************************************/
void StaticallyKnownCollection::serialize(::zorba::serialization::Archiver& ar)
{
  //serialize_baseclass(ar, (SimpleRCObject*)this);
    
  ar & theName;
  ar & theAnnotations;
  ar & theNodeType;
  ar & theCollectionType;
  SERIALIZE_ENUM(StaticContextConsts::declaration_property_t, theUpdateProperty);
  SERIALIZE_ENUM(StaticContextConsts::declaration_property_t, theOrderProperty);
  SERIALIZE_ENUM(StaticContextConsts::node_modifier_t, theNodeModifier);

}
  
  
}
/* vim:set et sw=2 ts=2: */
