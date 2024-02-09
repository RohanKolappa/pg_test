// ============================================================================
// 
// Copyright (c) 2003-2009 Barco N.V.
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
//	BFC.HTTP.Answer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_HTTP_Answer_H_
#define _BFC_HTTP_Answer_H_

// ============================================================================

#include "BFC.HTTP.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace HTTP {

BFC_PTR_DECL( BFC_HTTP_DLL, Answer )

} // namespace HTTP
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Debug.DObject.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace HTTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_JMHP

class BFC_HTTP_DLL Answer : virtual public DObject {

public :

	/// \brief Creates a new Answer.

	Answer(
	);

	Bool parse(
		const	Buffer &	partial
	);

	const BufferArray & getHeader(
	) const {
		return header;
	}

	Buffer getHeader(
		const	Buffer &	pTag
	) const;

	const Buffer & getPayload(
	) const {
		return pldBuf;
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Answer" );

	CLASS_EXCEPTION( Overflow, "Overflow" );

protected :

	Bool parsePayload(
		const	Buffer &	pData
	);

private :

	Bool		gotHdr;	///< Got complete header ?
	Bool		gotPld;	///< Got complete payload ?
	Buffer		hdrBuf;	///< Header (CR-NL separated lines).
	BufferArray	header;	///< Array of lines.
	Uint32		pldLen;	///< Payload length.
	Buffer		pldBuf;	///< Payload content.
	Bool		chunks;	///< Transfer-Encoding: chunked.
	Buffer		chkBuf;	///< Buffer for partial chunks.

	/// \brief Forbidden copy constructor.

	Answer(
		const	Answer &
	);

	/// \brief Forbidden copy operator.

	Answer & operator = (
		const	Answer &
	);

};

// ============================================================================

} // namespace HTTP
} // namespace BFC

// ============================================================================

#endif // _BFC_HTTP_Answer_H_

// ============================================================================

