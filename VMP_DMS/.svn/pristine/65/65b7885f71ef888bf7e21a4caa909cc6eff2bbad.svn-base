// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.DemuxerFilter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_DemuxerFilter_H_
#define _VMP_L2_DemuxerFilter_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.DemuxerFilterPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "VMP.MuxedSession.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all classes that can select sub sessions in a
///	MuxedSession, based on some criteria.
///
/// \ingroup VMP_L2

class VMP_L2_DLL DemuxerFilter : virtual public BFC::SObject {

public :

	static const BFC::Uint32	NoSessionId	= ( BFC::Uint32 )-1;

	/// \brief Selects a sub Session in a MuxedSession.
	///
	/// \param pMSession
	///	The session to filter out.
	///
	/// \returns
	///	The session id (first is 0) that matches the specific
	///	criteria, or NoSessionId if no match was found.

	virtual BFC::Uint32 filter(
			MuxedSessionCPtr	pMSession
	) = 0;

};

// ============================================================================

/// \brief Filter that never matches any sub Session.
///
/// \ingroup VMP_L2

class VMP_L2_DLL StreamSkipFilter : public DemuxerFilter {

public :

	virtual BFC::Uint32 filter(
			MuxedSessionCPtr	pMSession
	);

};

// ============================================================================

/// \brief Filter selecting a sub session, based on its stream title.
///
/// \ingroup VMP_L2

class VMP_L2_DLL StreamTitleFilter : public DemuxerFilter {

public :

	StreamTitleFilter(
		const	BFC::Buffer &		pStreamTitle
	) :
		streamTitle( pStreamTitle ) {
	}

	virtual BFC::Uint32 filter(
			MuxedSessionCPtr	pMSession
	);

private :

	BFC::Buffer	streamTitle;

};

// ============================================================================

/// \brief Filter selecting a sub session, based on the stream type.
///
/// \ingroup VMP_L2

class VMP_L2_DLL StreamClassFilter : public DemuxerFilter {

public :

	StreamClassFilter(
		const	BFC::DL::TypeCPtr &	pStreamClass,
		const	BFC::Uint32		pStreamIndex
	) :
		streamClass( pStreamClass ),
		streamIndex( pStreamIndex ) {
	}

	virtual BFC::Uint32 filter(
			MuxedSessionCPtr	pMSession
	);

private :

	BFC::DL::TypeCPtr	streamClass;
	BFC::Uint32		streamIndex;

};

// ============================================================================

/// \brief Filter selecting a sub session, based on the session type.
///
/// \ingroup VMP_L2

class VMP_L2_DLL SessionClassFilter : public DemuxerFilter {

public :

	SessionClassFilter(
		const	BFC::DL::TypeCPtr &	pSessionClass,
		const	BFC::Uint32		pSessionIndex
	) :
		sessionClass( pSessionClass ),
		sessionIndex( pSessionIndex ) {
	}

	virtual BFC::Uint32 filter(
			MuxedSessionCPtr	pMSession
	);

private :

	BFC::DL::TypeCPtr	sessionClass;
	BFC::Uint32		sessionIndex;

};

// ============================================================================

/// \brief Filter selecting a sub session, based on the stream id.
///
/// \ingroup VMP_L2

class VMP_L2_DLL StreamIdFilter : public DemuxerFilter {

public :

	StreamIdFilter(
		const	BFC::Uint32		pStreamId
	) :
		streamId( pStreamId ) {
	}

	virtual BFC::Uint32 filter(
			MuxedSessionCPtr	pMSession
	);

private :

	BFC::Uint32		streamId;

};

// ============================================================================

/// \brief Filter selecting a sub session, based on the sub index.
///
/// \ingroup VMP_L2

class VMP_L2_DLL SubIndexFilter : public DemuxerFilter {

public :

	SubIndexFilter(
		const	BFC::Uint32		pSubIndex
	) :
		subIndex( pSubIndex ) {
	}

	virtual BFC::Uint32 filter(
			MuxedSessionCPtr	pMSession
	);

private :

	BFC::Uint32		subIndex;

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_DemuxerFilter_H_

// ============================================================================

