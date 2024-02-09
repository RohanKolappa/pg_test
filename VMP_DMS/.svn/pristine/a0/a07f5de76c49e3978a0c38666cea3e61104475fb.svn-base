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
//	BFC.Buffer.cpp
// 
// Description:
//	...
// 
// Author( s ):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "BFC.Base.Buffer.h"
#include "BFC.Base.BufferData.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Buffer::Buffer( 
	const	Char	*p ) {

	if ( ! p ) {
		ptr = new BufferData;
		return;
	}

	ptr = new BufferData(
		(const Uchar *)p,
		(const Uint32)::strlen( p ) );

}

Buffer::Buffer( 
	const	Uchar	*p,
	const	Int32	len ) {

	if ( ! p || ! len ) {
		ptr = new BufferData;
		return;
	}

	Uint32 l =
		( len == -1
		? (Uint32) strlen( ( const Char * ) p )
		: (Uint32) len );

	ptr = new BufferData( p, l );

}

Buffer::Buffer( const Char src ) {

	ptr = new BufferData( (const Uchar *)&src, 1 );

}

Buffer::Buffer( const Uchar src ) {

	ptr = new BufferData( &src, 1 );

}

Buffer::Buffer(
	const 	Int32		src ) {

	Buffer res;

	if ( src < 0 ) {
		res = "-" + Buffer( Uint32( -src ) );
	}
	else {
		res = Buffer( Uint32( src ) );
	}

	res.ptr->incRefs();

	ptr = res.ptr;

}

Buffer::Buffer( 
	const	Uint32		src2,
	const	Base		b,
	const	Uint32		minLength ) {

	Uint32 src = src2;

	ptr = new BufferData;

	// And now, ladies and gentlemen, our own printf...

	Uchar	buffer[64];
	Uint32	i = 0;

	// 1 - write 'src' according to 'b' in 'buffer' in reverse order...

	static const Uchar	table[] = "0123456789ABCDEF";

	if ( src == 0 ) {
		// simply write "0"
		buffer[i++] = '0';
	}
	else {
		Uint32 base = ( Uint32 )b;
		while ( src ) {
			buffer[i++] = table[src % base];
			src /= base;
		}
	}

	// 2 - complete to have at least 'minLength' chars...

//	Uchar fillChar = ( b == Base10 ? ' ' : '0' );
	Uchar fillChar = '0';

	while ( i < minLength && i < 62 ) {
		buffer[i++] = fillChar;
	}

	// 3 - append base indication, if needed...

	switch ( b ) {
	case Base2: buffer[i++] = 'b'; buffer[i++] = '0'; break;
	case Base8: buffer[i++] = 'o'; buffer[i++] = '0'; break;
	case Base10: break;
	case Base16: buffer[i++] = 'x'; buffer[i++] = '0'; break;
	}

	// 4 - invert and dump to final destination...

	ptr->setUser( i );

	Uint32 j = 0;

	while ( i-- ) {
		ptr->setData( j++, buffer[i] );
	}

}

Buffer::Buffer( const Int64 src ) {

	Buffer res;

	if ( src < 0 ) {
		res = "-" + Buffer( Uint64( -src ) );
	}
	else {
		res = Buffer( Uint64( src ) );
	}

	res.ptr->incRefs();

	ptr = res.ptr;

}

Buffer::Buffer(
	const	Uint64		u ) {

	if ( ! u ) {
		ptr = new BufferData((const Uchar *)"0", 1);
	}

	Uint64 src = u;

	ptr = new BufferData;

	Uchar	buffer[64];
	Uint32	i = 0;

	static const Uchar	table[] = "0123456789ABCDEF";

	while ( src ) {
		buffer[i++] = table[ src % 10 ];
		src /= 10;
	}

	ptr->setUser( i );

	Uint32 j = 0;

	while ( i-- ) {
		ptr->setData( j++, buffer[i] );
	}

}

