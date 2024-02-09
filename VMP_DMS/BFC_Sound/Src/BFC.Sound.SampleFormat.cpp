// ============================================================================

#if defined( SOUND_OLD_API )

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
//	BFC.Sound.SampleFormat.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Sound.SampleFormat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Sound, SampleFormat )

// ============================================================================

Sound::SampleFormatCPtr Sound::SampleFormat::U8      = new Sound::SampleFormat( UUId( "399a044c-a4e9-4022-964b-b23df1ad0e77" ),	true,	Maybe,	false,	 8, "U8" );
Sound::SampleFormatCPtr Sound::SampleFormat::S8      = new Sound::SampleFormat( UUId( "8ccd06ca-dcc9-43b3-9596-4db6218750af" ),	true,	Maybe,	true,	 8, "S8" );
Sound::SampleFormatCPtr Sound::SampleFormat::U16LE   = new Sound::SampleFormat( UUId( "5c3ff80c-5e74-4a47-842d-21e0d145be25" ),	true,	true,	false,	16, "U16LE" );
Sound::SampleFormatCPtr Sound::SampleFormat::S16LE   = new Sound::SampleFormat( UUId( "e35f665b-591a-4b62-aa4a-674cb25bbd6e" ),	true,	true,	true,	16, "S16LE" );
Sound::SampleFormatCPtr Sound::SampleFormat::U16BE   = new Sound::SampleFormat( UUId( "1101bfb0-842c-4784-8908-06505acda235" ),	true,	false,	false,	16, "U16BE" );
Sound::SampleFormatCPtr Sound::SampleFormat::S16BE   = new Sound::SampleFormat( UUId( "5bf7dad7-81d4-49bb-a203-ed772e9d7f8d" ),	true,	false,	true,	16, "S16BE" );
Sound::SampleFormatCPtr Sound::SampleFormat::U24LE   = new Sound::SampleFormat( UUId( "38d75318-0725-4062-ab61-03fab0bb9a41" ),	true,	true,	false,	24, "U24LE" );
Sound::SampleFormatCPtr Sound::SampleFormat::S24LE   = new Sound::SampleFormat( UUId( "e021b654-4f9c-40b8-ab80-73b70fc5656b" ),	true,	true,	true,	24, "S24LE" );
Sound::SampleFormatCPtr Sound::SampleFormat::U24BE   = new Sound::SampleFormat( UUId( "816e1148-2739-413f-8c10-13b7ae3204a2" ),	true,	false,	false,	24, "U24BE" );
Sound::SampleFormatCPtr Sound::SampleFormat::S24BE   = new Sound::SampleFormat( UUId( "c8643fb1-ff30-4f22-b756-28bfc166f64f" ),	true,	false,	true,	24, "S24BE" );
Sound::SampleFormatCPtr Sound::SampleFormat::U32LE   = new Sound::SampleFormat( UUId( "a64a7a52-8f89-4f37-b477-d5fc09037281" ),	true,	true,	false,	32, "U32LE" );
Sound::SampleFormatCPtr Sound::SampleFormat::S32LE   = new Sound::SampleFormat( UUId( "5271d255-835b-48ed-a841-6544fd8bf100" ),	true,	true,	true,	32, "S32LE" );
Sound::SampleFormatCPtr Sound::SampleFormat::U32BE   = new Sound::SampleFormat( UUId( "4c1d64a2-b72a-43eb-aec4-a5051bed75ac" ),	true,	false,	false,	32, "U32BE" );
Sound::SampleFormatCPtr Sound::SampleFormat::S32BE   = new Sound::SampleFormat( UUId( "74935cc3-7cf9-4b34-8ea8-7fec646ac4b9" ),	true,	false,	true,	32, "S32BE" );
Sound::SampleFormatCPtr Sound::SampleFormat::F32LE   = new Sound::SampleFormat( UUId( "b222530c-e351-4393-a1bf-2a61503ff32a" ),	false,	true,	Maybe,	32, "F32LE" );
Sound::SampleFormatCPtr Sound::SampleFormat::F32BE   = new Sound::SampleFormat( UUId( "1882115a-ccca-4d77-8080-cbb1b0da8111" ),	false,	false,	Maybe,	32, "F32BE" );

// ============================================================================

Sound::SampleFormatCPtr Sound::SampleFormat::getLEInteger(
	const	Uint32		pSampSize,
	const	Bool		pSigned ) {

	if ( pSigned ) {

		switch ( pSampSize) {
		case 8 :
			return S8;
		case 16 :
			return S16LE;
		case 24 :
			return S24LE;
		case 32 :
			return S32LE;
		default :
			break;
		}

	}
	else {

		switch ( pSampSize) {
		case 8 :
			return U8;
		case 16 :
			return U16LE;
		case 24 :
			return U24LE;
		case 32 :
			return U32LE;
		default :
			break;
		}

	}

	throw InvalidArgument( "Unsupported sample size ("
		+ Buffer( pSampSize )
		+ ")!" );

}

// ============================================================================

#endif // 0

// ============================================================================

