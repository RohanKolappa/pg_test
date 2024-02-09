// ============================================================================
//
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
//	BFC.Time.Clock.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <string.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

#include "BFC.Base.BufferParser.h"
#include "BFC.Base.System.h"

#include "BFC.TL.BufferArray.h"

#include "BFC.Time.Clock.h"

// ============================================================================

using namespace BFC;

// ============================================================================

#if defined( PLATFORM_WIN32 )

time_t my_timegm(
		tm *		tm ) {

	time_t ret;
	const char* tzs = "TZ=";
	const char* tz;
	tz = getenv("TZ");
	if (tz) {
		char* zwzw = new char[strlen(tzs) + strlen(tz)+1];
		strcpy(zwzw, tzs);
		strcat(zwzw, tz);
		_putenv(tzs);
		_tzset();
		tz = zwzw;
	}
	ret = mktime(tm);
	if (tz) {
		_putenv(tz);
		_tzset();
		delete tz;
	}
	return ret;

}

#elif defined( PLATFORM_DARWIN )

#include <stdlib.h>

time_t my_timegm(
		tm *		tm ) {

	time_t ret;
	char *tz;

	tz = getenv("TZ");
	setenv("TZ", "", 1);
	tzset();
	ret = mktime(tm);
	if (tz)
		setenv("TZ", tz, 1);
	else
		unsetenv("TZ");
	tzset();
	return ret;
}

#elif defined( PLATFORM_LINUX )

#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t my_timegm(
		tm *		tm ) {

	char * tz = getenv( "TZ" );

	if ( tz ) {
		tz = ::strdup( tz );
	}

	::setenv( "TZ", "", 1 );
	::tzset();
	time_t ret = ::mktime( tm );

	if ( tz ) {
		::setenv( "TZ", tz, 1 );
		::free( tz );
	}
	else {
		::unsetenv("TZ");
	}

	::tzset();

	return ret;

}

#else

#define my_timegm ::timegm

#endif

Time::Clock::Clock(
	const	Uint32		ye,
	const	Uint32		mo,
	const	Uint32		da,
	const	Uint32		ho,
	const	Uint32		mi,
	const	Uint32		se,
	const	Uint32		ms ) {

	tm	v_tm;

	System::bzero( ( Uchar * )&v_tm, sizeof( v_tm ) );

	v_tm.tm_sec	= se;
	v_tm.tm_min	= mi;
	v_tm.tm_hour	= ho;
	v_tm.tm_mday	= da;
	v_tm.tm_mon	= mo - 1;
	v_tm.tm_year	= ye - 1900;
	v_tm.tm_isdst	= 0;

	u = ( Uint64 )my_timegm( &v_tm ) * ( Uint64 )1000000 + ( Uint64 )ms;

}

// ============================================================================

void Time::Clock::waitUntilReached() const {

	Uint64 n = now().u;

	if ( u > n ) {
		System::usleep( u - n );
	}

}

// ============================================================================

void Time::Clock::buildFrom(
	const	Buffer &	buf ) {

	BufferArray	tokens;

	tokens = BufferParser::tokenize( buf, "." );

	Uint64	days  = ( Uint64 )( tokens[ 0 ].toUint32() );
	Uint64	usecs = ( Uint64 )( tokens[ 2 ].toUint32() );

	tokens = BufferParser::tokenize( tokens[ 1 ], ":" );

	Uint64	hours = ( Uint64 )( tokens[ 0 ].toUint32() );
	Uint64	mins  = ( Uint64 )( tokens[ 1 ].toUint32() );
	Uint64	secs  = ( Uint64 )( tokens[ 2 ].toUint32() );

	u = usecs
	  + ( Uint64 )( 1000000 )
	  * ( secs
	    + ( Uint64 )( 60 )
	    * ( mins
	      + ( Uint64 )( 60 )
	      * ( hours
	        + ( Uint64 )( 24 )
	        * days ) ) );

}

// ============================================================================

