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

#ifndef GEODE_CACHEABLEBUILTINS_H_
#define GEODE_CACHEABLEBUILTINS_H_

/** @file CacheableBuiltins.hpp
 *  @brief Contains generic template definitions for Cacheable types
 *         and instantiations for built-in types.
 */

#include <cstring>

#include "Cacheable.hpp"
#include "CacheableKey.hpp"
#include "Serializer.hpp"
#include "CacheableKeys.hpp"
#include "CacheableString.hpp"

namespace apache {
namespace geode {
namespace client {

template <typename Target, int8_t TYPEID>
class CacheableArrayType;

/** sprintf implementation. */
extern int gf_sprintf(char* buffer, const char* fmt, ...);

/** snprintf implementation. */
extern int gf_snprintf(char* buffer, int32_t maxLength, const char* fmt, ...);

/** Template CacheableKey class for primitive types. */
template <typename TObj, int8_t TYPEID, const char* TYPENAME,
          const char* SPRINTFSYM, int32_t STRSIZE>
class CacheableKeyType : public CacheableKey {
 protected:
  TObj m_value;

  inline CacheableKeyType()
      : m_value(apache::geode::client::serializer::zeroObject<TObj>()) {}

  inline CacheableKeyType(const TObj value) : m_value(value) {}

 public:
  /** Gets the contained value. */
  inline TObj value() const { return m_value; }

  // Cacheable methods

  /** Serialize this object to given <code>DataOutput</code>. */
  virtual void toData(DataOutput& output) const {
    apache::geode::client::serializer::writeObject(output, m_value);
  }

  /** Deserialize this object from given <code>DataInput</code>. */
  virtual void fromData(DataInput& input) {
    apache::geode::client::serializer::readObject(input, m_value);
  }

  /**
   * Return the classId of the instance being serialized.
   *
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int32_t classId() const { return 0; }

  /**
   * Return the typeId byte of the instance being serialized.
   *
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int8_t typeId() const { return TYPEID; }

  /** Return a string representation of the object. */
  virtual CacheableStringPtr toString() const {
    char buffer[STRSIZE + 1];
    gf_sprintf(buffer, SPRINTFSYM, m_value);
    return CacheableString::create(buffer);
  }

  // CacheableKey methods

  /** Return the hashcode for this key. */
  virtual int32_t hashcode() const {
    return apache::geode::client::serializer::hashcode(m_value);
  }

  /** Return true if this key matches other. */
  virtual bool operator==(const CacheableKey& other) const {
    if (other.typeId() != TYPEID) {
      return false;
    }
    const CacheableKeyType& otherValue =
        static_cast<const CacheableKeyType&>(other);
    return apache::geode::client::serializer::equals(m_value,
                                                     otherValue.m_value);
  }

  /** Return true if this key matches other key value. */
  inline bool operator==(const TObj other) const {
    return apache::geode::client::serializer::equals(m_value, other);
  }

  /**
   * Copy the string form of the object into a char* buffer for
   * logging purposes.
   */
  virtual int32_t logString(char* buffer, int32_t maxLength) const {
    char fmt[64];
    gf_sprintf(fmt, "%s( %s )", TYPENAME, SPRINTFSYM);
    return gf_snprintf(buffer, maxLength, fmt, m_value);
  }

  /**
   * Return the size in bytes of the instance being serialized.
   *
   * This is used to determine whether the cache is using up more
   * physical memory than it has been configured to use. The method can
   * return zero if the user does not require the ability to control
   * cache memory utilization.
   */
  virtual uint32_t objectSize() const { return sizeof(CacheableKeyType); }
};

/** Function to copy an array from source to destination. */
template <typename TObj>
inline void copyArray(TObj* dest, const TObj* src, int32_t length) {
  std::memcpy(dest, src, length * sizeof(TObj));
}

/**
 * Function to copy an array of <code>std::shared_ptr</code>s from
 * source to destination.
 */
template <typename TObj>
inline void copyArray(std::shared_ptr<TObj>* dest,
                      const std::shared_ptr<TObj>* src, int32_t length) {
  for (int32_t index = 0; index < length; index++) {
    dest[index] = src[index];
  }
}

/**
 * Function to copy an array of
 * <code>std::shared_ptr<CacheableArrayType<>></code>s from source to
 * destination.
 */
template <typename TObj, int8_t TYPEID>
inline void copyArray(
    std::shared_ptr<CacheableArrayType<TObj, TYPEID>>* dest,
    const std::shared_ptr<CacheableArrayType<TObj, TYPEID>>* src,
    int32_t length) {
  for (int32_t index = 0; index < length; index++) {
    dest[index] = src[index];
  }
}

/** Template class for array of primitive types. */
template <typename TObj, int8_t TYPEID>
class CacheableArrayType : public Cacheable {
 protected:
  TObj* m_value;
  int32_t m_length;

