// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.DevNullEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.DevNullEngine.h"
#include "VMP.L2.EngineRegisterer.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, DevNullEngine )

// ============================================================================

const DL::TypeCPtr & L2::DevNullEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "bb7427db-bd4e-4f8f-8403-c784714ee22c" ),
		"VMP.L2.DevNullEngine",
		L2::FinalPushEngine::getClassType(),
		new UI::UObjectDescr(
			"NullDevice",
			"Utils",
			"NullDevice"
		)
	);

	return i;

}

// ============================================================================

L2::DevNullEngine::DevNullEngine(
	const	Bool		pDumpData ) :

	FinalPushEngine( getClassType() ),

	dumpData	( pDumpData ) {

	setNbrInputs( 1 );
	setInputType( 0, Session::getClassType() );

	numberSessions	= 0;
	chronoSessions.start();

}

// ============================================================================

void L2::DevNullEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

	numberSessions++;

	numberFrames	= 0;
	lengthFrames	= 0;
	chronoFrames.start();

	setInSession( true );

	if ( dumpData ) {
		msgDbg( "putSessionCallback():\n" + pInptSess->toBuffer() );
	}

}

void L2::DevNullEngine::endSessionCallback() {

	chronoFrames.stop();

	if ( dumpData ) {
		msgDbg( "endSessionCallback(): " + Buffer( numberFrames )
			+ " frame(s) (length: "
			+ Buffer( lengthFrames )
			+ ") in "
			+ Buffer( chronoFrames.getElapsed() )
			+ " usecs" );
	}

	setInSession( false );

}

void L2::DevNullEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

	numberFrames++;
	lengthFrames += pInptFrme->getLength();

	if ( dumpData ) {
		msgDbg( "putFrameCallback(): " + pInptFrme->toBuffer() );
	}

}

// ============================================================================

static L2::EngineRegisterer< L2::DevNullEngine > engineRegisterer;

// ============================================================================

