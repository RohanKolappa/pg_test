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
//	BFC.Crypto.TripleDES.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.DESCommon.h"
#include "BFC.Crypto.Functions.h"
#include "BFC.Crypto.TripleDES.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, TripleDES )

// ============================================================================

const DL::TypeCPtr & Crypto::TripleDES::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "accc4318-422e-47be-9123-4fe13a67b56d" ),
		"BFC.Crypto.TripleDES",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"3des",
				"Cipher",
				"TripleDES Cipher" ),
			24, 24, 8, 16 ) );

	return i;

}

// ============================================================================

Crypto::TripleDES::TripleDES() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::TripleDES::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 24 ) {
		throw InvalidArgument();
	}

	return 24;

}

// ============================================================================

void Crypto::TripleDES::setKey(
	const	Buffer &	pKey ) {

	if ( pKey.getLength() != 24 ) {
		throw InvalidArgument( "invalid keysize!" );
	}

	const Uchar * ptr = pKey.getCstPtr();

	DESCommon::deskey( ptr     , DESCommon::EN0, ek[ 0 ] );
	DESCommon::deskey( ptr +  8, DESCommon::DE1, ek[ 1 ] );
	DESCommon::deskey( ptr + 16, DESCommon::EN0, ek[ 2 ] );

	DESCommon::deskey( ptr     , DESCommon::DE1, dk[ 2 ] );
	DESCommon::deskey( ptr +  8, DESCommon::EN0, dk[ 1 ] );
	DESCommon::deskey( ptr + 16, DESCommon::DE1, dk[ 0 ] );

}

// ============================================================================

void Crypto::TripleDES::encrypt(
	const	Buffer &	pPlainText,
		Buffer &	pCipherText ) {

	if ( pPlainText.getLength() != 8 ) {
		throw InvalidArgument();
	}

	if ( pCipherText.getLength() != 8 ) {
		pCipherText.resize( 8 );
	}

	const Uchar *	pt = pPlainText.getCstPtr();
	Uchar *		ct = pCipherText.getVarPtr();

	Uint32		work[ 2 ];
	
	work[ 0 ] = LOAD32H( pt     );
	work[ 1 ] = LOAD32H( pt + 4 );

	DESCommon::desfunc( work, ek[ 0 ] );
	DESCommon::desfunc( work, ek[ 1 ] );
	DESCommon::desfunc( work, ek[ 2 ] );

	STORE32H( work[ 0 ], ct     );
	STORE32H( work[ 1 ], ct + 4 );

}

void Crypto::TripleDES::decrypt(
	const	Buffer &	pCipherText,
		Buffer &	pPlainText ) {

	if ( pCipherText.getLength() != 8 ) {
		throw InvalidArgument();
	}

	if ( pPlainText.getLength() != 8 ) {
		pPlainText.resize( 8 );
	}

	const Uchar *	ct = pCipherText.getCstPtr();
	Uchar *		pt = pPlainText.getVarPtr();

	Uint32		work[ 2 ];

	work[ 0 ] = LOAD32H( ct     );
	work[ 1 ] = LOAD32H( ct + 4 );

	DESCommon::desfunc( work, dk[ 0 ] );
	DESCommon::desfunc( work, dk[ 1 ] );
	DESCommon::desfunc( work, dk[ 2 ] );

	STORE32H( work[ 0 ], pt     );
	STORE32H( work[ 1 ], pt + 4 );

}

// ============================================================================

void Crypto::TripleDES::test() {

	Buffer	key;
	Buffer	msg;

	for ( Uint32 x = 0 ; x < 8 ; x++ ) {
		msg += ( Uchar )x;
	}
	
	for ( Uint32 x = 0 ; x < 24 ; x++ ) {
		key += ( Uchar )x;
	}

	setKey( key );

	Buffer enc = Cipher::encrypt( msg );
	Buffer dec = Cipher::decrypt( enc );
	
	if ( msg != dec ) {
		throw InternalError();
	}
	
}

// ============================================================================

static Crypto::CipherRegisterer< Crypto::TripleDES > tripledesRegisterer;

// ============================================================================

