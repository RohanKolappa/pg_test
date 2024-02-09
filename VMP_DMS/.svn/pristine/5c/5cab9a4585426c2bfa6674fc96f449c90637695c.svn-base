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
//	BFC.DOM.Document.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.IO.MemoryOutputStream.h"

#include "BFC.DOM.Document.h"
#include "BFC.DOM.DocumentImpl.h"
#include "BFC.DOM.Entity.h"
#include "BFC.DOM.NodeListImpl.h"
#include "BFC.DOM.Notation.h"

// ============================================================================

using namespace BFC;

// ============================================================================

DOM::Document::Document() :

	docImpl	( 0, true, true ) {

}

DOM::Document::Document(
	const	Buffer &	name ) :

	docImpl( new DocumentImpl( name ), true, true ) {

	impl = docImpl;

}

DOM::Document::Document(
	const	DocumentType	& doctype ) :

	docImpl( new DocumentImpl( ( DocumentTypeImplPtr )( doctype.impl ) ), true, true ) {

	impl = docImpl;

}

DOM::Document::Document(
	const	Document	& x ) :

	SObject	(),
	Node	( x ),
	docImpl	( x.docImpl, true, true ) {

}

DOM::Document::Document(
	const	DocumentImplPtr &
				x ) :

	Node	( x ),
	docImpl	( x, true, true ) {

}

// ============================================================================

DOM::Document & DOM::Document::operator = (
	const	Document	& x ) {

	Node::operator = ( x );

	docImpl = x.docImpl;

	return * this;

}

// ============================================================================

DOM::DocumentType DOM::Document::getDocType() const {

	if (!impl)
		return DocumentType();

	return DocumentType( docImpl->getDocType());

}

DOM::ImplementationCPtr DOM::Document::getImplementation() const {

	if ( ! impl ) {
		return 0;
	}

	return docImpl->getImplementation();

}

DOM::Element DOM::Document::getDocumentElement() const {

	if (!impl)
		return Element();

	return Element(docImpl->getDocumentElement());

}

DOM::NodeType DOM::Document::getNodeType() const {

	return DocumentNode;

}

// ============================================================================

DOM::Element DOM::Document::createElement(
	const	Buffer &	tagName ) {

	return Element( getDocImpl()->createElement( tagName ) );

}

DOM::DocumentFragment DOM::Document::createDocumentFragment()
{
	return DocumentFragment(getDocImpl()->createDocumentFragment());
}

DOM::Text DOM::Document::createTextNode(const Buffer& value)
{
	return Text(getDocImpl()->createTextNode(value));
}

DOM::Comment DOM::Document::createComment(const Buffer& value)
{
	return Comment(getDocImpl()->createComment(value));
}

DOM::CDATASection DOM::Document::createCDATASection(const Buffer& value)
{
	return CDATASection(getDocImpl()->createCDATASection(value));
}

DOM::ProcessingInstruction DOM::Document::createProcessingInstruction(const Buffer& target,
																	 const Buffer& data)
{
	return ProcessingInstruction(getDocImpl()->createProcessingInstruction(target, data));
}

DOM::Attr DOM::Document::createAttribute(const Buffer& name)
{
	return Attr(getDocImpl()->createAttribute(name));
}

DOM::EntityReference DOM::Document::createEntityReference(const Buffer& name)
{
	return EntityReference(getDocImpl()->createEntityReference(name));
}

DOM::Entity DOM::Document::createEntity(
	const	Buffer &	pEntityName,
	const	Buffer &	pPublicId,
	const	Buffer &	pSystemId,
	const	Buffer &	pNotationName,
	const	Buffer &	pValue ) {

	return Entity(
		getDocImpl()->createEntity(
			pEntityName,
			pPublicId,
			pSystemId,
			pNotationName,
			pValue ) );

}

DOM::Notation DOM::Document::createNotation(
	const	Buffer &	pNotationName,
	const	Buffer &	pPublicId,
	const	Buffer &	pSystemId ) {

	return Notation(
		getDocImpl()->createNotation(
			pNotationName,
			pPublicId,
			pSystemId ) );

}

DOM::NodeList DOM::Document::elementsByTagName(const Buffer& tagname) const
{
	return NodeList(new NodeListImpl(impl, tagname));
}

DOM::Node DOM::Document::importNode(const Node& importedNode, Bool deep)
{
	return Node(getDocImpl()->importNode(importedNode.impl, deep));
}

DOM::Element DOM::Document::createElementNS(const Buffer& nsURI, const Buffer& qName)
{
	return Element(getDocImpl()->createElementNS(nsURI, qName));
}

DOM::Attr DOM::Document::createAttributeNS(const Buffer& nsURI, const Buffer& qName)
{
	return Attr(getDocImpl()->createAttributeNS(nsURI, qName));
}

DOM::NodeList DOM::Document::elementsByTagNameNS(const Buffer& nsURI, const Buffer& localName)
{
	return NodeList(new NodeListImpl(impl, nsURI, localName));
}

DOM::Element DOM::Document::elementById(const Buffer& /*elementId*/)
{
	return Element();
}

// ============================================================================

void DOM::Document::setContent(
	const	Buffer		& text,
		Bool		namespaceProcessing ) {

	getDocImpl()->setContent(
		new XML::InputSource( text ),
		namespaceProcessing );

}

Buffer DOM::Document::toString(
	const	Uint32		indent ) const {

	IO::MemoryOutputStreamPtr s = new IO::MemoryOutputStream;

	s->open();

	save( s, indent);

	return s->getContent();

}

// ============================================================================

DOM::DocumentImplPtr DOM::Document::getDocImpl() {

	if ( ! docImpl ) {
		impl = docImpl = new DocumentImpl;
	}

	return docImpl;

}

// ============================================================================

