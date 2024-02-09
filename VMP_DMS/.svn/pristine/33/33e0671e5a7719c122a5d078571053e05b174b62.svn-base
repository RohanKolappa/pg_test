// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	Buffer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <string.h>

#include "Buffer.h"

// ============================================================================

Buffer::Buffer() {

	base = 0;
	used = 0;
	memo = 0;

}

Buffer::Buffer(
	const	Buffer&		b ) {

	if ( b.used ) {
		memo = b.memo;
		base = new char [memo];
		used = b.used;
		::memcpy( base, b.base, used );
	}
	else {
		base = 0;
		used = 0;
		memo = 0;
	}

}

Buffer::Buffer(
	const	char		*p,
	const	int		m ) {

	int l = ( m == -1 ? ::strlen( p ) : m );

	if ( l ) {
		memo = l;
		base = new char [memo];
		used = l;
		::memcpy( base, p, used );
	}
	else {
		base = 0;
		used = 0;
		memo = 0;
	}

}

Buffer::~Buffer() {

	if ( memo ) {
		delete[] base;
	}

}

// ============================================================================

Buffer& Buffer::operator = ( const Buffer& b ) {

	if ( this == &b ) {
		return *this;
	}

	if ( used ) {
		used = 0;
	}

	if ( memo ) {
		delete[] base;
		base = 0;
		memo = 0;
	}

	if ( b.used ) {
		memo = b.memo;
		base = new char [memo];
		used = b.used;
		::memcpy( base, b.base, used );
	}

	return *this;

}

// ============================================================================

bool Buffer::operator == ( const Buffer& p ) const {

	int l = p.length();

	if ( l != used ) {
		return false;
	}

	if ( ! l ) {
		return true;
	}

	return ( ::strncmp( base, p.base, l ) == 0 );

}

bool Buffer::operator != ( const Buffer& p ) const {

	return ( ! ( *this == p ) );

}

// ============================================================================

bool Buffer::contains( const char c ) const {

	for ( int i = 0 ; i < used ; i++ ) {
		if ( base[i] == c ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

Buffer& Buffer::operator += ( const Buffer& b ) {

	if ( b.isEmpty() ) {
		return *this;
	}

	int f = memo - used;

	if ( f < b.used ) {
		int nm = used + b.used;
		char *nb = new char [nm];
		if ( used ) {
			::memcpy( nb, base, used );
		}
		if ( memo ) {
			delete[] base;
		}
		base = nb;
		memo = nm;
	}

	::memcpy( base + used, b.base, b.used );

	used += b.used;

	return *this;

}

Buffer& Buffer::operator += ( const char c ) {

	if ( used == memo ) {
		int nm = used + 4096;
		char *nb = new char [nm];
		if ( used ) {
			::memcpy( nb, base, used );
		}
		if ( memo ) {
			delete[] base;
		}
		base = nb;
		memo = nm;
	}

	base[used] = c;

	used++;

	return *this;

}

// ============================================================================

Buffer Buffer::operator + ( const Buffer& b ) const {

	Buffer res = *this;

	res += b;

	return res;

}

Buffer Buffer::operator + ( const char c ) const {

	Buffer res = *this;

	res += c;

	return res;

}

// ============================================================================

Buffer Buffer::operator () ( const int start ) const {

	if ( start >= used ) {
		return Buffer();
	}
	else {
		return Buffer( base + start, used - start );
	}

}

Buffer Buffer::operator () ( const int start, const int len ) const {

	if ( start >= used ) {
		return Buffer();
	}
	else if ( len > used - start ) {
		return Buffer( base + start, used - start );
	}
	else {
		return Buffer( base + start, len );
	}

}

// ============================================================================

std::ostream& operator << ( std::ostream& os, const Buffer& b ) {

	if ( b.isEmpty() ) {
		return os;
	}

	os.write( b.base, b.used );

	return os;

}

Buffer operator + ( const char *p, const Buffer& b ) {

	Buffer res( p );

	res += b;

	return res;

}

// ============================================================================

