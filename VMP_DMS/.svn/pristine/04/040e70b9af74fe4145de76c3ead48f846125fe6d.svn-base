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
//	TBFC.VFS.FSNode.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _TBFC_Light_FSNode_H_
#define _TBFC_Light_FSNode_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"

#include "TBFC.Time.Clock.h"

#include "TBFC.VFS.FSPath.h"

#include "TBFC.Debug.DObject.h"

// ============================================================================

namespace TBFC {

// ============================================================================

struct FSNodeData;

// ============================================================================

/// \brief The FSNode class provides system-independent file information.
///
/// An FSNode object represents an existing, or not, element of a filesystem.

class TBFC_Light_DLL FSNode : virtual public DObject {

public:

	/// \brief Creates a new FSNode not associated to any path.

	FSNode(
	);

	/// \brief Creates a new FSNode associated to path \a path.

	FSNode(
		const	FSPath&		path
	);

	/// \brief Creates a new FSNode associated to the same path as \a node.

	FSNode(
		const	FSNode&		node
	);

	/// \brief Destroys this object.

	virtual ~FSNode(
	);

	/// \brief Associates this FSNode to the same path as \a node.

	FSNode& operator = (
		const	FSNode&		node
	);

	/// \brief Associates this FSNode to path \a name.

	void setPath(
		const	FSPath&		name
	);

	/// \brief Returns the path associated to this object.

	const FSPath& getPath(
	) const;

	/// \brief Returns true if the filesystem contains an element at the
	///	location specified by the path associated to this object.

	Bool exists(
	) const;

	/// \brief Rescans the filesystem at the location specified by the
	///	path associated to this object.

	void refresh(
	);

	/// \brief Returns true if this object represents a readable filesystem
	///	entry.

	Bool isReadable(
	) const;

	/// \brief Returns true if this object represents a writable filesystem
	///	entry.

	Bool isWritable(
	) const;

	/// \brief Returns true if this object represents an executable filesystem
	///	entry.

	Bool isExecutable(
	) const;

	/// \brief Returns true if this object represents a file.

	Bool isFile(
	) const;

	/// \brief Returns true if this object represents a directory.

	Bool isDir(
	) const;

	/// \brief Returns true if this object represents a symbolic link.

	Bool isSymLink(
	) const;

	/// \brief Returns the location pointed to by this symbolic link.

	Buffer readLink(
	) const;

	/// \brief Destroys the filesystem entry corresponding to this object.
	///
	/// If this object represents a file or a symbolic link, the
	/// corresponding filesystem entry is removed.
	///
	/// If this object represents a directory, the corresponding filesystem
	/// folder is first purged, then destroyed. Thus, the effect is the
	/// same as using the following command : "rm -fr <path>"!

	void remove(
	);

	/// \brief Returns the time of last access.
	///
	/// This value is changed by file accesses (read, ...).

	Time::Clock getLastAccessTime(
	) const;

	/// \brief Returns the time of last modification.
	///
	/// This value is changed by file modifications (create, write).
	/// Moreover, the value for a directory is changed by the creation or
	/// deletion of files in that directory.
	///
	/// This value is not changed for changes in owner, group, hard link
	/// count, or mode.

	Time::Clock getLastModifTime(
	) const;

	/// \brief Returns the time of last access.
	///
	/// This value is changed by writing or by setting inode information
	/// (i.e., owner, group, link count, mode, etc.).

	Time::Clock getLastChangeTime(
	) const;

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("TBFC.VFS.FSNode");

	CLASS_EXCEPTION(CantRemove, "Can't remove");
	CLASS_EXCEPTION(InvalidArgument, "Invalid argument");
	CLASS_EXCEPTION(NotAFile, "Not a file");

protected :

	void doCheck(
	);

private:

	FSNodeData	*data;	// ex: fic

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_FSNode_H_

// ============================================================================

