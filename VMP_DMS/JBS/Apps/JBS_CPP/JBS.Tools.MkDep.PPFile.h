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
//	PPFile.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_MkDep_PPFile_H_
#define _JBS_Tools_MkDep_PPFile_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Tools {
namespace MkDep {

// not using any namespace {

BFC_PTR_DECL(JBS_DLL_NONE,PPFile)

// } // not using any namespace
} // namespace MkDep
} // namespace Tools
} // namespace JBS

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.TL.List.h"
#include "BFC.Base.SObject.h"

#include "JBS.Tools.MkDep.Directive.h"

// ============================================================================

namespace JBS {
namespace Tools {
namespace MkDep {

// not using any namespace {

// ============================================================================

class JBS_DLL_NONE PPFile : virtual public BFC::SObject {

public :

	PPFile(
	);

	virtual ~PPFile(
	);

	BFC::Buffer			filename;
	BFC::Buffer			realname;
	DirectivesList			directives;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	PPFile(
		const	PPFile&
	);

	PPFile& operator = (
		const	PPFile&
	);

};

typedef BFC::List< PPFilePtr >		PPFileList;

// ============================================================================

// } // not using any namespace
} // namespace MkDep
} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_MkDep_PPFile_H_

// ============================================================================

