// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.IndexSerializer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.UUIdSerializer.h"

#include "BFC.DL.Type.h"

#include "VMP.IndexFactoryRegistry.h"
#include "VMP.IndexSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

void IndexSerializer::doSerialize(
		BFC::IO::ByteOutputStreamPtr	out,
		VMP::UnitsIndexPtr		data ) {

	// First, send the UUId...

	UUIdSerializer::doSerialize( data->getType()->getUUId(), out );

	data->doSerialize( out );

}

UnitsIndexPtr IndexSerializer::unSerialize(
		BFC::IO::ByteInputStreamPtr	in ) {

	// Read the UUId...

	UUId id = UUIdSerializer::unSerialize( in );

	// Instantiate appropriate factory...

	UnitsIndexPtr res = IndexFactoryRegistry::instance()->getNewIndex( id );

	// Unserialize the frame...

	res->unSerialize( in );

	return res;

}

// ============================================================================

