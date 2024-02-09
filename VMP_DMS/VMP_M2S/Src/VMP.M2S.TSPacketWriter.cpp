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
//	VMP.M2S.TSPacketWriter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.File.h"

#include "VMP.M2S.TSPacket.h"
#include "VMP.M2S.TSPacketWriter.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TSPacketWriter )

// ============================================================================

M2S::TSPacketWriter::TSPacketWriter(
	const	Buffer &	pFilename ) {

	setObjectName( "VMP.M2S.TSPacketWriter" );

	file = new IO::File( pFilename, IO::File::New );

	try {
		file->open();
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't open file!" );
	}

}

// ============================================================================

void M2S::TSPacketWriter::putObject(
		SPtr		pTSPacket ) {

	TSPacketPtr	tsPacket = pTSPacket;

	if ( file->isConnected() ) {
		file->putBytes( tsPacket->getRawCopy() );
	}

}

// ============================================================================

