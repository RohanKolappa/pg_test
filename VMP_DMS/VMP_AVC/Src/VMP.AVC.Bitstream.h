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
//	VMP.AVC.Bitstream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_Bitstream_H_
#define _VMP_AVC_Bitstream_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.BitstreamPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL Bitstream : virtual public BFC::DObject {

public :

	/// \brief Creates a new Bitstream.

	Bitstream(
		const	BFC::Buffer &	pData
	);

	BFC::Uint32 read_u_v(
		const	BFC::Uint32	pCount
	);

	BFC::Uint32 read_u_1(
	);

	BFC::Uint32 read_ue_v(
	);

	BFC::Int32 read_se_v(
	);

	void skip(
		const	BFC::Uint32	pSize
	) {
		strOff += pSize;
	}

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

	BFC::Uint32 readVLC(
		const	BFC::Bool	pSigned
	);

private :

	BFC::Buffer		strBuf;
	BFC::Uint32		strOff;
	BFC::Uint32		strLen;		///< Stream length (in bytes).
	BFC::Uint32		strBit;		///< Stream length (in bits).
	const BFC::Uchar *	strPtr;

	/// \brief Forbidden default constructor.

	Bitstream(
	);

	/// \brief Forbidden copy constructor.

	Bitstream(
		const	Bitstream &
	);

	/// \brief Forbidden copy operator.

	Bitstream & operator = (
		const	Bitstream &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_Bitstream_H_

// ============================================================================

