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
// FileName:
//	TBFC.Time.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Time_H_
#define _TBFC_Time_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Time.Clock.h"
#include "TBFC.Time.Chrono.h"
#include "TBFC.Time.Delta.h"

// ============================================================================

/// \defgroup TBFC_Time TBFC::Time (date/time support module)
///
/// \ingroup TBFC_Light
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
///	#include "TBFC.Time.h"
///
///	using namespace std;
///	using namespace TBFC;
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

#endif // _TBFC_Time_H_

// ============================================================================

