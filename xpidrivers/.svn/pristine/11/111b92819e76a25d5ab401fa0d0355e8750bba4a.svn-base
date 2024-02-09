//
// Copyright 2003 Lab X Technologies, LLC

#ifndef _LABX_H_
#define _LABX_H_

// System headers
#include <sstream>
#include <string>
#include <stdint.h>

// Project headers
#include "Debug.h"
#include "Exception.h"

// Declaration of namespace LabX

namespace LabX
{
  ////
  // Converts a canonical filename to a basic filename (no full path).
  // The returned pointer points into the passed string, so no deallocation
  // is required.
  const char* stripFilename(const char* filename);

  ////
  // Converts an integer to a string
  const std::string toString(const int32_t value);

  ////
  // Converts an integer to a string
  const std::string toString(const int16_t value);

  ////
  // Converts an integer to a string
  const std::string toString(const int8_t value);

  ////
  // Converts an unsigned integer to a string
  const std::string toString(const uint32_t value);

  ////
  // Converts an unsigned integer to a string
  const std::string toString(const uint16_t value);

  ////
  // Converts an unsigned integer to a string
  const std::string toString(const uint8_t value);

  ////
  // Converts a double-precision floating-point value to a string with
  // the passed number of significant digits
  const std::string toString(const double value, const unsigned int sigDigits);

  ////
  // Converts an integer to a hexadecimal-formatted string with the indicated
  // number of significant hexadecimal digits.  A leading "0x" is appended
  // by default.
  const std::string toHexString(const uint64_t value, 
                                const unsigned int hexDigits,
                                const bool basePrefix = true);

  ////
  // Converts a boolean value to a string
  const std::string toString(const bool value);

  ////
  // Converts the passed string to an int8_t
  const int8_t toint8_t(const std::string& strValue);

  ////
  // Converts the passed string to a uint8_t
  const uint8_t touint8_t(const std::string& strValue);

  ////
  // Converts the passed string to an int16_t
  const int16_t toint16_t(const std::string& strValue);

  ////
  // Converts the passed string to a uint16_t
  const uint16_t touint16_t(const std::string& strValue);

  ////
  // Converts the passed string to an int32_t
  const int32_t toint32_t(const std::string& strValue);

  ////
  // Converts the passed string to a uint32_t
  const uint32_t touint32_t(const std::string& strValue);

  ////
  // Converts the passed string to a double
  const double toDouble(const std::string& strValue);

  ////
  // Clamps the passed value to the passed range interval
  template<class DC>
    const DC rangeClamp(const DC value, const DC min, const DC max)
    {
      return((value < min) ? min : ((value > max) ? max : value));
    } // LabX::rangeClamp<DC>()

  /**
   * Calculate the minimum number of bits to hold a value.
   * This is adaped from the linux bitops fls function.
   *
   * @param x - Value to check
   * @return Number of bits required to hold x
   */
  static inline int MinBits(uint32_t x)
  {
    int r = 32;

    if (!x) return 1;
  
    if (!(x & 0xffff0000u)) {
        x <<= 16;
      r -= 16;
    }
    if (!(x & 0xff000000u)) {
      x <<= 8;
      r -= 8;
    }
    if (!(x & 0xf0000000u)) {
      x <<= 4;
      r -= 4;
    }
    if (!(x & 0xc0000000u)) {
      x <<= 2;
      r -= 2;
    }
    if (!(x & 0x80000000u)) {
      x <<= 1;
      r -= 1;
    }
    return r;
  }

}

#endif

