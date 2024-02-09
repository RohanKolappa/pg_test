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
//	TBFC.VFS.FSDir.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _TBFC_Light_FSDir_H_
#define _TBFC_Light_FSDir_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"

#include "TBFC.TL.Array.h"

#include "TBFC.VFS.FSPath.h"

#include "TBFC.Debug.DObject.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// The FSDir class provides access to directory structures and their contents in a platform-independent way.

class TBFC_Light_DLL FSDir : virtual public DObject {

public :

	/// Constructs a new FSDir (not open, i.e. no visible content).

	FSDir(
	);

	/// Destroys this FSDir object.

	virtual ~FSDir(
	);

	/// Opens an existing directory.

	void open(
		const	FSPath&		path
	);

	void open(
		const	Buffer&		path
	);

	Bool isOpen(
	) const;

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

	const FSPath& getPath(
	) const;

	FSPath getAbsPath(
	) const;

	FSPath getEntryPath(
		const	Buffer&		entryName
	) const;

	void cd(
		const	Buffer&		dirName
	);

	Uint32 size(
	) const;

	const Buffer& operator [] (
		const	Uint32		i
	) const;

	const BufferArray& getAllEntries(
	) const;

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

	BASE_CLASS_GEN_EXCEPTION("TBFC.VFS.FSDir");

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

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_FSDir_H_

// ============================================================================

