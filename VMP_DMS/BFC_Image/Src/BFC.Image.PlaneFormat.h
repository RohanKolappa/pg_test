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
//	BFC.Image.PlaneFormat.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_PlaneFormat_H_
#define _BFC_Image_PlaneFormat_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.IO.ByteInputStreamPtr.h"
#include "BFC.IO.ByteOutputStreamPtr.h"

#include "BFC.Image.ColorChannel.h"
#include "BFC.Image.PlaneFormatManifest.h"
#include "BFC.Image.PlaneFormatRegistry.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Provides information about the pixel format used by a Plane.
///
/// A Plane is a matrix of units of data, each unit containing information
/// about one or more pixel(s), and/or about one or more color channels.
///
/// The properties of a Plane are:
/// -	color channel: represents the color channel(s) this plane provides.
/// -	bits per pixel: gives the number of bits each elementary unit of
///	data uses in a plane.
/// -	pixels per unit: number of pixels per elementary unit of data.
/// -	X downsampling: optional downsampling in the X direction (a minimum
///	and a maximum).
/// -	Y downsampling: optional downsampling in the Y direction (a minimum
///	and a maximum).
///
/// The minimum downsampling factors give the resolution of this Plane,
/// compared to an original Picture, while the maximum downsampling factors
/// indicate the possible loss of precision.
///
/// Example: unique plane of the PictureFormat::YUYV predefined format.
///	-	color channel: all YUV channels
///	-	bits per pixel: 16
///	-	pixels per unit: 2
///	-	minimum downsampling: (1,1)
///	-	maximum downsampling: (2,1)
///
/// Example: unique plane of the PictureFormat::RGB24 predefined format.
///	-	color channel: all RGB channels
///	-	bits per pixel: 24
///	-	pixels per unit: 1
///	-	minimum downsampling: (1,1)
///	-	maximum downsampling: (1,1)
///
/// Example: second and third planes in the planar YUV420P predefined format.
///	-	color channel: U or V channel
///	-	bits per pixel: 8
///	-	pixels per unit: 1
///	-	minimum downsampling: (2,2)
///	-	maximum downsampling: (2,2)
///
/// \ingroup BFC_Image

class BFC_Image_DLL PlaneFormat : virtual public SObject {

public :

	/// \brief Enumeration of various predefined types.

	enum Format {

		Unknown	= 0,		///< unknown format

		Gray_8_1_1_1_1_1,
		Gray_12_1_1_1_1_1,
		Gray_16_1_1_1_1_1,

		RGB_R_8_1_1_1_1_1,
		RGB_G_8_1_1_1_1_1,
		RGB_B_8_1_1_1_1_1,
		RGB_A_8_1_1_1_1_1,

		RGB_R_10_1_1_1_1_1,
		RGB_G_10_1_1_1_1_1,
		RGB_B_10_1_1_1_1_1,
		RGB_A_10_1_1_1_1_1,

		RGB_R_12_1_1_1_1_1,
		RGB_G_12_1_1_1_1_1,
		RGB_B_12_1_1_1_1_1,
		RGB_A_12_1_1_1_1_1,

		RGB_R_16_1_1_1_1_1,
		RGB_G_16_1_1_1_1_1,
		RGB_B_16_1_1_1_1_1,
		RGB_A_16_1_1_1_1_1,

		RGB_All_8_1_1_1_1_1,
		RGB_All_8_1_1_1_2_2,
		RGB_All_15_1_1_1_1_1,
		RGB_All_16_1_1_1_1_1,
		RGB_All_24_1_1_1_1_1,
		RGB_All_30_1_1_1_1_1,
		RGB_All_36_1_1_1_1_1,
		RGB_All_48_1_1_1_1_1,
		RGBA_All_32_1_1_1_1_1,

		YUV_Y_8_1_1_1_1_1,
		YUV_U_8_1_1_1_1_1,
		YUV_U_8_1_2_1_2_1,
		YUV_U_8_1_4_1_4_1,
		YUV_U_8_1_2_2_2_2,
		YUV_U_8_1_4_4_4_4,
		YUV_V_8_1_1_1_1_1,
		YUV_V_8_1_2_1_2_1,
		YUV_V_8_1_4_1_4_1,
		YUV_V_8_1_2_2_2_2,
		YUV_V_8_1_4_4_4_4,
		YUV_A_8_1_1_1_1_1,

		YUV_All_16_2_1_1_2_1,
		YUV_All_24_1_1_1_1_1,
		YUVA_All_32_1_1_1_1_1,

