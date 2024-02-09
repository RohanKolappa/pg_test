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
//	Path.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_MkDep_Path_H_
#define _JBS_Tools_MkDep_Path_H_

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.TL.List.h"

// ============================================================================

namespace JBS {
namespace Tools {
namespace MkDep {

// not using any namespace {

// ============================================================================

typedef BFC::Buffer			Path;
typedef BFC::List< Path >		PathsList;

// ============================================================================

// } // not using any namespace
} // namespace MkDep
} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_MkDep_Path_H_

// ============================================================================

