// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::LightRCom".
// 
// "BFC::LightRCom" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::LightRCom" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::LightRCom"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.BufferSerializer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_BufferSerializer_H_
#define _BFC_BufferSerializer_H_

// ============================================================================

#include "BFC.LightRCom.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

namespace BFC {

// ============================================================================

class BFC_LightRCom_DLL BufferSerializer {

public :

	static void doSerialize(
		const	Buffer			& buffer,
			IO::ByteOutputStreamPtr	& ostream
	);

	static Buffer unSerialize(
			IO::ByteInputStreamPtr	& istream
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_BufferSerializer_H_

// ============================================================================

