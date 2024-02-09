// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::Light".
//
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.Base.Buffer.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include <stdio.h>
#include <stdlib.h>

#ifndef PLATFORM_WIN32
#include <stdint.h>
#endif

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.System.h"

#include "BFC.Math.Utils.h"

#include "BFC.Mem.Allocator.h"
#include "BFC.Mem.Data.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Buffer::Buffer(
	const	Char *		pPtr,
	const	Bool		pCpy ) {

	Mem::Data *	data = 0;
	Uint32		len = 0;

	if ( pPtr ) {
		const Uchar * str = ( const Uchar * )pPtr;
		len = System::strlen( str );
// FIXME: Don't copy unless needed! Code that still cause some trouble:
// FIXME: - TVCard vmp plugin
// FIXME: - FSDir
// FIXME: - ...
		data = new Mem::Data( str, len, len + 1, pCpy ? true : true );
	}
	else {
		data = new Mem::Data;
	}

	attachData( data, 0, len );

}

Buffer::Buffer(
	const	Uchar *		pPtr,
	const	Uint32		pLen,
	const	Bool		pCpy ) {

	Mem::Data *	data = 0;
	Uint32		len = 0;

	if ( pPtr && pLen ) {
		data = new Mem::Data( pPtr, pLen, pLen, pCpy );
		len = pLen;
	}
	else {
		data = new Mem::Data;
	}

	attachData( data, 0, len );

}

Buffer::Buffer(
	const	Char		src ) {

	attachData(
		new Mem::Data( ( const Uchar * )& src, 1, 1, true ),
		0,
		1 );

}

Buffer::Buffer(
	const	Uchar		src ) {

	attachData(
		new Mem::Data( & src, 1, 1, true ),
		0,
		1 );

}

Buffer::Buffer(
	const 	Int32		src ) {

	Mem::Data *	data = new Mem::Data( 16 );
	Uchar *		ptr = data->getPhyVarPtr() + 15;
	Uint32		len = 0;

	if ( src ) {
		Uint32 val = ( Uint32 )( src > 0 ? src : -src );
		while ( val ) {
			*ptr-- = ( Uchar )( '0' + ( val % 10 ) );
			len++;
			val /= 10;
		}
		if ( src < 0 ) {
			*ptr = '-';
			len++;
		}
	}
	else {
		*ptr = '0';
		len++;
	}

	attachData( data, 16 - len, len );

}

Buffer::Buffer(
	const	Uint32		pUint,
	const	Base		pBase,
	const	Uint32		pSize,
	const	Uchar		pFill,
	const	Bool		pPref ) {

	Uint32		alloc = Math::getMax( 18U, pSize + 2 );
	Mem::Data *	data = new Mem::Data( alloc );
	Uchar *		ptr = data->getPhyVarPtr() + alloc - 1;
	Uint32		len = 0;

	if ( pUint ) {
		static const Uchar table[] = "0123456789ABCDEF";
		Uint32	val = pUint;
		while ( val ) {
			*ptr-- = table[ val % ( Uint32 )pBase ];
			len++;
			val /= ( Uint32 )pBase;
		}
	}
	else {
		*ptr-- = '0';
		len++;
	}

	// Fill with '0', if needed...

	if ( pFill == '0' ) {
		while ( len < pSize ) {
			*ptr-- = '0';
			len++;
		}
	}

	// Prefix with base indicator, if needed...

	if ( pPref ) {
		switch ( pBase ) {
		case Base10 :
			break;
		case Base16 :
			*ptr-- = 'x';
			*ptr-- = '0';
			len += 2;
			break;
		case Base8 :
			*ptr-- = 'o';
			*ptr-- = '0';
			len += 2;
			break;
		case Base2 :
			*ptr-- = 'b';
			*ptr-- = '0';
			len += 2;
			break;
		}
	}

	// Fill with fill char, if needed...

	if ( pFill != '0' ) {
		while ( len < pSize ) {
			*ptr-- = pFill;
			len++;
		}
	}

	attachData( data, alloc - len, len );

}

Buffer::Buffer(
	const 	Int64		src ) {

	Mem::Data *	data = new Mem::Data( 32 );
	Uchar *		ptr = data->getPhyVarPtr() + 31;
	Uint32		len = 0;

	if ( src ) {
		Uint64 val = ( Uint64 )( src > 0 ? src : -src );
		while ( val ) {
			*ptr-- = ( Uchar )( '0' + ( val % 10 ) );
			len++;
			val /= 10;
		}
		if ( src < 0 ) {
			*ptr = '-';
			len++;
		}
	}
	else {
		*ptr = '0';
		len++;
	}

	attachData( data, 32 - len, len );

}

