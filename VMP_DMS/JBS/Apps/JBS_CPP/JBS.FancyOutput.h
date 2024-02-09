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
//	JBS.FancyOutput.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_FancyOutput_H_
#define _JBS_FancyOutput_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {

BFC_PTR_DECL( JBS_DLL_NONE, FancyOutput )

} // namespace JBS

// ============================================================================

#include "BFC.Base.SObject.h"
#include "BFC.Debug.ConsoleColor.h"

#include "JBS.Message.h"

// ============================================================================

namespace JBS {

// ============================================================================

class JBS_DLL_NONE FancyOutput : virtual public BFC::SObject {

public :

	FancyOutput(
	);

	virtual ~FancyOutput(
	);

	void msg(
		const	BFC::Buffer&		msg1,
		const	BFC::Buffer&		msg2,
		const	BFC::ConsoleColor	col1Fg = BFC::DefaultForeground,
		const	BFC::ConsoleColor	col1Bg = BFC::DefaultBackground,
		const	BFC::ConsoleColor	col2Fg = BFC::DefaultForeground,
		const	BFC::ConsoleColor	col2Bg = BFC::DefaultBackground,
		const	Message::Type		prio = Message::Normal
	) const;

};

// ============================================================================

} // namespace JBS

// ============================================================================

#endif // _JBS_FancyOutput_H_

// ============================================================================

