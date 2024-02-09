// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedFormat.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_MuxedFormat_H_
#define _VMP_MuxedFormat_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.IO.Serializable.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief Represents the format used to encode a MuxedStream.
///
/// A MuxedStream (i.e. the multiplexing of several Stream objects)
/// is always encoded using a specific format, represented by an object
/// of this class.
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL MuxedFormat : virtual public BFC::IO::Serializable {

public :

	/// \brief Enumerated values used by objects of this class.

	enum Enum {

		Unknown		= 0,	///< Unknown file format.

//		Uncompressed,		///< No substream is encoded.

		AVI,			///< AVI file format.
		MPEG1SS,		///< MPEG-1 System Stream (see ISO/IEC 11172-1).
		MPEG2PS,		///< MPEG-2 Program Stream (see ISO/IEC 13818-1).
		MPEG2TS,		///< MPEG-2 Transport Stream(see ISO/IEC 13818-1).

		MKV,			///< Matroska file format (http://www.matroska.org/).

		OGG,			///< Xiph.org Ogg format (http://www.xiph.org/).

		MPEG4ISO,		///< MPEG-4 Part 12 - ISO Base Media File Format.
		MPEG4MP4		///< MPEG-4 Part 14 - MP4 File Format.

	};

	/// \brief Constructs a new object representing MuxedStream format
	///	\a pFormat.

	MuxedFormat(
		const	Enum		pFormat = Unknown
	) :
		Serializable(),
		f( pFormat ) {
	}

	MuxedFormat(
		const	MuxedFormat &	pOther
	) :
		SObject	(),
		Serializable(),
		f( pOther.f ) {
	}

	BFC::Bool operator == (
		const	MuxedFormat &	pOther
	) const {
		return ( f == pOther.f );
	}

	BFC::Bool operator != (
		const	MuxedFormat &	pOther
	) const {
		return ( f != pOther.f );
	}

	/// \brief Returns the canonical name of this format.

	const char * name(
	) const {
		return nms[ f ];
	}

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

private :

	// internal state...

	Enum			f;		///< Internal value.
	static const char *	nms[];		///< Conversion table

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_MuxedFormat_H_

// ============================================================================

