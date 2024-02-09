// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.SPtr.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <typeinfo>

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.SPtr.h"
#include "BFC.Base.SPtrHub.h"

// ============================================================================

using namespace BFC;

// ============================================================================

SCPtr::SCPtr(
	const	SObject *	final,
	const	Bool		pMaster,
	const	Bool		pSticky ) {

	hub = ( final ? final->getHub() : 0 );
	mst = pMaster;
	stk = pSticky;

	if ( hub ) {
		hub->incPtr( mst );
		sfe = hub->isThreadSafe();
	}
	else {
		sfe = true;
	}

}

// ============================================================================

SCPtr::~SCPtr() {

	SPtrHub * oldHub;

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	oldHub = hub;
	hub = 0;

	if ( protect ) {
		getCriticalSection()->leave();
	}

	if ( oldHub && oldHub->decPtr( mst ) ) {
		delete oldHub;
	}

}

// ============================================================================

SCPtr & SCPtr::operator = (
	const	SObject *	final ) {

	SPtrHub *	oldHub;
	Bool		oldMst;

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	oldHub = hub;
	oldMst = mst;

	if ( ( hub = ( final ? final->getHub() : 0 ) ) ) {
		hub->incPtr( mst );
		sfe = hub->isThreadSafe();
	}
	else {
		sfe = true;
	}

	if ( protect ) {
		getCriticalSection()->leave();
	}

	if ( oldHub && oldHub->decPtr( oldMst ) ) {
		delete oldHub;
	}

	return * this;

}

SCPtr & SCPtr::operator = (
	const	SCPtr &		other ) {

	SPtrHub *	newHub;
	Bool		newMst;
	Bool		newSfe;

//	other.initClone( newHub, newMst, newSfe, mst, stk );
	other.initClone( newHub, newMst, newSfe, stk ? mst : true, stk );

	SPtrHub *	oldHub;
	Bool		oldMst;

	Bool protect = ( sfe || newSfe );

	if ( protect ) {
		getCriticalSection()->enter();
	}

	oldHub = hub;
	oldMst = mst;

	hub = newHub;
	mst = newMst;
	sfe = newSfe;

	if ( protect ) {
		getCriticalSection()->leave();
	}

	if ( oldHub && oldHub->decPtr( oldMst ) ) {
		delete oldHub;
	}

	return * this;

}

// ============================================================================

const char * SCPtr::typeName() const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	const char * res = ( hub ? hub->typeName() : "<NULL>" );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

// ============================================================================

Bool SCPtr::operator == (
	const	SObject *	ptr ) const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	Bool res = ( hub && * hub == ptr );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

Bool SCPtr::operator != (
	const	SObject *	ptr ) const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	Bool res = ( ! hub || * hub != ptr );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

Bool SCPtr::operator == (
	const	SCPtr &		other ) const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	Bool res = ( hub && other.hub && hub == other.hub );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

Bool SCPtr::operator != (
	const	SCPtr &		other ) const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	Bool res = ( ! hub || ! other.hub || hub != other.hub );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

// ============================================================================

SCPtr::operator Bool () const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	Bool res = ( hub && * hub );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

Bool SCPtr::operator ! () const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	Bool res = ( ! hub || ! * hub );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

// ============================================================================

void SCPtr::kill() {

	SPtrHub * oldHub;

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	oldHub = hub;
	hub = 0;

	if ( protect ) {
		getCriticalSection()->leave();
	}

	if ( oldHub && oldHub->decPtr( mst ) ) {
		delete oldHub;
	}

}

// ============================================================================

void SCPtr::setMaster(
	const	Bool		b ) {

	if ( stk && ( b != mst ) ) {
		throw InvalidArgument( "Modifyng a Sticky SCPtr!" );
	}

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	if ( b != mst ) {
		mst = b;
		if ( hub ) {
			if ( mst ) {
				hub->incMst();
			}
			else {
				hub->decMst();
			}
		}
	}

	if ( protect ) {
		getCriticalSection()->leave();
	}

}

Bool SCPtr::setThreadSafe(
	const	Bool		pSfe ) {

	Bool protect = ( sfe || pSfe );

	if ( protect ) {
		getCriticalSection()->enter();
	}

	if ( hub ) {
		sfe = hub->setThreadSafe( pSfe );
	}
	else {
		sfe = pSfe;
	}

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return sfe;

}

// ============================================================================

Uint32 SCPtr::getRefCount() const {

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	Uint32 res = ( hub ? hub->getRefCount() : 0 );

	if ( protect ) {
		getCriticalSection()->leave();
	}

	return res;

}

// ============================================================================

void SCPtr::initClone(
		SPtrHub * &	newHub,
		Bool &		newMst,
		Bool &		newSfe,
	const	Bool		expMst,
	const	Bool		sticky ) const {

	// Another SCPtr wants a copy of our fields!

	Bool protect = sfe;

	if ( protect ) {
		getCriticalSection()->enter();
	}

	newMst = ( sticky ? expMst : mst && expMst );

	if ( hub ) {
		hub->incPtr( newMst );
		newSfe = hub->isThreadSafe();
	}
	else {
		newSfe = true;
	}

	newHub = hub;

	if ( protect ) {
		getCriticalSection()->leave();
	}

}

// ============================================================================

FastCS * SCPtr::getCriticalSection() {

	static FastCS * cs = new FastCS; // LEAKED!

	return cs;

}

// ============================================================================

void SCPtr::throwNullPointer(
	const	char *		dstName ) const {

	throw NullPointer( "Type: " + Buffer( dstName ) );

}

void SCPtr::throwBadCast(
	const	char *		pSrcName,
	const	char *		pDstName ) const {

	throw BadCast( "Casting " + Buffer( pSrcName )
		+ " to " + Buffer( pDstName ) );

}

// ============================================================================