		RYUV_Y_16_1_1_1_1_1,
		RYUV_U_16_1_1_1_1_1,
		RYUV_V_16_1_1_1_1_1,
		RYUV_A_16_1_1_1_1_1,

		JYUV_Y_8_1_1_1_1_1,
		JYUV_U_8_1_1_1_1_1,
		JYUV_U_8_1_2_1_2_1,
		JYUV_U_8_1_4_1_4_1,
		JYUV_U_8_1_2_2_2_2,
		JYUV_U_8_1_4_4_4_4,
		JYUV_V_8_1_1_1_1_1,
		JYUV_V_8_1_2_1_2_1,
		JYUV_V_8_1_4_1_4_1,
		JYUV_V_8_1_2_2_2_2,
		JYUV_V_8_1_4_4_4_4,
		JYUV_A_8_1_1_1_1_1,

		MinFormat	= Gray_8_1_1_1_1_1,		///< minimum known format (incl.)
		MaxFormat	= JYUV_A_8_1_1_1_1_1 + 1	///< maximum known format (excl.)

	};

	/// \brief Creates a new PlaneFormat object, using the Unknown
	///	predefined format.

	PlaneFormat(
	) :
		idx	( Unknown ) {
	}

	/// \brief Creates a new PlaneFormat object, using format \a format.

	explicit PlaneFormat(
		const	Format		format
	) :
		idx	( format ) {
	}

	/// \brief Creates a new PlaneFormat object, using format whose index
	///	is \a index in the unique PlaneFormatRegistry.

	explicit PlaneFormat(
		const	Uint32		index
	) :
		idx	( index ) {
	}

	/// \brief Creates a new PlaneFormat, using the format univoquely
	///	identified by \a uuid.

