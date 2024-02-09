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
//	VMP.AVC.NALU.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_NALU_H_
#define _VMP_AVC_NALU_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.NALUPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL NALU : virtual public BFC::SObject {

public :

	static const BFC::Uchar TypeSlice	= 0x01;
	static const BFC::Uchar TypeDPA		= 0x02;
	static const BFC::Uchar TypeDPB		= 0x03;
	static const BFC::Uchar TypeDPC		= 0x04;
	static const BFC::Uchar TypeIDR		= 0x05;
	static const BFC::Uchar TypeSEI		= 0x06;
	static const BFC::Uchar TypeSPS		= 0x07;
	static const BFC::Uchar TypePPS		= 0x08;
	static const BFC::Uchar TypeAUD		= 0x09;
	static const BFC::Uchar TypeEOSeq	= 0x0A;
	static const BFC::Uchar TypeEOStream	= 0x0B;
	static const BFC::Uchar TypeFill	= 0x0C;
	static const BFC::Uchar TypeSPSExt	= 0x0D;
	static const BFC::Uchar TypePrefix	= 0x0E;
	static const BFC::Uchar TypeSubSPS	= 0x0F;
	static const BFC::Uchar TypeAuxSlice	= 0x13;
	static const BFC::Uchar TypeSliceExt	= 0x14;

	/// \brief Creates a new NALU.

	NALU(
		const	BFC::Buffer &	pData
	) :
		data	( pData ),
		type	( pData[ 0 ] & 0x1F ),
		ridc	( ( pData[ 0 ] & 0x60 ) >> 5 )
	{
	}

	BFC::Uchar getType(
	) const {
		return type;
	}

	BFC::Bool isVCL(
	) const {
		return ( type >= TypeSlice && type <= TypeIDR );
	}

	BFC::Bool isNonVCL(
	) const {
		return ( ! isVCL() );
	}

	BFC::Bool isSlice(	// 1
	) const {
		return ( type == TypeSlice );
	}

	BFC::Bool isDPA(	// 2
	) const {
		return ( type == TypeDPA );
	}

	BFC::Bool isDPB(	// 3
	) const {
		return ( type == TypeDPB );
	}

	BFC::Bool isDPC(	// 4
	) const {
		return ( type == TypeDPC );
	}

	BFC::Bool isIDR(	// 5
	) const {
		return ( type == TypeIDR );
	}

	BFC::Bool isSEI(	// 6
	) const {
		return ( type == TypeSEI );
	}

	BFC::Bool isSPS(	// 7
	) const {
		return ( type == TypeSPS );
	}

	BFC::Bool isPPS(	// 8
	) const {
		return ( type == TypePPS );
	}

	BFC::Uchar getRefIdc(
	) const {
		return ridc;
	}

	const BFC::Buffer & getData(
	) const {
		return data;
	}

	BFC::Buffer getRBSP(
	) const;

	static const BFC::Buffer & getTypeName(
		const	BFC::Uchar	pType
	);

protected :

private :

	BFC::Buffer	data;
	BFC::Uchar	type;	///< nalu_type.
	BFC::Uchar	ridc;	///< nalu_ref_idc.

	/// \brief Forbidden default constructor.

	NALU(
	);

	/// \brief Forbidden copy constructor.

	NALU(
		const	NALU &
	);

	/// \brief Forbidden copy operator.

	NALU & operator = (
		const	NALU &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_NALU_H_

// ============================================================================

