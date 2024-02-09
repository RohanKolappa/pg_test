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
//	BFC.DOM.Exception.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_DOM_Exception_H_
#define _BFC_DOM_Exception_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOMException.
///
/// DOM operations only raise exceptions in "exceptional" circumstances, i.e.,
/// when an operation is impossible to perform (either for logical reasons,
/// because data is lost, or because the implementation has become unstable).
/// In general, DOM methods return specific error values in ordinary processing
/// situations, such as out-of-bound errors when using NodeList.
///
/// Implementations should raise other exceptions under other circumstances.
/// For example, implementations should raise an implementation-dependent
/// exception if a null argument is passed when null was not expected.
///
/// Some languages and object systems do not support the concept of exceptions.
/// For such systems, error conditions may be indicated using native error
/// reporting mechanisms. For some bindings, for example, methods may return
/// error codes similar to those listed in the corresponding method
/// descriptions.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL Exception : public BFC::Exception {

public :

	/// \brief An integer indicating the type of error generated.
	///
	/// \note
	///	Other numeric codes are reserved for W3C for possible future
	///	use.

	enum Code {

		/// \brief If index or size is negative, or greater than the
		///	allowed value.

		INDEX_SIZE_ERR			= 1,

		/// \brief If the specified range of text does not fit into a
		///	DOMString.

		DOMSTRING_SIZE_ERR		= 2,

		/// \brief If any Node is inserted somewhere it doesn't belong.

		HIERARCHY_REQUEST_ERR		= 3,

		/// \brief If a Node  is used in a different document than the
		///	one that created it (that doesn't support it).

		WRONG_DOCUMENT_ERR		= 4,

		/// \brief If an invalid or illegal character is specified,
		///	such as in an XML name.

		INVALID_CHARACTER_ERR		= 5,

		/// \brief If data is specified for a Node  which does not
		///	support data.

		NO_DATA_ALLOWED_ERR		= 6,

		/// \brief If an attempt is made to modify an object where
		///	modifications are not allowed.

		NO_MODIFICATION_ALLOWED_ERR	= 7,

		/// \brief If an attempt is made to reference a Node  in a
		///	context where it does not exist.

		NOT_FOUND_ERR			= 8,

		/// \brief If the implementation does not support the requested
		///	type of object or operation.

		NOT_SUPPORTED_ERR		= 9,

		/// \brief If an attempt is made to add an attribute that is
		///	already in use elsewhere.

		INUSE_ATTRIBUTE_ERR		= 10,

		/// \brief If an attempt is made to use an object that is not,
		///	or is no longer, usable.
		///
		/// \since DOM Level 2

		INVALID_STATE_ERR		= 11,

		/// \brief If an invalid or illegal string is specified.
		///
		/// \since DOM Level 2

		SYNTAX_ERR			= 12,

		/// \brief If an attempt is made to modify the type of the
		///	underlying object.
		///
		/// \since DOM Level 2

		INVALID_MODIFICATION_ERR	= 13,

		/// \brief If an attempt is made to create or change an object
		///	in a way which is incorrect with regard to namespaces.
		///
		/// \since DOM Level 2

		NAMESPACE_ERR			= 14,

		/// \brief If a parameter or an operation is not supported by
		///	the underlying object.
		///
		/// \since DOM Level 2

		INVALID_ACCESS_ERR		= 15,

		/// \brief If a call to a method such as insertBefore or
		///	removeChild would make the Node  invalid with respect to "partial validity", this exception would be raised and the operation would not be done. This code is used in [DOM Level 3 Validation]. Refer to this specification for further information.
		///
		/// \since DOM Level 3

		VALIDATION_ERR			= 16,

		/// \brief If the type of an object is incompatible with the
		///	expected type of the parameter associated to the object. 
		///
		/// \since DOM Level 3

		TYPE_MISMATCH_ERR		= 17,

		/// \brief First invalid code.

		MaxCode

	};

	/// \brief Creates a new Exception with code \a pCode.

	Exception(
		const	Code		pCode
	);

private :

	Code	code;

	static const Char * msg[];

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Exception_H_

// ============================================================================

