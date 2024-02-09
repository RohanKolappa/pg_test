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
//	BFC.Image.Plane.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Image_Plane_H_
#define _BFC_Image_Plane_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"

#include "BFC.Dyn.VarUser.h"

#include "BFC.IO.ByteInputStreamPtr.h"
#include "BFC.IO.ByteOutputStreamPtr.h"

#include "BFC.Image.PlaneFormat.h"

#include "BFC.Mem.Block.h"
#include "BFC.Mem.Reader.h"
#include "BFC.Mem.Writer.h"

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Represents (the data of) one or more components of a Picture.
///
/// A Plane object is used to represent the data associated with 1
/// (for planar or grayscale pics) or more (packed pictures) components of a
/// Picture object.
///
/// A Plane contains \b height lines of samples, each line being
/// \b width samples wide.
///
/// A sample is simply an unsigned integer value, coded using
/// <b>bits per sample</b> bits (e.g. 8, 15, 16, 24, 32, 64, ...).
/// What those samples represent is to be specified using the PlaneFormat
/// property.
///
/// All those samples are stored in a big Mem::Block, where
/// each sample has to start on a byte boundary. This means that samples
/// on 15 bits physically occupy 2 bytes.
///
/// Finally, each line of samples in this big buffer start on a long word
/// boundary (32-bits).
///
/// \ingroup BFC_Image

class BFC_Image_DLL Plane : public Dyn::VarUser {

public :

	/// \brief Creates a new empty Plane (width and height are set to
	///	0, format is Unknown).

	Plane(
	);

	/// \brief Creates a new Plane of \a pPlaneW by \a pPlaneH
	///	pixels, using \a pBitsPP bits per pixel.
	///
	/// This method is obsolete, and should not be used in new code
	/// anymore, as it doesn't allow one to communicate some essential
	/// information about the plane format (e.g. downsampling).
	///
	/// A new Mem::Block is automatically allocated to hold the data. The
	/// length (in bytes) of this block is given by:
	/// Image::PlaneFormat( pBitsPP ).getBytes( pPlaneW, pPlaneH ).
	///
	/// \param pPlaneW
	///	Width (in pixels) of this Plane, i.e. AFTER any optional
	///	downsampling (which is not communicated).
	///
	/// \param pPlaneH
	///	Height (in pixels) of this Plane, i.e. AFTER any optional
	///	downsampling (which is not communicated).
	///
	/// \param pBitsPP
	///	Number of bits per pixel used by this Plane.
	///
	/// Example: the following code creates a 8x10 Plane, with pixels
	///	coded on 14 bits. Memory is automatically allocated.
	///
	/// \code
	///	Uint32 bpp = 14; // bits per pixel
	///	Uint32 plw = 8; // width
	///	Uint32 plh = 10; // height
	///	Plane p( plw, plh, bpp );
	/// \endcode

	Plane(
		const	Uint32		pPlaneW,
		const	Uint32		pPlaneH,
		const	Uint32		pBitsPP,
		const	Bool		pClrDat = true
	);

	/// \brief Creates a new Plane of \a pPlaneW by \a pPlaneH
	///	pixels, using \a pBitsPP bits per pixel, and whose
	///	data is given by \a pPlaneD.
	///
	/// This method is obsolete, and should not be used in new code
	/// anymore, as it doesn't allow one to communicate some essential
	/// information about the plane format (e.g. downsampling).
	///
	/// \param pPlaneD
	///	Plane data.
	///
	/// \param pPlaneW
	///	Width (in pixels) of this Plane, i.e. AFTER any optional
	///	downsampling (which is not communicated).
	///
	/// \param pPlaneH
	///	Height (in pixels) of this Plane, i.e. AFTER any optional
	///	downsampling (which is not communicated).
	///
	/// \param pBitsPP
	///	Number of bits per pixel used by this Plane.
	///
	/// \throws InvalidArgument
	///	If the length of \a pPlaneD is different than the
	///	the value given by:
	///	Image::PlaneFormat( pBitsPP ).getBytesIfPlane( pPlaneW, pPlaneH ).
	///
	/// Example: the following code creates a 8x10 Plane, with pixels
	///	coded on 14 bits, and using a copy-on-write buffer of
	///	appropriate length as data.
	///
	/// \code
	///	Uint32 bpp = 14; // bits per pixel
	///	Uint32 plw = 8; // width
	///	Uint32 plh = 10; // height
	///	Uint32 len = PlaneFormat( bpp ).getBytesIfPlane( plw, plh ); // == 160
	///	Buffer buf;
	///	buf.resize( len );
	///	// fill buf here
	///	Plane p( buf, plw, plh, bpp );
	/// \endcode

