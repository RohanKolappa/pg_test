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
//	VMP.RTP.SProgram.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_SProgram_H_
#define _VMP_RTP_SProgram_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.SProgramPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.Array.h"

#include "VMP.RTP.SBandwidthPtr.h"
#include "VMP.RTP.SConnInfoPtr.h"
#include "VMP.RTP.SEStreamPtr.h"
#include "VMP.RTP.SOriginPtr.h"
#include "VMP.RTP.STimeInfoPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL SProgram : virtual public BFC::SObject {

public :

	/// \brief Creates a new SProgram.

	SProgram(
	);

	BFC::Bool hasOrigin(
	) const {
		return ( BFC::Bool )progOrig;
	}

	SOriginCPtr getOrigin(
	) const {
		return progOrig;
	}

	void setOrigin(
			SOriginCPtr	pOrigin
	) {
		progOrig = pOrigin;
	}

	BFC::Bool hasTitle(
	) const {
		return progTtle.hasContent();
	}

	const BFC::Buffer & getTitle(
	) const {
		return progTtle;
	}

	void setTitle(
		const	BFC::Buffer &	pTitle
	) {
		progTtle = pTitle;
	}

	BFC::Bool hasProgramInfo(
	) const {
		return progInfo.hasContent();
	}

	const BFC::Buffer & getProgramInfo(
	) const {
		return progInfo;
	}

	void setProgramInfo(
		const	BFC::Buffer &	pInfo
	) {
		progInfo = pInfo;
	}

	BFC::Bool hasDescriptionURI(
	) const {
		return progDesc.hasContent();
	}

	const BFC::Buffer & getDescriptionURI(
	) const {
		return progDesc;
	}

	void setDescriptionURI(
		const	BFC::Buffer &	pDesc
	) {
		progDesc = pDesc;
	}

	BFC::Bool hasEmail(
	) const {
		return progMail.hasContent();
	}

	const BFC::Buffer & getEmail(
	) const {
		return progMail;
	}

	void setEmail(
		const	BFC::Buffer &	pMail
	) {
		progMail = pMail;
	}

	BFC::Bool hasPhone(
	) const {
		return progPhne.hasContent();
	}

	const BFC::Buffer & getPhone(
	) const {
		return progPhne;
	}

	void setPhone(
		const	BFC::Buffer &	pPhne
	) {
		progPhne = pPhne;
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
		return ( BFC::Bool )bandwdth;
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

	BFC::Bool hasTimeInfo(
	) const {
		return ( BFC::Bool )timeInfo;
	}

	STimeInfoCPtr getTimeInfo(
	) const {
		return timeInfo;
	}

	void setTimeInfo(
			STimeInfoCPtr	pInfo
	) {
		timeInfo = pInfo;
	}

	BFC::Uint32 getNbrSEStreams(
	) const {
		return strmList.getSize();
	}

	SEStreamCPtr getSEStream(
		const	BFC::Uint32	pIndex
	) const {
		return strmList[ pIndex ];
	}

	SEStreamPtr getSEStream(
		const	BFC::Uint32	pIndex
	) {
		return strmList[ pIndex ];
	}

	SEStreamPtr popSEStream(
		const	BFC::Uint32	pIndex
	) {
		return strmList.pop( pIndex );
	}

	void addSEStream(
			SEStreamPtr	pEStream
	);

	SEStreamCPtr findEStreamByMetaType(
		const	BFC::Buffer &	pMetaType
	) const;

	BFC::Buffer toBuffer(
	) const;

protected :

private :

	SOriginCPtr	progOrig;	///< Origin (mandatory - 'o' field).
	BFC::Buffer	progIdnt;	///< Session identifier (mandatory - 'o' field).
	BFC::Buffer	progTtle;	///< Session name (mandatory - 's' field).
	BFC::Buffer	progInfo;	///< Session information (optional - 'i' field).
	BFC::Buffer	progDesc;	///< Description URI (optional - 'u' field).
	BFC::Buffer	progMail;	///< Email address (optional - 'e' field).
	BFC::Buffer	progPhne;	///< Phone number (optional - 'p' field).
	SConnInfoCPtr	connInfo;	///< Connection information (optional - 'c' field).
	SBandwidthCPtr	bandwdth;	///< Bandwidth (optional - 'b' field).
	STimeInfoCPtr	timeInfo;	///< Time information (optional - 't' field).

	BFC::Array< SEStreamPtr >	strmList;

	/// \brief Forbidden copy constructor.

	SProgram(
		const	SProgram &
	);

	/// \brief Forbidden copy operator.

	SProgram & operator = (
		const	SProgram &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_SProgram_H_

// ============================================================================

