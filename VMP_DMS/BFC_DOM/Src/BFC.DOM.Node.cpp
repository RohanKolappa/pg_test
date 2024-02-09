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
//	BFC.DOM.Node.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.Document.h"
#include "BFC.DOM.ElementImpl.h"
#include "BFC.DOM.Entity.h"
#include "BFC.DOM.Implementation.h"
#include "BFC.DOM.NamedNodeMap.h"
#include "BFC.DOM.Node.h"
#include "BFC.DOM.NodeImpl.h"
#include "BFC.DOM.NodeList.h"
#include "BFC.DOM.NodeListImpl.h"
#include "BFC.DOM.Notation.h"

// ============================================================================

using namespace BFC;

// ============================================================================

DOM::Node::Node() :

//	impl	( 0, false, true ) {
	impl	( 0, true, true ) {

}

DOM::Node::Node(
	const	Node &		n ) :

	SObject	(),
//	impl	( 0, false, true ) {
	impl	( 0, true, true ) {

	impl = n.impl;

}


DOM::Node::Node(
		NodeImplPtr	n ) :

//	impl	( 0, false, true ) {
	impl	( 0, true, true ) {

	impl = n;
//	impl.setMaster( true );

}

// ============================================================================

DOM::Node & DOM::Node::operator = (
	const	Node &		n ) {

	impl = n.impl;

	return * this;

}

// ============================================================================

Bool DOM::Node::operator == (
	const	Node &		n ) const {

	return ( impl == n.impl );

}

Bool DOM::Node::operator != (
	const	Node &		n ) const {

	return ( impl != n.impl );

}

// ============================================================================

Buffer DOM::Node::getNodeName() const
{
	if (!impl)
		return Buffer();

	if (!impl->prefix.isEmpty())
		return impl->prefix + ":" + impl->name;
	return impl->name;
}

Buffer DOM::Node::getNodeValue() const
{
	if (!impl)
		return Buffer();
	return impl->value;
}

void DOM::Node::setNodeValue(const Buffer& v)
{
	if (!impl)
		return;
	impl->setNodeValue(v);
}

DOM::NodeType DOM::Node::getNodeType() const
{
	if (!impl)
		return BaseNode;
	return impl->getNodeType();
}

DOM::Node DOM::Node::parentNode() const
{
	if (!impl)
		return Node();
	return Node(impl->getParent());
}

DOM::NodeList DOM::Node::childNodes() const
{
	if (!impl)
		return NodeList();
	return NodeList(new NodeListImpl(impl));
}

DOM::Node DOM::Node::firstChild() const
{
	if (!impl)
		return Node();
	return Node(impl->first);
}

DOM::Node DOM::Node::lastChild() const
{
	if (!impl)
		return Node();
	return Node(impl->last);
}

DOM::Node DOM::Node::previousSibling() const
{
	if (!impl)
		return Node();
	return Node(impl->prev);
}

DOM::Node DOM::Node::nextSibling() const
{
	if (!impl)
		return Node();
	return Node(impl->next);
}


DOM::NamedNodeMap DOM::Node::getAttributes() const
{
	if (!impl || !impl->isElement())
		return NamedNodeMap();

	return NamedNodeMap( ( ( ElementImplPtr )impl )->getAttributes());
}

DOM::Document DOM::Node::getOwnerDocument() const
{
	if (!impl)
		return Document();
	return Document( impl->getOwnerDocument() );
}

DOM::Node DOM::Node::cloneNode(Bool deep) const
{
	if (!impl)
		return Node();
	return Node(impl->cloneNode(deep));
}

void DOM::Node::normalize()
{
	if (!impl)
		return;
	impl->normalize();
}

Bool DOM::Node::isSupported(
	const	Buffer &	/*feature*/,
	const	Buffer &	/*version*/ ) const {

	throw NotImplemented();

//	Implementation i;
//	return i.hasFeature(feature, version);

}

Buffer DOM::Node::namespaceURI() const
{
	if (!impl)
		return Buffer();
	return impl->nsURI;
}

