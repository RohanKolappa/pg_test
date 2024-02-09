// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_MuxedFrame_H_
#define _VMP_MuxedFrame_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL( VMP_Libs_Base_DLL, MuxedFrame )

} // namespace VMP

// ============================================================================

#include "VMP.Frame.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief Data unit of a MuxedSession.
///
/// A MuxedFrame encapsulates a generic Frame, and tags it with an index, which
/// is equal to the Session index this generic Frame belongs to.
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL MuxedFrame : public Frame {

public :

	/// \brief Unique DL::Type object identifying this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	enum {
		NoIndex = ( BFC::Uint32 )-1	///< Invalid session index.
	};

	/// \brief Creates a new MuxedFrame, not encapsulating any sub Frame.

	MuxedFrame(
	) :
		BFC::DL::Object	( getClassType() ),
		index		( NoIndex ) {
	}

	/// \brief Creates a new MuxedFrame, encapsulating Frame \a pFrame,
	///	which belongs to the Session whose index is \a pIndex.

	MuxedFrame(
		const	BFC::Uint32	pIndex,
		const	FramePtr &	pFrame
	) :
		BFC::DL::Object	( getClassType() ),
		index		( pIndex ),
		frame		( pFrame )
	{
		setPTS( pFrame->getPTS() );
		setDTS( pFrame->getDTS() );
	}

	/// \brief Copy constructor.

	MuxedFrame(
		const	MuxedFrame &	pOther
	) :
		SObject		(),
		Cloneable	(),
		DObject		(),
		BFC::DL::Object	( pOther ),
		Serializable	(),
		Frame		( pOther ),
		index		( pOther.index ),
		frame		( pOther.frame->clone() )
	{
	}

	virtual BFC::SPtr clone(
	) const;

	/// \brief Returns the index of the Session the encapsulated Frame
	///	belongs to.

	BFC::Uint32 getSubIndex(
	) const {
		return index;
	}

	/// \brief Returns the encapsulated Frame.

	FramePtr getSubFrame(
	) const {
		return frame;
	}

	/// \brief Sets the encapsulated Frame.

	void setSubFrame(
			FramePtr	pFrame
	) {
		frame = pFrame;
	}

	/// \brief Returns the length of this MuxedFrame, which is always 1.

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

	BFC::Uint32	index;	///< Sub Session index.
	FramePtr	frame;	///< Encapsulated frame.

	/// \brief Forbidden copy operator.

	MuxedFrame & operator = (
		const	MuxedFrame &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_MuxedFrame_H_

// ============================================================================

