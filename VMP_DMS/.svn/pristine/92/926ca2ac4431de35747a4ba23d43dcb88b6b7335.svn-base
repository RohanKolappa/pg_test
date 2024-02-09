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
//	BFC.Time.Delta.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Time.Delta.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Buffer Time::Delta::toBuffer() const {

	Bool	s = ( d < 0 );
	Uint64	v = ( d < 0 ? ( Uint64 )( -d ) : ( Uint64 )( d ) );

	Uint32	usecs = ( Uint32 )(v % 1000000); v /= 1000000;
	Uint32	secs  = ( Uint32 )(v %      60); v /=      60;
	Uint32	mins  = ( Uint32 )(v %      60); v /=      60;
	Uint32	hours = ( Uint32 )(v %      24); v /=      24;
	Uint32	days  = ( Uint32 )(v          );

	Buffer	res;

	res.supposedMaxSize( 21 + ( s ? 1 : 0 ) );

	if ( s ) {
		res += '-';
	}

	res += Buffer( days , Buffer::Base10, 5 );
	res += '.';
	res += Buffer( hours, Buffer::Base10, 2 );
	res += ':';
	res += Buffer( mins , Buffer::Base10, 2 );
	res += ':';
	res += Buffer( secs , Buffer::Base10, 2 );
	res += '.';
	res += Buffer( usecs, Buffer::Base10, 6 );

	return res;

}

// ============================================================================

const Time::Delta Time::OneYear  ( 60 * 60 * 24 * 365, 0 );
const Time::Delta Time::OneBYear ( 60 * 60 * 24 * 366, 0 );
const Time::Delta Time::OneDay   ( 60 * 60 * 24, 0 );
const Time::Delta Time::OneHour  ( 60 * 60, 0 );
const Time::Delta Time::OneMinute( 60, 0 );
const Time::Delta Time::OneSecond( 1, 0 );

// ============================================================================

