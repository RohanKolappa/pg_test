#ifndef __MAC_ADDRESS_H__
#define __MAC_ADDRESS_H__

// System headers
#include <stdint.h>
#include <ostream>

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an ethernet MAC address abstraction.
   *
   * Copyright (c) 2010, Lab X Technologies, LLC
   * @author Chris Pane
   */
  class MacAddress
  {
    // Public constants and types
    public:
      
      /**
       * Number of bytes in a MAC address
       */
      static const size_t k_MacAddressBytes = 6;
  
    // Constructors / Destructor
    public:
      /**
       * Create a MAC address from a byte array.
       * @param mac - MAC address
       */
      MacAddress(const uint8_t mac[k_MacAddressBytes]);
      
      /**
       * Create a MAC address from a uint64_t
       * @param mac - MAC address
       */
      MacAddress(uint64_t mac);

      /**
       * Create a MAC address from a string
       * @param addressString - String to parse a MAC address from
       */
      MacAddress(const std::string &addressString);

      /**
       * Create an empty (broadcast) MAC address.
       */
      MacAddress(void);
  
    // Public interface methods
    public:

      /**
       * Compare MAC addresses
       * @ch - compare this MAC to ch
       *
       * @return - true if MAC addresses are identical
       */
      bool operator==(const MacAddress &ch) const;

      /**
       * Compare MAC addresses (not equal)
       * @ch - compare this MAC to ch
       *
       * @return - true if MAC addresses are not identical
       */
      bool operator!=(const MacAddress &ch) const;

      /**
       * Compare MAC addresses (less than)
       * @ch - compare this MAC to ch
       *
       * @return - true if this MAC is smaller than ch (viewed as a big-endian 6 byte number)
       */
      bool operator<(const MacAddress &ch) const;

      /**
       * Assign a new value to this MAC object.
       * @param rhs - new MAC to assign
       *
       * @return - reference to this MAC object
       */
      MacAddress& operator=(const MacAddress &rhs);

      /**
       * Add to the MAC value.
       * @param rhs - addend
       * @return - MAC + addend
       */
      MacAddress operator+(int64_t rhs) const;

      /**
       * Subtract from the MAC value.
       * @param rhs - subtrahend
       * @return - MAC - subtrahend 
       */
     // int64_t operator-(const LabX::MacAddress &rhs) const;
      int64_t subtract(const LabX::MacAddress &rhs) const;

      /**
       * Return reference to mac bytes
       * @i - index to use for reference
       *
       * @return - byte value at m_mac[i]
       */
      uint8_t & operator[](int i) { return m_mac[i]; }

      /**
       * uint8_t* conversion opeartor
       * @return - MAC byte array (6 bytes)
       */
      operator const uint8_t * () const { return m_mac;};

      /**
       * Debug print function to display the MAC address on cout.
       */
      void print(void) const;

      /**
       * Convert the MAC address to a string
       * @return - String with the colon separated MAC
       */
      const std::string toString(void) const;

    // Private helper methods
    private:

      /**
       * Check if a character is a hex digit.
       * @param ch - Character to check
       * @return - true if the character can be a hex digit
       */
      static const bool isHexDigit(const char ch);

    private:
      /**
       * MAC address storage
       */
      uint8_t m_mac[k_MacAddressBytes];
  };

  /**
   * Stream output operator.
   * @param o   - ostream to write the MAC on
   * @param mac - MAC to write
   * @return    - ostream o
   */
  std::ostream& operator<<(std::ostream& o, const MacAddress& mac);

} // namespace LabX

#endif // __MAC_ADDRESS_H__
