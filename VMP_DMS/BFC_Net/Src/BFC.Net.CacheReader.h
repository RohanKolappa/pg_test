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
//	BFC.Net.CacheReader.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_CacheReader_H_
#define _BFC_Net_CacheReader_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, CacheReader )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.IO.ByteInputStream.h"

#include "BFC.TL.List.h"
#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents channels for retrieving resources from the ResponseCache.
///
/// Instances of such a class provide an BFC::IO::ByteInputStream that returns
/// the entity body, and also a getHeaders() method which returns the
/// associated response headers.
///
/// \ingroup BFC_Net

class BFC_Net_DLL CacheReader : virtual public SObject {

public :

	/// \brief Returns the response headers.
	///
	/// \returns
	///	An immutable Map from response header field names to lists of
	///	field values. The status line has null as its field name.

	virtual Map< Buffer, List< Buffer > > getHeaders(
	) = 0;

	/// \brief Returns the response body as a BFC::IO::ByteInputStream.
	///
	/// \returns
	///	A BFC::IO::InputStream from which the response body can be
	///	accessed.

	virtual IO::ByteInputStreamPtr getBody(
	) = 0;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_CacheReader_H_

// ============================================================================

