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
//	BFC.DOM.XDocType.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XDocType_H_
#define _BFC_DOM_XDocType_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XDocTypePtr.h"

// ============================================================================

#include "BFC.DOM.XNode.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM DocumentType.
///
/// Each Document has a doctype attribute whose value is either null or a
/// DocumentType object. The DocumentType interface in the DOM Core provides an
/// interface to the list of entities that are defined for the document, and
/// little else because the effect of namespaces and the various XML schema
/// efforts on DTD representation are not clearly understood as of this
/// writing.
///
/// DOM Level 3 doesn't support editing DocumentType nodes. DocumentType nodes
/// are read-only.
///
/// \since DOM Level 1
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XDocType : virtual public XNode {

public :

#if 0

	/// \brief Returns the name of DTD; i.e., the name immediately
	///	following the DOCTYPE keyword.
	///
	/// \since DOM Level 1

	virtual const Buffer & getName(
	) const = 0;

	/// \brief Returns a NamedNodeMap containing the general entities, both
	///	external and internal, declared in the DTD.
	///
	/// Parameter entities are not contained. Duplicates are discarded. For
	/// example in:
	/*!
		\verbatim
			<!DOCTYPE ex SYSTEM "ex.dtd" [
			  <!ENTITY foo "foo">
			  <!ENTITY bar "bar">
			  <!ENTITY bar "bar2">
			  <!ENTITY % baz "baz">
			]>
			<ex/>
		\endverbatim
	*/
	/// the interface provides access to foo and the first declaration of
	/// bar but not the second declaration of bar or baz. Every node in
	/// this map also implements the Entity interface.
	///
	/// The DOM Level 2 does not support editing entities, therefore
	/// entities cannot be altered in any way.
	///
	/// \since DOM Level 1

	virtual XNamedNodeMapPtr getEntities(
	) const = 0;

	/// \brief Returns a NamedNodeMap containing the notations declared in
	///	the DTD.
	///
	/// Duplicates are discarded. Every node in this map also implements
	/// the Notation interface.
	///
	/// The DOM Level 2 does not support editing notations, therefore
	/// notations cannot be altered in any way.
	///
	/// \since DOM Level 1

	virtual XNamedNodeMapPtr getNotations(
	) const = 0;

	/// \brief Returns the public identifier of the external subset.
	///
	/// \since DOM Level 2

	virtual const Buffer & getPublicId(
	) const = 0;

	/// \brief Returns the system identifier of the external subset.
	///
	/// This may be an absolute URI or not.
	///
	/// \since DOM Level 2

	virtual const Buffer & getSystemId(
	) const = 0;

	/// \brief Returns the internal subset as a string, or null if there is
	///	none.
	///
	/// This is does not contain the delimiting square brackets.
	///
	/// \note
	///	The actual content returned depends on how much information is
	///	available to the implementation. This may vary depending on
	///	various parameters, including the XML processor used to build
	///	the document.
	///
	/// \since DOM Level 2

	virtual const Buffer & getInternalSubset(
	) const = 0;

#endif // 0

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XDocType_H_

// ============================================================================

