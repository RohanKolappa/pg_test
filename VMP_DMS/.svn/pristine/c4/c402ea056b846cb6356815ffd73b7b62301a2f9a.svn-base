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
// Filename:
//	BFC.Image.PictureFormat.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_PictureFormat_H_
#define _BFC_Image_PictureFormat_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.FourCC.h"
#include "BFC.Base.SObject.h"

#include "BFC.Dyn.TEnumPtr.h"

#include "BFC.IO.ByteInputStreamPtr.h"
#include "BFC.IO.ByteOutputStreamPtr.h"

#include "BFC.Image.ColorChannel.h"
#include "BFC.Image.ColorSpace.h"
#include "BFC.Image.PictureFormatManifest.h"
#include "BFC.Image.PictureFormatRegistry.h"
#include "BFC.Image.PlaneFormat.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Provides information about the pixel format used by a Picture.
///
/// Example: iterating over all \b predefined types:
/// \code
///	for ( PictureFormat f = PictureFormat::MinFormat
///	    ; f < PictureFormat::MaxFormat ; f++ ) {
///		cerr << f.getShortName() << endl;
///	}
/// \endcode
///
/// \ingroup BFC_Image

class BFC_Image_DLL PictureFormat : virtual public SObject {

public :

	/// \brief Enumeration of various predefined types.
	///
	/// (see also http://www.fourcc.org/yuv.php).

	enum Format {

		// ============
		// Unknown type
		// ============

		Unknown	= 0,		///< Unknown format

		// ============
		// Planar types
		// ============

		// ... 64 bpp

		RYUVA444P,		///< Planar, 64 bpp, [Y U V A] (4:4:4)

		// ... 48 bpp

		RGB48P,			///< Planar, 48 bpp, [R G B]
		RYUV444P,		///< Planar, 48 bpp, [Y U V] (4:4:4)

		// ... 36 bpp

		RGB36P,			///< Planar, 36 bpp, [R G B]

		// ... 32 bpp

		RGBA,			///< Planar, 32 bpp, [R G B A]
		BGRA,			///< Planar, 32 bpp, [B G R A]
		YUVA444P,		///< Planar, 32 bpp, [Y U V A] (4:4:4)
		YVUA444P,		///< Planar, 32 bpp, [Y V U A] (4:4:4)

		// ... 30 bpp

		RGB30P,			///< Planar, 30 bpp, [R G B]

		// ... 24 bpp

		RGB,			///< Planar, 24 bpp, [R G B]
		BGR,			///< Planar, 24 bpp, [B G R]
		YUV444P,		///< Planar, 24 bpp, [Y U V] (4:4:4)
		I444	= YUV444P,	///< = YUV444P
		YVU444P,		///< Planar, 24 bpp, [Y V U] (4:4:4)
		YV24	= YVU444P,	///< = YVU444P
		JYUV444P,		///< Planar, 24 bpp, [Y U V] (4:4:4)

		// ... 16 bpp

		YUV422P,		///< Planar, 16 bpp, [Y U V] (4:2:2)
		YU16	= YUV422P,	///< = YUV422P
		I422	= YUV422P,	///< = YUV422P
		YVU422P,		///< Planar, 16 bpp, [Y V U] (4:2:2)
		YV16	= YVU422P,	///< = YVU422P
		JYUV422P,		///< Planar, 16 bpp, [Y U V] (4:2:2)

		// ... 12 bpp

		YUV411P,		///< Planar, 12 bpp, [Y U V] (4:1:1)
		YVU411P,		///< Planar, 12 bpp, [Y V U] (4:1:1)
		YUV420P,		///< Planar, 12 bpp, [Y U V] (4:2:0)
		I420	= YUV420P,	///< = YUV420P
		IYUV	= YUV420P,	///< = YUV420P
		YVU420P,		///< Planar, 12 bpp, [Y V U] (4:2:0)
		YV12	= YVU420P,	///< = YVU420P
//		NV12,			///< Planar, 12 bpp, [Y U+V] (4:2:0)
//		NV21,			///< Planar, 12 bpp, [Y V+U] (4:2:0)
		JYUV420P,		///< Planar, 12 bpp, [Y U V] (4:2:0)

