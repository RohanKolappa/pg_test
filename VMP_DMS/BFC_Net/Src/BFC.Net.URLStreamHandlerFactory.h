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
//	BFC.Net.URLStreamHandlerFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_URLStreamHandlerFactory_H_
#define _BFC_Net_URLStreamHandlerFactory_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.URLStreamHandlerFactoryPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.Net.URLStreamHandler.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Defines a factory for URL stream protocol handlers.
///
/// This interface is used by the URL class to create a URLStreamHandler for a
/// specific protocol.
///
/// \ingroup BFC_Net

class BFC_Net_DLL URLStreamHandlerFactory : virtual public SObject {

public :

	// \brief Creates a new URLStreamHandler instance with the specified
	///	protocol.
	///
	/// \param pProtocol
	///	The protocol ("ftp", "http", "nntp", etc.).
	///
	/// \returns
	///	A URLStreamHandler for the specific protocol.

	virtual URLStreamHandlerPtr createURLStreamHandler(
		const	Buffer &	pProtocol
	) = 0;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_URLStreamHandlerFactory_H_

#endif // 0

// ============================================================================

