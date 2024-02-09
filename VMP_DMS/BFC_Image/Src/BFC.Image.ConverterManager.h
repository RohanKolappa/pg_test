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
//	BFC.Image.ConverterManager.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_ConverterManager_H_
#define _BFC_Image_ConverterManager_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.ConverterManagerPtr.h"

// ============================================================================

#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.List.h"
#include "BFC.TL.Matrix.h"

#include "BFC.Image.ConverterPtr.h"
#include "BFC.Image.ConverterStrategyPtr.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

class PictureFormat;

// ============================================================================

/// \brief Provides ConverterStrategy objects to convert between two PictureFormat.
///
/// First of all,
/// this (singleton) class is used by all the Converter objects as a
/// central point to register themselves.
///
/// Then,
/// when asked to return a strategy to convert from some PictureFormat to some
/// other PictureFormat, and if at least one new Converter has
/// registered itself since the last time such a strategy was requested,
/// it then tries to [re]compute a ConverterStrategy
/// for each pair (PictureFormat, PictureFormat) using all the
/// registered converters.
///
/// As of this writing, there are 30 different PictureFormat values, and a few
/// dozens Converter objects, represented by the arrows on the picture
/// below.
///
/// \image html ConverterManager_all.jpg
/// \image latex ConverterManager_all.eps
///
/// \ingroup BFC_Image

class BFC_Image_DLL ConverterManager : virtual public SObject {

protected :

	/// Creates a new ConverterManager.

	ConverterManager(
	);

public :

	/// \brief Returns the single instance of this class.
	///
	/// This method returns a smart pointer to the single instance of
	/// this class.
	///
	/// \note It is safe to keep a Master Smart Pointer to this object.

	static ConverterManagerPtr instance(
	);

	/// \brief Registration point for Converter objects.
	///
	/// This method should be used by every Converter object to
	/// register itself as a potentiel format converter.
	///
	/// \note This ConverterManager will keep a Master Smart Pointer
	///	to this converter, thus preventing its deletion before the
	///	destruction of this object.

	void addConverter(
			ConverterPtr	converter,
		const	Bool		pPreferred = false
	);

	/// Returns a ConverterStrategy that converts from \a iFmt to \a oFmt.

	/// This method returns a const reference to a ConverterStrategy
	/// suitable to convert a picture in format \a iFmt to the new
	/// format \a oFmt.
	///
	/// \throw InternalError
	/// -	The additional message will explain what went wrong.

	ConverterStrategyPtr getStrategyFor(
		const	PictureFormat &	iFmt,
		const	PictureFormat &	oFmt
	);

	/// Forces to recompute all conversion strategies.

	/// This method forces this object to rebuild its internal table
	/// of strategies.
	///
	/// \note As the building of this table is done automatically, there
	///	is no need to call this method yourself!

	void forcePopulate(
	);

	PictureFormat selectNearestFormat(
		const	PictureFormat &	pInputFormat,
		const	Array< PictureFormat > &
					pCandidates
	);

protected :

private :

	// helpers...

	void computeStrategies(
	);

	// internal state...

	FastCS		lockable;
	Bool		convertersChanged;

	typedef List< ConverterStrategyPtr >	ConverterStrategyList;
	typedef List< ConverterPtr >		ConverterList;
	typedef Matrix< ConverterStrategyPtr >	ConverterStrategyMatrix;

	ConverterList		converters;
	ConverterStrategyMatrix	strategies;

	// others...

	ConverterManager(
		const	ConverterManager&
	);

	ConverterManager& operator = (
		const	ConverterManager&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_ConverterManager_H_

// ============================================================================

