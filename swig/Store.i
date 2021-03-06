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

%{  // start Implementation

  #include <zorba/store_consts.h>
 
  InMemoryStore InMemoryStore::getInstance() 
  { return InMemoryStore(zorba::StoreManager::getStore()); }

  InMemoryStore& InMemoryStore::operator=(const InMemoryStore& aStore)
  {
    theStore = aStore.theStore; 
    return *this;
  }

  void InMemoryStore::shutdown(InMemoryStore& aStore)
  { 
    zorba::StoreManager::shutdownStore(aStore.theStore); 
  }

  void* InMemoryStore::getStore() const
  {
     return theStore; 
  }

%}  // end   Implementation

%include "Store.h"
