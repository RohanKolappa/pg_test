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
//	BFC.Mem.UnivS32Writer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Mem.UnivS32Writer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::UnivS32Writer::SetTableEntry Mem::UnivS32Writer::setTable[] = {
	{
		& UnivS32Writer::setStart08,
		& UnivS32Writer::setIndex08,
		& UnivS32Writer::setXYPos08
	},
	{
		& UnivS32Writer::setStart16,
		& UnivS32Writer::setIndex16,
		& UnivS32Writer::setXYPos16
	},
	{
		& UnivS32Writer::setStart24,
		& UnivS32Writer::setIndex24,
		& UnivS32Writer::setXYPos24
	},
	{
		& UnivS32Writer::setStart32,
		& UnivS32Writer::setIndex32,
		& UnivS32Writer::setXYPos32
	}
};

// ============================================================================

void Mem::UnivS32Writer::setStart08(
	const	Int32		val ) {

	* ( Char * )checkAndGetAddr() = ( Char )val;

}

void Mem::UnivS32Writer::setStart16(
	const	Int32		val ) {

	* ( Int16 * )checkAndGetAddr() = ( Int16 )val;

}

void Mem::UnivS32Writer::setStart24(
	const	Int32		val ) {

	* ( Int24 * )checkAndGetAddr() = ( Int24 )val;

}

void Mem::UnivS32Writer::setStart32(
	const	Int32		val ) {

	* ( Int32 * )checkAndGetAddr() = ( Int32 )val;

}

void Mem::UnivS32Writer::setIndex08(
	const	Uint32		index,
	const	Int32		val ) {

	* ( Char * )checkAndGetAddr( index ) = ( Char )val;

}

void Mem::UnivS32Writer::setIndex16(
	const	Uint32		index,
	const	Int32		val ) {

	* ( Int16 * )checkAndGetAddr( index ) = ( Int16 )val;

}

void Mem::UnivS32Writer::setIndex24(
	const	Uint32		index,
	const	Int32		val ) {

	* ( Int24 * )checkAndGetAddr( index ) = ( Int24 )val;

}

void Mem::UnivS32Writer::setIndex32(
	const	Uint32		index,
	const	Int32		val ) {

	* ( Int32 * )checkAndGetAddr( index ) = ( Int32 )val;

}

void Mem::UnivS32Writer::setXYPos08(
	const	Uint32		x,
	const	Uint32		y,
	const	Int32		val ) {

	* ( Char * )checkAndGetAddr( x, y ) = ( Char )val;

}

void Mem::UnivS32Writer::setXYPos16(
	const	Uint32		x,
	const	Uint32		y,
	const	Int32		val ) {

	* ( Int16 * )checkAndGetAddr( x, y ) = ( Int16 )val;

}

void Mem::UnivS32Writer::setXYPos24(
	const	Uint32		x,
	const	Uint32		y,
	const	Int32		val ) {

	* ( Int24 * )checkAndGetAddr( x, y ) = ( Int24 )val;

}

void Mem::UnivS32Writer::setXYPos32(
	const	Uint32		x,
	const	Uint32		y,
	const	Int32		val ) {

	* ( Int32 * )checkAndGetAddr( x, y ) = ( Int32 )val;

}

// ============================================================================

