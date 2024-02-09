// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.DescriptorDecoder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.M2S.Descriptor.h"
#include "VMP.M2S.DescriptorDecoder.h"
#include "VMP.M2S.DescriptorRepository.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, DescriptorDecoder )

// ============================================================================

M2S::DescriptorDecoder::DescriptorDecoder(
		DescriptorRepositoryPtr
				pDescRepo ) :

	descRepo	( pDescRepo ) {

	setObjectName( "VMP.M2S.DescriptorDecoder" );

}

// ============================================================================

M2S::DescriptorArray M2S::DescriptorDecoder::decode(
	const	Buffer &	pDescData ) {

//	msgDbg( "decode(): "
//		+ Utils::toHex( pDescData ) );

	DescriptorArray res;

	const Uchar *	ptr = pDescData.getCstPtr();
	Uint32		len = pDescData.getLength();
	Uint32		pos = 0;

	while ( len ) {

		if ( len < 2 ) {
			throw InternalError( "Can't read descriptor header!" );
		}

		Uint32	descrTag = ptr[ 0 ];
		Uint32	descrLen = ptr[ 1 ];

		if ( len < 2 + descrLen ) {
			throw InternalError( "Can't read descriptor data!" );
		}

		DescriptorPtr d = descRepo->makeDescriptor( descrTag );

		d->decode( pDescData( pos + 2, descrLen ) );

		res += d;

		descrLen += 2;

		ptr += descrLen;
		len -= descrLen;
		pos += descrLen;

	}

	return res;

}

// ============================================================================

