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
// Filename:
//	TBFC.Base.PackedEnd.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _TBFC_Base_Packed_H_

	#error No previous include of TBFC.Base.PackedBegin.h!

#else

	#if defined( PLATFORM_WIN32 )
		#undef PACKED_STRUCT_DECL
		#if ( _MSC_VER >= 800 && ! defined( _M_I86 ) ) || defined( _PUSHPOP_SUPPORTED )
			#if ! defined( MIDL_PASS ) || defined( __midl )
				#pragma pack(pop)
			#else
				#pragma pack()
			#endif
		#else
			#pragma pack()
		#endif
	#else
		#undef PACKED_STRUCT_DECL
	#endif

	#undef _TBFC_Base_Packed_H_

#endif

// ============================================================================

