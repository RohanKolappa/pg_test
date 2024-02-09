// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_DLL_H_
#define _VMP_UI_DLL_H_

// ============================================================================

/// \defgroup VMP_UI VMP::UI (abstract user interface module)
///
/// \brief Provides classes to build a (graphical) user interface.

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

#if ! defined( VMP_UI_DLL )
#  if defined( VMP_UI_EXPORTS )
#    define VMP_UI_DLL JBS_DLL_EXPORT
#  else
#    define VMP_UI_DLL JBS_DLL_IMPORT
#  endif
#endif

// ============================================================================

#endif // _VMP_UI_DLL_H_

// ============================================================================

