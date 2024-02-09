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
//	BFC.DOM.ElementImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.AttrImpl.h"
#include "BFC.DOM.ElementImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, ElementImpl )

// ============================================================================

DOM::ElementImpl::ElementImpl(
		DocumentImplPtr	d,
		NodeImplPtr	p,
	const	Buffer &	tagname ) :

	NodeImpl( ElementNode, d, p ),
	m_attr	( 0, true, true ) {

	name = tagname;
	m_attr = new NamedNodeMapImpl( ElementImplPtr( this, false ) );

}

DOM::ElementImpl::ElementImpl(
		DocumentImplPtr	d,
		NodeImplPtr	p,
	const	Buffer &	pNsUri,
	const	Buffer &	qName ) :

	NodeImpl( ElementNode, d, p ),
	m_attr	( 0, true, true ) {

	splitNamespace(prefix, name, qName, !pNsUri.isEmpty());
	nsURI = pNsUri;
	createdWithDom1Interface = false;
	m_attr = new NamedNodeMapImpl( ElementImplPtr( this, false ) );

}

DOM::ElementImpl::ElementImpl(
		ElementImplPtr	n,
		Bool		deep ) :

	NodeImpl( ElementNode, n, deep ),
	m_attr	( 0, true, true ) {

	m_attr = n->m_attr->clone( ElementImplPtr( this, false ) );

}

// ============================================================================

DOM::NodeImplPtr DOM::ElementImpl::cloneNode(
		Bool		deep ) {

	return new ElementImpl( this, deep );

}

// ============================================================================

Buffer DOM::ElementImpl::getAttribute(
	const	Buffer &	pName,
	const	Buffer &	defValue ) const {

	NodeImplPtr n = m_attr->getNamedItem( pName );

	if (!n)
		return defValue;

	return n->getNodeValue();

}

Buffer DOM::ElementImpl::getAttributeNS(
	const	Buffer &	pNsUri,
	const	Buffer &	localName,
	const	Buffer &	defValue ) const {

	NodeImplPtr n = m_attr->getNamedItemNS( pNsUri, localName );

	if (!n)
		return defValue;

	return n->getNodeValue();

}

void DOM::ElementImpl::setAttribute(
	const	Buffer &	pName,
	const	Buffer &	newValue ) {

	NodeImplPtr n = m_attr->getNamedItem( pName );

	if (!n) {
		n = new AttrImpl(getOwnerDocument(), this, pName);
		n->setNodeValue(newValue);
		m_attr->setNamedItem(n);
	}
	else {
		n->setNodeValue(newValue);
	}

}

void DOM::ElementImpl::setAttributeNS(
	const	Buffer &	pNsUri,
	const	Buffer &	qName,
	const	Buffer &	newValue ) {

	Buffer prefix, localName;

	splitNamespace(prefix, localName, qName, true);

	NodeImplPtr n = m_attr->getNamedItemNS( pNsUri, localName );

	if (!n) {
		n = new AttrImpl(getOwnerDocument(), this, pNsUri, qName);
		n->setNodeValue(newValue);
		m_attr->setNamedItem(n);
	}
	else {
		n->setNodeValue(newValue);
		n->prefix = prefix;
	}

}

void DOM::ElementImpl::removeAttribute(
	const	Buffer &	pName ) {

	m_attr->removeNamedItem(pName);

}

DOM::AttrImplPtr DOM::ElementImpl::getAttributeNode(
	const	Buffer &	pName ) const {

	return (AttrImplPtr)m_attr->getNamedItem( pName );

}

DOM::AttrImplPtr DOM::ElementImpl::getAttributeNodeNS(
	const	Buffer &	pNsUri,
	const	Buffer &	localName ) const {

	return (AttrImplPtr)m_attr->getNamedItemNS( pNsUri, localName );

}

DOM::AttrImplPtr DOM::ElementImpl::setAttributeNode(
		AttrImplPtr	newAttr ) {

	NodeImplPtr n = m_attr->getNamedItem( newAttr->getNodeName() );

	m_attr->setNamedItem(newAttr);

	return n;

}

DOM::AttrImplPtr DOM::ElementImpl::setAttributeNodeNS(
		AttrImplPtr	newAttr ) {

	NodeImplPtr n = 0;

	if (!newAttr->prefix.isEmpty())
		n = m_attr->getNamedItemNS( newAttr->nsURI, newAttr->name );

	m_attr->setNamedItem(newAttr);

	return n;

}

DOM::AttrImplPtr DOM::ElementImpl::removeAttributeNode(
		AttrImplPtr	oldAttr ) {

	return m_attr->removeNamedItem(oldAttr->getNodeName());

}

Bool DOM::ElementImpl::hasAttribute(
	const	Buffer &	pName ) const {

	return m_attr->contains(pName);

}

Bool DOM::ElementImpl::hasAttributeNS(
	const	Buffer &	pNsUri,
	const	Buffer &	localName ) const {

	return m_attr->containsNS(pNsUri, localName);

}

Buffer DOM::ElementImpl::text() const {

	Buffer t("");

	NodeImplPtr p = first;

	while (p) {
		if (p->isText() || p->isCDATASection())
			t += p->getNodeValue();
		else if (p->isElement())
			t += ((ElementImplPtr)p)->text();
		p = p->next;
	}

	return t;

}

// ============================================================================

void DOM::ElementImpl::save(
		IO::DevicePtr	s,
		int		depth,
		int		indent ) const {

	if (!(prev && prev->isText())) {
		s->putBytes( Buffer::spaces( depth * indent ) );
	}

	Buffer qName(name);
	Buffer nsDecl("");
	if (!nsURI.isEmpty()) {
		// ### optimize this, so that you only declare namespaces that are not
		// yet declared -- we lose default namespace mappings, so maybe we
		// should rather store the information that we get from
		// startPrefixMapping()/endPrefixMapping() and use them (you have to
		// take care if the DOM tree is modified, though)
		if (prefix.isEmpty()) {
			nsDecl = " xmlns";
		}
		else {
			qName = prefix + ":" + name;
			nsDecl = " xmlns:" + prefix;
		}
		nsDecl += "=\"" + encodeAttr(nsURI) + "\"";
	}
	s->putBytes( "<" );
	s->putBytes( qName );
	s->putBytes( nsDecl );

	if (!m_attr->map.isEmpty()) {
		s->putBytes( " " );
		for ( Uint32 i = 0 ; i < m_attr->map.getSize() ; i++ ) {
			m_attr->map.getValue( i )->save(s, 0, indent);
			s->putBytes( " " );
		}
	}

	if (last) {
		// has child nodes
		if (first->isText()) {
			s->putBytes( ">" );
		}
		else {
			s->putBytes( ">\n" );
		}
		NodeImpl::save(s, depth + 1, indent);
		if ( ! last->isText() ) {
			s->putBytes( Buffer::spaces( depth * indent ) );
		}
		s->putBytes( "</" );
		s->putBytes( qName );
		s->putBytes( ">" );
	}
	else {
		s->putBytes( "/>" );
	}

	if (!(next && next->isText()))
		s->putBytes( "\n" );

}

// ============================================================================

