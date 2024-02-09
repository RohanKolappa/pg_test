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
//	BFC.DOM.Exception.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.DOM.Exception.h"

// ============================================================================

using namespace BFC;

// ============================================================================

DOM::Exception::Exception(
	const	Code		pCode ) :

	BFC::Exception	( "<global>", "Exception", 0, msg[ pCode < MaxCode ? pCode : 0 ] ),

	code		( pCode ) {

}

// ============================================================================

const Char * DOM::Exception::msg[] = {

	"<invalid exception code>",
	"INDEX_SIZE_ERR",
	"DOMSTRING_SIZE_ERR",
	"HIERARCHY_REQUEST_ERR",
	"WRONG_DOCUMENT_ERR",
	"INVALID_CHARACTER_ERR",
	"NO_DATA_ALLOWED_ERR",
	"NO_MODIFICATION_ALLOWED_ERR",
	"NOT_FOUND_ERR",
	"NOT_SUPPORTED_ERR",
	"INUSE_ATTRIBUTE_ERR",
	"INVALID_STATE_ERR",
	"SYNTAX_ERR",
	"INVALID_MODIFICATION_ERR",
	"NAMESPACE_ERR",
	"INVALID_ACCESS_ERR",
	"VALIDATION_ERR",
	"TYPE_MISMATCH_ERR"

};

// ============================================================================

