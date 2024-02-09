// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::VFS".
//
// "BFC::VFS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::VFS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::VFS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.VFS.FSPath.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _BFC_VFS_FSPath_H_
#define _BFC_VFS_FSPath_H_

// ============================================================================

#include "BFC.VFS.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "BFC.TL.BufferArray.h"
#include "BFC.TL.List.h"

// ============================================================================

namespace BFC {

// ============================================================================

class FSPath;

// ============================================================================

/// \brief An Array of FSPath objects.

typedef Array< FSPath >	FSPathArray;

/// \brief A List of FSPath objects.

typedef List< FSPath >	FSPathList;

// ============================================================================

class BFC_VFS_DLL FSPath : virtual public DObject {

public :

	static const Buffer dot1;
	static const Buffer dot2;

	/// Returns the separator character for this platform (typically a '/'
	///	on Unix, a '\\' on Windows, etc).

	static Uchar getSeparator(
	);

	/// Returns the path separator character for this platform (typically a
	///	':' on Unix, a ';' on Windows, etc).

	static Uchar getPathSeparator(
	);

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

	Bool operator != (
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

	/// Returns the FSPath relative from source dir
	FSPath getRelative(
		const	FSPath&		source
	) const;

	/// Invalidate this path.

	void kill(
	);

	/// Converts all backslashes to slashes.

	static Buffer slashify(
		const	Buffer&		name
	);

	operator Buffer (
	) const {
		return toBuffer();
	}

	static FSPath getCurrentPath(
	);

	static FSPath getHomePath(
	);

	static FSPath getRootPath(
	);

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

} // namespace BFC

// ============================================================================

#endif // _BFC_VFS_FSPath_H_

// ============================================================================

