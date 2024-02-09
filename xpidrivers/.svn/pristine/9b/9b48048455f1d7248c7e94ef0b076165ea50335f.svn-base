#include "MacAddress.h"
#include "Exception.h"
#include <iostream>
#include <string.h>
#include <sstream>

using std::cout;
using std::endl;
using std::hex;
using std::ios;
using std::istringstream;
using std::ostream;
using std::string;
using std::stringstream;

namespace LabX {

MacAddress::MacAddress(const unsigned char mac[k_MacAddressBytes])
{
  memcpy(m_mac,mac,k_MacAddressBytes);
}

MacAddress::MacAddress(uint64_t mac)
{
  for (size_t i=0; i<k_MacAddressBytes; i++)
  {
    m_mac[i] = (mac >> (((k_MacAddressBytes-1)-i)*8)) & 0xFF;
  }
}

MacAddress::MacAddress(const string &addressString) {
  uint32_t tempInteger;
  bool parseFailure = false;

  // First sanity-check the string length against the number of bytes
  if(addressString.length() == ((3 * k_MacAddressBytes) - 1)) {
    // Loop, converting each byte from a ##:##:##:##:##:## hexadecimal format
    uint32_t charIndex = 0;
    for(uint32_t byteIndex = 0; byteIndex < k_MacAddressBytes; byteIndex++, charIndex += 3) {
      // Make sure the next two characters are from the set of hexadecimal digits
      for(uint32_t checkIndex = 0; checkIndex < 2; checkIndex++) {
        if(!isHexDigit(addressString[charIndex + checkIndex])) parseFailure = true;
      }
      if(parseFailure) break;

      istringstream byteStream(addressString.substr(charIndex, 2));
      byteStream >> hex >> tempInteger;
      parseFailure = byteStream.fail();
      if(byteIndex < (k_MacAddressBytes - 1)) parseFailure |= (addressString[charIndex + 2] != ':');
      m_mac[byteIndex] = static_cast<uint8_t>(tempInteger);
    }
  } else parseFailure = true;

  if(parseFailure) {
    stringstream excStream;
    excStream << "MAC address string \"" 
              << addressString 
              << "\" is not a string of form \"##:##:##:##:##:##\"";
    throw(new Exception(excStream.str()));
  }
}

MacAddress::MacAddress(void)
{
  memset(m_mac, 0xFF, k_MacAddressBytes);
}

bool MacAddress::operator==(const MacAddress& mac) const
{
  return ((memcmp(m_mac, mac.m_mac, k_MacAddressBytes) == 0)); 
}

bool MacAddress::operator!=(const MacAddress& mac) const
{
  return !(*this == mac);
}

bool MacAddress::operator<(const MacAddress &mac) const
{
  return (memcmp(m_mac, mac.m_mac, k_MacAddressBytes) < 0);
}

MacAddress & MacAddress::operator=(const MacAddress &rhs)
{
  memcpy(m_mac, rhs.m_mac, k_MacAddressBytes);
  return *this;
}

MacAddress MacAddress::operator+(int64_t rhs) const
{
  uint64_t mac = 0;
  for (size_t i=0; i<k_MacAddressBytes; i++)
  {
    mac |= ((uint64_t)m_mac[i])<<(((k_MacAddressBytes-1)-i)*8);
  }

  mac += rhs;

  uint8_t newMacAddress[k_MacAddressBytes];
  for (size_t i=0; i<k_MacAddressBytes; i++)
  {
    newMacAddress[i] = (mac >> (((k_MacAddressBytes-1)-i)*8)) & 0xFF;
  }

  return MacAddress(newMacAddress);
}

// int64_t MacAddress::operator-(const LabX::MacAddress &rhs) const
int64_t MacAddress::subtract(const LabX::MacAddress &rhs) const
{
  uint64_t mac = 0;
  for (size_t i=0; i<k_MacAddressBytes; i++)
  {
    mac |= ((uint64_t)m_mac[i])<<(((k_MacAddressBytes-1)-i)*8);
  }

  uint64_t rhsMacAddress = 0;
  for (size_t i=0; i<k_MacAddressBytes; i++)
  {
    rhsMacAddress |= ((uint64_t)rhs.m_mac[i])<<(((k_MacAddressBytes-1)-i)*8);
  }

  return (mac - rhsMacAddress);
}

void MacAddress::print(void) const
{
  cout << *this << endl;
}

const string MacAddress::toString(void) const {
  stringstream macStream;
 
  macStream << (*this);
 
  return(macStream.str());
}

const bool MacAddress::isHexDigit(char ch) {
  return(((ch >= '0') & (ch <= '9')) |
         ((ch >= 'a') & (ch <= 'f')) |
         ((ch >= 'A') & (ch <= 'F')));
}

ostream& operator<<(ostream& o, const MacAddress& mac)
{
  const uint8_t *data = mac;
  static const char hexDigits[] = "0123456789abcdef";

  for(size_t i=0; i<MacAddress::k_MacAddressBytes; ++i)
  {
    o << hexDigits[(data[i]>>4) & 0x0F] << hexDigits[data[i] & 0x0F];
    if (i < (MacAddress::k_MacAddressBytes - 1)) o << ":";
  }

  return o;
}

} // namespace LabX