		// ... 9 bpp

		YUV410P,		///< Planar, 9 bpp, [Y U V] (4:1:0)
		YUV9	= YUV410P,	///< = YUV410P
		YVU410P,		///< Planar, 9 bpp, [Y V U] (4:1:0)
		YVU9	= YVU410P,	///< = YVU410

		// ============
		// Packed types
		// ============

		// ... 48 bpp

		RGB48,			///< Packed, 48 bpp, [R G B]
		BGR48,			///< Packed, 48 bpp, [B G R]

		// ... 36 bpp

		RGB36,			///< Packed, 36 bpp, [R G B]
		BGR36,			///< Packed, 36 bpp, [B G R]

		// ... 32 bpp

		RGBA32,			///< Packed, 32 bpp, [RAM: B G R A]
		BGRA32,			///< Packed, 32 bpp, [RAM: R G B A]
		AYUV,			///< Packed, 32 bpp, [RAM: A Y U V] (4:4:4)
		AYUV444	= AYUV,		///< = AYUV
//		V210,			///< Packed, 32 bpp, 10bpc, [RAM: ???] (4:2:2)

		// ... 30 bpp

		RGB30,			///< Packed, 30 bpp, [R G B]
		BGR30,			///< Packed, 30 bpp, [B G R]

		// ... 24 bpp

		RGB24,			///< Packed, 24 bpp, [RAM: R G B]
		BGR24,			///< Packed, 24 bpp, [RAM: B G R]
		IYU2,			///< Packed, 24 bpp, [RAM: U Y V] (4:4:4)
		UYV444	= IYU2,		///< = IYU2
//		UYVP,			///< Packed, 24 bpp, 10bpc, [RAM: ???] (4:2:2)
//		YUVP,			///< Packed, 24 bpp, 10bpc, [RAM: ???] (4:2:2)

		// ... 16 bpp

		RGB565,			///< Packed, 16 bpp, [W16: R G B]
		UYVY,			///< Packed, 16 bpp, [RAM: U Y V Y] (4:2:2)
		Y422	= UYVY,		///< = UYVY
		UYNV	= UYVY,		///< = UYVY
		YUV422	= UYVY,		///< = UYVY
		YUYV,			///< Packed, 16 bpp, [RAM: Y U Y V] (4:2:2)
		YUY2	= YUYV,		///< = YUYV
		YUNV	= YUYV,		///< = YUYV
		UVY422	= YUYV,		///< = YUYV
		YVYU,			///< Packed, 16 bpp, [RAM: Y V Y U] (4:2:2)
//		IUYV,			///< Packed, 16 bpp, [???] interlaced UYVY (4:2:2)
//		V655,			///< Packed, 16 bpp, [???] (4:2:2)
//		V422,			///< Packed, 16 bpp, [???] upside down UYVY

		// ... 15 bpp

		RGB555,			///< Packed, 15 bpp, [W16: R G B]

		// ... 12 bpp

//		Y41P,			///< Packed, 12 bpp, [RAM: U0 Y0 V0 Y1 | U4 Y2 V4 Y3 | Y4 Y5 Y6 Y7] (4:1:1)
//		Y411,			///< Packed, 12 bpp, [RAM: Ui Y0 Y1 Vi Y2 Y3 with i = 0 or 2] (4:1:1)
//		IYU1	= Y411,		///< = Y411
//		IY41,			///< Packed, 12 bpp, [???] interlaced Y41P

