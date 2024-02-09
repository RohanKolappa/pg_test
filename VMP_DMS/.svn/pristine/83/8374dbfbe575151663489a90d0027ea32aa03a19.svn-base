// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::MT".
// 
// "BFC::MT" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::MT" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::MT"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.MT.SFifoImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_MT_SFifoImpl_H_
#define _BFC_MT_SFifoImpl_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.MT.BrokenPipe.h"
#include "BFC.MT.Condition.h"
#include "BFC.MT.Locker.h"
#include "BFC.MT.Mutex.h"
#include "BFC.MT.SFifo.h"
#include "BFC.MT.TimeoutExpired.h"
#include "BFC.MT.WouldBlock.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief This class implements the behaviour exposed by the SFifo class.
///
/// Please don't use this class directly.

template < typename T >
class BFC_MT_DLL SFifoImpl : virtual public SObject {

public :

	SFifoImpl(
		const	Uint32		maxSize,
		const	Bool		blockng,
		const	Uint32		nbrProd,
		const	Uint32		nbrCons
	);

	virtual ~SFifoImpl(
	);

	void incNbrProd(
	);

	Uint32 decNbrProd(
	);

	Uint32 getNbrProd(
	) const;

	void incNbrCons(
	);

	Uint32 decNbrCons(
	);

	Uint32 getNbrCons(
	) const;

	Bool isEmpty(
	) const;

	Bool isNotEmpty(
	) const;

	Bool isFull(
	) const;

	Bool isNotFull(
	) const;

	void waitEmpty(
	);

	void waitEmpty(
		const	Time::Delta &	pTimeOut
	);

	void waitNotEmpty(
	);

	void waitNotEmpty(
		const	Time::Delta &	pTimeOut
	);

	void waitFull(
	);

	void waitFull(
		const	Time::Delta &	pTimeOut
	);

	void waitNotFull(
	);

	void waitNotFull(
		const	Time::Delta &	pTimeOut
	);

	Uint64 getNbrEmpty(
	) const;

	Uint64 getNbrFull(
	) const;

	Uint32 getSize(
	) const;

	Uint32 getMaxSize(
	) const;

	void kill(
	);

	T top(
	);

	T top(
		const	Time::Delta &	pTimeOut
	);

	T tryTop(
	) const;

	T pop(
	);

	T pop(
		const	Time::Delta &	pTimeOut
	);

	T tryPop(
	);

	void push(
		const	T &		pElement
	);

	void push(
		const	T &		pElement,
		const	Time::Delta &	pTimeOut
	);

private :

	class BFC_MT_DLL Cell {
	
	public :
	
		Cell(
		) : next( 0 ) {
		}
	
		Cell(
			const	T &		elt
		) : data( elt )
		  , next( 0 ) {
		}
	
		T		data;
		Cell *	next;	// next element in linked list
	
	};

	mutable Mutex	fifoMtx;	///< data protection mutex
	Condition	emptCnd;	///< "empty" condition
	Condition	nEmpCnd;	///< "not empty" condition
	Condition	fullCnd;	///< "full" condition
	Condition	nFulCnd;	///< "not full" condition
	Uint64		nbrEmpt;	///< "empty" counter
	Uint64		nbrFull;	///< "full" counter
	Uint32		nbrProd;	///< number of active producers
	Uint32		nbrCons;	///< number of active consumers
	Cell *		fstElmt;	///< first element in linked list
	Cell *		lstElmt;	///< last element in linked list
	Cell *		fstPool;	///< first element in pool list
	Cell *		lstPool;	///< last element in pool list
	Uint32		curSize;	///< current doubly linked list size
	Uint32		maxSize;	///< max linked list size
	Bool		blockng;	///< blocking mode ?

};

// ============================================================================

template < typename T >
SFifoImpl< T >::SFifoImpl(
	const	Uint32		__maxSize,
	const	Bool		__blockng,
	const	Uint32		__nbrProd,
	const	Uint32		__nbrCons )
	: emptCnd(   fifoMtx )
	, nEmpCnd(   fifoMtx )
	, fullCnd(   fifoMtx )
	, nFulCnd(   fifoMtx )
	, nbrEmpt(         0 )
	, nbrFull(         0 )
	, nbrProd( __nbrProd )
	, nbrCons( __nbrCons )
	, fstElmt(         0 )
	, lstElmt(         0 )
	, fstPool(         0 )
	, lstPool(         0 )
	, curSize(         0 )
	, maxSize( __maxSize )
	, blockng( __blockng ) {

}

