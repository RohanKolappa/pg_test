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
//	BFC.Image.Picture.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Image_Picture_H_
#define _BFC_Image_Picture_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Image {

BFC_PTR_DECL( BFC_Image_DLL, Picture )

} // namespace Image
} // namespace BFC

// ============================================================================

#include "BFC.Dyn.VarUser.h"

#include "BFC.IO.ByteInputStreamPtr.h"
#include "BFC.IO.ByteOutputStreamPtr.h"

#include "BFC.Image.Plane.h"
#include "BFC.Image.PictureFormat.h"

#include "BFC.Mem.Reader.h"
#include "BFC.Mem.Writer.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Represents a picture made up of data Plane objects, and using a
///	specific PictureFormat.
///
/// \ingroup BFC_Image

class BFC_Image_DLL Picture : public Dyn::VarUser {

public :

	/// \brief Creates a new empty Picture (width and height are set to
	///	0, format is Unknown, no planes).

	Picture(
	);

	/// \brief Creates a new Picture of \a width by \a height
	///	pixels, using format \a format, and automatically creates
	///	a set of Plane objects.
	///
	/// \param width
	///	The new Picture width (in pixels).
	///
	/// \param height
	///	The new Picture height (in pixels).
	///
	/// \param format
	///	The format used by the new Picture.
	///
	/// \throws InvalidArgument
	///	If \a format is PictureFormat::Unknown.

	Picture(
		const	Uint32		width,
		const	Uint32		height,
		const	PictureFormat &	format,
		const	Bool		pClrDat = true
	);

	/// \brief Creates a new Picture of \a width by \a height
	///	pixels, using format \a format, and whose data is stored in
	///	data Plane objects grouped into array \a planes.
	///
	/// \param width
	///	The new Picture width (in pixels).
	///
	/// \param height
	///	The new Picture height (in pixels).
	///
	/// \param format
	///	The format used by the new Picture.
	///
	/// \param planes
	///	The data Plane objects of this new Picture.
	///
	/// \throws InvalidArgument
	///	If any incoherency is detected between the parameters.

	Picture(
		const	Uint32		width,
		const	Uint32		height,
		const	PictureFormat &	format,
		const	PlaneArray &	planes
	);

	/// \brief Creates a new Picture of \a width by \a height
	///	pixels, using format \a format, and whose unique 3 data
	///	planes are given by \a plane0, \a plane1, and
	///	\a plane2.
	///
	/// \param width
	///	The new Picture width (in pixels).
	///
	/// \param height
	///	The new Picture height (in pixels).
	///
	/// \param format
	///	The format used by the new Picture.
	///
	/// \param plane0
	///	The data of the first data Plane of this new Picture.
	///
	/// \param plane1
	///	The data of the second data Plane of this new Picture.
	///
	/// \param plane2
	///	The data of the third data Plane of this new Picture.
	///
	/// \throws InvalidArgument
	///	If any incoherency is detected between the parameters, for
	///	example because the format \a format doesn't use exactly
	///	3 Plane objects.

	Picture(
		const	Uint32		width,
		const	Uint32		height,
		const	PictureFormat &	format,
		const	Buffer &	plane0,
		const	Buffer &	plane1,
		const	Buffer &	plane2
	);

	/// \brief Creates a new Picture of \a width by \a height
	///	pixels, using format \a format, and whose unique 3 data
	///	Plane objects are given by \a plane0, \a plane1, and
	///	\a plane2.
	///
	/// \param width
	///	The new Picture width (in pixels).
	///
	/// \param height
	///	The new Picture height (in pixels).
	///
	/// \param format
	///	The format used by the new Picture.
	///
	/// \param plane0
	///	The first data Plane of this new Picture.
	///
	/// \param plane1
	///	The second data Plane of this new Picture.
	///
	/// \param plane2
	///	The third data Plane of this new Picture.
	///
	/// \throws InvalidArgument
	///	If any incoherency is detected between the parameters, for
	///	example because the format \a format doesn't use exactly
	///	3 Plane objects.

	Picture(
		const	Uint32		width,
		const	Uint32		height,
		const	PictureFormat &	format,
		const	Plane &		plane0,
		const	Plane &		plane1,
		const	Plane &		plane2
	);

	/// \brief Creates a new Picture of \a width by \a height
	///	pixels, using format \a format, and whose unique Plane is
	///	given by \a packed.
	///
	/// \param width
	///	The new Picture width (in pixels).
	///
	/// \param height
	///	The new Picture height (in pixels).
	///
	/// \param format
	///	The format used by the new Picture.
	///
	/// \param packed
	///	The data of the unique data Plane of this new Picture.
	///
	/// \throws InvalidArgument
	///	If any incoherency is detected between the parameters, for
	///	example because the format \a format uses more than 1
	///	Plane.

	Picture(
		const	Uint32		width,
		const	Uint32		height,
		const	PictureFormat &	format,
		const	Buffer &	packed
	);

