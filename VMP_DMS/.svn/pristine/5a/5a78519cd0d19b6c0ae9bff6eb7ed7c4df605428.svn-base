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
//	BFC.Sound.Format.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Sound.Format.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Sound, Format )

// ============================================================================

Sound::Format::Format(
	const	Uint32		pNbrChans,
	const	Uint32		pSampRate,
	const	Type		pSampType ) :

	sampRate( pSampRate ),
	sampType( pSampType ),
	chanName( pNbrChans ) {

}

Sound::Format::Format(
	const	Uint32		pNbrChans,
	const	Uint32		pSampRate,
	const	Uint32		pSampSize ) :

	sampRate( pSampRate ),
	sampType( findSampleType( pSampSize ) ),
	chanName( pNbrChans ) {

}

Sound::Format::Format(
	const	Format &	pOther ) :

	SObject	(),

	sampRate( pOther.sampRate ),
	sampType( pOther.sampType ),
	chanName( pOther.chanName ) {

}

// ============================================================================

Sound::Format & Sound::Format::operator = (
	const	Format &	pOther ) {

	if ( this != &pOther ) {
		sampRate = pOther.sampRate;
		sampType = pOther.sampType;
		chanName = pOther.chanName;
	}

	return *this;

}

// ============================================================================

Bool Sound::Format::operator == (
	const	Format &	pOther ) const {

	return ( ( sampRate == pOther.sampRate )
	      && ( sampType == pOther.sampType )
	      && ( chanName == pOther.chanName ) );

}

Bool Sound::Format::operator != (
	const	Format &	pOther ) const {

	return ( ! operator == ( pOther ) );

}

// ============================================================================

Uint32 Sound::Format::getSampleSize(
	const	Uint32		pInBytes ) const {

	if ( sampType >= Unknown ) {
		throw InternalError();
	}

	static const Uint32 tbl[] = {

		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,

		9,
		10,
		11,
		12,
		13,
		14,
		15,
		16,

		17,
		18,
		19,
		20,
		21,
		22,
		23,
		24,

		25,
		26,
		27,
		28,
		29,
		30,
		31,
		32,

		32	// F32LE

	};

	return ( pInBytes
		? ( tbl[ sampType ] + 7 ) / 8
		: tbl[ sampType ] );

}

Uint32 Sound::Format::getUnusedSize() const {

	return ( getSampleSize( true ) * 8 - getSampleSize( false ) );

}

const Buffer & Sound::Format::getSampleName() const {

	return getTypeName( sampType );

}

// ============================================================================

Buffer Sound::Format::toBuffer() const {

	return Buffer( getNbrChannels() ) + "ch, "
		+ Buffer( getSampleRate() ) + "Hz, "
		+ getSampleName();

}

// ============================================================================

Sound::Format::Type Sound::Format::findSampleType(
	const	Uint32		pSampSize,
	const	Bool		pIsSigned ) {

	if ( pIsSigned
	  && pSampSize > 0
	  && pSampSize <= 32 ) {

		return ( Type )( pSampSize - 1 );

	}

	return Unknown;

}

Sound::Format::Type Sound::Format::findSampleType(
	const	Buffer &	pTypeName ) {

	for ( Uint32 i = 0 ; i < Unknown ; i++ ) {
		if ( ConvTble[ i ].typeName == pTypeName ) {
			return ConvTble[ i ].smplType;
		}
	}

	return Unknown;

}

const Buffer & Sound::Format::getTypeName(
	const	Type		pSampType ) {

	if ( pSampType > Unknown ) {
		throw InternalError();
	}


	return ConvTble[ pSampType ].typeName;

}

// ============================================================================

const Sound::Format::Entry Sound::Format::ConvTble[] = {

	{    S1,	"S1"	},
	{    S2,	"S2"	},
	{    S3,	"S3"	},
	{    S4,	"S4"	},
	{    S5,	"S5"	},
	{    S6,	"S6"	},
	{    S7,	"S7"	},
	{    S8,	"S8"	},

	{  S9LE,	"S9LE"	},
	{ S10LE,	"S10LE"	},
	{ S11LE,	"S11LE"	},
	{ S12LE,	"S12LE"	},
	{ S13LE,	"S13LE"	},
	{ S14LE,	"S14LE"	},
	{ S15LE,	"S15LE"	},
	{ S16LE,	"S16LE"	},

	{ S17LE,	"S17LE"	},
	{ S18LE,	"S18LE"	},
	{ S19LE,	"S19LE"	},
	{ S20LE,	"S20LE"	},
	{ S21LE,	"S21LE"	},
	{ S22LE,	"S22LE"	},
	{ S23LE,	"S23LE"	},
	{ S24LE,	"S24LE"	},

	{ S25LE,	"S25LE"	},
	{ S26LE,	"S26LE"	},
	{ S27LE,	"S27LE"	},
	{ S28LE,	"S28LE"	},
	{ S29LE,	"S29LE"	},
	{ S30LE,	"S30LE"	},
	{ S31LE,	"S31LE"	},
	{ S32LE,	"S32LE"	},

	{ F32LE,	"F32LE"	},

	{ Unknown,	"N/A" }

};

// ============================================================================

