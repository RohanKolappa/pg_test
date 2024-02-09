// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "Sys::DL".
// 
// "Sys::DL" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "Sys::DL" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "Sys::DL"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	Sys.DL.DllLoader.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _Sys_DL_DllLoader_H_
#define _Sys_DL_DllLoader_H_

// ============================================================================

#include "Sys.DL.DLL.h"

// ============================================================================

namespace Sys {
namespace DL {

// ============================================================================

/// \brief Wrapper for the "dlopen" and "dlclose" function calls.
///
/// This class provides a default implementation (no profiler support).
///
/// \ingroup Sys_DL

class Sys_DL_DLL DllLoader {

public :

	/// \brief Creates a new DllLoader.

	DllLoader(
	);

	/// \brief Destroys this object.

	virtual ~DllLoader(
	);

	static DllLoader * instance(
	);

	class Data; // Defined in Sys.DL.DllLoader.Data.h

	/// \brief Opens the DLL named \a pFilename.
	///
	/// \returns
	///	A pointer to an opaque internal structure, to be used in
	///	subsequent calls to this object.
	///
	/// \param pFilename
	///
	/// \param pLoadLazy
	///	If true, performs lazy binding (only resolve symbols as the
	///	code that references them is executed). If not set, all
	///	undefined symbols in the library are resolved before open()
	///	returns. If this cannot be done, NULL is returned.
	///
	/// \param pLoadLocal
	///	If true (the default), symbols defined in the library are not made available
	///	to resolve references in subsequently loaded libraries. If
	///	false, the symbols defined by the library will be
	///	made available for symbol resolution of subsequently loaded
	///	libraries.

	virtual Data * open(
		const	char *		pFilename,
		const	bool		pLoadLazy = false,
		const	bool		pLoadLocal = true
	);

	/// \brief Closes the DLL referenced by \a pData.
	///
	/// \param pData
	///	Automatically destroyed and set to 0.

	virtual bool close(
			Data *&		pData
	);

	virtual char * error(
	);

	virtual void * sym(
		const	Data *		opaque,
		const	char *		symbol
	);

protected :

private :

	static DllLoader *	dllLoader;

	/// \brief Forbidden copy constructor.

	DllLoader(
		const	DllLoader&
	);

	/// \brief Forbidden copy operator.

	DllLoader& operator = (
		const	DllLoader&
	);

};

// ============================================================================

} // namespace DL
} // namespace Sys

// ============================================================================

#endif // _Sys_DL_DllLoader_H_

// ============================================================================

