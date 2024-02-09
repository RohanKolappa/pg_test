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
//	JBS.Tools.Linker.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_Linker_H_
#define _JBS_Tools_Linker_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Tools {

BFC_PTR_DECL( JBS_DLL_NONE, Linker )

} // namespace Tools
} // namespace JBS

// ============================================================================

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"

#include "JBS.Atoms.ExeAtom.h"
#include "JBS.Atoms.LibAtom.h"

// ============================================================================

namespace JBS {
namespace Tools {

// ============================================================================

class JBS_DLL_NONE Linker : virtual public BFC::DObject {

public :

	Linker(
	);

	virtual ~Linker(
	);

	void link(
			Atoms::LibAtomPtr		libAtom
	) const;

	void link(
			Atoms::ExeAtomPtr		exeAtom
	) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.Tools.Linker" );

	CLASS_EXCEPTION( Failed, "Failed" );

protected :

	// <linker_name>	link.exe		g++
	// <flags>		@			@
	// <objects>		@			@
	// <dirs>		/libpath:"@"		-L @
	// <libs>		@.lib			-l @
	// <output>		/out:"@"		-o @
	// [ <tempfile> ]	/implib:"@"		n/a

	BFC::Buffer buildString(
		const	BFC::BufferArray&	flags,
		const	Atoms::AtomList&	objects,
		const	BFC::FSPathList&	dirs,
		const	BFC::BufferArray&	libs,
		const	BFC::Buffer&		output
#if defined( PLATFORM_WIN32 )
		,const	BFC::Buffer&		tempfile
#endif
	) const;

	void execute(
		const	BFC::Buffer&		string
	) const;

private :

	BFC::Buffer		JBS_LINKER;

	BFC::BufferArray	flgs_glb;
	BFC::BufferArray	flgs_dbg;
	BFC::BufferArray	flgs_rel;
	BFC::BufferArray	flgs_sta;
	BFC::BufferArray	flgs_sha;
	BFC::BufferArray	libs_glb;

};

// ============================================================================

} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_Linker_H_

// ============================================================================

