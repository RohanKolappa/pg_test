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
//	JBS.Atoms.ObjAtom.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _JBS_Atoms_ObjAtom_H_
#define _JBS_Atoms_ObjAtom_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, ObjAtom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "JBS.Atoms.Atom.h"
#include "JBS.Atoms.CppAtom.h"
#include "JBS.Atoms.DepAtom.h"
#include "JBS.Atoms.SbrAtom.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

class JBS_DLL_NONE ObjAtom : public TempAtom {

public :

	ObjAtom(
			AtomPtr		cppAtom,
			AtomPtr		depAtom,
		const	BFC::Bool	dbgMode
#if defined( PLATFORM_WIN32 )
		, const	BFC::Bool	staMode
#endif
	);

	virtual ~ObjAtom(
	);

	CppAtomPtr getCppAtom(
	) const {
		return cppAtom;
	}

	DepAtomPtr getDepAtom(
	) const {
		return depAtom;
	}

#if defined( PLATFORM_WIN32 )

	void setSbrAtom(
			SbrAtomPtr	sbrAtom
	) {
		this->sbrAtom = sbrAtom;
		this->sbrAtom.setMaster( false );
	}

	SbrAtomPtr getSbrAtom(
	) const {
		return sbrAtom;
	}

	BFC::Bool getStaticMode(
	) const {
		return staMode;
	}

#endif

	BFC::Bool getDebugMode(
	) const {
		return dbgMode;
	}

	static const BFC::Buffer fileExt;

	virtual void doBuildLocalTarget(
	);

private :

	CppAtomPtr		cppAtom;
	DepAtomPtr		depAtom;
#if defined( PLATFORM_WIN32 )
	SbrAtomPtr		sbrAtom;
	BFC::Bool		staMode;
#endif
	BFC::Bool		dbgMode;

};

// ============================================================================

} // namespace Atoms
} // namespace JBS

// ============================================================================

#endif // _JBS_Atoms_ObjAtom_H_

// ============================================================================

