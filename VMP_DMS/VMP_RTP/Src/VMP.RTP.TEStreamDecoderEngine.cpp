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
//	VMP.RTP.TEStreamDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.RTP.TEStreamDecoderEngine.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TEStreamDecoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TEStreamDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "23327fe7-23bf-4b5a-8787-6aa3d35b6415" ),
		"VMP.RTP.TEStreamDecoderEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Decoder (Push)",
			"Devices",
			"RTP Decoder (Push)" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TEStreamDecoderEngine::TEStreamDecoderEngine(
	const	DL::TypeCPtr &	pClssType ) :

	L2::ProxyPushEngine	( pClssType ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );

}

// ============================================================================

