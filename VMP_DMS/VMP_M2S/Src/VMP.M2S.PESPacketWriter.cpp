// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.PESPacketWriter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.IO.File.h"

#include "VMP.M2S.PESPacket.h"
#include "VMP.M2S.PESPacketWriter.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PESPacketWriter )

// ============================================================================

M2S::PESPacketWriter::PESPacketWriter(
	const	Buffer &	filename,
	const	Bool		wholePacket ) {

	setObjectName( "VMP.M2S.PESPacketWriter" );

	whole = wholePacket;
	file = new IO::File( filename, IO::File::New );

	try {
		file->open();
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't open file!" );
	}

}

// ============================================================================

void M2S::PESPacketWriter::putObject(
		SPtr		pPESPacket ) {

	PESPacketPtr	packet = pPESPacket;

	if ( file->isConnected() ) {
		file->putBytes( whole ? packet->packetDump : packet->data_byte );
	}

}

// ============================================================================