	/// \brief Creates a new Picture of \a width by \a height
	///	pixels, using format \a format, and whose unique Plane is
	///	given by \a packed.
	///
	/// \param width
	///	The new Picture width (in pixels).
	///
	/// \param height
	///	The new Picture height (in pixels).
	///
	/// \param format
	///	The format used by the new Picture.
	///
	/// \param packed
	///	The unique data Plane of this new Picture.
	///
	/// \throws InvalidArgument
	///	If any incoherency is detected between the parameters, for
	///	example because the format \a format uses more than 1
	///	Plane.

	Picture(
		const	Uint32		width,
		const	Uint32		height,
		const	PictureFormat &	format,
		const	Plane &		packed
	);

	/// \brief Creates a new Picture, which is a copy of \a other.

	Picture(
		const	Picture &	other
	);

	/// \brief Replaces the content of this Picture by a copy of
	///	\a other.

	Picture & operator = (
		const	Picture &	 other
	);

	Bool operator == (
		const	Picture &	other
	) const;

	Bool operator != (
		const	Picture &	other
	) const;

	/// \brief Returns true iff this Picture has the same width and height
	///	and uses the same format as \a pOther.

	Bool isSimilarTo(
		const	Picture &	pOther
	) const {
		return ( getWidth()	== pOther.getWidth()
		      && getHeight()	== pOther.getHeight()
		      && getFormat()	== pOther.getFormat() );
	}

	/// \brief Resets this Picture to an "empty" state (dimensions are 0,
	///	format is unknown, no planes).

	void kill(
	);

	/// \brief Returns true iff this Picture is empty (no width or height,
	///	or no plane).

	Bool isEmpty(
	) const {
		return ( ! picW || ! picH || picP.isEmpty() );
	}

	/// \brief Returns true.
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	Bool isValid(
	) const {
		return true;
	}

	/// \brief Returns the width (in pixels) of this Picture.
	///
	/// This method returns the width of this Picture, expressed
	/// as a number of pixels.

	Uint32 getWidth(
	) const {
		return picW;
	}

	/// \brief Returns the height (in pixels) of this Picture.
	///
	/// This method returns the height of this Picture, expressed
	/// as a number of pixels.

	Uint32 getHeight(
	) const {
		return picH;
	}

	/// \brief Returns the format used by this Picture.

	const PictureFormat & getFormat(
	) const {
		return picF;
	}

	/// \brief Returns the number of bytes needed to represent this
	///	Picture.
	///
	/// This method returns a logical value, which may be different than
	/// the number of bytes actually used by this Picture.

	Uint32 getLength(
	) const {
		return picF.getBytesPerPicture( picW, picH );
	}

	/// \brief Returns the number of planes used by this Picture.
	///
	/// This is the same value that the one returned by a call to
	/// getFormat().getNbrPlanes().

	Uint32 getNbrPlanes(
	) const {
		return picP.size();
	}

	/// \brief Synonym of getNbrPlanes().
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	Uint32 getPlanesNumber(
	) const {
		return getNbrPlanes();
	}

	/// \brief Synonym of getNbrPlanes().
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.

	Uint32 getNumPlanes(
	) const {
		return getNbrPlanes();
	}

	/// \brief Replaces the actual plane whose index is \a picPlane by
	///	\a newPlane.
	///
	/// \param picPlane
	///	The plane to replace.
	///
	/// \param newPlane
	///	The new Plane to use.
	///
	/// \throws InvalidArgument
	///	If \a picPlane is greater or equal to the number of planes,
	///	or if this Picture uses an unknown format, or if the new
	///	plane uses a known format, which is incompatible with the
	///	format of this Picture.

	void setPlane(
		const	Uint32		picPlane,
		const	Plane &		newPlane
	);

protected :

	Plane & privGetPlane(
		const	Uint32		picPlane
	);

public :

	/// \brief Returns a read-only reference to the plane whose index is
	///	\a picPlane.
	///
	/// \throws InvalidArgument
	///	If \a picPlane is greater or equal to the number of planes.

	const Plane & getPlane(
		const	Uint32		picPlane
	) const;

	/// \brief Returns a writeable reference to the plane whose index is
	///	\a picPlane.
	///
	/// This method should be used with caution! It is mandatory NOT to
	/// modify any else but the data of the plane.
	///
	/// \throws InvalidArgument
	///	If \a picPlane is greater or equal to the number of planes.

	Plane & getPlane(
		const	Uint32		picPlane
	);

	/// \brief Returns the first (index 0) data Plane.
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.
	///
	/// \throws InvalidArgument
	///	If this Picture doesn't use any Plane.

	const Plane & getPlane0(
	) const;

	/// \brief Returns the second (index 1) data Plane.
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.
	///
	/// \throws InvalidArgument
	///	If this Picture doesn't use more than 1 Plane.

	const Plane & getPlane1(
	) const;

