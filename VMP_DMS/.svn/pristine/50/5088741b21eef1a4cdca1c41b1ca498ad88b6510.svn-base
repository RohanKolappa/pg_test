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
//	BFC.Net.URI.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#include "BFC.Net.URI.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, URI )

// ============================================================================

void Net::URI::setScheme(
	const	Buffer &	pScheme ) {

	scheme = pScheme.toLower();
	schmId = SchemeDB::instance().getId( scheme );

}

void Net::URI::setSchemeFromURI(
	const	Buffer &	pURI ) {

	Uint32	pos = pURI.findFwd( ':' );

	if ( pos >= pURI.getLength() ) {
		throw InvalidArgument( "No scheme in URI!" );
	}

	setScheme( pURI( 0, pos ) );

}

// ============================================================================

void Net::URI::kill() {

	scheme.kill();
	schmId = SchemeUnknown;

}

// ============================================================================

Buffer Net::URI::percentEncode(
	const	Buffer &	pStr,
	const	Buffer &	pChars,
	const	Bool		encode_percents ) {

	Buffer		res;
	Uint32		len = pStr.getLength();
	const Uchar *	ptr = pStr.getCstPtr();

	while ( len-- ) {
		Uchar c = *ptr++;
		if ( ( encode_percents && c == '%' )
		  || c < ' '
		  || c > '~'
		  || pChars.contains( c ) ) {
			res += '%';
			res += valToDigit( ( c >> 4 ) & 0x0F );
			res += valToDigit( ( c      ) & 0x0F );
		}
		else {
			res += c;
		}
	}

	return res;

}

Buffer Net::URI::percentDecode(
	const	Buffer &	pStr ) {

	Buffer		res;
	Uint32		len = pStr.getLength();
	const Uchar *	ptr = pStr.getCstPtr();

	res.supposedMaxSize( len );

	while ( len-- ) {
		Uchar c = *ptr++;
		if ( c == '%'
		  && len >= 2
		  && isHexDigit( ptr[ 1 ] )
		  && isHexDigit( ptr[ 2 ] ) ) {
			res += ( digitToVal( ptr[ 1 ] ) << 4
			       | digitToVal( ptr[ 2 ] )      );
			ptr += 2;
		}
		else {
			res += c;
		}
	}

	return res;

}

// ============================================================================

static const Buffer NPT_URI_ALWAYS_ENCODE = " !\"<>\\^`{|}";

const Buffer Net::URI::PathCharsToEncode = NPT_URI_ALWAYS_ENCODE + "?#[]";
const Buffer Net::URI::QueryCharsToEncode = NPT_URI_ALWAYS_ENCODE + "#[]";
const Buffer Net::URI::FragmentCharsToEncode = NPT_URI_ALWAYS_ENCODE + "[]";
const Buffer Net::URI::UnsafeCharsToEncode = NPT_URI_ALWAYS_ENCODE;

// ============================================================================
// BFC::Net::URI::SchemeDB
// ============================================================================

