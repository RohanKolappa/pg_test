// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
//
// This file is part of "BFC::Light".
//
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
// 
// Filename:
//	BFC.Mem.Writer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_Writer_H_
#define _BFC_Mem_Writer_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Mem.Block.h"
#include "BFC.Mem.Reader.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Base class of all memory writers.
///
/// A Writer object is basically a temporary object attached to a data block,
/// and which grants write access to this block.
///
/// Various subclasses exist to help access the data block as if it was
/// containing elements of a particular length (in bytes). Examples:
/// UcharWriter, Uint24Writer, ...
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Writer {

public :

	/// \brief Creates a new Writer, not attached to any Block.

	Writer(
	);

	/// \brief Creates a new Writer, attached to the Branch of Block
	///	\a block, and using elements T of size \a eltLength.
	///
	/// The Block is seen as containing a single row of as many elements
	/// of type T as possible.
	///
	/// Example: the following code creates a 32-bytes long Block, and
	/// attaches a Writer to it, which will see it as a row of 10
	/// elements of size 3, terminated by a 2-bytes padding:
	/// \code
	///	Block	block( 32 );
	///	Writer	writer( block, 3 );
	/// \endcode

	Writer(
		const	Block		& block,
		const	Uint32		eltLength
	);

	/// \brief Creates a new Writer, attached to the Branch of Block
	///	\a block, and using elements T of size \a eltLength.
	///
	/// The Block is seen as containing \a nbrOfRows rows of \a nbrOfCols
	/// elements of type T.
	///
	/// Example: the following code creates a 32-bytes long Block, and
	/// attaches a Writer to it, which will see it as 4 rows of 2 elements
	/// of size 3, each row being terminated by a 2-bytes padding:
	/// \code
	///	Block	block( 32 );
	///	Writer	writer( block, 3, 2, 4 );
	/// \endcode
	///
	/// \throws InvalidArgument
	///	If the parameters are incompatible with the Block length.

	Writer(
		const	Block		& block,
		const	Uint32		eltLength,
		const	Uint32		nbrOfCols,
		const	Uint32		nbrOfRows
	);

	/// \brief Creates a new Writer, attached to the same Branch as
	///	\a other.
	///
	/// The element length and the dimensions are inherited from
	/// \a other, and the cursor position is set to the origin.

	Writer(
		const	Writer		& other
	);

	/// \brief Creates a new Writer, attached to the same Branch as
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

	Writer(
		const	Writer		& other,
		const	Uint32		eltLength
	);

	/// \brief Destroys this object, freeing the data block if this writer
	///	was the last object referencing it.

	/* virtual */ ~Writer(
	);

	/// \brief Suppresses the reference to the current Branch (if any),
	///	and replaces it by the Branch referenced by \a other.

	Writer & operator = (
		const	Writer		& other
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
	///	index is given by the pair (\a x, \a y).
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

	/// \brief Fills this block with 0's.

	void clearAll(
	);

	/// \brief Fills this block with value \a val.
	///
	/// \param val
	///	The value to copy. Only the first T least significant bits
	///	are used!

	void fillAll(
		const	Uint32		val
	);

	/// \brief Fills a rectangular area with value \a v.
	///
	/// \param x
	///	Top left point of rectangle X coord.
	///
	/// \param y
	///	Top left point of rectangle Y coord.
	///
	/// \param w
	///	Rectangle width.
	///
	/// \param h
	///	Rectangle height.
	///
	/// \param v
	///	Value to write.

	void fillRectangle(
		const	Uint32		x,
		const	Uint32		y,
		const	Uint32		w,
		const	Uint32		h,
		const	Uchar		v
	);

	void copyDataFrom(
		const	Reader		& reader
	);

	void copyDataFrom(
		const	Reader &	pReader,
		const	Uint32		pDstOff,
		const	Uint32		pDstBlk,
		const	Uint32		pSrcOff,
		const	Uint32		pSrcBlk
	);

	/// \brief Copies a rectangular area from source \a reader.
	///
	/// \param dx
	///	Top left point of rectangle X coord in destination block.
	///
	/// \param dy
	///	Top left point of rectangle Y coord in destination block.
	///
	/// \param rw
	///	Rectangle width.
	///
	/// \param rh
	///	Rectangle height.
	///
	/// \param r
	///	Source rectangle reader.
	///
	/// \param sx
	///	Top left point of rectangle X coord in source block.
	///
	/// \param sy
	///	Top left point of rectangle Y coord in source block.
	///
	/// \param c
	///	Allow bits per pixel conversion.

	void copyRectangle(
		const	Uint32		dx,
		const	Uint32		dy,
		const	Uint32		rw,
		const	Uint32		rh,
		const	Reader		& r,
		const	Uint32		sx,
		const	Uint32		sy,
		const	Bool		c
	);

	/// \brief Copies a segment from source \a reader.
	///
	/// \param dx
	///	Left point of segment X coord in destination block.
	///
	/// \param dy
	///	Left point of segment Y coord in destination block.
	///
	/// \param sw
	///	Segment width.
	///
	/// \param r
	///	Source segment reader.
	///
	/// \param sx
	///	Left point of segment X coord in source block.
	///
	/// \param sy
	///	Left point of segment Y coord in source block.
	///
	/// \param c
	///	Allow bits per pixel conversion.

	void copySegment(
		const	Uint32		dx,
		const	Uint32		dy,
		const	Uint32		sw,
		const	Reader		& r,
		const	Uint32		sx,
		const	Uint32		sy,
		const	Bool		c
	);

	/// \brief Returns the address of the first byte of the Block written
	///	by this Writer.
	///
	/// The address returned by this method is guaranteed not to change
	/// during the lifetime of this object.

	void * getBlockAddress(
	) {
		return ( void * )mBlkMinPtr;
	}

	/// \brief Returns the address of the first byte of the first data
	///	element of the row \a index.
	///
	/// The address returned by this method is guaranteed not to change
	/// during the lifetime of this object.

	void * getRowAddress(
		const	Uint32		y
	) {
		return ( void * )( mBlkMinPtr + y * mRowLength );
	}

	/// \brief Returns the address of the first byte of the data
	///	element \a x located in row \a y. 
	///
	/// The address returned by this method is guaranteed not to change
	/// during the lifetime of this object.

	void * getElementAddress(
		const	Uint32		x,
		const	Uint32		y
	) {
		return ( void * )(
			mBlkMinPtr
			+ y * mRowLength
			+ x * mEltLength );
	}

	/// \brief Returns the current address in the Block written by this
	///	Writer.
	///
	/// The address returned by this method is guaranteed to remain valid
	/// during the lifetime of this object.

	void * getCurrentAddress(
	) {
		return ( void * )mCursorPtr;
	}

	BASE_CLASS_GEN_EXCEPTION( "BFC.Mem.Writer" );

	CLASS_EXCEPTION( SegFault, "Segmentation fault" );

protected :

	/// \brief Pseudo copy operator, attaching this Writer to the same
	///	Branch as the other Writer \a other, and using elements T
	///	of size \a eltLength.

	void doCopy(
		const	Writer		& other,
		const	Uint32		eltLength
	);

	// Helpers...

	void * checkAndGetAddr(
	) const {
#if 0
// FIXME: Comment this test out in release mode!
		if ( mCursorIdx < 0 || ( Uint32 )mCursorIdx >= mNbrOfElts ) {
			throw SegFault();
		}
#endif
		return ( void * )mCursorPtr;
	}

	void * checkAndGetAddr(
		const	Uint32		offset
	) const {
		Int32 k = mCursorIdx + offset;
#if 0
// FIXME: Comment this test out in release mode!
		if ( k < 0 || ( Uint32 )k >= mNbrOfElts ) {
			throw SegFault();
		}
#endif
		return ( void * )(
			mBlkMinPtr
			+ ( k / mNbrOfCols ) * mRowLength
			+ ( k % mNbrOfCols ) * mEltLength );
	}

	void * checkAndGetAddr(
		const	Uint32		x,
		const	Uint32		y
	) const {
#if 0
// FIXME: Comment this test out in release mode!
		if ( x >= mNbrOfCols
		  || y >= mNbrOfRows ) {
			throw SegFault();
		}
#endif
		return ( void * )(
			mBlkMinPtr
			+ y * mRowLength
			+ x * mEltLength );
	}

private :

	// Helpers...

	void initBlock(
			Branch *	branchPtr,
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
	Uchar *		mBlkMinPtr;	///< Address of start of block.

	// Current element (cursor)...

	const Uchar *	mRowMinPtr;	///< Address of first element in current data row.
	const Uchar *	mRowMaxPtr;	///< Address of last+1 element in current data row.
	Uchar *		mCursorPtr;	///< Address of current element (cursor).
	Int32		mCursorIdx;	///< Index of current element (cursor).

};

// ============================================================================

/// \brief Specialized Writer, working with Uchar elements (1 byte).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL UcharWriter : public Writer {

public :

	UcharWriter(
	) {
	}

	UcharWriter(
		const	Block		& block
	) : Writer( block, 1 ) {
	}

	UcharWriter(
		const	UcharWriter	& other
	) : Writer( other, 1 ) {
	}

	UcharWriter(
		const	Writer		& other
	) : Writer( other, 1 ) {
	}

	UcharWriter & operator = (
		const	Writer		& other
	) {
		doCopy( other, 1 );
		return * this;
	}

	Uchar & operator * (
	) {
		return * ( Uchar * )checkAndGetAddr();
	}

	Uchar & operator [] (
		const	Uint32		index
	) {
		return * ( Uchar * )checkAndGetAddr( index );
	}

	Uchar & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
#if 0
// FIXME: Comment this test out in release mode!
		if ( x >= mNbrOfCols
		  || y >= mNbrOfRows ) {
			throw SegFault();
		}
#endif
		return * ( Uchar * )(
			( Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ x );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with Char elements (1 byte).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL CharWriter : public Writer {

public :

	CharWriter(
	) {
	}

	CharWriter(
		const	Block		& block
	) : Writer( block, 1 ) {
	}

	CharWriter(
		const	CharWriter	& other
	) : Writer( other, 1 ) {
	}

	CharWriter(
		const	Writer		& other
	) : Writer( other, 1 ) {
	}

	CharWriter & operator = (
		const	Writer		& other
	) {
		doCopy( other, 1 );
		return * this;
	}

	Char & operator * (
	) {
		return * ( Char * )checkAndGetAddr();
	}

	Char & operator [] (
		const	Uint32		index
	) {
		return * ( Char * )checkAndGetAddr( index );
	}

	Char & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
		return * ( Char * )(
			( Char * )getBlockAddress()
			+ y * getStrideLength()
			+ x );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with Uint16 elements (2 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint16Writer : public Writer {

public :

	Uint16Writer(
	) {
	}

	Uint16Writer(
		const	Block		& block
	) : Writer( block, 2 ) {
	}

	Uint16Writer(
		const	Uint16Writer	& other
	) : Writer( other, 2 ) {
	}

	Uint16Writer(
		const	Writer		& other
	) : Writer( other, 2 ) {
	}

	Uint16Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 2 );
		return * this;
	}

	Uint16 & operator * (
	) {
		return * ( Uint16 * )checkAndGetAddr();
	}

	Uint16 & operator [] (
		const	Uint32		index
	) {
		return * ( Uint16 * )checkAndGetAddr( index );
	}

	Uint16 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
#if 0
// FIXME: Comment this test out in release mode!
		if ( x >= mNbrOfCols
		  || y >= mNbrOfRows ) {
			throw SegFault();
		}
#endif
		return * ( Uint16 * )(
			( Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 1 ) );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with Int16 elements (2 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int16Writer : public Writer {

public :

	Int16Writer(
	) {
	}

	Int16Writer(
		const	Block		& block
	) : Writer( block, 2 ) {
	}

	Int16Writer(
		const	Int16Writer	& other
	) : Writer( other, 2 ) {
	}

	Int16Writer(
		const	Writer		& other
	) : Writer( other, 2 ) {
	}

	Int16Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 2 );
		return * this;
	}

	Int16 & operator * (
	) {
		return * ( Int16 * )checkAndGetAddr();
	}

	Int16 & operator [] (
		const	Uint32		index
	) {
		return * ( Int16 * )checkAndGetAddr( index );
	}

	Int16 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
		return * ( Int16 * )(
			( Char * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 1 ) );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with (logical) Uint24 elements
///	(3 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint24Writer : public Writer {

public :

	Uint24Writer(
	) {
	}

	Uint24Writer(
		const	Block		& block
	) : Writer( block, 3 ) {
	}

	Uint24Writer(
		const	Uint24Writer	& other
	) : Writer( other, 3 ) {
	}

	Uint24Writer(
		const	Writer		& other
	) : Writer( other, 3 ) {
	}

	Uint24Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 3 );
		return * this;
	}

//	void set(
//		const	Uint32		value
//	) {
//		Uchar * c = ( Uchar * )checkAndGetAddr();
//		* ( Uint16 * )( c     ) = ( value       );
//		* ( Uchar  * )( c + 2 ) = ( value >> 16 );
//	}

	Uint24 & operator * (
	) {
		return * ( Uint24 * )checkAndGetAddr();
	}

	Uint24 & operator [] (
		const	Uint32		index
	) {
		return * ( Uint24 * )checkAndGetAddr( index );
	}

	Uint24 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
#if 0
// FIXME: Comment this test out in release mode!
		if ( x >= mNbrOfCols
		  || y >= mNbrOfRows ) {
			throw SegFault();
		}
#endif
		return * ( Uint24 * )(
			( Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ x * 3 );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with (logical) Int24 elements
///	(3 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int24Writer : public Writer {

public :

	Int24Writer(
	) {
	}

	Int24Writer(
		const	Block		& block
	) : Writer( block, 3 ) {
	}

	Int24Writer(
		const	Int24Writer	& other
	) : Writer( other, 3 ) {
	}

	Int24Writer(
		const	Writer		& other
	) : Writer( other, 3 ) {
	}

	Int24Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 3 );
		return * this;
	}

//	void set(
//		const	Uint32		value
//	) {
//		Uchar * c = ( Uchar * )checkAndGetAddr();
//		* ( Uint16 * )( c     ) = ( value       );
//		* ( Uchar  * )( c + 2 ) = ( value >> 16 );
//	}

	Int24 & operator * (
	) {
		return * ( Int24 * )checkAndGetAddr();
	}

	Int24 & operator [] (
		const	Uint32		index
	) {
		return * ( Int24 * )checkAndGetAddr( index );
	}

	Int24 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
		return * ( Int24 * )(
			( Char * )getBlockAddress()
			+ y * getStrideLength()
			+ x * 3 );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with Uint32 elements (4 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint32Writer : public Writer {

public :

	Uint32Writer(
	) {
	}

	Uint32Writer(
		const	Block		& block
	) : Writer( block, 4 ) {
	}

	Uint32Writer(
		const	Uint32Writer	& other
	) : Writer( other, 4 ) {
	}

	Uint32Writer(
		const	Writer		& other
	) : Writer( other, 4 ) {
	}

	Uint32Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 4 );
		return * this;
	}

	Uint32 & operator [] (
		const	Uint32		index
	) {
		return * ( Uint32 * )checkAndGetAddr( index );
	}

	Uint32 & operator * (
	) {
		return * ( Uint32 * )checkAndGetAddr();
	}

	Uint32 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
#if 0
// FIXME: Comment this test out in release mode!
		if ( x >= mNbrOfCols
		  || y >= mNbrOfRows ) {
			throw SegFault();
		}
#endif
		return * ( Uint32 * )(
			( Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 2 ) );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with Int32 elements (4 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int32Writer : public Writer {

public :

	Int32Writer(
	) {
	}

	Int32Writer(
		const	Block		& block
	) : Writer( block, 4 ) {
	}

	Int32Writer(
		const	Int32Writer	& other
	) : Writer( other, 4 ) {
	}

	Int32Writer(
		const	Writer		& other
	) : Writer( other, 4 ) {
	}

	Int32Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 4 );
		return * this;
	}

	Int32 & operator [] (
		const	Uint32		index
	) {
		return * ( Int32 * )checkAndGetAddr( index );
	}

	Int32 & operator * (
	) {
		return * ( Int32 * )checkAndGetAddr();
	}

	Int32 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
		return * ( Int32 * )(
			( Char * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 2 ) );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with Uint64 elements (8 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint64Writer : public Writer {

public :

	Uint64Writer(
	) {
	}

	Uint64Writer(
		const	Block		& block
	) : Writer( block, 8 ) {
	}

	Uint64Writer(
		const	Uint64Writer	& other
	) : Writer( other, 8 ) {
	}

	Uint64Writer(
		const	Writer		& other
	) : Writer( other, 8 ) {
	}

	Uint64Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 8 );
		return * this;
	}

	Uint64 & operator * (
	) {
		return * ( Uint64 * )checkAndGetAddr();
	}

	Uint64 & operator [] (
		const	Uint32		index
	) {
		return * ( Uint64 * )checkAndGetAddr( index );
	}

	Uint64 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
#if 0
// FIXME: Comment this test out in release mode!
		if ( x >= mNbrOfCols
		  || y >= mNbrOfRows ) {
			throw SegFault();
		}
#endif
		return * ( Uint64 * )(
			( Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 3 ) );
	}

};

// ============================================================================

/// \brief Specialized Writer, working with Int64 elements (8 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int64Writer : public Writer {

public :

	Int64Writer(
	) {
	}

	Int64Writer(
		const	Block		& block
	) : Writer( block, 8 ) {
	}

	Int64Writer(
		const	Int64Writer	& other
	) : Writer( other, 8 ) {
	}

	Int64Writer(
		const	Writer		& other
	) : Writer( other, 8 ) {
	}

	Int64Writer & operator = (
		const	Writer		& other
	) {
		doCopy( other, 8 );
		return * this;
	}

	Int64 & operator * (
	) {
		return * ( Int64 * )checkAndGetAddr();
	}

	Int64 & operator [] (
		const	Uint32		index
	) {
		return * ( Int64 * )checkAndGetAddr( index );
	}

	Int64 & operator () (
		const	Uint32		x,
		const	Uint32		y
	) {
		return * ( Int64 * )(
			( Char * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 3 ) );
	}

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_Writer_H_

// ============================================================================

