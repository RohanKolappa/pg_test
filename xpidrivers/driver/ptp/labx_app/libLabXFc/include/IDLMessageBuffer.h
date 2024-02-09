#ifndef __IDL_MESSAGE_BUFFER_H_INCLUDED__
#define __IDL_MESSAGE_BUFFER_H_INCLUDED__

// System headers
#include <stdint.h>
#include <string.h> // For memcpy

// STL headers
#include <sstream>

// Lab X Foundation Classes headers
#include "Exception.h"

// Number of bytes of overhead consumed by marshalling some non-obvious
// primitive data types.
#define IDL_BOOL_BYTE_SIZE ((size_t) sizeof(uint32_t))

namespace LabX
{

  class IDLMessageBuffer
  {
  public:

      static const uint32_t k_LengthOffset         =  0;
      static const uint32_t k_NumLengthBytes   = 2;
      static const uint32_t k_MaxMessageLength = 1024;

      IDLMessageBuffer(void) { memset(bytes, 0, k_MaxMessageLength); }
      virtual ~IDLMessageBuffer(void) {}

      uint16_t getLength(void) const  { return get_uint16_t(k_LengthOffset); }

      void setLength(uint16_t length) { set_uint16_t(k_LengthOffset, length); }

      void set_bytes(uint32_t offset, const uint8_t* data, uint32_t length)
      { memcpy(&bytes[offset], data, length); }

      void set_uint8_t(uint32_t offset, uint8_t value)
      { bytes[offset] = value; }

      void set_uint16_t(uint32_t offset, uint16_t value)
      { bytes[offset] = value >> 8; bytes[offset+1] = value; }

      void set_uint32_t(uint32_t offset, uint32_t value)
      { set_uint16_t(offset, value >> 16); set_uint16_t(offset+2, value); }

      const uint8_t* get_bytes(uint32_t offset) const
      { return &bytes[offset]; }

      uint8_t get_uint8_t(uint32_t offset) const
      { return bytes[offset]; }

      uint16_t get_uint16_t(uint32_t offset) const
      { return (bytes[offset] << 8) | bytes[offset+1]; }

      uint32_t get_uint32_t(uint32_t offset) const
      { return ((uint32_t)get_uint16_t(offset) << 16) | (uint32_t)get_uint16_t(offset+2); }

    protected:
      uint8_t bytes[k_MaxMessageLength];
  };

  class IDLRequestMessageBuffer : public IDLMessageBuffer
  {
    public:
    
      static const uint32_t k_ClassCodeOffset      =  4;
      static const uint32_t k_InstanceNumberOffset =  6;
      static const uint32_t k_ServiceCodeOffset    =  8;
      static const uint32_t k_AttributeCodeOffset  = 10;
      static const uint32_t k_HeaderLength         = 12;
      static const uint32_t k_MinRequestLength     = k_HeaderLength;
      static const size_t k_MaxRequestLength       = k_MaxMessageLength;

      IDLRequestMessageBuffer(void) : IDLMessageBuffer() {}
      IDLRequestMessageBuffer(uint8_t *messageBytes, uint16_t length) {
        if(length > k_MaxMessageLength) {
          throw(new Exception("IDL request buffer max size exceeded"));
        }
        for(uint32_t byteIndex = 0; byteIndex < length; byteIndex++) {
          bytes[byteIndex] = messageBytes[byteIndex];
        }
        if(getLength() != length) {
          throw(new Exception("IDL message buffer length mismatch"));
        }
      }
      virtual ~IDLRequestMessageBuffer(void) {}

      void setClassCode(uint16_t classCode)           { set_uint16_t(k_ClassCodeOffset, classCode); }
      void setInstanceNumber(uint16_t instanceNumber) { set_uint16_t(k_InstanceNumberOffset, instanceNumber); }
      void setServiceCode(uint16_t serviceCode)       { set_uint16_t(k_ServiceCodeOffset, serviceCode); }
      void setAttributeCode(uint16_t attributeCode)   { set_uint16_t(k_AttributeCodeOffset, attributeCode); }

      uint16_t getPayloadOffset(void) const  { return k_HeaderLength; }
      uint16_t getClassCode(void) const      { return get_uint16_t(k_ClassCodeOffset); }
      uint16_t getInstanceNumber(void) const { return get_uint16_t(k_InstanceNumberOffset); }
      uint16_t getServiceCode(void) const    { return get_uint16_t(k_ServiceCodeOffset); }
      uint16_t getAttributeCode(void) const  { return get_uint16_t(k_AttributeCodeOffset); }
  };

  class IDLResponseMessageBuffer : public IDLMessageBuffer
  {
    public:
    
      static const uint32_t k_StatusCodeOffset  = 2;
      static const size_t k_HeaderLength        = 4;
      static const uint32_t k_MinResponseLength = k_HeaderLength;
      static const size_t k_MaxResponseLength   = k_MaxMessageLength;
      
      IDLResponseMessageBuffer(void) : IDLMessageBuffer() {}
      virtual ~IDLResponseMessageBuffer(void) {}

      void setStatusCode(uint16_t statusCode)         { set_uint16_t(k_StatusCodeOffset, statusCode); }

      uint16_t getPayloadOffset(void) const  { return k_HeaderLength; }
      uint16_t getStatusCode(void) const     { return get_uint16_t(k_StatusCodeOffset); }
  };

  // Common type helpers

  uint32_t inline uint8_t_marshal(IDLMessageBuffer& buffer, uint32_t offset, const uint8_t &value)
  { buffer.set_uint8_t(offset, value); return 1; }

