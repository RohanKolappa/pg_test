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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
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
//	BFC.Net.ContentHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#ifndef _BFC_Net_ContentHandler_H_
#define _BFC_Net_ContentHandler_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.ContentHandlerPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Net.URLConnection.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Superclass of all classes that read an SObject from a URLConnection.
///
/// An application does not generally call the getContent() method in this
/// class directly. Instead, an application calls the getContent() method in
/// class URLConnection. The application's content handler factory (an instance
/// of a class that implements the interface ContentHandlerFactory set up by a
/// call to setContentHandler()) is called with a String giving the MIME type
/// of the object being received on the socket. The factory returns an instance
/// of a subclass of ContentHandler, and its getContent method is called to
/// create the object.
///
/// \ingroup BFC_Net

class BFC_Net_DLL ContentHandler : virtual public SObject {

public :

	/// \brief Given a URLConnection positioned at the beginning of the
	///	representation of an object, this method reads that stream and
	///	creates an object from it.
	///
	/// \param urlc
	///	A URL connection.
	///
	/// \returns
	///	The object read by the ContentHandler.

	virtual SPtr getContent(
			URLConnectionPtr	urlc
	) = 0;

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_ContentHandler_H_

#endif // 0

// ============================================================================

