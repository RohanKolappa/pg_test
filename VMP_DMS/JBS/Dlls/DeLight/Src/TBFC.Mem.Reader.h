// ============================================================================
// 
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
//	TBFC.Mem.Reader.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Mem_Reader_H_
#define _TBFC_Mem_Reader_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Exception.h"

#include "TBFC.Mem.Block.h"

// ============================================================================

namespace TBFC {
namespace Mem {

// ============================================================================

/// \brief Base class of all memory readers.
///
/// A Reader object is basically a temporary object attached to a data block,
/// and which grants read-only access to this block.
///
/// Various subclasses exist to help access the data block as if it was
/// containing elements of a particular length (in bytes). Examples:
/// UcharReader, Uint24Reader, ...
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Reader {

public :

	/// \brief Creates a new Reader, not attached to any Block.

	Reader(
	);

	/// \brief Creates a new Reader, attached to the Branch of Block
	///	\a block, and using elements T of size \a eltLength.
	///
	/// The Block is seen as containing a single row of as many elements
	/// of type T as possible.
	///
	/// Example: the following code creates a 32-bytes long Block, and
	/// attaches a Reader to it, which will see it as a row of 10
	/// elements of size 3, terminated by a 2-bytes padding:
	/// \code
	///	Block	block( 32 );
	///	Reader	reader( block, 3 );
	/// \endcode

	Reader(
		const	Block		& block,
		const	Uint32		eltLength
	);

	/// \brief Creates a new Reader, attached to the Branch of Block
	///	\a block, and using elements T of size \a eltLength.
	///
	/// The Block is seen as containing \a nbrOfRows rows of \a nbrOfCols
	/// elements of type T.
	///
	/// Example: the following code creates a 32-bytes long Block, and
	/// attaches a Reader to it, which will see it as 4 rows of 2 elements
	/// of size 3, each row being terminated by a 2-bytes padding:
	/// \code
	///	Block	block( 32 );
	///	Reader	reader( block, 3, 2, 4 );
	/// \endcode
	///
	/// \throws InvalidArgument
	///	If the parameters are incompatible with the Block length.

	Reader(
		const	Block		& block,
		const	Uint32		eltLength,
		const	Uint32		nbrOfCols,
		const	Uint32		nbrOfRows
	);

	/// \brief Creates a new Reader, attached to the same Branch as
	///	\a other.
	///
	/// The element length and the dimensions are inherited from
	/// \a other, and the cursor position is set to the origin.

	Reader(
		const	Reader		& other
	);

	/// \brief Creates a new Reader, attached to the same Branch as
	///	\a other.
	///
	/// Settings are first inherited from \a other (element length
	/// and dimensions). Then the element
	/// length is set to \a eltLength, and the number of columns
	/// is adapted accordingly. The cursor position is set to the
	/// origin.
	///
	/// \throws InvalidArgument
	///	If the new element length is incompatible with the settings
	///	of \a other (ess. different padding).

	Reader(
		const	Reader		& other,
		const	Uint32		eltLength
	);

	/// \brief Destroys this object, freeing the data block if this reader
	///	was the last object referencing it.

	/* virtual */ ~Reader(
	);

	/// \brief Suppresses the reference to the current Branch (if any),
	///	and replaces it by the Branch referenced by \a other.

	Reader & operator = (
		const	Reader		& other
	);

	/// \brief Sets the dimensions.
	///
	/// The cursor is repositioned on the origin (0,0).
	///
	/// \param nbrOfCols
	///	Number of data elements (each of size getSizeOfElement() bytes)
	///	per data row.
	///
	/// \param nbrOfRows
	///	Number of data rows.

	void setDimensions(
		const	Uint32		nbrOfCols,
		const	Uint32		nbrOfRows
	);

	/// \brief Returns true iff the cursor is still in the
	///	readable area.

	operator Bool (
	) const {
		return ( mCursorIdx >= 0 && ( Uint32 )mCursorIdx < mNbrOfElts );
	}

	/// \brief Repositions the cursor on the first readable
	///	element of the first data row.

	void gotoFirst(
	) {
		gotoElement( 0, 0 );
	}

	/// \brief Repositions the cursor on the first non readable
	///	element.

	void gotoLast(
	) {
		gotoElement( 0, mNbrOfRows );
	}

	/// \brief Repositions the cursor on the element whose  absolute
	///	index is given by the pair ( \a x, \a y).
	///
	/// \param x
	///	Index in the data row. Must be < getNbrCols().
	///
	/// \param y
	///	Index of the data row. Must be <= getNbrRows(). If
	///	== getNbrRows(), then x must be == 0 (this later
	///	case is equivalent to calling gotoLast()).

	void gotoElement(
		const	Uint32		x,
		const	Uint32		y
	);

	/// \brief Repositions the cursor on the element whose  absolute
	///	index is given by index.
	///
	/// \param index
	///	Index of the element. Must be <= getNbrElements().

	void gotoElement(
		const	Uint32		index
	);

	/// \brief Returns the size of the elements read by this
	///	object (in bytes).

	Uint32 getEltLength(
	) const {
		return mEltLength;
	}

	/// \brief Synonym of getEltLength().

	Uint32 getSizeOfElement(
	) const {
		return mEltLength;
	}

	/// \brief Returns the length \b L of the attached Block (in bytes,
	///	including any padding).

	Uint32 getBlockLength(
	) const {
		return mBlkLength;
	}

	/// \brief Returns the right padding \b Pr (in bytes).

	Uint32 getRowPadding(
	) const {
		return mRowPaddng;
	}

	/// \brief Returns the number \b M of data elements per data row.

	Uint32 getNbrCols(
	) const {
		return mNbrOfCols;
	}

	/// \brief Returns the number \b N of data rows.

	Uint32 getNbrRows(
	) const {
		return mNbrOfRows;
	}

	/// \brief Returns the total number \b E of data elements.

	Uint32 getNbrElements(
	) const {
		return mNbrOfElts;
	}

	/// \brief Returns the total number of bytes occupied by the data
	///	elements ( == M.N.T ).

	Uint32 getDataLength(
	) const {
		return ( mNbrOfCols * mNbrOfRows * mEltLength );
	}

	/// \brief Returns the number \b S of bytes in each row (incl.
	///	padding).

	Uint32 getRowLength(
	) const {
		return mRowLength;
	}

	/// \brief [Obsolete] Returns 0.

	Uint32 getBufferPadding(
	) const {
		return 0;
	}

	/// \brief [Obsolete] Returns 0.

	Uint32 getTopPadding(
	) const {
		return 0;
	}

	/// \brief [Obsolete] Returns 0.

	Uint32 getBottomPadding(
	) const {
		return 0;
	}

	/// \brief [Obsolete] Returns 0.

	Uint32 getLeftPadding(
	) const {
		return 0;
	}

	/// \brief [Obsolete] Synonym of getRowPadding().

	Uint32 getRightPadding(
	) const {
		return mRowPaddng;
	}

	/// \brief [Obsolete] Synonym of getNbrRows().

	Uint32 getNbrStrides(
	) const {
		return mNbrOfRows;
	}

	/// \brief [Obsolete] Synonym of getNbrCols().

	Uint32 getNbrElementsPerStride(
	) const {
		return mNbrOfCols;
	}

	/// \brief [Obsolete] Synonym of getRowLength().

	Uint32 getStrideLength(
	) const {
		return mRowLength;
	}

	/// \brief Repositions the cursor on the next element.

	void gotoNext(
	) {
		mCursorPtr += mEltLength;
		mCursorIdx++;
		if ( mCursorPtr >= mRowMaxPtr ) {
			// Goto first element of next row!
			mCursorPtr += mRowPaddng;
			// Recompute boundaries too!
			mRowMinPtr += mRowLength;
			mRowMaxPtr += mRowLength;
		}
	}

	/// \brief Repositions the cursor on the previous element.

	void gotoPrevious(
	) {
		mCursorPtr -= mEltLength;
		mCursorIdx--;
		if ( mCursorPtr < mRowMinPtr ) {
			// Goto last element of previous row!
			mCursorPtr -= mRowPaddng;
			// Recompute boundaries too!
			mRowMinPtr -= mRowLength;
			mRowMaxPtr -= mRowLength;
		}
	}

	/// \brief Repositions the cursor on the next element.

	void operator ++ (
	) {
		gotoNext();
	}

	/// \brief Repositions the cursor on the next element of type T.

	void operator ++ (
			int
	) {
		gotoNext();
	}

	/// \brief Repositions the cursor on the previous element of type T.

	void operator -- (
	) {
		gotoPrevious();
	}

	/// \brief Repositions the cursor on the previous element of type T.

	void operator -- (
			int
	) {
		gotoPrevious();
	}

	/// \brief Returns the address of the first byte of the Block read by
	///	this Reader.
	///
	/// The address returned by this method is guaranteed not to change
	/// during the lifetime of this object.

	const void * getBlockAddress(
	) const {
		return ( const void * )mBlkMinPtr;
	}

	/// \brief Returns the address of the first byte of the first data
	///	element of the row \a y.
	///
	/// The address returned by this method is guaranteed not to change
	/// during the lifetime of this object.

	const void * getRowAddress(
		const	Uint32		y
	) const {
		return ( const void * )( mBlkMinPtr + y * mRowLength );
	}

	/// \brief Returns the address of the first byte of the data
	///	element \a x located in row \a y. 
	///
	/// The address returned by this method is guaranteed not to change
	/// during the lifetime of this object.

	const void * getElementAddress(
		const	Uint32		x,
		const	Uint32		y
	) const {
		return ( const void * )(
			mBlkMinPtr
			+ y * mRowLength
			+ x * mEltLength );
	}

	/// \brief Returns the current address in the Block read by this
	///	Reader.
	///
	/// The address returned by this method is guaranteed to remain valid
	/// during the lifetime of this object.

	const void * getCurrentAddress(
	) const {
		return ( const void * )mCursorPtr;
	}

	BASE_CLASS_GEN_EXCEPTION( "TBFC.Mem.Reader" );

	CLASS_EXCEPTION( InvalidArgument, "Invalid argument(s)" );
	CLASS_EXCEPTION( SegFault, "Segmentation fault" );

protected :

	/// \brief Pseudo copy operator, attaching this Reader to the same
	///	Branch as the other Reader \a other, and using elements T
	///	of size \a eltLength.

	void doCopy(
		const	Reader		& other,
		const	Uint32		eltLength
	);

	// Helpers...

	const void * checkAndGetAddr(
	) const {
#if 0
// FIXME: Comment this test in release mode!
		if ( mCursorIdx < 0 || ( Uint32 )mCursorIdx >= mNbrOfElts ) {
			throw SegFault();
		}
#endif
		return ( const void * )mCursorPtr;
	}

	const void * checkAndGetAddr(
		const	Uint32		offset
	) const {
		Int32 k = mCursorIdx + offset;
#if 0
// FIXME: Comment this test in release mode!
		if ( k < 0 || ( Uint32 )k >= mNbrOfElts ) {
			throw SegFault();
		}
#endif
		return ( const void * )(
			mBlkMinPtr
			+ ( k / mNbrOfCols ) * mRowLength
			+ ( k % mNbrOfCols ) * mEltLength );
	}

	const void * checkAndGetAddr(
		const	Uint32		x,
		const	Uint32		y
	) const {
		return checkAndGetAddr( x + y * mNbrOfCols );
	}

private :

	// Helpers...

	void initBlock(
			Branch		* branchPtr,
		const	Uint32		eltLength
	);

	// From constructor...

	Branch *	mBranchPtr;	///< Pointer to the shared Branch.
	Uint32		mBlkLength;	///< Buffer length (in bytes, incl. padding).
	Uint32		mRowLength;	///< Stride length (in bytes, incl. padding).
	Uint32		mRowPaddng;	///< Stride padding (in bytes).
	Uint32		mEltLength;	///< Sizeof( typename T ).
	Uint32		mNbrOfElts;	///< Number of data elements.
	Uint32		mNbrOfCols;	///< Number of data elements per data row.
	Uint32		mNbrOfRows;	///< Number of data rows.
	const Uchar *	mBlkMinPtr;	///< Address of start of block.

	// Current element (cursor)...

	const Uchar *	mRowMinPtr;	///< Address of first element in current data row.
	const Uchar *	mRowMaxPtr;	///< Address of last+1 element in current data row.
	const Uchar *	mCursorPtr;	///< Address of current element (cursor).
	Int32		mCursorIdx;	///< Index of current element (cursor).

};

// ============================================================================

/// \brief Specialized Reader, working with Uchar elements (1 byte).
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL UcharReader : public Reader {

public :

	UcharReader(
	) {
	}

	UcharReader(
		const	Block		& block
	) : Reader( block, 1 ) {
	}

	UcharReader(
		const	UcharReader	& other
	) : Reader( other, 1 ) {
	}

	UcharReader(
		const	Reader		& other
	) : Reader( other, 1 ) {
	}

	UcharReader & operator = (
		const	Reader		& other
	) {
		doCopy( other, 1 );
		return * this;
	}

	Uchar operator * (
	) const {
		return * ( const Uchar * )checkAndGetAddr();
	}

	Uchar operator [] (
		const	Uint32		index
	) const {
		return * ( const Uchar * )checkAndGetAddr( index );
	}

	Uchar operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Uchar * )checkAndGetAddr( x, y );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Uint16 elements (2 bytes).
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Uint16Reader : public Reader {

public :

	Uint16Reader(
	) {
	}

	Uint16Reader(
		const	Block		& block
	) : Reader( block, 2 ) {
	}

	Uint16Reader(
		const	Uint16Reader	& other
	) : Reader( other, 2 ) {
	}

	Uint16Reader(
		const	Reader		& other
	) : Reader( other, 2 ) {
	}

	Uint16Reader & operator = (
		const	Reader		& other
	) {
		doCopy( other, 2 );
		return * this;
	}

	Uint16 operator * (
	) const {
		return * ( const Uint16 * )checkAndGetAddr();
	}

	Uint16 operator [] (
		const	Uint32		index
	) const {
		return * ( const Uint16 * )checkAndGetAddr( index );
	}

	Uint16 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Uint16 * )checkAndGetAddr( x, y );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with (logical) Uint24 elements
///	(3 bytes).
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Uint24Reader : public Reader {

public :

	Uint24Reader(
	) {
	}

	Uint24Reader(
		const	Block		& block
	) : Reader( block, 3 ) {
	}

	Uint24Reader(
		const	Uint24Reader	& other
	) : Reader( other, 3 ) {
	}

	Uint24Reader(
		const	Reader		& other
	) : Reader( other, 3 ) {
	}

	Uint24Reader & operator = (
		const	Reader		& other
	) {
		doCopy( other, 3 );
		return * this;
	}

	Uint24 operator * (
	) const {
		return * ( const Uint24 * )checkAndGetAddr();
	}

	Uint24 operator [] (
		const	Uint32		index
	) const {
		return * ( const Uint24 * )checkAndGetAddr( index );
	}

	Uint24 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Uint24 * )checkAndGetAddr( x, y );
	}

//	Uint32 operator * (
//	) const {
//		const Uchar * ptr = ( const Uchar * )checkAndGetAddr();
//		return ( ( ( Uint32 )*( const Uint16 * )( ptr     ) )       )
//		     | ( ( ( Uint32 )*                  ( ptr + 2 ) ) << 16 );
//	}
//
//	Uint32 operator [] (
//		const	Uint32		index
//	) const {
//		const Uchar * ptr = ( const Uchar * )checkAndGetAddr( index );
//		return ( ( ( Uint32 )*( const Uint16 * )( ptr     ) )       )
//		     | ( ( ( Uint32 )*                  ( ptr + 2 ) ) << 16 );
//	}
//
//	Uint32 operator () (
//		const	Uint32		x,
//		const	Uint32		y
//	) const {
//		const Uchar * ptr = ( const Uchar * )checkAndGetAddr( x, y );
//		return ( ( ( Uint32 )*( const Uint16 * )( ptr     ) )       )
//		     | ( ( ( Uint32 )*                  ( ptr + 2 ) ) << 16 );
//	}

};

