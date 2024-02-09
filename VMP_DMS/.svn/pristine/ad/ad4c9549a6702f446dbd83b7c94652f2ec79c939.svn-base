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
//	BFC.Net.SignalIgnorer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if ! defined( PLATFORM_WIN32 )
#include <signal.h>
#endif

#include "BFC.Net.SignalIgnorer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Net::SignalIgnorer::SignalIgnorer() {

#if ! defined( PLATFORM_WIN32 )

	old_sigpipe = ::signal( SIGPIPE, SIG_IGN );

#endif

}

Net::SignalIgnorer::~SignalIgnorer() {

#if ! defined( PLATFORM_WIN32 )

	::signal( SIGPIPE, old_sigpipe );

#endif

}

// ============================================================================

