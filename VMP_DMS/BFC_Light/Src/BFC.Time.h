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
// FileName:
//	BFC.Time.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Time_H_
#define _BFC_Time_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Time.Clock.h"
#include "BFC.Time.Chrono.h"
#include "BFC.Time.Delta.h"

// ============================================================================

/// \defgroup BFC_Time BFC::Time (Date/time support module)
///
/// \ingroup BFC_Light
///
/// \brief Time-oriented manipulation classes and functions.
///
/// This group provides classes, functions, and constants which help handle
/// and manipulate date and time oriented data.
///
/// Example:
///
/// \code
///	#include <iostream>
///
///	#include "BFC.Time.h"
///
///	using namespace std;
///	using namespace BFC;
///
///	int main() {
///
///		Time::Chrono	chrono;
///		Time::Delta	toWait = Time::OneSecond * 5;
///		Time::Clock	limit = Time::now() + toWait;
///
///		chrono.start();
///		limit.waitUntilReached();
///		chrono.stop();
///
///		cerr << "Elapsed: " << ( Int64 )chrono.getElapsed() << endl;
///
///		return 0;
///
///	}
/// \endcode
///
/// Take a look at the TimeTester.cpp test program for a slightly more
/// complex example.

// ============================================================================

#endif // _BFC_Time_H_

// ============================================================================