	Plane(
		const	Buffer &	pPlaneD,
		const	Uint32		pPlaneW,
		const	Uint32		pPlaneH,
		const	Uint32		pBitsPP
	);

	/// \brief Creates a new Plane using format \a pPlaneF, whose
	///	width is given by
	///	( pPictureW / pPlaneF.getXDownsampling() ),
	///	and whose height is given by
	///	( pPictureH / pPlaneF.getYDownsampling() ).
	///
	/// \param pPictureW
	///	Width (in pixels) BEFORE the optional downsampling specified
	///	in \a pPlaneF, i.e. width of the original Picture.
	///
	/// \param pPictureH
	///	Height (in pixels) BEFORE the optional downsampling specified
	///	in \a pPlaneF, i.e. height of the original Picture.
	///
	/// \param pPlaneF
	///	The format to be used by this Plane.
	///	
	/// A new Mem::Block is automatically allocated to hold the data. The
	/// length (in bytes) of this block is given by:
	/// pPlaneF.getBytesIfPicture( pPictureW, pPictureH ).
	///
	/// Example: the following code creates a 4x5 Plane.
	///
	/// \code
	///	Uint32 bpp = 8; // bits per pixel
	///	Uint32 xdown = 2; // x downsampling
	///	Uint32 ydown = 2; // y downsampling
	///	PlaneFormat fmt( ColorChannel::Unknown, bpp, xdown, ydown );
	///	Plane p( 8, 10, fmt );
	/// \endcode

	Plane(
		const	Uint32		pPictureW,
		const	Uint32		pPictureH,
		const	PlaneFormat &	pPlaneF,
		const	Bool		pClrDat = true
	);

	/// \brief Creates a new Plane using format \a pPlaneF, whose
	///	width is given by
	///	( pPictureW / pPlaneF.getXDownsampling() ),
	///	whose height is given by
	///	( pPictureH / pPlaneF.getYDownsampling() ),
	///	and whose data is given by \a pPlaneD.
	///
	/// \param pPictureW
	///	Width (in pixels) BEFORE the optional downsampling specified
	///	in \a pPlaneF, i.e. width of the original Picture.
	///
	/// \param pPictureH
	///	Height (in pixels) BEFORE the optional downsampling specified
	///	in \a pPlaneF, i.e. height of the original Picture.
	///
	/// \param pPlaneF
	///	The format to be used by this Plane.
	///
	/// \param pPlaneD
	///	Plane data.
	///
	/// \throws InvalidArgument
	///	If the length of \a pPlaneD is different than the
	///	the value given by:
	///	pPlaneF.getBytesIfPicture( pPictureW, pPictureH ).
	///
	/// Example: the following code creates a 4x5 Plane, with pixels
	///	coded on 14 bits, and using a copy-on-write block of
	///	appropriate length as data.
	///
	/// \code
	///	Uint32 bpp = 14; // bits per pixel
	///	Uint32 pcw = 8; // picture width
	///	Uint32 pch = 10; // picture height
	///	Uint32 xdown = 2; // x downsampling
	///	Uint32 ydown = 2; // y downsampling
	///	PlaneFormat fmt( ColorChannel::Unknown, bpp, xdown, ydown );
	///	Uint32 len = fmt.getBytesIfPicture( pcw, pch ); // == 40
	///	Block blk( len );
	///	// fill blk here
	///	Plane p( pcw, pch, fmt, blk );
	/// \endcode

