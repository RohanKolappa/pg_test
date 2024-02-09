// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.FrameStore.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.FrameStore.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, FrameStore )

// ============================================================================

L2::FrameStore::FrameStore(
	const	Uint32		maxSize ) {

	size = ( maxSize ? maxSize : 250 );
	ring.resize( size );
	head = ( Uint64 )0;

}

L2::FrameStore::~FrameStore() {



}

// ============================================================================

Bool L2::FrameStore::hasFrame(
	const	Uint64		index ) {

	return ( index >= head
	      && index < head + ( Uint64 )size
	      && ring[ ( Uint32 )( index % size ) ] );

}

FramePtr L2::FrameStore::getFrame(
	const	Uint64		index ) {

	Uint32 i = ( Uint32 )( index % size );

	if ( index < head
	  || index >= head + size
	  || ! ring[ i ] ) {
		throw NoSuchFrame();
	}

	return ring[ i ];

}

void L2::FrameStore::addFrame(
	const	Uint64		index,
		FramePtr	data ) {

	if ( index < head ) {
		for ( Uint64 i = index
		    ; i < index + size && i < head
		    ; i++ ) {
			ring[ ( Uint32 )( i % size ) ].kill();
		}
		ring[ ( Uint32 )( index % size ) ] = data;
		head = index;
	}
	else if ( index < head + size ) {
		ring[ ( Uint32 )( index % size ) ] = data;
	}
	else {
		for ( Uint64 i = index
		    ; i + size > index && i >= head + size
		    ; i-- ) {
			ring[ ( Uint32 )( i % size ) ].kill();
		}
		ring[ ( Uint32 )( index % size ) ] = data;
		head = index - size + 1;
	}

}

// ============================================================================