Buffer::Buffer( 
	const Double	d,
	const Char	*formatTemp ) {

	ptr = new BufferData;

	// Format string...

	Char	format[64];

	if ( ! formatTemp ) {
		strcpy( format, "%g" );
	}
	else {
		size_t length = strlen( formatTemp );
		if ( length >= 30 )
			return;
		strcpy( format, "%" );
		strcat( format, formatTemp );
		if ( length == 0 || strchr( "eEfgG", formatTemp[length-1] ) == 0 )
			strcat( format, "g" );
	}

	// Let's proceed to the real conversion!

	Uchar buffer[1024];

	Int32 len;

#if defined( PLATFORM_WIN32 )
	len = _snprintf( ( char * )buffer, 1024, format, d );
#else
	len = snprintf( ( char * )buffer, 1024, format, d );
#endif

	if ( len < 1 )
		return;

	ptr->setUser( len );

	while ( len-- ) {
		ptr->setData( len, buffer[len] );
	}

}

Buffer::Buffer( 
	const Double	d,
	const Buffer&	formatTemp ) {

	ptr = new BufferData;

	// Format string...

	if ( formatTemp.isEmpty() || formatTemp.length() >= 60 ) {
		return;
	}

	Buffer full;
	full = "%";
	full += formatTemp;
	full += "g";

	Char	format[64];

	full.dumpToBuffer( ( Uchar * )format, 64 );

	// Let's proceed to the real conversion!

	Uchar buffer[1024];

	Int32 len;

#if defined( PLATFORM_WIN32 )
	len = _snprintf( ( char * )buffer, 1024, format, d );
#else
	len = snprintf( ( char * )buffer, 1024, format, d );
#endif

	if ( len < 1 )
		return;

	ptr->setUser( len );

	while ( len-- ) {
		ptr->setData( len, buffer[len] );
	}

}

Buffer::Buffer(
	const	Buffer&		src ) {

	src.ptr->incRefs();

	ptr = src.ptr;

}

Buffer::~Buffer() {

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

}

void Buffer::use(
		Uchar		*src,
		Int32		len ) {

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

	ptr = new BufferData;

	if ( ! src ) {
		return;
	}

	if ( len == -1 ) {
		len = ::strlen( ( const Char * )src );
	}

	ptr->reuse( src, len, len );

}

void Buffer::use(
		Uchar		*src,
		Uint32		len,
		Uint32		alloc ) {

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

	ptr = new BufferData;

	if ( ! src ) {
		return;
	}

	ptr->reuse( src, len, alloc );

}

Buffer& Buffer::operator = ( const Buffer& src ) {

	if ( this == &src ) {
		return *this;
	}

	if ( ptr == src.ptr ) {
		return *this;
	}

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

	src.ptr->incRefs();

	ptr = src.ptr;

	return *this;

}

Buffer Buffer::operator + ( const Buffer& src ) const {

	Buffer res( *this );

	res += src;

	return res;

}

Buffer& Buffer::operator += ( const Buffer& src ) {

	if ( ! src.ptr->getUser() ) {
		return *this;
	}

	if ( ! ptr->getUser() ) {
		*this = src;
		return *this;
	}

	Uint32 newLen = ptr->getUser() + src.ptr->getUser();

	if ( ptr == src.ptr ) {
		BufferData *newPtr = new BufferData;
		newPtr->setUser( newLen );
		newPtr->setData(
			0,
			ptr->getAddr(),
			ptr->getUser() );
		newPtr->setData(
			ptr->getUser(),
			src.ptr->getAddr(),
			src.ptr->getUser() );
		if ( ! ptr->decRefs() ) {
			delete ptr;
		}
		ptr = newPtr;
		return *this;
	}

// FIXME: condition 'users == 1' could change while we modify *ptr!!!

	if ( ptr->getRefs() == 1
	  && newLen <= ptr->getPhys() ) {
		ptr->setData(
			ptr->getUser(),
			src.ptr->getAddr(),
			src.ptr->getUser() );
		ptr->setUser(
			newLen );
		return *this;
	}

	BufferData *newPtr = new BufferData;

	newPtr->setPhys(
		newLen );
	newPtr->setData(
		0,
		ptr->getAddr(),
		ptr->getUser() );
	newPtr->setData(
		ptr->getUser(),
		src.ptr->getAddr(),
		src.ptr->getUser() );
	newPtr->setUser(
		newLen );

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

	ptr = newPtr;

	return *this;

}