	Plane(
		const	Uint32		pPictureW,
		const	Uint32		pPictureH,
		const	PlaneFormat &	pPlaneF,
		const	Mem::Block &	pPlaneD
	);

	/// \brief Creates a new Plane, which is a copy of \a pOther.

	Plane(
		const	Plane &		pOther
	);

	/// \brief Replaces the content of this object by a copy of \a pOther.

	Plane & operator = (
		const	Plane &		pOther
	);

	Bool operator == (
		const	Plane &		pOther
	) const;

	Bool operator != (
		const	Plane &		pOther
	) const {
		return ( ! operator == ( pOther ) );
	}

	/// \brief Resets this Plane to an "empty" state (dimensions are 0,
	///	format is unknown, no data).

	void kill(
	);

	/// \brief Returns true iff this Plane doesn't contain any data.

	Bool isEmpty(
	) const {
		return ( planeW == 0 || planeH == 0 );
	}

	Uint32 getWidth(
	) const {
		return planeW;
	}

	Uint32 getHeight(
	) const {
		return planeH;
	}

	void setFormat(
		const	PlaneFormat &	pFormat
	);

	const PlaneFormat & getFormat(
	) const {
		return planeF;
	}

	Uint32 getBitsPerSample(
	) const {
		return planeF.getBitsPerPixel();
	}

	Uint32 getBytesPerSample(
	) const {
		return planeF.getBytesPerPixel();
	}

	Uint32 getBytesPerLine(
	) const {
		return ( planeH ? planeD.getLength() / planeH : 0 );
	}

	ColorChannel getType(
	) const {
		return planeF.getColorChannel();
	}

//	/// \brief Returns the data Block used internally to store the plane
//	///	data.
//	///
//	/// This method allows one to get access (in read only mode) to the
//	/// internal data representation used by this object.
//	///
//	/// The way this object uses the data Block is not exposed, and using
//	/// this method directly is discouraged.
//	///
//	/// If you want to read the logical data of this Plane, use the
//	/// getDataReader() method, which is beter suited for this purpose.
//	///
//	/// \note There is no "setDataBlock()" method! Use a combination of
//	///	the copy operator, and one of the constructors instead!
//
//	const Mem::Block & getDataBlock(
//	) const {
//		return planeD;
//	}

	/// \brief Returns a Mem::Reader, suitable for reading this object.
	///
	/// The returned Reader has been initialized with the following values:
	/// -	element length (see Mem::Reader::getEltLength()): set to
	///	getFormat().getBytesPerPixel();
	/// -	dimensions (see Mem::Reader::getNbrCols() and
	///	Mem::Reader::getNbrRows()): set to getWidth() and getHeight().

	Mem::Reader getDataReader(
	) const;

	/// \brief Returns a Mem::Writer, suitable for writing this object.
	///
	/// The returned Reader has been initialized with the following values:
	/// -	element length (see Mem::Reader::getEltLength()): set to
	///	getFormat().getbytesPerPixel();
	/// -	dimensions (see Mem::Reader::getNbrCols() and
	///	Mem::Reader::getNbrRows()): set to getWidth() and getHeight().

	Mem::Writer getDataWriter(
	);

	/// \brief Returns a part of this Plane.
	///
	/// \param pClip
	///	Preinitialized Plane to fill with data from this Plane.
	///
	/// \param pPicStartX
	///	Horizontal start position, in picture coordinates.
	///
	/// \param pPicStartY
	///	Vertical start position, in picture coordinates.

	void getClip(
			Plane &		pClip,
		const	Uint32		pPicStartX,
		const	Uint32		pPicStartY
	) const;

	/// \brief Fills a part of this Plane with a copy of \a pClip.
	///
	/// \param pClip
	///	Data to copy, starting at position (\a pPicStartX,
	///	\a pPicStartY).
	///
	/// \param pPicStartX
	///	Horizontal start position, in picture coordinates.
	///
	/// \param pPicStartY
	///	Vertical start position, in picture coordinates.

	void setClip(
		const	Plane &		pClip,
		const	Uint32		pPicStartX,
		const	Uint32		pPicStartY
	);