	explicit PlaneFormat(
		const	UUId &		uuid
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
	/// \param bitsPerPixel
	///	Number of bits per pixel the format should use.
	///
	/// \param colorChannel
	///	Color channel of the predefined format (default: any color
	///	channel).

	PlaneFormat(
		const	Uint32		bitsPerPixel,
		const	ColorChannel	colorChannel
	);

	/// \brief Creates a new PlaneFormat object, which is a copy of
	///	\a other.
	///
	/// \param other
	///	The PlaneFormat to copy.

	PlaneFormat(
		const	PlaneFormat &	other
	) :
		SObject	(),
		idx	( other.idx ) {
	}

	/// \brief Replaces the content of this object by a copy of \a other.
	///
	/// \param other
	///	The PlaneFormat to copy.

	PlaneFormat & operator = (
		const	PlaneFormat &	other
	) {
		idx = other.idx;
		return *this;
	}

	/// \brief Resets this PlaneFormat to the Unknown predefined value.

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
		const	PlaneFormat &	other
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
		const	PlaneFormat &	other
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
	/// unique PlaneFormatRegistry.

	Uint32 getPredefined(
	) const {
		return idx;
	}

	/// \brief Returns the manifest of the format used by this object.

	const PlaneFormatManifest & getManifest(
	) const {
		return ( PlaneFormatRegistry::instance()->getManifest( idx ) );
	}

	/// \brief Returns the UUId (universal unique id) of the manifest
	///	used by this object.

	const UUId & getUUId(
	) const {
		return getManifest().getUUId();
	}

	/// \brief Returns the actual color channel(s).

	ColorChannel getColorChannel(
	) const {
		return getManifest().colorChannel;
	}

	/// \brief Returns the number of pixels per elementary unit of data.

	Uint32 getPixelsPerElement(
	) const {
		return getManifest().pixelsPerElt;
	}

	/// \brief Returns the number of bits per pixel.

	Uint32 getBitsPerPixel(
	) const {
		return getManifest().bitsPerPixel;
	}

	/// \brief Returns the number of bytes per pixel.
	///
	/// This value is the minimum number of bytes needed to store
	/// getBitsPerPixel() bits.

	Uint32 getBytesPerPixel(
	) const {
		return bitsToBytes( getBitsPerPixel() );
	}

	/// \brief Returns the number of bytes needed to represent a single
	///	line of data of a picture which is \a picWidth pixels wide.
	///
	/// This value is always a multiple of 4 (dword alignment).
	///
	/// \param picWidth
	///	Width of the original picture, in pixels. This value is
	///	adjusted if this PlaneFormat uses downsampling in the X
	///	direction.

	Uint32 getBytesPerLineIfPicture(
		const	Uint32		picWidth
	) const {
		return aligned( getBytesPerPixel()
		              * getWidth( picWidth ) );
	}

	Uint32 getBytesPerLineIfPlane(
		const	Uint32		picWidth
	) const {
		return aligned( getBytesPerPixel() * picWidth );
	}

	/// \brief Returns the number of bytes needed to represent a whole
	///	plane of data of a picture which is \a picWidth by
	///	\a picHeight pixels.
	///
	/// \param picWidth
	///	Width of the original picture, in pixels. This value is
	///	adjusted if this PlaneFormat uses downsampling in the X
	///	direction.
	///
	/// \param picHeight
	///	Height of the original picture, in pixels. This value is
	///	adjusted if this PlaneFormat uses downsampling in the Y
	///	direction.

	Uint32 getBytesIfPicture(
		const	Uint32		picWidth,
		const	Uint32		picHeight
	) const {
		return ( getBytesPerLineIfPicture( picWidth )
		       * getHeight( picHeight ) );
	}

	Uint32 getBytesIfPlane(
		const	Uint32		plnWidth,
		const	Uint32		plnHeight
	) const {
		return ( aligned( getBytesPerPixel() * plnWidth )
		       * plnHeight );
	}

	Bool checkBytesIfPicture(
		const	Uint32		picWidth,
		const	Uint32		picHeight,
		const	Uint32		bufLength
	) const {
		return checkBytesIfPlane(
			getWidth( picWidth ),
			getHeight( picHeight ),
			bufLength );
	}

	Bool checkBytesIfPlane(
		const	Uint32		plnWidth,
		const	Uint32		plnHeight,
		const	Uint32		bufLength
	) const {
		return ( ! plnHeight
		      || ( ( bufLength % plnHeight ) == 0
		        && ( bufLength / plnHeight ) >= aligned( getBytesPerPixel() * plnWidth )
		        && isAligned( bufLength / plnHeight ) ) );
	}

	/// \brief Returns the minimum downsampling factor in the X direction.

	Uint32 getMinXDownsampling(
	) const {
		return getManifest().minXDownsmpl;
	}

	/// \brief Returns the minimum downsampling factor in the Y direction.

	Uint32 getMinYDownsampling(
	) const {
		return getManifest().minYDownsmpl;
	}

	/// \brief Returns the maximum downsampling factor in the X direction.

	Uint32 getMaxXDownsampling(
	) const {
		return getManifest().maxXDownsmpl;
	}

	/// \brief Returns the maximum downsampling factor in the Y direction.

	Uint32 getMaxYDownsampling(
	) const {
		return getManifest().maxYDownsmpl;
	}

	/// \brief Returns the real width a Plane using this object should
	///	have if it was part of a Picture which was \a picWidth pixels
	///	wide.
	///
	/// \param picWidth
	///	Width of the original picture, in pixels. This value is
	///	adjusted if this PlaneFormat uses downsampling in the X
	///	direction.

	Uint32 getWidth(
		const	Uint32		picWidth
	) const {
		return ( picWidth / getManifest().minXDownsmpl );
	}

	/// \brief Returns the real height a Plane using this object should
	///	have if it was part of a Picture which was \a picHeight
	///	pixels high.
	///
	/// \param picHeight
	///	Height of the original picture, in pixels. This value is
	///	adjusted if this PlaneFormat uses downsampling in the Y
	///	direction.

	Uint32 getHeight(
		const	Uint32		picHeight
	) const {
		return ( picHeight / getManifest().minYDownsmpl );
	}

	Buffer toBuffer(
	) const {
		return ( getManifest().toBuffer() );
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

protected :

	/// \brief Converts a number of bits into a number of bytes, after
	///	proper padding with enough bits to get to a byte boundary
	///	(byte alignment).

	static Uint32 bitsToBytes(
		const	Uint32		bits
	) {
		return ( ( bits + 7 ) >> 3 );
	}

	/// \brief Aligns a number of bytes to the smallest integral multiple
	///	of 4 (dword alignment).

	static Uint32 aligned(
		const	Uint32		bytes
	) {
		return ( ( bytes + 3 ) & ( Uint32 )( -4 ) );
	}

	static Bool isAligned(
		const	Uint32		bytes
	) {
		return ( bytes == aligned( bytes ) );
	}

private :

	Uint32		idx;	///< Index in registry.


};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_PlaneFormat_H_

// ============================================================================

