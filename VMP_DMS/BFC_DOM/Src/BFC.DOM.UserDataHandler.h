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
//	BFC.DOM.UserDataHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_UserDataHandler_H_
#define _BFC_DOM_UserDataHandler_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.UserDataHandlerPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.DOM.XNodePtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM UserDataHandler.
///
/// When associating an object to a key on a node using Node.setUserData(), the
/// application can provide a handler that gets called when the node the object
/// is associated to is being cloned, imported, or renamed. This can be used by
/// the application to implement various behaviors regarding the data it
/// associates to the DOM nodes. This interface defines that handler.
///
/// \since DOM Level 3
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL UserDataHandler : virtual public SObject {

public :

///An integer indicating the type of operation being performed on a node.
	enum OperationType {
		NODE_CLONED	= 1,	///< The node is cloned, using Node.cloneNode().
		NODE_IMPORTED	= 2,	///< The node is imported, using Document.importNode().
		NODE_DELETED	= 3,	///< The node is deleted.
///
///Note: This may not be supported or may not be reliable in certain environments, such as Java, where the implementation has no real control over when objects are actually deleted.
		NODE_RENAMED	= 4,	///< The node is renamed, using Document.renameNode().
		NODE_ADOPTED	= 5	///< The node is adopted, using Document.adoptNode().
	};

///This method is called whenever the node for which this handler is registered is imported or cloned.
///DOM applications must not raise exceptions in a UserDataHandler. The effect of throwing exceptions from the handler is DOM implementation dependent.
///Parameters
///
///operation of type unsigned short
///    Specifies the type of operation that is being performed on the node.
///key of type DOMString
///    Specifies the key for which this handler is being called.
///data of type DOMUserData
///    Specifies the data for which this handler is being called.
///src of type Node
///    Specifies the node being cloned, adopted, imported, or renamed. This is null when the node is being deleted.
///dst of type Node
///    Specifies the node newly created if any, or null.

	virtual void handle(
		const	OperationType	operation,
		const	Buffer &	key,
			SPtr		data,
			XNodePtr	src, 
			XNodePtr	dst
	) = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_UserDataHandler_H_

// ============================================================================

