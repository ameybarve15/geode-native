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

#ifndef GEODE_TESTOBJECT_VARIOUSPDXTYPES_H_
#define GEODE_TESTOBJECT_VARIOUSPDXTYPES_H_

#include <geode/PdxSerializable.hpp>
#include <geode/GeodeCppCache.hpp>
#include <geode/PdxWriter.hpp>
#include <geode/PdxReader.hpp>

#ifdef _WIN32
#ifdef BUILD_TESTOBJECT
#define TESTOBJECT_EXPORT LIBEXP
#else
#define TESTOBJECT_EXPORT LIBIMP
#endif
#else
#define TESTOBJECT_EXPORT
#endif

using namespace apache::geode::client;

namespace PdxTests {
/************************************************************
 *  PdxTypes1
 * *********************************************************/

class TESTOBJECT_EXPORT PdxTypes1 : public PdxSerializable {
 private:
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes1();

  virtual ~PdxTypes1();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes1"; }
  int32_t getm_i1() { return m_i1; }
  static PdxSerializable* createDeserializable() { return new PdxTypes1(); }
};
typedef std::shared_ptr<PdxTypes1> PdxTypes1Ptr;

/************************************************************
 *  PdxTypes2
 * *********************************************************/

class TESTOBJECT_EXPORT PdxTypes2 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes2();

  virtual ~PdxTypes2();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes2"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes2(); }
};
typedef std::shared_ptr<PdxTypes2> PdxTypes2Ptr;

/************************************************************
 *  PdxTypes3
 * *********************************************************/

class TESTOBJECT_EXPORT PdxTypes3 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes3();

  virtual ~PdxTypes3();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes3"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes3(); }
};
typedef std::shared_ptr<PdxTypes3> PdxTypes3Ptr;

/************************************************************
 *  PdxTypes4
 * *********************************************************/
class TESTOBJECT_EXPORT PdxTypes4 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes4();

  virtual ~PdxTypes4();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes4"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes4(); }
};
typedef std::shared_ptr<PdxTypes4> PdxTypes4Ptr;

/************************************************************
 *  PdxTypes5
 * *********************************************************/
class TESTOBJECT_EXPORT PdxTypes5 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  char* m_s2;
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes5();

  virtual ~PdxTypes5();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes5"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes5(); }
};
typedef std::shared_ptr<PdxTypes5> PdxTypes5Ptr;

/************************************************************
 *  PdxTypes6
 * *********************************************************/
class TESTOBJECT_EXPORT PdxTypes6 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  char* m_s2;
  int8_t* bytes128;
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes6();

  virtual ~PdxTypes6();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes6"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes6(); }
};
typedef std::shared_ptr<PdxTypes6> PdxTypes6Ptr;

/************************************************************
 *  PdxTypes7
 * *********************************************************/
class TESTOBJECT_EXPORT PdxTypes7 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  char* m_s2;
  int32_t m_i1;
  int8_t* bytes38000;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes7();

  virtual ~PdxTypes7();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes7"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes7(); }
};
typedef std::shared_ptr<PdxTypes7> PdxTypes7Ptr;

/************************************************************
 *  PdxTypes8
 * *********************************************************/

class TESTOBJECT_EXPORT PdxTypes8 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  char* m_s2;
  int32_t m_i1;
  int8_t* bytes300;
  CacheablePtr _enum;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxTypes8();

  virtual ~PdxTypes8();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes8"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes8(); }
};
typedef std::shared_ptr<PdxTypes8> PdxTypes8Ptr;

/************************************************************
 *  PdxTypes9
 * *********************************************************/
class TESTOBJECT_EXPORT PdxTypes9 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  char* m_s2;
  char* m_s3;
  int8_t* m_bytes66000;
  char* m_s4;
  char* m_s5;

 public:
  PdxTypes9();

  virtual ~PdxTypes9();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes9"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes9(); }
};
typedef std::shared_ptr<PdxTypes9> PdxTypes9Ptr;

/************************************************************
 *  PdxTypes10
 * *********************************************************/

class TESTOBJECT_EXPORT PdxTypes10 : public PdxSerializable {
 private:
  char* m_s1;  //"one"
  char* m_s2;
  char* m_s3;
  int8_t* m_bytes66000;
  char* m_s4;
  char* m_s5;

 public:
  PdxTypes10();

  virtual ~PdxTypes10();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests.PdxTypes10"; }

  static PdxSerializable* createDeserializable() { return new PdxTypes10(); }
};
typedef std::shared_ptr<PdxTypes10> PdxTypes10Ptr;

/************************************************************
 *  NestedPdx
 * *********************************************************/

class TESTOBJECT_EXPORT NestedPdx : public PdxSerializable {
 private:
  PdxTypes1Ptr m_pd1;
  PdxTypes2Ptr m_pd2;
  char* m_s1;  //"one"
  char* m_s2;
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  NestedPdx();
  NestedPdx(char* key);

  virtual ~NestedPdx();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests::NestedPdx"; }
  const char* getString() { return m_s1; }

  static PdxSerializable* createDeserializable() { return new NestedPdx(); }
};
typedef std::shared_ptr<NestedPdx> NestedPdxPtr;

/************************************************************
 *  Mixed Version NestedPdx
 * *********************************************************/

class TESTOBJECT_EXPORT MixedVersionNestedPdx : public PdxSerializable {
 private:
  PdxTypes1Ptr m_pd1;
  PdxTypes2Ptr m_pd2;
  char* m_s1;  //"one"
  char* m_s2;
  char* m_s3;
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  MixedVersionNestedPdx();
  MixedVersionNestedPdx(char* key);

  virtual ~MixedVersionNestedPdx();

  int32_t getHashCode();

  bool equals(PdxSerializablePtr obj);

  CacheableStringPtr toString() const;

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void fromData(PdxReaderPtr pr);

  virtual void toData(PdxWriterPtr pw);

  const char* getClassName() const { return "PdxTests::MixedVersionNestedPdx"; }
  const char* getString() { return m_s1; }

  static PdxSerializable* createDeserializable() {
    return new MixedVersionNestedPdx();
  }
};
typedef std::shared_ptr<MixedVersionNestedPdx> MixedVersionNestedPdxPtr;

/************************************************************
 *  PdxInsideIGeodeSerializable
 * *********************************************************/

class TESTOBJECT_EXPORT PdxInsideIGeodeSerializable : public Serializable {
 private:
  NestedPdxPtr m_npdx;
  PdxTypes3Ptr m_pdx3;

  char* m_s1;  //"one"
  char* m_s2;
  int32_t m_i1;
  int32_t m_i2;
  int32_t m_i3;
  int32_t m_i4;

 public:
  PdxInsideIGeodeSerializable();

  virtual ~PdxInsideIGeodeSerializable();

  int32_t getHashCode();

  bool equals(SerializablePtr obj);

  CacheableStringPtr toString() const;

  virtual void fromData(DataInput& input);

  virtual void toData(DataOutput& output) const;

  virtual int32_t classId() const { return 0x10; }

  const char* getClassName() const {
    return "PdxTests::PdxInsideIGeodeSerializable";
  }

  static Serializable* createDeserializable() {
    return new PdxInsideIGeodeSerializable();
  }
};
typedef std::shared_ptr<PdxInsideIGeodeSerializable>
    PdxInsideIGeodeSerializablePtr;

} /* namespace PdxTests */

#endif  // GEODE_TESTOBJECT_VARIOUSPDXTYPES_H_
