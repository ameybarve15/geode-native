#pragma once

#ifndef GEODE_LOCALREGION_H_
#define GEODE_LOCALREGION_H_

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

/**
 * @file
 */

#include <geode/geode_globals.hpp>
#include <geode/CacheStatistics.hpp>
#include <geode/ExceptionTypes.hpp>
#include <geode/CacheableKey.hpp>
#include <geode/Cacheable.hpp>
#include <geode/Cache.hpp>
#include <geode/EntryEvent.hpp>
#include <geode/RegionEvent.hpp>
#include "EventType.hpp"
#include <geode/PersistenceManager.hpp>
#include <geode/RegionEntry.hpp>
#include <geode/CacheListener.hpp>
#include <geode/CacheWriter.hpp>
#include <geode/CacheLoader.hpp>
#include <geode/AttributesMutator.hpp>
#include <geode/AttributesFactory.hpp>

#include "RegionInternal.hpp"
#include "RegionStats.hpp"
#include "EntriesMapFactory.hpp"
#include "SerializationRegistry.hpp"
#include "MapWithLock.hpp"
#include "CacheableToken.hpp"
#include "ExpMapEntry.hpp"
#include "TombstoneList.hpp"

#include <ace/ACE.h>
#include <ace/Hash_Map_Manager_T.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <string>
#include <unordered_map>
#include "TSSTXStateWrapper.hpp"

