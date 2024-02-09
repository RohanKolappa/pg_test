// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "JBS".
// 
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	JBS.DirTraverser.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_DirTraverser_H_
#define _JBS_DirTraverser_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, DirTraverser )

} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.VFS.FSPath.h"
#include "BFC.TL.List.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE DirTraverser : virtual public BFC::DObject {

public :

	DirTraverser(
		const	BFC::FSPath&	startPath
	);

	virtual ~DirTraverser(
	);

	BFC::FSPath getNext(
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.DirTraverser" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

	BFC::List< BFC::FSPath >	paths;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DirTraverser(
	);

	DirTraverser(
		const	DirTraverser&
	);

	DirTraverser& operator = (
		const	DirTraverser&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_DirTraverser_H_

// ============================================================================

