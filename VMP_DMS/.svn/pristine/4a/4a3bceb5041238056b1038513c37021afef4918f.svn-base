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
//	VMP.RTP.SEStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_SEStream_H_
#define _VMP_RTP_SEStream_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.SEStreamPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Frac64.h"
#include "BFC.Base.SObject.h"

#include "VMP.StreamPtr.h"

#include "VMP.RTP.NSessionPtr.h"
#include "VMP.RTP.SBandwidthPtr.h"
#include "VMP.RTP.SConnInfoPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL SEStream : virtual public BFC::SObject {

public :

	/// \brief Creates a new SEStream.

	SEStream(
	) :
		strmPort	( 0 ),
		nbrPorts	( 0 ),
		dataType	( ( BFC::Uint32 )-1 )
	{
	}

	/// \brief Creates a new SEStream.

	SEStream(
		const	BFC::Buffer &	pMetaType,
		const	BFC::Uint16	pStrmPort,
		const	BFC::Uint32	pNbrPorts,
		const	BFC::Buffer &	pStrmPrfl,
		const	BFC::Uint32	pDataType
	) :
		metaType	( pMetaType ),
		strmPort	( pStrmPort ),
		nbrPorts	( pNbrPorts ),
		strmPrfl	( pStrmPrfl ),
		dataType	( pDataType )
	{
	}

	const BFC::Buffer & getMetaType(
	) const {
		return metaType;
	}

	void setMetaType(
		const	BFC::Buffer &	pMetaType
	) {
		metaType = pMetaType;
	}

	BFC::Uint16 getStreamPort(
	) const {
		return strmPort;
	}

	BFC::Uint32 getNbrPorts(
	) const {
		return nbrPorts;
	}

	const BFC::Buffer & getStreamProfile(
	) const {
		return strmPrfl;
	}

	BFC::Uint32 getDataType(
	) const {
		return dataType;
	}

	void setDataType(
		const	BFC::Uint32	pDataType
	) {
		dataType = pDataType;
	}

	BFC::Bool hasEStreamInfo(
	) const {
		return strmInfo.hasContent();
	}

	const BFC::Buffer & getEStreamInfo(
	) const {
		return strmInfo;
	}

	void setEStreamInfo(
		const	BFC::Buffer &	pInfo
	) {
		strmInfo = pInfo;
	}

	BFC::Bool hasConnectionInfo(
	) const {
		return ( BFC::Bool )connInfo;
	}

	SConnInfoCPtr getConnectionInfo(
	) const {
		return connInfo;
	}

	void setConnectionInfo(
			SConnInfoCPtr	pInfo
	) {
		connInfo = pInfo;
	}

	BFC::Bool hasBandwidth(
	) const {
		return ( BFC::Bool )( bandwdth );
	}

	SBandwidthCPtr getBandwidth(
	) const {
		return bandwdth;
	}

	void setBandwidth(
			SBandwidthCPtr	pInfo
	) {
		bandwdth = pInfo;
	}

	BFC::Bool hasEncoding(
	) const {
		return strmCodc.hasContent();
	}

	const BFC::Buffer & getEncoding(
	) const {
		return strmCodc;
	}

	void setEncoding(
		const	BFC::Buffer &	pInfo
	) {
		strmCodc = pInfo;
	}

	BFC::Bool hasFormat(
	) const {
		return strmFrmt.hasContent();
	}

	const BFC::Buffer & getFormat(
	) const {
		return strmFrmt;
	}

	void setFormat(
		const	BFC::Buffer &	pInfo
	) {
		strmFrmt = pInfo;
	}

	BFC::Bool hasFrameRate(
	) const {
		return ( frmeRate.toInt64() != 0 );
	}

	const BFC::Frac64 & getFrameRate(
	) const {
		return frmeRate;
	}

	void setFrameRate(
		const	BFC::Frac64 &	pFrmeRate
	) {
		frmeRate = pFrmeRate;
	}

	BFC::Buffer toBuffer(
	) const;

	StreamPtr toStream(
	) const;

protected :

private :

	BFC::Buffer	metaType;	///< EStream type (e.g. "video", "audio", ...).
	BFC::Uint16	strmPort;	///< EStream port (e.g. 0).
	BFC::Uint32	nbrPorts;	///< Number of ports (for multi-layered multicast).
	BFC::Buffer	strmPrfl;	///< EStream profile (e.g. "RTP/AVP").
	BFC::Uint32	dataType;	///< EStream data type (e.g. 96).
	BFC::Buffer	strmInfo;	///< EStream information (optional).
	SConnInfoCPtr	connInfo;	///< Connection information (optional).
	SBandwidthCPtr	bandwdth;	///< Bandwidth (optional).
	BFC::Buffer	strmCodc;	///< Encoding (e.g. "MP4V-ES/90000").
	BFC::Buffer	strmFrmt;	///< Format (e.g. "profile-level-id=245; config=000001B0F5000001B50900000100000001200088401928B02240A21F;").
	BFC::Frac64	frmeRate;	///< Video frame rate.

	/// \brief Forbidden copy constructor.

	SEStream(
		const	SEStream &
	);

	/// \brief Forbidden copy operator.

	SEStream & operator = (
		const	SEStream &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_SEStream_H_

// ============================================================================

