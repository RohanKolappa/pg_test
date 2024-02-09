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
//	TBFC.VFS.FSPath.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _TBFC_Light_FSPath_H_
#define _TBFC_Light_FSPath_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"

#include "TBFC.TL.BufferArray.h"
#include "TBFC.TL.List.h"

#include "TBFC.Debug.DObject.h"

// ============================================================================

namespace TBFC {

// ============================================================================

class FSPath;

// ============================================================================

/// \brief An Array of FSPath objects.

typedef Array< FSPath >	FSPathArray;

/// \brief A List of FSPath objects.

typedef List< FSPath >	FSPathList;

// ============================================================================

class TBFC_Light_DLL FSPath : virtual public DObject {

public :

	static const Buffer dot1;
	static const Buffer dot2;

	FSPath(
	);

	FSPath(
		const	Buffer&
	);

	FSPath(
		const	FSPath&
	);

	virtual ~FSPath(
	);

	/// Initializes this object with path \a path.

	FSPath& operator = (
		const	FSPath&		path
	);

	Bool operator == (
		const	FSPath&		path
	) const;

	/// \brief
	///	Returns the FSPath obtained by concatenating the current path,
	///	and component \a component.

	FSPath operator + (
		const	Buffer&		component
	) const;

	/// \brief
	///	Returns the FSPath obtained by concatenating the current path,
	///	and the relative path \a path.

	FSPath operator + (
		const	FSPath&		subpath
	) const;

	/// Initializes this object with path \a path.

	void set(
		const	Buffer&		path
	);

	/// Appends component \a cmpt to this object.

	void append(
		const	Buffer&		cmpt
	);

	/// Appends the __relative__path \a path to this object.

	void append(
		const	FSPath&		path
	);

	/// Removes last component from this object.

	void removeLast(
	);

	/// Checks whether or not this path is valid (initialized).

	Bool isValid(
	) const;

	/// Checks if this object represents a relative path.

	Bool isRelative(
	) const;

	/// Checks if this object represents an absolute path.

	Bool isAbsolute(
	) const;

	/// Converts this object to an absolute path, if necessary.

	void forceAbsolute(
	);

	/// Returns the number of path components.

	Uint32 size(
	) const;

	/// Checks if source is a subdir of this path.

	Bool isParentDirOf(
		const	FSPath&		source
	) const;

	/// Returns component \a index.

	const Buffer& operator [] (
		const	Uint32		index
	) const;

	/// Returns the last component.

	const Buffer& getLastComponent(
	) const;

	/// Replaces the last component by \a component.

	void replaceLastComponent(
		const	Buffer&		component
	);

	/// Returns the dir path (everything but the last component).

	FSPath getDirPath(
	) const;

	/// Returns the first \a nbr components as a new FSPath.

	FSPath getFirst(
		const	Uint32		nbr
	) const;

	/// Returns the last \a nbr components as a new FSPath.

	FSPath getLast(
		const	Uint32		nbr
	) const;

	/// Returns the base name of the last component.

	Buffer getBaseName(
		const	Buffer&		expectedExtension
	) const;

	/// Returns the base name of the last component.

	Buffer getBaseName(
	) const;

	/// Returns full path (relative of absolute).

	const Buffer & toBuffer(
	) const;

// KAE not tested yet
#if 0 
	/// Returns the FSPath relative from source dir
	FSPath getRelative(
		const	FSPath&		source
	) const;
#endif // KAE not tested yet

	/// Invalidate this path.

	void kill(
	);

	/// Returns the path separator character for this platform.

	static Uchar getSeparator(
	);

	/// Converts all backslashes to slashes.

	static Buffer slashify(
		const	Buffer&		name
	);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("TBFC.VFS.FSPath");

	CLASS_EXCEPTION(InvalidArgument, "Invalid argument");
	CLASS_EXCEPTION(NotValid, "Not valid");

protected :

	static Bool isLetter(
		const	Uchar
	);

	static Uchar toUpper(
		const	Uchar
	);

	void compactComponents(
	);

	void computeFullPath(
	);

private :

	// internal state...

	Bool		valid;
	Bool		relative;
#if defined( PLATFORM_WIN32 )
	Uchar		driveLetter;	// if ! rel && server.isEmpty()
	Buffer		serverName;	// if ! rel
	Buffer		shareName;	// if ! rel
#endif
	BufferArray	components;	// from root if ! rel
	Buffer		fullPath;	// cache...

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_FSPath_H_

// ============================================================================

