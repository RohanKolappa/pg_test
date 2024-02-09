// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Frame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_Frame_H_
#define _VMP_Frame_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.FramePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Cloneable.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "BFC.IO.Serializable.h"

#include "BFC.Image.Picture.h"

#include "BFC.Time.Clock.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief Base class of all objects that get transferred between entities
///	in a Session.
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL Frame :

	virtual public BFC::Cloneable,
	virtual public BFC::DL::Object,
	virtual public BFC::IO::Serializable {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	static const BFC::Uint64 NoOffset;

protected :

	Frame(
	) :
		BFC::DL::Object	( getClassType() ),
		frmeOffs	( NoOffset ),
		keyFrame	( false ),
		segmStrt	( false ),
		dataDisc	( false ),
		timeDisc	( false )
	{
	}

	Frame(
			FrameCPtr	pTmplFrme
	) :
		BFC::DL::Object	( getClassType() ),
		frmeOffs	( NoOffset ),
		framePts	( pTmplFrme->framePts ),
		frameDts	( pTmplFrme->frameDts ),
		keyFrame	( pTmplFrme->keyFrame ),
		segmStrt	( pTmplFrme->segmStrt ),
		dataDisc	( pTmplFrme->dataDisc ),
		timeDisc	( pTmplFrme->timeDisc )
	{
	}

	Frame(
		const	BFC::Uint32	pFrmeOffs,
		const	BFC::Time::Clock &
					pFramePts,
		const	BFC::Time::Clock &
					pFrameDts,
		const	BFC::Bool	pIsKeyFrm,
		const	BFC::Bool	pSegmStrt
	) :
		BFC::DL::Object	( getClassType() ),
		frmeOffs	( pFrmeOffs ),
		framePts	( pFramePts ),
		frameDts	( pFrameDts ),
		keyFrame	( pIsKeyFrm ),
		segmStrt	( pSegmStrt ),
		dataDisc	( false ),
		timeDisc	( false )
	{
	}

	Frame(
		const	Frame &		pOther
	) :
		SObject		(),
		Cloneable	(),
		DObject		(),
		BFC::DL::Object	( pOther ),
		Serializable	(),
		frmeOffs	( pOther.frmeOffs ),
		framePts	( pOther.framePts ),
		frameDts	( pOther.frameDts ),
		keyFrame	( pOther.keyFrame ),
		segmStrt	( pOther.segmStrt ),
		dataDisc	( pOther.dataDisc ),
		timeDisc	( pOther.timeDisc )
	{
	}

public :

	/// \brief Sets the current frame offset.

	void setOffset(
		const	BFC::Uint64	pFrmeOffs
	) {
		frmeOffs = pFrmeOffs;
	}

	/// \brief Returns true iff the frame offset has already been set.

	BFC::Bool hasOffset(
	) const {
		return ( frmeOffs != NoOffset );
	}

	/// \brief Returns the current frame offset.

	BFC::Uint64 getOffset(
	) const {
		return frmeOffs;
	}

	/// Resets the current frame offset to NoOffset.

	void resetOffset(
	) {
		frmeOffs = NoOffset;
	}

	const BFC::Time::Clock & getPTS(
	) const {
		return framePts;
	}

	void setPTS(
		const	BFC::Time::Clock &
					pFramePts
	) {
		framePts = pFramePts;
		if ( ! frameDts ) {
			frameDts = pFramePts;
		}
	}

	const BFC::Time::Clock & getDTS(
	) const {
		return frameDts;
	}

	void setDTS(
		const	BFC::Time::Clock &
					pFrameDts
	) {
		frameDts = pFrameDts;
	}

	void setKeyFrame(
		const	BFC::Bool	pKeyFrame
	) {
		keyFrame = pKeyFrame;
	}

	BFC::Bool isKeyFrame(
	) const {
		return keyFrame;
	}

	void setSegmentStart(
		const	BFC::Bool	pSegmStrt
	) {
		segmStrt = pSegmStrt;
	}

	BFC::Bool isSegmentStart(
	) const {
		return segmStrt;
	}

	BFC::Bool hasDataDisc(
	) const {
		return dataDisc;
	}

	void setDataDisc(
		const	BFC::Bool	pDataDisc = true
	) {
		dataDisc = pDataDisc;
	}

	BFC::Bool hasTimeDisc(
	) const {
		return timeDisc;
	}

	void setTimeDisc(
		const	BFC::Bool	pTimeDisc = true
	) {
		timeDisc = pTimeDisc;
	}

	BFC::Bool hasDisc(
	) const {
		return ( dataDisc || timeDisc );
	}

	virtual BFC::Uint32 getLength(
	) const = 0;

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

	BFC::Uint64		frmeOffs;
	BFC::Time::Clock	framePts;	///< Presentation time stamp.
	BFC::Time::Clock	frameDts;	///< Decoding time stamp.
	BFC::Bool		keyFrame;
	BFC::Bool		segmStrt;	///< Start of segment ?
	BFC::Bool		dataDisc;	///< Data discontinuity.
	BFC::Bool		timeDisc;	///< Time discontinuity.

	Frame & operator = (
		const	Frame &		pOther
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_Frame_H_

// ============================================================================

#include "VMP.AudioFrame.h"
#include "VMP.BytesFrame.h"
#include "VMP.VideoFrame.h"

// ============================================================================