  inline CacheableArrayType() : m_value(nullptr), m_length(0) {}

  inline CacheableArrayType(int32_t length) : m_length(length) {
    if (length > 0) {
      GF_NEW(m_value, TObj[length]);
    }
  }

  inline CacheableArrayType(TObj* value, int32_t length)
      : m_value(value), m_length(length) {}

  inline CacheableArrayType(const TObj* value, int32_t length, bool copy)
      : m_value(nullptr), m_length(length) {
    if (length > 0) {
      GF_NEW(m_value, TObj[length]);
      copyArray(m_value, value, length);
    }
  }

  virtual ~CacheableArrayType() { GF_SAFE_DELETE_ARRAY(m_value); }

  FRIEND_STD_SHARED_PTR(CacheableArrayType)

 private:
  // Private to disable copy constructor and assignment operator.
  CacheableArrayType(const CacheableArrayType& other)
      : m_value(other.m_value), m_length(other.m_length) {}

  CacheableArrayType& operator=(const CacheableArrayType& other) {
    return *this;
  }

 public:
  /** Get the underlying array. */
  inline const TObj* value() const { return m_value; }

  /** Get the length of the array. */
  inline int32_t length() const { return m_length; }

  /** Get the element at given index. */
  inline TObj operator[](uint32_t index) const {
    if (static_cast<int32_t>(index) >= m_length) {
      throw OutOfRangeException(
          "CacheableArray::operator[]: Index out of range.");
    }
    return m_value[index];
  }

  // Cacheable methods

  /** Serialize this object to the given <code>DataOutput</code>. */
  virtual void toData(DataOutput& output) const {
    apache::geode::client::serializer::writeObject(output, m_value, m_length);
  }

  /** Deserialize this object from the given <code>DataInput</code>. */
  virtual void fromData(DataInput& input) {
    GF_SAFE_DELETE_ARRAY(m_value);
    apache::geode::client::serializer::readObject(input, m_value, m_length);
  }

  /**
   * Return the classId of the instance being serialized.
   *
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int32_t classId() const { return 0; }

  /**
   * Return the typeId byte of the instance being serialized.
   *
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int8_t typeId() const { return TYPEID; }

  /**
   * Return the size in bytes of the instance being serialized.
   *
   * This is used to determine whether the cache is using up more
   * physical memory than it has been configured to use. The method can
   * return zero if the user does not require the ability to control
   * cache memory utilization.
   */
  virtual uint32_t objectSize() const {
    return static_cast<uint32_t>(
        sizeof(CacheableArrayType) +
        apache::geode::client::serializer::objectSize(m_value, m_length));
  }
};

/** Template class for container Cacheable types. */
template <typename TBase, int8_t TYPEID>
class CacheableContainerType : public Cacheable, public TBase {
 protected:
  inline CacheableContainerType() : TBase() {}

  inline CacheableContainerType(const int32_t n) : TBase(n) {}

 public:
  // Cacheable methods

  /** Serialize this object to the given <code>DataOutput</code>. */
  virtual void toData(DataOutput& output) const {
    apache::geode::client::serializer::writeObject(output, *this);
  }

  /** Deserialize this object from the given <code>DataInput</code>. */
  virtual void fromData(DataInput& input) {
    apache::geode::client::serializer::readObject(input, *this);
  }

