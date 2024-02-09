// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.StreamSerializer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.UUIdSerializer.h"

#include "VMP.StreamFactoryRegistry.h"
#include "VMP.StreamSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

void StreamSerializer::doSerialize(
		BFC::IO::ByteOutputStreamPtr	out,
		VMP::StreamPtr			data ) {

	// First, send the UUId...

	UUIdSerializer::doSerialize( data->getType()->getUUId(), out );

	data->doSerialize( out );

}

StreamPtr StreamSerializer::unSerialize(
		BFC::IO::ByteInputStreamPtr	in ) {

	// Read the UUId...

	UUId id = UUIdSerializer::unSerialize( in );

	// Instantiate appropriate factory...

	StreamPtr res = StreamFactoryRegistry::instance()->getNewStream( id );

	// Unserialize the frame...

	res->unSerialize( in );

	return res;

}

// ============================================================================