const Net::URI::SchemeDB::Entry Net::URI::SchemeDB::tbl[] = {
	{ SchemeAAA,		"aaa",		"Diameter Protocol",						"[RFC3588]" },
	{ SchemeAAAS,		"aaas",		"Diameter Protocol with Secure Transport",			"[RFC3588]" },
	{ SchemeACAP,		"acap",		"application configuration access protocol",			"[RFC2244]" },
	{ SchemeCAP,		"cap",		"Calendar Access Protocol",					"[RFC4324]" },
	{ SchemeCID,		"cid",		"content identifier",						"[RFC2392]" },
	{ SchemeCRID,		"crid",		"TV-Anytime Content Reference Identifier",			"[RFC4078]" },
	{ SchemeData,		"data",		"data",								"[RFC2397]" },
	{ SchemeDAV,		"dav",		"dav",								"[RFC4918]" },
	{ SchemeDict,		"dict",		"dictionary service protocol",					"[RFC2229]" },
	{ SchemeDNS,		"dns",		"Domain Name System",						"[RFC4501]" },
	{ SchemeFax,		"fax",		"fax (historical, see [RFC3966])",				"[RFC2806]" },
	{ SchemeFile,		"file",		"Host-specific file names",					"[RFC1738]" },
	{ SchemeFTP,		"ftp",		"File Transfer Protocol	",					"[RFC1738]" },
	{ SchemeGo,		"go",			"go",								"[RFC3368]" },
	{ SchemeGopher,		"gopher",		"The Gopher Protocol",						"[RFC4266]" },
	{ SchemeH323,		"h323",		"H.323",							"[RFC3508]" },
	{ SchemeHTTP,		"http",		"Hypertext Transfer Protocol",					"[RFC2616]" },
	{ SchemeHTTPS,		"https",		"Hypertext Transfer Protocol Secure",				"[RFC2818]" },
	{ SchemeIAX,		"iax",		"Inter-Asterisk eXchange Version 2",				"[RFC-guy-iax-05.txt]" },
	{ SchemeICAP,		"icap",		"Internet Content Adaptation Protocol",				"[RFC3507]" },
	{ SchemeIM,		"im",			"Instant Messaging",						"[RFC3860]" },
	{ SchemeIMAP,		"imap",		"internet message access protocol",				"[RFC5092]" },
	{ SchemeInfo,		"info",		"Information Assets with Identifiers in Public Namespaces",	"[RFC4452]" },
	{ SchemeIPP,		"ipp",		"Internet Printing Protocol",					"[RFC3510]" },
	{ SchemeIRIS,		"iris",		"Internet Registry Information Service",			"[RFC3981]" },
	{ SchemeIRISbeep,	"iris.beep",		"iris.beep",							"[RFC3983]" },
	{ SchemeIRISxpc,	"iris.xpc",		"iris.xpc",							"[RFC4992]" },
	{ SchemeIRISxpcs,	"iris.xpcs",		"iris.xpcs",							"[RFC4992]" },
	{ SchemeIRISlwz,	"iris.lwz",		"iris.lwz",							"[RFC4993]" },
	{ SchemeLDAP,		"ldap",		"Lightweight Directory Access Protocol",			"[RFC4516]" },
	{ SchemeMailto,		"mailto",		"Electronic mail address",					"[RFC2368]" },
	{ SchemeMID,		"mid",		"message identifier",						"[RFC2392]" },
	{ SchemeModem,		"modem",		"modem (historical, see [RFC3966])",				"[RFC2806]" },
	{ SchemeMSRP,		"msrp",		"Message Session Relay Protocol",				"[RFC4975]" },
	{ SchemeMSRPS,		"msrps",		"Message Session Relay Protocol Secure",			"[RFC4975]" },
	{ SchemeMTQP,		"mtqp",		"Message Tracking Query Protocol",				"[RFC3887]" },
	{ SchemeMUpdate,	"mupdate",		"Mailbox Update (MUPDATE) Protocol",				"[RFC3656]" },
	{ SchemeNews,		"news",		"USENET news",							"[RFC-ellermann-news-nntp-uri-11.txt]" },
	{ SchemeNFS,		"nfs",		"network file system protocol",					"[RFC2224]" },
	{ SchemeNNTP,		"nntp",		"USENET news using NNTP access",				"[RFC-ellermann-news-nntp-uri-11.txt]" },
	{ SchemeOpaqueLockToken,"opaquelocktoken",	"opaquelocktokent",						"[RFC4918]" },
	{ SchemePOP,		"pop",		"Post Office Protocol v3",					"[RFC2384]" },
	{ SchemePres,		"pres",		"Presence",							"[RFC3859]" },
	{ SchemeRTSP,		"rtsp",		"real time streaming protocol",					"[RFC2326]" },
	{ SchemeService,	"service",		"service location",						"[RFC2609]" },
	{ SchemeSHTTP,		"shttp",		"Secure Hypertext Transfer Protocol",				"[RFC2660]" },
	{ SchemeSieve,		"sieve",		"ManageSieve Protocol",						"[RFC-ietf-sieve-managesieve-09.txt]" },
	{ SchemeSIP,		"sip",		"session initiation protocol",					"[RFC3261]" },
	{ SchemeSIPS,		"sips",		"secure session initiation protocol",				"[RFC3261]" },
	{ SchemeSNMP,		"snmp",		"Simple Network Management Protocol",				"[RFC4088]" },
	{ SchemeSoapBeep,	"soap.beep",		"soap.beep",							"[RFC4227]" },
	{ SchemeSoapBeeps,	"soap.beeps",		"soap.beeps",							"[RFC4227]" },
	{ SchemeTag,		"tag",		"tag",								"[RFC4151]" },
	{ SchemeTel,		"tel",		"telephone",							"[RFC3966]" },
	{ SchemeTelnet,		"telnet",		"Reference to interactive sessions",				"[RFC4248]" },
	{ SchemeTFTP,		"tftp",		"Trivial File Transfer Protocol",				"[RFC3617]" },
	{ SchemeThisMessage,	"thismessage",	"multipart/related relative reference resolution",		"[RFC2557]" },
	{ SchemeTIP,		"tip",		"Transaction Internet Protocol",				"[RFC2371]" },
	{ SchemeTV,		"tv",			"TV Broadcasts",						"[RFC2838]" },
	{ SchemeURN,		"urn",		"Uniform Resource Names (click for registry)",			"[RFC2141]" },
	{ SchemeVEMMI,		"vemmi",		"versatile multimedia interface",				"[RFC2122]" },
	{ SchemeXMLRPCBeep,	"xmlrpc.beep",	"xmlrpc.beep",							"[RFC3529]" },
	{ SchemeXMLRPCBeeps,	"xmlrpc.beeps",	"xmlrpc.beeps",							"[RFC3529]" },
	{ SchemeXMPP,		"xmpp",		"Extensible Messaging and Presence Protocol",			"[RFC5122]" },
	{ SchemeZ3950R,		"z39.50r",		"Z39.50 Retrieval",						"[RFC2056]" },
	{ SchemeZ3950S,		"z39.50s",		"Z39.50 Session	",						"[RFC2056]" },
	{ SchemeUnknown, 0, 0, 0 }
};

