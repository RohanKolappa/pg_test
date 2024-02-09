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
//	BFC.Crypto.Yarrow.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Crypto.AES.h"
#include "BFC.Crypto.CTR.h"
#include "BFC.Crypto.PRNGRegisterer.h"
#include "BFC.Crypto.SHA256.h"
#include "BFC.Crypto.Yarrow.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, Yarrow )

// ============================================================================

const DL::TypeCPtr & Crypto::Yarrow::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "dd762265-4821-4823-8a1f-384bc586ca8d" ),
		"BFC.Crypto.Yarrow",
		Crypto::PRNG::getClassType(),
		new PRNGDescriptor(
			DL::Descr(
				"yarrow",
				"PRNG",
				"Yarrow PRNG" ),
			64 ) );

	return i;

}

// ============================================================================

Crypto::Yarrow::Yarrow() :

	PRNG( getClassType() ) {

	cipher	= new AES;
	hash	= new SHA256;

}

// ============================================================================

void Crypto::Yarrow::start() {

//	pool = Buffer::chars( hash->getHashSize(), 0x00 );
	pool = Buffer::chars( 128, 0x00 );

}

void Crypto::Yarrow::add_entropy(
	const	Buffer &	pData ) {

	FastCSLocker	locker( cs );

	hash->init();
	hash->process( pool( 0, hash->getHashSize() ) );
	hash->process( pData );

	Buffer tmp;

	hash->done( tmp );

	pool = tmp + pool( tmp.getLength() );

}

void Crypto::Yarrow::ready() {

	FastCSLocker	locker( cs );

	Uint32	ks = cipher->getKeySize( hash->getHashSize() );

	ctr = new CTR( false );

	ctr->init(
		cipher,
		pool( 0, cipher->getBlockSize() ),	/* IV */
		pool( 0, ks ) );			/* KEY and key size */

}

void Crypto::Yarrow::done() {

	FastCSLocker	locker( cs );

	ctr->done();

	ctr.kill();

}

// ============================================================================

Uint32 Crypto::Yarrow::read(
		Buffer &	pBuffer ) {

	pBuffer = ctr->encrypt(
		Buffer::chars( cipher->getBlockSize(), 0x00 ) );

	return pBuffer.getLength();

}

// ============================================================================

Buffer Crypto::Yarrow::exportState() {

	return PRNG::read( 64 );

}

void Crypto::Yarrow::importState(
	const	Buffer &	pState ) {

	if ( pState.getLength() != 64 ) {
		throw InvalidArgument();
	}

	start();
	add_entropy( pState );

}

// ============================================================================

void Crypto::Yarrow::test() {

	cipher->test();
	hash->test();

}

// ============================================================================

static Crypto::PRNGRegisterer< Crypto::Yarrow > yarrowRegisterer;

// ============================================================================