Bool Buffer::operator == ( const Buffer& src ) const {

	if ( ptr == src.ptr ) {
		return true;
	}

	if ( ptr->getUser() != src.ptr->getUser() ) {
		return false;
	}

	if ( ! ::strncmp( ( const Char * )ptr->getAddr(),
	                ( const Char * )src.ptr->getAddr(),
	                ptr->getUser() ) ) {
		return true;
	}

	return false;

}

Bool Buffer::operator != ( const Buffer& src ) const {

	if ( ptr == src.ptr ) {
		return false;
	}

	if ( ptr->getUser() != src.ptr->getUser() ) {
		return true;
	}

	if ( ! ::strncmp( ( const Char * )ptr->getAddr(),
	                ( const Char * )src.ptr->getAddr(),
	                ptr->getUser() ) ) {
		return false;
	}

	return true;

}

Bool Buffer::operator < ( const Buffer& src ) const {

	if ( ptr == src.ptr )
		return false;

	Uint32 l = ( 
		length() < src.length()
		? length()
		: src.length() );

	if ( ::strncmp( ( const Char * )ptr->getAddr(),
	            ( const Char * )src.ptr->getAddr(), l ) >= 0 )
		return false;

	return ( length() <= src.length() );

}

Bool Buffer::operator > ( const Buffer& src ) const {

	if ( ptr == src.ptr )
		return false;

	Uint32 l = ( 
		length() < src.length()
		? length()
		: src.length() );

	if ( ::strncmp( ( const Char * )ptr->getAddr(),
	            ( const Char * )src.ptr->getAddr(), l ) <= 0 )
		return false;

	return ( length() >= src.length() );

}

Bool Buffer::isEmpty() const {

	return ( ! ptr->getUser() );

}

Uint32 Buffer::length() const {

	return ( ptr->getUser() );

}

const Uchar *Buffer::getBufferAddr() const {

	return ( ptr->getAddr() );

}

Uchar& Buffer::operator [] ( const Uint32 pos ) {

	if ( pos >= ptr->getUser() )
		return dummy_char;

	if ( ptr->getRefs() > 1 ) {

		BufferData *newPtr = new BufferData;
		newPtr->setUser( ptr->getUser() );
		newPtr->setData( 0, ptr->getAddr(), ptr->getUser() );
		ptr->decRefs();
		ptr = newPtr;

	}

	return *( ptr->getAddr() + pos );

}

Uchar Buffer::operator [] ( 
	const	Uint32		pos ) const {

	if ( pos >= ptr->getUser() )
		return '\0';

	return *( ptr->getAddr() + pos );

}

Buffer Buffer::operator () ( 
	const	Uint32		beg,
	const	Uint32		len ) const {

	if ( beg >= ptr->getUser() ) {
		return Buffer();
	}

	if ( beg + len > ptr->getUser() ) {
		return Buffer( ptr->getAddr() + beg, ptr->getUser() - beg );
	}

	return Buffer( ptr->getAddr() + beg, len );

}

Buffer Buffer::spaces( const Uint32 l ) {

	return chars( l, ' ' );

}

Buffer Buffer::chars( 
	const	Uint32	l,
	const	Uchar	c ) {

	Buffer res;

	res.resize( l );

	for ( Uint32 i = 0 ; i < l ; i++ )
		res[i] = c;

	return res;

}

Int32 Buffer::getIntAt( Uint32& i ) const {

	if ( i >= ptr->getUser() ) {
		i = 0;
		return 0;
	}

	// OK. Let's build our own `atoi' function...

	Uchar * const buf = ptr->getAddr();
	Bool sign;

	if ( buf[i] == '-' ) {
		sign = true;
		i++;
	}
	else if ( buf[i] == '+' ) {
		sign = false;
		i++;
	}
	else
		sign = false;

	Int32	result = 0;
	Uint32	initpos = i;
	Uchar	c;

	if ( i + 2 < ptr->getUser()
	 && buf[i] == '0'
	 && ( buf[i+1] == 'x' || buf[i+1] == 'X' ) ) {
		i += 2;
		initpos = i;
		while ( i < ptr->getUser() ) {
			c = buf[i];
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
			else
				break;
			i++;
		}
	}

	else {
		while ( i < ptr->getUser() ) {
			c = buf[i];
			if ( c < '0' || c > '9' )
				break;
			result *= 10;
			result += ( c - '0' );
			i++;
		}
	}

	if ( i == initpos ) { // we didn't read anything, beside the optional '-'
		i = 0;
		return 0;
	}

	if ( sign )
		return -result;

	return result;

}

