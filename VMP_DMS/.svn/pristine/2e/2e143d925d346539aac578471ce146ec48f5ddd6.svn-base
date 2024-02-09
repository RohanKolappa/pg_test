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
//	BFC.Base.BufferIO.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferIO.h"
#include "BFC.Base.System.h"

// ============================================================================

using namespace BFC;

// ============================================================================

std::ostream & BFC::operator << (
		std::ostream	& os,
	const	Buffer		& src ) {

	if ( src.isEmpty() )
		return os;

	os.write( ( const char * )src.getCstPtr(), src.getLength() );

	return os;

}

std::istream & BFC::operator >> (
		std::istream	& is,
		Buffer		& src ) {

	Uchar buffer[ 4096 ];

	is.getline( ( char * )buffer, 4095 );

	src = Buffer( buffer, System::strlen( buffer ), true );

	return is;

}

// ============================================================================




