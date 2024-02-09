// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Net.Address.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_Address_H_
#define _BFC_Net_Address_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.AddressPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Comparable.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net

class BFC_Net_DLL Address : virtual public Comparable {

public :

	/// \brief Constructs a string representation of this SockAddr.
	///
	/// This Buffer is constructed by calling toBuffer() on the IPAddr and
	/// concatenating the port number (with a colon). If the address is
	/// unresolved then the part before the colon will only contain the
	/// hostname.
	///
	/// \returns
	///	A string representation of this object.

	virtual Buffer toBuffer(
		const	Bool		pReverse = true
	) const = 0;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_Address_H_

// ============================================================================