		// ... 8 bpp

//		Y211,			///< Packed, 8 bpp, [RAM: Y0 U0 Y2 V0 | Y4 U4 Y6 V4] (2:1:1)
//		CLJR,			///< Packed, 8 bpp, [W32: Y3 Y2 Y1 Y0 U V with 5 bpY, 6 bpUV] (4:1:1)

		RGB323,			///< Packed, 8 bpp.
		SGBRG8,			///< Packed Bayer, 8 bpp, L0 W16: G B, L1 W16: R G)

		// ===========
		// Other types
		// ===========

		// ... 16 bpp

		GRAY16,			///< 16 bpp, linear grayscale

		// ... 12 bpp

		GRAY12,			///< 12 bpp, linear grayscale

		// ... 8 bpp

		GRAY8,			///< 8 bpp, linear grayscale
		Y800,			///< 8 bpp, Y component only (4:0:0)
		YUV400	= Y800,		///< = Y800
		Y8	= Y800,		///< = Y800
		JYUV400,		///< 8 bpp, Y [0-255] component only (4:0:0)  

		// ==========
		// Boundaries
		// ==========

		MinFormat	= Unknown + 1,	///< minimum known format (incl.)
		MaxFormat	= JYUV400 + 1	///< maximum known format (excl.)

	};

	/// \brief Creates a new PictureFormat, using the Unknown predefined
	///	format.

	PictureFormat(
	) :
		idx	( Unknown ) {
	}

	/// \brief Creates a new PictureFormat object, based on the predefined
	///	format specified by \a pFormat.
	///
	/// \param pFormat
	///	The predefined format to use.

	PictureFormat(
		const	Format		pFormat
	) :
		idx	( pFormat ) {
	}

	/// \brief Creates a new PictureFormat, using the format univoquely
	///	identified by \a pUuid.

	explicit PictureFormat(
		const	UUId &		pUuid
	);

	/// \brief Creates a new PictureFormat, identified by \a fourcc, or
	///	the predefined Unknown format if \a fourcc does not refer to
	///	any other predefined format.

	explicit PictureFormat(
		const	FourCC &	fourcc
	);

	/// \brief Creates a new PictureFormat, identified by its \a pIndex
	///	into the PictureFormatRegistry.

	explicit PictureFormat(
		const	Uint32		pIndex
	);

	/// \brief Tries to find a predefined format that uses the same values
	///	as the provided ones.
	///
	/// This constructor tries to find a predefined format that matches
	/// the user-supplied criteria.
	///
	/// If there are multiple possibilities, then the first one is choosen.
	///
	/// If no appropriate format is found, then the unknown format is used.
	///
	/// \param pBitsPerPixel
	///	Number of bits per pixel the format should use.
	///
	/// \param pNbrPlanes
	///	Number of planes the format should use.
	///
	/// \param pColorSpace
	///	Color space of the predefined format (default: any color
	///	space).
	///
	/// Example 1: PictureFormat( 15, 1 ) would create a new PictureFormat
	/// using the predefined RGB555 format.
	///
	/// Example 2: PictureFormat( 24, 3 ) would create a new PictureFormat
	/// using the predefined RGB format.
	///
	/// Example 3: PictureFormat( 24, 3, ColorSpace::YUV ) would create a
	/// new PictureFormat using the predefined YUV444P format.

	PictureFormat(
		const	Uint32		pBitsPerPixel,
		const	Uint32		pNbrPlanes,
		const	ColorSpace	pColorSpace = ColorSpace::Unknown
	);

	/// \brief Tries to find a predefined format whose short name is
	///	\a pName.
	///
	/// Example 1: PictureFormat( "YUV400" ) would create a new
	/// PictureFormat using the predefined YUV400 format.
	///
	/// Example 2: PictureFormat( "MTC987" ) would create a new
	/// PictureFormat using the predefined Unknown format.

	PictureFormat(
		const	Buffer &	pName
	);

	/// \brief Initializes this object with the content of \a pOther.

	PictureFormat(
		const	PictureFormat &	pOther
	) :
		SObject	(),
		idx	( pOther.idx ) {
	}

