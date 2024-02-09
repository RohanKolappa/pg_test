// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::DOM".
// 
// "BFC::DOM" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::DOM" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::DOM"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.DOM.Notation.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Notation_H_
#define _BFC_DOM_Notation_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.Node.h"
#include "BFC.DOM.NotationImplPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief The Notation class represents an XML notation.
/// 
/// A notation either declares, by name, the format of an unparsed
/// entity (see section 4.7 of the XML 1.0 specification), or is used
/// for formal declaration of processing instruction targets (see
/// section 2.6 of the XML 1.0 specification).
/// 
/// DOM does not support editing notation nodes; they are therefore
/// read-only.
/// 
/// A notation node does not have any parent.
/// 
/// You can retrieve the publicId() and systemId() from a notation
/// node.
/// 
/// For further information about the Document Object Model see
/// \l{http://www.w3.org/TR/REC-DOM-Level-1/}{Level 1} and
/// \l{http://www.w3.org/TR/DOM-Level-2-Core/}{Level 2 Core}.
/// For a more general introduction of the DOM implementation see the
/// Document documentation.
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL Notation : public Node {

public:

	/// \brief Constructs a new Notation.

	Notation(
	);

	/// \brief Constructs a copy of \a x.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Notation(
		const	Notation &	x
	);

	/// \brief Assigns \a x to this DOM notation.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Notation & operator = (
		const	Notation &
	);

	/// \brief Returns the public identifier of this notation.

	Buffer publicId(
	) const;

	/// \brief Returns the system identifier of this notation.

	Buffer systemId(
	) const;

	virtual NodeType getNodeType(
	) const;

private:

	Notation(
		const	NotationImplPtr &
	);

	friend class Document;
	friend class Node;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Notation_H_

// ============================================================================

