// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Session.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_Session_H_
#define _VMP_Session_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.SessionPtr.h"

// ============================================================================

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "BFC.IO.Serializable.h"

#include "VMP.Filter.h"
#include "VMP.Frame.h"
#include "VMP.Index.h"
#include "VMP.Stream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL Session :

	virtual public BFC::DL::Object,
	virtual public BFC::IO::Serializable {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	static const BFC::Uint32 NoIndex;

	enum Flags {
		Live		= 0,
		Pausable,
		Seekable
	};

	Session(
		const	Flags		pFlags
	);

	Session(
			StreamPtr	pStream,
		const	Flags		pFlags
	);

	Session(
		const	Session &	pOther
	);

	virtual SessionPtr clone(
	) const = 0;

	virtual BFC::Bool isCompatibleWith(
			SessionCPtr	pSession
	) const = 0;

	void setLength(
		const	BFC::Uint64	i
	) {
		length = i;
	}

	BFC::Bool hasLength(
	) const {
		return ( length != (BFC::Uint64)0 );
	}

	BFC::Uint64 getLength(
	) const {
		return length;
	}

	void setFlags(
		const	Flags		pFlags
	) {
		flags = pFlags;
	}

	Flags getFlags(
	) const {
		return flags;
	}

	BFC::Buffer getFlagsName(
	) const;

	static BFC::Buffer getFlagsName(
		const	Flags		pFlags
	);

	void setIndex(
		const	BFC::Uint32	pIndex
	) {
		index = pIndex;
	}

	BFC::Bool hasIndex(
	) const {
		return ( index != NoIndex );
	}

	BFC::Uint32 getIndex(
	) const {
		return index;
	}

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

	StreamPtr getStream(
	) {
		return stream;
	}

	StreamCPtr getStream(
	) const {
		return stream;
	}

	BFC::Bool hasStream(
	) const {
		return ( BFC::Bool )stream;
	}

	FilterPtr getFilter(
	) {
		return filter;
	}

	FilterCPtr getFilter(
	) const {
		return filter;
	}

	BFC::Bool hasFilter(
	) const {
		return ( BFC::Bool )filter;
	}

	BFC::Bool doesUseSegmentStart(
	) const {
		return segmStrt;
	}

	void setUseSegmentStart(
		const	BFC::Bool	pSegmStrt
	) {
		segmStrt = pSegmStrt;
	}

protected :

	void setStream(
			StreamPtr	pStream
	) {
		stream = pStream;
	}

	void resetStream(
	) {
		stream.kill();
	}

	void setFilter(
			FilterPtr	pFilter
	) {
		filter = pFilter;
	}

	void resetFilter(
	) {
		filter.kill();
	}

private :

	BFC::Uint64		length;
	Flags			flags;
	BFC::Uint32		index;
	StreamPtr		stream;
	FilterPtr		filter;
	BFC::Bool		segmStrt;	///< Use segment start field in Frame ?

	/// \brief Forbidden default constructor.

	Session(
	);

	/// \brief Forbidden copy operator.

	Session & operator = (
		const	Session &
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_Session_H_

// ============================================================================

#include "VMP.AudioSession.h"
#include "VMP.BytesSession.h"
#include "VMP.VideoSession.h"

// ============================================================================