template < typename T >
SFifoImpl< T >::~SFifoImpl() {

	try {
		kill();
	}
	catch ( Exception & /* e */ ) {
//		msgExc( e, "~SFifoImpl()" );
	}

}

// ============================================================================

template < typename T >
void SFifoImpl< T >::incNbrProd() {

	Locker locker( fifoMtx );

	nbrProd++;

}

template < typename T >
Uint32 SFifoImpl< T >::decNbrProd() {

	Locker locker( fifoMtx );

	if ( ! nbrProd ) {
		throw InternalError( "decNbrProd(): already 0" );
	}

	nbrProd--;

	if ( ! nbrProd ) {
		nEmpCnd.broadcast();
		fullCnd.broadcast();
	}

	return nbrProd;

}

template < typename T >
Uint32 SFifoImpl< T >::getNbrProd() const {

	Locker locker( fifoMtx );

	return nbrProd;

}

// ============================================================================

template < typename T >
void SFifoImpl< T >::incNbrCons() {

	Locker locker( fifoMtx );

	nbrCons++;

}

template < typename T >
Uint32 SFifoImpl< T >::decNbrCons() {

	if ( ! nbrCons ) {
		throw InternalError( "decNbrCons(): already 0" );
	}

	Locker locker( fifoMtx );

	nbrCons--;

	if ( ! nbrCons ) {
		emptCnd.broadcast();
		nFulCnd.broadcast();
	}

	return nbrCons;

}

template < typename T >
Uint32 SFifoImpl< T >::getNbrCons() const {

	Locker locker( fifoMtx );

	return nbrCons;

}

// ============================================================================

template < typename T >
Bool SFifoImpl< T >::isEmpty() const {

	Locker locker( fifoMtx );

	return ( curSize == 0 );

}

template < typename T >
Bool SFifoImpl< T >::isNotEmpty() const {

	Locker locker( fifoMtx );

	return ( curSize > 0 );

}

template < typename T >
Bool SFifoImpl< T >::isFull() const {

	Locker locker( fifoMtx );

	return ( maxSize > 0 && curSize == maxSize );

}

template < typename T >
Bool SFifoImpl< T >::isNotFull() const {

	Locker locker( fifoMtx );

	return ( maxSize == 0 || curSize < maxSize );

}

// ============================================================================

template < typename T >
void SFifoImpl< T >::waitEmpty() {

	Locker locker( fifoMtx );

	while ( curSize > 0 && nbrCons > 0 ) {
		emptCnd.wait();
	}

	if ( nbrCons == 0 ) {
		throw BrokenPipe();
	}

}

template < typename T >
void SFifoImpl< T >::waitEmpty(
	const	Time::Delta &	pTimeOut ) {

	Locker locker( fifoMtx );

	Time::Clock c = Time::now() + pTimeOut;

	while ( curSize > 0 && nbrCons > 0 ) {
		if ( ! emptCnd.timedWait( c ) ) {
			throw TimeoutExpired();
		}
	}

	if ( nbrCons == 0 ) {
		throw BrokenPipe();
	}

}

template < typename T >
void SFifoImpl< T >::waitNotEmpty() {

	Locker locker( fifoMtx );

	while ( curSize == 0 && nbrProd > 0 ) {
		nEmpCnd.wait();
	}

	if ( nbrProd == 0 ) {
		throw BrokenPipe();
	}

}

template < typename T >
void SFifoImpl< T >::waitNotEmpty(
	const	Time::Delta &	pTimeOut ) {

	Locker locker( fifoMtx );

	Time::Clock c = Time::now() + pTimeOut;

	while ( curSize == 0 && nbrProd > 0 ) {
		if ( ! nEmpCnd.timedWait( c ) ) {
			throw TimeoutExpired();
		}
	}

	if ( nbrProd == 0 ) {
		throw BrokenPipe();
	}

}

template < typename T >
void SFifoImpl< T >::waitFull() {

	Locker locker( fifoMtx );

	if ( maxSize == 0 ) {
		throw WouldBlock();
	}

	while ( curSize < maxSize && nbrProd > 0 ) {
		fullCnd.wait();
	}

	if ( nbrProd == 0 ) {
		throw BrokenPipe();
	}

}

template < typename T >
void SFifoImpl< T >::waitFull(
	const	Time::Delta &	pTimeOut ) {

	Locker locker( fifoMtx );

	Time::Clock c = Time::now() + pTimeOut;

	if ( maxSize == 0 ) {
		throw WouldBlock();
	}

	while ( curSize < maxSize && nbrProd > 0 ) {
		if ( ! fullCnd.timedWait( c ) ) {
			throw TimeoutExpired();
		}
	}

	if ( nbrProd == 0 ) {
		throw BrokenPipe();
	}

}

