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
//	BFC.Mem.UnivU32Reader.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Mem.UnivU32Reader.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::UnivU32Reader::GetTableEntry Mem::UnivU32Reader::getTable[] = {
	{
		& UnivU32Reader::getStart08,
		& UnivU32Reader::getIndex08,
		& UnivU32Reader::getXYPos08
	},
	{
		& UnivU32Reader::getStart16,
		& UnivU32Reader::getIndex16,
		& UnivU32Reader::getXYPos16
	},
	{
		& UnivU32Reader::getStart24,
		& UnivU32Reader::getIndex24,
		& UnivU32Reader::getXYPos24
	},
	{
		& UnivU32Reader::getStart32,
		& UnivU32Reader::getIndex32,
		& UnivU32Reader::getXYPos32
	}
};

// ============================================================================

Uint32 Mem::UnivU32Reader::getStart08() const {

	return ( Uint32 ) * ( const Uchar * )checkAndGetAddr();

}

Uint32 Mem::UnivU32Reader::getStart16() const {

	return ( Uint32 ) * ( const Uint16 * )checkAndGetAddr();

}

Uint32 Mem::UnivU32Reader::getStart24() const {

	return ( Uint32 ) * ( const Uint24 * )checkAndGetAddr();

}

//Uint32 Mem::UnivU32Reader::getStart24() const {
//
//	const Uchar * ptr = ( const Uchar * )checkAndGetAddr();
//
//	return ( ( ( Uint32 ) * ( const Uint16 * )( ptr     ) )       )
//	     | ( ( ( Uint32 ) *                   ( ptr + 2 ) ) << 16 );
//
//}

Uint32 Mem::UnivU32Reader::getStart32() const {

	return ( Uint32 ) * ( const Uint32 * )checkAndGetAddr();

}


Uint32 Mem::UnivU32Reader::getIndex08(
	const	Uint32		index ) const {

	return ( Uint32 ) * ( const Uchar * )checkAndGetAddr( index );

}

Uint32 Mem::UnivU32Reader::getIndex16(
	const	Uint32		index ) const {

	return ( Uint32 ) * ( const Uint16 * )checkAndGetAddr( index );

}

Uint32 Mem::UnivU32Reader::getIndex24(
	const	Uint32		index ) const {

	return ( Uint32 ) * ( const Uint24 * )checkAndGetAddr( index );

}

//Uint32 Mem::UnivU32Reader::getIndex24(
//	const	Uint32		index ) const {
//
//	const Uchar * ptr = ( const Uchar * )checkAndGetAddr( index );
//
//	return ( ( ( Uint32 ) * ( const Uint16 * )( ptr     ) )       )
//	     | ( ( ( Uint32 ) *                   ( ptr + 2 ) ) << 16 );
//
//}

Uint32 Mem::UnivU32Reader::getIndex32(
	const	Uint32		index ) const {

	return ( Uint32 ) * ( const Uint32 * )checkAndGetAddr( index );

}

Uint32 Mem::UnivU32Reader::getXYPos08(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Uint32 ) * ( const Uchar * )checkAndGetAddr( x, y );

}

Uint32 Mem::UnivU32Reader::getXYPos16(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Uint32 ) * ( const Uint16 * )checkAndGetAddr( x, y );

}

Uint32 Mem::UnivU32Reader::getXYPos24(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Uint32 ) * ( const Uint24 * )checkAndGetAddr( x, y );

}

//Uint32 Mem::UnivU32Reader::getXYPos24(
//	const	Uint32		x,
//	const	Uint32		y ) const {
//
//	const Uchar * ptr = ( const Uchar * )checkAndGetAddr( x, y );
//
//	return ( ( ( Uint32 ) * ( const Uint16 * )( ptr     ) )       )
//	     | ( ( ( Uint32 ) *                   ( ptr + 2 ) ) << 16 );
//
//}

Uint32 Mem::UnivU32Reader::getXYPos32(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Uint32 ) * ( const Uint32 * )checkAndGetAddr( x, y );

}

// ============================================================================

