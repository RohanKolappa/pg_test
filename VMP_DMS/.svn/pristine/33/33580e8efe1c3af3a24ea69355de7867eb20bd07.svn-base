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
//	JBS.Atoms.CppAtom.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Atoms_CppAtom_H_
#define _JBS_Atoms_CppAtom_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, CppAtom )
BFC_PTR_DECL( JBS_DLL_NONE, MocCppAtom )
BFC_PTR_DECL( JBS_DLL_NONE, VuiImplCppAtom )
BFC_PTR_DECL( JBS_DLL_NONE, UicImplCppAtom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "JBS.Atoms.Atom.h"
#include "JBS.Atoms.HdrAtom.h"
#include "JBS.Atoms.UiAtom.h"
#include "JBS.Atoms.VuiAtom.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

class JBS_DLL_NONE CppAtom : virtual public Atom {

public :

	CppAtom(
	);

	virtual ~CppAtom(
	);

	static const BFC::Buffer fileExt;

};

// ============================================================================

class JBS_DLL_NONE UserCppAtom : public UserAtom, public CppAtom {

public :

	UserCppAtom(
			ProjectPtr	proj,
		const	BFC::FSPath&	path
	);

	virtual ~UserCppAtom(
	);

};

// ============================================================================

class JBS_DLL_NONE MocCppAtom : public TempAtom, public CppAtom {

public :

	MocCppAtom(
			AtomPtr		hdrFile
	);

	virtual ~MocCppAtom(
	);

	HdrAtomPtr getHdrAtom(
	) const {
		return hdrAtom;
	}

	virtual void doBuildLocalTarget(
	);

private :

	HdrAtomPtr	hdrAtom;

};

// ============================================================================

class JBS_DLL_NONE VuiImplCppAtom : public TempAtom, public CppAtom {

public :

	VuiImplCppAtom(
			AtomPtr		vuiAtom,
			AtomPtr		hdrAtom
	);

	virtual ~VuiImplCppAtom(
	);

	VuiAtomPtr getVuiAtom(
	) const {
		return vuiAtom;
	}

	HdrAtomPtr getHdrAtom(
	) const {
		return hdrAtom;
	}

	virtual void doBuildLocalTarget(
	);

private :

	VuiAtomPtr	vuiAtom;
	HdrAtomPtr	hdrAtom;

};

// ============================================================================

class JBS_DLL_NONE UicImplCppAtom : public TempAtom, public CppAtom {

public :

	UicImplCppAtom(
			AtomPtr		uiFile,
			AtomPtr		hdrFile
	);

	virtual ~UicImplCppAtom(
	);

	UiAtomPtr getUiAtom(
	) const {
		return uiAtom;
	}

	HdrAtomPtr getHdrAtom(
	) const {
		return hdrAtom;
	}

	virtual void doBuildLocalTarget(
	);

private :

	UiAtomPtr	uiAtom;
	HdrAtomPtr	hdrAtom;

};

// ============================================================================

} // namespace Atoms
} // namespace JBS

// ============================================================================

#endif // _JBS_Atoms_CppAtom_H_

// ============================================================================

