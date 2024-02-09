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
//	BFC.Net.Authenticator.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#include "BFC.Base.FastCSLocker.h"

#include "BFC.Net.Authenticator.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, Authenticator )

// ============================================================================

Net::PassAuthPtr Net::Authenticator::requestPassAuth(
	const	IPAddrCPtr &	pAddr,
	const	Uint16		pPort,
	const	Buffer &	pProt,
	const	Buffer &	pText,
	const	Buffer &	pSchm ) {

	FastCSLocker	lock( crit );

	reset();

	addr = pAddr;
	port = pPort;
	prot = pProt;
	text = pText;
	schm = pSchm;

	return getPassAuth();

}

Net::PassAuthPtr Net::Authenticator::requestPassAuth(
	const	Buffer &	pHost,
	const	IPAddrCPtr &	pAddr,
	const	Uint16		pPort,
	const	Buffer &	pProt,
	const	Buffer &	pText,
	const	Buffer &	pSchm ) {

	FastCSLocker	lock( crit );

	reset();

	host = pHost;
	addr = pAddr;
	port = pPort;
	prot = pProt;
	text = pText;
	schm = pSchm;

	return getPassAuth();

}

Net::PassAuthPtr Net::Authenticator::requestPassAuth(
	const	Buffer &	pHost,
	const	IPAddrCPtr &	pAddr,
	const	Uint16		pPort,
	const	Buffer &	pProt,
	const	Buffer &	pText,
	const	Buffer &	pSchm,
	const	URL &		pRURL,
	const	RequestorType	reqType ) {

	FastCSLocker	lock( crit );

	reset();

	host = pHost;
	addr = pAddr;
	port = pPort;
	prot = pProt;
	text = pText;
	schm = pSchm;
	rURL = pRURL;
	type = reqType;

	return getPassAuth();

}

// ============================================================================

Net::PassAuthPtr Net::Authenticator::getPassAuth() {

	return 0;

}

// ============================================================================

void Net::Authenticator::reset() {

	host.kill();
	addr.kill();
	port = ( Uint16 )-1;
	prot.kill();
	text.kill();
	schm.kill();
	rURL.kill();
	type = SERVER;

}

// ============================================================================

Net::AuthenticatorPtr Net::Authenticator::systAuth;

#endif // 0

// ============================================================================

