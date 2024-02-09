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
//	BFC.DOM.CharacterData.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.CharacterData.h"
#include "BFC.DOM.CharacterDataImpl.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

using namespace BFC;
using namespace BFC::DOM;

// ============================================================================

CharacterData::CharacterData()
{
}

CharacterData::CharacterData(
	const	CharacterData	& x ) :

	SObject(),
	Node( x ) {



}

CharacterData::CharacterData(CharacterDataImplPtr n)
	: Node(n)
{
}

CharacterData& CharacterData::operator= (const CharacterData& x)
{
	Node::operator = ( x );
	return * this;
}

const Buffer & CharacterData::getData() const {

	return ( impl ? impl->getNodeValue() : emptyBuffer );

}

void CharacterData::setData(const Buffer& v)
{
	if (impl)
		impl->setNodeValue(v);
}

Uint32 CharacterData::length() const
{
	if (impl)
		return ( ( CharacterDataImplPtr )impl )->getLength();
	return 0;
}

Buffer CharacterData::substringData(const Uint32 offset, const Uint32 count)
{
	if (!impl)
		return emptyBuffer;
	return ( ( CharacterDataImplPtr )impl )->substringData(offset, count);
}

void CharacterData::appendData(const Buffer& arg)
{
	if (impl)
		( ( CharacterDataImplPtr )impl )->appendData(arg);
}

void CharacterData::insertData(const Uint32 offset, const Buffer& arg)
{
	if (impl)
		( ( CharacterDataImplPtr )impl )->insertData(offset, arg);
}

void CharacterData::deleteData(const Uint32 offset, const Uint32 count)
{
	if (impl)
		( ( CharacterDataImplPtr )impl )->deleteData(offset, count);
}

void CharacterData::replaceData(const Uint32 offset, const Uint32 count, const Buffer& arg)
{
	if (impl)
		( ( CharacterDataImplPtr )impl )->replaceData(offset, count, arg);
}

NodeType CharacterData::getNodeType() const
{
	if (!impl)
		return CharacterDataNode;
	return Node::getNodeType();
}

// ============================================================================