  /**
   * Return the classId of the instance being serialized.
   *
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int32_t classId() const { return 0; }

  /**
   * Return the typeId byte of the instance being serialized.
   *
   * This is used by deserialization to determine what instance
   * type to create and deserialize into.
   */
  virtual int8_t typeId() const { return TYPEID; }

  /**
   * Return the size in bytes of the instance being serialized.
   *
   * This is used to determine whether the cache is using up more
   * physical memory than it has been configured to use. The method can
   * return zero if the user does not require the ability to control
   * cache memory utilization.
   */
  virtual uint32_t objectSize() const {
    return static_cast<uint32_t>(
        sizeof(CacheableContainerType) +
        apache::geode::client::serializer::objectSize(*this));
  }
};

#ifdef _SOLARIS
#define TEMPLATE_EXPORT template class
#else
#ifdef BUILD_CPPCACHE
#define TEMPLATE_EXPORT template class CPPCACHE_EXPORT
#else
#define TEMPLATE_EXPORT extern template class CPPCACHE_EXPORT
#endif
#endif

// Disable extern template warning on MSVC compiler
#ifdef _MSC_VER
#pragma warning(disable : 4231)
#endif

#define _GF_CACHEABLE_KEY_TYPE_DEF_(p, k, sz)                                 \
  extern const char tName_##k[];                                              \
  extern const char tStr_##k[];                                               \
  TEMPLATE_EXPORT                                                             \
  CacheableKeyType<p, GeodeTypeIds::k, tName_##k, tStr_##k, sz>;              \
  typedef CacheableKeyType<p, GeodeTypeIds::k, tName_##k, tStr_##k, sz> _##k; \
  class CPPCACHE_EXPORT k;                                                    \
  typedef std::shared_ptr<k> k##Ptr;

// use a class instead of typedef for bug #283
#define _GF_CACHEABLE_KEY_TYPE_(p, k, sz)                                      \
  class CPPCACHE_EXPORT k : public _##k {                                      \
   protected:                                                                  \
    inline k() : _##k() {}                                                     \
    inline k(const p value) : _##k(value) {}                                   \
    FRIEND_STD_SHARED_PTR(k)                                                   \
                                                                               \
   public:                                                                     \
    /** Factory function registered with serialization registry. */            \
    static Serializable* createDeserializable() { return new k(); }            \
    /** Factory function to create a new default instance. */                  \
    inline static k##Ptr create() { return std::make_shared<k>(); }            \
    /** Factory function to create an instance with the given value. */        \
    inline static k##Ptr create(const p value) {                               \
      return std::make_shared<k>(value);                                       \
    }                                                                          \
  };                                                                           \
  inline CacheableKeyPtr createKey(const p value) { return k::create(value); } \
  inline CacheablePtr createValue(const p value) { return k::create(value); }

#define _GF_CACHEABLE_ARRAY_TYPE_DEF_(p, c)               \
  TEMPLATE_EXPORT CacheableArrayType<p, GeodeTypeIds::c>; \
  typedef CacheableArrayType<p, GeodeTypeIds::c> _##c;    \
  class CPPCACHE_EXPORT c;                                \
  typedef std::shared_ptr<CacheableArrayType<p, GeodeTypeIds::c>> c##Ptr;

// use a class instead of typedef for bug #283
#define _GF_CACHEABLE_ARRAY_TYPE_(p, c)                                       \
  class CPPCACHE_EXPORT c : public _##c {                                     \
   protected:                                                                 \
    inline c() : _##c() {}                                                    \
    inline c(int32_t length) : _##c(length) {}                                \
    inline c(p* value, int32_t length) : _##c(value, length) {}               \
    inline c(const p* value, int32_t length, bool copy)                       \
        : _##c(value, length, true) {}                                        \
                                                                              \
   private:                                                                   \
    /* Private to disable copy constructor and assignment operator. */        \
    c(const c& other);                                                        \
    c& operator=(const c& other);                                             \
                                                                              \
    FRIEND_STD_SHARED_PTR(c)                                                  \
                                                                              \
   public:                                                                    \
    /** Factory function registered with serialization registry. */           \
    static Serializable* createDeserializable() { return new c(); }           \
    /** Factory function to create a new default instance. */                 \
    inline static c##Ptr create() { return std::make_shared<c>(); }           \
    /** Factory function to create a cacheable array of given size. */        \
    inline static c##Ptr create(int32_t length) {                             \
      return std::make_shared<c>(length);                                     \
    }                                                                         \
    /** Create a cacheable array copying from the given array. */             \
    inline static c##Ptr create(const p* value, int32_t length) {             \
      return nullptr == value ? nullptr                                       \
                              : std::make_shared<c>(value, length, true);     \
    }                                                                         \
    /**                                                                       \
     * Create a cacheable array taking ownership of the given array           \
     * without creating a copy.                                               \
     *                                                                        \
     * Note that the application has to ensure that the given array is        \
     * not deleted (apart from this class) and is allocated on the heap       \
     * using the "new" operator.                                              \
     */                                                                       \
    inline static c##Ptr createNoCopy(p* value, int32_t length) {             \
      return nullptr == value ? nullptr : std::make_shared<c>(value, length); \
    }                                                                         \
  };

