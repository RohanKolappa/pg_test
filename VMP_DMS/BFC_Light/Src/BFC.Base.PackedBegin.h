// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::Light".
//
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.Base.PackedBegin.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Base_Packed_H_

	#define _BFC_Base_Packed_H_

	/// \def PACKED_STRUCT_DECL
	///
	/// Platform-dependent string to be placed between the "struct" keyword
	/// and the structure name, to indicate that the following structure
	/// should be packed.
	///
	/// \ingroup BFC_Base

	#if defined( PLATFORM_WIN32 )
		#if ( _MSC_VER >= 800 && !defined(_M_I86)) || defined(_PUSHPOP_SUPPORTED)
			// Disable warning C4103:
			//	'<...>' : alignment changed after including header,
			//	may be due to missing pragma pack(pop).
			#pragma warning ( disable : 4103 )
			#if ! defined( MIDL_PASS ) || defined( __midl )
				#pragma pack(push,1)
			#else
				#pragma pack(1)
			#endif
		#else
			#pragma pack(1)
		#endif
		#define PACKED_STRUCT_DECL
	#else
		#define PACKED_STRUCT_DECL	__attribute__((__packed__))
	#endif

#else

	#error Nested includes of BFC.Base.PackedBegin.h!

#endif

// ============================================================================

