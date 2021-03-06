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

#include "begin_native.hpp"
#include <geode/Cache.hpp>
#include <GeodeTypeIdsImpl.hpp>
#include "SerializationRegistry.hpp"
#include "CacheRegionHelper.hpp"
#include "end_native.hpp"

#include <vcclr.h>

#include "DataInput.hpp"
#include "Cache.hpp"
#include "CacheableString.hpp"
#include "CacheableHashMap.hpp"
#include "CacheableStack.hpp"
#include "CacheableVector.hpp"
#include "CacheableArrayList.hpp"
#include "CacheableIDentityHashMap.hpp"
#include "CacheableDate.hpp"
#include "CacheableObjectArray.hpp"
#include "Serializable.hpp"
#include "impl/PdxHelper.hpp"

using namespace System;
using namespace System::IO;
using namespace apache::geode::client;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {
      namespace native = apache::geode::client;

      DataInput::DataInput(System::Byte* buffer, int size, const native::Cache* cache)
      {
        m_ispdxDesrialization = false;
        m_isRootObjectPdx = false;
        m_cache = cache;
        if (buffer != nullptr && size > 0) {
          _GF_MG_EXCEPTION_TRY2

          m_nativeptr = gcnew native_conditional_unique_ptr<native::DataInput>(cache->createDataInput(buffer, size));
          m_cursor = 0;
          m_isManagedObject = false;
          m_forStringDecode = gcnew array<Char>(100);

          try
          {
            m_buffer = const_cast<System::Byte*>(m_nativeptr->get()->currentBufferPosition());
            m_bufferLength = m_nativeptr->get()->getBytesRemaining();
          }
          finally
          {
            GC::KeepAlive(m_nativeptr);
          }

          _GF_MG_EXCEPTION_CATCH_ALL2
        }
        else {
          throw gcnew IllegalArgumentException("DataInput.ctor(): "
                                               "provided buffer is null or empty");
        }
      }

      DataInput::DataInput(array<Byte>^ buffer, const native::Cache * cache)
      {
        m_ispdxDesrialization = false;
        m_isRootObjectPdx = false;
        m_cache =  cache;
        if (buffer != nullptr && buffer->Length > 0) {
          _GF_MG_EXCEPTION_TRY2

            System::Int32 len = buffer->Length;
          GF_NEW(m_buffer, System::Byte[len]);
          pin_ptr<const Byte> pin_buffer = &buffer[0];
          memcpy(m_buffer, (void*)pin_buffer, len);
          m_nativeptr = gcnew native_conditional_unique_ptr<native::DataInput>(m_cache->createDataInput(m_buffer, len));

          m_cursor = 0;
          m_isManagedObject = false;
          m_forStringDecode = gcnew array<Char>(100);

          try
          {
            m_buffer = const_cast<System::Byte*>(m_nativeptr->get()->currentBufferPosition());
            m_bufferLength = m_nativeptr->get()->getBytesRemaining();
          }
          finally
          {
            GC::KeepAlive(m_nativeptr);
          }

          _GF_MG_EXCEPTION_CATCH_ALL2
        }
        else {
          throw gcnew IllegalArgumentException("DataInput.ctor(): "
                                               "provided buffer is null or empty");
        }
      }

      DataInput::DataInput(array<Byte>^ buffer, System::Int32 len, const native::Cache* cache)
      {
        m_ispdxDesrialization = false;
        m_isRootObjectPdx = false;
        m_cache = cache;
        if (buffer != nullptr) {
          if (len == 0 || (System::Int32)len > buffer->Length) {
            throw gcnew IllegalArgumentException(String::Format(
              "DataInput.ctor(): given length {0} is zero or greater than "
              "size of buffer {1}", len, buffer->Length));
          }
          //m_bytes = gcnew array<Byte>(len);
          //System::Array::Copy(buffer, 0, m_bytes, 0, len);
          _GF_MG_EXCEPTION_TRY2

            GF_NEW(m_buffer, System::Byte[len]);
          pin_ptr<const Byte> pin_buffer = &buffer[0];
          memcpy(m_buffer, (void*)pin_buffer, len);
          m_nativeptr = gcnew native_conditional_unique_ptr<native::DataInput>(m_cache->createDataInput(m_buffer, len));

          try
          {
            m_buffer = const_cast<System::Byte*>(m_nativeptr->get()->currentBufferPosition());
            m_bufferLength = m_nativeptr->get()->getBytesRemaining();
          }
          finally
          {
            GC::KeepAlive(m_nativeptr);
          }

          _GF_MG_EXCEPTION_CATCH_ALL2
        }
        else {
          throw gcnew IllegalArgumentException("DataInput.ctor(): "
                                               "provided buffer is null");
        }
      }

      void DataInput::CheckBufferSize(int size)
      {
        if ((unsigned int)(m_cursor + size) > m_bufferLength)
        {
          Log::Debug("DataInput::CheckBufferSize m_cursor:" + m_cursor + " size:" + size + " m_bufferLength:" + m_bufferLength);
          throw gcnew OutOfRangeException("DataInput: attempt to read beyond buffer");
        }
      }

      DataInput^ DataInput::GetClone()
      {
        return gcnew DataInput(m_buffer, m_bufferLength, m_cache);
      }

      Byte DataInput::ReadByte()
      {
        CheckBufferSize(1);
        return m_buffer[m_cursor++];
      }

      SByte DataInput::ReadSByte()
      {
        CheckBufferSize(1);
        return m_buffer[m_cursor++];
      }

      bool DataInput::ReadBoolean()
      {
        CheckBufferSize(1);
        Byte val = m_buffer[m_cursor++];
        if (val == 1)
          return true;
        else
          return false;
      }

      Char DataInput::ReadChar()
      {
        CheckBufferSize(2);
        Char data = m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        return data;
      }

      array<Byte>^ DataInput::ReadBytes()
      {
        System::Int32 length;
        length = ReadArrayLen();

        if (length >= 0) {
          if (length == 0)
            return gcnew array<Byte>(0);
          else {
            array<Byte>^ bytes = ReadBytesOnly(length);
            return bytes;
          }
        }
        return nullptr;
      }

      int DataInput::ReadArrayLen()
      {
        int code;
        int len;

        code = Convert::ToInt32(ReadByte());

        if (code == 0xFF) {
          len = -1;
        }
        else {
          unsigned int result = code;
          if (result > 252) {  // 252 is java's ((byte)-4 && 0xFF)
            if (code == 0xFE) {
              result = ReadUInt16();
            }
            else if (code == 0xFD) {
              result = ReadUInt32();
            }
            else {
              throw gcnew IllegalStateException("unexpected array length code");
            }
            //TODO:: illegal length
          }
          len = (int)result;
        }
        return len;
      }

      array<SByte>^ DataInput::ReadSBytes()
      {
        System::Int32 length;
        length = ReadArrayLen();

        if (length > -1) {
          if (length == 0)
            return gcnew array<SByte>(0);
          else {
            array<SByte>^ bytes = ReadSBytesOnly(length);
            return bytes;
          }
        }
        return nullptr;
      }

      array<Byte>^ DataInput::ReadBytesOnly(System::UInt32 len)
      {
        if (len > 0) {
          CheckBufferSize(len);
          array<Byte>^ bytes = gcnew array<Byte>(len);

          for (unsigned int i = 0; i < len; i++)
            bytes[i] = m_buffer[m_cursor++];

          return bytes;
        }
        return nullptr;
      }

      void DataInput::ReadBytesOnly(array<Byte> ^ buffer, int offset, int count)
      {
        if (count > 0) {
          CheckBufferSize((System::UInt32)count);

          for (int i = 0; i < count; i++)
            buffer[offset + i] = m_buffer[m_cursor++];
        }
      }

      array<SByte>^ DataInput::ReadSBytesOnly(System::UInt32 len)
      {
        if (len > 0) {
          CheckBufferSize(len);
          array<SByte>^ bytes = gcnew array<SByte>(len);

          for (unsigned int i = 0; i < len; i++)
            bytes[i] = (SByte)m_buffer[m_cursor++];

          return bytes;
        }
        return nullptr;
      }

      System::UInt16 DataInput::ReadUInt16()
      {
        CheckBufferSize(2);
        System::UInt16 data = m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        return data;
      }

      System::UInt32 DataInput::ReadUInt32()
      {
        CheckBufferSize(4);
        System::UInt32 data = m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];

        return data;
      }

      System::UInt64 DataInput::ReadUInt64()
      {
        System::UInt64 data;

        CheckBufferSize(8);

        data = m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];
        data = (data << 8) | m_buffer[m_cursor++];

        return data;
      }

      System::Int16 DataInput::ReadInt16()
      {
        return ReadUInt16();
      }

      System::Int32 DataInput::ReadInt32()
      {
        return ReadUInt32();
      }

      System::Int64 DataInput::ReadInt64()
      {
        return ReadUInt64();
      }

      array<Byte>^ DataInput::ReadReverseBytesOnly(int len)
      {
        CheckBufferSize(len);

        int i = 0;
        int j = m_cursor + len - 1;
        array<Byte>^ bytes = gcnew array<Byte>(len);

        while (i < len)
        {
          bytes[i++] = m_buffer[j--];
        }
        m_cursor += len;
        return bytes;
      }

      float DataInput::ReadFloat()
      {
        float data;

        array<Byte>^ bytes = nullptr;
        if (BitConverter::IsLittleEndian)
          bytes = ReadReverseBytesOnly(4);
        else
          bytes = ReadBytesOnly(4);

        data = BitConverter::ToSingle(bytes, 0);

        return data;
      }

      double DataInput::ReadDouble()
      {
        double data;

        array<Byte>^ bytes = nullptr;
        if (BitConverter::IsLittleEndian)
          bytes = ReadReverseBytesOnly(8);
        else
          bytes = ReadBytesOnly(8);

        data = BitConverter::ToDouble(bytes, 0);

        return data;
      }

      String^ DataInput::ReadUTF()
      {
        int length = ReadUInt16();
        CheckBufferSize(length);
        String^ str = DecodeBytes(length);
        return str;
      }

      String^ DataInput::ReadUTFHuge()
      {
        int length = ReadUInt32();
        CheckBufferSize(length);

        array<Char>^ chArray = gcnew array<Char>(length);

        for (int i = 0; i < length; i++)
        {
          Char ch = ReadByte();
          ch = ((ch << 8) | ReadByte());
          chArray[i] = ch;
        }

        String^ str = gcnew String(chArray);

        return str;
      }

      String^ DataInput::ReadASCIIHuge()
      {
        int length = ReadInt32();
        CheckBufferSize(length);
        String^ str = DecodeBytes(length);
        return str;
      }

      Object^ DataInput::ReadObject()
      {
        return ReadInternalObject();
      }

      /*	Object^ DataInput::ReadGenericObject( )
        {
        return ReadInternalGenericObject();
        }*/

      Object^ DataInput::ReadDotNetTypes(int8_t typeId)
      {
        switch (typeId)
        {
        case apache::geode::client::GeodeTypeIds::CacheableByte:
        {
          return ReadSByte();
        }
        case apache::geode::client::GeodeTypeIds::CacheableBoolean:
        {
          bool obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableWideChar:
        {
          Char obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableDouble:
        {
          Double obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableASCIIString:
        {
          /*	CacheableString^ cs = static_cast<CacheableString^>(CacheableString::CreateDeserializable());
            cs->FromData(this);
            return cs->Value;*/
          return ReadUTF();
        }
        case apache::geode::client::GeodeTypeIds::CacheableASCIIStringHuge:
        {
          /*CacheableString^ cs = static_cast<CacheableString^>(CacheableString::createDeserializableHuge());
          cs->FromData(this);
          return cs->Value;*/
          return ReadASCIIHuge();
        }
        case apache::geode::client::GeodeTypeIds::CacheableString:
        {
          /*CacheableString^ cs = static_cast<CacheableString^>(CacheableString::createUTFDeserializable());
          cs->FromData(this);
          return cs->Value;*/
          return ReadUTF();
        }
        case apache::geode::client::GeodeTypeIds::CacheableStringHuge:
        {
          //TODO: need to look all strings types
          /*CacheableString^ cs = static_cast<CacheableString^>(CacheableString::createUTFDeserializableHuge());
          cs->FromData(this);
          return cs->Value;*/
          return ReadUTFHuge();
        }
        case apache::geode::client::GeodeTypeIds::CacheableFloat:
        {
          float obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableInt16:
        {
          Int16 obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableInt32:
        {
          Int32 obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableInt64:
        {
          Int64 obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableDate:
        {
          CacheableDate^ cd = CacheableDate::Create();
          cd->FromData(this);
          return cd->Value;
        }
        case apache::geode::client::GeodeTypeIds::CacheableBytes:
        {
          return ReadBytes();
        }
        case apache::geode::client::GeodeTypeIds::CacheableDoubleArray:
        {
          array<Double>^ obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableFloatArray:
        {
          array<float>^ obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableInt16Array:
        {
          array<Int16>^ obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableInt32Array:
        {
          array<Int32>^ obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::BooleanArray:
        {
          array<bool>^ obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CharArray:
        {
          array<Char>^ obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableInt64Array:
        {
          array<Int64>^ obj;
          ReadObject(obj);
          return obj;
        }
        case apache::geode::client::GeodeTypeIds::CacheableStringArray:
        {
          return ReadStringArray();
        }
        case apache::geode::client::GeodeTypeIds::CacheableHashTable:
        {
          return ReadHashtable();
        }
        case apache::geode::client::GeodeTypeIds::CacheableHashMap:
        {
          CacheableHashMap^ chm = static_cast<CacheableHashMap^>(CacheableHashMap::CreateDeserializable());
          chm->FromData(this);
          return chm->Value;
        }
        case apache::geode::client::GeodeTypeIds::CacheableIdentityHashMap:
        {
          CacheableIdentityHashMap^ chm = static_cast<CacheableIdentityHashMap^>(CacheableIdentityHashMap::CreateDeserializable());
          chm->FromData(this);
          return chm->Value;
        }
        case apache::geode::client::GeodeTypeIds::CacheableVector:
        {
          /*CacheableVector^ cv = static_cast<CacheableVector^>(CacheableVector::CreateDeserializable());
          cv->FromData(this);
          return cv->Value;*/
          int len = ReadArrayLen();
          System::Collections::ArrayList^ retA = gcnew System::Collections::ArrayList(len);

          for (int i = 0; i < len; i++)
          {
            retA->Add(this->ReadObject());
          }
          return retA;
        }
        case apache::geode::client::GeodeTypeIds::CacheableArrayList:
        {
          /*CacheableArrayList^ cv = static_cast<CacheableArrayList^>(CacheableArrayList::CreateDeserializable());
          cv->FromData(this);
          return cv->Value;*/
          int len = ReadArrayLen();
          System::Collections::Generic::List<Object^>^ retA = gcnew System::Collections::Generic::List<Object^>(len);
          for (int i = 0; i < len; i++)
          {
            retA->Add(this->ReadObject());
          }
          return retA;

        }
        case apache::geode::client::GeodeTypeIds::CacheableLinkedList:
        {
          /*CacheableArrayList^ cv = static_cast<CacheableArrayList^>(CacheableArrayList::CreateDeserializable());
          cv->FromData(this);
          return cv->Value;*/
          int len = ReadArrayLen();
          System::Collections::Generic::LinkedList<Object^>^ retA = gcnew System::Collections::Generic::LinkedList<Object^>();
          for (int i = 0; i < len; i++)
          {
            retA->AddLast(this->ReadObject());
          }
          return retA;

        }
        case apache::geode::client::GeodeTypeIds::CacheableStack:
        {
          CacheableStack^ cv = static_cast<CacheableStack^>(CacheableStack::CreateDeserializable());
          cv->FromData(this);
          return cv->Value;
        }
        default:
          return nullptr;
        }
      }

      Object^ DataInput::ReadInternalObject()
      {
        try
        {
        //Log::Debug("DataInput::ReadInternalObject m_cursor " + m_cursor);
        bool findinternal = false;
        int8_t typeId = ReadByte();
        System::Int64 compId = typeId;
        TypeFactoryMethodGeneric^ createType = nullptr;

        if (compId == GeodeTypeIds::NullObj) {
          return nullptr;
        }
        else if (compId == GeodeClassIds::PDX)
        {
          //cache current state and reset after reading pdx object
          int cacheCursor = m_cursor;
          System::Byte* cacheBuffer = m_buffer;
          unsigned int cacheBufferLength = m_bufferLength;
          Object^ ret = Internal::PdxHelper::DeserializePdx(this, false, CacheRegionHelper::getCacheImpl(m_cache)->getSerializationRegistry().get());
          int tmp = m_nativeptr->get()->getBytesRemaining();
          m_cursor = cacheBufferLength - tmp;
          m_buffer = cacheBuffer;
          m_bufferLength = cacheBufferLength;
          m_nativeptr->get()->rewindCursor(m_cursor);

          if (ret != nullptr)
          {
            PdxWrapper^ pdxWrapper = dynamic_cast<PdxWrapper^>(ret);

            if (pdxWrapper != nullptr)
            {
              return pdxWrapper->GetObject();
            }
          }
          return ret;
        }
        else if (compId == GeodeClassIds::PDX_ENUM)
        {
          int8_t dsId = ReadByte();
          int tmp = ReadArrayLen();
          int enumId = (dsId << 24) | (tmp & 0xFFFFFF);

          Object^ enumVal = Internal::PdxHelper::GetEnum(enumId, m_cache);
          return enumVal;
        }
        else if (compId == GeodeTypeIds::CacheableNullString) {
          //return SerializablePtr(CacheableString::createDeserializable());
          //TODO::
          return nullptr;
        }
        else if (compId == GeodeTypeIdsImpl::CacheableUserData) {
          int8_t classId = ReadByte();
          //compId |= ( ( (System::Int64)classId ) << 32 );
          compId = (System::Int64)classId;
        }
        else if (compId == GeodeTypeIdsImpl::CacheableUserData2) {
          System::Int16 classId = ReadInt16();
          //compId |= ( ( (System::Int64)classId ) << 32 );
          compId = (System::Int64)classId;
        }
        else if (compId == GeodeTypeIdsImpl::CacheableUserData4) {
          System::Int32 classId = ReadInt32();
          //compId |= ( ( (System::Int64)classId ) << 32 );
          compId = (System::Int64)classId;
        }
        else if (compId == GeodeTypeIdsImpl::FixedIDByte) {//TODO: need to verify again
          int8_t fixedId = ReadByte();
          compId = fixedId;
          findinternal = true;
        }
        else if (compId == GeodeTypeIdsImpl::FixedIDShort) {
          System::Int16 fixedId = ReadInt16();
          compId = fixedId;
          findinternal = true;
        }
        else if (compId == GeodeTypeIdsImpl::FixedIDInt) {
          System::Int32 fixedId = ReadInt32();
          compId = fixedId;
          findinternal = true;
        }
        if (findinternal) {
          compId += 0x80000000;
          createType = Serializable::GetManagedDelegateGeneric((System::Int64)compId);
        }
        else {
          createType = Serializable::GetManagedDelegateGeneric(compId);
          if (createType == nullptr)
          {
            Object^ retVal = ReadDotNetTypes(typeId);

            if (retVal != nullptr)
              return retVal;

            if (m_ispdxDesrialization && typeId == apache::geode::client::GeodeTypeIds::CacheableObjectArray)
            {//object array and pdxSerialization
              return readDotNetObjectArray();
            }
            compId += 0x80000000;
            createType = Serializable::GetManagedDelegateGeneric(compId);

            /*if (createType == nullptr)
            {
            //TODO:: final check for user type if its not in cache
            compId -= 0x80000000;
            createType = Serializable::GetManagedDelegate(compId);
            }*/
          }
        }

        if (createType == nullptr) {
          throw gcnew IllegalStateException("Unregistered typeId " + typeId + " in deserialization, aborting.");
        }

        bool isPdxDeserialization = m_ispdxDesrialization;
        m_ispdxDesrialization = false;//for nested objects
        IGeodeSerializable^ newObj = createType();
        newObj->FromData(this);
        m_ispdxDesrialization = isPdxDeserialization;
        return newObj;
        }
        finally
        {
          GC::KeepAlive(m_nativeptr);
        }
      }

      Object^ DataInput::readDotNetObjectArray()
      {
        int len = ReadArrayLen();
        String^ className = nullptr;
        if (len >= 0)
        {
          ReadByte(); // ignore CLASS typeid
          className = (String^)ReadObject();
          className = Serializable::GetLocalTypeName(className);
          System::Collections::IList^ list = nullptr;
          if (len == 0)
          {
            list = (System::Collections::IList^)Serializable::GetArrayObject(className, len);
            return list;
          }
          //read first object

          Object^ ret = ReadObject();//in case it returns pdxinstance or java.lang.object

          list = (System::Collections::IList^)Serializable::GetArrayObject(ret->GetType()->FullName, len);

          list[0] = ret;
          for (System::Int32 index = 1; index < list->Count; ++index)
          {
            list[index] = ReadObject();
          }
          return list;
        }
        return nullptr;
      }

      Object^ DataInput::ReadInternalGenericObject()
      {
        bool findinternal = false;
        int8_t typeId = ReadByte();
        System::Int64 compId = typeId;
        TypeFactoryMethodGeneric^ createType = nullptr;

        if (compId == GeodeTypeIds::NullObj) {
          return nullptr;
        }
        else if (compId == GeodeClassIds::PDX)
        {
          return Internal::PdxHelper::DeserializePdx(this, false, CacheRegionHelper::getCacheImpl(m_cache)->getSerializationRegistry().get());
        }
        else if (compId == GeodeTypeIds::CacheableNullString) {
          //return SerializablePtr(CacheableString::createDeserializable());
          //TODO::
          return nullptr;
        }
        else if (compId == GeodeTypeIdsImpl::CacheableUserData) {
          int8_t classId = ReadByte();
          //compId |= ( ( (System::Int64)classId ) << 32 );
          compId = (System::Int64)classId;
        }
        else if (compId == GeodeTypeIdsImpl::CacheableUserData2) {
          System::Int16 classId = ReadInt16();
          //compId |= ( ( (System::Int64)classId ) << 32 );
          compId = (System::Int64)classId;
        }
        else if (compId == GeodeTypeIdsImpl::CacheableUserData4) {
          System::Int32 classId = ReadInt32();
          //compId |= ( ( (System::Int64)classId ) << 32 );
          compId = (System::Int64)classId;
        }
        else if (compId == GeodeTypeIdsImpl::FixedIDByte) {//TODO: need to verify again
          int8_t fixedId = ReadByte();
          compId = fixedId;
          findinternal = true;
        }
        else if (compId == GeodeTypeIdsImpl::FixedIDShort) {
          System::Int16 fixedId = ReadInt16();
          compId = fixedId;
          findinternal = true;
        }
        else if (compId == GeodeTypeIdsImpl::FixedIDInt) {
          System::Int32 fixedId = ReadInt32();
          compId = fixedId;
          findinternal = true;
        }
        if (findinternal) {
          compId += 0x80000000;
          createType = Serializable::GetManagedDelegateGeneric((System::Int64)compId);
        }
        else {
          createType = Serializable::GetManagedDelegateGeneric(compId);
          if (createType == nullptr)
          {
            Object^ retVal = ReadDotNetTypes(typeId);

            if (retVal != nullptr)
              return retVal;

            compId += 0x80000000;
            createType = Serializable::GetManagedDelegateGeneric(compId);
          }
        }

        if (createType != nullptr)
        {
          IGeodeSerializable^ newObj = createType();
          newObj->FromData(this);
          return newObj;
        }

        throw gcnew IllegalStateException("Unregistered typeId in deserialization, aborting.");
      }

      System::UInt32 DataInput::BytesRead::get()
      {
        AdvanceUMCursor();
        SetBuffer();

        try
        {
          return m_nativeptr->get()->getBytesRead();
        }
        finally
        {
          GC::KeepAlive(m_nativeptr);
        }
      }

      System::UInt32 DataInput::BytesReadInternally::get()
      {
        return m_cursor;
      }

      System::UInt32 DataInput::BytesRemaining::get()
      {
        AdvanceUMCursor();
        SetBuffer();
        try
        {
          return m_nativeptr->get()->getBytesRemaining();
        }
        finally
        {
          GC::KeepAlive(m_nativeptr);
        }
      }

      void DataInput::AdvanceCursor(System::Int32 offset)
      {
        m_cursor += offset;
      }

      void DataInput::RewindCursor(System::Int32 offset)
      {
        AdvanceUMCursor();
        try
        {
          m_nativeptr->get()->rewindCursor(offset);
        }
        finally
        {
          GC::KeepAlive(m_nativeptr);
        }
        SetBuffer();
      }

      void DataInput::Reset()
      {
        AdvanceUMCursor();
        try
        {
          m_nativeptr->get()->reset();
        }
        finally
        {
          GC::KeepAlive(m_nativeptr);
        }
        SetBuffer();
      }

      void DataInput::Cleanup()
      {
        //TODO:
        //GF_SAFE_DELETE_ARRAY(m_buffer);
      }

      void DataInput::ReadDictionary(System::Collections::IDictionary^ dict)
      {
        int len = this->ReadArrayLen();

        if (len > 0)
        {
          for (int i = 0; i < len; i++)
          {
            Object^ key = this->ReadObject();
            Object^ val = this->ReadObject();

            dict->Add(key, val);
          }
        }
      }

      IDictionary<Object^, Object^>^ DataInput::ReadDictionary()
      {
        int len = this->ReadArrayLen();

        if (len == -1)
          return nullptr;
        else
        {
          IDictionary<Object^, Object^>^ dict = gcnew Dictionary<Object^, Object^>();
          for (int i = 0; i < len; i++)
          {
            Object^ key = this->ReadObject();
            Object^ val = this->ReadObject();

            dict->Add(key, val);
          }
          return dict;
        }
      }

      System::DateTime DataInput::ReadDate()
      {
        long ticks = (long)ReadInt64();
        if (ticks != -1L)
        {
          m_cursor -= 8;//for above
          CacheableDate^ cd = CacheableDate::Create();
          cd->FromData(this);
          return cd->Value;
        }
        else
        {
          DateTime dt(0);
          return dt;
        }
      }

      void DataInput::ReadCollection(System::Collections::IList^ coll)
      {
        int len = ReadArrayLen();
        for (int i = 0; i < len; i++)
        {
          coll->Add(ReadObject());
        }
      }

      array<Char>^ DataInput::ReadCharArray()
      {
        array<Char>^ arr;
        this->ReadObject(arr);
        return arr;
      }

      array<bool>^ DataInput::ReadBooleanArray()
      {
        array<bool>^ arr;
        this->ReadObject(arr);
        return arr;
      }

      array<Int16>^ DataInput::ReadShortArray()
      {
        array<Int16>^ arr;
        this->ReadObject(arr);
        return arr;
      }

      array<Int32>^ DataInput::ReadIntArray()
      {
        array<Int32>^ arr;
        this->ReadObject(arr);
        return arr;
      }

      array<Int64>^ DataInput::ReadLongArray()
      {
        array<Int64>^ arr;
        this->ReadObject(arr);
        return arr;
      }

      array<float>^ DataInput::ReadFloatArray()
      {
        array<float>^ arr;
        this->ReadObject(arr);
        return arr;
      }

      array<double>^ DataInput::ReadDoubleArray()
      {
        array<double>^ arr;
        this->ReadObject(arr);
        return arr;
      }

      List<Object^>^ DataInput::ReadObjectArray()
      {
        //this to know whether it is null or it is empty
        int storeCursor = m_cursor;
        int len = this->ReadArrayLen();
        if (len == -1)
          return nullptr;
        //this will be read further by fromdata
        m_cursor = m_cursor - (m_cursor - storeCursor);


        CacheableObjectArray^ coa = CacheableObjectArray::Create();
        coa->FromData(this);
        List<Object^>^ retObj = (List<Object^>^)coa;

        if (retObj->Count >= 0)
          return retObj;
        return nullptr;
      }

      array<array<Byte>^>^ DataInput::ReadArrayOfByteArrays()
      {
        int len = ReadArrayLen();
        if (len >= 0)
        {
          array<array<Byte>^>^ retVal = gcnew array<array<Byte>^>(len);
          for (int i = 0; i < len; i++)
          {
            retVal[i] = this->ReadBytes();
          }
          return retVal;
        }
        else
          return nullptr;
      }

      void DataInput::ReadObject(array<UInt16>^% obj)
      {
        int len = ReadArrayLen();
        if (len >= 0)
        {
          obj = gcnew array<UInt16>(len);
          for (int i = 0; i < len; i++)
          {
            obj[i] = this->ReadUInt16();
          }
        }
      }

      void DataInput::ReadObject(array<UInt32>^% obj)
      {
        int len = ReadArrayLen();
        if (len >= 0)
        {
          obj = gcnew array<UInt32>(len);
          for (int i = 0; i < len; i++)
          {
            obj[i] = this->ReadUInt32();
          }
        }
      }

      void DataInput::ReadObject(array<UInt64>^% obj)
      {
        int len = ReadArrayLen();
        if (len >= 0)
        {
          obj = gcnew array<UInt64>(len);
          for (int i = 0; i < len; i++)
          {
            obj[i] = this->ReadUInt64();
          }
        }
      }

      String^ DataInput::ReadString()
      {
        UInt32 typeId = (Int32)ReadByte();

        if (typeId == GeodeTypeIds::CacheableNullString)
          return nullptr;

        if (typeId == GeodeTypeIds::CacheableASCIIString ||
            typeId == GeodeTypeIds::CacheableString)
        {
          return ReadUTF();
        }
        else if (typeId == GeodeTypeIds::CacheableASCIIStringHuge)
        {
          return ReadASCIIHuge();
        }
        else
        {
          return ReadUTFHuge();
        }  // namespace Client
      }  // namespace Geode
    }  // namespace Apache

  }
}
