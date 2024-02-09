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
//	BFC.Net.TCPSocketProvider.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TCPSocketProvider_H_
#define _BFC_TCPSocketProvider_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.TCPSocketProviderPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.TCPSocket.h"

#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

class SocketData;

// ============================================================================

/// \brief A simple TCP server socket implementation.
///
/// This class implements the basic services anyone expects from a TCP server
/// socket.
///
/// \ingroup BFC_Net

class BFC_Net_DLL TCPSocketProvider : virtual public SObject {

public :

	/// \brief Creates a new TCP server socket.
	/// 
	/// Inits the internal. To become useful, this socket has to be
	/// bound and a backlog has to be set before starting to accept
	/// new connections.

	TCPSocketProvider(
	);

	/// \brief Destroys this socket.
	/// 
	/// Destroys this socket (kills it).

	virtual ~TCPSocketProvider(
	);

	/// \brief Available socket options.
	/// 
	/// Those options are provided to tune some of the internal socket
	/// functionalities.

	enum {

		/// Indicates that the rules used in validating the address
		/// supplied to the init() method should allow reuse of local
		/// addresses.

		ReuseAddr = 0x01		//! RTFM

	};

	/// \brief Initializes this server socket.
	/// 
	/// This method bounds this server socket to port \a port, creates
	/// a \a backlog long backlog list, using options provided in \a
	/// options.
	/// 
	/// \throws AlreadyInitialized
	/// 	This socket has already been initialized.
	/// 
	/// \throws NullArgument
	/// 	\a backlog == 0.
	/// 
	/// \throws CantSystemCreate
	/// 	No system resource available to create a new socket.
	/// 
	/// \throws CantSystemSetSockOpt
	/// 	\a options contains a socket option that could not be
	/// 	set.
	/// 
	/// \throws CantSystemBind
	/// 	Binding to port \a port has failed.
	/// 
	/// \throws CantSystemListen
	/// 	Can't initialize the backlog.

	void init(
		const	Uint16		port,
		const	Uint32		backlog,
		const	Uint32		options = 0	// ReuseAddr | ...
	);

	/// \brief Initializes this server socket.
	/// 
	/// This method bounds this server socket to addr \a pAddr and port
	/// \a pPort, creates a \a pBacklog long backlog list, using options
	/// provided in \a pOptions.
	/// 
	/// \throws AlreadyInitialized
	/// 	This socket has already been initialized.
	/// 
	/// \throws NullArgument
	/// 	\a backlog == 0.
	/// 
	/// \throws CantSystemCreate
	/// 	No system resource available to create a new socket.
	/// 
	/// \throws CantSystemSetSockOpt
	/// 	\a options contains a socket option that could not be
	/// 	set.
	/// 
	/// \throws CantSystemBind
	/// 	Binding to port \a port has failed.
	/// 
	/// \throws CantSystemListen
	/// 	Can't initialize the backlog.

	void init(
		const	IPv4Address &	pAddr,
		const	Uint16		pPort,
		const	Uint32		pBacklog,
		const	Uint32		pOptions	// ReuseAddr | ...
	);

	Uint16 getPort(
	) const {
		return port;
	}

	/// \brief Returns \b true iff this socket has been initialized.

	Bool isInitialized(
	) const;

	/// \brief Kills (uninitializes) this socket.
	///
	/// If another thread was blocked on the accept() method, this
	/// thread is unblocked and receives a SocketKilled() exception.

	void kill(
	);

	/// \brief Starts to synchronously accept incoming connections.
	/// 
	/// After this server socket has been bound to some port and the
	/// backlog has been specified using `init()', starts accepting
	/// new connections.
	/// 
	/// This method IS BLOCKING and returns iff there is some kind of
	/// error preventing the acception, or a new connection has been
	/// accepted.
	/// 
	/// Once a new connection has been accepted, a new TCPSocket
	/// is created, initialized with this connection, and returned.
	/// 
	/// \throws NotInitialized
	/// 	This socket has not been initialized.
	/// 
	/// \throws SocketAlreadyConnected
	/// 	The \a newClientSocket argument is a valid socket that
	/// 	is already connected.
	/// 
	/// \throws CantSystemAccept
	/// 	We could not accept a new connection. This could be
	/// 	caused for example by an interrupted system call.
	/// 
	/// \throws CantSystemGetSockName
	/// 	A new connection has been accepted, but we can't get
	/// 	its parameters (port, ...). This connection is
	/// 	automatically destroyed.

	TCPSocketPtr accept(
		const	Time::Delta &	pWaitTime = Time::Delta( 0, 0 )
	);

	BASE_CLASS_GEN_EXCEPTION( "BFC.Net.TCPSocketProvider" );

	CLASS_EXCEPTION( AlreadyInitialized, "Already initialized" );
	CLASS_EXCEPTION( CantSystemAccept, "Can't system accept()" );
	CLASS_EXCEPTION( CantSystemBind, "Can't system bind()" );
	CLASS_EXCEPTION( CantSystemCreate, "Can't system create()" );
	CLASS_EXCEPTION( CantSystemGetSockName, "Can't getsocketname()" );
	CLASS_EXCEPTION( CantSystemListen, "Can't system listen()" );
	CLASS_EXCEPTION( CantSystemSelect, "Can't system select()" );
	CLASS_EXCEPTION( CantSystemSetSockOpt, "Can't system setsockopt()" );
	CLASS_EXCEPTION( NotInitialized, "Not initialized" );
	CLASS_EXCEPTION( NullArgument, "Null argument" );
	CLASS_EXCEPTION( SocketKilled, "Socket killed" );

protected :

//	/// \brief Not documented here!
//	/// 
//	/// \throws CantSystemClose
//
//	void myClose(
//		SocketData *	data
//	);

private :

	// internal state

	SocketData *		data;	///< Socket descriptor.
	Uint16			port;	///< Listening port.

	// others...

	TCPSocketProvider(
		const TCPSocketProvider&
	);

	TCPSocketProvider& operator = (
		const TCPSocketProvider&
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_TCPSocketProvider_H_

// ============================================================================





