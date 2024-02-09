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
//	BFC.Crypto.OMAC.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.Crypto.CipherRepository.h"
#include "BFC.Crypto.MACRegisterer.h"
#include "BFC.Crypto.OMAC.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::Crypto::OMACBase
// ============================================================================

Crypto::OMACBase::OMACBase(
	const	DL::DescrCPtr &
				pDescr ) :

	MAC	( pDescr ) {

}

// ============================================================================

void Crypto::OMACBase::process(
	const	Buffer &	pData ) {

	buf += pData;

	while ( buf.getLength() > blkLen ) { 
		prv = cipher->encrypt( buf( 0, blkLen ) ^ prv );
		buf = buf( blkLen );
	}

}

void Crypto::OMACBase::done(
		Buffer &	pTag ) {

	Uint32	mode;

	if ( buf.getLength() < blkLen ) {
		buf += ( Uchar )0x80;
		buf.pad( blkLen );
		mode = 1;
	}
	else {
		mode = 0;
	}

	cipher->encrypt( buf ^ prv ^ Lu[ mode ], pTag );

}

// ============================================================================

void Crypto::OMACBase::shiftL(
		Buffer &	pBuffer ) {

	Uint32	l = pBuffer.getLength();
	Uchar *	p0 = pBuffer.getVarPtr();
	Uchar *	p1 = p0 + 1;
	Uint32	f = ( *p0 & 0x80 );	// MSB

	while ( --l ) {
		*p0 <<= 1;
		*p0++ |= ( *p1++ >> 7 );
	}

	*p0 <<= 1;

	if ( f ) {
		*p0 ^= ( pBuffer.getLength() == 8 ? 0x1B : 0x87 );
	}

}

void Crypto::OMACBase::shiftR(
		Buffer &	pBuffer ) {

	Uint32	l = pBuffer.getLength();
	Uchar *	p0 = pBuffer.getVarPtr() + l - 1;
	Uchar *	p1 = p0 - 1;
	Uint32	f = ( *p0 & 0x01 );	// LSB

	if ( f ) {
		*p0 ^= ( ( l == 8 ? 0x0D : 0x43 ) << 1 );
	}

	while ( --l ) {
		*p0 >>= 1;
		*p0-- |= ( *p1-- << 7 );
	}

	*p0 >>= 1;

	if ( f ) {
		*p0 ^= 0x80;
	}

}

// ============================================================================
// BFC::Crypto::OMAC
// ============================================================================

BFC_PTR_IMPL_NS( Crypto, OMAC )

// ============================================================================

const DL::TypeCPtr & Crypto::OMAC::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "f2e9fae7-3dc9-4e8e-9caf-8b839af927f5" ),
		"BFC.Crypto.OMAC",
		Crypto::MAC::getClassType(),
		new DL::Descr(
			"omac",
			"MAC",
			"OMAC MAC" ) );

	return i;

}

// ============================================================================

Crypto::OMAC::OMAC() :

	OMACBase( getClassType() ) {

}

// ============================================================================

void Crypto::OMAC::init(
		CipherPtr	pCipher,
	const	Buffer &	pKey ) {

	cipher	= pCipher;
	blkLen	= cipher->getBlockSize();

	if ( blkLen != 8 && blkLen != 16 ) {
		throw InvalidArgument( "Invalid cipher block size!" );
	}

	cipher->setKey( pKey );

	buf.kill();
	prv.kill();
	prv.pad( blkLen );

	Lu[ 0 ] = cipher->encrypt( Buffer::chars( blkLen, 0x00 ) );
	shiftL( Lu[ 0 ] );
	Lu[ 1 ] = Lu[ 0 ];
	shiftL( Lu[ 1 ] );

}

// ============================================================================

void Crypto::OMAC::test() {

	static const struct {
		const char *	key;
		const char *	msg;
		const char *	tag;
	} tests[] = {

	// OMAC1 Test Vectors for AES-128

	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"",
		"bb1d6929e95937287fa37d129b756746"
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"6bc1bee22e409f96e93d7e117393172a",
		"070a16b46b4d4144f79bdd9dd04a287c"
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411",
		"dfa66747de9ae63030ca32611497c827"
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
		"51f0bebf7e3b9d92fc49741779363cfe"
	},

	// OMAC1 Test Vectors for AES-192

	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
		"",
		"d17ddf46adaacde531cac483de7a9367"
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
		"6bc1bee22e409f96e93d7e117393172a",
		"9e99a7bf31e710900662f65e617c5184"
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411",
		"8a1de5be2eb31aad089a82e6ee908b0e"
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
		"a1d5df0eed790f794d77589659f39a11"
	},

	// OMAC1 Test Vectors for AES-256

	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		"",
		"028962f61b7bf89efc6b551f4667d983"
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		"6bc1bee22e409f96e93d7e117393172a",
		"28a7023f452e8f82bd4bf28d8c37c35c"
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411",
		"aaf3d8f1de5640c232f5b169b9c911e6"
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
		"e1992190549f6ed5696a2c056c315410"
	}

	};

	CipherPtr aes = CipherRepository::instance()->findFactoryFor( "aes" )
				->makeObject();

	for ( Uint32 x = 0 ; x < (sizeof(tests)/sizeof(tests[0])) ; x++ ) {

		Buffer	key = Utils::fromHex( tests[ x ].key, false );
		Buffer	msg = Utils::fromHex( tests[ x ].msg, false );
		Buffer	tag = Utils::fromHex( tests[ x ].tag, false );
		Buffer	out;

		init( aes, key );
		process( msg );
		done( out );

		if ( out != tag ) {
			throw InternalError();
		}

	}

}

// ============================================================================

static Crypto::MACRegisterer< Crypto::OMAC > omacRegisterer;

