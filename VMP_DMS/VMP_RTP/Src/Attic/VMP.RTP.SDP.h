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
//	VMP.RTP.SDP.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_SDP_H_
#define _VMP_RTP_SDP_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.SDPPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "BFC.Net.SockAddr.h"

#include "BFC.TL.Map.h"

#include "VMP.BytesSessionPtr.h"
#include "VMP.MuxedStreamPtr.h"
#include "VMP.StreamPtr.h"

#include "VMP.RTP.THandlerPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL SDP : virtual public BFC::DObject {

public :

	/// \brief Creates a new SDP.

	SDP(
	);

	BytesSessionPtr decode(
		const	BFC::Buffer &	pData
	);

	BFC::Uint32 getNbrEntries(
	) const {
		return entrList.getSize();
	}

	BFC::Net::SockAddrCPtr getSockAddr(
		const	BFC::Uint32	pIndex
	) const {
		return new BFC::Net::SockAddr(
				entrList.getValue( pIndex )->addr,
				entrList.getValue( pIndex )->port );
	}

protected :

	void decodeLine(
		const	BFC::Buffer &	message
	);

	void decodeMLine(
		const	BFC::Buffer &	content
	);

	void decodeALine(
		const	BFC::Buffer &	content
	);

	StreamPtr makeStream(
		const	BFC::Buffer &	pMetaType	// "video", "audio", ...
	) const;

	THandlerPtr decodeType(
		const	BFC::Buffer &	pMetaType,	// "video", "audio", ...
		const	BFC::Uint32	pDataType,	// 96, ...
			StreamPtr	pCurrStrm
	) const;

	THandlerPtr decodeType(
		const	BFC::Buffer &	pMetaType,	// "video", "audio", ...
		const	BFC::Buffer &	pMimeType,	// "H264/90000"
			StreamPtr	pCurrStrm
	) const;

private :

	BFC_PTR_DECL( VMP_RTP_DLL, Entry )

	class VMP_RTP_DLL Entry : virtual public BFC::SObject {
	public :
		BFC::Buffer		meta;	// "video", "audio", ...
		BFC::Uint16		port;	// 0, ...
		BFC::Buffer		prfl;	// "RTP/AVP"
		BFC::Uint32		type;	// 96
		StreamPtr		strm;	//
		THandlerPtr	phlp;	// Helper...
		BFC::Net::IPAddrCPtr	addr;
	};

	typedef BFC::Map< BFC::Uint32, EntryPtr >	EntryMap;

	BytesSessionPtr		oBytSess;
	MuxedStreamPtr		oMuxStrm;

	EntryMap		entrList;
	EntryPtr		currEntr;

	/// \brief Forbidden copy constructor.

	SDP(
		const	SDP &
	);

	/// \brief Forbidden copy operator.

	SDP & operator = (
		const	SDP &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_SDP_H_

// ============================================================================

