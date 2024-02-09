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
//	BFC.DOM.ProcessingInstruction.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_ProcessingInstruction_H_
#define _BFC_DOM_ProcessingInstruction_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.Node.h"
#include "BFC.DOM.ProcessingInstructionImplPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief The ProcessingInstruction class represents an XML processing
///	instruction.
///
/// Processing instructions are used in XML to keep processor-specific
/// information in the text of the document.
/// 
/// The content of the processing instruction is retrieved with getData()
/// and set with setData(). The processing instruction's target is
/// retrieved with target().
/// 
/// For further information about the Document Object Model see
/// \l{http://www.w3.org/TR/REC-DOM-Level-1/}{Level 1} and
/// \l{http://www.w3.org/TR/DOM-Level-2-Core/}{Level 2 Core}.
/// For a more general introduction of the DOM implementation see the
/// Document documentation.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL ProcessingInstruction : public Node {

public:

	/// \brief Constructs an empty processing instruction.
	///
	/// Use Document::createProcessingInstruction() to create a processing
	/// instruction with content.

	ProcessingInstruction(
	);

	/// \brief Constructs a copy of \a x.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	ProcessingInstruction(
		const	ProcessingInstruction &	x
	);

	/// \brief Assigns \a x to this processing instruction.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	ProcessingInstruction & operator = (
		const	ProcessingInstruction &	x
	);

	/// \brief Returns the target of this processing instruction.
	/// 
	/// \sa getData()

	Buffer target(
	) const;

	/// \brief Returns the content of this processing instruction.
	///
	/// \sa setData(), target()

	Buffer getData(
	) const;

	/// \brief Sets the data contained in the processing instruction to
	///	\a d.
	/// 
	/// \sa getData()

	void setData(
		const	Buffer &	d
	);

	virtual NodeType getNodeType(
	) const;

private:

	friend class Document;
	friend class Node;

	ProcessingInstruction(
		const	ProcessingInstructionImplPtr &
	);

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_ProcessingInstruction_H_

// ============================================================================

