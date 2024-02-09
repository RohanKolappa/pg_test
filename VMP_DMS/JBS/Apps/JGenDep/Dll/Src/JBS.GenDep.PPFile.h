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
//	JBS.GenDep.PPFile.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _PPFile_H_
#define _PPFile_H_

// ============================================================================

#include "JBS.GenDep.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace GenDep {

TBFC_PTR_DECL(JBS_GenDep_DLL,PPFile)

} // namespace GenDep
} // namespace JBS

// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.TL.List.h"
#include "TBFC.Base.SObject.h"

#include "JBS.GenDep.Directive.h"

// ============================================================================

namespace JBS {
namespace GenDep {

// ============================================================================

class JBS_GenDep_DLL PPFile : virtual public TBFC::SObject {

public :

	PPFile(
	);

	virtual ~PPFile(
	);

	TBFC::Buffer			filename;
	TBFC::Buffer			realname;
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

typedef TBFC::List< PPFilePtr >		PPFileList;

// ============================================================================

} // namespace GenDep
} // namespace JBS

// ============================================================================

#endif // _PPFile_H_

// ============================================================================

