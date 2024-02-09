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
//	JBS.Atoms.DepAtom.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Atoms_DepAtom_H_
#define _JBS_Atoms_DepAtom_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, DepAtom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "JBS.Atoms.Atom.h"
#include "JBS.Atoms.CppAtom.h"
#include "JBS.Atoms.ListAtom.h"
#include "JBS.CppProject.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

class JBS_DLL_NONE DepAtom : public TempAtom {

public :

	DepAtom(
			AtomPtr		cppFile
	);

	virtual ~DepAtom(
	);

	static const BFC::Buffer fileExt;

	virtual BFC::Time::Clock getBuildTime(
	) const;

protected :

	virtual void doBuildLocalTarget(
	);

	void appendDefsFrom(
			BFC::BufferArray&	defs,
			CppProjectPtr		project
	) const;

	void appendDirsFrom(
			BFC::BufferArray&	dirs,
			CppProjectPtr		project
	) const;

	void saveFile(
	);

	void loadFile(
	);

	void populateDepsList(
		const	BFC::BufferArray&	hdrs
	);

	HdrAtomPtr resolveHdrAtom(
		const	BFC::Buffer&		hdrName,
		const	CppProjectList&		allProjects
	);

private :

	CppAtomPtr	cppAtom;
	ListAtomPtr	hdrList;	// equ. to the file content

};

// ============================================================================

} // namespace Atoms
} // namespace JBS

// ============================================================================

#endif // _JBS_Atoms_DepAtom_H_

// ============================================================================

