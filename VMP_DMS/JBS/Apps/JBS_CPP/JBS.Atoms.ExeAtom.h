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
//	JBS.Atoms.ExeAtom.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Atoms_ExeAtom_H_
#define _JBS_Atoms_ExeAtom_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, ExeAtom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "JBS.Atoms.Atom.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

class JBS_DLL_NONE ExeAtom : public TempAtom {

public :

	ExeAtom(
			ProjectPtr	project,
		const	AtomList&	objList,
		const	AtomList&	depList,
		const	BFC::Bool	dbgMode,
		const	BFC::Bool	staMode
	);

	virtual ~ExeAtom(
	);

	const AtomList& getObjList(
	) const {
		return objList;
	}

	const AtomList& getDepList(
	) const {
		return depList;
	}

	BFC::Bool getStaMode(
	) const {
		return staMode;
	}

	BFC::Bool getDbgMode(
	) const {
		return dbgMode;
	}

	static const BFC::Buffer fileExt;

	virtual void doBuildLocalTarget(
	);

private :

	AtomList	objList;	// objects
	AtomList	depList;	// libraries
	BFC::Bool	staMode;
	BFC::Bool	dbgMode;

};

// ============================================================================

} // namespace Atoms
} // namespace JBS

// ============================================================================

#endif // _JBS_Atoms_ExeAtom_H_

// ============================================================================

