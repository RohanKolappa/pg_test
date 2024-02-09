// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Time.Clock.cpp
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
#else
#include <time.h>
#include <sys/time.h>
#endif

//#include <iostream>

#include "TBFC.Base.BufferParser.h"
#include "TBFC.Base.System.h"

#include "TBFC.TL.BufferArray.h"

#include "TBFC.Time.Clock.h"

// ============================================================================

//using namespace std;
using namespace TBFC;
using namespace TBFC::Time;

// ============================================================================

void Clock::waitUntilReached() const {

	Uint64 n = now().u;

	if ( u > n ) {
		System::usleep( u - n );
	}

}

// ============================================================================

void Clock::buildFrom(
	const	Buffer		& buf ) {

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

void Clock::buildFrom(
	const	Buffer		& src,
	const	Buffer		& pattern ) {

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
				year = val;
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

	struct tm	v_tm;

	v_tm.tm_sec = second;
	v_tm.tm_min = minute;
	v_tm.tm_hour = hour;
	v_tm.tm_mday = day;
	v_tm.tm_mon = month;
	v_tm.tm_year = year - 1900;
	v_tm.tm_isdst = 0;

	u = ( Uint64 )::mktime( & v_tm ) * ( Uint64 )1000000;

}

// ============================================================================

Buffer Clock::toBuffer() const {

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

Buffer Clock::dateToBuffer() const {

//	cerr << "dateToBuffer(): u == " << u << endl;

	Buffer	res;
	/* Uint32 */ time_t	secs = ( Uint32 )( u / 1000000 );

	res.supposedMaxSize( 16 );

	struct tm * v_tm;

#if defined( PLATFORM_WIN32 )
	struct tm r_tm;
	v_tm = &r_tm;
	::localtime_s( v_tm, ( const time_t * )&secs );
#else
	v_tm = ::localtime( ( const time_t * )&secs );
#endif

	res += Buffer( ( Uint32 )( v_tm->tm_year + 1900 ), Buffer::Base10, 4 );
	res += "-";
	res += Buffer( ( Uint32 )( v_tm->tm_mon + 1 ), Buffer::Base10, 2 );
	res += "-";
	res += Buffer( ( Uint32 )( v_tm->tm_mday ), Buffer::Base10, 2 );

	return res;

}

Buffer Clock::timeToBuffer() const {

//	cerr << "dateToBuffer(): u == " << u << endl;

	Buffer	res;
	/* Uint32 */ time_t	secs = ( Uint32 )( u / ( Uint64 )1000000 );

	res.supposedMaxSize( 16 );

	struct tm * v_tm;

#if defined( PLATFORM_WIN32 )
	struct tm r_tm;
	v_tm = &r_tm;
	::localtime_s( v_tm, ( const time_t * )&secs );
#else
	v_tm = ::localtime( ( const time_t * )&secs );
#endif

	res += Buffer( ( Uint32 )( v_tm->tm_hour ), Buffer::Base10, 2 );
	res += ":";
	res += Buffer( ( Uint32 )( v_tm->tm_min ), Buffer::Base10, 2 );
	res += ":";
	res += Buffer( ( Uint32 )( v_tm->tm_sec ), Buffer::Base10, 2 );

	return res;

}

Buffer Clock::dateTimeToBuffer() const {

	Buffer	res;
	/* Uint32 */ time_t secs = ( Uint32 )( u / 1000000 );

	struct tm * v_tm;

#if defined( PLATFORM_WIN32 )
	struct tm r_tm;
	v_tm = &r_tm;
	::localtime_s( v_tm, ( const time_t * )&secs );
#else
	v_tm = ::localtime( ( const time_t * )&secs );
#endif

	res.supposedMaxSize( 32 );

	res += Buffer( ( Uint32 )( v_tm->tm_year + 1900 ), Buffer::Base10, 4 );
	res += "-";
	res += Buffer( ( Uint32 )( v_tm->tm_mon + 1 ), Buffer::Base10, 2 );
	res += "-";
	res += Buffer( ( Uint32 )( v_tm->tm_mday ), Buffer::Base10, 2 );
	res += "_";
	res += Buffer( ( Uint32 )( v_tm->tm_hour ), Buffer::Base10, 2 );
	res += ":";
	res += Buffer( ( Uint32 )( v_tm->tm_min ), Buffer::Base10, 2 );
	res += ":";
	res += Buffer( ( Uint32 )( v_tm->tm_sec ), Buffer::Base10, 2 );

	return res;

}

// ============================================================================

Clock Time::now() {

#if defined( PLATFORM_WIN32 )
	struct timeb tb;
	::ftime( &tb );

//	cerr << "now: " << tb.time << ", " << tb.millitm << endl;

	return Clock( ( Uint32 )( tb.time ), ( Uint32 )( tb.millitm ) * 1000 );
#else
	struct timeval tv;
	::gettimeofday( &tv, 0 );

//	cerr << "now: " << tv.tv_sec << ", " << tv.tv_usec << endl;

	return Clock( ( Uint32 )( tv.tv_sec ), ( Uint32 )( tv.tv_usec ) );
#endif
}

// ============================================================================

