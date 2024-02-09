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
//	BFC.MT.SFifo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_SFifo_H_
#define _BFC_MT_SFifo_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.SFifoPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

template < typename T >
class SFifoImpl;

// ============================================================================

/// \brief A FIFO of SPtr objects.
///
/// \ingroup BFC_MT

class BFC_MT_DLL SFifo : virtual public SObject {

public :

	/// \brief Creates an empty FIFO.
	///
	/// This constructor creates an empty FIFO of SPtr, whose maximum
	/// size is \a maxSize. If \a maxSize is 0, the FIFO can grow without
	/// limit.
	///
	/// All calls to blocking methods will generate a WouldBlock exception
	/// when they would block and the \a blockng flag has been set to
	/// false. Otherwise, they just block until some condition is met.
	///
	/// This FIFO also remembers how many producers and how many consumers
	/// are active at a time. The number of each type is initially set to
	/// \a nbrProd and \a nbrCons respectively (default is 1). Decreasing
	/// the number of producers or consumers is used to unblock waiting
	/// threads in some circumstances.
	///
	/// \param maxSize
	///	The maximum size of the FIFO. 0 means no limit.
	///
	/// \param blockng
	///
	/// \param nbrProd
	///
	/// \param nbrCons

	SFifo(
		const	Uint32		maxSize = 0,
		const	Bool		blockng = false,
		const	Uint32		nbrProd = 1,
		const	Uint32		nbrCons = 1
	);

	/// \brief Destroys this FIFO, deleting any used resource.

	virtual ~SFifo(
	);

	/// \brief Tells this object that one more producer has become active.
	///
	/// When a new SFifo is created, the number of active producers is
	/// by default set
	/// to 1, so there is no need to call this method initially.

	void incNbrProd(
	);

	/// \brief Tells this object that one producer is not active anymore.
	///
	/// When the number of active producers reach 0,
	/// all consumers waiting for data and all threads waiting for one of
	/// the "not empty" or "full" condition will receive a BrokenPipe
	/// exception.
	///
	/// \returns The number of producers remaining after this call.

	Uint32 decNbrProd(
	);

	/// \brief Returns the number of active producers.

	Uint32 getNbrProd(
	) const;

	/// \brief Tells this object that one more consumer has become active.
	///
	/// When a new SFifo is created, the number of active consumers is
	/// by default set
	/// to 1, so there is no need to call this method initially.

	void incNbrCons(
	);

	/// \brief Tells this object that one consumer is not active anymore.
	///
	/// When the number of active consumers reach 0,
	/// all threads waiting for one of the "not full" or "empty condition"
	/// (producers and
	/// observers) will receive a BrokenPipe exception.
	///
	/// \returns The number of consumers remaining after this call.

	Uint32 decNbrCons(
	);

	/// \brief Returns the number of active consumers.

	Uint32 getNbrCons(
	) const;

	/// \brief Returns true iff this FIFO is empty.

	Bool isEmpty(
	) const;

	/// \brief Returns true iff this FIFO contains at least one element.

	Bool isNotEmpty(
	) const;

	/// \brief Returns true iff this FIFO is full.

	Bool isFull(
	) const;

	/// \brief Blocks until this FIFO becomes empty.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitEmpty(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	becomes empty.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitEmpty(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Blocks until this FIFO contains at least one element.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitNotEmpty(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	contains at least one element.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitNotEmpty(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Blocks until this FIFO becomes full.
	///
	/// \throws WouldBlock
	///	If there is no maximum size set.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitFull(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	becomes full.
	///
	/// \throws WouldBlock
	///	If there is no maximum size set.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitFull(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Blocks until this FIFO is not full.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitNotFull(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	is not full.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitNotFull(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Returns the number of times the "empty" condition occured
	///	since this object was created.

	Uint64 getNbrEmpty(
	) const;

	/// \brief Returns the number of times the "full" condition occured
	///	since this object was created.

	Uint64 getNbrFull(
	) const;

	/// \brief Returns the number of entries contained in this FIFO.

	Uint32 getSize(
	) const;

	/// \brief Returns the max size of this FIFO, or 0 if no limit.

	Uint32 getMaxSize(
	) const;

	/// \brief Deletes all items contained in this FIFO.

	void kill(
	);

	/// \brief Returns the oldest element in this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SPtr top(
	) const;

	/// \brief Returns the oldest element in this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SPtr top(
		const	Time::Delta &	pTimeOut
	) const;

	/// \brief Returns the oldest element in this FIFO, or a NULL pointer
	///	if this FIFO is empty.

	SPtr tryTop(
	) const;

	/// \brief Returns the oldest element in this FIFO, and removes it.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SPtr pop(
	);

	/// \brief Returns the oldest element in this FIFO, and removes it.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SPtr pop(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Returns the oldest element in this FIFO, and removes it, or
	///	a NULL pointer if this FIFO is empty.

	SPtr tryPop(
	);

	/// \brief Appends a new element to this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and full.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void push(
		const	SPtr &		pElement
	);

	/// \brief Appends a new element to this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and full.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void push(
		const	SPtr &		pElement,
		const	Time::Delta &	pTimeOut
	);

protected :

	SFifoImpl< SPtr > *	impl;

private :

	SFifo(
		const	SFifo &
	);

	SFifo & operator = (
		const	SFifo &
	);

};

// ============================================================================

/// \brief A FIFO of SCPtr objects.
///
/// \ingroup BFC_MT

class BFC_MT_DLL SCFifo : virtual public SObject {

public :

