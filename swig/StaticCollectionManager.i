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

%{  // start Implementation

  ItemSequence StaticCollectionManager::availableCollections()
  {
    return ItemSequence( theStaticManager->availableCollections() );
  }

  ItemSequence StaticCollectionManager::availableIndexes()
  {
    return ItemSequence( theStaticManager->availableIndexes() );
  }

  void StaticCollectionManager::createCollection(const Item &aName )
  {
    theStaticManager->createCollection(aName.theItem );
  }

  void StaticCollectionManager::createCollection(const Item &aName, const ItemSequence &aContents )
  {
    theStaticManager->createCollection(aName.theItem, aContents.theItemSequence );
  }

  void StaticCollectionManager::createIndex( const Item & aQName )
  {
    theStaticManager->createIndex(aQName.theItem );
  }

  ItemSequence StaticCollectionManager::declaredCollections()
  {
    return ItemSequence( theStaticManager->declaredCollections() );
  }

  ItemSequence StaticCollectionManager::declaredIndexes()
  {
    return ItemSequence( theStaticManager->declaredIndexes() );
  }

  void StaticCollectionManager::deleteCollection(const Item &aName )
  {
    theStaticManager->deleteCollection(aName.theItem );
  }

  void StaticCollectionManager::deleteIndex(const Item &aQName )
  {
    theStaticManager->deleteIndex(aQName.theItem );
  }

  Collection StaticCollectionManager::getCollection(const Item &aName )
  {
    return Collection(theStaticManager->getCollection(aName.theItem ));
  }

  bool StaticCollectionManager::isAvailableCollection(const Item &aName )
  {
    return theStaticManager->isAvailableCollection(aName.theItem );
  }

  bool StaticCollectionManager::isAvailableIndex(const Item &aQName )
  {
    return theStaticManager->isAvailableIndex(aQName.theItem );
  }

  bool StaticCollectionManager::isDeclaredCollection( const Item & aQName )
  {
    return theStaticManager->isDeclaredCollection(aQName.theItem );
  }

  bool StaticCollectionManager::isDeclaredIndex( const Item & aQName )
  {
    return theStaticManager->isDeclaredIndex(aQName.theItem );
  }

  void StaticCollectionManager::registerDiagnosticHandler(DiagnosticHandler *aDiagnosticHandler)
  {
    theStaticManager->registerDiagnosticHandler(aDiagnosticHandler);
  }

%}  // end   Implementation

%include "StaticCollectionManager.h"
