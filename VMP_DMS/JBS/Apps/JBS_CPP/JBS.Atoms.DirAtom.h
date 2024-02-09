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
//	JBS.Atoms.DirAtom.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Atoms_DirAtom_H_
#define _JBS_Atoms_DirAtom_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Atoms {

BFC_PTR_DECL( JBS_DLL_NONE, DirAtom )

} // namespace Atoms
} // namespace JBS

// ============================================================================

#include "JBS.Atoms.Atom.h"

// ============================================================================

namespace JBS {
namespace Atoms {

// ============================================================================

/// \brief Manages a temp folder to hold all temp files.
///
/// This atom is responsible for creating and deleting the temporary folder
/// used to hold all temporary files.
///
/// This folder is created right under the Project's main folder, it's name is
/// given by "$(JBS_BUILD_SUBDIR)", and a file named "$(JBS_NOENTER_FILE)" is
/// automatically created in it.

class JBS_DLL_NONE DirAtom : public TempAtom {

public :

	DirAtom(
			ProjectPtr	proj
	);

	virtual ~DirAtom(
	);

	virtual void doBuildLocalTarget(
	);

	virtual void doCleanLocalTarget(
		const	BFC::Bool	distClean
	);

	virtual BFC::Time::Clock getBuildTime(
	) const;

protected :

private :

	BFC::Buffer	tempDirName;
	BFC::Buffer	noEnterName;

};

// ============================================================================

} // namespace Atoms
} // namespace JBS

// ============================================================================

#endif // _JBS_Atoms_DirAtom_H_

// ============================================================================

