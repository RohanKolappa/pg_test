// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Base.SPtr.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <typeinfo>

#include "TBFC.Base.FastCSLocker.h"
#include "TBFC.Base.SObject.h"
#include "TBFC.Base.SPtr.h"
#include "TBFC.Base.SPtrHub.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

SPtr::SPtr() {

	hub = 0;
	mst = true;

}

SPtr::SPtr(
		SObject		* final ) {

	hub = ( final ? final->getHub() : 0 );
	mst = true;

	if ( hub ) {
		hub->incPtr( mst );
	}

//	cerr	<< "SPtr( SObject * ): this == "
//		<< this
//		<< ", object == "
//		<< final
//		<< ", & hub == "
//		<< ( & hub )
//		<< ", hub == "
//		<< hub
//		<< ", typename == "
//		<< ( hub ? hub->typeName() : "?" )
//		<< endl;

}

SPtr::SPtr(
	const	SPtr		& o ) {

//	cerr	<< "SPtr( SPtr & ): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	mst = o.mst;

	if ( ( hub = o.hub ) ) {
		hub->incPtr( mst );
	}

}

// ============================================================================

SPtr::~SPtr() {

//	cerr	<< "~SPtr(): this == "
//		<< this
//		<< endl;

	SPtrHub * oldHub;

	{

		FastCSLocker locker(*getCriticalSection());

		oldHub = hub;
		hub = 0;

	}

	if ( oldHub && oldHub->decPtr( mst ) ) {
		delete oldHub;
	}

}

// ============================================================================

SPtr& SPtr::operator = (
		SObject		* final ) {

//	cerr	<< "SPtr:: operator = ( SObject * ): this == "
//		<< this
//		<< endl;

	SPtrHub		* oldHub;
	Bool		oldMst;

	{

		FastCSLocker locker(*getCriticalSection());

		SPtrHub * newHub = ( final ? final->getHub() : 0 );

		if ( newHub ) {
			newHub->incPtr( mst );
		}

		oldHub = hub;
		oldMst = mst;

		hub = newHub;

	}

	if ( oldHub && oldHub->decPtr( oldMst ) ) {
		delete oldHub;
	}

	return * this;

}

SPtr& SPtr::operator = (
	const	SPtr		& other ) {

//	cerr	<< "SPtr::operator = ( SPtr & ): this == "
//		<< this
//		<< endl;

	SPtrHub		*oldHub;
	Bool		oldMst;

	{

		FastCSLocker locker(*getCriticalSection());

		oldHub = hub;
		oldMst = mst;

		hub = other.hub;
		mst = other.mst;

		if ( hub ) {
			hub->incPtr( mst );
		}

	}

	if ( oldHub && oldHub->decPtr( oldMst ) ) {
		delete oldHub;
	}

	return * this;

}

// ============================================================================

const char *SPtr::typeName() const {

//	cerr << "SPtr::typeName(): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	return ( hub ? hub->typeName() : "<NULL>" );

}

// ============================================================================

Bool SPtr::operator == (
	const	SObject		* ptr ) const {

//	cerr << "SPtr::operator == (): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	return ( hub && * hub == ptr );

}

Bool SPtr::operator != (
	const	SObject		* ptr ) const {

//	cerr << "SPtr::operator != (): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	return ( ! hub || * hub != ptr );

}

Bool SPtr::operator == (
	const	SPtr		& other ) const {

//	cerr << "SPtr::operator == (): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	return ( hub && other.hub && hub == other.hub );

}

Bool SPtr::operator != (
	const	SPtr		& other ) const {

//	cerr << "SPtr::operator != (): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	return ( ! hub || ! other.hub || hub != other.hub );

}

// ============================================================================

SPtr::operator Bool () const {

//	cerr << "SPtr::operator Bool (): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	return ( hub && * hub );

}

Bool SPtr::operator ! () const {

//	cerr << "SPtr::operator ! (): this == "
//		<< this
//		<< endl;

	FastCSLocker locker(*getCriticalSection());

	return ( ! hub || ! * hub );

}

// ============================================================================

void SPtr::kill() {

//	cerr << "SPtr::kill(): this == "
//		<< this
//		<< endl;

	SPtrHub * oldHub;

	{

		FastCSLocker locker(*getCriticalSection());

		oldHub = hub;
		hub = 0;

	}

	if ( oldHub && oldHub->decPtr( mst ) ) {
		delete oldHub;
	}

}

// ============================================================================

void SPtr::setMaster(
	const	Bool		b ) {

	FastCSLocker locker(*getCriticalSection());

	if ( b == mst ) {
		return;
	}

	mst = b;

	if ( ! hub ) {
		return;
	}

	if ( mst ) {
		hub->incMst();
	}
	else {
		hub->decMst();
	}

}

// ============================================================================

SObject *SPtr::getRoot(
	const	SPtr		& other ) const {

	return other.getRoot();

}

// SObject *SPtr::getRoot() const {
// 
// //	FastCSLocker locker(*getCriticalSection());
// 
// 	SObject * res = ( hub ? hub->getRoot() : 0 );
// 
// 	return res;
// 
// }

// ============================================================================

FastCS *SPtr::getCriticalSection() {

	static FastCS * cs = new FastCS; // LEAKED!

	return cs;

}

// ============================================================================