	/// \brief Returns the third (index 2) data Plane.
	///
	/// \note This method is obsolete and is only kept to let old code
	///	compile smoothly. Don't use it anymore.
	///
	/// \throws InvalidArgument
	///	If this Picture doesn't use more than 2 Planes.

	const Plane & getPlane2(
	) const;

	/// \brief Returns all Plane objects in one array.

	const PlaneArray & getPlanes(
	) const {
		return picP;
	}

	/// \brief Returns a Mem::UcharReader, with correct striding parameters
	///	set, and attached to the plane whose index is \a picPlane.
	///
	/// \throws InvalidArgument
	///	If \a picPlane is greater or equal to the number of planes.

	Mem::Reader getPlaneReader(
		const	Uint32		picPlane
	) const {
		return picP[ picPlane ].getDataReader();
	}

	/// \brief Returns a Mem::UcharWriter, with correct striding parameters
	///	set, and attached to the plane whose index is \a picPlane.

	Mem::Writer getPlaneWriter(
		const	Uint32		picPlane
	) {
		return picP[ picPlane ].getDataWriter();
	}

	/// \brief Converts this Picture to the new format \a format.
	///
	/// \throws CantConvert
	///	If the format used by this Picture cannot be understood (see
	///	testConvertible()).
	///
	/// \throws InvalidArgument
	///	If \a format cannot be used with the dimensions of this
	///	Picture (because of downsampling).

	void convertTo(
		const	PictureFormat &	format
	);

	void convertFrom(
		const	Picture &	pOriginal,
		const	PictureFormat &	pNewFormat
	);

	/// \brief Converts this Picture to some planar format.
	///
	/// If this Picture is not using some packed format (see the
	/// isPacked() method), then this method does nothing.
	///
	/// Otherwise, if this Picture is using the RGB color space, then
	/// this method converts it to the PictureFormat::RGB format.
	///
	/// Otherwise, if this Picture is using the YUV color space, then
	/// this method converts it to the PictureFormat::YUV444P format.
	///
	/// Otherwise, a CantConvert exception is thrown.
	///
	/// throws CantConvert
	///	If the format used by this Picture cannot be understood (see
	///	testConvertible()), or if none of the conditions above is met.

	void forcePlanar(
	);

	/// \brief Flips this picture leftside right.
	///
	/// \throws CantConvert
	///	If the format used by this Picture cannot be understood (see
	///	testConvertible()).

	void flipLR(
	);

	/// \brief Flips this Picture upside down.
	///
	/// \throws CantConvert
	///	If the format used by this Picture cannot be understood (see
	///	testConvertible()).

	void flipTB(
	);

	/// \brief Resizes this Picture to the new dimensions \a newWidth
	///	by \a newHeight.
	///
	/// \param newWidth
	///	New picture width (in pixels).
	///
	/// \param newHeight
	///	New picture height (in pixels).
	///
	/// \throws CantConvert
	///	If the format used by this Picture cannot be understood (see
	///	testConvertible()).
	///
	/// \throws InvalidArgument
	///	If the format used by this Picture is not compatible with at
	///	least one of the arguments (because of downsampling).

	void resize(
		const	Uint32		newWidth,
		const	Uint32		newHeight
	);

	/// \brief Rescales this Picture by downsampling all its planes by a
	///	factor 2 in both directions.

	void downscaleXY2(
	);

	/// \brief Rescales this Picture by downsampling all its planes by a
	///	factor 4 in both directions.

	void downscaleXY4(
	);

	/// \brief Returns a part of this Picture.
	///
	/// \param pStartX
	///	Horizontal start position.
	///
	/// \param pStartY
	///	Vertical start position.
	///
	/// The dimensions are automatically adjusted so the copied area
	/// is completely included in this Picture.

	Picture getClip(
		const	Uint32		pStartX,
		const	Uint32		pStartY,
		const	Uint32		pClipW,
		const	Uint32		pClipH
	) const;

	/// \brief Fills a part of this Picture with a copy of \a pClip.
	///
	/// \param pClip
	///	Data to copy, starting at position (\a pStartX, \a pStartY).
	///
	/// \param pStartX
	///	Horizontal start position.
	///
	/// \param pStartY
	///	Vertical start position.

	void setClip(
		const	Picture &	pClip,
		const	Uint32		pStartX,
		const	Uint32		pStartY
	);

	/// \brief Sets all pixels of this Picture to random values.

	void randomize(
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

	/// \brief Throws an InvalidArgument exception if any incoherency
	///	is found between the member variables.

	void checkValidness(
	);

	/// \brief Throws a CantConvert exception if this format is a user
	///	defined format, of if its the predefined Unknown format.

	void testConvertible(
	) const;

private :

	Uint32		picW;	///< Picture width (in pixels).
	Uint32		picH;	///< Picture height (in pixels).
	PictureFormat	picF;	///< Picture format.
	PlaneArray	picP;	///< Picture planes.

};

// ============================================================================

typedef Array< Picture > PictureArray;

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Picture_H_

// ============================================================================

