// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::IO".
// 
// "TBFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.IO.Pipe.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_Pipe_H
#define _TBFC_Light_Pipe_H

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SPtr.h"
#include "TBFC.IO.FinalDevice.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

// FIXME: to reimplement!!!

#if 0

/// A pipe.

///
/// \ingroup TBFC_Light

class TBFC_Light_DLL Pipe : public IODevice {

public :

	Pipe();

	virtual ~Pipe();

	//
	//	bool create();
	//

	virtual void create();

	virtual void disconnectSend();
	virtual void disconnectRecv();

	BASE_CLASS_GEN_EXCEPTION("TBFC.Pipe");

	CLASS_EXCEPTION(AlreadyCreated, "Already created");
	CLASS_EXCEPTION(CantSetNonBlockingReceives, "Can't force non-blocking receives");
	CLASS_EXCEPTION(CantSetNonBlockingSends, "Can't force non-blocking sends");
	CLASS_EXCEPTION(CantSystemClose, "Can't system close");
	CLASS_EXCEPTION(CantSystemCreate, "Can't system create");
	CLASS_EXCEPTION(CantSystemRead, "Can't system read");
	CLASS_EXCEPTION(CantSystemWrite, "Can't system write");
	CLASS_EXCEPTION(NotImplemented, "Not implemented");

protected :

	//	=========================================================
	//	===              low-level IO methods                 ===
	//	=========================================================

	virtual Uint32 rawSend(
		const	Uchar	*buffer,
			Uint32	towrite
	);

	virtual Uint32 rawRecv(
			Uchar	*buffer,
			Uint32	toread
	);

	virtual Uint32 rawSeek(
			Int32	offset,
			SeekDirection	direction
	);

	virtual void rawClose(
	);

private :

	// internal state

	int		fd0;	// read
	int		fd1;	// write

	// others...

	Pipe(
		const Pipe&
	);

	Pipe& operator = (
		const Pipe&
	);

};

#endif

// ============================================================================

} // namespace IO
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_Pipe_H

// ============================================================================

