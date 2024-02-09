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
//	VMP.M2S.TSPacketWriter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_TSPacketWriter_H_
#define _VMP_M2S_TSPacketWriter_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSPacketWriterPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.IO.FilePtr.h"

#include "VMP.GPC.Consumer.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL TSPacketWriter : public GPC::Consumer {

public :

	/// \brief Creates a new TSPacketWriter.

	TSPacketWriter(
		const	BFC::Buffer &	pFilename
	);

	virtual void putObject(
			BFC::SPtr	pPESPacket
	);

protected :

private :

	BFC::IO::FilePtr	file;

	/// \brief Forbidden default constructor.

	TSPacketWriter(
	);

	/// \brief Forbidden copy constructor.

	TSPacketWriter(
		const	TSPacketWriter &
	);

	/// \brief Forbidden copy operator.

	TSPacketWriter & operator = (
		const	TSPacketWriter &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSPacketWriter_H_

// ============================================================================

