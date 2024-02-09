// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "JBS".
// 
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	JBS.BuildConfig.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.BuildConfig.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

Buffer BuildConfig::toBuffer() const {

	Buffer msg1, msg2;

	switch ( dbgMode ) {
	case No : msg1 = "release"; break;
	case Yes : msg1 = "debug"; break;
	default :
		break;
	}

	switch ( staMode ) {
	case No : msg2 = "shared"; break;
	case Yes : msg2 = "static"; break;
	default :
		break;
	}

	if ( msg1.isEmpty() ) return msg2;
	if ( msg2.isEmpty() ) return msg1;
	return msg1 + ", " + msg2;

}

// ============================================================================

Buffer BuildConfig::getVal(
	const	UserFlag	v ) {

	switch ( v ) {
	case No		: return Buffer( "no" );
	case Yes	: return Buffer( "yes" );
	case DontKnow	: return Buffer( "/" );
	}

	return Buffer();

}

// ============================================================================

Bool BuildConfig::isCompatibleWith(
	const	BuildConfig&	o ) const {

	return ( ( dbgMode != No || o.dbgMode != Yes )
	      && ( dbgMode != Yes || o.dbgMode != No )
	      && ( staMode != No || o.staMode != Yes )
	      && ( staMode != Yes || o.staMode != No ) );

}

// ============================================================================