Buffer::Buffer(
	const	Uint64		pUint,
	const	Base		pBase,
	const	Uint32		pSize,
	const	Uchar		pFill ) {

	Uint32		alloc = Math::getMax( 34U, pSize + 2 );
	Mem::Data *	data = new Mem::Data( alloc );
	Uchar *		ptr = data->getPhyVarPtr() + alloc - 1;
	Uint32		len = 0;

	if ( pUint ) {
		static const Uchar table[] = "0123456789ABCDEF";
		Uint64	val = pUint;
		while ( val ) {
			*ptr-- = table[ val % ( Uint32 )pBase ];
			len++;
			val /= ( Uint32 )pBase;
		}
	}
	else {
		*ptr-- = '0';
		len++;
	}

	// Fill with '0', if needed...

	if ( pFill == '0' ) {
		while ( len < pSize ) {
			*ptr-- = '0';
			len++;
		}
	}

	// Prefix with base indicator, if needed...

	switch ( pBase ) {
	case Base10 :
		break;
	case Base16 :
		*ptr-- = 'x';
		*ptr-- = '0';
		len += 2;
		break;
	case Base8 :
		*ptr-- = 'o';
		*ptr-- = '0';
		len += 2;
		break;
	case Base2 :
		*ptr-- = 'b';
		*ptr-- = '0';
		len += 2;
		break;
	}

	// Fill with fill char, if needed...

	if ( pFill != '0' ) {
		while ( len < pSize ) {
			*ptr-- = pFill;
			len++;
		}
	}

	attachData( data, alloc - len, len );

}

Buffer::Buffer(
	const	Double		pDouble,
	const	Char *		pFormat ) {

	Mem::Data *	data = new Mem::Data( 128 );
	char *		ptr = ( char * )data->getPhyVarPtr();
	int		len;
	const char *	fmt = ( pFormat ? pFormat : "%f" );

#if defined( PLATFORM_WIN32 )
#define MY_SNPRINTF ::_snprintf
#else
#define MY_SNPRINTF ::snprintf
#endif

	if ( pDouble == DoubleMax ) {
		len = MY_SNPRINTF( ptr, 128, "%s", "inf" );
	}
	else if ( pDouble == DoubleMax ) {
		len = MY_SNPRINTF( ptr, 128, "%s", "-inf" );
	}
	else {
		len = MY_SNPRINTF( ptr, 128, fmt, pDouble );
	}

#undef MY_SNPRINTF

	attachData(
		data,
		0,
		len >= 1 ? Math::getMin( 127U, ( Uint32 )len ) : 0 );

}

Buffer::Buffer(
	const	void *		ptr ) {

	static const Uchar table[] = "0123456789ABCDEF";
	
	Uint32	len = 0;
	if ( sizeof( void * ) == 4 ) {
		len = 10;
	}
	else if ( sizeof( void * ) == 8 ) {
		len = 18;
	}
	else {
		throw InternalError( "sizeof( int ) ?????????????" );
	}

	uintptr_t	u = ( /* const */ uintptr_t )ptr;
	Mem::Data *	d = new Mem::Data( len );
	Uchar *		p = d->getPhyVarPtr();
	
	System::memcpy(
		p,
		(len==10?( const Uchar * )"0x00000000":( const Uchar * )"0x0000000000000000"),
		len );
	
	p += len-1;

	while ( u ) {
		*p-- = table[ u & 0xF ];
		u >>= 4;
	}

	attachData( d, 0, len );

}

// ============================================================================

void Buffer::use(
		Uchar *		src,
	const	Uint32		len ) {

	attachData(
		new Mem::Data( src, len, Mem::StdCppAllocator::instance( false ) ),
		0,
		len );

}

// ============================================================================

Buffer & Buffer::operator = (
	const	Buffer &	pOther ) {

	Mem::DataSharer::operator = ( pOther );

	return * this;

}

// ============================================================================

Buffer Buffer::operator + (
	const	Buffer &	pOther ) const {

	Buffer	res = * this;

	res += pOther;

	return res;

}

// ============================================================================

void Buffer::operator += (
	const	Buffer &	pSrc ) {

#if 1

	if ( pSrc.isEmpty() ) {
		return;
	}

	Uint32 delta = getLength();

	resize( delta + pSrc.getLength() );

	System::memcpy(
		getVarPtr() + delta,
		pSrc.getCstPtr(),
		pSrc.getLength() );

#else

	Buffer	src2 = pSrc;

	if ( src2.isEmpty() ) {
		return;
	}

	Buffer	src1 = * this;

	if ( src1.isEmpty() ) {
		operator = ( src2 );
		return;
	}

	Uint32 delta = src1.getLength();

	src1.resize( src1.getLength() + src2.getLength() );

	System::memcpy(
		src1.getVarPtr() + delta,
		src2.getCstPtr(),
		src2.getLength() );

	operator = ( src1 );

#endif

}

