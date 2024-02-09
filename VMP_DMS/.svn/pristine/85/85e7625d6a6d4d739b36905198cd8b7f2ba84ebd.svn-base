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
//	BFC.Net.CacheWriter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_CacheWriter_H_
#define _BFC_Net_CacheWriter_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, CacheWriter )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Represents channels for storing resources in the ResponseCache.
///
/// Instances of such a class provide a BFC::IO::ByteOutputStream object which
/// is called by protocol handlers to store the resource data into the cache,
/// and also an abort() method which allows a cache store operation to be
/// interrupted and abandoned. If an IOException is encountered while reading
/// the response or writing to the cache, the current cache store operation
/// will be aborted.
///
/// \ingroup BFC_Net

class BFC_Net_DLL CacheWriter : virtual public SObject {

public :

	/// \brief Returns a BFC::IO::ByteOutputStream to which the response
	///	body can be written.
	///
	/// \returns
	///	A BFC::IO::ByteOutputStream to which the response body can be
	///	written.

	virtual IO::ByteOutputStreamPtr getBody(
	) = 0;

	/// \brief Aborts the attempt to cache the response.
	///
	/// If an IOException is encountered while reading the response or
	/// writing to the cache, the current cache store operation will be
	/// abandoned.

	virtual void abort(
	) = 0;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_CacheWriter_H_

// ============================================================================

