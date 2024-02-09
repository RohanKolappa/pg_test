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
//	BFC.DOM.DocumentType.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.DocumentType.h"
#include "BFC.DOM.DocumentTypeImpl.h"

// ============================================================================

using namespace BFC;
using namespace BFC::DOM;

// ============================================================================

DocumentType::DocumentType() : Node()
{
}

DocumentType::DocumentType(const DocumentType& n)
	: SObject(), Node(n)
{
}

DocumentType::DocumentType(DocumentTypeImplPtr n)
	: Node(n)
{
}

DocumentType& DocumentType::operator= (const DocumentType& n)
{
	Node::operator = ( n );
	return * this;
}

Buffer DocumentType::name() const
{
	if (!impl)
		return Buffer();
	return ( ( DocumentTypeImplPtr )impl )->getNodeName();
}

void DocumentType::setName(
	const	Buffer &	pName ) {

	if ( impl ) {
		DocumentTypeImplPtr( impl )->setNodeName( pName );
	}

}

NamedNodeMap DocumentType::entities() const
{
	if (!impl)
		return NamedNodeMap();
	return NamedNodeMap(( ( DocumentTypeImplPtr )impl )->entities);
}

NamedNodeMap DocumentType::notations() const
{
	if (!impl)
		return NamedNodeMap();
	return NamedNodeMap(( ( DocumentTypeImplPtr )impl )->notations);
}

Buffer DocumentType::publicId() const
{
	if (!impl)
		return Buffer();
	return ( ( DocumentTypeImplPtr )impl )->publicId;
}

void DocumentType::setPublicId(
	const	Buffer &	pPublicId ) {

	if ( impl ) {
		DocumentTypeImplPtr( impl )->publicId = pPublicId;
	}

}

Buffer DocumentType::systemId() const
{
	if (!impl)
		return Buffer();
	return ( ( DocumentTypeImplPtr )impl )->systemId;
}

void DocumentType::setSystemId(
	const	Buffer &	pSystemId ) {

	if ( impl ) {
		DocumentTypeImplPtr( impl )->systemId = pSystemId;
	}

}

Buffer DocumentType::internalSubset() const
{
	if (!impl)
		return Buffer();
	return ( ( DocumentTypeImplPtr )impl )->internalSubset;
}

// ============================================================================

