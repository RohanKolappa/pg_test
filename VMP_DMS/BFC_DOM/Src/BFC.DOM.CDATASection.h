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
//	BFC.DOM.CDATASection.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_CDATASection_H_
#define _BFC_DOM_CDATASection_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.CDATASectionImplPtr.h"
#include "BFC.DOM.Text.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/// \brief Represents an XML CDATA section.
/// 
/// CDATA sections are used to escape blocks of text containing
/// characters that would otherwise be regarded as markup. The only
/// delimiter that is recognized in a CDATA section is the "]]&gt;"
/// string that terminates the CDATA section. CDATA sections cannot be
/// nested. Their primary purpose is for including material such as
/// XML fragments, without needing to escape all the delimiters.
/// 
/// Adjacent CDATASection nodes are not merged by the Node::normalize()
/// function.
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL CDATASection : public Text {

public:

	/// \brief Constructs an empty CDATA section.
	///
	/// To create a CDATA section with content, use the
	/// Document::createCDATASection() function.

	CDATASection(
	);

	/// \brief Constructs a copy of \a x.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	CDATASection(
		const	CDATASection	& x
	);

	/// \brief Assigns \a x to this CDATA section.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	CDATASection & operator = (
		const	CDATASection	& x
	);

	virtual NodeType getNodeType() const;

private:

	CDATASection(
			CDATASectionImplPtr
	);

	friend class Document;
	friend class Node;

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_CDATASection_H_

// ============================================================================

