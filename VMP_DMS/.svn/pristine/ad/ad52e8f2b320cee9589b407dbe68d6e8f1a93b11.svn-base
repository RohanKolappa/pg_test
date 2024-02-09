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
//	BFC.DOM.CDATASection.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.CDATASection.h"

// ============================================================================

using namespace BFC::DOM;

// ============================================================================

CDATASection::CDATASection() {



}

CDATASection::CDATASection(
	const	CDATASection	& x ) :

	SObject(),
	Text( x ) {



}

CDATASection::CDATASection(
		CDATASectionImplPtr	n ) :

	Text( n ) {



}

// ============================================================================

CDATASection& CDATASection::operator = (
	const	CDATASection	& x ) {

	Text::operator = ( x );

	return * this;

}

// ============================================================================

NodeType CDATASection::getNodeType() const {

	return CDATASectionNode;

}

// ============================================================================

