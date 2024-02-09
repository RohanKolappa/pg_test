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
//	BFC.Mem.UnivS32Reader.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Mem.UnivS32Reader.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::UnivS32Reader::GetTableEntry Mem::UnivS32Reader::getTable[] = {
	{
		& UnivS32Reader::getStart08,
		& UnivS32Reader::getIndex08,
		& UnivS32Reader::getXYPos08
	},
	{
		& UnivS32Reader::getStart16,
		& UnivS32Reader::getIndex16,
		& UnivS32Reader::getXYPos16
	},
	{
		& UnivS32Reader::getStart24,
		& UnivS32Reader::getIndex24,
		& UnivS32Reader::getXYPos24
	},
	{
		& UnivS32Reader::getStart32,
		& UnivS32Reader::getIndex32,
		& UnivS32Reader::getXYPos32
	}
};

// ============================================================================

Int32 Mem::UnivS32Reader::getStart08() const {

	return ( Int32 ) * ( const Char * )checkAndGetAddr();

}

Int32 Mem::UnivS32Reader::getStart16() const {

	return ( Int32 ) * ( const Int16 * )checkAndGetAddr();

}

Int32 Mem::UnivS32Reader::getStart24() const {

	return ( Int32 ) * ( const Int24 * )checkAndGetAddr();

}

Int32 Mem::UnivS32Reader::getStart32() const {

	return ( Int32 ) * ( const Int32 * )checkAndGetAddr();

}


Int32 Mem::UnivS32Reader::getIndex08(
	const	Uint32		index ) const {

	return ( Int32 ) * ( const Char * )checkAndGetAddr( index );

}

Int32 Mem::UnivS32Reader::getIndex16(
	const	Uint32		index ) const {

	return ( Int32 ) * ( const Int16 * )checkAndGetAddr( index );

}

Int32 Mem::UnivS32Reader::getIndex24(
	const	Uint32		index ) const {

	return ( Int32 ) * ( const Int24 * )checkAndGetAddr( index );

}

Int32 Mem::UnivS32Reader::getIndex32(
	const	Uint32		index ) const {

	return ( Int32 ) * ( const Int32 * )checkAndGetAddr( index );

}

Int32 Mem::UnivS32Reader::getXYPos08(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Int32 ) * ( const Char * )checkAndGetAddr( x, y );

}

Int32 Mem::UnivS32Reader::getXYPos16(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Int32 ) * ( const Int16 * )checkAndGetAddr( x, y );

}

Int32 Mem::UnivS32Reader::getXYPos24(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Int32 ) * ( const Int24 * )checkAndGetAddr( x, y );

}

Int32 Mem::UnivS32Reader::getXYPos32(
	const	Uint32		x,
	const	Uint32		y ) const {

	return ( Int32 ) * ( const Int32 * )checkAndGetAddr( x, y );

}

// ============================================================================

