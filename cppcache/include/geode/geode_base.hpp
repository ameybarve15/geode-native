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

#ifndef GEODE_BASE_H_
#define GEODE_BASE_H_

#if defined(_WIN32)
/** Library Export */
#define LIBEXP __declspec(dllexport)
/** Library Implementation */
#define LIBIMP __declspec(dllimport)
/** Library Call */
#define LIBCALL __stdcall
/** Library Export a type */
#define LIBEXPORT(type) LIBEXP type LIBCALL
#else
/** Library Export */
#define LIBEXP
/** Library Implementation */
#define LIBIMP extern
/** Library Call */
#define LIBCALL /**/
/** Library Export a type */
#define LIBEXPORT(type) type
#endif

/** Defines a Geode C extern */
#ifdef BUILD_GEMFIRE
#define GFCEXTERN(type) LIBEXP type LIBCALL
#else
#define GFCEXTERN(type) LIBIMP type LIBCALL
#endif /* BUILD_GEMFIRE    */

/** Defines a Geode CPPCACHE export */
#if defined(_WIN32)
#ifdef BUILD_CPPCACHE
#define CPPCACHE_EXPORT LIBEXP
#else
#define CPPCACHE_EXPORT LIBIMP
#endif
#else
#define CPPCACHE_EXPORT
#endif /* BUILD_CPPCACHE */

/** Define GF_TEMPLATE_EXPORT */
#if defined(_WIN32)
#ifdef BUILD_CPPCACHE
#define GF_TEMPLATE_EXPORT __declspec(dllexport)
#else
#define GF_TEMPLATE_EXPORT __declspec(dllimport)
#endif
#else
#define GF_TEMPLATE_EXPORT
#endif

#if defined(_MSC_VER)
/* Windows does not have inttypes.h */
/* 32 bit Windows only, for now */
#if !defined PRId8
#define PRId8 "d"
#endif
#if !defined PRIi8
#define PRIi8 "i"
#endif
#if !defined PRIo8
#define PRIo8 "o"
#endif
#if !defined PRIu8
#define PRIu8 "u"
#endif
#if !defined PRIx8
#define PRIx8 "x"
#endif
#if !defined PRIX8
#define PRIX8 "X"
#endif
#if !defined PRId16
#define PRId16 "d"
#endif
#if !defined PRIi16
#define PRIi16 "i"
#endif
#if !defined PRIo16
#define PRIo16 "o"
#endif
#if !defined PRIu16
#define PRIu16 "u"
#endif
#if !defined PRIx16
#define PRIx16 "x"
#endif
#if !defined PRIX16
#define PRIX16 "X"
#endif
#if !defined PRId32
#define PRId32 "d"
#endif
#if !defined PRIi32
#define PRIi32 "i"
#endif
#if !defined PRIo32
#define PRIo32 "o"
#endif
#if !defined PRIu32
#define PRIu32 "u"
#endif
#if !defined PRIx32
#define PRIx32 "x"
#endif
#if !defined PRIX32
#define PRIX32 "X"
#endif
#if !defined PRId64
#define PRId64 "lld"
#endif
#if !defined PRIi64
#define PRIi64 "lli"
#endif
#if !defined PRIo64
#define PRIo64 "llo"
#endif
#if !defined PRIu64
#define PRIu64 "llu"
#endif
#if !defined PRIx64
#define PRIx64 "llx"
#endif
#if !defined PRIX64
#define PRIX64 "llX"
#endif
/* end inttypes.h */

#else
/* Unix, including both Sparc Solaris and Linux */
#define __STDC_FORMAT_MACROS

#endif /* _WIN32*/

/**@namespace geode This namespace contains all the Geode
 * C++ API classes, enumerations and globals.
 */

/**@namespace geode_statistics This namespace contains all the Geode
 * C++ statistics API classes.
 */

/**@namespace apache::geode::client::TypeHelper This namespace contains type
 * traits helper
 * structs/classes to determine type information at compile time
 * using typename. Useful for templates in particular.
 */

/**
 * @file
 *
 *  Definitions of types and functions supported in the Geode C++ interface
 */

/** default timeout for query response */
#define DEFAULT_QUERY_RESPONSE_TIMEOUT 15

