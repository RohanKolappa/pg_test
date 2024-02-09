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
//	BFC.Mem.UnivU32Reader.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_UnivU32Reader_H_
#define _BFC_Mem_UnivU32Reader_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Mem.Writer.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Specialized Reader, working with elements whose size is 1, 2, 3, or
///	4 bytes.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL UnivU32Reader : public Reader {

public :

	UnivU32Reader(
	) {
	}

	UnivU32Reader(
		const	Block &		pBlock,
		const	Uint32		pEltLength
	) :
		Reader( pBlock, pEltLength )
	{
		branchGetMethods( getEltLength() );
	}

	UnivU32Reader(
		const	UnivU32Reader &	pOther,
		const	Uint32		pEltLength
	) :
		Reader( pOther, pEltLength )
	{
		branchGetMethods( getEltLength() );
	}

	UnivU32Reader(
		const	UnivU32Reader &	pOther
	) :
		Reader( pOther )
	{
		branchGetMethods( getEltLength() );
	}

	UnivU32Reader(
		const	Reader &	pOther,
		const	Uint32		pEltLength
	) :
		Reader( pOther, pEltLength )
	{
		branchGetMethods( getEltLength() );
	}

	UnivU32Reader(
		const	Reader &	pOther
	) :
		Reader( pOther )
	{
		branchGetMethods( getEltLength() );
	}

	UnivU32Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, pOther.getEltLength() );
		return * this;
	}

	Uint32 get(
	) const {
		return ( this->*getStartMethod )();
	}

	Uint32 get(
		const	Uint32		pIndex
	) const {
		return ( this->*getIndexMethod )( pIndex );
	}

	Uint32 get(
		const	Uint32		x,
		const	Uint32		y
	) const {
		return ( this->*getXYPosMethod )( x, y );
	}

	Uint32 operator * (
	) const {
		return ( this->*getStartMethod )();
	}

	Uint32 operator [] (
		const	Uint32		pIndex
	) const {
		return ( this->*getIndexMethod )( pIndex );
	}

	Uint32 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return ( this->*getXYPosMethod )( x, y );
	}

protected :

	typedef Uint32 ( UnivU32Reader::*GetStartMethodType ) (
		) const;
	typedef Uint32 ( UnivU32Reader::*GetIndexMethodType ) (
			const	Uint32		pIndex
		) const;
	typedef Uint32 ( UnivU32Reader::*GetXYPosMethodType ) (
			const	Uint32		x,
			const	Uint32		y
		) const;

	Uint32 getStart08() const;
	Uint32 getStart16() const;
	Uint32 getStart24() const;
	Uint32 getStart32() const;

	Uint32 getIndex08( const Uint32 pIndex ) const;
	Uint32 getIndex16( const Uint32 pIndex ) const;
	Uint32 getIndex24( const Uint32 pIndex ) const;
	Uint32 getIndex32( const Uint32 pIndex ) const;

	Uint32 getXYPos08( const Uint32 x, const Uint32 y ) const;
	Uint32 getXYPos16( const Uint32 x, const Uint32 y ) const;
	Uint32 getXYPos24( const Uint32 x, const Uint32 y ) const;
	Uint32 getXYPos32( const Uint32 x, const Uint32 y ) const;

	static struct GetTableEntry {
		GetStartMethodType	s;
		GetIndexMethodType	i;
		GetXYPosMethodType	x;
	} getTable[];

	GetStartMethodType	getStartMethod;
	GetIndexMethodType	getIndexMethod;
	GetXYPosMethodType	getXYPosMethod;

	void branchGetMethods(
		const	Uint32		pEltLength
	) {
		if ( ! pEltLength || pEltLength > 4 ) {
			throw InvalidArgument();
		}
		getStartMethod = getTable[ pEltLength - 1 ].s;
		getIndexMethod = getTable[ pEltLength - 1 ].i;
		getXYPosMethod = getTable[ pEltLength - 1 ].x;
	}

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_UnivU32Reader_H_

// ============================================================================

