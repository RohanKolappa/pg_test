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
//	BFC.Base.BufferBuilder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Kester Aernoudt
// 
// ============================================================================

#include "BFC.Base.BufferBuilder.h"

#include <stdio.h>

// ============================================================================

using namespace BFC;

// ============================================================================

BufferBuilder::BufferBuilder() {



}

BufferBuilder::~BufferBuilder() {



}

// ============================================================================

/* static */
Buffer BufferBuilder::format(const Char *format, ...) {
	va_list argptr;
	va_start(argptr, format);

	Buffer s;
	s = formatV(format, argptr);

	va_end(argptr);

	return s;
}

/* static */
Buffer BufferBuilder::formatV(const Char *format, va_list argptr) {
	Buffer s;

	Int32 size = 1024;

	for ( ;; ) {
		s.resize(size+1);
		
		// vsnprintf() may modify the original arg pointer, so pass it
		// only a copy
		va_list argptrcopy;
#ifdef va_copy
		va_copy(argptrcopy, argptr);
#else
		argptrcopy = argptr;
#endif
		Int32 len = vsnprintf((char*)s.getVarPtr(), size, (const char*)format, argptrcopy);
		va_end(argptrcopy);

		// vsnprintf() may return either -1 (traditional Unix behaviour) or the
		// total number of characters which would have been written if the
		// buffer were large enough (newer standards such as Unix98)
		if ( len >= 0 && len <= size )
		{
			// ok, there was enough space
			s.resize(len+1);
			break;
		}

		// still not enough, double it again
		size *= 2;
	}
	return s;
}

// ============================================================================