// ============================================================================

/// \brief Specialized Reader, working with Uint32 elements (4 bytes).
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Uint32Reader : public Reader {

public :

	Uint32Reader(
	) {
	}

	Uint32Reader(
		const	Block		& block
	) : Reader( block, 4 ) {
	}

	Uint32Reader(
		const	Uint32Reader	& other
	) : Reader( other, 4 ) {
	}

	Uint32Reader(
		const	Reader		& other
	) : Reader( other, 4 ) {
	}

	Uint32Reader & operator = (
		const	Reader		& other
	) {
		doCopy( other, 4 );
		return * this;
	}

	Uint32 operator * (
	) const {
		return * ( const Uint32 * )checkAndGetAddr();
	}

	Uint32 operator [] (
		const	Uint32		index
	) const {
		return * ( const Uint32 * )checkAndGetAddr( index );
	}

	Uint32 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Uint32 * )checkAndGetAddr( x, y );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with elements whose size is 1, 2, 3, or
///	4 bytes.
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Univ32Reader : public Reader {

public :

	Univ32Reader(
	) {
	}

	Univ32Reader(
		const	Block		& block,
		const	Uint32		eltLength
	) : Reader( block, eltLength ) {
		branchGetMethods( getEltLength() );
	}

	Univ32Reader(
		const	Univ32Reader	& other,
		const	Uint32		eltLength
	) : Reader( other, eltLength ) {
		branchGetMethods( getEltLength() );
	}

	Univ32Reader(
		const	Univ32Reader	& other
	) : Reader( other ) {
		branchGetMethods( getEltLength() );
	}

	Univ32Reader(
		const	Reader		& other,
		const	Uint32		eltLength
	) : Reader( other, eltLength ) {
		branchGetMethods( getEltLength() );
	}

	Univ32Reader(
		const	Reader		& other
	) : Reader( other ) {
		branchGetMethods( getEltLength() );
	}

	Univ32Reader & operator = (
		const	Reader		& other
	) {
		doCopy( other, other.getEltLength() );
		return * this;
	}

	Uint32 operator * (
	) const {
		return ( this->*getStartMethod )();
	}

	Uint32 operator [] (
		const	Uint32		index
	) const {
		return ( this->*getIndexMethod )( index );
	}

	Uint32 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return ( this->*getXYPosMethod )( x, y );
	}

