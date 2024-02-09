// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Crypto".
// 
// "BFC::Crypto" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Crypto" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Crypto"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// FileName:
//	BFC.Crypto.DLL.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_Crypto_DLL_H_
#define _BFC_Crypto_DLL_H_

// ============================================================================

// Doxygen group definition. All classes in this component should be made
// members of this group by using an \ingroup tag in the class documentation.

/// \defgroup BFC_Crypto BFC::Crypto (Cryptographic module)
///
/// \brief Provides cryptographic primitives (ciphers, modes, ...).

/// \mainpage
///
/// BFC::Crypto is a fairly comprehensive, modular and portable cryptographic
/// toolkit that provides developers with a vast array of well known published
/// block ciphers, one-way hash functions, chaining modes, pseudo-random number
/// generators, public key cryptography and a plethora of other routines.
///
/// BFC::Crypto has been designed from the ground up to be very simple to use.
/// It has a modular and standard API that allows new ciphers, hashes and PRNGs
/// to be added or removed without change to the overall end application. It
/// features easy to use functions and a complete user manual which has many
/// source snippet examples.
///
/// Additional information:
/// -	\link BFC::Crypto::Cipher Block Cipher \endlink
/// -	\link BFC::Crypto::Mode Mode of Operation \endlink
/// -	\link BFC::Crypto::Hash Hash \endlink
/// -	Message Authentication
/// -	Message Encrypt + Authenticate Modes
/// -	Pseudo-Random Number Generators
/// -	Public Key Algorithms
/// -	Other standards

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

#if ! defined( BFC_Crypto_DLL )
#  if defined( BFC_Crypto_EXPORTS )
#    define BFC_Crypto_DLL JBS_DLL_EXPORT
#  else
#    define BFC_Crypto_DLL JBS_DLL_IMPORT
#  endif
#endif

// ============================================================================

// Now, act as an all-in-one header for this DLL.

//#include "BFC.Crypto.MyClass.h"

// ============================================================================

#endif // _BFC_Crypto_DLL_H_

// ============================================================================

