// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// FileName:
//	BFC.Plugins.File.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Plugins_File_H_
#define _BFC_Plugins_File_H_

// ============================================================================

#include "BFC.Plugins.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Plugins {

BFC_PTR_DECL( BFC_Plugins_DLL, File )

}
}

// ============================================================================

#include "Sys.DL.DllLoader.h"

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"

#include "BFC.Debug.DObject.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Plugins {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Plugins

class BFC_Plugins_DLL File : virtual public DObject {

public :

	File(
	);

	virtual ~File(
	);

	void setFileName(
		const	Buffer&			n
	);

	const Buffer& getFileName(
	) const;

	/// Loads a plugin.

	/// \throws EmptyFileName
	/// \throws AlreadyLoaded
	/// \throws CantOpenFile

	void load(
	);

	/// Unloads a plugin.

	/// \throws NotLoaded
	/// \throws CantCloseFile

	void unload(
	);

	Bool isLoaded(
	) const {
		return ( data != 0 );
	}

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("BFC.Plugins.File");

	CLASS_EXCEPTION(AlreadyLoaded, "Already loaded");
	CLASS_EXCEPTION(CantCloseFile, "Can't close file");
	CLASS_EXCEPTION(CantOpenFile, "Can't open file");
	CLASS_EXCEPTION(EmptyFileName, "Empty filename");
	CLASS_EXCEPTION(InternalError, "INTERNAL ERROR");
	CLASS_EXCEPTION(NotLoaded, "Not loaded");

protected :

private :

	// internal state...

	Buffer			name;	// filename
	Sys::DL::DllLoader::Data *	data;	// internal data

	// others...

	File(
		const	File&
	);

	File& operator = (
		const	File&
	);

};

// ============================================================================

typedef Array< FilePtr >	FileArray;

// ============================================================================

} // namespace Plugins
} // namespace BFC

// ============================================================================

#endif // _BFC_Plugins_File_H_

// ============================================================================