	/// \brief Returns a copy-on-write copy of the data.
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	Buffer getData(
	) const {
		return planeD.toBuffer();
	}

	/// \brief Returns the minimum value of all pixels contained in this
	///	Plane.

	Uint32 getMin(
	) const;

	/// \brief Returns the maximum value of all pixels contained in this
	///	Plane.

	Uint32 getMax(
	) const;

	/// \brief Returns true iff this Plane is \a pPlaneW by pPlaneH
	///	pixels, and each pixels is coded using \a pBitsPP bits.

	Bool is(
		const	Uint32		pPlaneW,
		const	Uint32		pPlaneH,
		const	Uint32		pBitsPP
	) const {
		return ( planeW == pPlaneW
		      && planeH == pPlaneH
		      && planeF.getBitsPerPixel() == pBitsPP );
	}

	/// \brief Returns true iff this Plane has the same dimensions
	///	as \a pOther.

	Bool sameSize(
		const	Plane		& pOther
	) const {
		return ( planeW == pOther.planeW
		      && planeH == pOther.planeH );
	}

	/// \brief Downsamples this Plane by a factor 2 in the X direction.

	void downsampleX2(
	);

	/// \brief Downsamples this Plane by a factor 4 in the X direction.

	void downsampleX4(
	);

	/// \brief Downsamples this Plane by a factor 2 in the Y direction.

	void downsampleY2(
	);

	/// \brief Downsamples this Plane by a factor 4 in the Y direction.

	void downsampleY4(
	);

	/// \brief Downsamples this Plane by a factor 2 in both directions.

	void downsampleXY2(
	);

	/// \brief Downsamples this Plane by a factor 4 in both directions.

	void downsampleXY4(
	);

	/// \brief Upsamples this Plane by a factor 2 in the X direction.

	void upsampleX2(
	);

	/// \brief Upsamples this Plane by a factor 4 in the X direction.

	void upsampleX4(
	);

	/// \brief Upsamples this Plane by a factor 2 in the Y direction.

	void upsampleY2(
	);

	/// \brief Upsamples this Plane by a factor 4 in the Y direction.

	void upsampleY4(
	);

	/// \brief Upsamples this Plane by a factor 2 in both directions.

	void upsampleXY2(
	);

	/// \brief Upsamples this Plane by a factor 4 in both directions.

	void upsampleXY4(
	);

	/// \brief Flips this picture leftside right.

	void flipLR(
	);

	/// \brief Flips this Picture upside down.

	void flipTB(
	);

	/// \brief Resizes this Plane to the new dimensions \a pNewWidth
	///	by \a pNewHeight.

	void resize(
		const	Uint32		pNewWidth,
		const	Uint32		pNewHeight
	);

	/// \brief Sets all pixels of this Plane to random values.

	void randomize(
	);

	/// \brief Returns the mean value of 3 planes.

	static Plane getMean(
		const	Plane &		pPlane0,
		const	Plane &		pPlane1,
		const	Plane &		pPlane2
	);

	/// \brief Returns the absolute difference between 2 planes.

	static Plane getAbsDiff(
		const	Plane &		pPlane0,
		const	Plane &		pPlane1
	);

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

protected :

private :

	class Axis {
	public :
		Axis(
			const	Uint32		pUsr
		) :
			bef	( 0 ),
			usr	( pUsr ),
			aft	( 0 )
		{
		}
		operator Uint32(
		) const {
			return usr;
		}
	private :
		Uint32	bef;
		Uint32	usr;
		Uint32	aft;
	};

//	Uint32		planeW;		///< Width (in pixels).
//	Uint32		planeH;		///< Height (in pixels).
	Axis		planeW;		///< Width (in pixels).
	Axis		planeH;		///< Height (in pixels).
	PlaneFormat	planeF;		///< Format specification.
	Mem::Block	planeD;		///< Data.

};

// ============================================================================

/// \brief Array of Plane objects.
///
/// \ingroup BFC_Image

typedef CompArray< Plane > PlaneArray;

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Plane_H_

// ============================================================================

