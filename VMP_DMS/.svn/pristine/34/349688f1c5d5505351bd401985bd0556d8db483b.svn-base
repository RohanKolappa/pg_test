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
//	JBS.Message.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _JBS_Message_H_
#define _JBS_Message_H_

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.VFS.FSPath.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE Message {

public :

	enum Type {
		Debug		= 0,
		Normal,
		Warning
	};

	enum Action {
		Building	= 0,
		Mocifying,
		Uicifying,
		Compiling,
		Linking_lib,
		Linking_exe,
		Gen_bsc,
		Stripping_exe,
		Gendepends,
		Gendefs,
		GenPacks,
		Vui_imp,
		Vui_dec,
		Ui_vui,
		Removing,
		Touching,
		Mkdir,
		Rmdir,
		Entering,
		Cleaning,
		Analyzing,
		MsgDebug,
		MsgInfo,
		MsgWarning,
		MsgError
	};

	Message(
		const	Action			action,
		const	BFC::FSPath&		path
	);

	Message(
		const	Action			action,
		const	BFC::Buffer&		message
	);

	virtual ~Message(
	);

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_Message_H_

// ============================================================================

