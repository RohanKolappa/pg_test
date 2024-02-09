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
//	BFC.DOM.EntityImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.EntityImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, EntityImpl )

// ============================================================================

DOM::EntityImpl::EntityImpl(
		DocumentImplPtr	d,
		NodeImplPtr	parent,
	const	Buffer &	aname,
	const	Buffer &	pub,
	const	Buffer &	sys,
	const	Buffer &	notation ) :

	NodeImpl( EntityNode, d, parent ) {

	name = aname;
	m_pub = pub;
	m_sys = sys;
	m_notationName = notation;

}

DOM::EntityImpl::EntityImpl(
		EntityImplPtr	n,
		Bool		deep ) :

	NodeImpl( EntityNode, n, deep ) {

	m_sys = n->m_sys;
	m_pub = n->m_pub;
	m_notationName = n->m_notationName;

}

// ============================================================================

DOM::NodeImplPtr DOM::EntityImpl::cloneNode(
		Bool		deep ) {

	return new EntityImpl( this, deep );

}

// ============================================================================

const Buffer & DOM::EntityImpl::getPublicId() const {

	return m_pub;

}

const Buffer & DOM::EntityImpl::getSystemId() const {

	return m_sys;

}

const Buffer & DOM::EntityImpl::getNotationName() const {

	return m_notationName;

}

const Buffer & DOM::EntityImpl::getInputEncoding() const {

	throw NotImplemented();

}

const Buffer & DOM::EntityImpl::getXMLEncoding() const {

	throw NotImplemented();

}

const Buffer & DOM::EntityImpl::getXMLVersion() const {

	throw NotImplemented();

}

// ============================================================================

void DOM::EntityImpl::save(
		IO::DevicePtr	s,
		int,
		int ) const {

	Buffer tmpName = name;

	if ( tmpName.beginsWith( "%" ) ) {
		tmpName = "% " + tmpName( 1 );
	}

	if ( m_sys.isEmpty() && m_pub.isEmpty() ) {
		s->putBytes( "<!ENTITY " );
		s->putBytes( tmpName );
		s->putBytes( " \"" );
		s->putBytes( encodeEntity( value ) );
		s->putBytes( "\">\n" );
	}
	else {
		s->putBytes( "<!ENTITY " );
		s->putBytes( tmpName );
		s->putBytes( " " );
		if ( m_pub.isEmpty() ) {
			s->putBytes( "SYSTEM " );
			s->putBytes( quotedValue( m_sys ) );
		}
		else {
			s->putBytes( "PUBLIC " );
			s->putBytes( quotedValue( m_pub ) );
			s->putBytes( " " );
			s->putBytes( quotedValue( m_sys ) );
		}
		if ( ! m_notationName.isEmpty() ) {
			s->putBytes( " NDATA " );
			s->putBytes( m_notationName );
		}
		s->putBytes( ">\n" );
	}

}

// ============================================================================