#define _GF_CACHEABLE_CONTAINER_TYPE_DEF_(p, c)               \
  TEMPLATE_EXPORT CacheableContainerType<p, GeodeTypeIds::c>; \
  typedef CacheableContainerType<p, GeodeTypeIds::c> _##c;    \
  class CPPCACHE_EXPORT c;                                    \
  typedef std::shared_ptr<c> c##Ptr;

// use a class instead of typedef for bug #283
#define _GF_CACHEABLE_CONTAINER_TYPE_(p, c)                            \
  class CPPCACHE_EXPORT c : public _##c {                              \
   protected:                                                          \
    inline c() : _##c() {}                                             \
    inline c(const int32_t n) : _##c(n) {}                             \
                                                                       \
    FRIEND_STD_SHARED_PTR(c)                                           \
                                                                       \
   public:                                                             \
    /** Factory function registered with serialization registry. */    \
    static Serializable* createDeserializable() { return new c(); }    \
    /** Factory function to create a default instance. */              \
    inline static c##Ptr create() { return std::make_shared<c>(); }    \
    /** Factory function to create an instance with the given size. */ \
    inline static c##Ptr create(const int32_t n) {                     \
      return std::make_shared<c>(n);                                   \
    }                                                                  \
  };

// Instantiations for the built-in CacheableKeys

_GF_CACHEABLE_KEY_TYPE_DEF_(bool, CacheableBoolean, 3);
/**
 * An immutable wrapper for booleans that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(bool, CacheableBoolean, 3);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(bool, BooleanArray);
/**
 * An immutable wrapper for array of booleans that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(bool, BooleanArray);

_GF_CACHEABLE_KEY_TYPE_DEF_(uint8_t, CacheableByte, 15);
/**
 * An immutable wrapper for bytes that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(uint8_t, CacheableByte, 15);

_GF_CACHEABLE_KEY_TYPE_DEF_(double, CacheableDouble, 63);
/**
 * An immutable wrapper for doubles that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(double, CacheableDouble, 63);

_GF_CACHEABLE_KEY_TYPE_DEF_(float, CacheableFloat, 63);
/**
 * An immutable wrapper for floats that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(float, CacheableFloat, 63);

_GF_CACHEABLE_KEY_TYPE_DEF_(int16_t, CacheableInt16, 15);
/**
 * An immutable wrapper for 16-bit integers that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(int16_t, CacheableInt16, 15);

_GF_CACHEABLE_KEY_TYPE_DEF_(int32_t, CacheableInt32, 15);
/**
 * An immutable wrapper for 32-bit integers that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(int32_t, CacheableInt32, 15);

_GF_CACHEABLE_KEY_TYPE_DEF_(int64_t, CacheableInt64, 31);
/**
 * An immutable wrapper for 64-bit integers that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(int64_t, CacheableInt64, 31);

_GF_CACHEABLE_KEY_TYPE_DEF_(wchar_t, CacheableWideChar, 3);
/**
 * An immutable wrapper for wide-characters that can serve as
 * a distributable key object for caching.
 */