// ============================================================================

Bool Buffer::operator == (
	const	Buffer &	pSrc ) const {

	Buffer		src1 = * this;
	Buffer		src2 = pSrc;

	const Uint32	len1 = src1.getLength();
	const Uint32	len2 = src2.getLength();

	if ( len1 != len2 ) {
		return false;
	}

	const Uchar *	ptr1 = src1.getCstPtr();
	const Uchar *	ptr2 = src2.getCstPtr();

	if ( ptr1 == ptr2 ) {
		return true;
	}

	Int32 r = System::memcmp( ptr1, ptr2, len1 );

	return ( r == 0 );

}

Bool Buffer::operator != (
	const	Buffer &	pSrc ) const {

	return ( ! operator == ( pSrc ) );

}

Bool Buffer::operator < (
	const	Buffer &	pSrc ) const {

	Buffer		src1 = * this;
	Buffer		src2 = pSrc;

	const Uint32	len1 = src1.getLength();
	const Uint32	len2 = src2.getLength();

	const Uchar *	ptr1 = src1.getCstPtr();
	const Uchar *	ptr2 = src2.getCstPtr();

	if ( ptr1 == ptr2 ) {
		return false;
	}

	Int32 r = System::strncmp( ptr1, ptr2, Math::getMin( len1, len2 ) );

	return ( r < 0 || ( r == 0 && len1 < len2 ) );

}

Bool Buffer::operator > (
	const	Buffer &	pSrc ) const {

	Buffer		src1 = * this;
	Buffer		src2 = pSrc;

	const Uint32	len1 = src1.getLength();
	const Uint32	len2 = src2.getLength();

	const Uchar *	ptr1 = src1.getCstPtr();
	const Uchar *	ptr2 = src2.getCstPtr();

	if ( ptr1 == ptr2 ) {
		return false;
	}

	Int32 r = System::strncmp( ptr1, ptr2, Math::getMin( len1, len2 ) );

	return ( r > 0 || ( r == 0 && len1 > len2 ) );

}

Bool Buffer::operator <= (
	const	Buffer &	pSrc ) const {

	return ( ! operator > ( pSrc ) );

}

Bool Buffer::operator >= (
	const	Buffer &	pSrc ) const {

	return ( ! operator < ( pSrc ) );

}

Int32 Buffer::compare(
	const	Buffer &	pSrc,
	const	Bool		pIgn ) const {

	Buffer		src1 = * this;
	Buffer		src2 = pSrc;

	const Uchar *	ptr1 = src1.getCstPtr();
	const Uchar *	ptr2 = src2.getCstPtr();

	if ( ptr1 == ptr2 ) {
		return 0;
	}

	const Uint32	len1 = src1.getLength();
	const Uint32	len2 = src2.getLength();
	const Uint32	len = Math::getMin( len1, len2 );

	Int32 res = ( pIgn
		? System::strncasecmp( ptr1, ptr2, len )
		: System::strncmp( ptr1, ptr2, len ) );

	return ( res
		? res
		: len1 > len2
		? 1
		: len1 < len2
		? -1
		: 0 );

}

// ============================================================================

const Char * Buffer::getCStringPtr() {

	static const Char *	empty = "";

	if ( isEmpty() ) {
		return empty;
	}

	// Shortcut possible ?

//	Uint32		logOffset = getLogOffset();
	Uint32		logLength = getLogLength();
//	Uint32		phyLength = getPhyLength();
	const Uchar *	logCstPtr = getLogCstPtr();

// JFGO: Too many issues.
//	if ( logOffset + logLength < phyLength
//	  && logCstPtr[ logLength ] == '\0' ) {
//		return ( const Char * )logCstPtr;
//	}

	// Fallback solution! Copy and append a trailing '\0'...

	Mem::Data * data = new Mem::Data( logLength + 1 );

	System::memcpy(
		data->getPhyVarPtr(),
		logCstPtr,
		logLength );

	data->getPhyVarPtr()[ logLength ] = '\0';

	attachData( data, 0, logLength );

	return ( const Char * )getCstPtr();

}

// ============================================================================

Buffer Buffer::operator () (
	const	Uint32		beg ) const {

	return Buffer( * this, beg );

}

Buffer Buffer::operator () (
	const	Uint32		beg,
	const	Uint32		len ) const {

	return Buffer( * this, beg, len );

}

// ============================================================================