/**
 * @enum GfErrType
 *Error codes returned by Geode C++ interface functions
 */
typedef enum {
  GF_NOERR = 0,           /**< success - no error               */
  GF_DEADLK = 1,          /**< deadlock detected                */
  GF_EACCES = 2,          /**< permission problem               */
  GF_ECONFL = 3,          /**< class creation conflict          */
  GF_EINVAL = 4,          /**< invalid argument                 */
  GF_ENOENT = 5,          /**< entity does not exist            */
  GF_ENOMEM = 6,          /**< insufficient memory              */
  GF_ERANGE = 7,          /**< index out of range               */
  GF_ETYPE = 8,           /**< type mismatch                    */
  GF_NOTOBJ = 9,          /**< invalid object reference         */
  GF_NOTCON = 10,         /**< not connected to Geode         */
  GF_NOTOWN = 11,         /**< lock not owned by process/thread */
  GF_NOTSUP = 12,         /**< operation not supported          */
  GF_SCPGBL = 13,         /**< attempt to exit global scope     */
  GF_SCPEXC = 14,         /**< maximum scopes exceeded          */
  GF_TIMOUT = 15,         /**< operation timed out              */
  GF_OVRFLW = 16,         /**< arithmetic overflow              */
  GF_IOERR = 17,          /**< paging file I/O error            */
  GF_EINTR = 18,          /**< interrupted Geode call         */
  GF_MSG = 19,            /**< message could not be handled     */
  GF_DISKFULL = 20,       /**< disk full                        */
  GF_NOSERVER_FOUND = 21, /** NoServer found */
  GF_SERVER_FAILED = 22,

  GF_CLIENT_WAIT_TIMEOUT = 23,
  GF_CLIENT_WAIT_TIMEOUT_REFRESH_PRMETADATA = 24,

  GF_CACHE_REGION_NOT_FOUND = 101, /**< No region with the specified name. */
  GF_CACHE_REGION_INVALID = 102,   /**< the region is not valid */
  GF_CACHE_REGION_KEYS_NOT_STRINGS = 103, /**< Entry keys are not strings */
  GF_CACHE_REGION_ENTRY_NOT_BYTES =
      104,                          /**< Entry's value is not a byte array */
  GF_CACHE_REGION_NOT_GLOBAL = 105, /**< Distributed locks not supported */
  GF_CACHE_PROXY = 106, /**< Errors detected in CacheProxy processing */
  GF_CACHE_ILLEGAL_ARGUMENT_EXCEPTION =
      107, /**< IllegalArgumentException in Cache Proxy */
  GF_CACHE_ILLEGAL_STATE_EXCEPTION =
      108,                          /**< IllegalStateException in CacheProxy */
  GF_CACHE_TIMEOUT_EXCEPTION = 109, /**< TimeoutException in CacheProxy */
  GF_CACHE_WRITER_EXCEPTION = 110,  /**< CacheWriterException in CacheProxy */
  GF_CACHE_REGION_EXISTS_EXCEPTION =
      111,                         /**< RegionExistsException in CacheProxy */
  GF_CACHE_CLOSED_EXCEPTION = 112, /**< CacheClosedException in CacheProxy */
  GF_CACHE_LEASE_EXPIRED_EXCEPTION =
      113,                         /**< LeaseExpiredException in CacheProxy */
  GF_CACHE_LOADER_EXCEPTION = 114, /**< CacheLoaderException in CacheProxy */
  GF_CACHE_REGION_DESTROYED_EXCEPTION =
      115, /**< RegionDestroyedException in CacheProxy */
  GF_CACHE_ENTRY_DESTROYED_EXCEPTION =
      116, /**< EntryDestroyedException in CacheProxy */
  GF_CACHE_STATISTICS_DISABLED_EXCEPTION =
      117, /**< StatisticsDisabledException in CacheProxy */
  GF_CACHE_CONCURRENT_MODIFICATION_EXCEPTION =
      118, /**< ConcurrentModificationException in CacheProxy */
  GF_CACHE_ENTRY_NOT_FOUND = 119, /**< EntryNotFoundException in CacheProxy */
  GF_CACHE_ENTRY_EXISTS = 120,    /**< EntryExistsException in CacheProxy */
  GF_CACHEWRITER_ERROR =
      121, /**< An Exception occured while invoking a cachewritter callback*/
  GF_CANNOT_PROCESS_GII_REQUEST =
      123, /**< A failure other than timeout occured durring a batch request */
  GF_CACHESERVER_EXCEPTION = 124, /**< Java cache server exception sent to the
                                     thin client by java cache server */
  // GF_CACHE_REDUNDANCY_FAILURE = 125, /**< redundancy level not satisfied */
  GF_AUTHENTICATION_FAILED_EXCEPTION = 126, /**<Authentication Fails */
  GF_NOT_AUTHORIZED_EXCEPTION = 127, /**<Non Authorized Operation Tried */
  GF_AUTHENTICATION_REQUIRED_EXCEPTION = 128, /**No Authentication Provided */
  GF_DUPLICATE_DURABLE_CLIENT =
      129, /**< Java cache server refused duplicate durable client  */
  GF_REMOTE_QUERY_EXCEPTION = 130,   /** Query exception on java cache server */
  GF_CACHE_LISTENER_EXCEPTION = 131, /** Exception in CacheListener */
  GF_ALL_CONNECTIONS_IN_USE_EXCEPTION = 132, /** ALl connections in use*/
  /**
   * local entry was updated while a remote modification operation was
   * in progress
   */
  GF_CACHE_ENTRY_UPDATED = 133,
  GF_CACHE_LOCATOR_EXCEPTION = 134, /** Exception in Locator */
  GF_INVALID_DELTA = 135,
  GF_FUNCTION_EXCEPTION = 136,
  GF_ROLLBACK_EXCEPTION = 137,
  GF_COMMIT_CONFLICT_EXCEPTION = 138,
  GF_TRANSACTION_DATA_NODE_HAS_DEPARTED_EXCEPTION = 139,
  GF_TRANSACTION_DATA_REBALANCED_EXCEPTION = 140,
  GF_PUTALL_PARTIAL_RESULT_EXCEPTION = 141,
  GF_EUNDEF = 999 /**< unknown exception */
} GfErrType;

