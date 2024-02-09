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
//	BFC.Image.ConverterStrategy.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_ConverterStrategy_H_
#define _BFC_Image_ConverterStrategy_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.ConverterStrategyPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.TL.List.h"

#include "BFC.Image.ConverterPtr.h"
#include "BFC.Image.PictureFormat.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

class Picture;

// ============================================================================

/// \brief Represents a succession of Converter objects.
///
/// Objects of this class represent successions of Converter objects
/// to be used (sequentially) to convert between two different PictureFormat,
/// thus allowing one to implement a limited set of format converters, and
/// then let the system (in this case, the unique ConverterManager),
/// compute suitable
/// ConverterStrategy objects to convert between any two formats.
///
/// \note It is guaranteed that a strategy will never include a converter
///	that would otherwise introduce
///	data loss because of unneeded/unwanted downsampling.
///
/// \ingroup BFC_Image

class BFC_Image_DLL ConverterStrategy : virtual public SObject {

public :

	/// \brief Creates a strategy that doesn't convert anything.

	ConverterStrategy(
	);

	/// \brief Creates a strategy that can be applied to pics in format
	///	\a iFmt.

	ConverterStrategy(
		const	PictureFormat		& iFmt
	);

	/// \brief Creates a new strategy, formed by concatenating \a __other,
	///	and Converter \a __converter.

	ConverterStrategy(
		const	ConverterStrategyPtr	& __other,
		const	ConverterPtr		& __converter
	);

	/// Creates a copy of strategy \a other.

	ConverterStrategy(
		const	ConverterStrategy	& other
	);

	/// Replaces the actual content by \a other.

	ConverterStrategy& operator = (
		const	ConverterStrategy	& other
	);

	/// Returns true iff this strategy has the same effect as \a other.

	Bool operator == (
		const	ConverterStrategy	& other
	) const;

	/// Returns true iff this strategy has not the same effect as \a other.

	Bool operator != (
		const	ConverterStrategy	& other
	) const;

	/// Returns the input format of this strategy.

	PictureFormat getIFormat(
	) const;

	/// \brief Returns the output format of this strategy.
	///
	/// This method returns the output format obtained by applying this
	/// strategy. This format is the same as the input format iff this
	/// strategy is empty.

	PictureFormat getOFormat(
	) const;

	/// Returns true iff this strategy is empty (input format == output format).

	Bool isEmpty(
	) const;

	/// Returns the number of converters applied by this strategy.

	Uint32 getLength(
	) const;

	/// Returns true iff converter \a conv can be appended at the end of this strategy.

	/// This method returns true iff the converter \a conv can be
	/// appended at the end of this strategy. This is the case if all
	/// the following conditions are met:
	/// -	\a conv can convert pictures represented by the format
	///	returned by invoking the getOFormat() method (continuity);
	/// -	applying \a conv should not produce a picture in a format
	///	already used by this strategy (no looping);
	/// -	the potential new strategy, made by concatenating this
	///	strategy and the new converter \a conv, should not contain
	///	intermediary formats using a downscaling factor superior to
	///	the ones of the initial and final formats (no data loss).

	Bool canAdd(
			ConverterPtr	conv
	) const;

//	/// Appends converter \a conv at the end of this strategy.
//
//	/// This method appends the new converter \a conv at the end of this
//	/// strategy.
//	///
//	/// \throw InvalidFormat
//	/// -	\a conv cannot be appended (see #canAdd).
// 
//	void add(
//			ConverterPtr	conv
//	);

	/// Applies this strategy to picture \a iPic, returning a new Picture.

	Picture applyTo(
		const	Picture &	iPic
	);

	void applyBetween(
		const	Picture &	iPic,
			Picture &	oPic
	);

	/// Returns a textual description of this strategy.

	Buffer toBuffer(
	) const;

private :

	struct Entry{
		Bool			forw;
		ConverterPtr		conv;
	};

	typedef List< Entry >	EntryList;

	PictureFormat		iFormat;
	EntryList		entries;

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_ConverterStrategy_H_

// ============================================================================

