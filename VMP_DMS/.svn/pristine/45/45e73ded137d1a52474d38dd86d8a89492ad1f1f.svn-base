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
//	BFC.DOM.NotationImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.NotationImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, NotationImpl )

// ============================================================================

DOM::NotationImpl::NotationImpl(
	const	DocumentImplPtr	&
				d,
	const	NodeImplPtr &	parent,
	const	Buffer &	aname,
	const	Buffer &	pub,
	const	Buffer &	sys ) :

	NodeImpl( NotationNode, d, parent ) {

	name = aname;
	m_pub = pub;
	m_sys = sys;

}

DOM::NotationImpl::NotationImpl(
	const	NotationImplPtr	&
				n,
		Bool		deep ) :

	NodeImpl( NotationNode, n, deep ) {

	m_sys = n->m_sys;
	m_pub = n->m_pub;

}

// ============================================================================

DOM::NodeImplPtr DOM::NotationImpl::cloneNode(
		Bool		deep ) {

	return new NotationImpl( this, deep );

}

// ============================================================================

const Buffer & DOM::NotationImpl::getPublicId() const {

	return m_pub;

}

const Buffer & DOM::NotationImpl::getSystemId() const {

	return m_sys;

}

// ============================================================================

void DOM::NotationImpl::save(
		IO::DevicePtr	s,
		int,
		int ) const {

	s->putBytes( "<!NOTATION " );
	s->putBytes( name );
	s->putBytes( " " );

	if ( ! m_pub.isEmpty() )  {
		s->putBytes( "PUBLIC " );
		s->putBytes( quotedValue( m_pub ) );
		if ( ! m_sys.isEmpty() ) {
			s->putBytes( " " );
			s->putBytes( quotedValue( m_sys ) );
		}
	}
	else {
		s->putBytes( "SYSTEM " );
		s->putBytes( quotedValue( m_sys ) );
	}

	s->putBytes( ">\n" );

}

// ============================================================================