	/// \brief Replaces the content of this object with a copy of the
	///	content of \a pOther.

	PictureFormat & operator = (
		const	PictureFormat &	pOther
	) {
		idx = pOther.idx;
		return *this;
	}

	/// \brief Resets this PictureFormat to the Unknown predefined value.

	void kill(
	) {
		idx = Unknown;
	}

	/// \brief Returns true iff this object is equal to \a pOther.
	///
	/// 2 formats are equal if they refer to the same manifest.
	///
	/// \param pOther
	///	The other format to compare this object with.

	Bool operator == (
		const	PictureFormat &	pOther
	) const {
		return ( idx == pOther.idx );
	}

	/// \brief Returns true iff this object is not equal to \a pOther.
	///
	/// 2 formats are not equal if they don't refer to the same manifest.
	///
	/// \param pOther
	///	The other format to compare this object with.

	Bool operator != (
		const	PictureFormat &	pOther
	) const {
		return ( idx != pOther.idx );
	}

	Bool operator < (
		const	PictureFormat &	pOther
	) const {
		return ( idx < pOther.idx );
	}

	void operator ++ (
	) {
		idx++;
	}

	void operator ++ (
			int
	) {
		idx++;
	}

	void operator -- (
	) {
		idx--;
	}

	void operator -- (
			int
	) {
		idx--;
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
	/// unique PictureFormatRegistry.

	Uint32 getPredefined(
	) const {
		return idx;
	}

	/// \brief Returns the manifest of the format used by this object.

	const PictureFormatManifest & getManifest(
	) const {
		return ( PictureFormatRegistry::instance()->getManifest( idx ) );
	}

	/// \brief Returns the UUId (universal unique id) of the manifest
	///	used by this object.

	const UUId & getUUId(
	) const {
		return getManifest().getUUId();
	}

	/// \brief Returns the long name of this format.
	///
	/// A long name usually contains the short name (see getShortName()),
	/// augmented with some explanation about the memory layout used by
	/// this format.

	const Buffer & getLongName(
	) const {
		return getManifest().longName;
	}

	/// \brief Returns a short name for this format.
	///
	/// A short name is usually a well known code (ex. "RGB", "YUV422",
	/// ...) no longer than a few characters. The short names are
	/// typically used in ComboBoxes to let the user select a specific
	/// format, while the long name is used as a ToolTip.

	const Buffer & getShortName(
	) const {
		return getManifest().shortName;
	}

	/// \brief Synonym of getShortName().
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	const Buffer & name(
	) const {
		return getShortName();
	}

	/// \brief Synonym of getShortName().
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	const Buffer & shortName(
	) const {
		return getShortName();
	}

	/// \brief Returns true iff this format puts every color channel into
	///	a separate Plane.
	///
	/// The answer is valid for predefined formats only! It should not be
	/// relied upon for user-defined formats, as there is no way to
	/// know the content of the planes in such formats.

	Bool isPlanar(
	) const {
		return ( getNbrPlanes() >= getColorSpace().getNbrChannels() );
	}

	PictureFormat getPlanar(
	) const;

	/// \brief Returns true iff this format packs multiple color channels
	///	into one Plane.
	///
	/// The answer is valid for predefined formats only! It should not be
	/// relied upon for user-defined formats, as there there is no way to
	/// know the content of the planes in such formats.

	Bool isPacked(
	) const {
		return ( getNbrPlanes() < getColorSpace().getNbrChannels() );
	}

	/// \brief Returns the number of planes this format is using.

	Uint32 getNbrPlanes(
	) const {
		return getManifest().planes.size();
	}

	/// \brief Synonym of getNbrPlanes().
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	Uint32 getPlanesNumber(
	) const {
		return getNbrPlanes();
	}

	/// \brief Returns the format of plane \a pPicPlane.
	///
	/// \param pPicPlane
	///	The plane to consider.

	PlaneFormat getPlane(
		const	Uint32		pPicPlane
	) const {
		return PlaneFormat( getManifest().planes[ pPicPlane ] );
	}

	/// \brief Synonym of getPlane().

	PlaneFormat operator [] (
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane );
	}

