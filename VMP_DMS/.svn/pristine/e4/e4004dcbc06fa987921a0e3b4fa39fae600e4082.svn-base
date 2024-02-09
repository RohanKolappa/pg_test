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
//	BFC.Net.Log.Server.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.Scheduler.h"

#include "BFC.Net.TCPSocketProvider.h"

#include "BFC.Net.Log.Handler.h"
#include "BFC.Net.Log.Server.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS2( Net, Log, Server )

// ============================================================================

Net::Log::Server::Server(
	const	Bool		pCons,
	const	Bool		pFile,
	const	Uint16		pPort ) :

	cons	( pCons ),
	file	( pFile ),
	port	( pPort ) {

	setObjectName( "BFC.Net.Log.Server" );

}

Net::Log::Server::~Server() {

	safeStopAndJoin();

}

// ============================================================================

void Net::Log::Server::run() {

	Net::TCPSocketProviderPtr	serv = new Net::TCPSocketProvider;

	serv->init( port, 5, Net::TCPSocketProvider::ReuseAddr );

	port = serv->getPort();

//	msgDbg( "Listening on port " + Buffer( port ) );

	MT::SchedulerPtr		schd = new MT::Scheduler;

	while ( ! shouldStop() ) {
		try {
			schd->addThread( new Handler(
				serv->accept(), cons, file ) );
		}
		catch ( ThreadStopped & ) {
			break;
		}
	}

}

// ============================================================================