	/// \brief Creates an empty FIFO.
	///
	/// This constructor creates an empty FIFO of SCPtr, whose maximum
	/// size is \a maxSize. If \a maxSize is 0, the FIFO can grow without
	/// limit.
	///
	/// All calls to blocking methods will generate a WouldBlock exception
	/// when they would block and the \a blockng flag has been set to
	/// false. Otherwise, they just block until some condition is met.
	///
	/// This FIFO also remembers how many producers and how many consumers
	/// are active at a time. The number of each type is initially set to
	/// \a nbrProd and \a nbrCons respectively (default is 1). Decreasing
	/// the number of producers or consumers is used to unblock waiting
	/// threads in some circumstances.
	///
	/// \param maxSize
	///	The maximum size of the FIFO. 0 means no limit.
	///
	/// \param blockng
	///
	/// \param nbrProd
	///
	/// \param nbrCons

	SCFifo(
		const	Uint32		maxSize = 0,
		const	Bool		blockng = false,
		const	Uint32		nbrProd = 1,
		const	Uint32		nbrCons = 1
	);

	/// \brief Destroys this FIFO, deleting any used resource.

	virtual ~SCFifo(
	);

	/// \brief Tells this object that one more producer has become active.
	///
	/// When a new SCFifo is created, the number of active producers is
	/// by default set
	/// to 1, so there is no need to call this method initially.

	void incNbrProd(
	);

	/// \brief Tells this object that one producer is not active anymore.
	///
	/// When the number of active producers reach 0,
	/// all consumers waiting for data and all threads waiting for one of
	/// the "not empty" or "full" condition will receive a BrokenPipe
	/// exception.
	///
	/// \returns The number of producers remaining after this call.

	Uint32 decNbrProd(
	);

	/// \brief Returns the number of active producers.

	Uint32 getNbrProd(
	) const;

	/// \brief Tells this object that one more consumer has become active.
	///
	/// When a new SCFifo is created, the number of active consumers is
	/// by default set
	/// to 1, so there is no need to call this method initially.

	void incNbrCons(
	);

	/// \brief Tells this object that one consumer is not active anymore.
	///
	/// When the number of active consumers reach 0,
	/// all threads waiting for one of the "not full" or "empty condition"
	/// (producers and
	/// observers) will receive a BrokenPipe exception.
	///
	/// \returns The number of consumers remaining after this call.

	Uint32 decNbrCons(
	);

	/// \brief Returns the number of active consumers.

	Uint32 getNbrCons(
	) const;

	/// \brief Returns true iff this FIFO is empty.

	Bool isEmpty(
	) const;

	/// \brief Returns true iff this FIFO contains at least one element.

	Bool isNotEmpty(
	) const;

	/// \brief Returns true iff this FIFO is full.

	Bool isFull(
	) const;

	/// \brief Blocks until this FIFO becomes empty.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitEmpty(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	becomes empty.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitEmpty(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Blocks until this FIFO contains at least one element.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitNotEmpty(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	contains at least one element.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitNotEmpty(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Blocks until this FIFO becomes full.
	///
	/// \throws WouldBlock
	///	If there is no maximum size set.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitFull(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	becomes full.
	///
	/// \throws WouldBlock
	///	If there is no maximum size set.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	void waitFull(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Blocks until this FIFO is not full.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitNotFull(
	);

	/// \brief Blocks for a maximum of time of \a pTimeOut until this FIFO
	///	is not full.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void waitNotFull(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Returns the number of times the "empty" condition occured
	///	since this object was created.

	Uint64 getNbrEmpty(
	) const;

	/// \brief Returns the number of times the "full" condition occured
	///	since this object was created.

	Uint64 getNbrFull(
	) const;

	/// \brief Returns the number of entries contained in this FIFO.

	Uint32 getSize(
	) const;

	/// \brief Returns the max size of this FIFO, or 0 if no limit.

	Uint32 getMaxSize(
	) const;

	/// \brief Deletes all items contained in this FIFO.

	void kill(
	);

	/// \brief Returns the oldest element in this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SCPtr top(
	) const;

	/// \brief Returns the oldest element in this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SCPtr top(
		const	Time::Delta &	pTimeOut
	) const;

	/// \brief Returns the oldest element in this FIFO, or a NULL pointer
	///	if this FIFO is empty.

	SCPtr tryTop(
	) const;

	/// \brief Returns the oldest element in this FIFO, and removes it.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SCPtr pop(
	);

	/// \brief Returns the oldest element in this FIFO, and removes it.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and empty.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of producers drops to 0 while waiting.

	SCPtr pop(
		const	Time::Delta &	pTimeOut
	);

	/// \brief Returns the oldest element in this FIFO, and removes it, or
	///	a NULL pointer if this FIFO is empty.

	SCPtr tryPop(
	);

	/// \brief Appends a new element to this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and full.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void push(
		const	SCPtr &		pElement
	);

	/// \brief Appends a new element to this FIFO.
	///
	/// \throws WouldBlock
	///	If this fifo is non blocking and full.
	///
	/// \throw TimeoutExpired
	///	If the timeout expired.
	///
	/// \throws BrokenPipe
	///	If the number of consumers drops to 0 while waiting.

	void push(
		const	SCPtr &		pElement,
		const	Time::Delta &	pTimeOut
	);

protected :

	SFifoImpl< SCPtr > *	impl;

private :

	SCFifo(
		const	SCFifo &
	);

	SCFifo & operator = (
		const	SCFifo &
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_SFifo_H_

// ============================================================================

