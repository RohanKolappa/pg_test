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
//	BFC.Net.URI.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if 0

#ifndef _BFC_Net_URI_H_
#define _BFC_Net_URI_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, URI )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Net

class BFC_Net_DLL URI : virtual public SObject {

public :

	enum SchemeId {
		SchemeUnknown		= 0,

		SchemeAAA,
		SchemeAAAS,
		SchemeACAP,
		SchemeCAP,
		SchemeCID,
		SchemeCRID,
		SchemeData,
		SchemeDAV,
		SchemeDict,
		SchemeDNS,
		SchemeFax,
		SchemeFile,
		SchemeFTP,
		SchemeGo,
		SchemeGopher,
		SchemeH323,
		SchemeHTTP,
		SchemeHTTPS,
		SchemeIAX,
		SchemeICAP,
		SchemeIM,
		SchemeIMAP,
		SchemeInfo,
		SchemeIPP,
		SchemeIRIS,
		SchemeIRISbeep,
		SchemeIRISxpc,
		SchemeIRISxpcs,
		SchemeIRISlwz,
		SchemeLDAP,
		SchemeMailto,
		SchemeMID,
		SchemeModem,
		SchemeMSRP,
		SchemeMSRPS,
		SchemeMTQP,
		SchemeMUpdate,
		SchemeNews,
		SchemeNFS,
		SchemeNNTP,
		SchemeOpaqueLockToken,
		SchemePOP,
		SchemePres,
		SchemeRTSP,
		SchemeService,
		SchemeSHTTP,
		SchemeSieve,
		SchemeSIP,
		SchemeSIPS,
		SchemeSNMP,
		SchemeSoapBeep,
		SchemeSoapBeeps,
		SchemeTag,
		SchemeTel,
		SchemeTelnet,
		SchemeTFTP,
		SchemeThisMessage,
		SchemeTIP,
		SchemeTV,
		SchemeURN,
		SchemeVEMMI,
		SchemeXMLRPCBeep,
		SchemeXMLRPCBeeps,
		SchemeXMPP,
		SchemeZ3950R,
		SchemeZ3950S

	};

	/// \brief Creates a new URI.

	URI(
	) :
		schmId( SchemeUnknown ) {
	}

	/// \brief Creates a copy of \a pOther.

	URI(
		const	URI &		pOther
	) :
		SObject(),
		scheme( pOther.scheme ),
		schmId( pOther.schmId ) {
	}

	/// \brief Copy operator.

	URI & operator = (
		const	URI &		pOther
	) {
		if ( this != &pOther ) {
			scheme = pOther.scheme;
			schmId = pOther.schmId;
		}
		return *this;
	}

	const Buffer & getScheme(
	) const {
		return scheme;
	}

	void setScheme(
		const	Buffer &	pScheme
	);

	void setSchemeFromURI(
		const	Buffer &	pURI
	);

	SchemeId getSchemeId(
	) const {
		return schmId;
	}

	virtual void kill(
	);

	static const Buffer PathCharsToEncode;
	static const Buffer QueryCharsToEncode;
	static const Buffer FragmentCharsToEncode;
	static const Buffer UnsafeCharsToEncode;

	static Buffer percentEncode(
		const	Buffer &	pInputBuffer,
		const	Buffer &	pCharsToEncode,
		const	Bool		pEncodePercent = true
	);

	static Buffer percentDecode(
		const	Buffer &	str
	);

	class BFC_Net_DLL SchemeDB {
	protected :
		SchemeDB(
		);
	public :
		static const SchemeDB & instance(
		);
		const Buffer & getName(
			const	SchemeId	pSchemeId
		) const;
		const Buffer & getText(
			const	SchemeId	pSchemeId
		) const;
		const Buffer & getRFC(
			const	SchemeId	pSchemeId
		) const;
		SchemeId getId(
			const	Buffer &	pSchemeName
		) const;
	private :
		static const struct Entry {
			const SchemeId	sid;
			const Char *	str;
			const Char *	txt;
			const Char *	rfc;
		} tbl[];
		Map< Buffer, SchemeId >	mapRev;
		Map< SchemeId, Buffer >	mapStr;
		Map< SchemeId, Buffer >	mapTxt;
		Map< SchemeId, Buffer >	mapRFC;
	};

protected :

	static SchemeId parseScheme(
		const	Buffer &	pScheme
	);

	static Bool isHexDigit(
		const	Uchar		pChar
	) {
		return ( ( pChar >= '0' && pChar <= '9' )
		      || ( pChar >= 'A' && pChar <= 'F' )
		      || ( pChar >= 'a' && pChar <= 'f' ) );
	}

	static Uchar valToDigit(
		const	Uchar		pChar,
		const	Bool		pUpper = true
	) {
		return ( pChar < 10
			? ( Uchar )( '0' + pChar )
			: pUpper
			? ( Uchar )( 'A' + ( pChar - 10 ) )
			: ( Uchar )( 'a' + ( pChar - 10 ) ) );
	
	}

	static Uchar digitToVal(
		const	Uchar		pChar
	) {
		return ( ( pChar >= 'a' && pChar <= 'f' )
			? ( Uchar )( ( pChar - 'a' ) + 10 )
			: ( pChar >= 'A' && pChar <= 'F' )
			? ( Uchar )( ( pChar - 'A' ) + 10 )
			: ( Uchar )( pChar - '0' ) );
	}

private :

	Buffer		scheme;
	SchemeId	schmId;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_URI_H_

#endif // 0

// ============================================================================