Uint32 Buffer::getUintAt( Uint32& i ) const {

	if ( i >= ptr->getUser() ) {
		i = 0;
		return 0;
	}

	Uchar * const	buf = ptr->getAddr();
	Uint32		result = 0,
			initpos = i;
	Uchar		c;

	if ( i + 2 < ptr->getUser()
	 && buf[i] == '0'
	 && ( buf[i+1] == 'x' || buf[i+1] == 'X' ) ) {
		i += 2;
		initpos = i;
		while ( i < ptr->getUser() ) {
			c = buf[i];
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
			else
				break;
			i++;
		}
	}

	else {
		while ( i < ptr->getUser() ) {
			c = buf[i];
			if ( c < '0' || c > '9' )
				break;
			result *= 10;
			result += ( c - '0' );
			i++;
		}
	}

	if ( i == initpos ) { // we didn't read anything
		i = 0;
		return 0;
	}

	return result;

}

Uint64 Buffer::getUlonglongAt( Uint32& i ) const {

	if ( i >= ptr->getUser() ) {
		i = 0;
		return 0;
	}

	Uchar * const	buf = ptr->getAddr();
	Uint32		initpos = i;
	Uint64	result = 0;
	Uchar		c;

	if ( i + 2 < ptr->getUser()
	 && buf[i] == '0'
	 && ( buf[i+1] == 'x' || buf[i+1] == 'X' ) ) {
		i += 2;
		initpos = i;
		while ( i < ptr->getUser() ) {
			c = buf[i];
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
			else
				break;
			i++;
		}
	}

	else {
		while ( i < ptr->getUser() ) {
			c = buf[i];
			if ( c < '0' || c > '9' )
				break;
			result *= 10;
			result += ( c - '0' );
			i++;
		}
	}

	if ( i == initpos ) { // we didn't read anything
		i = 0;
		return 0;
	}

	return result;

}

Uint16 Buffer::toUint16() const {

	Uint32	pos = 0;
	Uint16	res = ( Uint16 )getUintAt( pos );

	return ( pos ? res : 0 );

}

Uint32 Buffer::toUint32() const {

	Uint32	pos = 0;
	Uint32	res = getUintAt( pos );

	return ( pos ? res : 0 );

}

Uint64 Buffer::toUint64() const {

	Uint32	pos = 0;
	Uint64	res = getUlonglongAt( pos );

	return ( pos ? res : 0 );

}

Int32 Buffer::toInt32() const {

	Uint32	pos = 0;
	Int32	res = getIntAt( pos );

	return ( pos ? res : 0 );

}

