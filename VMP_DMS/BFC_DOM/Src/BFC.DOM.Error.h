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
//	BFC.DOM.Error.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_Error_H_
#define _BFC_DOM_Error_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.ErrorPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.DOM.LocatorPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Error.
///
/// Error is an interface that describes an error.
///
/// \since DOM Level 3
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL Error : virtual public SObject {

public :

	/// \brief An integer indicating the severity of the error. 

	enum Severity {

		/// \brief The severity of the error described by the Error is
		///	warning.
		///
		/// A SEVERITY_WARNING will not cause the processing to stop,
		/// unless ErrorHandler::handleError() returns false.

		SEVERITY_WARNING               = 1,

		/// \brief The severity of the error described by the Error is
		///	error.
		/// A SEVERITY_ERROR  may not cause the processing to stop if
		/// the error can be recovered, unless
		/// ErrorHandler::handleError() returns false.

		SEVERITY_ERROR                 = 2,

		/// \brief The severity of the error described by the Error is
		///	fatal error.
		///
		/// A SEVERITY_FATAL_ERROR will cause the normal processing to
		/// stop. The return value of ErrorHandler::handleError() is
		/// ignored unless the implementation chooses to continue, in
		/// which case the behavior becomes undefined.

		SEVERITY_FATAL_ERROR           = 3

	};

	/// \brief Returns the severity of the error, either SEVERITY_WARNING,
	///	SEVERITY_ERROR, or SEVERITY_FATAL_ERROR.

	virtual Severity getSeverity(
	) const = 0;

	/// \brief Returns an implementation specific string describing the
	///	error that occurred.

	virtual const Buffer & getMessage(
	) const = 0;

	/// \brief Returns a Buffe indicating which related data is expected in
	///	relatedData.
	///
	/// Users should refer to the specification of the error in order to
	/// find its Buffer type and relatedData definitions if any.
	///
	/// As an example, Document::normalizeDocument() does generate warnings
	/// when the "split-cdata-sections" parameter is in use. Therefore, the
	/// method generates a SEVERITY_WARNING with type
	/// "cdata-sections-splitted" and the first CDATASection node in
	/// document order resulting from the split is returned by the
	/// relatedData attribute.

	virtual const Buffer & getType(
	) const = 0;

	/// \brief Returns the related platform dependent exception if any.

	virtual SPtr getRelatedException(
	) const = 0;

	/// \brief Returns the related Error::getType() dependent data if any.

	virtual SPtr getRelatedData(
	) const = 0;

	/// \brief Returns the location of the error.

	virtual LocatorPtr getLocation(
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Error_H_

// ============================================================================

