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
//	VMP.BytesFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_BytesFrame_H_
#define _VMP_BytesFrame_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.BytesFramePtr.h"

// ============================================================================

#include "VMP.Frame.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL BytesFrame : public Frame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new BytesFrame.

	BytesFrame(
	) :
		BFC::DL::Object	( getClassType() ),
		unitOffs	( Frame::NoOffset ) {
	}

	BytesFrame(
		const	BFC::Buffer &	pPayload
	) :
		BFC::DL::Object	( getClassType() ),
		frmeData	( pPayload ),
		unitOffs	( Frame::NoOffset ) {
	}

	BytesFrame(
			FrameCPtr	pTmplFrme,
		const	BFC::Buffer &	pPayload
	) :
		BFC::DL::Object	( getClassType() ),
		Frame		( pTmplFrme ),
		frmeData	( pPayload ),
		unitOffs	( Frame::NoOffset ) {
	}

	BytesFrame(
		const	BFC::Buffer &	pPayload,
		const	BFC::Uint32	pUnitOffs,
		const	BFC::Uint32	pFrmeOffs,
		const	BFC::Time::Clock &
					pFramePts,
		const	BFC::Time::Clock &
					pFrameDts,
		const	BFC::Bool	pIsKeyFrm,
		const	BFC::Bool	pSegmStrt
	) :
		BFC::DL::Object	( getClassType() ),
		Frame		( pFrmeOffs, pFramePts, pFrameDts, pIsKeyFrm, pSegmStrt ),
		frmeData	( pPayload ),
		unitOffs	( pUnitOffs ) {
	}

	BytesFrame(
		const	BytesFrame &	pOther
	) :
		BFC::SObject	(),
		BFC::Cloneable	(),
		BFC::DObject	(),
		BFC::DL::Object	( pOther ),
		BFC::IO::Serializable	(),
		Frame		( pOther ),
		frmeData	( pOther.frmeData ),
		unitOffs	( pOther.unitOffs ) {
	}

	virtual BFC::SPtr clone(
	) const;

	void setData(
		const	BFC::Buffer &	pFrmeData
	) {
		frmeData = pFrmeData;
	}

	void resetData(
	) {
		frmeData.kill();
	}

	BFC::Bool hasData(
	) const {
		return ( ! frmeData.isEmpty() );
	}

	const BFC::Buffer & getData(
	) const {
		return frmeData;
	}

	/// \brief Sets the unit offset to \a pUnitOffs.
	///
	/// \param pUnitOffs
	///	The new unit offset of this frame, expressed as a number
	///	of units, NOT a number of bytes!!!

	void setUnitOffset(
		const	BFC::Uint64	pUnitOffs
	) {
		unitOffs = pUnitOffs;
	}

	/// \brief Resets the unit offset of this frame to the NoOffset value.

	void resetUnitOffset(
	) {
		unitOffs = Frame::NoOffset;
	}

	/// \brief Returns true iff the unit offset has been set.

	BFC::Bool hasUnitOffset(
	) const {
		return ( unitOffs != Frame::NoOffset );
	}

	/// \brief Returns the unit offset of this frame.

	BFC::Uint64 getUnitOffset(
	) const {
		return unitOffs;
	}

	virtual BFC::Uint32 getLength(
	) const;

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	BFC::Buffer		frmeData;	///< Frame payload.
	BFC::Uint64		unitOffs;	///< unit offset.

	/// \brief Forbidden copy operator.

	BytesFrame & operator = (
		const	BytesFrame &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_BytesFrame_H_

// ============================================================================

