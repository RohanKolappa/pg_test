// ============================================================================
//
//	Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
//	Filename: VMP_DynVar.cpp
//
//	Creation:
//		Author(s):	Jean-Francois GOBBERS
//		Date:		
//		Version:	
//		Description:	
//
// CVS: $Id: signature.class.cpp,v 1.1.1.1 2001/10/26 14:04:55 gobbers Exp $
//
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "VMP.DynVar.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL(DynVar)

// ============================================================================

DynVar::DynVar() {



}

DynVar::DynVar(
	const	DynVar&		other )
	: /**/ SObject() {

	vars = other.vars;

}

DynVar::~DynVar() {



}

// ============================================================================

DynVar& DynVar::operator = (
	const	DynVar&		other ) {

	if ( this != &other ) {
		vars = other.vars;
	}

	return *this;

}

// ============================================================================

void DynVar::add(
	const	Buffer &	pVarTag,
	const	Buffer &	pVarVal ) {

	FastCSLocker	locker( crit );

	vars += new Dyn::VBuffer( pVarTag, pVarVal );

}

void DynVar::del(
	const	Buffer &	pVarTag ) {

	FastCSLocker	locker( crit );

	for ( Uint32 i = 0 ; i < vars.getSize() ; i++ ) {
		if ( vars[ i ]->getName() == pVarTag ) {
			vars.pop( i );
			return;
		}
	}

	throw NoSuchVariable( "Name: \"" + pVarTag + "\"" );

}

// ============================================================================

void DynVar::set(
	const	Buffer &	pVarTag,
	const	Buffer &	pVarVal ) {

	FastCSLocker	locker( crit );

	for ( Uint32 i = 0 ; i < vars.getSize() ; i++ ) {
		if ( vars[ i ]->getName() == pVarTag ) {
			vars[ i ]->setValue( pVarVal );
			return;
		}
	}

	throw NoSuchVariable( "Name: \"" + pVarTag + "\"" );

}

// ============================================================================

Buffer DynVar::get(
	const	Buffer &	pVarTag ) const {

	FastCSLocker	locker( crit );

	for ( Uint32 i = 0 ; i < vars.getSize() ; i++ ) {
		if ( vars[ i ]->getName() == pVarTag ) {
			return vars[ i ]->getValue();
		}
	}

	throw NoSuchVariable( "Name: \"" + pVarTag + "\"" );

}

// ============================================================================

Buffer DynVar::getBuffer(
	const	Buffer &	pVarTag ) const {

	return get( pVarTag );

}

Uint32 DynVar::getUint32(
	const	Buffer &	pVarTag ) const {

	return get( pVarTag ).toUint32();

}

Bool DynVar::getBool(
	const	Buffer &	pVarTag ) const {

	return get( pVarTag ).toBool();

}

Double DynVar::getDouble(
	const	Buffer &	pVarTag ) const {

	return get( pVarTag ).toDouble();

}

// ============================================================================

