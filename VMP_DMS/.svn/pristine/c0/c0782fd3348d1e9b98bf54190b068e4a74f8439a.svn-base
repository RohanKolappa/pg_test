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
//	BFC.SysSemaphore.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_DARWIN )

// ============================================================================

#include <errno.h>
#include <string.h>

#include "BFC.MT.SysSemaphore.h"
#include "BFC.MT.Mutex.h"

// ============================================================================

using namespace BFC;

// ============================================================================

struct MT::SysSemaphoreData {
	Mutex			mutex;
	Mutex			delay;
	Uint32			value;
};

// ============================================================================

MT::SysSemaphore::SysSemaphore(
	const	Uint32		initialValue) {

	data = new SysSemaphoreData;
	data->value = initialValue;

}

MT::SysSemaphore::~SysSemaphore() {

	delete data;

}

// ============================================================================

void MT::SysSemaphore::wait() {

	data->delay.lock();
	data->mutex.lock();
	data->value--;
	if (data->value > 0)
		data->delay.unlock();
	data->mutex.unlock();

}

Bool MT::SysSemaphore::tryWait() {

	throw NotImplemented("SysSemaphore::tryWait on Darwin platform not yet implemented!!!");
	return false;
}

void MT::SysSemaphore::post() {

	data->mutex.lock();
	data->value++;
	if (data->value == 1)
		data->delay.unlock();
	data->mutex.unlock();

}

// ============================================================================

#endif // PLATFORM_DARWIN

// ============================================================================

