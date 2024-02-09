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
//	JBS.ListUtils.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_ListUtils_H_
#define _JBS_ListUtils_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, ListUtils )

} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.VFS.FSPath.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE ListUtils : virtual public BFC::SObject {

public :

	ListUtils(
	);

	virtual ~ListUtils(
	);

	static void keepExtension(
		const	BFC::Buffer&		extension,
			BFC::FSPathList&	list
	);

	static void appendSubstExt(
			BFC::FSPathList&	listToAugment,
		const	BFC::Buffer&		initialExtension,
		const	BFC::Buffer&		finalExtension,
		const	BFC::FSPathList&	listToAdd
	);

	static void filterOut(
		const	BFC::FSPathList&	toRemove,
			BFC::FSPathList&	toFilter
	);

	static void join(
			BFC::FSPathList&	result,
		const	BFC::FSPathList&	in1,
		const	BFC::FSPathList&	in2
	);

	static void removeDuplicates(
			BFC::FSPathList&	list
	);

	static void dump(
		const	BFC::FSPathList&	in
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.ListUtils" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	ListUtils(
		const	ListUtils&
	);

	ListUtils& operator = (
		const	ListUtils&
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_ListUtils_H_

// ============================================================================

