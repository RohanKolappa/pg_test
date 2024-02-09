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
//	BFC.Net.IPv4Impl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_IPv4Impl_H_
#define _BFC_Net_IPv4Impl_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, IPv4Impl )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Net.IPImpl.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net

class BFC_Net_DLL IPv4Impl : public IPImpl {

protected :

	/// \brief Creates a new IPv4Impl.

	IPv4Impl(
	);

public :

	static IPv4ImplPtr instance(
	);

	// Constants

	virtual IPAddrCPtr getAnyLocal(
	) const;

	virtual IPAddrCPtr getLoopback(
	) const;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_IPv4Impl_H_

// ============================================================================