Buffer Buffer::trim() const {

	Buffer		res = * this;
	Uint32		l = res.getLength();
	Uint32		k = 0;
	const Uchar *	p = res.getCstPtr();

	while ( k < l && p[ k ] <= 0x20 ) {
		k++;
	}

	while ( k < l && p[ l - 1 ] <= 0x20 ) {
		l--;
	}

	return res( k, l - k );

}

// ============================================================================

void Buffer::replace(
	const	Uchar		oldChar,
	const	Uchar		newChar ) {

	const Uint32	l = getLength();
	const Uchar *	p = getCstPtr();
	Uint32		i;

	for ( i = 0 ; i < l ; i++ ) {
		if ( p[ i ] == oldChar ) {
			break;
		}
	}

	if ( i >= l ) {
		return;
	}

	Uchar *		q = getVarPtr();

	for ( i = 0 ; i < l ; i++ ) {
		if ( q[ i ] == oldChar ) {
			q[ i ] = newChar;
		}
	}

}

void Buffer::replace(
	const	Uint32		pPos,
	const	Uint32		pLen,
	const	Buffer &	pSub ) {

	Buffer	src = * this;

	operator = (
		src( 0, pPos )
		+ pSub
		+ src( pPos + pLen ) );

}

void Buffer::overwrite(
	const	Uint32		pPos,
	const	Buffer &	pSub,
	const	Uint32		pBeg,
	const	Uint32		pLen ) {

	if ( pPos >= getLength() ) {
		throw InvalidArgument( "Dst index out of bound!" );
	}

	if ( pBeg >= pSub.getLength() ) {
		throw InvalidArgument( "Src index out of bound!" );
	}

	Uint32 rLen = ( pLen == ( Uint32 )-1 ? pSub.getLength() - pBeg : pLen );

	if ( pBeg + rLen > pSub.getLength() ) {
		throw InvalidArgument( "Src underflow!" );
	}

	if ( pPos + rLen > getLength() ) {
		throw InvalidArgument( "Dst overflow!" );
	}

	const Uchar *	src = pSub.getCstPtr() + pBeg;
	Uchar *		dst = getVarPtr() + pPos;
	Uint32		len = rLen;

	while ( len-- ) {
		*dst++ = *src++;
	}

}

// ============================================================================

Uchar Buffer::firstChar() const {

	return * getCstPtr( 0 );

}

Uchar Buffer::lastChar() const {

	return * getCstPtr( getLength() - 1 );

}

// ============================================================================

Buffer Buffer::spaces(
	const	Uint32		l ) {

	return chars( l, ' ' );

}

Buffer Buffer::chars(
	const	Uint32		l,
	const	Uchar		c ) {

	Mem::Data * data = new Mem::Data( l );

	Uchar *	ptr = data->getPhyVarPtr();
	Uint32	cnt = l;

	while ( cnt-- ) {
		*ptr++ = c;
	}

	return Buffer( data );

}

Buffer Buffer::rand(
	const	Uint32		l ) {

	Mem::Data * data = new Mem::Data( l );

	Uchar *	ptr = data->getPhyVarPtr();
	Uint32	cnt = l;

	while ( cnt-- ) {
		*ptr++ = ( Uchar )Math::rand( ( Uint32 )0xFF );
	}

	return Buffer( data );

}

// ============================================================================

Int32 Buffer::getIntAt(
		Uint32 &	i ) const {

	return ( Int32 )convertToInt( i );

}

Uint32 Buffer::getUintAt(
		Uint32 &	i ) const {

	return ( Uint32 )( Int32 )convertToInt( i );

}

Uint64 Buffer::getUlonglongAt(
		Uint32 &	i ) const {

	return ( Uint64 )convertToInt( i );

}

Double Buffer::getDoubleAt(
		Uint32 &	i ) const {

	return convertToDouble( i );

}

// ============================================================================

Uint16 Buffer::toUint16() const {

	Uint32	pos = 0;

	return ( Uint16 )( Uint64 )convertToInt( pos );

}

Uint32 Buffer::toUint32() const {

	Uint32	pos = 0;

	return ( Uint32 )( Uint64 )convertToInt( pos );

}

Uint64 Buffer::toUint64() const {

	Uint32	pos = 0;

	return ( Uint64 )convertToInt( pos );

}

Int32 Buffer::toInt32() const {

	Uint32	pos = 0;

	return ( Int32 )convertToInt( pos );

}

Int64 Buffer::toInt64() const {

	Uint32	pos = 0;

	return convertToInt( pos );

}

Double Buffer::toDouble() const {

	Uint32	pos = 0;

	return convertToDouble( pos );

}

