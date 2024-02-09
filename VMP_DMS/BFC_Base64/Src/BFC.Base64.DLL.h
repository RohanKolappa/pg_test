// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Base64".
// 
// "BFC::Base64" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Base64" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Base64"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// FileName:
//	BFC.Base64.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base64_DLL_H_
#define _BFC_Base64_DLL_H_

// ============================================================================

// Doxygen group definition. All classes in this component should be made
// members of this group by using an \ingroup tag in the class documentation.

/// \defgroup BFC_Base64 BFC::Base64 (base64 support module)
///
/// \brief [[[short description]]].

// +---------------+---------------+---------------+
// |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
// +---------------+---------------+---------------+
//  \_________/ \_________/ \_________/ \_________/
//
//       |_          |_____       |_________ |______________
//         |               |                |               |
//   _____________   _____________   _____________   _____________
//  /             \ /             \ /             \ /             \ .
// +---------------+---------------+---------------+---------------+
// |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
// +---------------+---------------+---------------+---------------+

// ============================================================================

// Some preliminary definitions.
// Those used to be provided by the BFC.Base.Types.h header file. They are now
// replicated all over the place :-)

#if ! defined( JBS_DLL_DEFS )
#  define JBS_DLL_DEFS
#  define JBS_DLL_NONE
#  if defined( PLATFORM_WIN32 )
#    define JBS_DLL_EXPORT __declspec( dllexport )
#    define JBS_DLL_IMPORT __declspec( dllimport )
#  else
#    define JBS_DLL_EXPORT
#    define JBS_DLL_IMPORT
#  endif
#endif

// [KOMA 2006-10-09: smarter in/export handling on Win32]

#if ! defined( BFC_Base64_DLL )
#  if defined( BFC_Base64_EXPORTS )
#    define BFC_Base64_DLL JBS_DLL_EXPORT
#  else
#    define BFC_Base64_DLL JBS_DLL_IMPORT
#  endif
#endif

// ============================================================================

// Now, act as an all-in-one header for this DLL.

//#include "AVI.RGB.IO.MyClass.h"

// ============================================================================

#endif // _BFC_Base64_DLL_H_

// ============================================================================

