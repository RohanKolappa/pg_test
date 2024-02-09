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
//	BFC.XML.ErrorHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_ErrorHandler_H_
#define _BFC_XML_ErrorHandler_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, ErrorHandler )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief The ErrorHandler class provides an interface to report
///	errors in XML data.
/// 
/// If you want your application to report errors to the user or to
/// perform customized error handling, you should subclass this class.
/// 
/// You can set the error handler with Reader::setErrorHandler().
/// 
/// Errors can be reported using warning(), error() and fatalError(),
/// with the error text being reported with getErrorString().
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL ErrorHandler : virtual public SObject {

public :

	ErrorHandler(
	);

	virtual ~ErrorHandler(
	);

	/// \brief Called by the reader to report a warning.
	///
	/// Warnings are
	/// conditions that are not errors or fatal errors as defined by the
	/// XML 1.0 specification. Details of the warning are stored in \a
	/// exception.

	virtual void warning(
		const	Buffer &	exception
	) = 0;

	/// \brief Called by the reader to report a recoverable error.
	///
	/// A recoverable error corresponds to the definiton of "error" in
	/// section 1.2 of the XML 1.0 specification. Details of the error are
	/// stored in \a exception.
	/// 
	/// The reader must continue to provide normal parsing events after
	/// invoking this function.

	virtual void error(
		const	Buffer &	exception
	) = 0;

	/// \brief Called by the reader to report a non-recoverable error.
	///
	/// Details of the error are stored in \a exception.

	virtual void fatalError(
		const	Buffer &	exception
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	ErrorHandler(
		const	ErrorHandler&
	);

	ErrorHandler& operator = (
		const	ErrorHandler&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_ErrorHandler_H_

// ============================================================================

