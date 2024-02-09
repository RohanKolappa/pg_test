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
//	BFC.Crypto.RNGWin32.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )

// ============================================================================

#include "BFC.Crypto.RNGWin32.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Crypto::RNGWin32::RNGWin32() {

	hProv = 0;

	if ( CryptAcquireContext(
			&hProv,
			NULL,
			MS_DEF_PROV,
			PROV_RSA_FULL,
			CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET )
	  || CryptAcquireContext(
			&hProv,
			NULL,
			MS_DEF_PROV,
			PROV_RSA_FULL,
			CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET | CRYPT_NEWKEYSET ) ) {
		setValid();
	}

}

Crypto::RNGWin32::~RNGWin32() {

	if ( isValid() ) {
		CryptReleaseContext( hProv, 0 );
	}

}

// ============================================================================

void Crypto::RNGWin32::getBytes(
		Buffer &	pBuffer ) {

	if ( ! isValid() ) {
		throw NotValid();
	}

	unsigned char *	buf = ( unsigned char * )pBuffer.getVarPtr();
	unsigned long	len = ( unsigned long )pBuffer.getLength();

	if ( CryptGenRandom( hProv, len, buf ) == FALSE ) {
		throw InternalError();
	}

}

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

