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
//	JBS.Atoms.HdrAtom.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Atoms_HdrAtom_H_
#define _JBS_Atoms_HdrAtom_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, HdrAtom )
BFC_PTR_DECL( JBS_DLL_NONE, VuiDeclHdrAtom )
BFC_PTR_DECL( JBS_DLL_NONE, UicDeclHdrAtom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "JBS.Atoms.Atom.h"
#include "JBS.Atoms.UiAtom.h"
#include "JBS.Atoms.VuiAtom.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

class JBS_DLL_NONE HdrAtom : virtual public Atom {

public :

	HdrAtom(
	);

	virtual ~HdrAtom(
	);

	static const BFC::Buffer fileExt;

private :

};

// ============================================================================

class JBS_DLL_NONE UserHdrAtom : public UserAtom, public HdrAtom {

public :

	UserHdrAtom(
			ProjectPtr	proj,
		const	BFC::FSPath&	path
	);

	virtual ~UserHdrAtom(
	);

private :

};

// ============================================================================

class JBS_DLL_NONE VuiDeclHdrAtom : public TempAtom, public HdrAtom {

public :

	VuiDeclHdrAtom(
			AtomPtr		vuiAtom
	);

	virtual ~VuiDeclHdrAtom(
	);

	virtual void doBuildLocalTarget(
	);

	VuiAtomPtr getVuiAtom(
	) const {
		return vuiAtom;
	}

private :

	VuiAtomPtr	vuiAtom;

};

// ============================================================================

class JBS_DLL_NONE UicDeclHdrAtom : public TempAtom, public HdrAtom {

public :

	UicDeclHdrAtom(
			AtomPtr		uiFile
	);

	virtual ~UicDeclHdrAtom(
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

#endif // _JBS_Atoms_HdrAtom_H_

// ============================================================================

