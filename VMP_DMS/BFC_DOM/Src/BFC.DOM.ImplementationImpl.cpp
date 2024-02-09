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
//	BFC.DOM.ImplementationImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.DocumentTypeImpl.h"
#include "BFC.DOM.ImplementationImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, ImplementationImpl )

// ============================================================================

DOM::ImplementationImpl::ImplementationImpl() {

}

// ============================================================================

DOM::ImplementationPtr DOM::ImplementationImpl::clone() const {

	return new ImplementationImpl;

}

// ============================================================================

Bool DOM::ImplementationImpl::hasFeature(
	const	Buffer &	feature,
	const	Buffer &	version ) const {

	if ( feature == "XML") {
		if ( version.isEmpty() || version == "1.0") {
			return true;
		}
	}

// FIXME: add DOM level 2 features

	return false;

}

DOM::DocumentType DOM::ImplementationImpl::createDocumentType(
	const	Buffer &	qName,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) const {

	Bool ok;
	Buffer fixedName = fixedXmlName(qName, &ok, true);

	if (!ok)
		return DocumentType();

	Buffer fixedPublicId = fixedPubidLiteral(publicId, &ok);
	if (!ok)
		return DocumentType();

	Buffer fixedSystemId = fixedSystemLiteral(systemId, &ok);
	if (!ok)
		return DocumentType();

	DocumentTypeImplPtr dt = new DocumentTypeImpl( 0 );

	dt->name = fixedName;
	if (systemId.isEmpty()) {
		dt->publicId.kill();
		dt->systemId.kill();
	} else {
		dt->publicId = fixedPublicId;
		dt->systemId = fixedSystemId;
	}

	return DocumentType( dt );

}

DOM::Document DOM::ImplementationImpl::createDocument(
	const	Buffer		& nsURI,
	const	Buffer		& qName,
	const	DocumentType	& doctype ) const {

	Document doc(doctype);
	Element root = doc.createElementNS(nsURI, qName);
	if (root.isEmpty())
		return Document();
	doc.appendChild(root);
	return doc;

}

SCPtr DOM::ImplementationImpl::getFeature(
	const	Buffer &	/*pFeature*/,
	const	Buffer &	/*pVersion*/ ) const {

	throw NotImplemented();

}

//Bool DOM::Implementation::isEmpty()
//{
//	return ( ! impl );
//}

// ============================================================================

//InvalidDataPolicy DOM::Implementation::invalidDataPolicy()
//{
//	return ImplementationImpl::invalidDataPolicy;
//}
//
//void DOM::Implementation::setInvalidDataPolicy(InvalidDataPolicy policy)
//{
//	ImplementationImpl::invalidDataPolicy = policy;
//}

// ============================================================================

