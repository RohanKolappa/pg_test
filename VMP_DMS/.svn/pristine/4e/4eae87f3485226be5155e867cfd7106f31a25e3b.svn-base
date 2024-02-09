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
//	BFC.VFS.FSDir.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_VFS_FSDir_H_
#define _BFC_VFS_FSDir_H_

// ============================================================================

#include "BFC.VFS.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.Array.h"

#include "BFC.VFS.FSPath.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// The FSDir class provides access to directory structures and their contents in a platform-independent way.

class BFC_VFS_DLL FSDir : virtual public SObject {

public :

	/// Constructs a new FSDir (not open, i.e. no visible content).

	FSDir(
	);

	/// Opens an existing directory.

	void open(
		const	FSPath&		path
	);

	void open(
		const	Buffer&		path
	);

	Bool isOpen(
	) const {
		return ( path.isValid() );
	}

	void refresh(
	);

	void close(
	);

	Bool operator == (
		const	FSDir&
	) const;

	Bool operator != (
		const	FSDir&
	) const;

	const FSPath & getPath(
	) const {
		return path;
	}

	FSPath getAbsPath(
	) const;

	FSPath getEntryPath(
		const	Buffer&		entryName
	) const;

	void cd(
		const	Buffer&		dirName
	);

	Uint32 size(
	) const {
		return content.getSize();
	}

	Uint32 getSize(
	) const {
		return content.getSize();
	}

	const Buffer & operator [] (
		const	Uint32		pIndex
	) const {
		return content[ pIndex ];
	}

	const BufferArray & getAllEntries(
	) const {
		return content;
	}

	void create(
		const	Buffer&		fileName
	);

	void mkdir(
		const	Buffer&		dirName
	);

	void rmdir(
		const	Buffer&		dirName
	);

	void rm(
		const	Buffer&		fileName
	);

	void mv(
		const	Buffer&		oldName,
		const	Buffer&		newName
	);

	///

	static void setCurrentDir(
		const	FSPath&		path
	);

	static void setCurrentDir(
		const	Buffer&		path
	);

	///

	static Buffer getCurrentDir(
	);

	static Buffer getHomeDir(
	);

	static Buffer getRootDir(
	);

	static BufferArray getDrives(
	);

	BASE_CLASS_GEN_EXCEPTION("BFC.VFS.FSDir");

	CLASS_EXCEPTION(AlreadyOpen, "Already open");
	CLASS_EXCEPTION(CantChdir, "Can't change directory");
	CLASS_EXCEPTION(CantMkdir, "Can't make directory");
	CLASS_EXCEPTION(CantOpenDir, "Can't open dir");
	CLASS_EXCEPTION(CantRefreshDir, "Can't refresh dir");
	CLASS_EXCEPTION(CantRename, "Can't rename");
	CLASS_EXCEPTION(CantRmdir, "Can't remove directory");
	CLASS_EXCEPTION(CantTouch, "Can't touch file");
	CLASS_EXCEPTION(CantUnlink, "Can't unlink");
	CLASS_EXCEPTION(InvalidArgument, "Invalid argument");
	CLASS_EXCEPTION(InvalidDirName, "Invalid directory name");
	CLASS_EXCEPTION(NotOpen, "Not open");

protected :

private :

	// internal state...

	FSPath		path;
	BufferArray	content;

	// others...

	FSDir(
		const	FSDir&		o
	);

	FSDir& operator = (
		const	FSDir&		o
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_VFS_FSDir_H_

// ============================================================================

