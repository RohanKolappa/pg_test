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
//	BFC.MT.TaskletRunner.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.TaskletRunner.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( MT, TaskletRunner )

// ============================================================================

MT::TaskletRunner::TaskletRunner(
		TaskletPtr	pTask ) :

	task( pTask ) {

	setObjectName( task->getObjectName() );

}

// ============================================================================

void MT::TaskletRunner::run() {

//	msgDbg( "run(): --->" );

	try {
		task->doInit();
		task->doRun();
	}
	catch ( ThreadStopped & ) {
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't run Tasklet!" );
	}

//	msgDbg( "run(): <---" );

}

void MT::TaskletRunner::abort() {

	try {
		task->doStop();
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't abort Tasklet!" );
	}

}

// ============================================================================

