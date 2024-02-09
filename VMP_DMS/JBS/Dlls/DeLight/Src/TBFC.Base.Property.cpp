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
//	TBFC.Base.Property.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.FastCSLocker.h"
#include "TBFC.Base.Property.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

Property::Property()
	: valLock( false ) {



}

Property::Property(
	const	Buffer		& __tag )
	: propTag( __tag )
	, valLock( false ) {



}

Property::Property(
	const	Buffer		& __tag,
	const	Buffer		& __val )
	: propTag( __tag )
	, propVal( __val )
	, valLock(  true ) {



}

Property::Property(
	const	Property	& __other )
	: propTag( __other.propTag )
	, propVal( __other.propVal )
	, valLock( __other.valLock ) {



}

// ============================================================================

Property::~Property() {



}

// ============================================================================

Property & Property::operator = (
	const	Property	& __other ) {

	Buffer	newPropTag;
	Buffer	newPropVal;
	Bool	newValLock;

	{
		FastCSLocker locker( __other.critSec );
		newPropTag = __other.propTag;
		newPropVal = __other.propVal;
		newValLock = __other.valLock;
	}

	{
		FastCSLocker locker( critSec );
		propTag = newPropTag;
		propVal = newPropVal;
		valLock = newValLock;
	}

	return * this;

}

// ============================================================================

void Property::setTag(
	const	Buffer		& __tag ) {

	FastCSLocker locker( critSec );

	if ( ! propTag.isEmpty() ) {
		throw Locked();
	}

	propTag = __tag;

}

const Buffer & Property::getTag() const {

	return propTag;

}

// ============================================================================

void Property::setVal(
	const	Buffer		& __val ) {

	FastCSLocker locker( critSec );

	if ( valLock ) {
		throw Locked();
	}

	propVal = __val;

}

void Property::lockVal() {

	FastCSLocker locker( critSec );

	valLock = true;

}

Bool Property::isValLocked() const {

	FastCSLocker locker( critSec );

	return valLock;

}

const Buffer & Property::getVal() const {

	return propVal;

}

// ============================================================================

