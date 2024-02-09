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
//	BFC.SPtr.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>
#include <typeinfo>

#include "BFC.Base.SObject.h"
#include "BFC.Base.SPtr.h"
#include "BFC.Base.SPtrHub.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

SPtr::SPtr(
) {

	hub = 0;
	mst = true;

//	currCount++ ; if ( currCount > peakCount ) peakCount = currCount;

}

SPtr::SPtr(
		SObject		*final
) {

	hub = ( final ? final->getHub() : 0 );
	mst = true;

	if ( hub ) {
		hub->incPtr( mst );
	}

//	currCount++ ; if ( currCount > peakCount ) peakCount = currCount;

}

SPtr::SPtr(
	const	SPtr&		o
) {

	if ( ( hub = o.hub ) ) {
		hub->incPtr(true);
	}

	mst = true;

//	currCount++ ; if ( currCount > peakCount ) peakCount = currCount;

}

SPtr::~SPtr(
) {

	SPtrHub *oldHub;

	{

		oldHub = hub;
		hub = 0;

	}

	if ( oldHub && oldHub->decPtr( mst ) ) {
		delete oldHub;
	}

//	currCount--;
//
//	if ( ! currCount ) {
//		cerr << "peak # SPtr == " << peakCount << endl;
//	}

}

SPtr& SPtr::operator = (
		SObject		*final
) {

	SPtrHub		*oldHub;
	Bool		oldMst;

	{

		SPtrHub *newHub = ( final ? final->getHub() : 0 );

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

	return *this;

}

SPtr& SPtr::operator = (
	const	SPtr&	o
) {

	SPtrHub		*oldHub;
	Bool		oldMst;
	SPtrHub		*newHub;

	{

		if ( ( newHub = o.hub ) ) {
			newHub->incPtr( mst );
		}

		oldHub = hub;
		oldMst = mst;

		hub = newHub;

	}

	if ( oldHub && oldHub->decPtr( oldMst ) ) {
		delete oldHub;
	}

	return *this;

}

// ============================================================================

const char *SPtr::typeName(
) const {

	return ( hub ? hub->typeName() : "<NULL>" );

}

// ============================================================================

Bool SPtr::operator == (
	const	SObject	*ptr
) const {

	return ( hub && *hub == ptr );

}

Bool SPtr::operator != (
	const	SObject	*ptr
) const {

	return ( ! hub || *hub != ptr );

}

Bool SPtr::operator == (
	const	SPtr&	o
) const {

	return ( hub && o.hub && hub == o.hub );

}

Bool SPtr::operator != (
	const	SPtr&	o
) const {

	return ( ! hub || ! o.hub || hub != o.hub );

}

// ============================================================================

SPtr::operator Bool () const {

	return ( hub && *hub );

}

Bool SPtr::operator ! () const {

	return ( ! hub || ! *hub );

}

// ============================================================================

void SPtr::kill() {

	SPtrHub *oldHub;

	{

		oldHub = hub;
		hub = 0;

	}

	if ( oldHub && oldHub->decPtr( mst ) ) {
		delete oldHub;
	}

}

// ============================================================================

void SPtr::setMaster(
	const	Bool		b) {

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

SObject *SPtr::getRoot(const SPtr& o) const {

	return o.getRoot();

}

SObject *SPtr::getRoot() const {

	return ( hub ? hub->getRoot() : 0 );

}

// ============================================================================

//Uint32 SPtr::currCount = 0;
//Uint32 SPtr::peakCount = 0;

// ============================================================================