_GF_CACHEABLE_KEY_TYPE_(wchar_t, CacheableWideChar, 3);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(wchar_t, CharArray);
/**
 * An immutable wrapper for array of wide-characters that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(wchar_t, CharArray);

// Instantiations for array built-in Cacheables

_GF_CACHEABLE_ARRAY_TYPE_DEF_(uint8_t, CacheableBytes);
/**
 * An immutable wrapper for byte arrays that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(uint8_t, CacheableBytes);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(double, CacheableDoubleArray);
/**
 * An immutable wrapper for array of doubles that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(double, CacheableDoubleArray);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(float, CacheableFloatArray);
/**
 * An immutable wrapper for array of floats that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(float, CacheableFloatArray);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(int16_t, CacheableInt16Array);
/**
 * An immutable wrapper for array of 16-bit integers that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(int16_t, CacheableInt16Array);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(int32_t, CacheableInt32Array);
/**
 * An immutable wrapper for array of 32-bit integers that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(int32_t, CacheableInt32Array);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(int64_t, CacheableInt64Array);
/**
 * An immutable wrapper for array of 64-bit integers that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(int64_t, CacheableInt64Array);

_GF_CACHEABLE_ARRAY_TYPE_DEF_(CacheableStringPtr, CacheableStringArray);
/**
 * An immutable wrapper for array of strings that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_ARRAY_TYPE_(CacheableStringPtr, CacheableStringArray);

// Instantiations for container types (Vector/HashMap/HashSet) Cacheables

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(VectorOfCacheable, CacheableVector);
/**
 * A mutable <code>Cacheable</code> vector wrapper that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(VectorOfCacheable, CacheableVector);

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(HashMapOfCacheable, CacheableHashMap);
/**
 * A mutable <code>CacheableKey</code> to <code>Serializable</code>
 * hash map that can serve as a distributable object for caching.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(HashMapOfCacheable, CacheableHashMap);

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(HashSetOfCacheableKey, CacheableHashSet);
/**
 * A mutable <code>CacheableKey</code> hash set wrapper that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(HashSetOfCacheableKey, CacheableHashSet);

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(VectorOfCacheable, CacheableArrayList);
/**
 * A mutable <code>Cacheable</code> array list wrapper that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(VectorOfCacheable, CacheableArrayList);

// linketlist for JSON formattor issue
_GF_CACHEABLE_CONTAINER_TYPE_DEF_(VectorOfCacheable, CacheableLinkedList);
/**
 * A mutable <code>Cacheable</code> array list wrapper that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(VectorOfCacheable, CacheableLinkedList);

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(VectorOfCacheable, CacheableStack);
/**
 * A mutable <code>Cacheable</code> stack wrapper that can serve as
 * a distributable object for caching.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(VectorOfCacheable, CacheableStack);

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(HashMapOfCacheable, CacheableHashTable);
/**
 * A mutable <code>CacheableKey</code> to <code>Serializable</code>
 * hash map that can serve as a distributable object for caching.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(HashMapOfCacheable, CacheableHashTable);

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(HashMapOfCacheable, CacheableIdentityHashMap);
/**
 * A mutable <code>CacheableKey</code> to <code>Serializable</code>
 * hash map that can serve as a distributable object for caching. This is
 * provided for compability with java side, though is functionally identical
 * to <code>CacheableHashMap</code> i.e. does not provide the semantics of
 * java <code>IdentityHashMap</code>.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(HashMapOfCacheable, CacheableIdentityHashMap);

_GF_CACHEABLE_CONTAINER_TYPE_DEF_(HashSetOfCacheableKey,
                                  CacheableLinkedHashSet);
/**
 * A mutable <code>CacheableKey</code> hash set wrapper that can serve as
 * a distributable object for caching. This is provided for compability
 * with java side, though is functionally identical to
 * <code>CacheableHashSet</code> i.e. does not provide the predictable
 * iteration semantics of java <code>LinkedHashSet</code>.
 */
_GF_CACHEABLE_CONTAINER_TYPE_(HashSetOfCacheableKey, CacheableLinkedHashSet);

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_CACHEABLEBUILTINS_H_
