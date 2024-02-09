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
//	VMP.AVC.NALUInputStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.EndOfStream.h"

#include "VMP.AVC.NALU.h"
#include "VMP.AVC.NALUInputStream.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, NALUInputStream )

// ============================================================================

AVC::NALUInputStream::NALUInputStream(
		IO::ByteInputStreamPtr
				pIStream ) :

	strm	( pIStream ) {

	setObjectName( "VMP.AVC.NALUInputStream" );

}

// ============================================================================

void AVC::NALUInputStream::init() {

	while ( strm->peekBEUint32() != 1 ) {
		strm->skip( 1 );
	}

	strm->skip( 4 );
	done = false;

}

// ============================================================================

AVC::NALUPtr AVC::NALUInputStream::peekNALU() {

	fillNALU();

	return nalu;

}

AVC::NALUPtr AVC::NALUInputStream::getNALU() {

	fillNALU();

	NALUPtr res = nalu;

	nalu.kill();

	return res;

}

AVC::NALUPtr AVC::NALUInputStream::getNALU(
	const	Uchar		pType ) {

	NALUPtr cand;

	for (;;) {
		fillNALU();
		cand = nalu;
		nalu.kill();
		if ( cand->getType() == pType ) {
			break;
		}
	}

	return cand;

}

void AVC::NALUInputStream::popNALU() {

	fillNALU();

	nalu.kill();

}

// ============================================================================

void AVC::NALUInputStream::fillNALU() {

	if ( nalu ) {
		return;
	}

	if ( done ) {
		throw IO::EndOfStream();
	}

	Uint32	indx;

	for (;;) {
		indx = findStartCode( buff );
		if ( indx < buff.getLength() ) {
			break;
		}
		try {
			buff += strm->getBytes();
		}
		catch ( IO::EndOfStream & ) {
			done = true;
			break;
		}
		catch ( IO::BrokenPipe & ) {
			done = true;
			buff.kill();
			break;
		}
	}

	if ( indx < buff.getLength() ) {
		nalu = new NALU( buff( 0, indx ) );
		buff = buff( indx + 4 );
	}
	else if ( buff.hasContent() ) {
		nalu = new NALU( buff );
		buff.kill();
	}
	else {
		throw IO::EndOfStream();
	}

}

// ============================================================================

Uint32 AVC::NALUInputStream::findStartCode(
	const	Buffer &	pData ) {

	Uint32		len = pData.getLength();
	const Uchar *	ptr = pData.getCstPtr();

	for ( Uint32 pos = 0 ; pos + 3 < len ; ) {
		if ( *ptr++ != 0x00 ) {
			pos++;
		}
		else if ( *ptr++ != 0x00 ) {
			pos += 2;
		}
		else if ( *ptr++ != 0x00 ) {
			pos += 3;
		}
		else if ( *ptr++ != 0x01 ) {
			pos += 4;
		}
		else {
			return pos;
		}
	}

	return pData.getLength();

}

// ============================================================================

