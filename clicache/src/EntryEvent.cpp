/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "EntryEvent.hpp"
#include "Region.hpp"
#include "impl/SafeConvert.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {

      generic<class TKey, class TValue>
      IRegion<TKey, TValue>^ EntryEvent<TKey, TValue>::Region::get( )
      {
        apache::geode::client::RegionPtr regionptr = m_nativeptr->getRegion();
        return Client::Region<TKey, TValue>::Create( regionptr );
      }

      generic<class TKey, class TValue>
      TKey EntryEvent<TKey, TValue>::Key::get( )
      {
        apache::geode::client::CacheableKeyPtr& keyptr( m_nativeptr->getKey( ) );
        return Serializable::GetManagedValueGeneric<TKey>( keyptr );
      }

      generic<class TKey, class TValue>
      TValue EntryEvent<TKey, TValue>::OldValue::get( )
      {
        apache::geode::client::CacheablePtr& valptr( m_nativeptr->getOldValue( ) );
        return Serializable::GetManagedValueGeneric<TValue>( valptr );
      }

      generic<class TKey, class TValue>
      TValue EntryEvent<TKey, TValue>::NewValue::get( )
      {
        apache::geode::client::CacheablePtr& valptr( m_nativeptr->getNewValue( ) );
        return Serializable::GetManagedValueGeneric<TValue>( valptr );
      }

      generic<class TKey, class TValue>
      Object^ EntryEvent<TKey, TValue>::CallbackArgument::get()
      {
        apache::geode::client::SerializablePtr& valptr(m_nativeptr->getCallbackArgument());
        return Serializable::GetManagedValueGeneric<Object^>( valptr );
      }

      generic<class TKey, class TValue>
      bool EntryEvent<TKey, TValue>::RemoteOrigin::get()
      {
        return m_nativeptr->remoteOrigin();
      }
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache

