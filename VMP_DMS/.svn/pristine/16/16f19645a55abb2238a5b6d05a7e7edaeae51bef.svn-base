// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Stream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_Stream_H_
#define _VMP_Stream_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.StreamPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.FourCC.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "BFC.IO.Serializable.h"

#include "BFC.Image.PictureFormat.h"

#include "BFC.TL.Array.h"

#include "BFC.Time.Clock.h"

#include "VMP.Formats.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL Stream :

	virtual public BFC::DL::Object,
	virtual public BFC::IO::Serializable {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	static const BFC::Uint32 NoStreamId;

	Stream(
	);

	Stream(
		const	Stream &	pOther
	);

	Stream& operator = (
		const	Stream&
	);

	virtual StreamPtr clone(
	) const = 0;

	virtual BFC::Bool isCompatibleWith(
			StreamCPtr	pStream
	) const = 0;

	void setTitle(
		const	BFC::Buffer&	b
	) {
		title = b;
	}

	BFC::Bool hasTitle(
	) const {
		return ( ! title.isEmpty() );
	}

	const BFC::Buffer& getTitle(
	) const {
		return title;
	}

	void setTimeBase(
		const	BFC::Time::Clock &
					pTimeBase
	) {
		timeBase = pTimeBase;
	}

	const BFC::Time::Clock & getTimeBase(
	) const {
		return timeBase;
	}

//	void setStop(
//		const	BFC::Time::Clock&	t
//	) {
//		stopTime = t;
//	}
//
//	const BFC::Time::Clock& getStop(
//	) const {
//		return stopTime;
//	}

	/// \brief Sets the stream id of this Stream.
	///
	/// The stream id is format specific.

	void setStreamId(
		const	BFC::Uint32		pStreamId
	) {
		streamId = pStreamId;
	}

	BFC::Bool hasStreamId(
	) const {
		return ( streamId != NoStreamId );
	}

	BFC::Uint32 getStreamId(
	) const {
		return streamId;
	}

	void setExtraData(
		const	BFC::Buffer &	d
	) {
		extraData = d;
	}

	BFC::Buffer getExtraData(
	) const {
		return extraData;
	}

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

private :

	BFC::Buffer		title;
	BFC::Time::Clock	timeBase;
//	BFC::Time::Clock	stopTime;
	BFC::Uint32		streamId;
	BFC::Buffer		extraData;

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_Stream_H_

// ============================================================================