// ============================================================================
// BFC::Crypto::OMAC2
// ============================================================================

BFC_PTR_IMPL_NS( Crypto, OMAC2 )

// ============================================================================

const DL::TypeCPtr & Crypto::OMAC2::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1be403b6-57b2-47e3-af6b-216b7da31456" ),
		"BFC.Crypto.OMAC2",
		Crypto::MAC::getClassType(),
		new DL::Descr(
			"omac2",
			"MAC",
			"OMAC2 MAC" ) );

	return i;

}

// ============================================================================

Crypto::OMAC2::OMAC2() :

	OMACBase( getClassType() ) {

}

// ============================================================================

void Crypto::OMAC2::init(
		CipherPtr	pCipher,
	const	Buffer &	pKey ) {

	cipher	= pCipher;
	blkLen	= cipher->getBlockSize();

	if ( blkLen != 8 && blkLen != 16 ) {
		throw InvalidArgument( "Invalid cipher block size!" );
	}

	cipher->setKey( pKey );

	buf.kill();
	prv.kill();
	prv.pad( blkLen );

	Buffer L = cipher->encrypt( Buffer::chars( blkLen, 0x00 ) );

	Lu[ 0 ] = L;
	shiftL( Lu[ 0 ] );
	Lu[ 1 ] = L;
	shiftR( Lu[ 1 ] );

}

// ============================================================================

void Crypto::OMAC2::test() {

	static const struct {
		const char *	key;
		const char *	msg;
		const char *	tag;
	} tests[] = {

	// OMAC2 Test Vectors for AES-128

	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"",
		"f6bc6a41f4f84593809e59b719299cfe"
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"6bc1bee22e409f96e93d7e117393172a",
		"070a16b46b4d4144f79bdd9dd04a287c"
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"6bc1bee22e409f96e93d7e117393172a"
			"ae2d8a571e03ac9c9eb76fac45af8e51"
			"30c81c46a35ce411",
		"23fdaa0831cd314491ce4b25acb6023b"
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c",
		"6bc1bee22e409f96e93d7e117393172a"
			"ae2d8a571e03ac9c9eb76fac45af8e51"
			"30c81c46a35ce411e5fbc1191a0a52ef"
			"f69f2445df4f9b17ad2b417be66c3710",
		"51f0bebf7e3b9d92fc49741779363cfe"
	},

	// OMAC2 Test Vectors for AES-192

	{
		"8e73b0f7da0e6452c810f32b809079e5"
			"62f8ead2522c6b7b",
		"",
		"149f579df2129d45a69266898f55aeb2"
	},
	{
		"8e73b0f7da0e6452c810f32b809079e5"
			"62f8ead2522c6b7b",
		"6bc1bee22e409f96e93d7e117393172a",
		"9e99a7bf31e710900662f65e617c5184"
	},
	{
		"8e73b0f7da0e6452c810f32b809079e5"
			"62f8ead2522c6b7b",
		"6bc1bee22e409f96e93d7e117393172a"
			"ae2d8a571e03ac9c9eb76fac45af8e51"
			"30c81c46a35ce411",
		"b35e2d1b73aed49b78bdbdfe61f646df"
	},
	{
		"8e73b0f7da0e6452c810f32b809079e5"
			"62f8ead2522c6b7b",
		"6bc1bee22e409f96e93d7e117393172a"
			"ae2d8a571e03ac9c9eb76fac45af8e51"
			"30c81c46a35ce411e5fbc1191a0a52ef"
			"f69f2445df4f9b17ad2b417be66c3710",
		"a1d5df0eed790f794d77589659f39a11"
	},

	// OMAC2 Test Vectors for AES-256

	{
		"603deb1015ca71be2b73aef0857d7781"
			"1f352c073b6108d72d9810a30914dff4",
		"",
		"47fbde71866eae6080355b5fc7ff704c"
	},
	{
		"603deb1015ca71be2b73aef0857d7781"
			"1f352c073b6108d72d9810a30914dff4",
		"6bc1bee22e409f96e93d7e117393172a",
		"28a7023f452e8f82bd4bf28d8c37c35c"
	},
	{
		"603deb1015ca71be2b73aef0857d7781"
			"1f352c073b6108d72d9810a30914dff4",
		"6bc1bee22e409f96e93d7e117393172a"
			"ae2d8a571e03ac9c9eb76fac45af8e51"
			"30c81c46a35ce411",
		"f018e6053611b34bc872d6b7ff24749f"
	},
	{
		"603deb1015ca71be2b73aef0857d7781"
			"1f352c073b6108d72d9810a30914dff4",
		"6bc1bee22e409f96e93d7e117393172a"
			"ae2d8a571e03ac9c9eb76fac45af8e51"
			"30c81c46a35ce411e5fbc1191a0a52ef"
			"f69f2445df4f9b17ad2b417be66c3710",
		"e1992190549f6ed5696a2c056c315410"
	}

	};

	CipherPtr aes = CipherRepository::instance()->findFactoryFor( "aes" )
				->makeObject();

	for ( Uint32 x = 0 ; x < (sizeof(tests)/sizeof(tests[0])) ; x++ ) {

		Buffer	key = Utils::fromHex( tests[ x ].key, false );
		Buffer	msg = Utils::fromHex( tests[ x ].msg, false );
		Buffer	tag = Utils::fromHex( tests[ x ].tag, false );
		Buffer	out;

		init( aes, key );
		process( msg );
		done( out );

		if ( out != tag ) {
			throw InternalError( "Test " + Buffer( x ) );
		}

	}

}

// ============================================================================

static Crypto::MACRegisterer< Crypto::OMAC2 > omac2Registerer;

// ============================================================================

