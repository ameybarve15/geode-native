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

#pragma once

#ifndef GEODE_INTERNAL_UTILS_H_
#define GEODE_INTERNAL_UTILS_H_

/**
 * @file
 */

#include <typeinfo>
#include <string>
#include <unordered_set>
#include <memory>

#include <geode/geode_globals.hpp>
#include <geode/geode_base.hpp>
#include <geode/ExceptionTypes.hpp>
#include <geode/CacheableString.hpp>
#include <geode/DataOutput.hpp>
#include <geode/statistics/Statistics.hpp>
#include <geode/SystemProperties.hpp>
#include <geode/DistributedSystem.hpp>
#include <typeinfo>
#include <string>
#include <unordered_set>
#include <memory>
#include <chrono>

#ifdef __GNUC__
extern "C" {
#include <cxxabi.h>
}
#endif

namespace apache {
namespace geode {
namespace client {
class CPPCACHE_EXPORT Utils {
  /**
   * utilities
   *
   */
 public:
  /**
   * Get the value of an environment variable.
   * On windows the maximum length of value supported is 8191.
   */
  static std::string getEnv(const char* varName);
  static int32_t getLastError();

#ifdef __GNUC__
  inline static char* _gnuDemangledName(const char* typeIdName, size_t& len) {
    int status;
    char* demangledName =
        abi::__cxa_demangle(typeIdName, nullptr, &len, &status);
    if (status == 0 && demangledName != nullptr) {
      return demangledName;
    }
    return nullptr;
  }
#endif

  inline static void demangleTypeName(const char* typeIdName,
                                      std::string& str) {
#ifdef __GNUC__
    size_t len;
    char* demangledName = _gnuDemangledName(typeIdName, len);
    if (demangledName != nullptr) {
      str.append(demangledName, len);
      free(demangledName);
      return;
    }
#endif
    str.append(typeIdName);
  }

  inline static CacheableStringPtr demangleTypeName(const char* typeIdName) {
#ifdef __GNUC__
    size_t len;
    char* demangledName = _gnuDemangledName(typeIdName, len);
    if (demangledName != nullptr) {
      return CacheableString::createNoCopy(demangledName, len);
    }
#endif
    return CacheableString::create(typeIdName);
  }

  static int logWideString(char* buf, size_t maxLen, const wchar_t* wStr);

  /**
   * The only operations that is well defined on the result is "asChar".
   */
  inline static CacheableStringPtr getCacheableKeyString(
      const CacheableKeyPtr& key) {
    CacheableStringPtr result;
    if (key != nullptr) {
      char* buf;
      GF_NEW(buf, char[_GF_MSG_LIMIT + 1]);
      key->logString(buf, _GF_MSG_LIMIT);
      // the length given here is not correct but we want to save
      // the cost of a "strlen" and the value here does not matter
      // assuming the caller will use the result only for logging by
      // invoking "->asChar()"
      result = CacheableString::createNoCopy(buf, _GF_MSG_LIMIT);
    } else {
      result = CacheableString::create("(null)");
    }
    return result;
  }

  static CacheableStringPtr getCacheableString(const CacheablePtr& val) {
    if (val != nullptr) {
      if (const auto key = std::dynamic_pointer_cast<CacheableKey>(val)) {
        return getCacheableKeyString(key);
      } else {
        const CacheableStringPtr& cStr = val->toString();
        if (cStr != nullptr) {
          if (cStr->isCString()) {
            return cStr;
          } else {
            char buf[_GF_MSG_LIMIT + 1];
            (void)logWideString(buf, _GF_MSG_LIMIT, cStr->asWChar());
            return CacheableString::create(buf);
          }
        }
      }
    }

    return CacheableString::create("(null)");
  }

  static int64_t startStatOpTime();

  // Check objectSize() implementation return value and log a warning at most
  // once.
  inline static uint32_t checkAndGetObjectSize(const CacheablePtr& theObject) {
    uint32_t objectSize = theObject->objectSize();
    static bool youHaveBeenWarned = false;
    if ((objectSize == 0 || objectSize == (static_cast<uint32_t>(-1))) &&
        !youHaveBeenWarned) {
      LOGWARN(
          "Object size for Heap LRU returned by class ID %d is 0 (zero) or -1 "
          "(UINT32_MAX). "
          "Even for empty objects the size returned should be at least one (1 "
          "byte) and "
          "should not be -1 or UINT32_MAX.",
          theObject->classId());
      youHaveBeenWarned = true;
      LOGFINE("Type ID is %d for the object returning zero HeapLRU size",
              theObject->typeId());
    }
    GF_DEV_ASSERT(objectSize != 0 && objectSize != ((uint32_t)-1));
    return objectSize;
  }

  static void updateStatOpTime(statistics::Statistics* m_regionStats,
                               int32_t statId, int64_t start);

  static void parseEndpointNamesString(
      const char* endpoints, std::unordered_set<std::string>& endpointNames);
  static void parseEndpointString(const char* endpoints, std::string& host,
                                  uint16_t& port);

  static std::string convertHostToCanonicalForm(const char* endpoints);

  static char* copyString(const char* str);

  /**
   * Convert the byte array to a string as "%d %d ...".
   * <code>maxLength</code> as zero implies no limit.
   */
  static CacheableStringPtr convertBytesToString(
      const uint8_t* bytes, int32_t length, size_t maxLength = _GF_MSG_LIMIT);

  /**
   * Convert the byte array to a string as "%d %d ...".
   * <code>maxLength</code> as zero implies no limit.
   */
  inline static CacheableStringPtr convertBytesToString(
      const char* bytes, int32_t length, size_t maxLength = _GF_MSG_LIMIT) {
    return convertBytesToString(reinterpret_cast<const uint8_t*>(bytes),
                                length);
  }
};

// Generate random numbers 0 to max-1
class RandGen {
 public:
  int operator()(size_t max);
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_INTERNAL_UTILS_H_
