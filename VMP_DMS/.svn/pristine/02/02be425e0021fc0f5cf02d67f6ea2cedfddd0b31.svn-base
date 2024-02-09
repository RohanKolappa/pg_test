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
//	BFC.DOM.ErrorHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_ErrorHandler_H_
#define _BFC_DOM_ErrorHandler_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.ErrorHandlerPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.DOM.Error.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM ErrorHandler.
///
/// ErrorHandler is a callback interface that the DOM implementation can call
/// when reporting errors that happens while processing XML data, or when doing
/// some other processing (e.g. validating a document). A ErrorHandler object
/// can be attached to a Document using the "error-handler" on the
/// Configuration interface. If more than one error needs to be reported during
/// an operation, the sequence and numbers of the errors passed to the error
/// handler are implementation dependent.
///
/// The application that is using the DOM implementation is expected to
/// implement this interface.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL ErrorHandler : virtual public SObject {

public :

///This method is called on the error handler when an error occurs.
///If an exception is thrown from this method, it is considered to be equivalent of returning true.
///Parameters
///
///error of type DOMError
///    The error object that describes the error. This object may be reused by the DOM implementation across multiple calls to the handleError method.
///
///Return Value
///
///boolean
///	
///
///If the handleError method returns false, the DOM implementation should stop the current processing when possible. If the method returns true, the processing may continue depending on DOMError.severity. 

	virtual Bool handleError(
			ErrorCPtr	error
	) = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_ErrorHandler_H_

// ============================================================================

