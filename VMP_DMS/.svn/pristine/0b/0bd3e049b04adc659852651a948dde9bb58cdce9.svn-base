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
//	BFC.DOM.XEntityRef.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XEntityRef_H_
#define _BFC_DOM_XEntityRef_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XEntityRefPtr.h"

// ============================================================================

#include "BFC.DOM.XNode.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM EntityReference.
///
/// EntityReference nodes may be used to represent an entity reference in the
/// tree. Note that character references and references to predefined entities
/// are considered to be expanded by the HTML or XML processor so that
/// characters are represented by their Unicode equivalent rather than by an
/// entity reference. Moreover, the XML processor may completely expand
/// references to entities while building the Document, instead of providing
/// EntityReference nodes. If it does provide such nodes, then for an
/// EntityReference node that represents a reference to a known entity, an
/// Entity exists, and the subtree of the EntityReference node is a copy of the
/// Entity node subtree. However, the latter may not be true when an entity
/// contains an unbound namespace prefix. In such a case, because the namespace
/// prefix resolution depends on where the entity reference is, the descendants
/// of the EntityReference node may be bound to different namespace URIs. When
/// an EntityReference node represents a reference to an unknown entity, the
/// node has no children and its replacement value, when used by Attr.value for
/// example, is empty.
///
/// As for Entity nodes, EntityReference nodes and all their descendants are
/// readonly.
///
/// \note
///	EntityReference nodes may cause element content and attribute value
///	normalization problems when, such as in XML 1.0 and XML Schema, the
///	normalization is performed after entity reference are expanded.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XEntityRef : virtual public XNode {

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XEntityRef_H_

// ============================================================================

