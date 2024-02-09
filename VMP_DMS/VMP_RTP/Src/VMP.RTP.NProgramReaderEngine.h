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
//	VMP.RTP.NProgramReaderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_NProgramReaderEngine_H_
#define _VMP_RTP_NProgramReaderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.NProgramReaderEnginePtr.h"

// ============================================================================

#include "BFC.MT.SchedulerPtr.h"

#include "BFC.Net.NetIfacePtr.h"

#include "BFC.TL.Array.h"

#include "VMP.L2.ProxyProdEngine.h"
#include "VMP.L2.PumperPtr.h"

#include "VMP.RTP.SProgramPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief Reads a mux of NSession from the network.
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL NProgramReaderEngine : public L2::ProxyProdEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new NProgramReaderEngine.

	NProgramReaderEngine(
	);

	/// \brief Destroys this object.

	virtual ~NProgramReaderEngine(
	);

	void init(
			SProgramCPtr	pSProgram,
			BFC::Net::NetIfacePtr
					pNetIface = BFC::Net::NetIfacePtr()
	);

	virtual void fireWork(
	);

	virtual void haltWork(
	);

	virtual void waitDone(
	);

	virtual BFC::Bool isDone(
	) const;

protected :

private :

	SProgramCPtr			sProgram;
	BFC::Net::NetIfacePtr		netIface;
	BFC::MT::SchedulerPtr		taskSchd;
	BFC::Array< L2::PumperPtr >	pumpList;

	/// \brief Forbidden copy constructor.

	NProgramReaderEngine(
		const	NProgramReaderEngine &
	);

	/// \brief Forbidden copy operator.

	NProgramReaderEngine & operator = (
		const	NProgramReaderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_NProgramReaderEngine_H_

// ============================================================================

