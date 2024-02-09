// File        : IDLSupport.h
// Author      : Eldridge M. Mount IV
// Description : Support functions for using IDL within the Lab X
//               Foundation Class framework
//
// Copyright (c) 2011, Lab X Technologies, LLC
// All rights reserved

#ifndef _IDL_SUPPORT_H_
#define _IDL_SUPPORT_H_

// STL headers
#include <stdint.h>
#include <stdio.h>
#include <vector>

// Library headers
#include "IDLMessageBuffer.h"

// Enumeration of service codes
typedef enum {
  k_SC_getAttribute = 1,
  k_SC_setAttribute = 2,
} CommonServiceCode;

// Type definitions for shorthand, used by generated code
typedef LabX::IDLMessageBuffer MessageBuffer_t;
typedef LabX::IDLRequestMessageBuffer RequestMessageBuffer_t;
typedef LabX::IDLResponseMessageBuffer ResponseMessageBuffer_t;
typedef std::string string_t;

// Map the IDL sequence type to a native C++ collection type
#define sequence_t std::vector

// Number of bytes of overhead consumed by an IDL sequence marshal
#define IDL_SEQUENCE_OVERHEAD  ((size_t) sizeof(uint32_t))

// Sequence marshal / unmarshal implementations for common primitve types

static inline uint32_t sequence_t_uint8_t__marshal(MessageBuffer_t &buffer,
                                                    uint32_t offset,
                                                    const sequence_t<uint8_t> &value) {
  uint32_t sequenceOffset = 0;
  sequenceOffset += uint32_t_marshal(buffer, offset, value.size());
  for (std::vector<uint8_t>::const_iterator i = value.begin(); i != value.end(); i++) {
    sequenceOffset += uint8_t_marshal(buffer, (offset + sequenceOffset), (*i));
  }
  return(sequenceOffset);
}

static inline uint32_t sequence_t_uint8_t__unmarshal(const MessageBuffer_t &buffer,
                                                      uint32_t offset,
                                                      sequence_t<uint8_t> &value) {
  uint32_t sequenceOffset = 0;
  uint32_t sequenceLength = 0;
  sequenceOffset += uint32_t_unmarshal(buffer, offset, sequenceLength);
  while(sequenceLength--) {
    uint8_t entry = 0;
    sequenceOffset += uint8_t_unmarshal(buffer, (offset + sequenceOffset), entry);
    value.push_back(entry);
  }
  return(sequenceOffset);
}

static inline uint32_t sequence_t_uint16_t__marshal(MessageBuffer_t &buffer, 
                                                    uint32_t offset, 
                                                    const sequence_t<uint16_t> &value) {
  uint32_t sequenceOffset = 0;
  sequenceOffset += uint32_t_marshal(buffer, offset, value.size());
  for (std::vector<uint16_t>::const_iterator i = value.begin(); i != value.end(); i++) {
    sequenceOffset += uint16_t_marshal(buffer, (offset + sequenceOffset), (*i));
  }  
  return(sequenceOffset);
}

static inline uint32_t sequence_t_uint16_t__unmarshal(const MessageBuffer_t &buffer, 
                                                      uint32_t offset, 
                                                      sequence_t<uint16_t> &value) {
  uint32_t sequenceOffset = 0;
  uint32_t sequenceLength = 0;
  sequenceOffset += uint32_t_unmarshal(buffer, offset, sequenceLength);
  while(sequenceLength--) {
    uint16_t entry = 0;
    sequenceOffset += uint16_t_unmarshal(buffer, (offset + sequenceOffset), entry);
    value.push_back(entry);
  }
  return(sequenceOffset);
}

static inline uint32_t sequence_t_uint32_t__marshal(MessageBuffer_t &buffer, 
                                                    uint32_t offset, 
                                                    const sequence_t<uint32_t> &value) {
  uint32_t sequenceOffset = 0;
  sequenceOffset += uint32_t_marshal(buffer, offset, value.size());
  for (std::vector<uint32_t>::const_iterator i = value.begin(); i != value.end(); i++) {
    sequenceOffset += uint32_t_marshal(buffer, (offset + sequenceOffset), (*i));
  }
  return(sequenceOffset);
}

static inline uint32_t sequence_t_uint32_t__unmarshal(const MessageBuffer_t &buffer, 
                                                      uint32_t offset, 
                                                      sequence_t<uint32_t> &value) {
  uint32_t sequenceOffset = 0;
  uint32_t sequenceLength = 0;
  sequenceOffset += uint32_t_unmarshal(buffer, offset, sequenceLength);
  while(sequenceLength--) {
    uint32_t entry = 0;
    sequenceOffset += uint32_t_unmarshal(buffer, (offset + sequenceOffset), entry);
    value.push_back(entry);
  }
  return(sequenceOffset);
}

#endif
