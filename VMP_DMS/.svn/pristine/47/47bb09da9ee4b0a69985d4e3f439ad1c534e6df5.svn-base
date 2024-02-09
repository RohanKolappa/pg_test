// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.SessionSerializer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.UUIdSerializer.h"

#include "VMP.SessionFactoryRegistry.h"
#include "VMP.SessionSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

void SessionSerializer::doSerialize(
		BFC::IO::ByteOutputStreamPtr	out,
		VMP::SessionPtr			session ) {

	// First, send the UUId...

//	msgDbg( "doSerialize(): UUId: " + session->getType()->getUUId().toBuffer() );

	UUIdSerializer::doSerialize( session->getType()->getUUId(), out );

	session->doSerialize( out );

}

SessionPtr SessionSerializer::unSerialize(
		BFC::IO::ByteInputStreamPtr	in ) {

	// Read the UUId...

	UUId id = UUIdSerializer::unSerialize( in );

//	msgDbg( "unSerialize(): UUId: " + id.toBuffer() );

	// Instantiate appropriate factory...

	SessionPtr res = SessionFactoryRegistry::instance()->getNewSession( id );

	// Unserialize the session...

	res->unSerialize( in );

	return res;

}

// ============================================================================

