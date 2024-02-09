// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::RegEx".
// 
// "BFC::RegEx" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::RegEx" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::RegEx"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.RegEx.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_RegEx_DLL_H_
#define _BFC_RegEx_DLL_H_

// ============================================================================

/// \defgroup BFC_RegEx BFC::RegEx (regex module)
///
/// \brief Classes for matching character sequences against patterns specified
///	by regular expressions.
///
/// An instance of the Pattern class represents a regular expression that is
/// specified in string form in a syntax similar to that used by Perl.
///
/// Instances of the Matcher class are used to match character sequences
/// against a given pattern.
///
/// An excellent tutorial and overview of regular expressions is
/// <a href="http://www.oreilly.com/catalog/regex/"><i>Mastering Regular
/// Expressions</i>, Jeffrey E. F. Friedl, O'Reilly and Associates, 1997</a>.

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

#if ! defined( BFC_RegEx_DLL )
#  if defined( BFC_RegEx_EXPORTS )
#    define BFC_RegEx_DLL JBS_DLL_EXPORT
#  else
#    define BFC_RegEx_DLL JBS_DLL_IMPORT
#  endif
#endif

// ============================================================================

#endif // _BFC_RegEx_DLL_H_

// ============================================================================

