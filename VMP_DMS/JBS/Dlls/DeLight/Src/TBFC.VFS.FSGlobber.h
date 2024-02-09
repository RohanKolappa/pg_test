// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "TBFC::VFS".
//
// "TBFC::VFS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "TBFC::VFS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::VFS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	TBFC.VFS.FSGlobber.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _TBFC_Light_FSGlobber_H_
#define _TBFC_Light_FSGlobber_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.SPtr.h"

// ============================================================================

namespace TBFC {

TBFC_PTR_DECL( TBFC_Light_DLL, FSGlobber )

} // namespace TBFC

// ============================================================================

#include "TBFC.TL.Array.h"
#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.VFS.FSPath.h"
#include "TBFC.Base.SObject.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Finds pathnames matching a pattern.

class TBFC_Light_DLL FSGlobber : virtual public SObject {

public :

	FSGlobber(
	);

	virtual ~FSGlobber(
	);

	void clearMatches(
	);

	/// \brief Searches for pathnames matching \a pattern.
	///
	/// This method searches for all the pathnames matching \a pattern,
	/// according to the rules used by the shell.
	///
	/// No tilde expansion or parameter substitution is done.
	///
	/// The pathnames that were found are appended to any previous
	/// result in the internal list of pathnames.
	///
	/// \returns The number of new matches found.

	Uint32 findMatches(
		const	Buffer&		pattern
	);

	/// \brief Searches for pathnames matching \a pattern.
	///
	/// This method searches for all the pathnames matching \a pattern,
	/// according to the rules used by the shell, and inserts them at the
	/// end of output parameter \a results.
	///
	/// No tilde expansion or parameter substitution is done.
	///
	/// \returns The number of new matches found.

	static Uint32 findMatches(
		const	Buffer&		pattern,
			FSPathList&	results
	);

	/// \brief Returns the number of pathnames already found.

	Uint32 getNumMatches(
	) const;

	/// \brief Iterator type, used to consult the list of results.

	typedef FSPathList::CstIterator	MatchIterator;

	/// \brief Returns the first pathname in the internal list.

	MatchIterator firstElt(
	) const {
		return matches.firstElt();
	}

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "TBFC.VFS.FSGlobber" );

	CLASS_EXCEPTION( CantFindMatches, "Can't find matches" );

protected :

#if defined( PLATFORM_WIN32 )

	static Uint32 findMatches(
		const	FSPath&		pattern,
		const	Uint32		startExpandAt,
			FSPathList&	matches
	);

#endif

private :

	FSPathList	matches;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	FSGlobber(
		const	FSGlobber&
	);

	FSGlobber& operator = (
		const	FSGlobber&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_FSGlobber_H_

// ============================================================================

