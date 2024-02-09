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
//	BFC.Mem.UnivS32Writer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_UnivS32Writer_H_
#define _BFC_Mem_UnivS32Writer_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Mem.Writer.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Specialized Writer, working with elements whose size is 1, 2, 3, or
///	4 bytes.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL UnivS32Writer : public Writer {

public :

	UnivS32Writer(
	) {
	}

	UnivS32Writer(
		const	Block &		block,
		const	Uint32		eltLength
	) :
		Writer( block, eltLength )
	{
		branchSetMethods( getEltLength() );
	}

	UnivS32Writer(
		const	UnivS32Writer &	other,
		const	Uint32		eltLength
	) :
		Writer( other, eltLength )
	{
		branchSetMethods( getEltLength() );
	}

	UnivS32Writer(
		const	UnivS32Writer &	other
	) :
		Writer( other )
	{
		branchSetMethods( getEltLength() );
	}

	UnivS32Writer(
		const	Writer &	other,
		const	Uint32		eltLength
	) :
		Writer( other, eltLength )
	{
		branchSetMethods( getEltLength() );
	}

	UnivS32Writer(
		const	Writer &	other
	) :
		Writer( other )
	{
		branchSetMethods( getEltLength() );
	}

	UnivS32Writer & operator = (
		const	UnivS32Writer &	other
	) {
		doCopy( other, other.getEltLength() );
		return * this;
	}

	void set(
		const	Int32		val
	) {
		( this->*setStartMethod )( val );
	}

	void set(
		const	Uint32		index,
		const	Int32		val
	) {
		( this->*setIndexMethod )( index, val );
	}

	void set(
		const	Uint32		x,
		const	Uint32		y,
		const	Int32		val
	) {
		( this->*setXYPosMethod )( x, y, val );
	}

protected :

	typedef void ( UnivS32Writer::*SetStartMethodType )(
			const	Int32		val
		);
	typedef void ( UnivS32Writer::*SetIndexMethodType )(
			const	Uint32		index,
			const	Int32		val
		);
	typedef void ( UnivS32Writer::*SetXYPosMethodType )(
			const	Uint32		x,
			const	Uint32		y,
			const	Int32		val
		);

	void setStart08( const Int32 val );
	void setStart16( const Int32 val );
	void setStart24( const Int32 val );
	void setStart32( const Int32 val );

	void setIndex08( const Uint32 index, const Int32 val );
	void setIndex16( const Uint32 index, const Int32 val );
	void setIndex24( const Uint32 index, const Int32 val );
	void setIndex32( const Uint32 index, const Int32 val );

	void setXYPos08( const Uint32 x, const Uint32 y, const Int32 val );
	void setXYPos16( const Uint32 x, const Uint32 y, const Int32 val );
	void setXYPos24( const Uint32 x, const Uint32 y, const Int32 val );
	void setXYPos32( const Uint32 x, const Uint32 y, const Int32 val );

	static struct SetTableEntry {
		SetStartMethodType	s;
		SetIndexMethodType	i;
		SetXYPosMethodType	x;
	} setTable[];

	SetStartMethodType	setStartMethod;
	SetIndexMethodType	setIndexMethod;
	SetXYPosMethodType	setXYPosMethod;

	void branchSetMethods(
		const	Uint32		eltLength
	) {
		setStartMethod = setTable[ eltLength - 1 ].s;
		setIndexMethod = setTable[ eltLength - 1 ].i;
		setXYPosMethod = setTable[ eltLength - 1 ].x;
	}

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_UnivS32Writer_H_

// ============================================================================

