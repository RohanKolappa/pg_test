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
//	BFC.DOM.Utils.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Utils_H_
#define _BFC_DOM_Utils_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \ingroup BFC_DOM

Buffer encodeAttr(
	const	Buffer &	str,
		Bool		encodeQuotes = true
);

/// \ingroup BFC_DOM

Buffer encodeEntity(
	const	Buffer &	str
);

/// \ingroup BFC_DOM

Buffer quotedValue(
	const	Buffer &	data
);

/// \brief Helper to split a qualified name in the prefix and local name.
///
/// \ingroup BFC_DOM

void splitNamespace(
		Buffer &	prefix,
		Buffer &	name,
	const	Buffer &	qName,
	const	Bool		hasURI
);

// [5] Name ::= (Letter | '_' | ':') (NameChar)*

/// \ingroup BFC_DOM

Buffer fixedXmlName(
	const	Buffer &	name,
		Bool *		ok = 0,
		Bool		namespaces = false
);

// [14] CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
// '<', '&' and "]]>" will be escaped when writing

/// \ingroup BFC_DOM

Buffer fixedCharData(
	const	Buffer &	data,
		Bool *		ok
);

// [15] Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
// can't escape "--", since entities are not recognised within comments

/// \ingroup BFC_DOM

Buffer fixedComment(
	const	Buffer &	data,
		Bool *		ok
);

// [20] CData ::= (Char* - (Char* ']]>' Char*))
// can't escape "]]>", since entities are not recognised within comments

/// \ingroup BFC_DOM

Buffer fixedCDataSection(
	const	Buffer &	data,
		Bool *		ok
);

// [16] PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'

/// \ingroup BFC_DOM

Buffer fixedPIData(
	const	Buffer &	data,
		Bool *		ok
);

// [12] PubidLiteral ::= '"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
// The correct quote will be chosen when writing

/// \ingroup BFC_DOM

Buffer fixedPubidLiteral(
	const	Buffer &	data,
		Bool *		ok
);

// [11] SystemLiteral ::= ('"' [^"]* '"') | ("'" [^']* "'")
// The correct quote will be chosen when writing

/// \ingroup BFC_DOM

Buffer fixedSystemLiteral(
	const	Buffer &	data,
		Bool *		ok
);

// ============================================================================

extern Int64 nodeListTime;

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Utils_H_

// ============================================================================

