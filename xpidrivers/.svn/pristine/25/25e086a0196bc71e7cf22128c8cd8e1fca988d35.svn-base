//
// Copyright 2003 Lab X Technologies, LLC

// System headers
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Project headers
#include "LabX.h"

using std::string;

const char*
LabX::stripFilename(const char* filename)
{
  const char* strippedFilename;

  // Strip just the filename off the canonical pathname
  strippedFilename = strrchr(filename, '/');
  return(strippedFilename != NULL ? (strippedFilename + 1) : filename);
} // LabX::stripFilename()	 

const string
LabX::toString(const int32_t value)
{
  char buf[32];

  snprintf(buf, 32, "%d", value);
  buf[31] = '\0';

  return(buf);
} // LabX::toString()

const string
LabX::toString(const int16_t value)
{
  return(toString(static_cast<int32_t>(value)));
} // LabX::toString()

const string
LabX::toString(const int8_t value)
{
  return(toString(static_cast<int32_t>(value)));
} // LabX::toString()

const string
LabX::toString(const uint32_t value)
{
  char buf[32];

  snprintf(buf, 32, "%u", value);
  buf[31] = '\0';

  return(buf);
} // LabX::toString()

const string
LabX::toString(const uint16_t value)
{
  return(toString(static_cast<uint32_t>(value)));
} // LabX::toString()

const string
LabX::toString(const uint8_t value)
{
  return(toString(static_cast<uint32_t>(value)));
} // LabX::toString()

const string
LabX::toString(const double value, const unsigned int sigDigits)
{
  char format[32];
  char buf[32];

  snprintf(format, 32, "%%.%df", sigDigits);
  format[31] = '\0';
  snprintf(buf, 32, format, value);
  buf[31] = '\0';

  return(buf);
} // LabX::toString()

const string
LabX::toHexString(const uint64_t value, 
                  const unsigned int hexDigits,
                  const bool basePrefix)
{
  string retValue;
  char buf[32];

  if(basePrefix) retValue.append("0x");
  snprintf(buf, 32, "%llX", value);
  buf[31] = '\0';

  // Append any necessary leading zeros to the value
  int zeros = (hexDigits - strlen(buf));
  if(zeros > 0) {
    retValue.append(zeros, '0');
  }
  retValue.append(buf);
  return(retValue);
} // LabX::toHexString()

const string
LabX::toString(const bool value)
{
  return(value ? "true" : "false");
} // LabX::toString()

const int8_t
LabX::toint8_t(const string& strValue)
{
  return(static_cast<int8_t>(toint32_t(strValue)));
} // LabX::toint8_t()

const uint8_t
LabX::touint8_t(const string& strValue)
{
  return(static_cast<const uint8_t>(toint8_t(strValue)));
} // LabX::touint8_t()

const int16_t
LabX::toint16_t(const string& strValue)
{
  return(static_cast<int16_t>(toint32_t(strValue)));
} // LabX::toint16_t()

const uint16_t
LabX::touint16_t(const string& strValue)
{
  return(static_cast<const uint16_t>(toint16_t(strValue)));
} // LabX::touint16_t()

const int32_t
LabX::toint32_t(const string& strValue)
{
  return(static_cast<int32_t>(strtol(strValue.c_str(), NULL, 10)));
} // LabX::toint32_t()

const uint32_t
LabX::touint32_t(const string& strValue)
{
  return(static_cast<const uint32_t>(toint32_t(strValue)));
} // LabX::touint32_t()

const double
LabX::toDouble(const string& strValue)
{
  return(atof(strValue.c_str()));
}

