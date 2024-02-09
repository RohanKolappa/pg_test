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
//	BFC.Net.DefaultProxySelector.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_DefaultProxySelector_H_
#define _BFC_Net_DefaultProxySelector_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Net {

BFC_PTR_DECL( BFC_Net_DLL, DefaultProxySelector )

} // namespace Net
} // namespace BFC

// ============================================================================

#include "BFC.Net.ProxySelector.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Default proxy selector.
///
/// \ingroup BFC_Net

class BFC_Net_DLL DefaultProxySelector : public ProxySelector {

public :

	virtual ProxyCArray select(
		const	URL &		url
	);

	virtual void connectFailed(
		const	URL &		url,
		const	SockAddrCPtr &	addr
	);

protected :

	static ProxyCPtr getProxy(
		const	Buffer &	pProt,
		const	Buffer &	pHost
	);

	static Bool isLoopback(
		const	Buffer &	phost
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_DefaultProxySelector_H_

// ============================================================================

