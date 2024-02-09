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
//	BFC.DOM.DocumentImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.XML.SimpleReader.h"

#include "BFC.DOM.AttrImpl.h"
#include "BFC.DOM.CDATASectionImpl.h"
#include "BFC.DOM.CommentImpl.h"
#include "BFC.DOM.DocumentFragmentImpl.h"
#include "BFC.DOM.DocumentImpl.h"
#include "BFC.DOM.DocumentTypeImpl.h"
#include "BFC.DOM.ElementImpl.h"
#include "BFC.DOM.EntityImpl.h"
#include "BFC.DOM.EntityReferenceImpl.h"
#include "BFC.DOM.Handler.h"
#include "BFC.DOM.ImplementationImpl.h"
#include "BFC.DOM.NotationImpl.h"
#include "BFC.DOM.ProcessingInstructionImpl.h"
#include "BFC.DOM.TextImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, DocumentImpl )

// ============================================================================

DOM::DocumentImpl::DocumentImpl() :

	NodeImpl( DocumentNode, 0 ),
	impl	( 0, true, true ),
	type	( 0, true, true ) {

	impl = new ImplementationImpl;
	type = new DocumentTypeImpl(
			DocumentImplPtr( this, false ),
			NodeImplPtr( this, false ) );

	name = "#document";

}

DOM::DocumentImpl::DocumentImpl(
	const	Buffer &	aname ) :

	NodeImpl( DocumentNode, 0 ),
	impl	( 0, true, true ),
	type	( 0, true, true ) {

	impl = new ImplementationImpl;
	type = new DocumentTypeImpl(
			DocumentImplPtr( this, false ),
			NodeImplPtr( this, false ) );
	type->name = aname;

	name = "#document";

}

DOM::DocumentImpl::DocumentImpl(
		DocumentTypeImplPtr	dt ) :

	NodeImpl( DocumentNode, 0 ),
	impl	( 0, true, true ),
	type	( 0, true, true ) {

	impl = new ImplementationImpl;

	if (dt) {
		type = dt;
	}
	else {
		type = new DocumentTypeImpl(
			DocumentImplPtr( this, false ),
			NodeImplPtr( this, false ) );
	}

	name = "#document";

}

DOM::DocumentImpl::DocumentImpl(
		DocumentImplPtr	n,
		Bool		deep) :

	NodeImpl( DocumentNode, n, deep ),
	impl	( 0, true, true ),
	type	( 0, true, true ) {

	impl = n->impl->clone();
	type = (DocumentTypeImplPtr)n->type->cloneNode();
	type->setParent( DocumentImplPtr( this, false ) );

}

// ============================================================================

void DOM::DocumentImpl::clear() {

	impl.kill();
	type.kill();
	NodeImpl::clear();

}

// ============================================================================

void DOM::DocumentImpl::setContent(
		XML::InputSourcePtr	source,
		Bool		namespaceProcessing ) {

	XML::SimpleReader reader;

	reader.setFeature(
		"http://xml.org/sax/features/namespaces",
		namespaceProcessing );
	reader.setFeature(
		"http://xml.org/sax/features/namespace-prefixes",
		! namespaceProcessing );
	reader.setFeature(
		"http://trolltech.com/xml/features/report-whitespace-only-CharData",
		false );

	setContent( source, & reader );

}

void DOM::DocumentImpl::setContent(
		XML::InputSourcePtr	source,
		XML::Reader *		reader ) {

	clear();

	impl = new ImplementationImpl;
	type = new DocumentTypeImpl(this, this);

	Bool namespaceProcessing = reader->getFeature("http://xml.org/sax/features/namespaces")
		&& !reader->getFeature("http://xml.org/sax/features/namespace-prefixes");

	HandlerImpl hnd(this, namespaceProcessing);
	reader->setContentHandler(&hnd);
	reader->setErrorHandler(&hnd);
	reader->setLexicalHandler(&hnd);
	reader->setDeclHandler(&hnd);
	reader->setDTDHandler(&hnd);

	reader->parse( source );

}

// ============================================================================

DOM::NodeImplPtr DOM::DocumentImpl::cloneNode(
		Bool		deep ) {

	return new DocumentImpl( this, deep );

}

// ============================================================================

DOM::ElementImplPtr DOM::DocumentImpl::getDocumentElement() const {

	NodeImplPtr p = first;

	while (p && !p->isElement())
		p = p->next;

	return p;

}

// ============================================================================

DOM::ElementImplPtr DOM::DocumentImpl::createElement(
	const	Buffer &	tagName ) {

	Bool	ok;
	Buffer	fixedName = fixedXmlName( tagName, &ok );

	if ( ! ok ) {
		throw InvalidArgument( "Invalid tagname: \"" + tagName + "\"!" );
//		return ElementImplPtr();
	}

	return new ElementImpl( this, 0, fixedName );

}

DOM::ElementImplPtr DOM::DocumentImpl::createElementNS(
	const	Buffer &	nsURI,
	const	Buffer &	qName ) {

	Bool ok;
	Buffer fixedName = fixedXmlName(qName, &ok, true);

	if (!ok)
		return ElementImplPtr();

	return new ElementImpl( this, 0, nsURI, fixedName );

}

DOM::DocumentFragmentImplPtr DOM::DocumentImpl::createDocumentFragment() {

	return new DocumentFragmentImpl(this, (NodeImplPtr)0);

}

DOM::TextImplPtr DOM::DocumentImpl::createTextNode(
	const	Buffer &	data ) {

	Bool ok;
	Buffer fixedData = fixedCharData(data, &ok);

	if (!ok)
		return TextImplPtr();

	return new TextImpl(this, 0, fixedData);

}

