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
//	BFC.Net.SocketData.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_Net_SocketData_H_
#define _BFC_Net_SocketData_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <string.h>
#include <windows.h>
#else
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "BFC.Net.SockAddrPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

#if defined( PLATFORM_WIN32 )

typedef SOCKET		SocketType;
typedef int		SocketLenType;

inline int getErrno(
) {
	return WSAGetLastError();
}

#else

typedef int		SocketType;
typedef socklen_t	SocketLenType;

inline int getErrno(
) {
	return errno;
}

#endif

extern	const	SocketType		InvSocket;	///< ::accept()
extern	const	SocketType		SocketError;
extern	const	SocketType		SocketIntr;
extern	const	SocketType		CodeAgain;
extern	const	SocketType		CodeBrokenPipe;
extern	const	SocketType		CodeConnReset;
extern	const	SocketType		CodeInProgress;
extern	const	SocketType		CodeWouldBlock;

// ============================================================================

class BFC_Net_DLL SocketData {

public :

	SocketData(
		const	SocketType	pDesc = InvSocket
	) :
		desc	( pDesc ) {
	}

	enum Mode {
		DGram,
		Stream
	};

	Bool isValid(
	) const {
		return ( desc != InvSocket );
	}

	void openSD(
		const	Mode		mode
	);

	SocketType getSD(
	) const {
		return desc;
	}

	void closeSD(
	);

	void bindSD(
			SockAddrCPtr	pSockAddr
	);

	void connectSD(
			SockAddrCPtr	pSockAddr
	);

	SockAddrCPtr getSockName(
	);

protected :

private :

	SocketType	desc;		///< Socket descriptor.

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_SocketData_H_

// ============================================================================

