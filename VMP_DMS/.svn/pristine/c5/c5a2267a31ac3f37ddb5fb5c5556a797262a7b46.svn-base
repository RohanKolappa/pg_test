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
//	VMP.ConfigInfoSerializer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferSerializer.h"
#include "BFC.Base.UUIdSerializer.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.ConfigInfo.h"
#include "VMP.ConfigInfoRepository.h"
#include "VMP.ConfigInfoSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

void ConfigInfoSerializer::doSerialize(
		IO::ByteOutputStreamPtr
				pOutpStrm,
		ConfigInfoCPtr	pCnfgInfo ) {

	UUId		uuid = pCnfgInfo->getType()->getUUId();
	Buffer		data = pCnfgInfo->encode();

	UUIdSerializer::doSerialize( uuid, pOutpStrm );
	BufferSerializer::doSerialize( data, pOutpStrm );

}

ConfigInfoPtr ConfigInfoSerializer::unSerialize(
		IO::ByteInputStreamPtr
				pInptStrm ) {

	UUId		uuid = UUIdSerializer::unSerialize( pInptStrm );
	Buffer		data = BufferSerializer::unSerialize( pInptStrm );
	ConfigInfoPtr	info = ConfigInfoRepository::instance()->makeObject( uuid );

	info->decode( data );

	return info;

}

// ============================================================================

