// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.FrameSerializer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.UUIdSerializer.h"

#include "VMP.FrameFactoryRegistry.h"
#include "VMP.FrameSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

void FrameSerializer::doSerialize(
		BFC::IO::ByteOutputStreamPtr	out,
		VMP::FramePtr			data ) {

	// First, send the UUId...

	UUIdSerializer::doSerialize( data->getType()->getUUId(), out );
	data->doSerialize( out );
	out->flush();

}

FramePtr FrameSerializer::unSerialize(
		BFC::IO::ByteInputStreamPtr	in ) {

	// Read the UUId...

	UUId id = UUIdSerializer::unSerialize( in );

	// Instantiate appropriate factory...

	FramePtr res = FrameFactoryRegistry::instance()->getNewFrame( id );

	// Unserialize the frame...

	res->unSerialize( in );

	return res;

}

// ============================================================================

