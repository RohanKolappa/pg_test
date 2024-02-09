// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Image.Resolution.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_Resolution_H_
#define _BFC_Image_Resolution_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.UUId.h"

#include "BFC.Image.ResolutionManifest.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Enumerates some standard picture resolutions.
///
/// \ingroup BFC_Image

class BFC_Image_DLL Resolution {

public :

	/// \brief Enumeration of various predefined types.
	///
	/// (see also http://www.fourcc.org/yuv.php).

	enum Format {

		Unknown		= 0,		///< unknown format

		QQVGA,		///<	160	120	"Quarter QVGA"
		EVGA,		///<	240	180	"1/8 VGA"
		QVGA,		///<	320	240	"Quarter VGA"
		VGA,		///<	640	480	"VGA"
		WVGA,		///<	720	400	"Wide VGA"
		SVGA,		///<	800	600	"SVGA"
		WSVGA,		///<	1024	600	"Wide SVGA"
		XGA,		///<	1024	768	"XGA"
		WXGA,		///<	1280	768	"Wide XGA"
		SXGA,		///<	1280	1024	"SXGA"
		WSXGA,		///<	1600	1024	"Wide SXGA"
		UXGA,		///<	1600	1200	"Ultra XGA"
		WUXGA,		///<	1920	1200	"Wide Ultra XGA"
		SUXGA,		///<	2048	1536	"Super UXGA"
		QXGA,		///<	2048	1536	"Quad XGA"
		QUXGA,		///<	3200	2400	"Quad UXGA"
		QWUXGA,		///<	3840	2400	"Quad WUXGA"
		WQUXGA,		///<	3840	2400	"Wide QUXGA"
		SQCIF,		///<	128	96	"SQCIF"
		QCIF,		///<	176	144	"QCIF"
		SIF,		///<	352	240	"SIF"
		SIF_N,		///<	360	240	"SIF(NTSC)"
		SIF_P,		///<	360	288	"SIF(PAL)"
		CIF,		///<	352	288	"CIF"
		CIF4,		///<	704	576	"4CIF"
		CIF9,		///<	1056	864	"9CIF"
		CIF16,		///<	1408	1152	"16CIF"
		PAL,		///<	720	576	"PAL"
		PALW,		///<	1024	576	"PAL-Wide"
		NTSC,		///<	720	480	"NTSC"
		NTSC486,	///<	720	486	"NTSC 486"
		HDTV720,	///<	1280	720	"HDTV 720"
		HDTV1080,	///<	1920	1080	"HDTV 1080"

		MinFormat	= QQVGA,	///< Minimum known format (incl.)
		MaxFormat	= HDTV1080 + 1	///< Maximum known format (excl.)

	};

	/// \brief Creates a new Resolution, using the Unknown predefined
	///	format.

	Resolution(
	) : idx( Unknown ) {
	}

	/// \brief Creates a new Resolution object, based on the predefined
	///	format specified by \a pFormat.
	///
	/// \param pFormat
	///	The predefined format to use.

	Resolution(
		const	Format		pFormat
	) : idx( pFormat ) {
	}

	Resolution(
		const	Uint32		width,
		const	Uint32		height
	);

	/// \brief Creates a new Resolution, using the format univoquely
	///	identified by \a pUuid.

	explicit Resolution(
		const	UUId &		pUuid
	);

	/// \brief Tries to find a predefined format whose short name is
	///	\a pName.
	///
	/// Note the comparison is case-insensitive!
	///
	/// Example 1: Resolution( "YUV400" ) would create a new
	/// Resolution using the predefined YUV400 format.
	///
	/// Example 2: Resolution( "MTC987" ) would create a new
	/// Resolution using the predefined Unknown format.

	Resolution(
		const	Buffer &	pName
	);

	/// \brief Initializes this object with the content of \a other.

	Resolution(
		const	Resolution &	other
	) : idx( other.idx ) {
	}

	/// \brief Replaces the content of this object with a copy of the
	///	content of \a other.

	Resolution & operator = (
		const	Resolution &	other
	) {
		idx = other.idx;
		return *this;
	}

	/// \brief Resets this Resolution to the Unknown predefined value.

	void kill(
	) {
		idx = Unknown;
	}

	/// \brief Returns true iff this object is equal to \a other.
	///
	/// 2 formats are equal if they refer to the same manifest.
	///
	/// \param other
	///	The other format to compare this object with.

	Bool operator == (
		const	Resolution &	other
	) const {
		return ( idx == other.idx );
	}

	/// \brief Returns true iff this object is not equal to \a other.
	///
	/// 2 formats are not equal if they don't refer to the same manifest.
	///
	/// \param other
	///	The other format to compare this object with.

	Bool operator != (
		const	Resolution &	other
	) const {
		return ( idx != other.idx );
	}

	/// \brief Returns true iff this object is using the Unknown predefined
	///	format.

	Bool isUnknown(
	) const {
		return ( idx == Unknown );
	}

	/// \brief Returns true iff this object is using
	///	a format other than the Unknown one.

	Bool isKnown(
	) const {
		return ( idx != Unknown );
	}

	/// \brief Returns true iff this object is using a predefined format
	///	(including the Unknown format).

	Bool isPredefined(
	) const {
		return ( idx < MaxFormat );
	}

	/// \brief Returns true iff this object is using a user-defined format.

	Bool isUserDefined(
	) const {
		return ( idx >= MaxFormat );
	}

	/// \brief Returns the index of the manifest used by this object.
	///
	/// The returned index is relative to the databse maintained by the
	/// unique ResolutionRegistry.

	Uint32 getPredefined(
	) const {
		return idx;
	}

	/// \brief Returns the manifest of the format used by this object.

	const ResolutionManifest & getManifest(
	) const;

	/// \brief Returns the UUId (universal unique id) of the manifest
	///	used by this object.

	const UUId & getUUId(
	) const {
		return getManifest().getUUId();
	}

	/// Returns the width associated to this format.

	Uint32 getWidth(
	) const {
		return getManifest().getWidth();
	}

	/// Returns the height associated to this format.

	Uint32 getHeight(
	) const {
		return getManifest().getHeight();
	}

	const Buffer & getShortName(
	) const {
		return getManifest().getShortName();
	}

	const Buffer & getLongName(
	) const {
		return getManifest().getLongName();
	}

private :

	Uint32		idx;	///< Index in registry.

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Resolution_H_

// ============================================================================

