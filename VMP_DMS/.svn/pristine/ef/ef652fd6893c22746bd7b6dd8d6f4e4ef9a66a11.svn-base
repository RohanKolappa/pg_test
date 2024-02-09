// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.KeyIndex.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.KeyIndex.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( KeyIndex )

// ============================================================================

KeyIndex::KeyIndex() {



}

KeyIndex::KeyIndex(
	const	KeyIndex &	o ) :

	SObject(),
	Serializable(),
	indexes( o.indexes ) {

}

KeyIndex::~KeyIndex() {



}

// ============================================================================

Uint32 KeyIndex::getBefore(
	const	Uint32		index ) const {

	if ( indexes.isEmpty() ) {
		return index;
	}

	Uint32	i;
	Uint32	l = indexes.size();

	for ( i = 0
	    ; i + 1 < l && indexes[ i + 1 ] <= index
	    ; i++ ) {
	}

	return indexes[ i ];

}

// ============================================================================

Buffer KeyIndex::toBuffer() const {

	Buffer res;

	res	= Buffer("KeyFrames Index\n")
		+ "    nbr of entries: "
		+ Buffer(indexes.size());

	Uint32 i;

	for (i = 0 ; i < 5 && i < indexes.size() ; i++) {
		res	+= "\n    entry["
			+ Buffer(i)
			+ "]: "
			+ Buffer(indexes[i]);
	}

	return res;

}

// ============================================================================

void KeyIndex::doSerialize(
		BFC::IO::ByteOutputStreamPtr	out ) const {

	out->putLEUint32( indexes.size() );

	for ( Uint32 i = 0 ; i < indexes.size() ; i++ ) {
		out->putLEUint32( indexes[i] );
	}

}

void KeyIndex::unSerialize(
		BFC::IO::ByteInputStreamPtr	in ) {

	Uint32 size = in->getLEUint32();

	indexes.kill();
	indexes.resize( size );

	for ( Uint32 i = 0 ; i < size ; i++ ) {
		indexes[i] = in->getLEUint32();
	}

}

// ============================================================================

