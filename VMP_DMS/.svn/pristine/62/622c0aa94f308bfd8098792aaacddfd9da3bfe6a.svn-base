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
//	BFC.Net.Log.Handler.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Debug.ColoredBuffer.h"
#include "BFC.Debug.ConsoleColor.h"
#include "BFC.Debug.Debugger.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.File.h"

#include "BFC.Net.TCPSocket.h"

#include "BFC.Net.Log.Handler.h"

#include "BFC.Time.Clock.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS2( Net, Log, Handler )

// ============================================================================

Net::Log::Handler::Handler(
		Net::TCPSocketPtr
				pClntSock,
	const	Bool		pConsFlag,
	const	Bool		pFileFlag ) :

	clntSock	( pClntSock ),
	peerAddr	( pClntSock->getPeerAddr().getDot() + "."
			+ Buffer( pClntSock->getPeerPort() ) ),
	consFlag	( pConsFlag ),
	fileFlag	( pFileFlag ) {

	setObjectName( "BFC.Net.Log.Handler[ " + peerAddr + " ]" );

}

Net::Log::Handler::~Handler() {

	safeStopAndJoin();

}

// ============================================================================

void Net::Log::Handler::run() {

//	msgDbg( "Logging started for " + peerAddr + "." );

	IO::FilePtr	file;

	if ( fileFlag ) {
		file = new IO::File( "netlog." + peerAddr + ".txt", IO::File::New );
		file->open();
	}

	IO::ByteInputStreamPtr inptStrm = new IO::ByteInputStream( clntSock );

	inptStrm->open();

	while ( ! shouldStop() ) {

		Buffer		txt;
		ColoredBuffer	msg;

		try {
			Uint32	len = inptStrm->getLEUint32();
			txt = inptStrm->getBytes( len );
			Uint32	nbr = inptStrm->getLEUint32();
			ColoredBuffer::ColorsList	lst;
			for ( Uint32 i = 0 ; i < nbr ; i++ ) {
				Uint32	startPos = inptStrm->getLEUint32();
				Uint32	foreColr = inptStrm->getLEUint32();
				Uint32	backColr = inptStrm->getLEUint32();
				lst += ColoredBuffer::ColorModif( startPos, ( ConsoleColor )foreColr, ( ConsoleColor )backColr );
			}
			msg = ColoredBuffer( txt, lst );
		}
		catch ( IO::EndOfStream & ) {
			break;
		}

		if ( fileFlag ) {
			file->putBytes( txt + "\n" );
		}

		if ( consFlag ) {
			if ( ! txt.isEmpty() && txt.lastChar() == '\n' ) {
				txt.dropLastChar();
			}
			ColoredBuffer prfx
				= ColoredBuffer(
					Time::now().dateTimeToBuffer( true, true ),
					/* ConsoleColor:: */ Yellow,
					/* ConsoleColor:: */ Black )
				+ ColoredBuffer(
					" " )
				+ ColoredBuffer(
					peerAddr,
					/* ConsoleColor:: */ Green,
					/* ConsoleColor:: */ Black );
			ColoredBuffer cmsg
				= prefixedBy(
					prfx,
					true,
					msg );
			StderrDebugger::instance()->consumeMessage( cmsg );
		}
	}

//	msgDbg( "Logging stopped." );

}

// ============================================================================