void Time::Clock::buildFrom(
	const	Buffer &	src,
	const	Buffer &	pattern ) {

	Uint32	srcidx = 0;
	Uint32	patidx = 0;
	Uint32	pos;
	Uint32	val;
	Uchar	c;
	Uint32	second = 0;
	Uint32	minute = 0;
	Uint32	hour   = 0;
	Uint32	day    = 0;
	Uint32	month  = 0;
	Uint32	year   = 0;

	while (patidx < pattern.length()) {

		c = pattern[patidx++];

		if (c == 's' || c == 'm' || c == 'h'
		 || c == 'D' || c == 'M' || c == 'Y') {
			pos = srcidx;
			val = src.getUintAt(pos);
			if (pos == srcidx)
				throw InvalidArgument(src);
			srcidx = pos;
			switch (c) {
			case 's' :
				if (val >= 60)
					throw InvalidArgument();
				second = val;
				break;
			case 'm' :
				if (val >= 60)
					throw InvalidArgument();
				minute = val;
				break;
			case 'h' :
				if (val >= 24)
					throw InvalidArgument();
				hour = val;
				break;
			case 'D' :
				if (val == 0 || val >= 32)
					throw InvalidArgument();
				day = val;
				break;
			case 'M' :
				if (val == 0 || val >= 13)
					throw InvalidArgument();
				month = val - 1;
				break;
			case 'Y' :
				if (val < 1970)
					throw InvalidArgument();
				year = val - 1900;
				break;
			}
		}

		else if (srcidx >= src.length()) {
			throw InvalidArgument( "Pattern too short!" );
		}

		else if (src[srcidx++] != c) {
			throw InvalidArgument( "Invalid pattern!" );
		}

	}

	if (srcidx != src.length()) {
		throw InvalidArgument( "Junk at end!" );
	}

	tm	v_tm;

	v_tm.tm_sec = second;
	v_tm.tm_min = minute;
	v_tm.tm_hour = hour;
	v_tm.tm_mday = day;
	v_tm.tm_mon = month;
	v_tm.tm_year = year;
	v_tm.tm_isdst = 0;

	u = ( Uint64 )my_timegm( & v_tm ) * ( Uint64 )1000000;

}

// ============================================================================

Buffer Time::Clock::toBuffer() const {

	Buffer	res;
	Uint64	v = u;

	res.supposedMaxSize( 24 );

	// usecs

	Uint32	usecs = ( Uint32 )(v % 1000000); v /= 1000000;
	Uint32	secs  = ( Uint32 )(v %      60); v /=      60;
	Uint32	mins  = ( Uint32 )(v %      60); v /=      60;
	Uint32	hours = ( Uint32 )(v %      24); v /=      24;
	Uint32	days  = ( Uint32 )(v          );

	res += Buffer( days , Buffer::Base10, 5 );
	res += ".";
	res += Buffer( hours, Buffer::Base10, 2 );
	res += ":";
	res += Buffer( mins , Buffer::Base10, 2 );
	res += ":";
	res += Buffer( secs , Buffer::Base10, 2 );
	res += ".";
	res += Buffer( usecs, Buffer::Base10, 6 );

	return res;

}

// ============================================================================

Buffer Time::Clock::dateToBuffer(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	Buffer	res;

	res.supposedMaxSize( 16 );

	res += Buffer( ye, Buffer::Base10, 4 );
	res += "-";
	res += Buffer( mo, Buffer::Base10, 2 );
	res += "-";
	res += Buffer( da, Buffer::Base10, 2 );

	return res;

}

Buffer Time::Clock::timeToBuffer(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	Buffer	res;

	res.supposedMaxSize( 16 );

	res += Buffer( ho, Buffer::Base10, 2 );
	res += ":";
	res += Buffer( mi, Buffer::Base10, 2 );
	res += ":";
	res += Buffer( se, Buffer::Base10, 2 );

	return res;

}

