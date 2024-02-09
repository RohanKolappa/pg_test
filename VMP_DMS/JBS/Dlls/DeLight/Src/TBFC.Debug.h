// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Debug".
// 
// "TBFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// FileName:
//	TBFC.Debug.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_H_
#define _TBFC_Light_H_

// ============================================================================

#include "TBFC.Debug.ColoredBuffer.h"
#include "TBFC.Debug.Console.h"
#include "TBFC.Debug.ConsoleColor.h"
#include "TBFC.Debug.DObject.h"
#include "TBFC.Debug.Debugger.h"

// ============================================================================

/// \defgroup TBFC_Light TBFC::Debug (debugging facilities module)
///
/// \brief Debug-oriented manipulation classes and functions.
///
/// This group provides classes, functions, and constants which help handle
/// and manipulate debugging information.
///
/// Example:
///
/// \code
///	#include "TBFC.Base.Buffer.h"
///	#include "TBFC.Base.Exception.h"
///
///	#include "TBFC.Debug.DObject.h"
///
///	class MyClass : virtual public TBFC::DObject {
///	public :
///		MyClass( const TBFC::Buffer & b ) {
///			setObjectName( "MyClass[" + b + "]" );
///		}
///		void someMethod() {
///			msgDbg( "someMethod(): Hello, World!" );
///			try {
///				...
///			}
///			catch ( TBFC::Exception & e ) {
///				msgExc( e, "someMethod(): Failed to ...!" );
///			}
///		}
///	};
/// \endcode

// ============================================================================

#endif // _TBFC_Light_H_

// ============================================================================

