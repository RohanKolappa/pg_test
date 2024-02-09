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
//	VMP.M2S.TS2PSConverter.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TS2PSConverter_h_
#define _VMP_M2S_TS2PSConverter_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TS2PSConverterPtr.h"

// ============================================================================

#include "BFC.IO.DevicePtr.h"

#include "VMP.GPC.Consumer.h"

#include "VMP.M2S.TimeStampPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TS2PSConverter : public GPC::Consumer {

public :

	TS2PSConverter(
	);

	void attachDevice(
			BFC::IO::DevicePtr
					outputDevice
	);

	void detachDevice(
	);

	virtual void putObject(
			BFC::SPtr	packet
	);

protected :

	void handlePES(
			PESPacketPtr	packet
	);

	void handlePCR(
			TimeStampCPtr	pcr
	);

private :

	BFC::IO::DevicePtr	device;

	TimeStampCPtr		syncPoint;

	/// \brief Forbidden copy constructor.

	TS2PSConverter(
		const	TS2PSConverter &
	);

	/// \brief Forbidden copy operator.

	TS2PSConverter & operator = (
		const	TS2PSConverter &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TS2PSConverter_h_

// ============================================================================