  uint32_t inline uint8_t_unmarshal(const IDLMessageBuffer& buffer, uint32_t offset, uint8_t &value)
  { value = buffer.get_uint8_t(offset); return 1; }

  uint32_t inline uint16_t_marshal(IDLMessageBuffer& buffer, uint32_t offset, const uint16_t &value)
  { buffer.set_uint16_t(offset, value); return 2; }

  uint32_t inline uint16_t_unmarshal(const IDLMessageBuffer& buffer, uint32_t offset, uint16_t &value)
  { value = buffer.get_uint16_t(offset); return 2; }

  uint32_t inline uint32_t_marshal(IDLMessageBuffer& buffer, uint32_t offset, const uint32_t &value)
  { buffer.set_uint32_t(offset, value); return 4; }

  uint32_t inline uint32_t_unmarshal(const IDLMessageBuffer& buffer, uint32_t offset, uint32_t &value)
  { value = buffer.get_uint32_t(offset); return 4; }

  uint32_t inline float_marshal(IDLMessageBuffer& buffer, uint32_t offset, const float &value)
  { union { float f; uint32_t v; } u; u.f = value; buffer.set_uint32_t(offset, u.v); return 4; }

  uint32_t inline float_unmarshal(const IDLMessageBuffer& buffer, uint32_t offset, float &value)
  { union { float f; uint32_t v; } u; u.v = buffer.get_uint32_t(offset); value = u.f; return 4; }

  uint32_t inline bool_marshal(IDLMessageBuffer& buffer, uint32_t offset, const bool &value)
  { buffer.set_uint32_t(offset, (value) ? 0x00000001 : 0x00000000); return 4; }

  uint32_t inline bool_unmarshal(const IDLMessageBuffer& buffer, uint32_t offset, bool &value)
  { value = (buffer.get_uint32_t(offset) != 0); return 4; }

  // String marshalling involves putting both a NULL terminator, as well as the string length
  // (which must include the NULL terminator!)
  uint32_t inline string_t_marshal(IDLMessageBuffer& buffer, uint32_t offset, const std::string &value)
  { buffer.set_uint32_t(offset, (value.size() + 1));
    buffer.set_bytes(offset+4, (const uint8_t*)value.c_str(), (value.size() + 1)); return (value.size() + 1 + 4); }

  // When un-marshalling, we can ignore the NULL terminator, as the std::string constructor we're using
  // for safety's sake is explicitly provided the number of characters to incorporate.
  uint32_t inline string_t_unmarshal(const IDLMessageBuffer& buffer, uint32_t offset, std::string &value)
  { uint32_t length = buffer.get_uint32_t(offset);

    // The length must never be zero or we'll overflow things in the call to the std::string constructor.
    // Even a null string ("") has a NULL terminator, which is included in the length.
    if(length == 0) throw(new Exception("string_t_unmarshal() : Encountered zero-length string"));

    // It must also never extend past the end of the message!
    if((offset + 4 + length) > buffer.getLength()) {
      std::stringstream excStream;

      excStream << "string_t_unmarshal : "
                << length
                << "-byte string_t at offset "
                << offset
                << " exceeds overall buffer length ("
                << buffer.getLength()
                << ")";
      throw(new Exception(excStream.str()));
    }

    // Bounds-checking passed, create a std::string
    value = std::string((const char*)buffer.get_bytes(offset + 4), (length - 1)); return (length + 4); 
  }

  // IDL exceptions

  class InvalidServiceCodeException : public LabX::Exception
  {
    public:
      InvalidServiceCodeException(uint16_t classCode, uint16_t serviceCode) : 
    Exception(""), classCode(classCode), serviceCode(serviceCode) {}

    virtual std::string toString(void) const {
      std::stringstream excStream;
      excStream << "Invalid IDL Service Code: classCode = "
                << classCode
                << ", serviceCode = "
                << serviceCode;
      return(excStream.str());
    }

    public:
      uint16_t classCode;
      uint16_t serviceCode;
  };

  class InvalidAttributeCodeException : public Exception
  {
    public:
      InvalidAttributeCodeException(uint16_t classCode, uint16_t serviceCode, uint16_t attributeCode) : 
    Exception(""), classCode(classCode), serviceCode(serviceCode), attributeCode(attributeCode) {}

    virtual std::string toString(void) const {
      std::stringstream excStream;
      excStream << "Invalid IDL Attribute Code: classCode = "
                << classCode
                << ", serviceCode = "
                << serviceCode
                << ", attributeCode = "
                << attributeCode;
      return(excStream.str());
    }

    public:
      uint16_t classCode;
      uint16_t serviceCode;
      uint16_t attributeCode;
  };

  class InvalidInstanceException : public LabX::Exception
  {
    public:
      InvalidInstanceException(uint16_t classCode, uint16_t serviceCode, uint16_t attributeCode) :
    Exception(""), classCode(classCode), serviceCode(serviceCode), attributeCode(attributeCode) {}

    virtual std::string toString(void) const {
      std::stringstream excStream;
      excStream << "Invalid IDL Instance: classCode = "
                << classCode
                << ", serviceCode = "
                << serviceCode
                << ", attributeCode = "
                << attributeCode;
      return(excStream.str());
    }

    public:
      uint16_t classCode;
      uint16_t serviceCode;
      uint16_t attributeCode;
  };

} // namespace LabX

#endif // __IDL_MESSAGE_BUFFER_H_INCLUDED__