Bool Buffer::toBool() const {

	static Buffer allowed[] = {
		"yes",
		"y",
		"YES",
		"Y",
		"true",
		"TRUE",
		"1"
	};

	for ( Uint32 i = 0 ; i < 7 ; i++ ) {
		if ( *this == allowed[ i ] ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

Buffer Buffer::popToken(
	const	Char *		pSeps ) {

	const Uint32	cnt = getLength();

	if ( ! cnt ) {
		throw InvalidArgument( "popToken): this empty!" );
	}

	Buffer		sep = pSeps;
	const Uchar *	ptr = getCstPtr();
	Uint32		idx = 0;

	while ( idx < cnt && ! sep.contains( *ptr ) ) {
		idx++;
		ptr++;
	}

	Buffer res = Buffer( *this, 0, idx );

	while ( idx < cnt && sep.contains( *ptr ) ) {
		idx++;
		ptr++;
	}

	dropFirstChars( idx );

	return res;

}

// ============================================================================

Bool Buffer::getLineAt(
		Uint32 &	i,
		Buffer &	line ) const {

	line.kill();

	const Uint32	l = getLength();

	if ( i >= l ) {
		return false;
	}

	Uint32		beg = i;
	const Uchar *	p = getCstPtr();

	while ( i < l ) {
		Uchar c = p[ i ];
		if ( c == '\r' ) {
			if ( i + 1 == l ) {
				// This `CR' is the last char!
				if ( i > beg ) {
					line = *this;
					line = line( beg, i - beg );
				}
				i++;
				return true;
			}
			if ( p[ i + 1 ] == '\n' ) {
				if ( i > beg ) {
					line = *this;
					line = line( beg, i - beg );
				}
				i += 2;
				return true;
			}
			// This `CR' is a regular char in the string.
		}
		else if ( c == '\n' ) {
			// We got a `NL'!
			if ( i > beg ) {
				line = *this;
				line = line( beg, i - beg );
			}
			i++;
			return true;
		}
		i++;
	}

	if ( i > beg ) {
		line = *this;
		line = line( beg, i - beg );
	}

	return true;

}

// ============================================================================

Bool Buffer::findBufferAt(
		Uint32 &	pos,
	const	Buffer &	substring ) const {

	// Make sure we want get interrupted...

	Buffer	buffer = * this;
	Buffer	toFind = substring;

	if ( toFind.isEmpty() ) {
		return buffer.isEmpty();
	}

	const Uint32	dstLen = buffer.getLength();
	const Uint32	srcLen = toFind.getLength();

	if ( pos + srcLen > dstLen ) {
		return false;
	}

	Uint32		maxpos = dstLen - srcLen;
	Uint32		counter;
	const Uchar *	subdata;
	const Uchar *	tmpdata;
	const Uchar *	dstPtr = buffer.getCstPtr();
	const Uchar *	srcPtr = toFind.getCstPtr();

	while ( pos <= maxpos ) {
		tmpdata = dstPtr + pos;
		subdata = srcPtr;
		counter = srcLen;
		while ( counter ) {
			if ( * tmpdata != * subdata ) {
				break;
			}
			tmpdata++;
			subdata++;
			counter--;
		}
		if ( ! counter ) { // found !!!
			return true;
		}
		pos++;
	}

	return false;

}

// ============================================================================

Uint32 Buffer::replaceBuffer(
	const	Buffer &	pToFind,
	const	Buffer &	pToCopy,
	const	Uint32		pBegPos,
	const	Bool		pGlobal ) {

//	if ( pToCopy.contains( pToFind ) ) {
//		throw InternalError( "Buffer::replaceBuffer(): arg problem!" );
//	}

	Uint32	res = 0;
	Uint32	cursorPos = pBegPos;
	Buffer	srcBuffer = * this;

	for (;;) {
		if ( ! srcBuffer.findBufferAt( cursorPos, pToFind ) ) {
			break;
		}
		srcBuffer = srcBuffer( 0, cursorPos )
		          + pToCopy
		          + srcBuffer( cursorPos + pToFind.getLength() );
		cursorPos += pToCopy.getLength();
		if ( ! pGlobal ) {
			break;
		}
	}

	operator = ( srcBuffer );

	return res;

}

// ============================================================================

Bool Buffer::contains(
	const	Uchar		c ) const {

//	FastCSLocker	locker( crt );

	Uint32		len = getLength();
	const Uchar *	ptr = getCstPtr();

	while ( len-- ) {
		if ( *ptr++ == c ) {
			return true;
		}
	}

	return false;

}

Bool Buffer::contains(
	const	Buffer &	s ) const {

	Uint32 pos = 0;

	return findBufferAt( pos, s );

}

Bool Buffer::containsAny(
	const	Buffer &	pMatch,
		Uint32 &	pPos ) const {

	return ( ( pPos = findAnyFwd( pMatch ) ) == getLength() );

}

Bool Buffer::beginsWith(
	const	Buffer &	e,
	const	Uint32		s ) const {

	Buffer	b1 = *this;
	Buffer	b2 = e;

	if ( s ) {
		b1 = b1( s );
	}

	Uint32	t1 = b1.getLength();
	Uint32	t2 = b2.getLength();

	if ( t1 < t2 ) {
		return false;
	}
	else if ( t1 == t2 ) {
		return ( b1 == b2 );
	}
	else {
		return ( b1( 0, t2 ) == b2 );
	}

}

Bool Buffer::endsWith(
	const	Buffer &	e ) const {

	Buffer	b1 = *this;
	Buffer	b2 = e;

	Uint32	t1 = b1.getLength();
	Uint32	t2 = b2.getLength();

	if ( t1 < t2 ) {
		return false;
	}
	else if ( t1 == t2 ) {
		return ( b1 == b2 );
	}
	else {
		return ( b1( t1 - t2 ) == b2 );
	}

}

Uint32 Buffer::findClosingChar(
	const	Uint32		start ) const {

	Buffer	b1 = *this;
	Uint32	t1 = b1.getLength();

	if ( start >= t1 ) {
		return 0;
	}

	const Uchar *	p1 = b1.getCstPtr();
	Uchar		rChar;
	Uchar		lChar = p1[ start ];

	switch ( lChar ) {
	case '(' : rChar = ')'; break;
	case '[' : rChar = ']'; break;
	case '{' : rChar = '}'; break;
	default  : return 0;
	}

	Uint32	levels = 1;    // we already got one opening bracket
	Uint32	i = start + 1; // pos to start the search
	Uchar	current;

	for (;;) {
		if ( i >= t1 ) {
			return 0;
		}
		current = p1[ i ];
		if ( current == lChar ) {
			levels++;
		}
		else if ( current == rChar ) {
			levels--;
		}
		if ( levels == 0 ) {
			break;
		}
		i++;
	}

	return i;

}

// ============================================================================

Uint32 Buffer::findFwd(
	const	Uchar		c,
	const	Uint32		pos ) const {

	Uint32	cnt;
	Uint32	res;

	if ( pos < getLength() ) {
		cnt = getLength() - pos;
		res = pos;
	}
	else {
		cnt = getLength();
		res = 0;
	}

	const Uchar *	p = getCstPtr();

	while ( cnt-- ) {
		if ( p[ res ] == c ) {
			break;
		}
		res++;
	}

	return res;

}

Uint32 Buffer::findAnyFwd(
	const	Buffer &	c,
	const	Uint32		pos ) const {

	Uint32	cnt;
	Uint32	res;

	if ( pos < getLength() ) {
		cnt = getLength() - pos;
		res = pos;
	}
	else {
		cnt = 0;
		res = getLength();
	}

	const Uchar *	p = getCstPtr();

	while ( cnt-- ) {
		if ( c.contains( p[ res ] ) ) {
			break;
		}
		res++;
	}

	return res;

}

Uint32 Buffer::findNoneFwd(
	const	Buffer &	c,
	const	Uint32		pos ) const {

	Uint32	cnt;
	Uint32	res;

	if ( pos < getLength() ) {
		cnt = getLength() - pos;
		res = pos;
	}
	else {
		cnt = 0;
		res = getLength();
	}

	const Uchar *	p = getCstPtr();

	while ( cnt-- ) {
		if ( ! c.contains( p[ res ] ) ) {
			break;
		}
		res++;
	}

	return res;

}

Uint32 Buffer::findFwd(
	const	Buffer &	s,
	const	Uint32		p ) const {

	Uint32 pos = p;

	return ( findBufferAt( pos, s ) ? pos : getLength() );

}

Uint32 Buffer::findRev(
	const	Uchar		c,
	const	Uint32		pos ) const {

	Uint32	cnt;
	Uint32	res;

	if ( pos < getLength() && pos != ( Uint32 )-1) {
		cnt = pos + 1;
		res = pos;
	}
	else {
		cnt = getLength();
		res = cnt - 1;
	}

	const Uchar *	p = getCstPtr();

	while ( cnt-- ) {
		if ( p[ res ] == c ) {
			break;
		}
		res--;
	}

	return res;

}

Uint32 Buffer::findAnyRev(
	const	Buffer &	c,
	const	Uint32		pos ) const {

	Uint32	cnt;
	Uint32	res;

	if ( pos < getLength() && pos != ( Uint32 )-1) {
		cnt = pos + 1;
		res = pos;
	}
	else {
		cnt = getLength();
		res = cnt - 1;
	}

	const Uchar *	p = getCstPtr();

	while ( cnt-- ) {
		if ( c.contains( p[ res ] ) ) {
			break;
		}
		res--;
	}

	return res;

}

// ============================================================================

Buffer Buffer::toLower() const {

	Buffer res = * this;

	Uchar *	ptr = res.getVarPtr();
	Uint32	len = res.getLength();

	while ( len-- ) {
		if ( *ptr >= 'A' && *ptr <= 'Z' ) {
			*ptr = ( Uchar )( *ptr - 'A' + 'a' );
		}
		ptr++;
	}

	return res;

}

Buffer Buffer::toUpper() const {

	Buffer res = * this;

	Uchar *	ptr = res.getVarPtr();
	Uint32	len = res.getLength();

	while ( len-- ) {
		if ( *ptr >= 'a' && *ptr <= 'z' ) {
			*ptr = ( Uchar )( *ptr - 'a' + 'A' );
		}
		ptr++;
	}

	return res;

}

// ============================================================================

Buffer Buffer::operator ^ (
	const	Buffer &	pOther ) const {

	Uint32	len = getLength();

	if ( pOther.getLength() != len ) {
		throw InvalidArgument();
	}

	Buffer	res;

	res.resize( len );

	const Uchar *	src1 = getCstPtr();
	const Uchar *	src2 = pOther.getCstPtr();
	Uchar *		dst = res.getVarPtr();

	while ( len >= 8 ) {
		*( Uint64 * )dst = *( const Uint64 * )src1
		                 ^ *( const Uint64 * )src2;
		src1 += 8;
		src2 += 8;
		dst += 8;
		len -= 8;
	}

	while ( len-- ) {
		*dst++ = ( Uchar )( *src1++ ^ *src2++ );
	}

	return res;

}

void Buffer::operator ^= (
	const	Buffer &	pOther ) {

	Uint32	len = getLength();

	if ( pOther.getLength() != len ) {
		throw InvalidArgument();
	}

	const Uchar *	src = pOther.getCstPtr();
	Uchar *		dst = getVarPtr();

	while ( len >= 8 ) {
		*( Uint64 * )dst ^= *( const Uint64 * )src;
		src += 8;
		dst += 8;
		len -= 8;
	}

	while ( len-- ) {
		*dst++ ^= *src++;
	}

}

// ============================================================================

void Buffer::dumpToBuffer(
		Uchar		buf[],
	const	Uint32		len ) const {

	if ( len <= getLength() ) {
		throw OutOfRange();
	}

	System::memcpy(
		buf,
		getCstPtr(),
		getLength() );

	buf[ getLength() ] = '\0';

}

void Buffer::safeDumpToBuffer(
		Uchar		buf[],
	const	Uint32		len ) const {

	if ( len < getLength() ) {
		throw OutOfRange();
	}

	System::memcpy(
		buf,
		getCstPtr(),
		getLength() );

}

// ============================================================================

void Buffer::resize(
	const	Uint32		newSize ) {

	setLogLength( newSize );

}

void Buffer::pad(
	const	Uint32		pLength,
	const	Uchar		pChar ) {

	Uint32 oldLength = getLength();

	if ( oldLength >= pLength ) {
		return;
	}

	resize( pLength );

	Uchar *		ptr = getVarPtr() + oldLength;
	Uint32		len = pLength - oldLength;

	while ( len-- ) {
		*ptr++ = pChar;
	}

}

// ============================================================================

void Buffer::dropFirstChar() {

	if ( getLength() ) {
		setLogOffset( getLogOffset() + 1 );
	}

}

void Buffer::dropLastChar() {

	if ( getLength() ) {
		setLogLength( getLogLength() - 1 );
	}

}

void Buffer::dropFirstChars(
	const	Uint32		pLength ) {

	if ( getLength() > pLength ) {
		setLogOffset( getLogOffset() + pLength );
	}
	else {
		setLogLength( 0 );
	}

}

void Buffer::dropLastChars(
	const	Uint32		pLength ) {

	if ( getLength() >= pLength ) {
		setLogLength( getLogLength() - pLength );
	}
	else {
		setLogLength( 0 );
	}

}

void Buffer::pop(
	const	Uint32		pPos,
	const	Uint32		pLen ) {

	if ( pPos + pLen > getLength() ) {
		throw OutOfRange();
	}

	Uchar *		pd = getVarPtr() + pPos;
	const Uchar *	ps = getCstPtr() + pPos + pLen;
	const Uchar *	pm = getCstPtr() + getLength();

	while ( ps < pm ) {
		*pd++ = *ps++;
	}

	setLogLength( getLogLength() - pLen );

}

void Buffer::remove(
	const	Uchar		c ) {

	Uint32	popped = 0;

	Uchar *		v = getVarPtr();
	const Uchar *	p = getCstPtr();
	const Uint32	l = getLength();

	for ( Uint32 i = l ; i ; i--, p++ ) {
		if ( *p == c ) {
			popped++;
		}
		else {
			*v++ = *p;
		}
	}

	setLogLength( l - popped );

}

void Buffer::remove(
	const	Buffer &	/*pSub*/ ) {

	// todo

}

void Buffer::insert(
	const	Uint32		pPos,
	const	Buffer &	pSub ) {

	Buffer	src = * this;

	operator = (
		src( 0, pPos )
		+ pSub
		+ src( pPos ) );

}

// ============================================================================

void Buffer::supposedMaxSize(
	const	Uint32		s ) {

	setBlkLength( s );

}

// ============================================================================

Mem::AllocatorPtr Buffer::releaseData(
		Uchar * &	pPhyVarPtr,
		Uint32 &	pPhyLength,
	const	Bool		pRetrievable ) {

	return DataSharer::releaseData(
			pPhyVarPtr,
			pPhyLength,
			pRetrievable );

}

// ============================================================================

Int64 Buffer::convertToInt(
		Uint32 &	i ) const {

//	FastCSLocker locker( crt );

	if ( i >= getLength() ) {
		i = 0;
		return 0;
	}

	// OK. Let's build our own `atoi' function...

	const Uchar *	buf = getCstPtr();
	Bool		sign;

	if ( buf[ i ] == '-' ) {
		sign = true;
		i++;
	}
	else if ( buf[ i ] == '+' ) {
		sign = false;
		i++;
	}
	else {
		sign = false;
	}

	Int64	result = 0;
	Uint32	initpos = i;
	Uchar	c;

	if ( i + 2 < getLength()
	 && buf[ i ] == '0'
	 && ( buf[ i + 1 ] == 'x' || buf[ i + 1 ] == 'X' ) ) {
		i += 2;
		initpos = i;
		while ( i < getLength() ) {
			c = buf[ i ];
			if ( c >= '0' && c <= '9' ) {
				result <<= 4;
				result += ( c - '0' );
			}
			else if ( c >= 'A' && c <= 'F' ) {
				result <<= 4;
				result += ( ( c - 'A' ) + 10 );
			}
			else if ( c >= 'a' && c <= 'f' ) {
				result <<= 4;
				result += ( ( c - 'a' ) + 10 );
			}
			else {
				break;
			}
			i++;
		}
	}

	else {
		while ( i < getLength() ) {
			c = buf[ i ];
			if ( c >= '0' && c <= '9' ) {
				result *= 10;
				result += ( c - '0' );
			}
			else {
				break;
			}
			i++;
		}
	}

	if ( i == initpos ) {
		// we didn't read anything, beside the optional '-'
		i = 0;
		return 0;
	}

	return ( sign ? - result : result );

}

Double Buffer::convertToDouble(
		Uint32 &	i ) const {

	if ( i >= getLength() ) {
		i = 0;
		return 0;
	}

/*
	// OK. Let's build our own `atof' function...

	const Uchar *	buf = getCstPtr();
	Bool		sign;
	Bool		frac = false;

	if ( buf[ i ] == '-' ) {
		sign = true;
		i++;
	}
	else if ( buf[ i ] == '+' ) {
		sign = false;
		i++;
	}
	else {
		sign = false;
	}

	Double	result = 0.0;
	Double	factor = 0.1;
	Uint32	initpos = i;
	Uchar	c;

	while ( i < getLength() ) {
		c = buf[ i ];
		if ( c == '.' ) {
			if ( frac ) {
				i = 0;
				return 0.0;
			}
			frac = true;
			i++;
			continue;
		}
		if ( c < '0' || c > '9' )
			break;
		if ( ! frac ) {
			result *= 10.0;
			result += ( Double )( c - '0' );
		}
		else {
			result += ( Double )( c - '0' ) * factor;
			factor *= 0.1;
		}
		i++;
	}

	if ( i == initpos ) { // we didn't read anything, beside the optional '-'
		i = 0;
		return 0.0;
	}

	return ( sign ? - result : result );
*/

	Buffer	copy = *this;
	Double	res;

	if ( copy( i, 3 ) == "inf" ) {
		res = DoubleMax;
		i += 3;
	}
	else if ( copy( i, 4 ) == "-inf" ) {
		res = - DoubleMax;
		i += 4;
	}
	else {
		const char* p = copy.getCStringPtr();
		char* end;
		res = strtod(p+i, &end);
		i = end - p;
	}

	return res;

}

// ============================================================================

Buffer BFC::operator + (
	const	Char *		s1,
	const	Buffer &	s2 ) {

	return ( Buffer( s1 ) + s2 );

}

// ============================================================================