// ============================================================================

Net::URI::SchemeDB::SchemeDB() {

	for ( const Entry * e = tbl ; e->str ; e++ ) {
		mapRev.add( e->str, e->sid );
		mapStr.add( e->sid, e->str );
		mapTxt.add( e->sid, e->txt );
		mapRFC.add( e->sid, e->rfc );
	}

	mapStr.add( SchemeUnknown, "???" );
	mapTxt.add( SchemeUnknown, "???" );
	mapRFC.add( SchemeUnknown, "???" );

}

// ============================================================================

const Net::URI::SchemeDB & Net::URI::SchemeDB::instance() {

	static SchemeDB i;

	return i;

}

// ============================================================================

const Buffer & Net::URI::SchemeDB::getName(
	const	SchemeId		pSchemeId ) const {

	return mapStr[ pSchemeId ];

}

const Buffer & Net::URI::SchemeDB::getText(
	const	SchemeId		pSchemeId ) const {

	return mapTxt[ pSchemeId ];

}

const Buffer & Net::URI::SchemeDB::getRFC(
	const	SchemeId		pSchemeId ) const {

	return mapRFC[ pSchemeId ];

}

Net::URI::SchemeId Net::URI::SchemeDB::getId(
	const	Buffer &	pSchemeName ) const {

	return mapRev.get( pSchemeName, SchemeUnknown );

}

#endif // 0

// ============================================================================

