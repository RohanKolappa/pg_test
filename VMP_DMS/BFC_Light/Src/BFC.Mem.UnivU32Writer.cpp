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
//	BFC.Mem.UnivU32Writer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Mem.UnivU32Writer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::UnivU32Writer::SetTableEntry Mem::UnivU32Writer::setTable[] = {
	{
		& UnivU32Writer::setStart08,
		& UnivU32Writer::setIndex08,
		& UnivU32Writer::setXYPos08
	},
	{
		& UnivU32Writer::setStart16,
		& UnivU32Writer::setIndex16,
		& UnivU32Writer::setXYPos16
	},
	{
		& UnivU32Writer::setStart24,
		& UnivU32Writer::setIndex24,
		& UnivU32Writer::setXYPos24
	},
	{
		& UnivU32Writer::setStart32,
		& UnivU32Writer::setIndex32,
		& UnivU32Writer::setXYPos32
	}
};

// ============================================================================

void Mem::UnivU32Writer::setStart08(
	const	Uint32		val ) {

	* ( Uchar * )checkAndGetAddr() = ( Uchar )val;

}

void Mem::UnivU32Writer::setStart16(
	const	Uint32		val ) {

	* ( Uint16 * )checkAndGetAddr() = ( Uint16 )val;

}

void Mem::UnivU32Writer::setStart24(
	const	Uint32		val ) {

	* ( Uint24 * )checkAndGetAddr() = ( Uint24 )val;

}

void Mem::UnivU32Writer::setStart32(
	const	Uint32		val ) {

	* ( Uint32 * )checkAndGetAddr() = ( Uint32 )val;

}

void Mem::UnivU32Writer::setIndex08(
	const	Uint32		index,
	const	Uint32		val ) {

	* ( Uchar * )checkAndGetAddr( index ) = ( Uchar )val;

}

void Mem::UnivU32Writer::setIndex16(
	const	Uint32		index,
	const	Uint32		val ) {

	* ( Uint16 * )checkAndGetAddr( index ) = ( Uint16 )val;

}

void Mem::UnivU32Writer::setIndex24(
	const	Uint32		index,
	const	Uint32		val ) {

	* ( Uint24 * )checkAndGetAddr( index ) = ( Uint24 )val;

}

void Mem::UnivU32Writer::setIndex32(
	const	Uint32		index,
	const	Uint32		val ) {

	* ( Uint32 * )checkAndGetAddr( index ) = ( Uint32 )val;

}

void Mem::UnivU32Writer::setXYPos08(
	const	Uint32		x,
	const	Uint32		y,
	const	Uint32		val ) {

	* ( Uchar * )checkAndGetAddr( x, y ) = ( Uchar )val;

}

void Mem::UnivU32Writer::setXYPos16(
	const	Uint32		x,
	const	Uint32		y,
	const	Uint32		val ) {

	* ( Uint16 * )checkAndGetAddr( x, y ) = ( Uint16 )val;

}

void Mem::UnivU32Writer::setXYPos24(
	const	Uint32		x,
	const	Uint32		y,
	const	Uint32		val ) {

	* ( Uint24 * )checkAndGetAddr( x, y ) = ( Uint24 )val;

}

void Mem::UnivU32Writer::setXYPos32(
	const	Uint32		x,
	const	Uint32		y,
	const	Uint32		val ) {

	* ( Uint32 * )checkAndGetAddr( x, y ) = ( Uint32 )val;

}

// ============================================================================