template < typename T >
void SFifoImpl< T >::waitNotFull() {

	Locker locker( fifoMtx );

	if ( maxSize == 0 ) {
		return;
	}

	while ( curSize == maxSize && nbrCons > 0 ) {
		nFulCnd.wait();
	}

	if ( nbrCons == 0 ) {
		throw BrokenPipe();
	}

}

template < typename T >
void SFifoImpl< T >::waitNotFull(
	const	Time::Delta &	pTimeOut ) {

	Locker locker( fifoMtx );

	Time::Clock c = Time::now() + pTimeOut;

	if ( maxSize == 0 ) {
		return;
	}

	while ( curSize == maxSize && nbrCons > 0 ) {
		if ( ! nFulCnd.timedWait( c ) ) {
			throw TimeoutExpired();
		}
	}

	if ( nbrCons == 0 ) {
		throw BrokenPipe();
	}

}

// ============================================================================

template < typename T >
Uint64 SFifoImpl< T >::getNbrEmpty() const {

	Locker locker( fifoMtx );

	return nbrEmpt;

}

template < typename T >
Uint64 SFifoImpl< T >::getNbrFull() const {

	Locker locker( fifoMtx );

	return nbrFull;

}

// ============================================================================

template < typename T >
Uint32 SFifoImpl< T >::getSize() const {

	Locker locker( fifoMtx );

	return curSize;

}

template < typename T >
Uint32 SFifoImpl< T >::getMaxSize() const {

	Locker locker( fifoMtx );

	return maxSize;

}

// ============================================================================

template < typename T >
void SFifoImpl< T >::kill() {

//	msgDbg( "kill(): --->" );

	Locker locker( fifoMtx );

	while ( fstElmt ) {
		Cell *oldFrst = fstElmt;
		fstElmt = fstElmt->next;
		delete oldFrst;
	}

	while ( fstPool ) {
		Cell *oldFrst = fstPool;
		fstPool = fstPool->next;
		delete oldFrst;
	}

	lstElmt = 0;
	lstPool = 0;
	curSize = 0;

	emptCnd.broadcast();
	nFulCnd.broadcast();

//	msgDbg( "kill(): <---" );

}

// ============================================================================

template < typename T >
T SFifoImpl< T >::top() {

	Locker locker( fifoMtx );

	if ( curSize == 0 ) {
		if ( ! blockng ) {
			throw WouldBlock();
		}
		while ( curSize == 0 && nbrProd > 0 ) {
			nEmpCnd.wait();
		}
		if ( nbrProd == 0 ) {
			throw BrokenPipe();
		}
	}

	return fstElmt->data;

}

template < typename T >
T SFifoImpl< T >::top(
	const	Time::Delta &	pTimeOut ) {

	Locker locker( fifoMtx );

	if ( curSize == 0 ) {
		if ( ! blockng ) {
			throw WouldBlock();
		}
		Time::Clock c = Time::now() + pTimeOut;
		while ( curSize == 0 && nbrProd > 0 ) {
			if ( ! nEmpCnd.timedWait( c ) ) {
				throw TimeoutExpired();
			}
		}
		if ( nbrProd == 0 ) {
			throw BrokenPipe();
		}
	}

	return fstElmt->data;

}

template < typename T >
T SFifoImpl< T >::tryTop() const {

	Locker locker( fifoMtx );

	if ( curSize == 0 ) {
		return T();
	}

	return fstElmt->data;

}

// ============================================================================

template < typename T >
T SFifoImpl< T >::pop() {

	Locker locker( fifoMtx );

	if ( curSize == 0 ) {
		if ( ! blockng ) {
			throw WouldBlock();
		}
		while ( curSize == 0 && nbrProd > 0 ) {
			nEmpCnd.wait();
		}
		if ( nbrProd == 0 ) {
			throw BrokenPipe();
		}
	}

	if ( maxSize > 0 && curSize == maxSize ) {
		nFulCnd.broadcast();
	}

	curSize--;

	Cell *oldFrst = fstElmt;

	fstElmt = fstElmt->next;

	if ( ! fstElmt ) {
		lstElmt = 0;
		emptCnd.broadcast();
		nbrEmpt++;
	}
	else {
		oldFrst->next = 0;
	}

	if ( lstPool ) {
		lstPool->next = oldFrst;
	}
	else {
		fstPool = oldFrst;
	}

	lstPool = oldFrst;

	T res = oldFrst->data;

	oldFrst->data.kill();

	return res;

}