	/// \brief Returns the ColorSpace of this format.

	ColorSpace getColorSpace(
	) const {
		return getManifest().colorSpace;
	}

	/// \brief Returns the ColorChannel corresponding to plane
	///	\a pPicPlane.
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	ColorChannel getColorChannel(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getColorChannel();
	}

	/// \brief Returns the width of a particular plane, after possible
	///	downsampling.
	///
	/// \param pPicPlane
	///	The plane to consider.
	///
	/// \param pPicWidth
	///	Width of the picture.
	///
	/// Example:
	/// \code
	///	PictureFormat f = PictureFormat::YUV420P;
	///	for ( Uint32 i = 0 ; i < f.getNbrPlanes() ; i++ ) {
	///		cout << f.getWidthPerPlane( i, 320 ) << endl;
	///	}
	/// \endcode
	/// would print:
	/// \code
	///	320
	///	160
	///	160
	/// \endcode
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getWidthPerPlane(
		const	Uint32		pPicPlane,
		const	Uint32		pPicWidth
	) const {
		return getPlane( pPicPlane ).getWidth( pPicWidth );
	}

	/// \brief Returns the height of a particular plane, after possible
	///	downsampling.
	///
	/// \param pPicPlane
	///	The plane to consider.
	///
	/// \param pPicHeight
	///	Height of the picture.
	///
	/// Example:
	/// \code
	///	PictureFormat f = PictureFormat::YUV420P;
	///	for ( Uint32 i = 0 ; i < f.getNbrPlanes() ; i++ ) {
	///		cout << f.getHeightPerPlane( i, 320 ) << endl;
	///	}
	/// \endcode
	/// would print:
	/// \code
	///	320
	///	160
	///	160
	/// \endcode
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getHeightPerPlane(
		const	Uint32		pPicPlane,
		const	Uint32		pPicHeight
	) const {
		return getPlane( pPicPlane ).getHeight( pPicHeight );
	}

	/// \brief Returns the average number of bits per pixel.
	///
	/// This method returns the average number of bits per pixel
	/// needed when using this format. This number is the sum of
	/// the number of bits per pixel taken over all planes, with possible
	/// downsampling in the X or Y direction taken into account.
	///
	/// Note this is a theoretical number, since the actual physical
	/// implementation will sometimes use more bits, due to alignment
	/// on byte or dword boundaries.
	///
	/// Examples:
	/// -	GRAY12 ---> 12
	/// -	YUV410P (planar) ---> 9
	/// -	RGB555 ---> 15
	/// -	YUYV ---> 16 (NOT 32!!!)
	/// -	RGBA (packed) ---> 32

	Uint32 getBitsPerPixel(
	) const {
		return getManifest().bitsPerPixel;
	}

	/// \brief Returns the number of pixels per elementary unit of data
	///	in plane \a pPicPlane.
	///
	/// Some planes store multiple pixels per elementary unit of data.
	/// For example, the YUYV format uses 2 pixels per element.
	///
	/// \param pPicPlane
	///	The plane to consider.
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getPixelsPerElementPerPlane(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getPixelsPerElement();
	}

