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
//	BFC.DOM.ProcessingInstructionImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.ProcessingInstructionImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, ProcessingInstructionImpl )

// ============================================================================

DOM::ProcessingInstructionImpl::ProcessingInstructionImpl(
	const	DocumentImplPtr	&
				d,
	const	NodeImplPtr &	parent,
	const	Buffer &	target,
	const	Buffer &	data ) :

	NodeImpl( ProcessingInstructionNode, d, parent ) {

	name = target;
	value = data;

}

DOM::ProcessingInstructionImpl::ProcessingInstructionImpl(
	const	ProcessingInstructionImplPtr &
				n,
		Bool		deep ) :

	NodeImpl( ProcessingInstructionNode, n, deep ) {

}

// ============================================================================

DOM::NodeImplPtr DOM::ProcessingInstructionImpl::cloneNode(
		Bool		deep ) {

	return new ProcessingInstructionImpl( this, deep );

}

// ============================================================================

const Buffer & DOM::ProcessingInstructionImpl::getTarget() const {

	return name;

}

const Buffer & DOM::ProcessingInstructionImpl::getData() const {

	return value;

}

void DOM::ProcessingInstructionImpl::setData(
	const	Buffer &	pData ) {

	value = pData;

}

// ============================================================================

void DOM::ProcessingInstructionImpl::save(
		IO::DevicePtr	s,
		int,
		int ) const {

	s->putBytes( "<?" );
	s->putBytes( name );
	s->putBytes( " " );
	s->putBytes( value );
	s->putBytes( "?>\n" );

}

// ============================================================================

