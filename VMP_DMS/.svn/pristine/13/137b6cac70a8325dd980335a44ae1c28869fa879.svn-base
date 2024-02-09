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
//	BFC.Net.Log.Debugger.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.FastCSLocker.h"

#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Net.SockAddr.h"
#include "BFC.Net.TCPSocket.h"

#include "BFC.Net.Log.Debugger.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS2( Net, Log, Debugger )

// ============================================================================

Net::Log::Debugger::Debugger() {

}

// ============================================================================

void Net::Log::Debugger::init(
		SockAddrCPtr	pSockAddr ) {

	TCPSocketPtr		clntSock;
	IO::ByteOutputStreamPtr	candStrm;

	clntSock = new TCPSocket( pSockAddr );
	candStrm = new IO::ByteOutputStream( clntSock );

	candStrm->open();

	outpStrm = candStrm;

}

// ============================================================================

void Net::Log::Debugger::consumeMessage(
	const	ColoredBuffer &	msg ) {

	FastCSLocker l( critSect );

	try {
		outpStrm->putLEUint32( msg.getBuffer().getLength() );
		outpStrm->putBytes( msg.getBuffer() );
		Uint32	nbr = msg.getColorsNumber();
		outpStrm->putLEUint32( nbr );
		for ( Uint32 i = 0 ; i < nbr ; i++ ) {
			outpStrm->putLEUint32( msg.getStartPosition( i ) );
			outpStrm->putLEUint32( msg.getForeground( i ) );
			outpStrm->putLEUint32( msg.getBackground( i ) );
		}
		outpStrm->flush();
	}
	catch ( Exception & e ) {
		cerr << e.what() << endl;
	}

}

// ============================================================================

