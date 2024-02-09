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
//	VMP.GPC.ProxyProducer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_GPC_ProxyProducer_H_
#define _VMP_GPC_ProxyProducer_H_

// ============================================================================

#include "VMP.GPC.DLL.h"

// ============================================================================

#include "VMP.GPC.ProxyProducerPtr.h"

// ============================================================================

#include "VMP.GPC.Producer.h"
#include "VMP.GPC.ProducerProxy.h"

// ============================================================================

namespace VMP {
namespace GPC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_GPC

class VMP_GPC_DLL ProxyProducer :

	public Producer,
	public ProducerProxy {

};

// ============================================================================

} // namespace GPC
} // namespace VMP

// ============================================================================

#endif // _VMP_GPC_ProxyProducer_H_

// ============================================================================

