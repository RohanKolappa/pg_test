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

#include "BFC.MT.Locker.h"
#include "BFC.MT.SFifo.h"
#include "BFC.MT.SFifoImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::MT::SFifo
// ============================================================================

BFC_PTR_IMPL_NS( MT, SFifo )

// ============================================================================

MT::SFifo::SFifo(
	const	Uint32		maxSize,
	const	Bool		blockng,
	const	Uint32		nbrProd,
	const	Uint32		nbrCons ) {

	impl = new SFifoImpl< SPtr >( maxSize, blockng, nbrProd, nbrCons );

}

MT::SFifo::~SFifo() {

	delete impl;

}

// ============================================================================

void MT::SFifo::incNbrProd() {

	impl->incNbrProd();

}

Uint32 MT::SFifo::decNbrProd() {

	return impl->decNbrProd();

}

Uint32 MT::SFifo::getNbrProd() const {

	return impl->getNbrProd();

}

// ============================================================================

void MT::SFifo::incNbrCons() {

	impl->incNbrCons();

}

Uint32 MT::SFifo::decNbrCons() {

	return impl->decNbrCons();

}

Uint32 MT::SFifo::getNbrCons() const {

	return impl->getNbrCons();

}

// ============================================================================

Bool MT::SFifo::isEmpty() const {

	return impl->isEmpty();

}

Bool MT::SFifo::isNotEmpty() const {

	return impl->isNotEmpty();

}

Bool MT::SFifo::isFull() const {

	return impl->isFull();

}

// ============================================================================

void MT::SFifo::waitEmpty() {

	impl->waitEmpty();

}

void MT::SFifo::waitEmpty(
	const	Time::Delta &	pTimeOut ) {

	impl->waitEmpty( pTimeOut );

}

void MT::SFifo::waitNotEmpty() {

	impl->waitNotEmpty();

}

void MT::SFifo::waitNotEmpty(
	const	Time::Delta &	pTimeOut ) {

	impl->waitNotEmpty( pTimeOut );

}

void MT::SFifo::waitFull() {

	impl->waitFull();

}

void MT::SFifo::waitFull(
	const	Time::Delta &	pTimeOut ) {

	impl->waitFull( pTimeOut );

}

void MT::SFifo::waitNotFull() {

	impl->waitNotFull();

}

void MT::SFifo::waitNotFull(
	const	Time::Delta &	pTimeOut ) {

	impl->waitNotFull( pTimeOut );

}

// ============================================================================

Uint64 MT::SFifo::getNbrEmpty() const {

	return impl->getNbrEmpty();

}

Uint64 MT::SFifo::getNbrFull() const {

	return impl->getNbrFull();

}

// ============================================================================

Uint32 MT::SFifo::getSize() const {

	return impl->getSize();

}

Uint32 MT::SFifo::getMaxSize() const {

	return impl->getMaxSize();

}

// ============================================================================

void MT::SFifo::kill() {

	impl->kill();

}

// ============================================================================

SPtr MT::SFifo::top() const {

	return impl->top();

}

SPtr MT::SFifo::tryTop() const {

	return impl->tryTop();

}

// ============================================================================

SPtr MT::SFifo::pop() {

	return impl->pop();

}

SPtr MT::SFifo::tryPop() {

	return impl->tryPop();

}

// ============================================================================

void MT::SFifo::push(
	const	SPtr &		t ) {

	impl->push( t );

}

// ============================================================================
// BFC::MT::SCFifo
// ============================================================================

BFC_PTR_IMPL_NS( MT, SCFifo )

// ============================================================================

MT::SCFifo::SCFifo(
	const	Uint32		maxSize,
	const	Bool		blockng,
	const	Uint32		nbrProd,
	const	Uint32		nbrCons ) {

	impl = new SFifoImpl< SCPtr >( maxSize, blockng, nbrProd, nbrCons );

}

MT::SCFifo::~SCFifo() {

	delete impl;

}

// ============================================================================

void MT::SCFifo::incNbrProd() {

	impl->incNbrProd();

}

Uint32 MT::SCFifo::decNbrProd() {

	return impl->decNbrProd();

}

Uint32 MT::SCFifo::getNbrProd() const {

	return impl->getNbrProd();

}

// ============================================================================

void MT::SCFifo::incNbrCons() {

	impl->incNbrCons();

}

Uint32 MT::SCFifo::decNbrCons() {

	return impl->decNbrCons();

}

Uint32 MT::SCFifo::getNbrCons() const {

	return impl->getNbrCons();

}

// ============================================================================

Bool MT::SCFifo::isEmpty() const {

	return impl->isEmpty();

}

Bool MT::SCFifo::isNotEmpty() const {

	return impl->isNotEmpty();

}

Bool MT::SCFifo::isFull() const {

	return impl->isFull();

}

// ============================================================================

void MT::SCFifo::waitEmpty() {

	impl->waitEmpty();

}

void MT::SCFifo::waitEmpty(
	const	Time::Delta &	pTimeOut ) {

	impl->waitEmpty( pTimeOut );

}

void MT::SCFifo::waitNotEmpty() {

	impl->waitNotEmpty();

}

void MT::SCFifo::waitNotEmpty(
	const	Time::Delta &	pTimeOut ) {

	impl->waitNotEmpty( pTimeOut );

}

void MT::SCFifo::waitFull() {

	impl->waitFull();

}

void MT::SCFifo::waitFull(
	const	Time::Delta &	pTimeOut ) {

	impl->waitFull( pTimeOut );

}

void MT::SCFifo::waitNotFull() {

	impl->waitNotFull();

}

void MT::SCFifo::waitNotFull(
	const	Time::Delta &	pTimeOut ) {

	impl->waitNotFull( pTimeOut );

}

// ============================================================================

Uint64 MT::SCFifo::getNbrEmpty() const {

	return impl->getNbrEmpty();

}

Uint64 MT::SCFifo::getNbrFull() const {

	return impl->getNbrFull();

}

// ============================================================================

Uint32 MT::SCFifo::getSize() const {

	return impl->getSize();

}

Uint32 MT::SCFifo::getMaxSize() const {

	return impl->getMaxSize();

}

// ============================================================================

void MT::SCFifo::kill() {

	impl->kill();

}

// ============================================================================

SCPtr MT::SCFifo::top() const {

	return impl->top();

}

SCPtr MT::SCFifo::tryTop() const {

	return impl->tryTop();

}

// ============================================================================

SCPtr MT::SCFifo::pop() {

	return impl->pop();

}

SCPtr MT::SCFifo::tryPop() {

	return impl->tryPop();

}

// ============================================================================

void MT::SCFifo::push(
	const	SCPtr &		t ) {

	impl->push( t );

}

// ============================================================================