namespace apache {
namespace geode {
namespace client {

#ifndef CHECK_DESTROY_PENDING
#define CHECK_DESTROY_PENDING(lock, function)                      \
  lock checkGuard(m_rwLock, m_destroyPending);                     \
  if (m_destroyPending) {                                          \
    std::string err_msg = ": region " + m_fullPath + " destroyed"; \
    throw RegionDestroyedException(#function, err_msg.c_str());    \
  }
#endif

#ifndef CHECK_DESTROY_PENDING_NOTHROW
#define CHECK_DESTROY_PENDING_NOTHROW(lock)     \
  lock checkGuard(m_rwLock, m_destroyPending);  \
  if (m_destroyPending) {                       \
    return GF_CACHE_REGION_DESTROYED_EXCEPTION; \
  }
#endif

class PutActions;
class PutActionsTx;
class CreateActions;
class DestroyActions;
class RemoveActions;
class InvalidateActions;

typedef std::unordered_map<CacheableKeyPtr, std::pair<CacheablePtr, int> >
    MapOfOldValue;

/**
 * @class LocalRegion LocalRegion.hpp
 *
 * This class manages subregions and cached data. Each region
 * can contain multiple subregions and entries for data.
 * Regions provide a hierachical name space
 * within the cache. Also, a region can be used to group cached
 * objects for management purposes.
 *
 * The Region interface basically contains two set of APIs: Region management
 * APIs; and (potentially) distributed operations on entries. Non-distributed
 * operations on entries  are provided by <code>RegionEntry</code>.
 *
 * Each <code>Cache</code>  defines regions called the root regions.
 * User applications can use the root regions to create subregions
 * for isolated name space and object grouping.
 *
 * A region's name can be any String except that it should not contain
 * the region name separator, a forward slash (/).
 *
 * <code>Regions</code>  can be referenced by a relative path name from any
 * region
 * higher in the hierarchy in {@link Region::getSubregion}. You can get the
 * relative
 * path from the root region with {@link Region::getFullPath}. The name
 * separator is used to concatenate all the region names together from the root,
 * starting with the root's subregions.
 */
typedef std::shared_ptr<LocalRegion> LocalRegionPtr;

class CPPCACHE_EXPORT LocalRegion : public RegionInternal {
  /**
   * @brief Public Methods for Region
   */
 public:
  /**
   * @brief constructor/destructor
   */
  LocalRegion(const std::string& name, CacheImpl* cache,
              const RegionInternalPtr& rPtr,
              const RegionAttributesPtr& attributes,
              const CacheStatisticsPtr& stats, bool shared = false,
              bool enableTimeStatistics = true);
  virtual ~LocalRegion();

  const char* getName() const;
  const char* getFullPath() const;
  RegionPtr getParentRegion() const;
  RegionAttributesPtr getAttributes() const { return m_regionAttributes; }
  AttributesMutatorPtr getAttributesMutator() const {
    return std::make_shared<AttributesMutator>(
        std::const_pointer_cast<LocalRegion>(
            std::static_pointer_cast<const LocalRegion>(shared_from_this())));
  }
  void updateAccessAndModifiedTime(bool modified);
  CacheStatisticsPtr getStatistics() const;
  virtual void clear(const SerializablePtr& aCallbackArgument = nullptr);
  virtual void localClear(const SerializablePtr& aCallbackArgument = nullptr);
  GfErrType localClearNoThrow(
      const SerializablePtr& aCallbackArgument = nullptr,
      const CacheEventFlags eventFlags = CacheEventFlags::NORMAL);
  void invalidateRegion(const SerializablePtr& aCallbackArgument = nullptr);
  void localInvalidateRegion(const SerializablePtr& aCallbackArgument = nullptr);
  void destroyRegion(const SerializablePtr& aCallbackArgument = nullptr);
  void localDestroyRegion(const SerializablePtr& aCallbackArgument = nullptr);
  RegionPtr getSubregion(const char* path);
  RegionPtr createSubregion(const char* subregionName,
                            const RegionAttributesPtr& aRegionAttributes);
  void subregions(const bool recursive, VectorOfRegion& sr);
  RegionEntryPtr getEntry(const CacheableKeyPtr& key);
  void getEntry(const CacheableKeyPtr& key, CacheablePtr& valuePtr);
  CacheablePtr get(const CacheableKeyPtr& key,
                   const SerializablePtr& aCallbackArgument);
  void put(const CacheableKeyPtr& key, const CacheablePtr& value,
           const SerializablePtr& aCallbackArgument = nullptr);
  void localPut(const CacheableKeyPtr& key, const CacheablePtr& value,
                const SerializablePtr& aCallbackArgument = nullptr);
  void create(const CacheableKeyPtr& key, const CacheablePtr& value,
              const SerializablePtr& aCallbackArgument = nullptr);
  void localCreate(const CacheableKeyPtr& key, const CacheablePtr& value,
                   const SerializablePtr& aCallbackArgument = nullptr);
  void invalidate(const CacheableKeyPtr& key,
                  const SerializablePtr& aCallbackArgument = nullptr);
  void localInvalidate(const CacheableKeyPtr& key,
                       const SerializablePtr& aCallbackArgument = nullptr);
  void destroy(const CacheableKeyPtr& key,
               const SerializablePtr& aCallbackArgument = nullptr);
  void localDestroy(const CacheableKeyPtr& key,
                    const SerializablePtr& aCallbackArgument = nullptr);
  bool remove(const CacheableKeyPtr& key, const CacheablePtr& value,
              const SerializablePtr& aCallbackArgument = nullptr);
  bool removeEx(const CacheableKeyPtr& key,
                const SerializablePtr& aCallbackArgument = nullptr);
  bool localRemove(const CacheableKeyPtr& key, const CacheablePtr& value,
                   const SerializablePtr& aCallbackArgument = nullptr);
  bool localRemoveEx(const CacheableKeyPtr& key,
                     const SerializablePtr& aCallbackArgument = nullptr);
  void keys(VectorOfCacheableKey& v);
  void serverKeys(VectorOfCacheableKey& v);
  void values(VectorOfCacheable& vc);
  void entries(VectorOfRegionEntry& me, bool recursive);
  void getAll(const VectorOfCacheableKey& keys, HashMapOfCacheablePtr values,
              HashMapOfExceptionPtr exceptions, bool addToLocalCache,
              const SerializablePtr& aCallbackArgument = nullptr);
  void putAll(const HashMapOfCacheable& map,
              uint32_t timeout = DEFAULT_RESPONSE_TIMEOUT,
              const SerializablePtr& aCallbackArgument = nullptr);
  void removeAll(const VectorOfCacheableKey& keys,
                 const SerializablePtr& aCallbackArgument = nullptr);
  uint32_t size();
  virtual uint32_t size_remote();
  RegionServicePtr getRegionService() const;
  virtual bool containsValueForKey_remote(const CacheableKeyPtr& keyPtr) const;
  bool containsValueForKey(const CacheableKeyPtr& keyPtr) const;
  bool containsKey(const CacheableKeyPtr& keyPtr) const;
  virtual bool containsKeyOnServer(const CacheableKeyPtr& keyPtr) const;
  virtual void getInterestList(VectorOfCacheableKey& vlist) const;
  virtual void getInterestListRegex(VectorOfCacheableString& vregex) const;

  /** @brief Public Methods from RegionInternal
   *  There are all virtual methods
   */
  PersistenceManagerPtr getPersistenceManager() { return m_persistenceManager; }
  void setPersistenceManager(PersistenceManagerPtr& pmPtr);

  virtual GfErrType getNoThrow(const CacheableKeyPtr& key, CacheablePtr& value,
                               const SerializablePtr& aCallbackArgument);
  virtual GfErrType getAllNoThrow(
      const VectorOfCacheableKey& keys, const HashMapOfCacheablePtr& values,
      const HashMapOfExceptionPtr& exceptions, bool addToLocalCache,
      const SerializablePtr& aCallbackArgument = nullptr);
  virtual GfErrType putNoThrow(const CacheableKeyPtr& key,
                               const CacheablePtr& value,
                               const SerializablePtr& aCallbackArgument,
                               CacheablePtr& oldValue, int updateCount,
                               const CacheEventFlags eventFlags,
                               VersionTagPtr versionTag,
                               DataInput* delta = nullptr,
                               EventIdPtr eventId = nullptr);
  virtual GfErrType putNoThrowTX(const CacheableKeyPtr& key,
                                 const CacheablePtr& value,
                                 const SerializablePtr& aCallbackArgument,
                                 CacheablePtr& oldValue, int updateCount,
                                 const CacheEventFlags eventFlags,
                                 VersionTagPtr versionTag,
                                 DataInput* delta = nullptr,
                                 EventIdPtr eventId = nullptr);
  virtual GfErrType createNoThrow(const CacheableKeyPtr& key,
                                  const CacheablePtr& value,
                                  const SerializablePtr& aCallbackArgument,
                                  int updateCount,
                                  const CacheEventFlags eventFlags,
                                  VersionTagPtr versionTag);
  virtual GfErrType destroyNoThrow(const CacheableKeyPtr& key,
                                   const SerializablePtr& aCallbackArgument,
                                   int updateCount,
                                   const CacheEventFlags eventFlags,
                                   VersionTagPtr versionTag);
  virtual GfErrType destroyNoThrowTX(const CacheableKeyPtr& key,
                                     const SerializablePtr& aCallbackArgument,
                                     int updateCount,
                                     const CacheEventFlags eventFlags,
                                     VersionTagPtr versionTag);
  virtual GfErrType removeNoThrow(const CacheableKeyPtr& key,
                                  const CacheablePtr& value,
                                  const SerializablePtr& aCallbackArgument,
                                  int updateCount,
                                  const CacheEventFlags eventFlags,
                                  VersionTagPtr versionTag);
  virtual GfErrType removeNoThrowEx(const CacheableKeyPtr& key,
                                    const SerializablePtr& aCallbackArgument,
                                    int updateCount,
                                    const CacheEventFlags eventFlags,
                                    VersionTagPtr versionTag);
  virtual GfErrType putAllNoThrow(
      const HashMapOfCacheable& map,
      uint32_t timeout = DEFAULT_RESPONSE_TIMEOUT,
      const SerializablePtr& aCallbackArgument = nullptr);
  virtual GfErrType removeAllNoThrow(
      const VectorOfCacheableKey& keys,
      const SerializablePtr& aCallbackArgument = nullptr);
  virtual GfErrType invalidateNoThrow(const CacheableKeyPtr& keyPtr,
                                      const SerializablePtr& aCallbackArgument,
                                      int updateCount,
                                      const CacheEventFlags eventFlags,
                                      VersionTagPtr versionTag);
  virtual GfErrType invalidateNoThrowTX(const CacheableKeyPtr& keyPtr,
                                        const SerializablePtr& aCallbackArgument,
                                        int updateCount,
                                        const CacheEventFlags eventFlags,
                                        VersionTagPtr versionTag);
  GfErrType invalidateRegionNoThrow(const SerializablePtr& aCallbackArgument,
                                    const CacheEventFlags eventFlags);
  GfErrType destroyRegionNoThrow(const SerializablePtr& aCallbackArgument,
                                 bool removeFromParent,
                                 const CacheEventFlags eventFlags);
  void tombstoneOperationNoThrow(const CacheableHashMapPtr& tombstoneVersions,
                                 const CacheableHashSetPtr& tombstoneKeys);

  //  moved putLocal to public since this is used by a few other
  // classes like CacheableObjectPartList now
  /** put an entry in local cache without invoking any callbacks */
  GfErrType putLocal(const char* name, bool isCreate,
                     const CacheableKeyPtr& keyPtr,
                     const CacheablePtr& valuePtr, CacheablePtr& oldValue,
                     bool cachingEnabled, int updateCount, int destroyTracker,
                     VersionTagPtr versionTag, DataInput* delta = nullptr,
                     EventIdPtr eventId = nullptr);
  GfErrType invalidateLocal(const char* name, const CacheableKeyPtr& keyPtr,
                            const CacheablePtr& value,
                            const CacheEventFlags eventFlags,
                            VersionTagPtr versionTag);

  void setRegionExpiryTask();
  void acquireReadLock() { m_rwLock.acquire_read(); }
  void releaseReadLock() { m_rwLock.release(); }

  // behaviors for attributes mutator
  uint32_t adjustLruEntriesLimit(uint32_t limit);
  ExpirationAction::Action adjustRegionExpiryAction(
      ExpirationAction::Action action);
  ExpirationAction::Action adjustEntryExpiryAction(
      ExpirationAction::Action action);
  int32_t adjustRegionExpiryDuration(int32_t duration);
  int32_t adjustEntryExpiryDuration(int32_t duration);

  // other public methods
  RegionStats* getRegionStats() { return m_regionStats; }
  inline bool cacheEnabled() { return m_regionAttributes->getCachingEnabled(); }
  inline bool cachelessWithListener() {
    return !m_regionAttributes->getCachingEnabled() && (m_listener != nullptr);
  }
  virtual bool isDestroyed() const { return m_destroyPending; }
  /* above public methods are inherited from RegionInternal */

  virtual void adjustCacheListener(const CacheListenerPtr& aListener);
  virtual void adjustCacheListener(const char* libpath,
                                   const char* factoryFuncName);
  virtual void adjustCacheLoader(const CacheLoaderPtr& aLoader);
  virtual void adjustCacheLoader(const char* libpath,
                                 const char* factoryFuncName);
  virtual void adjustCacheWriter(const CacheWriterPtr& aWriter);
  virtual void adjustCacheWriter(const char* libpath,
                                 const char* factoryFuncName);
  virtual CacheImpl* getCacheImpl() const;
  virtual void evict(int32_t percentage);

  virtual void acquireGlobals(bool isFailover){};
  virtual void releaseGlobals(bool isFailover){};

  virtual bool getProcessedMarker() { return true; }
  EntriesMap* getEntryMap() { return m_entries; }
  virtual TombstoneListPtr getTombstoneList();

 protected:
  /* virtual protected methods */
  virtual void release(bool invokeCallbacks = true);
  virtual GfErrType getNoThrow_remote(const CacheableKeyPtr& keyPtr,
                                      CacheablePtr& valPtr,
                                      const SerializablePtr& aCallbackArgument,
                                      VersionTagPtr& versionTag);
  virtual GfErrType putNoThrow_remote(const CacheableKeyPtr& keyPtr,
                                      const CacheablePtr& cvalue,
                                      const SerializablePtr& aCallbackArgument,
                                      VersionTagPtr& versionTag,
                                      bool checkDelta = true);
  virtual GfErrType putAllNoThrow_remote(
      const HashMapOfCacheable& map,
      VersionedCacheableObjectPartListPtr& versionedObjPartList,
      uint32_t timeout, const SerializablePtr& aCallbackArgument);
  virtual GfErrType removeAllNoThrow_remote(
      const VectorOfCacheableKey& keys,
      VersionedCacheableObjectPartListPtr& versionedObjPartList,
      const SerializablePtr& aCallbackArgument);
  virtual GfErrType createNoThrow_remote(const CacheableKeyPtr& keyPtr,
                                         const CacheablePtr& cvalue,
                                         const SerializablePtr& aCallbackArgument,
                                         VersionTagPtr& versionTag);
  virtual GfErrType destroyNoThrow_remote(const CacheableKeyPtr& keyPtr,
                                          const SerializablePtr& aCallbackArgument,
                                          VersionTagPtr& versionTag);
  virtual GfErrType removeNoThrow_remote(const CacheableKeyPtr& keyPtr,
                                         const CacheablePtr& cvalue,
                                         const SerializablePtr& aCallbackArgument,
                                         VersionTagPtr& versionTag);
  virtual GfErrType removeNoThrowEX_remote(const CacheableKeyPtr& keyPtr,
                                           const SerializablePtr& aCallbackArgument,
                                           VersionTagPtr& versionTag);
  virtual GfErrType invalidateNoThrow_remote(
      const CacheableKeyPtr& keyPtr, const SerializablePtr& aCallbackArgument,
      VersionTagPtr& versionTag);
  virtual GfErrType getAllNoThrow_remote(
      const VectorOfCacheableKey* keys, const HashMapOfCacheablePtr& values,
      const HashMapOfExceptionPtr& exceptions,
      const VectorOfCacheableKeyPtr& resultKeys, bool addToLocalCache,
      const SerializablePtr& aCallbackArgument);
  virtual GfErrType invalidateRegionNoThrow_remote(
      const SerializablePtr& aCallbackArgument);
  virtual GfErrType destroyRegionNoThrow_remote(
      const SerializablePtr& aCallbackArgument);
  virtual GfErrType unregisterKeysBeforeDestroyRegion();
  virtual const PoolPtr& getPool() { return m_attachedPool; }

  void setPool(const PoolPtr& p) { m_attachedPool = p; }

  TXState* getTXState() const {
    return TSSTXStateWrapper::s_geodeTSSTXState->getTXState();
  }

  CacheablePtr handleReplay(GfErrType& err, CacheablePtr value) const;

  bool isLocalOp(const CacheEventFlags* eventFlags = nullptr) {
    return typeid(*this) == typeid(LocalRegion) ||
           (eventFlags && eventFlags->isLocal());
  }

  // template method for put and create
  template <typename TAction>
  GfErrType updateNoThrow(const CacheableKeyPtr& key, const CacheablePtr& value,
                          const SerializablePtr& aCallbackArgument,
                          CacheablePtr& oldValue, int updateCount,
                          const CacheEventFlags eventFlags,
                          VersionTagPtr versionTag, DataInput* delta = nullptr,
                          EventIdPtr eventId = nullptr);

  template <typename TAction>
  GfErrType updateNoThrowTX(const CacheableKeyPtr& key,
                            const CacheablePtr& value,
                            const SerializablePtr& aCallbackArgument,
                            CacheablePtr& oldValue, int updateCount,
                            const CacheEventFlags eventFlags,
                            VersionTagPtr versionTag,
                            DataInput* delta = nullptr,
                            EventIdPtr eventId = nullptr);

  int64_t startStatOpTime();
  void updateStatOpTime(Statistics* m_regionStats, int32_t statId,
                        int64_t start);

  /* protected attributes */
  std::string m_name;
  RegionPtr m_parentRegion;
  MapOfRegionWithLock m_subRegions;
  std::string m_fullPath;
  CacheImpl* m_cacheImpl;
  volatile bool m_destroyPending;
  CacheListenerPtr m_listener;
  CacheWriterPtr m_writer;
  CacheLoaderPtr m_loader;
  volatile bool m_released;
  EntriesMap* m_entries;  // map containing cache entries...
  RegionStats* m_regionStats;
  CacheStatisticsPtr m_cacheStatistics;
  bool m_transactionEnabled;
  TombstoneListPtr m_tombstoneList;
  bool m_isPRSingleHopEnabled;
  PoolPtr m_attachedPool;
  bool m_enableTimeStatistics;

  mutable ACE_RW_Thread_Mutex m_rwLock;
  void keys_internal(VectorOfCacheableKey& v);
  bool containsKey_internal(const CacheableKeyPtr& keyPtr) const;
  int removeRegion(const std::string& name);

  bool invokeCacheWriterForEntryEvent(const CacheableKeyPtr& key,
                                      CacheablePtr& oldValue,
                                      const CacheablePtr& newValue,
                                      const SerializablePtr& aCallbackArgument,
                                      CacheEventFlags eventFlags,
                                      EntryEventType type);
  bool invokeCacheWriterForRegionEvent(const SerializablePtr& aCallbackArgument,
                                       CacheEventFlags eventFlags,
                                       RegionEventType type);
  GfErrType invokeCacheListenerForEntryEvent(
      const CacheableKeyPtr& key, CacheablePtr& oldValue,
      const CacheablePtr& newValue, const SerializablePtr& aCallbackArgument,
      CacheEventFlags eventFlags, EntryEventType type, bool isLocal = false);
  GfErrType invokeCacheListenerForRegionEvent(
      const SerializablePtr& aCallbackArgument, CacheEventFlags eventFlags,
      RegionEventType type);
  // functions related to expirations.
  void updateAccessAndModifiedTimeForEntry(MapEntryImplPtr& ptr, bool modified);
  void registerEntryExpiryTask(MapEntryImplPtr& entry);
  void subregions_internal(const bool recursive, VectorOfRegion& sr);
  void entries_internal(VectorOfRegionEntry& me, const bool recursive);

  PersistenceManagerPtr m_persistenceManager;

  bool isStatisticsEnabled();
  bool useModifiedTimeForRegionExpiry();
  bool useModifiedTimeForEntryExpiry();
  bool isEntryIdletimeEnabled();
  ExpirationAction::Action getEntryExpirationAction() const;
  ExpirationAction::Action getRegionExpiryAction() const;
  uint32_t getRegionExpiryDuration() const;
  uint32_t getEntryExpiryDuration() const;
  void invokeAfterAllEndPointDisconnected();
  // Disallow copy constructor and assignment operator.
  LocalRegion(const LocalRegion&);
  LocalRegion& operator=(const LocalRegion&);

  virtual GfErrType getNoThrow_FullObject(EventIdPtr eventId,
                                          CacheablePtr& fullObject,
                                          VersionTagPtr& versionTag);

  // these classes encapsulate actions specific to update operations
  // used by the template <code>updateNoThrow</code> class
  friend class PutActions;
  friend class PutActionsTx;
  friend class CreateActions;
  friend class DestroyActions;
  friend class RemoveActions;
  friend class InvalidateActions;
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_LOCALREGION_H_