#include <new>

/** Allocates x and throws OutOfMemoryException if it fails */
#define GF_NEW(v, stmt)                                                 \
  {                                                                     \
    try {                                                               \
      v = new stmt;                                                     \
    } catch (const std::bad_alloc &) {                                  \
      throw apache::geode::client::OutOfMemoryException(                \
          "Out of Memory while executing \"" #v " = new " #stmt ";\""); \
    }                                                                   \
  }

/** Deletes x only if it exists */
#define GF_SAFE_DELETE(x) \
  {                       \
    delete x;             \
    x = nullptr;          \
  }

/** Deletes array x only if it exists */
#define GF_SAFE_DELETE_ARRAY(x) \
  {                             \
    delete[] x;                 \
    x = nullptr;                \
  }

// TODO shared_ptr - Consider making con/destructors public.
/*
 * Allows std::shared_ptr to access protected constructors and destructors.
 */
#if defined(__clang__)
#define FRIEND_STD_SHARED_PTR(_T)                                      \
  friend std::__libcpp_compressed_pair_imp<std::allocator<_T>, _T, 1>; \
  friend std::__shared_ptr_emplace<_T, std::allocator<_T> >;           \
  friend std::default_delete<_T>;
#elif defined(__GNUC__) || defined(__SUNPRO_CC)
#define FRIEND_STD_SHARED_PTR(_T) friend __gnu_cxx::new_allocator<_T>;
#elif defined(_MSC_VER)
#if defined(_MANAGED)
#define FRIEND_STD_SHARED_PTR(_T) \
  friend std::_Ref_count_obj<_T>; \
  friend std::_Ref_count<_T>;     \
  friend std::_Ptr_base<_T>;      \
  friend std::default_delete<_T>; \
  friend std::shared_ptr<_T>;
#else
#define FRIEND_STD_SHARED_PTR(_T) friend std::_Ref_count_obj<_T>;
#endif
#else
#define FRIEND_STD_SHARED_PTR(_T)
#endif

#endif  // GEODE_BASE_H_
