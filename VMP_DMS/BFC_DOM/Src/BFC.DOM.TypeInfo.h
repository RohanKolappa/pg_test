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
//	BFC.DOM.TypeInfo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_TypeInfo_H_
#define _BFC_DOM_TypeInfo_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.TypeInfoPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief [no brief description]
/// The TypeInfo interface represents a type referenced from Element  or Attr  nodes, specified in the schemas  associated with the document. The type is a pair of a namespace URI and name properties, and depends on the document's schema.
///
///If the document's schema is an XML DTD [XML 1.0], the values are computed as follows:
///
///    * If this type is referenced from an Attr node, typeNamespace is "http://www.w3.org/TR/REC-xml" and typeName represents the [attribute type] property in the [XML Information Set]. If there is no declaration for the attribute, typeNamespace and typeName are null.
///    * If this type is referenced from an Element node, typeNamespace and typeName are null.
///
///If the document's schema is an XML Schema [XML Schema Part 1], the values are computed as follows using the post-schema-validation infoset contributions (also called PSVI contributions):
///
///    * If the [validity] property exists AND is "invalid" or "notKnown": the {target namespace} and {name} properties of the declared type if available, otherwise null.
///
///      Note: At the time of writing, the XML Schema specification does not require exposing the declared type. Thus, DOM implementations might choose not to provide type information if validity is not valid.
///    * If the [validity] property exists and is "valid":
///         1. If [member type definition] exists:
///               1. If {name} is not absent, then expose {name} and {target namespace} properties of the [member type definition] property;
///               2. Otherwise, expose the namespace and local name of the corresponding anonymous type name.
///         2. If the [type definition] property exists:
///               1. If {name} is not absent, then expose {name} and {target namespace} properties of the [type definition] property;
///               2. Otherwise, expose the namespace and local name of the corresponding anonymous type name.
///         3. If the [member type definition anonymous] exists:
///               1. If it is false, then expose [member type definition name] and [member type definition namespace] properties;
///               2. Otherwise, expose the namespace and local name of the corresponding anonymous type name.
///         4. If the [type definition anonymous] exists:
///               1. If it is false, then expose [type definition name] and [type definition namespace] properties;
///               2. Otherwise, expose the namespace and local name of the corresponding anonymous type name.
///
///Note: Other schema languages are outside the scope of the W3C and therefore should define how to represent their type systems using TypeInfo.
///
/// \since DOM Level 3
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL TypeInfo : virtual public SObject {

public :

///If the document's schema is an XML Schema [XML Schema Part 1], this constant represents the union  if simple types are involved.
///The reference type definition is derived by union from the other type definition if there exists two type definitions T1 and T2 such as the reference type definition is derived from T1 by DERIVATION_RESTRICTION or DERIVATION_EXTENSION, T2 is derived from the other type definition by DERIVATION_RESTRICTION, T1 has {variety} union, and one of the {member type definitions} is T2. Note that T1 could be the same as the reference type definition, and T2 could be the same as the other type definition. 
	virtual const Buffer & getTypeName(
	) const = 0;

///The namespace of the type declared for the associated element or attribute or null if the element does not have declaration or if no namespace information is available. 
	virtual const Buffer & getTypeNamespace(
	) const = 0;

/// These are the available values for the derivationMethod parameter used by the method TypeInfo.isDerivedFrom(). It is a set of possible types of derivation, and the values represent bit positions. If a bit in the derivationMethod parameter is set to 1, the corresponding type of derivation will be taken into account when evaluating the derivation between the reference type definition and the other type definition. When using the isDerivedFrom method, combining all of them in the derivationMethod parameter is equivalent to invoking the method for each of them separately and combining the results with the OR boolean function. This specification only defines the type of derivation for XML Schema.
///
///In addition to the types of derivation listed below, please note that:
///
///    * any type derives from xsd:anyType.
///    * any simple type derives from xsd:anySimpleType by restriction.
///    * any complex type does not derive from xsd:anySimpleType by restriction.


	enum DerivationMethods {

///If the document's schema is an XML Schema [XML Schema Part 1], this constant represents the derivation by restriction  if complex types are involved, or a restriction  if simple types are involved.
///The reference type definition is derived by restriction from the other type definition if the other type definition is the same as the reference type definition, or if the other type definition can be reached recursively following the {base type definition} property from the reference type definition, and all the derivation methods involved are restriction. 
		DERIVATION_RESTRICTION         = 0x00000001,

///If the document's schema is an XML Schema [XML Schema Part 1], this constant represents the derivation by extension.
///The reference type definition is derived by extension from the other type definition if the other type definition can be reached recursively following the {base type definition} property from the reference type definition, and at least one of the derivation methods involved is an extension. 
		DERIVATION_EXTENSION           = 0x00000002,

///If the document's schema is an XML Schema [XML Schema Part 1], this constant represents the union  if simple types are involved.
///The reference type definition is derived by union from the other type definition if there exists two type definitions T1 and T2 such as the reference type definition is derived from T1 by DERIVATION_RESTRICTION or DERIVATION_EXTENSION, T2 is derived from the other type definition by DERIVATION_RESTRICTION, T1 has {variety} union, and one of the {member type definitions} is T2. Note that T1 could be the same as the reference type definition, and T2 could be the same as the other type definition. 
		DERIVATION_UNION               = 0x00000004,

///If the document's schema is an XML Schema [XML Schema Part 1], this constant represents the list.
///The reference type definition is derived by list from the other type definition if there exists two type definitions T1 and T2 such as the reference type definition is derived from T1 by DERIVATION_RESTRICTION or DERIVATION_EXTENSION, T2 is derived from the other type definition by DERIVATION_RESTRICTION, T1 has {variety} list, and T2 is the {item type definition}. Note that T1 could be the same as the reference type definition, and T2 could be the same as the other type definition. 
		DERIVATION_LIST                = 0x00000008

	};

///This method returns if there is a derivation between the reference type definition, i.e. the TypeInfo on which the method is being called, and the other type definition, i.e. the one passed as parameters.
///Parameters
///
///typeNamespaceArg of type DOMString
///    the namespace of the other type definition.
///typeNameArg of type DOMString
///    the name of the other type definition.
///derivationMethod of type unsigned long
///    the type of derivation and conditions applied between two types, as described in the list of constants provided in this interface.
///
///Return Value
///
///boolean
///	
///
///If the document's schema is a DTD or no schema is associated with the document, this method will always return false.
///If the document's schema is an XML Schema, the method will true if the reference type definition is derived from the other type definition according to the derivation parameter. If the value of the parameter is 0 (no bit is set to 1 for the derivationMethod parameter), the method will return true if the other type definition can be reached by recursing any combination of {base type definition}, {item type definition}, or {member type definitions} from the reference type definition.

	virtual Bool isDerivedFrom(
		const	Buffer &	typeNamespaceArg, 
		const	Buffer &	typeNameArg, 
		const	Uint32		derivationMethod
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_TypeInfo_H_

// ============================================================================

