// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.L2.Engine.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.L2.Engine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, Engine )

// ============================================================================

const DL::TypeCPtr & L2::Engine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "391da70b-d865-409a-9a6a-70a8a167c229" ),
		"VMP.L2.Engine",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

L2::Engine::Engine(
	const	Mode		pMode ) :

	UI::UObject	( getClassType() ),

	uuid		( UUId::makeV1() ),
	mode		( pMode ),
	inSession	( false ) {

}

L2::Engine::Engine(
	const	Mode		pMode,
	const	DL::TypeCPtr &	pType ) :

	UI::UObject	( pType ),

	uuid		( UUId::makeV1() ),
	mode		( pMode ),
	inSession	( false ) {

}

// ============================================================================

//void L2::Engine::setObjectName(
//	const	Buffer &	pName ) {
//
//	DObject::setObjectName( pName );
//
//	if ( backend ) {
//		backend->setObjectName( pName );
//	}
//
//}

// ============================================================================

const UUId & L2::Engine::getUUId() const {

	return uuid;

}

L2::Engine::Mode L2::Engine::getMode() const {

	return mode;

}

// ============================================================================

void L2::Engine::setBackend(
		L3::BackendPtr		pBackend ) {

	backend = pBackend;

	attachDynVarList( backend );

}

// ============================================================================

Uint32 L2::Engine::getNbrInputs() const {

	return iTypes.size();

}

const DL::TypeCPtr & L2::Engine::getInputType(
	const	Uint32		idx ) const {

	return iTypes[ idx ];

}

Uint32 L2::Engine::getNbrOutputs() const {

	return oTypes.size();

}

const DL::TypeCPtr & L2::Engine::getOutputType(
	const	Uint32		idx ) const {

	return oTypes[ idx ];

}

// ============================================================================

void L2::Engine::setUserInterface(
		UI::IElementPtr		pUi ) {

	if ( backend ) {
		backend->setUserInterface( pUi );
	}
	else {
		UI::UObject::setUserInterface( pUi );
	}

}

UI::IElementPtr L2::Engine::getUserInterface() const {

	if ( backend ) {
		return backend->getUserInterface();
	}
	else {
		return UI::UObject::getUserInterface();
	}

}

// ============================================================================

DynVar L2::Engine::getUserSettings() {

	if ( backend ) {
		return backend->getUserSettings();
	}
	else {
		DynVar res;
		for ( Uint32 i = 0 ; i < getDynVars().getSize() ; i++ ) {
			res.add(
				getDynVars()[ i ]->getName(),
				getDynVars()[ i ]->toBuffer() );
		}
		return res;
	}

}

void L2::Engine::setUserSettings(
	const	DynVar&			settings ) {

	if ( backend ) {
		backend->setUserSettings( settings );
	}
	else if ( ! settings.isEmpty() ) {
		for ( Uint32 i = 0 ; i < settings.size() ; i++ ) {
			Buffer	t = settings.getTag( i );
			Buffer	v = settings.getVal( i );
			getDynVar( t )->fromBuffer( v );
		}
	}

}

// ============================================================================

void L2::Engine::startEngine() {

//	msgDbg( "startEngine()" );

	if ( backend ) {
		backend->startBackend();
	}
	else if ( hasUserInterface() ) {
		getUserInterface()->setEnabled( true );
	}

}

void L2::Engine::stopEngine() {

//	msgDbg( "stopEngine()" );

	if ( backend ) {
		backend->stopBackend();
	}
	else if ( hasUserInterface() ) {
		getUserInterface()->setEnabled( false );
	}

}

// ============================================================================

void L2::Engine::setNbrInputs(
	const	Uint32		size ) {

	iTypes.resize( size );

}

void L2::Engine::setInputType(
	const	Uint32		idx,
	const	DL::TypeCPtr &	type ) {

	iTypes[ idx ] = type;

}

void L2::Engine::setNbrOutputs(
	const	Uint32		size ) {

	oTypes.resize( size );

}

void L2::Engine::setOutputType(
	const	Uint32		idx,
	const	DL::TypeCPtr &	type ) {

	oTypes[ idx ] = type;

}

// ============================================================================

