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
//	BFC.DOM.Element.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <stdio.h>

#include "BFC.DOM.Element.h"
#include "BFC.DOM.ElementImpl.h"
#include "BFC.DOM.NodeListImpl.h"

// ============================================================================

using namespace BFC;
using namespace BFC::DOM;

// ============================================================================

Element::Element() {

}

Element::Element(
	const	Element &	x ) :

	SObject	(),
	Node	( x ) {

}

Element::Element(
		ElementImplPtr	n ) :

	Node	( n ) {

}

Element & Element::operator = (
	const	Element &	x ) {

	Node::operator = ( x );

	return * this;

}

void Element::setTagName(const Buffer& name)
{
	if (impl)
		( ( ElementImplPtr )impl )->name = name;
}

Buffer Element::tagName() const
{
	if (!impl)
		return Buffer();
	return ( ( ElementImplPtr )impl )->getNodeName();
}


NamedNodeMap Element::getAttributes() const
{
	if (!impl)
		return NamedNodeMap();
	return NamedNodeMap(( ( ElementImplPtr )impl )->getAttributes());
}

Buffer Element::getAttribute( const Buffer& name, const Buffer& defValue ) const
{
	if (!impl)
		return defValue;
	return ( ( ElementImplPtr )impl )->getAttribute( name, defValue );
}

void Element::setAttribute(const Buffer& name, const Buffer& value)
{
	if (!impl)
		return;
	( ( ElementImplPtr )impl )->setAttribute(name, value);
}

void Element::setAttribute(const Buffer& name, Int64 value)
{
	if (!impl)
		return;
	Buffer x;
	x = Buffer(value);
	( ( ElementImplPtr )impl )->setAttribute(name, x);
}

void Element::setAttribute(const Buffer& name, Uint64 value)
{
	if (!impl)
		return;
	Buffer x;
	x = Buffer(value);
	( ( ElementImplPtr )impl )->setAttribute(name, x);
}

void Element::setAttribute(const Buffer& name, Float value)
{
	if (!impl)
		return;
	Buffer x;
	x = Buffer(value);
	( ( ElementImplPtr )impl )->setAttribute(name, x);
}

void Element::setAttribute(const Buffer& name, Double value)
{
	if (!impl)
		return;
	Buffer x;
	char buf[256];
#if defined( PLATFORM_WIN32 )
	int count = ::_snprintf(buf, sizeof(buf), "%.16g", value);
#else
	int count = ::snprintf(buf, sizeof(buf), "%.16g", value);
#endif
	if (count > 0)
		x = Buffer( ( const Uchar * )buf, count);
	else
		x = Buffer(value); // Fallback
	( ( ElementImplPtr )impl )->setAttribute(name, x);
}

void Element::removeAttribute(const Buffer& name)
{
	if (!impl)
		return;
	( ( ElementImplPtr )impl )->removeAttribute(name);
}

Attr Element::getAttributeNode( const Buffer& name )
{
	if (!impl)
		return Attr();
	return Attr(( ( ElementImplPtr )impl )->getAttributeNode( name ));
}

Attr Element::setAttributeNode(const Attr& newAttr)
{
	if (!impl)
		return Attr();
	return Attr(( ( ElementImplPtr )impl )->setAttributeNode(((AttrImplPtr)newAttr.impl)));
}

Attr Element::removeAttributeNode(const Attr& oldAttr)
{
	if (!impl)
		return Attr(); // ### should this return oldAttr?
	return Attr(( ( ElementImplPtr )impl )->removeAttributeNode(((AttrImplPtr)oldAttr.impl)));
}

NodeList Element::elementsByTagName(const Buffer& tagname) const
{
	return NodeList(new NodeListImpl(impl, tagname));
}

Bool Element::hasAttribute(const Buffer& name) const
{
	if (!impl)
		return false;
	return ( ( ElementImplPtr )impl )->hasAttribute(name);
}

Buffer Element::getAttributeNS( const Buffer & nsURI, const Buffer& localName, const Buffer& defValue ) const
{
	if (!impl)
		return defValue;
	return ( ( ElementImplPtr )impl )->getAttributeNS( nsURI, localName, defValue );
}

void Element::setAttributeNS(const Buffer & nsURI, const Buffer& qName, const Buffer& value)
{
	if (!impl)
		return;
	( ( ElementImplPtr )impl )->setAttributeNS(nsURI, qName, value);
}

void Element::setAttributeNS(const Buffer & nsURI, const Buffer& qName, Int64 value)
{
	if (!impl)
		return;
	Buffer x;
	x = Buffer(value);
	( ( ElementImplPtr )impl )->setAttributeNS(nsURI, qName, x);
}

void Element::setAttributeNS(const Buffer & nsURI, const Buffer& qName, Uint64 value)
{
	if (!impl)
		return;
	Buffer x;
	x = Buffer(value);
	( ( ElementImplPtr )impl )->setAttributeNS(nsURI, qName, x);
}

void Element::setAttributeNS(const Buffer & nsURI, const Buffer& qName, Double value)
{
	if (!impl)
		return;
	Buffer x;
	x = Buffer(value);
	( ( ElementImplPtr )impl )->setAttributeNS(nsURI, qName, x);
}

void Element::removeAttributeNS(const Buffer& nsURI, const Buffer& localName)
{
	if (!impl)
		return;
	NodeImplPtr n = ( ( ElementImplPtr )impl )->getAttributeNodeNS( nsURI, localName );
	if (!n)
		return;
	( ( ElementImplPtr )impl )->removeAttribute(n->getNodeName());
}

Attr Element::getAttributeNodeNS( const Buffer& nsURI, const Buffer& localName )
{
	if (!impl)
		return Attr();
	return Attr(( ( ElementImplPtr )impl )->getAttributeNodeNS( nsURI, localName ));
}

Attr Element::setAttributeNodeNS(const Attr& newAttr)
{
	if (!impl)
		return Attr();
	return Attr(( ( ElementImplPtr )impl )->setAttributeNodeNS(((AttrImplPtr)newAttr.impl)));
}

NodeList Element::elementsByTagNameNS(const Buffer& nsURI, const Buffer& localName) const
{
	return NodeList(new NodeListImpl(impl, nsURI, localName));
}

Bool Element::hasAttributeNS(const Buffer& nsURI, const Buffer& localName) const
{
	if (!impl)
		return false;
	return ( ( ElementImplPtr )impl )->hasAttributeNS(nsURI, localName);
}

Buffer Element::text() const
{
	if (!impl)
		return Buffer();
	return ( ( ElementImplPtr )impl )->text();
}

// ============================================================================