DOM::CommentImplPtr DOM::DocumentImpl::createComment(
	const	Buffer &	data ) {

	Bool ok;
	Buffer fixedData = fixedComment(data, &ok);

	if (!ok)
		return CommentImplPtr();

	return new CommentImpl(this, 0, fixedData);

}

DOM::CDATASectionImplPtr DOM::DocumentImpl::createCDATASection(
	const	Buffer &	data ) {

	Bool ok;
	Buffer fixedData = fixedCDataSection(data, &ok);

	if (!ok)
		return CDATASectionImplPtr();

	return new CDATASectionImpl(this, 0, fixedData);

}

DOM::ProcessingInstructionImplPtr DOM::DocumentImpl::createProcessingInstruction(
	const	Buffer &	target,
	const	Buffer &	data ) {

	Bool ok;
	Buffer fixedData = fixedPIData(data, &ok);

	if (!ok)
		return 0;

	// [17] PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
	Buffer fixedTarget = fixedXmlName(target, &ok);

	if (!ok)
		return 0;

	if ( /* ImplementationImpl::invalidDataPolicy == ReturnNullNode
		&& */ fixedTarget.toLower() == "xml" )
		return 0;

	return new ProcessingInstructionImpl(this, 0, fixedTarget, fixedData);

}

DOM::AttrImplPtr DOM::DocumentImpl::createAttribute(
	const	Buffer &	aname ) {

	Bool ok;
	Buffer fixedName = fixedXmlName(aname, &ok);

	if (!ok)
		return AttrImplPtr();

	return new AttrImpl(this, 0, fixedName);

}

DOM::AttrImplPtr DOM::DocumentImpl::createAttributeNS(
	const	Buffer &	nsURI,
	const	Buffer &	qName ) {

	Bool ok;
	Buffer fixedName = fixedXmlName(qName, &ok, true);

	if (!ok)
		return AttrImplPtr();

	return new AttrImpl(this, 0, nsURI, fixedName);

}

DOM::EntityReferenceImplPtr DOM::DocumentImpl::createEntityReference(
	const	Buffer &	aname ) {

	Bool ok;
	Buffer fixedName = fixedXmlName(aname, &ok);

	if (!ok)
		return EntityReferenceImplPtr();

	return new EntityReferenceImpl(this, 0, fixedName);

}

//DOM::ElementImplPtr DOM::DocumentImpl::createElement(
//	const	Buffer &	tagName ) {
//
//	Bool ok;
//	Buffer fixedName = fixedXmlName(tagName, &ok);
//
//	if (!ok)
//		return ElementImplPtr();
//
//	return new ElementImpl( this, 0, fixedName );
//
//}

DOM::EntityImplPtr DOM::DocumentImpl::createEntity(
	const	Buffer &	pEntityName,
	const	Buffer &	pPublicId,
	const	Buffer &	pSystemId,
	const	Buffer &	pNotationName,
	const	Buffer &	pValue ) {

	EntityImplPtr e = new EntityImpl(
		this,
		0,
		fixedXmlName( pEntityName ),
		fixedXmlName( pPublicId ),
		fixedXmlName( pSystemId ),
		fixedXmlName( pNotationName ) );

	e->value = pValue;

	return e;

}

DOM::NotationImplPtr DOM::DocumentImpl::createNotation(
	const	Buffer &	pNotationName,
	const	Buffer &	pPublicId,
	const	Buffer &	pSystemId ) {

	return new NotationImpl(
		this,
		0,
		fixedXmlName( pNotationName ),
		fixedXmlName( pPublicId ),
		fixedXmlName( pSystemId ) );

}

// ============================================================================

DOM::NodeImplPtr DOM::DocumentImpl::importNode(
	const	NodeImplPtr	importedNode,
		Bool		deep ) {

	NodeImplPtr node;

	switch (importedNode->getNodeType()) {
	case AttributeNode:
		node = new AttrImpl((AttrImplPtr)importedNode, true);
		break;
	case DocumentFragmentNode:
		node = new DocumentFragmentImpl((DocumentFragmentImplPtr)importedNode, deep);
		break;
	case ElementNode:
		node = new ElementImpl((ElementImplPtr)importedNode, deep);
		break;
	case EntityNode:
		node = new EntityImpl((EntityImplPtr)importedNode, deep);
		break;
	case EntityReferenceNode:
		node = new EntityReferenceImpl((EntityReferenceImplPtr)importedNode, false);
		break;
	case NotationNode:
		node = new NotationImpl((NotationImplPtr)importedNode, deep);
		break;
	case ProcessingInstructionNode:
		node = new ProcessingInstructionImpl((ProcessingInstructionImplPtr)importedNode, deep);
		break;
	case TextNode:
		node = new TextImpl((TextImplPtr)importedNode, deep);
		break;
	case CDATASectionNode:
		node = new CDATASectionImpl((CDATASectionImplPtr)importedNode, deep);
		break;
	case CommentNode:
		node = new CommentImpl((CommentImplPtr)importedNode, deep);
		break;
	default:
		break;
	}

	if (node) {
		node->setOwnerDocument(this);
	}

	return node;

}

// ============================================================================

void DOM::DocumentImpl::save(
		IO::DevicePtr	s,
		int,
		int		indent ) const {

	Bool doc = false;

	NodeImplPtr n = first;

	while (n) {
		if (!doc && !(n->isProcessingInstruction()&&n->getNodeName()=="xml")) {
			// save doctype after XML declaration
			type->save(s, 0, indent);
			doc = true;
		}
		n->save(s, 0, indent);
		n = n->next;
	}

}

// ============================================================================

