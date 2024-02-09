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
//	BFC.DOM.Entity.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.Entity.h"
#include "BFC.DOM.EntityImpl.h"

// ============================================================================

using namespace BFC;
using namespace BFC::DOM;

// ============================================================================

Entity::Entity()
	: Node()
{
}


Entity::Entity(const Entity& x)
	: SObject(), Node(x)
{
}

Entity::Entity(EntityImplPtr n)
	: Node(n)
{
}

Entity& Entity::operator= (const Entity& x)
{
	return (Entity&) Node::operator=(x);
}

Buffer Entity::publicId() const
{
	if (!impl)
		return Buffer();
	return ( ( EntityImplPtr )impl )->getPublicId();
}

Buffer Entity::systemId() const
{
	if (!impl)
		return Buffer();
	return ( ( EntityImplPtr )impl )->getSystemId();
}

Buffer Entity::notationName() const
{
	if (!impl)
		return Buffer();
	return ( ( EntityImplPtr )impl )->getNotationName();
}

// ============================================================================

