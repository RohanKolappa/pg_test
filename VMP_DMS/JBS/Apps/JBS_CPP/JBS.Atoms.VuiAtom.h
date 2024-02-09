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
//	JBS.Atoms.VuiAtom.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Atoms_VuiAtom_H_
#define _JBS_Atoms_VuiAtom_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, VuiAtom )
BFC_PTR_DECL( JBS_DLL_NONE, Ui2VuiAtom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "JBS.Atoms.Atom.h"
#include "JBS.Atoms.UiAtom.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

class JBS_DLL_NONE VuiAtom : virtual public Atom {

public :

	VuiAtom(
	);

	virtual ~VuiAtom(
	);

	static const BFC::Buffer fileExt;

};

// ============================================================================

class JBS_DLL_NONE UserVuiAtom : public UserAtom, public VuiAtom {

public :

	UserVuiAtom(
			ProjectPtr	proj,
		const	BFC::FSPath&	path
	);

	virtual ~UserVuiAtom(
	);

};

// ============================================================================

class JBS_DLL_NONE Ui2VuiAtom : public TempAtom, public VuiAtom {

public :

	Ui2VuiAtom(
			AtomPtr		uiFile
	);

	virtual ~Ui2VuiAtom(
	);

	virtual void doBuildLocalTarget(
	);

	UiAtomPtr getUiAtom(
	) const {
		return uiAtom;
	}

private :

	UiAtomPtr	uiAtom;

};

// ============================================================================

} // namespace Atoms
} // namespace JBS

// ============================================================================

#endif // _JBS_Atoms_VuiAtom_H_

// ============================================================================