template < typename T >
T SFifoImpl< T >::pop(
	const	Time::Delta &	pTimeOut ) {

	Locker locker( fifoMtx );

	if ( curSize == 0 ) {
		if ( ! blockng ) {
			throw WouldBlock();
		}
		Time::Clock c = Time::now() + pTimeOut;
		while ( curSize == 0 && nbrProd > 0 ) {
			if ( ! nEmpCnd.timedWait( c ) ) {
				throw TimeoutExpired();
			}
		}
		if ( nbrProd == 0 ) {
			throw BrokenPipe();
		}
	}

	if ( maxSize > 0 && curSize == maxSize ) {
		nFulCnd.broadcast();
	}

	curSize--;

	Cell *oldFrst = fstElmt;

	fstElmt = fstElmt->next;

	if ( ! fstElmt ) {
		lstElmt = 0;
		emptCnd.broadcast();
		nbrEmpt++;
	}
	else {
		oldFrst->next = 0;
	}

	if ( lstPool ) {
		lstPool->next = oldFrst;
	}
	else {
		fstPool = oldFrst;
	}

	lstPool = oldFrst;

	T res = oldFrst->data;

	oldFrst->data.kill();

	return res;

}

template < typename T >
T SFifoImpl< T >::tryPop() {

	Locker locker( fifoMtx );

	if ( curSize == 0 ) {
		return T();
	}

	if ( maxSize > 0 && curSize == maxSize ) {
		nFulCnd.broadcast();
	}

	curSize--;

	Cell *oldFrst = fstElmt;

	fstElmt = fstElmt->next;

	if ( ! fstElmt ) {
		lstElmt = 0;
		emptCnd.broadcast();
		nbrEmpt++;
	}
	else {
		oldFrst->next = 0;
	}

	if ( lstPool ) {
		lstPool->next = oldFrst;
	}
	else {
		fstPool = oldFrst;
	}

	lstPool = oldFrst;

	T res = oldFrst->data;

	oldFrst->data.kill();

	return res;

}

// ============================================================================

template < typename T >
void SFifoImpl< T >::push(
	const	T&		t ) {

	Locker locker( fifoMtx );

	if ( maxSize > 0 && curSize == maxSize ) {
		if ( ! blockng ) {
			throw WouldBlock();
		}
		while ( maxSize > 0 && curSize == maxSize && nbrCons > 0 ) {
			nFulCnd.wait();
		}
		if ( nbrCons == 0 ) {
			throw BrokenPipe();
		}
	}

	if ( ! fstPool ) {
		fstPool = lstPool = new Cell;
	}

	Cell *newFrst = fstPool;

	fstPool = fstPool->next;

	if ( ! fstPool ) {
		lstPool = 0;
	}
	else {
		newFrst->next = 0;
	}

	if ( lstElmt ) {
		lstElmt->next = newFrst;
	}
	else {
		fstElmt = newFrst;
	}

	lstElmt = newFrst;

	newFrst->data = t;

	curSize++;

	if ( maxSize > 0 && curSize == maxSize ) {
		fullCnd.broadcast();
		nbrFull++;
	}

	nEmpCnd.broadcast();

}

template < typename T >
void SFifoImpl< T >::push(
	const	T&		t,
	const	Time::Delta &	pTimeOut  ) {

	Locker locker( fifoMtx );

	if ( maxSize > 0 && curSize == maxSize ) {
		if ( ! blockng ) {
			throw WouldBlock();
		}
		Time::Clock c = Time::now() + pTimeOut;
		while ( maxSize > 0 && curSize == maxSize && nbrCons > 0 ) {
			if ( ! nFulCnd.timedWait( c ) ) {
				throw TimeoutExpired();
			}
		}
		if ( nbrCons == 0 ) {
			throw BrokenPipe();
		}
	}

	if ( ! fstPool ) {
		fstPool = lstPool = new Cell;
	}

	Cell *newFrst = fstPool;

	fstPool = fstPool->next;

	if ( ! fstPool ) {
		lstPool = 0;
	}
	else {
		newFrst->next = 0;
	}

	if ( lstElmt ) {
		lstElmt->next = newFrst;
	}
	else {
		fstElmt = newFrst;
	}

	lstElmt = newFrst;

	newFrst->data = t;

	curSize++;

	if ( maxSize > 0 && curSize == maxSize ) {
		fullCnd.broadcast();
		nbrFull++;
	}

	nEmpCnd.broadcast();

}

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_SFifoImpl_H_

// ============================================================================

