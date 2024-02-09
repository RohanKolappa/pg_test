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
//	BFC.Net.ContentHandlerFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_ContentHandlerFactory_H_
#define _BFC_Net_ContentHandlerFactory_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.ContentHandlerFactoryPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Net.ContentHandler.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Defines a factory for content handlers. An implementation of this
///	interface should map a MIME type into an instance of ContentHandler.
///
/// This interface is used by the URLStreamHandler class to create a
/// ContentHandler for a MIME type.
///
/// \ingroup BFC_Net

class BFC_Net_DLL ContentHandlerFactory : virtual public SObject {

public :

	/// \brief Creates a new ContentHandler to read an object of type
	///	\a mimetype.
	///
	/// \param mimetype
	///	The MIME type for which a content handler is desired.
	///
	/// \returns
	///	A new ContentHandler to read an object from a URLStreamHandler.

	virtual ContentHandlerPtr createContentHandler(
		const	Buffer &	mimetype
	) = 0;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_ContentHandlerFactory_H_

#endif // 0

// ============================================================================

