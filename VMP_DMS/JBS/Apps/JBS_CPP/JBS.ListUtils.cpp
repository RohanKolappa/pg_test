// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "JBS".
// 
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	JBS.ListUtils.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.Context.h"
#include "JBS.ListUtils.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( ListUtils )

// ============================================================================

ListUtils::ListUtils() {



}

ListUtils::~ListUtils() {



}

// ============================================================================

void ListUtils::keepExtension(
	const	Buffer&		extension,
		FSPathList&	list ) {

	for ( FSPathList::Iterator it = list.firstElt() ; it ; ) {
		if ( it->getLastComponent().endsWith( extension ) ) {
			it++;
		}
		else {
			list.pop( it );
		}
	}

}

// ============================================================================

void ListUtils::appendSubstExt(
		FSPathList&	listToAugment,
	const	Buffer&		initialExtension,
	const	Buffer&		finalExtension,
	const	FSPathList&	listToAdd ) {

	for ( FSPathList::CstIterator it = listToAdd.firstElt() ; it ; it++ ) {
		FSPath toAdd = *it;
		Buffer cmpt = toAdd.getLastComponent();
		if ( cmpt.endsWith( initialExtension ) ) {
			cmpt = cmpt( 0, cmpt.length() - initialExtension.length() )
			     + finalExtension;
			toAdd.replaceLastComponent( cmpt );
		}
		listToAugment.insertLast( toAdd );
	}

}

// ============================================================================

void ListUtils::filterOut(
	const	FSPathList&	toRemove,
		FSPathList&	toFilter ) {

	for ( FSPathList::CstIterator iit = toRemove.firstElt() ; iit ; iit++ ) {
	for ( FSPathList::Iterator oit = toFilter.firstElt() ; oit ; ) {
		if ( *iit == *oit ) {
			toFilter.pop( oit );
		}
		else {
			oit++;
		}
	}
	}

}

// ============================================================================

void ListUtils::join(
		FSPathList&	result,
	const	FSPathList&	in1,
	const	FSPathList&	in2 ) {

	result.kill();
	result.insertLast( in1 );
	result.insertLast( in2 );

}

// ============================================================================

void ListUtils::removeDuplicates(
		FSPathList&	list ) {

	for ( FSPathList::Iterator it = list.firstElt() ; it ; it++ ) {
		FSPathList::Iterator it2 = it;
		it2++;
		while ( it2 ) {
			if ( *it == *it2 ) {
				list.pop( it2 );
			}
			else {
				it2++;
			}
		}
	}

}

// ============================================================================

void ListUtils::dump(
	const	FSPathList&	in ) {

	if ( in.isEmpty() ) {
		return;
	}

	Buffer msg;

	for ( FSPathList::CstIterator it = in.firstElt() ; it ; it++ ) {
		msg += it->toBuffer() + " ";
	}

	Message( Message::MsgInfo, msg );

}

// ============================================================================