	/// \brief Returns the number of bits per pixel of a specific plane.
	///
	/// This method returns the number of bits used by each pixel
	/// of a particular plane. This number DOES NOT take possible
	/// downsampling into account! This means that taking the sum of
	/// the values returned by this method over all planes does not
	/// always give the same value as the value returned by
	/// getBitsPerPixel()!
	///
	/// \param pPicPlane
	///	The plane to consider.
	///
	/// Examples of planar types:
	/// -	RGB (24 bpp) ---> ( 8, 8, 8 )
	/// -	YUV422P (16 bpp!!!) ---> ( 8, 8, 8 )
	///
	/// Examples of packed types:
	/// -	RGB24, BGR24 ---> ( 24 )
	/// -	RGBA ---> ( 32 )
	/// -	RGB565, YUV422, GRAY16 ---> ( 16 )
	/// -	RGB555 ---> ( 15 )
	///
	/// Other examples:
	/// -	GRAY12 ---> ( 12 )
	/// -	GRAY8 ---> ( 8 )
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getBitsPerPixelPerPlane(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getBitsPerPixel();
	}

	/// \brief Synonym of getBitsPerPixelPerPlane().
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	Uint32 getBitsPerSamplePerPlane(
		const	Uint32		pPicPlane
	) const {
		return getBitsPerPixelPerPlane( pPicPlane );
	}

	/// \brief Returns the number of bytes per pixel in a specific plane.
	///
	/// This method returns the number of bytes used by each pixel
	/// of a particular plane. This number DOES NOT take possible
	/// downsampling into account!
	///
	/// \param pPicPlane
	///	The plane to consider.
	///
	/// Examples of planar types:
	/// -	RGB (24 bpp) ---> ( 1, 1, 1 )
	/// -	YUV422P (16 bpp) ---> ( 1, 1, 1 )
	///
	/// Examples of packed types:
	/// -	RGB24, BGR24 ---> 3
	/// -	RGBA ---> 4
	/// -	RGB565, YUV422, GRAY16 ---> 2
	/// -	RGB555 ---> 2
	///
	/// Other examples:
	/// -	GRAY12 ---> 2
	/// -	GRAY8 ---> 1
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getBytesPerPixelPerPlane(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getBytesPerPixel();
	}

	/// \brief Returns the number of bytes needed to represent a line of
	///	data of a Picture which is \a pPicWidth pixels wide, in plane
	///	\a pPicPlane.
	///
	/// See PlaneFormat::getBytesPerLine() for more information.
	///
	/// \param pPicPlane
	///	The plane to consider.
	///
	/// \param pPicWidth
	///	The original width of the picture, in pixels. This value is
	///	automatically adjusted if downsampling is used in plane
	///	\a pPicPlane.
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getBytesPerLinePerPlane(
		const	Uint32		pPicPlane,
		const	Uint32		pPicWidth
	) const {
		return getPlane( pPicPlane ).getBytesPerLineIfPicture( pPicWidth );
	}

	/// \brief Returns the number of bytes needed to represent the data
	///	of plane \a pPicPlane, if the original
	///	picture is \a pPicWidth by \a pPicHeight pixels.
	///
	/// See PlaneFormat::getBytes() for more information.
	///
	/// \param pPicPlane
	///	The plane to consider.
	///
	/// \param pPicWidth
	///	The original width of the picture, in pixels. This value is
	///	automatically adjusted if downsampling is used in plane
	///	\a pPicPlane.
	///
	/// \param pPicHeight
	///	The original height of the picture, in pixels. This value is
	///	automatically adjusted if downsampling is used in plane
	///	\a pPicPlane.
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getBytesPerPlane(
		const	Uint32		pPicPlane,
		const	Uint32		pPicWidth,
		const	Uint32		pPicHeight
	) const {
		return getPlane( pPicPlane ).getBytesIfPicture( pPicWidth, pPicHeight );
	}

	/// \brief Returns the number of bytes needed to represent a picture
	///	of \a pPicWidth by \a pPicHeight pixels.
	///
	/// \param pPicWidth
	///	The width of the picture, in pixels.
	///
	/// \param pPicHeight
	///	The height of the picture, in pixels.

	Uint32 getBytesPerPicture(
		const	Uint32		pPicWidth,
		const	Uint32		pPicHeight
	) const;

