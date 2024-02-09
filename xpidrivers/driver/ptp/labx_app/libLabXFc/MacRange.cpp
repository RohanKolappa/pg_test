#include "MacRange.h"

using namespace std;

namespace LabX {

MacRange::MacRange(const MacAddress& mac, size_t count) :
  start(mac), count(count)
{
}

MacRange::MacRange(const MacAddress& macStart, const MacAddress& macEnd) :
  start(macStart), count(macEnd.subtract(macStart))
{
}

MacRange::MacRange(void) :
  count(0)
{
}

bool MacRange::operator==(const MacRange& range) const
{
  return ((start == range.start) && (count == range.count));
}

bool MacRange::operator!=(const MacRange& range) const
{
  return !(*this == range);
}

bool MacRange::operator<(const MacRange &range) const
{
  return (start < range.start);
}

MacRange & MacRange::operator=(const MacRange &rhs)
{
  start = rhs.start;
  count = rhs.count;
  return *this;
}

static inline const MacAddress& max(const MacAddress& a, const MacAddress&b)
{
  return (a < b) ? b : a;
}

static inline const MacAddress& min(const MacAddress& a, const MacAddress&b)
{
  return (a < b) ? a : b;
}

MacRange MacRange::intersect(const MacRange &rhs) const
{
  MacAddress thisEnd = start + count;
  MacAddress rhsEnd = rhs.start + rhs.count;

  if ((thisEnd < rhs.start) || (rhsEnd < start))
  {
    // Empty range
    return MacRange();
  }

  return MacRange(max(start, rhs.start), min(thisEnd, rhsEnd));
}

ostream& operator<<(ostream& o, const MacRange& macRange)
{
  o << macRange.getStart() << "+" << macRange.getCount();
  return o;
}


} // namespace LabX

