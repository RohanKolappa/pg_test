// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::DOM".
// 
// "BFC::DOM" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::DOM" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::DOM"; if not, write to:
//			Free Software Foundation
//			59 Temple Place, Suite 330
//			Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.DOM.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_DLL_H_
#define _BFC_DOM_DLL_H_

// ============================================================================

/// \defgroup BFC_DOM BFC::DOM
///
/// XML DOM level 1 and DOM level 2 implementation module.

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

#if ! defined( BFC_DOM_DLL )
#  if defined( BFC_DOM_EXPORTS )
#    define BFC_DOM_DLL JBS_DLL_EXPORT
#  else
#    define BFC_DOM_DLL JBS_DLL_IMPORT
#  endif
#endif

// ============================================================================

// Now, act as an all-in-one header for this DLL.

//#include "AVI.RGB.IO.MyClass.h"

// ============================================================================

#endif // _BFC_DOM_DLL_H_

// ============================================================================