Buffer DOM::Node::prefix() const
{
	if (!impl)
		return Buffer();
	return impl->prefix;
}

void DOM::Node::setPrefix(const Buffer& pre)
{
	if (!impl || impl->prefix.isEmpty())
		return;
	if (isAttr() || isElement())
		impl->prefix = pre;
}

Buffer DOM::Node::localName() const {

	if (!impl || impl->createdWithDom1Interface)
		return Buffer();
	return impl->name;

}

Bool DOM::Node::hasAttributes() const
{
	if (!impl || !impl->isElement())
		return false;
	return static_cast<ElementImplPtr >(impl)->hasAttributes();
}

DOM::Node DOM::Node::insertBefore(const Node& newChild, const Node& refChild)
{
	if (!impl)
		return Node();
	return Node(impl->insertBefore(newChild.impl, refChild.impl));
}

DOM::Node DOM::Node::insertAfter(const Node& newChild, const Node& refChild)
{
	if (!impl)
		return Node();
	return Node(impl->insertAfter(newChild.impl, refChild.impl));
}

DOM::Node DOM::Node::replaceChild(const Node& newChild, const Node& oldChild)
{
	if (!impl || !newChild.impl || !oldChild.impl)
		return Node();
	return Node(impl->replaceChild(newChild.impl, oldChild.impl));
}

DOM::Node DOM::Node::removeChild(const Node& oldChild)
{
	if (!impl)
		return Node();

	if (oldChild.isEmpty())
		return Node();

	return Node(impl->removeChild(oldChild.impl));
}

DOM::Node DOM::Node::appendChild(const Node& newChild)
{
	if (!impl)
		return Node();
	return Node(impl->appendChild(newChild.impl));
}

Bool DOM::Node::hasChildNodes() const
{
	if (!impl)
		return false;
	return impl->first;
}

Bool DOM::Node::isEmpty() const {

	return ( ! impl );

}

void DOM::Node::clear()
{
	impl.kill();
}

DOM::Node DOM::Node::getNamedItem( const Buffer& name ) const
{
	if (!impl)
		return Node();
	return Node(impl->getNamedItem( name ));
}

void DOM::Node::save(IO::DevicePtr str, int indent) const
{
	if (impl)
		impl->save(str, 1, indent);
}

//IO::Device & operator<<(IO::Device & str, const DOM::Node& node)
//{
//	node.save(&str, 1);
//
//	return str;
//}

// ============================================================================

Bool DOM::Node::isAttr() const
{
	if(impl)
		return impl->isAttr();
	return false;
}

Bool DOM::Node::isCDATASection() const
{
	if(impl)
		return impl->isCDATASection();
	return false;
}

Bool DOM::Node::isDocumentFragment() const
{
	if(impl)
		return impl->isDocumentFragment();
	return false;
}

Bool DOM::Node::isDocument() const
{
	if(impl)
		return impl->isDocument();
	return false;
}

Bool DOM::Node::isDocumentType() const
{
	if(impl)
		return impl->isDocumentType();
	return false;
}

Bool DOM::Node::isElement() const
{
	if(impl)
		return impl->isElement();
	return false;
}

Bool DOM::Node::isEntityReference() const
{
	if(impl)
		return impl->isEntityReference();
	return false;
}

Bool DOM::Node::isText() const
{
	if(impl)
		return impl->isText();
	return false;
}

Bool DOM::Node::isEntity() const
{
	if(impl)
		return impl->isEntity();
	return false;
}

Bool DOM::Node::isNotation() const
{
	if(impl)
		return impl->isNotation();
	return false;
}

Bool DOM::Node::isProcessingInstruction() const
{
	if(impl)
		return impl->isProcessingInstruction();
	return false;
}

Bool DOM::Node::isCharacterData() const
{
	if (impl)
		return impl->isCharacterData();
	return false;
}

Bool DOM::Node::isComment() const
{
	if (impl)
		return impl->isComment();
	return false;
}

// ============================================================================

