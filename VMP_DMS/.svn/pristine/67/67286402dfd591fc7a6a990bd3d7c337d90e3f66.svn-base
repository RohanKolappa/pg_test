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
//	VMP.M2S.PES2VMPConverter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_PES2VMPConverter_H_
#define _VMP_M2S_PES2VMPConverter_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.PES2VMPConverterPtr.h"

// ============================================================================

#include "VMP.BytesSessionPtr.h"

#include "VMP.GPC.Consumer.h"

#include "VMP.L2.PushEnginePtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL PES2VMPConverter : public GPC::Consumer {

public :

	/// \brief Creates a new PES2VMPConverter.

	PES2VMPConverter(
			BytesSessionPtr	pOutpSess
	);

	void setPeerEngine(
			L2::PushEnginePtr
					pConsumer
	);

	void delPeerEngine(
	);

	virtual void putObject(
			BFC::SPtr	pPESPacket
	);

protected :

private :

	BytesSessionPtr		outpSess;
	L2::PushEnginePtr	consumer;

	/// \brief Forbidden default constructor.

	PES2VMPConverter(
	);

	/// \brief Forbidden copy constructor.

	PES2VMPConverter(
		const	PES2VMPConverter &
	);

	/// \brief Forbidden copy operator.

	PES2VMPConverter & operator = (
		const	PES2VMPConverter &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_PES2VMPConverter_H_

// ============================================================================

