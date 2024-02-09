// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Sound.Engine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Sound_Engine_H_
#define _BFC_Sound_Engine_H_

// ============================================================================

#include "BFC.Sound.DLL.h"

// ============================================================================

#include "BFC.Sound.EnginePtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Sound.Block.h"
#include "BFC.Sound.Format.h"

// ============================================================================

namespace BFC {
namespace Sound {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Sound

class BFC_Sound_DLL Engine : virtual public DObject {

public :

	virtual FormatCPtr processFormat(
			FormatCPtr	pFormat
	) = 0;

	virtual BlockCPtr processBlock(
			BlockCPtr	pBlock
	) = 0;

};

// ============================================================================

} // namespace Sound
} // namespace BFC

// ============================================================================

#endif // _BFC_Sound_Engine_H_

// ============================================================================