protected :

	typedef Uint32 ( Univ32Reader::*GetStartMethodType ) (
		) const;
	typedef Uint32 ( Univ32Reader::*GetIndexMethodType ) (
			const	Uint32		index
		) const;
	typedef Uint32 ( Univ32Reader::*GetXYPosMethodType ) (
			const	Uint32		x,
			const	Uint32		y
		) const;

	Uint32 getStart08() const;
	Uint32 getStart16() const;
	Uint32 getStart24() const;
	Uint32 getStart32() const;

	Uint32 getIndex08( const Uint32 index ) const;
	Uint32 getIndex16( const Uint32 index ) const;
	Uint32 getIndex24( const Uint32 index ) const;
	Uint32 getIndex32( const Uint32 index ) const;

	Uint32 getXYPos08( const Uint32 x, const Uint32 y ) const;
	Uint32 getXYPos16( const Uint32 x, const Uint32 y ) const;
	Uint32 getXYPos24( const Uint32 x, const Uint32 y ) const;
	Uint32 getXYPos32( const Uint32 x, const Uint32 y ) const;

	static struct GetTableEntry {
		GetStartMethodType	s;
		GetIndexMethodType	i;
		GetXYPosMethodType	x;
	} getTable[];

	GetStartMethodType	getStartMethod;
	GetIndexMethodType	getIndexMethod;
	GetXYPosMethodType	getXYPosMethod;

	void branchGetMethods(
		const	Uint32		eltLength
	) {
		if ( ! eltLength || eltLength > 4 ) {
			throw InvalidArgument();
		}
		getStartMethod = getTable[ eltLength - 1 ].s;
		getIndexMethod = getTable[ eltLength - 1 ].i;
		getXYPosMethod = getTable[ eltLength - 1 ].x;
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Uint64 elements (8 bytes).
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Uint64Reader : public Reader {

public :

	Uint64Reader(
	) {
	}

	Uint64Reader(
		const	Block		& block
	) : Reader( block, 8 ) {
	}

	Uint64Reader(
		const	Uint64Reader	& other
	) : Reader( other, 8 ) {
	}

	Uint64Reader(
		const	Reader		& other
	) : Reader( other, 8 ) {
	}

	Uint64Reader & operator = (
		const	Reader		& other
	) {
		doCopy( other, 8 );
		return * this;
	}

	Uint64 operator * (
	) const {
		return * ( const Uint64 * )checkAndGetAddr();
	}

	Uint64 operator [] (
		const	Uint32		index
	) const {
		return * ( const Uint64 * )checkAndGetAddr( index );
	}

	Uint64 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Uint64 * )checkAndGetAddr( x, y );
	}

};

// ============================================================================

} // namespace Mem
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Mem_Reader_H_

// ============================================================================