Buffer Time::Clock::dateTimeToBuffer(
	const	Bool		pLocalTime,
	const	Bool		pWithUSecs ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	Buffer	res;

	res.supposedMaxSize( 32 );

	res += Buffer( ye, Buffer::Base10, 4 );
	res += "-";
	res += Buffer( mo, Buffer::Base10, 2 );
	res += "-";
	res += Buffer( da, Buffer::Base10, 2 );
	res += "_";
	res += Buffer( ho, Buffer::Base10, 2 );
	res += ":";
	res += Buffer( mi, Buffer::Base10, 2 );
	res += ":";
	res += Buffer( se, Buffer::Base10, 2 );
	if ( pWithUSecs ) {
		res += ".";
		res += Buffer( us, Buffer::Base10, 6 );
	}

	return res;

}

// ============================================================================

Uint32 Time::Clock::getYear(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	return ye;

}

Uint32 Time::Clock::getMonth(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	return mo;

}

Uint32 Time::Clock::getDay(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	return da;

}

Uint32 Time::Clock::getHour(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	return ho;

}

Uint32 Time::Clock::getMinute(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	return mi;

}

Uint32 Time::Clock::getSecond(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	return se;

}

Uint32 Time::Clock::getuSecond(
	const	Bool		pLocalTime ) const {

	Uint32	ye, mo, da, ho, mi, se, us;

	getSplitTime( ye, mo, da, ho, mi, se, us, pLocalTime );

	return us;

}

void Time::Clock::getSplitTime(
		Uint32 &	ye,
		Uint32 &	mo,
		Uint32 &	da,
		Uint32 &	ho,
		Uint32 &	mi,
		Uint32 &	se,
		Uint32 &	us,
	const	Bool		pLocalTime ) const {

	Uint32	wd, yd;

	getSplitTime( ye, mo, da, ho, mi, se, us, wd, yd, pLocalTime );

}

void Time::Clock::getSplitTime(
		Uint32 &	ye,
		Uint32 &	mo,
		Uint32 &	da,
		Uint32 &	ho,
		Uint32 &	mi,
		Uint32 &	se,
		Uint32 &	us,
		Uint32 &	wd,
		Uint32 &	yd,
	const	Bool		pLocalTime ) const {

//	time_t	secs = ( Uint32 )( u / 1000000 );
	time_t	secs = ( time_t )( u / 1000000 );
	tm	r_tm;

#if defined( PLATFORM_WIN32 )
	if ( pLocalTime ) {
		::localtime_s( &r_tm, ( const time_t * )&secs );
	}
	else {
		::gmtime_s( &r_tm, ( const time_t * )&secs );
	}
#else
	if ( pLocalTime ) {
		::localtime_r( &secs, &r_tm );
	}
	else {
		::gmtime_r( &secs, &r_tm );
	}
#endif

	ye = ( Uint32 )( r_tm.tm_year + 1900 );
	mo = ( Uint32 )( r_tm.tm_mon + 1 );
	da = ( Uint32 )( r_tm.tm_mday );
	ho = ( Uint32 )( r_tm.tm_hour );
	mi = ( Uint32 )( r_tm.tm_min );
	se = ( Uint32 )( r_tm.tm_sec );
	us = ( Uint32 )( u % 1000000 );
	wd = ( Uint32 )( r_tm.tm_wday );
	yd = ( Uint32 )( r_tm.tm_yday );

}

// ============================================================================

Time::Clock Time::now(
	const	Bool		pLocalTime ) {

#if defined( PLATFORM_WIN32 )

	if ( ! pLocalTime ) {
		throw NotImplemented();
	}

	timeb tb;
	::ftime( &tb );

//	cerr << "now: " << tb.time << ", " << tb.millitm << endl;

	return Clock( ( Uint32 )( tb.time ), ( Uint32 )( tb.millitm ) * 1000 );

#else

	if ( pLocalTime ) {
		timeval	tv;
		::gettimeofday( &tv, 0 );
		return Clock( ( Uint32 )( tv.tv_sec ), ( Uint32 )( tv.tv_usec ) );
	}
	else {
		timespec ts;
		::clock_gettime( CLOCK_REALTIME, &ts );
		return Clock( ( Uint32 )( ts.tv_sec ), ( Uint32 )( ts.tv_nsec / 1000 ) );
	}

#endif

}

// ============================================================================

const Time::Clock Time::Epoch( 1970, 1, 1, 0, 0, 0 );

// ============================================================================