DOM::Element DOM::Node::firstChildElement(const Buffer &tagName) const
{
	for (Node child = firstChild(); !child.isEmpty(); child = child.nextSibling()) {
		if (child.isElement()) {
			Element elt = child.toElement();
			if (tagName.isEmpty() || elt.tagName() == tagName)
				return elt;
		}
	}
	return Element();
}

DOM::Element DOM::Node::lastChildElement(const Buffer &tagName) const
{
	for (Node child = lastChild(); !child.isEmpty(); child = child.previousSibling()) {
		if (child.isElement()) {
			Element elt = child.toElement();
			if (tagName.isEmpty() || elt.tagName() == tagName)
				return elt;
		}
	}
	return Element();
}

DOM::Element DOM::Node::nextSiblingElement(const Buffer &tagName) const
{
	for (Node sib = nextSibling(); !sib.isEmpty(); sib = sib.nextSibling()) {
		if (sib.isElement()) {
			Element elt = sib.toElement();
			if (tagName.isEmpty() || elt.tagName() == tagName)
				return elt;
		}
	}
	return Element();
}

DOM::Element DOM::Node::previousSiblingElement(const Buffer &tagName) const
{
	for (Node sib = previousSibling(); !sib.isEmpty(); sib = sib.previousSibling()) {
		if (sib.isElement()) {
			Element elt = sib.toElement();
			if (tagName.isEmpty() || elt.tagName() == tagName)
				return elt;
		}
	}
	return Element();
}

// ============================================================================

DOM::Attr DOM::Node::toAttr() const
{
	if (impl && impl->isAttr())
		return Attr(((AttrImplPtr)impl));
	return Attr();
}

DOM::CDATASection DOM::Node::toCDATASection() const
{
	if (impl && impl->isCDATASection())
		return CDATASection(((CDATASectionImplPtr)impl));
	return CDATASection();
}

DOM::DocumentFragment DOM::Node::toDocumentFragment() const
{
	if (impl && impl->isDocumentFragment())
		return DocumentFragment(((DocumentFragmentImplPtr)impl));
	return DocumentFragment();
}

DOM::Document DOM::Node::toDocument() const
{
	if (impl && impl->isDocument())
		return Document(((DocumentImplPtr)impl));
	return Document();
}

DOM::DocumentType DOM::Node::toDocumentType() const
{
	if (impl && impl->isDocumentType())
		return DocumentType(((DocumentTypeImplPtr)impl));
	return DocumentType();
}

DOM::Element DOM::Node::toElement() const
{
	if (impl && impl->isElement())
		return Element(((ElementImplPtr)impl));
	return Element();
}

DOM::EntityReference DOM::Node::toEntityReference() const
{
	if (impl && impl->isEntityReference())
		return EntityReference(((EntityReferenceImplPtr)impl));
	return EntityReference();
}

DOM::Text DOM::Node::toText() const
{
	if (impl && impl->isText())
		return Text(((TextImplPtr)impl));
	return Text();
}

DOM::Entity DOM::Node::toEntity() const
{
	if (impl && impl->isEntity())
		return Entity(((EntityImplPtr)impl));
	return Entity();
}

DOM::Notation DOM::Node::toNotation() const
{
	if (impl && impl->isNotation())
		return Notation(((NotationImplPtr)impl));
	return Notation();
}

DOM::ProcessingInstruction DOM::Node::toProcessingInstruction() const
{
	if (impl && impl->isProcessingInstruction())
		return ProcessingInstruction(((ProcessingInstructionImplPtr)impl));
	return ProcessingInstruction();
}

DOM::CharacterData DOM::Node::toCharacterData() const
{
	if (impl && impl->isCharacterData())
		return CharacterData(((CharacterDataImplPtr)impl));
	return CharacterData();
}

DOM::Comment DOM::Node::toComment() const
{
	if (impl && impl->isComment())
		return Comment(((CommentImplPtr)impl));
	return Comment();
}

// ============================================================================

DOM::Node DOM::Node::getParent() const {

	if ( ! impl ) {
		return Node();
	}

	return Node( impl->getParent() );

}

// ============================================================================


