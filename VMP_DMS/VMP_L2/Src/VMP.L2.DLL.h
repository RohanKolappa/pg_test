// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_L2_DLL_H_
#define _VMP_L2_DLL_H_

// ============================================================================

/// \defgroup VMP_L2 VMP::L2 (engines layer module).
///
/// \brief Provides abstract L2 engines.

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

#if ! defined( VMP_L2_DLL )
#  if defined( VMP_L2_EXPORTS )
#    define VMP_L2_DLL JBS_DLL_EXPORT
#  else
#    define VMP_L2_DLL JBS_DLL_IMPORT
#  endif
#endif

// ============================================================================

#endif // _VMP_L2_DLL_H_

// ============================================================================

