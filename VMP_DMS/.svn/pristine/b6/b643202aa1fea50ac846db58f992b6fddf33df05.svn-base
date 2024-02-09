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
//	VMP.AAC.BitStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AAC_BitStream_H_
#define _VMP_AAC_BitStream_H_

// ============================================================================

#include "VMP.AAC.DLL.h"

// ============================================================================

#include "VMP.AAC.BitStreamPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

// ============================================================================

namespace VMP {
namespace AAC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AAC

class VMP_AAC_DLL BitStream : virtual public BFC::DObject {

public :

	/// \brief Creates a new BitStream.

	BitStream(
		const	BFC::Buffer &	pData
	);

	BFC::Uint32 read_u_v(
		const	BFC::Uint32	pCount
	);

	BFC::Uint32 read_u_1(
	);

	BFC::Uint32 tell(
	) const {
		return strOff;
	}

	BFC::Uint32 length(
	) const {
		return strBit;
	}

protected :

	BFC::Uint32 readFLC(
		const	BFC::Uint32	pToRead
	);

private :

	BFC::Buffer		strBuf;
	BFC::Uint32		strOff;
	BFC::Uint32		strLen;		///< Stream length (in bytes).
	BFC::Uint32		strBit;		///< Stream length (in bits).
	const BFC::Uchar *	strPtr;

	/// \brief Forbidden default constructor.

	BitStream(
	);

	/// \brief Forbidden copy constructor.

	BitStream(
		const	BitStream &
	);

	/// \brief Forbidden copy operator.

	BitStream & operator = (
		const	BitStream &
	);

};

// ============================================================================

} // namespace AAC
} // namespace VMP

// ============================================================================

#endif // _VMP_AAC_BitStream_H_

// ============================================================================

