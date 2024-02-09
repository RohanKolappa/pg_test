// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Image".
// 
// "BFC::Image" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Image" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Image"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// FileName:
//	BFC.Image.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_DLL_H_
#define _BFC_Image_DLL_H_

// ============================================================================

/// \defgroup BFC_Image BFC::Image
///
/// \brief I2P Image Class library.

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

#if ! defined( BFC_Image_DLL )
#  if defined( BFC_Image_EXPORTS )
#    define BFC_Image_DLL JBS_DLL_EXPORT
#  else
#    define BFC_Image_DLL JBS_DLL_IMPORT
#  endif
#endif

namespace BFC {
namespace Image {
}

namespace Img = Image;

}

// ============================================================================

namespace BFC {
namespace Image {
}
namespace Img = Image;
}

// ============================================================================

// Now, act as an all-in-one header for this DLL.

//#include "BFC.Image.MyClass.h"

// ============================================================================

#endif // _BFC_Image_DLL_H_

// ============================================================================

