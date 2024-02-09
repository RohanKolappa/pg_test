#ifndef __MAC_RANGE_H__
#define __MAC_RANGE_H__

// System headers
#include <stdint.h>
#include <stddef.h>

// Library headers
#include "MacAddress.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class provides an ethernet MAC address range abstraction.
   *
   * Copyright (c) 2010, Lab X Technologies, LLC
   * @author Chris Wulff
   */
  class MacRange
  {
    // Constructors / Destructor
    public:
      /**
       * Create a MAC address range from a MAC and a count.
       * @param mac   - MAC address for the start of the range
       * @param count - number of addresses in the range
       */
      MacRange(const MacAddress& mac, size_t count);

      /**
       * Create a MAC address range from a pair of MACs (inclusive)
       * @param macStart - MAC address for the start of the range
       * @param macEnd   - MAC address for the end of the range
       */
      MacRange(const MacAddress& macStart, const MacAddress& macEnd);

      /**
       * Create an empty MAC address range.
       */
      MacRange(void);
  
    // Public interface methods
    public:

      /**
       * Compare MAC address ranges
       * @range - compare this MAC range to range
       *
       * @return - true if MAC addresses ranges are identical
       */
      bool operator==(const MacRange &range) const;

      /**
       * Compare MAC address ranges (not equal)
       * @range - compare this MAC range to range
       *
       * @return - true if MAC address ranged are not identical
       */
      bool operator!=(const MacRange &range) const;

      /**
       * Compare MAC address ranges (less than)
       * @range - compare this MAC range to range
       *
       * @return - true if this MAC range starts before range
       */
      bool operator<(const MacRange &range) const;

      /**
       * Assign a new value to this MAC range object.
       * @param rhs - new MAC range to assign
       *
       * @return - reference to this MAC range object
       */
      MacRange& operator=(const MacRange &rhs);

      /**
       * Get the intersection of two ranges.
       * @param rhs - range to intersect with this
       * @return - the intersection of these ranges (or an empty range)
       */
      MacRange intersect(const MacRange &rhs) const;

      /**
       * Get the MAC at the start of the range.
       * @return - range starting MAC
       */
      const MacAddress& getStart(void) const { return start; }

      /**
       * Get the number of MAC addresses in the range.
       * @return - number of addresses in the range
       */
      size_t getCount(void) const { return count; }

    private:

      /**
       * Mac range start.
       */
      MacAddress start;

      /**
       * Number of MACs in the range
       */
      size_t count;
  };

  /**
   * Stream output operator.
   * @param o        - ostream to write the Mac Range on
   * @param macRange - MAC range to write
   * @return         - ostream o
   */
  std::ostream& operator<<(std::ostream& o, const MacRange& macRange);


} // namespace LabX

#endif // __MAC_RANGE_H__