Double Buffer::getDoubleAt( Uint32& i ) const {

	if ( i >= ptr->getUser() ) {
		i = 0;
		return 0;
	}

	// OK. Let's build our own `atof' function...

	Uchar * const buf = ptr->getAddr();
	Bool	sign;
	Bool	frac = false;

	if ( buf[i] == '-' ) {
		sign = true;
		i++;
	}
	else if ( buf[i] == '+' ) {
		sign = false;
		i++;
	}
	else
		sign = false;

	Double	result = 0.0;
	Double	factor = 0.1;
	Uint32	initpos = i;
	Uchar	c;

	while ( i < ptr->getUser() ) {
		c = buf[i];
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

	if ( sign )
		return -result;

	return result;

}

Double Buffer::toDouble() const {

	Uint32	pos = 0;

	return getDoubleAt( pos );

}

Bool Buffer::toBool() const {

	Buffer b( *this );

	return ( b == "yes" || b == "y" || b == "YES" || b == "Y"
	      || b == "true" || b == "TRUE"
	      || b == "1" );

}

Bool Buffer::getLineAt( Uint32& i, Buffer& line ) const {

	line.kill();

	if ( i >= ptr->getUser() )
		return false;

	Uint32	beg = i;
	Uchar		c;

	while ( i < ptr->getUser() ) {
		c = ptr->getAddr()[i];
		if ( c != '\r' && c != '\n' ) {
			i++;
			continue;
		}
		if ( c == '\r' ) {
			if ( i + 1 == ptr->getUser() ) {
				// This `CR' is the last char!
				i++;
				return true;
			}
			if ( ptr->getAddr()[i+1] != '\n' ) {
				// This `CR' is a regular char in the string.
				i++;
				continue;
			}
			if ( i > beg )
				line = Buffer( ptr->getAddr() + beg, i - beg );
			i += 2;
			return true;
		}
		// We got a `NL'!
		if ( i > beg )
			line = Buffer( ptr->getAddr() + beg, i - beg );
		i++;
		return true;
	}
	line = Buffer( ptr->getAddr() + beg, i - beg );
	return true;

}

Bool Buffer::findBufferAt( 
	Uint32& pos,
	const Buffer& substring ) const {

	const BufferData *subptr = substring.ptr;

	if ( ! subptr->getUser() )
		return false;

	if ( pos + subptr->getUser() > ptr->getUser() )
		return false;

	Uint32
		maxpos = ptr->getUser() - subptr->getUser(),
		counter;
	const Uchar
		*subdata,
		*tmpdata;

	while ( pos <= maxpos ) {

		tmpdata = ptr->getAddr() + pos;
		subdata = subptr->getAddr();
		counter = subptr->getUser();
		while ( counter ) {
			if ( *tmpdata != *subdata )
				break;
			tmpdata++;
			subdata++;
			counter--;
		}

		if ( ! counter ) // found !!!
			return true;

		pos++;

	}

	return false;

}

Bool Buffer::contains( const Uchar c ) const {

	if ( ! ptr->getUser() )
		return false;

	for ( Uint32 i = 0 ; i < ptr->getUser() ; i++ )
		if ( ptr->getAddr()[i] == c )
			return true;

	return false;

}

Bool Buffer::contains( const Buffer& s ) const {

	Uint32 pos = 0;

	return findBufferAt( pos, s );

}

Bool Buffer::beginsWith( 
	const	Buffer&		e ) const {

	Uint32 t1 = ptr->getUser();
	Uint32 t2 = e.ptr->getUser();

	if ( t1 < t2 ) {
		return false;
	}

	if ( t1 == t2 ) {
		return ( *this == e );
	}

	const Uchar *p1 = ptr->getAddr();
	const Uchar *p2 = e.ptr->getAddr();

	while ( t2-- ) {
		if ( *p1++ != *p2++ ) {
			return false;
		}
	}

	return true;

}

Bool Buffer::endsWith( 
	const	Buffer&		e ) const {

	Uint32 t1 = ptr->getUser();
	Uint32 t2 = e.ptr->getUser();

	if ( t1 < t2 ) {
		return false;
	}

	if ( t1 == t2 ) {
		return ( *this == e );
	}

	const Uchar *p1 = ptr->getAddr() + t1 - t2;
	const Uchar *p2 = e.ptr->getAddr();

	while ( t2-- ) {
		if ( *p1++ != *p2++ ) {
			return false;
		}
	}

	return true;

}

Uint32 Buffer::findClosingChar( const Uint32 start ) const {

	if ( start >= ptr->getUser() )
		return 0;

	Uchar	closechar,
		openchar = ptr->getAddr()[start];

	switch ( ptr->getAddr()[start] ) {
	case '(' : closechar = ')'; break;
	case '[' : closechar = ']'; break;
	case '{' : closechar = '}'; break;
	default  : return 0;
	}

	Uint32	levels = 1,    // we already got one opening bracket
		i = start + 1; // pos to start the search
	Uchar	current;

	for ( ;; ) {

		if ( i >= ptr->getUser() )
			return 0;

		current = ptr->getAddr()[i];
		if ( current == openchar )
			levels++;
		else if ( current == closechar )
			levels--;

		if ( levels == 0 )
			break;

		i++;

	}

	return i;

}

Uint32 Buffer::findFwd(
	const	Uchar		c,
	const	Uint32		pos) const {

	Uint32 cnt = length();
	Uint32 res;

	if ( pos < length() ) {
		cnt = length() - pos;
		res = pos;
	}
	else {
		cnt = length();
		res = 0;
	}

	const Uchar *p = ptr->getAddr();

	while ( cnt-- ) {
		if ( p[res] == c ) {
			break;
		}
		res++;
	}

	return res;

}

Uint32 Buffer::findRev(
	const	Uchar		c,
	const	Uint32		pos) const {

	Uint32 cnt = length();
	Uint32 res;

	if ( pos < length() && pos != (Uint32)-1) {
		cnt = pos + 1;
		res = pos;
	}
	else {
		cnt = length();
		res = cnt - 1;
	}

	const Uchar *p = ptr->getAddr();

	while ( cnt-- ) {
		if ( p[res] == c ) {
			break;
		}
		res--;
	}

	return res;

}

Buffer Buffer::toLower() const {

	Buffer res = *this;

	for ( Uint32 i = 0 ; i < res.length() ; i++ ) {
		if ( res[i] >= 'A' && res[i] <= 'Z' ) {
			res[i] = res[i] - 'A' + 'a';
		}
	}

	return res;

}

Buffer Buffer::toUpper() const {

	Buffer res = *this;

	for ( Uint32 i = 0 ; i < res.length() ; i++ ) {
		if ( res[i] >= 'a' && res[i] <= 'z' ) {
			res[i] = res[i] - 'a' + 'A';
		}
	}

	return res;

}

Bool Buffer::dumpToBuffer( Uchar buf[], const Uint32 len ) const {

	if ( len <= ptr->getUser() ) {
		if ( len > 0 )
			buf[0] = '\0';
		return false;
	}

	::memcpy( buf, ptr->getAddr(), ptr->getUser() );
	buf[ptr->getUser()] = '\0';

	return true;

}

Bool Buffer::safeDumpToBuffer( Uchar buf[], const Uint32 len ) const {

	if ( len < ptr->getUser() ) {
		return false;
	}

	::memcpy( buf, ptr->getAddr(), ptr->getUser() );

	return true;

}

void Buffer::resize( const Uint32 newsize ) {

	// some kind of realloc, in fact ;- )

	if ( ! newsize ) {
		kill();
		return;
	}

	if ( newsize == ptr->getUser() ) {
		return;
	}

	if ( ptr->getUser() == 0 ) {
		if ( ptr->getRefs() > 1 ) {
			// in fact, we were sharing it
			ptr->decRefs();
			ptr = new BufferData;
		}
		ptr->setUser( newsize );
		return;
	}

	if ( ptr->getRefs() == 1
	  && newsize <= ptr->getPhys() ) {
		ptr->setUser( newsize );
		return;
	}

	BufferData *newPtr = new BufferData;

	newPtr->setUser( newsize );

	if ( newsize < ptr->getUser() ) {
		newPtr->setData( 0, ptr->getAddr(), newsize );
	}
	else {
		newPtr->setData( 0, ptr->getAddr(), ptr->getUser() );
	}

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

	ptr = newPtr;

}

void Buffer::supposedMaxSize( const Uint32 s ) {

	if ( s <= ptr->getPhys() ) {
		return;
	}

	BufferData *newPtr = new BufferData;

	newPtr->setPhys( s );

	if ( ptr->getUser() ) {
		newPtr->setUser( ptr->getUser() );
		newPtr->setData( 0, ptr->getAddr(), ptr->getUser() );
	}

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

	ptr = newPtr;

}

void Buffer::kill() {

	if ( ! ptr->decRefs() ) {
		delete ptr;
	}

	ptr = new BufferData;

}

void Buffer::ensureSingleUser() {

	if ( ptr->getRefs() > 1 ) {

		BufferData *newPtr = new BufferData;

		if ( ptr->getUser() ) {
			newPtr->setUser(
				ptr->getUser() );
			newPtr->setData(
				0,
				ptr->getAddr(),
				ptr->getUser() );
		}

		ptr->decRefs();

		ptr = newPtr;

	}

}

// ============================================================================

Uchar Buffer::dummy_char;

// ============================================================================

#if 0

std::ostream& BFC::operator << ( std::ostream& os, const Buffer& src ) {

	if ( src.isEmpty() )
		return os;

	os.write( ( const char * )src.getBufferAddr(), src.length() );

	return os;

}

std::istream& BFC::operator >> ( std::istream& is, Buffer& src ) {

	Uchar buffer[4096];

	is.getline( ( char * )buffer, 4095 );

	src = Buffer( buffer );

	return is;

}

#endif

// ============================================================================

Buffer BFC::operator + ( const Char *s1, const Buffer& s2 ) {

	return ( Buffer( s1 ) + s2 );
}

// ============================================================================




