// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UnitsIndex.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Math.Utils.h"

#include "VMP.Frame.h"
#include "VMP.IndexFactory.h"
#include "VMP.UnitsIndex.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( UnitsIndex )

BFC_PTR_IMPL( CFLIndex )
BFC_PTR_IMPL( VFLIndex )

// ============================================================================

UnitsIndex::UnitsIndex() :

	DL::Object	( getClassType() ) {

}

UnitsIndex::UnitsIndex(
	const	UnitsIndex &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	() {

	if ( pOther.keyIndex ) {
		keyIndex = new KeyIndex( *pOther.keyIndex );
	}

}

// ============================================================================

Buffer UnitsIndex::toBuffer() const {

	return ( keyIndex ? keyIndex->toBuffer() : Buffer( "No key index." ) );

}

// ============================================================================

void UnitsIndex::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	if ( hasKeyIndex() ) {
		out->putUchar( 0x01 );
		getKeyIndex()->doSerialize( out );
	}
	else {
		out->putUchar( 0x00 );
	}

}

void UnitsIndex::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Uchar c;

	c = in->getUchar();

	if ( c == 0x01 ) {
		KeyIndexPtr tmpIndex = new KeyIndex;
		tmpIndex->unSerialize( in );
		setKeyIndex( tmpIndex );
	}
	else if ( c == 0x00 ) {
		resetKeyIndex();
	}
	else {
		throw InternalError( "Can't unserialize UnitsIndex: corrupted stream!" );
	}

}

// ============================================================================

CFLIndex::CFLIndex(
	const	Uint32		__size,
	const	Uint32		__length ) :

	DL::Object	( getClassType() ),
	size		( __size ),
	length		( __length ) {

}

CFLIndex::CFLIndex(
	const	CFLIndex &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	UnitsIndex	( pOther ),
	size		( pOther.size ),
	length		( pOther.length ) {

}

// ============================================================================

UnitsIndexPtr CFLIndex::clone() const {

	return new CFLIndex( *this );

}

// ============================================================================

Uint32 CFLIndex::getSize() const {

	return size;

}

// ============================================================================

Uint64 CFLIndex::getByteOffset(
	const	Uint32		i ) {

	return ( Uint64 )Math::getMin( size, i ) * ( Uint64 )length;

}

Uint32 CFLIndex::getUnitOffset(
	const	Uint64		byteIndex ) {

	return ( length
		? Math::getMin( size, ( Uint32 )( byteIndex / length ) )
		: 0 );

}

// ============================================================================

Buffer CFLIndex::toBuffer() const {

	return UnitsIndex::toBuffer() + "\n"
		+ "Constant Frames Length Index\n"
		+ "    nbr of frames: " + Buffer( size ) + "\n"
		+ "    frames length: " + Buffer( length );

}

// ============================================================================

void CFLIndex::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	UnitsIndex::doSerialize( out );

	out->putLEUint32( size );
	out->putLEUint32( length );

}

void CFLIndex::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	UnitsIndex::unSerialize( in );

	size = in->getLEUint32();
	length = in->getLEUint32();

}

// ============================================================================

VFLIndex::VFLIndex() :

	DL::Object	( getClassType() ) {

	isReady = false;

}

