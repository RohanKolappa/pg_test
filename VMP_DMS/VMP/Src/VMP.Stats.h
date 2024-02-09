// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.Stats.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_Stats_H_
#define _VMP_Stats_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL( VMP_Libs_Base_DLL, StatsFrame )
BFC_PTR_DECL( VMP_Libs_Base_DLL, StatsSession )
BFC_PTR_DECL( VMP_Libs_Base_DLL, StatsStream )

} // namespace VMP

// ============================================================================

#include "BFC.Base.UUId.h"

#include "VMP.Frame.h"
#include "VMP.Session.h"
#include "VMP.ElementaryStream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL StatsFrame : public Frame {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	StatsFrame(
	) :
		BFC::DL::Object	( getClassType() ),
		v		( 0 ) {
	}

	StatsFrame(
		const	StatsFrame &	pOther
	) :
		SObject		(),
		Cloneable	(),
		DObject		(),
		BFC::DL::Object	( pOther ),
		Serializable(),
		Frame		( pOther ),
		v		( pOther.v ) {
	}

	virtual BFC::SPtr clone(
	) const;

	// Temp API...

	void setValue(
		const	BFC::Double	i
	) {
		v = i;
	}

	BFC::Double getValue(
	) const {
		return v;
	}

	virtual BFC::Uint32 getLength(
	) const {
		return 1;
	}

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

private :

	BFC::Double	v;

	StatsFrame & operator = (
		const	StatsFrame &
	);

};

// ============================================================================

class VMP_Libs_Base_DLL StatsStream : public ElementaryStream {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	StatsStream(
	);

	StatsStream(
		const	StatsStream &	o
	);

	StatsStream& operator = (
		const	StatsStream&
	);

	virtual StreamPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			StreamCPtr	pStream
	) const;

//	virtual BFC::Buffer toBuffer(
//	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

private :

};

// ============================================================================

class VMP_Libs_Base_DLL StatsSession : public Session {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	StatsSession(
		const	Flags		f = Seekable
	);

	StatsSession(
		const	StatsSession &	pOther
	);

	virtual SessionPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			SessionCPtr	pSession
	) const;

	StatsStreamPtr getContent(
	) const {
		return content;
	}

	void setContent(
			StatsStreamPtr	content
	) {
		this->content = content;
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

	// internal state...

	StatsStreamPtr		content;

	// others...

	StatsSession& operator = (
		const	StatsSession&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_Stats_H_

// ============================================================================