	/// \brief Returns the minimum downsampling factor in the X direction
	///	used by plane \a pPicPlane.
	///
	/// This method returns the minimum downsampling factor applied in the
	/// X direction in plane \a pPicPlane.
	///
	/// A value of 1 means no downsampling.
	///
	/// Typical values are 1, 2, and 4.
	///
	/// \param pPicPlane
	///	The plane to consider (default: 1).
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getMinXDownsampling(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getMinXDownsampling();
	}

	/// \brief Returns the minimum downsampling factor in the Y direction
	///	used by plane \a pPicPlane.
	///
	/// This method returns the minimum downsampling factor applied in the
	/// Y direction in plane \a pPicPlane.
	///
	/// A value of 1 means no downsampling.
	///
	/// Typical values are 1, 2, and 4.
	///
	/// \param pPicPlane
	///	The plane to consider (default: 1).
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getMinYDownsampling(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getMinYDownsampling();
	}

	/// \brief Returns the maximum downsampling factor in the X direction
	///	used by plane \a pPicPlane.
	///
	/// This method returns the maximum downsampling factor applied in the
	/// X direction in plane \a pPicPlane.
	///
	/// A value of 1 means no downsampling.
	///
	/// Typical values are 1, 2, and 4.
	///
	/// \param pPicPlane
	///	The plane to consider (default: 1).
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getMaxXDownsampling(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getMaxXDownsampling();
	}

	/// \brief Returns the maximum downsampling factor in the Y direction
	///	used by plane \a pPicPlane.
	///
	/// This method returns the maximum downsampling factor applied in the
	/// Y direction in plane \a pPicPlane.
	///
	/// A value of 1 means no downsampling.
	///
	/// Typical values are 1, 2, and 4.
	///
	/// \param pPicPlane
	///	The plane to consider (default: 1).
	///
	/// \throws OutOfRange
	///	If \a pPicPlane is greater or equal to the number of planes.

	Uint32 getMaxYDownsampling(
		const	Uint32		pPicPlane
	) const {
		return getPlane( pPicPlane ).getMaxYDownsampling();
	}

	/// \brief Returns the minimum downsampling factor in the X
	///	direction, taken over all planes.
	///
	/// Typical values are 1, 2, and 4.

	Uint32 getMinXDownsampling(
	) const {
		return getManifest().minXDownsmpl;
	}

	/// \brief Returns the minimum downsampling factor in the Y
	///	direction, taken over all planes.
	///
	/// Typical values are 1, 2, and 4.

	Uint32 getMinYDownsampling(
	) const {
		return getManifest().minYDownsmpl;
	}

	/// \brief Returns the maximum downsampling factor in the X
	///	direction, taken over all planes.
	///
	/// Typical values are 1, 2, and 4.

	Uint32 getMaxXDownsampling(
	) const {
		return getManifest().maxXDownsmpl;
	}

	/// \brief Returns the maximum downsampling factor in the Y
	///	direction, taken over all planes.
	///
	/// Typical values are 1, 2, and 4.

	Uint32 getMaxYDownsampling(
	) const {
		return getManifest().maxYDownsmpl;
	}

	/// \brief Returns true iff this format can represent a pic of size
	///	(W x H).
	///
	/// This method verifies that the possible downsampling used by
	/// this format is compatible with the picture dimensions
	/// \a pPicWidth and \a pPicHeight.
	///
	/// \param pPicWidth
	///	The width of the picture, in pixels.
	///
	/// \param pPicHeight
	///	The height of the picture, in pixels.

	Bool canRepresent(
		const	Uint32		pPicWidth,
		const	Uint32		pPicHeight
	) const;

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

	static BFC::Dyn::TEnumCPtr getTEnum(
		const	Bool		pUnknown = false
	);

protected :

	static BFC::Dyn::TEnumCPtr makeTEnum(
		const	Bool		pUnknown = false
	);

private :

	Uint32		idx;	///< Index in registry.

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_PictureFormat_H_

// ============================================================================

