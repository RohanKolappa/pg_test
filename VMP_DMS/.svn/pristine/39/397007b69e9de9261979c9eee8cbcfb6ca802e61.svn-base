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
//	BFC.DOM.XProcInstr.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XProcInstr_H_
#define _BFC_DOM_XProcInstr_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XProcInstrPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DOM.XNode.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM ProcessingInstruction.
///
/// The ProcessingInstruction interface represents a "processing instruction",
/// used in XML as a way to keep processor-specific information in the text of
/// the document.
///
/// No lexical check is done on the content of a processing instruction and it
/// is therefore possible to have the character sequence "?>" in the content,
/// which is illegal a processing instruction per section 2.6 of [XML 1.0]. The
/// presence of this character sequence must generate a fatal error during
/// serialization.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XProcInstr : virtual public XNode {

public :

	/// \brief Returns the target of this processing instruction.
	///
	/// XML defines this as being the first token following the markup that
	/// begins the processing instruction.

	virtual const Buffer & getTarget(
	) const = 0;

	/// \brief Returns the content of this processing instruction.
	///
	/// This is from the first non white space character after the target
	/// to the character immediately preceding the ?>.

	virtual const Buffer & getData(
	) const = 0;

	/// \brief Sets the context of this processing instruction.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR)
	///	Raised when the node is readonly.

	virtual void setData(
		const	Buffer &	pData
	) = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XProcInstr_H_

// ============================================================================

