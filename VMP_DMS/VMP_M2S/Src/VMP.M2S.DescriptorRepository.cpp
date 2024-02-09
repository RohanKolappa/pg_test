// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.DescriptorRepository.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.M2S.Descriptor.h"
#include "VMP.M2S.DescriptorFactory.h"
#include "VMP.M2S.DescriptorRepository.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, DescriptorRepository )

// ============================================================================

M2S::DescriptorRepository::DescriptorRepository() {

	unknFact = new DescriptorFactoryTmpl< UnknownDescriptor >;

	addFactory( 0x02, new DescriptorFactoryTmpl< VideoStreamDescriptor > );
	addFactory( 0x09, new DescriptorFactoryTmpl< CADescriptor > );
	addFactory( 0x0A, new DescriptorFactoryTmpl< ISO639LanguageDescriptor > );
	addFactory( 0x13, new DescriptorFactoryTmpl< CarouselIdentifierDescriptor > );
	addFactory( 0x14, new DescriptorFactoryTmpl< AssociationTagDescriptor > );

	for ( Uint32 i = 0 ; i < 0x100 ; i++ ) {
		usedList[ i ] = false;
	}

}

// ============================================================================

M2S::DescriptorPtr M2S::DescriptorRepository::makeDescriptor(
	const	Uchar		pDescrTag ) {

	usedList[ pDescrTag ] = true;

	DescriptorPtr d = ( factList[ pDescrTag ]
			? factList[ pDescrTag ]->makeDescriptor()
			: unknFact->makeDescriptor() );

	d->tag = pDescrTag;

	return d;

}

// ============================================================================

Buffer M2S::DescriptorRepository::getNotFound() const {

	Buffer msg;

	for ( Uint32 i = 0 ; i < 0x100 ; i++ ) {
		if ( usedList[ i ] && ! factList[ i ] ) {
			if ( ! msg.isEmpty() ) {
				msg += ", ";
			}
			msg += Buffer( i, Buffer::Base16, 2 );
		}
	}

	return msg;

}

// ============================================================================

void M2S::DescriptorRepository::addFactory(
	const	Uchar		pDescrTag,
		DescriptorFactoryPtr
				pDescFact ) {

	factList[ pDescrTag ] = pDescFact;

}

// ============================================================================

