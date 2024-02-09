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
//	BFC.DOM.XComment.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XComment_H_
#define _BFC_DOM_XComment_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XCommentPtr.h"

// ============================================================================

#include "BFC.DOM.XCharData.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Comment.
///
/// This interface inherits from CharacterData and represents the content of a
/// comment, i.e., all the characters between the starting '\<!--' and ending
/// '--\>'. Note that this is the definition of a comment in XML, and, in
/// practice, HTML, although some HTML tools may implement the full SGML
/// comment structure.
///
/// No lexical check is done on the content of a comment and it is therefore
/// possible to have the character sequence "--" (double-hyphen) in the
/// content, which is illegal in a comment per section 2.5 of [XML 1.0]. The
/// presence of this character sequence must generate a fatal error during
/// serialization. 
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XComment : virtual public XCharData {

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XComment_H_

// ============================================================================

