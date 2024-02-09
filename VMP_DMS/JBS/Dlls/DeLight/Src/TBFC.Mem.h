// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// FileName:
//	TBFC.Mem.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Mem_H_
#define _TBFC_Mem_H_

// ============================================================================

#include "TBFC.Mem.Allocator.h"
#include "TBFC.Mem.Block.h"
#include "TBFC.Mem.Data.h"
#include "TBFC.Mem.Reader.h"
#include "TBFC.Mem.Writer.h"

// ============================================================================

/// \defgroup TBFC_Mem TBFC::Mem (memory management module)
///
/// \ingroup TBFC_Light
///
/// \brief Memory management module.
///
/// This module provides some specialized memory allocators, which implement
/// specific strategies to allocate memory in a transparent way.
///
/// It also provides objects that allow combined reading and writing using
/// different word lengths.
///
/// Example:
///
/// \code
///	#include <iostream>
///
///	#include "TBFC.Mem.h"
///
///	using namespace std;
///	using namespace TBFC;
///
///	int main() {
///
///		Mem::Block	block( 64 );
///
///		Mem::Uint32Writer	w( block );
///		Mem::Uint24Reader	r( block );
///
///		while ( w ) {
///			*w = 0x12345678;
///			w++;
///			cerr << hex << ( Uint32 )*r << endl;;
///			r++;
///		}
///
///		return 0;
///
///	}
/// \endcode
///
/// Take a look at the MemTester.cpp test program for a slightly more
/// complex example.

// \section Normal_mode Normal mode
//
// In its most simple configuration, an Strider sees a data block as a
// sequence of \b M elements of type T', each element occupying \b T
// ( = sizeof( T' ) ) bytes, this sequence being optionally followed by
// \b Q padding bytes, as depicted below:
//
// \verbatim
// |<------------ M elements ----------->|<-- Q bytes -->|
//
// +-----+-----+-----+-------------+-----+---------------+
// |  T' |  T' |  T' |     ...     |  T' |   <padding>   |
// +-----+-----+-----+-------------+-----+---------------+
// \endverbatim
//
// If \b L is the length (in bytes) of the data block, then the number \b M
// of elements and the eventual padding length \b Q are given by:
//
// \f[
//	M = \left \lfloor \frac{ L }{ T } \right \rfloor
// \f]
//
// \f[
//	Q = L - M.T
// \f]
//
// \section Striding_mode Striding mode
//
// A second, more complex, configuration of an Strider is by using
// "striding".
//
// In this mode, the data block is divided into a sequence of
// strides of equal length, optionally followed by \b Q padding bytes.
//
// The strides are partioned into 3 consecutive groups:
// -	the first (top) group is composed of \b Pt padding strides
// -	the second (middle) group is composed of \b N data strides
// -	the third (bottom) group is composed of \b Pb padding strides.
//
// Each data stride contains 3 consecutive groups of bytes:
// -	the first (left) group is composed of \b Pl padding bytes
// -	the second (middle) group is composed of \b M elements of type T'
// -	the third (right) group is composed of \b Pr padding bytes.
//
// \verbatim
// |<-- Pl bytes -->|<------------ M elements ----------->|<-- Pr bytes -->|
//
// +-----------------------------------------------------------------------+ ----------
// |                               <padding>                               |      ^
// +-----------------------------------------------------------------------+      |
// |                                                                       |      |
// |                                  ...                                  | Pt strides
// |                                                                       |      |
// +-----------------------------------------------------------------------+      |
// |                               <padding>                               |      v
// +----------------+-----+-----+-----+-------------+-----+----------------+ ----------
// |   <padding>    |  T' |  T' |  T' |     ...     |  T' |   <padding>    |      ^
// +----------------+-----+-----+-----+-------------+-----+----------------+      |
// |   <padding>    |  T' |  T' |  T' |     ...     |  T' |   <padding>    |      |
// +----------------+-----+-----+-----+-------------+-----+----------------+      |
// |                |                                     |                |      |
// |                |                                     |                |  N strides
// |      ...       |                   ...               |      ...       |      |
// |                |                                     |                |      |
// |                |                                     |                |      |
// +----------------+-----+-----+-----+-------------+-----+----------------+      |
// |   <padding>    |  T' |  T' |  T' |     ...     |  T' |   <padding>    |      v
// +----------------+-----+-----+-----+-------------+-----+----------------+ ----------
// |                               <padding>                               |      ^
// +-----------------------------------------------------------------------+      |
// |                                                                       |      |
// |                                  ...                                  | Pb strides
// |                                                                       |      |
// +-----------------------------------------------------------------------+      |
// |                               <padding>                               |      v
// +---------------------+-------------------------------------------------+ ----------
// |      <padding>      |
// +---------------------+
//
// |<----- Q bytes ----->|
// \endverbatim
//
// The total number \b E of data elements is given by:
//
// \f[
//	E = M . N
// \f]
//
// The length \b S (in bytes) of any stride (data or padding) is given by:
//
// \f[
//	S = Pl + M . T + Pr
// \f]
//
// The total length \b R (in bytes) of all strides (data and padding) is
// given by:
//
// \f[
//	R = ( Pt + N + Pb ) . S
// \f]
//
// If \b L is the length (in bytes) of the data block, then the length \b Q of
// the optional padding bytes found after the strides is given by:
//
// \f[
//	Q = L - R
// \f]
//
// Note that using the standard mode is obtained by taking: \b N = 1, and
// \b Pt = \b Pl = \b Pr = \b Pb = 0, and by minimizing \b Q.

// ============================================================================

#endif // _TBFC_Mem_H_

// ============================================================================