VFLIndex::VFLIndex(
	const	VFLIndex &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	UnitsIndex	( pOther ),
	lengths		( pOther.lengths ),
	offsets		( pOther.offsets ),
	isReady		( pOther.isReady ) {

}

// ============================================================================

UnitsIndexPtr VFLIndex::clone() const {

	return new VFLIndex( *this );

}

// ============================================================================

void VFLIndex::setSize(
	const	Uint32		size ) {

	lengths.kill();
	offsets.kill();
	isReady = false;

	lengths.resize( size );

	Uint32 s = size;

	while ( s-- ) {
		lengths[ s ] = 0;
	}

}

Uint32 VFLIndex::getSize() const {

	return lengths.size();

}

// ============================================================================

Uint64 VFLIndex::getByteOffset(
	const	Uint32		unitIndex ) {

	ensureHasOffsets();

//	msgDbg( "getByteOffset: unitIndex == "
//		+ Buffer( unitIndex )
//		+ ", offsets.len == "
//		+ Buffer( offsets.size() ) );

	return offsets[ Math::getMin( unitIndex, getSize() ) ];

}

Uint32 VFLIndex::getUnitOffset(
	const	Uint64		byteIndex ) {

	ensureHasOffsets();

	Uint32	posMin = 0;
	Uint32	posMax = getSize();

	// Now, loop until we found the answer... Each step shortens the
	// range, or returns.

	for (;;) {

		if ( posMax == posMin ) {
			// Obvious!
			return posMin;
		}

		if ( byteIndex < offsets[ posMin + 1 ] ) {
			return posMin;
		}

		posMin++;

		if ( posMax == posMin ) {
			return posMin;
		}

		if ( byteIndex >= offsets[ posMax ] ) {
			return posMax;
		}

		posMax--;

		if ( posMax == posMin ) {
			return posMin;
		}

		// Let's compute the approximate position of the frame
		// containing byteIndex...

		Uint64 yd = offsets[ posMax ] - offsets[ posMin ];
		Uint64 xd = posMax - posMin;

		if ( yd == 0 ) {
			// Frames of length 0... Let's pick the first one!
			return posMin;
		}

		Uint32 mid = posMin
		           + ( Uint32 )( ( byteIndex - offsets[ posMin ] )
		                       * xd
		                       / yd );

		if ( byteIndex < offsets[ mid ] ) {
			// ---> mid > posMin!
			posMax = mid - 1;
		}
		else {
			posMin = mid;
		}

	}

}

// ============================================================================

Buffer VFLIndex::toBuffer() const {

	return UnitsIndex::toBuffer() + "\n"
		+ "Variable Frames Length Index\n"
		+ "    nbr of frames: " + Buffer( getSize() );

}

// ============================================================================

void VFLIndex::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	UnitsIndex::doSerialize( out );

	out->putLEUint32( lengths.size() );

	for ( Uint32 i = 0 ; i < lengths.size() ; i++ ) {
		out->putLEUint32( lengths[ i ] );
	}

}

void VFLIndex::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	UnitsIndex::unSerialize( in );

	Uint32 size = in->getLEUint32();

	lengths.kill();
	offsets.kill();

	lengths.resize( size );

	for ( Uint32 i = 0 ; i < size ; i++ ) {
		lengths[ i ] = in->getLEUint32();
	}

	isReady = false;

}

// ============================================================================

void VFLIndex::computeOffsets() {

	Uint32 s = lengths.size();

	offsets.resize( s + 1 );
	offsets[ 0 ] = 0;

	Uint64 cur = 0;
	Uint32 idx = 0;

	while ( s-- ) {
		cur += lengths[ idx++ ];
		offsets[ idx ] = cur;
	}

	isReady = true;

}

// ============================================================================

const DL::TypeCPtr & UnitsIndex::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "90a8223d-8a89-4a44-8c87-7706a0ba7e97" ),
		"VMP.UnitsIndex",
		DL::Object::getClassType() );

	return i;

}

const DL::TypeCPtr & CFLIndex::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "a7faf6c5-67d8-435a-ab19-022e72885fab" ),
		"VMP.CFLIndex",
		UnitsIndex::getClassType() );

	return i;

}

const DL::TypeCPtr & VFLIndex::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "b14fd09a-f5d4-40ca-8f42-764a7ff5ee8b" ),
		"VMP.VFLIndex",
		UnitsIndex::getClassType() );

	return i;

}

// ============================================================================

static IndexFactoryPtr cflFactory = new IndexFactoryTmpl< CFLIndex >;
static IndexFactoryPtr vflFactory = new IndexFactoryTmpl< VFLIndex >;

// ============================================================================

