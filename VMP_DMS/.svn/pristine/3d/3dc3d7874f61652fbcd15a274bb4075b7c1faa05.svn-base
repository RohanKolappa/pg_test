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
//	BFC.Mem.Reader.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_Reader_H_
#define _BFC_Mem_Reader_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.Int24.h"
#include "BFC.Base.Uint24.h"

#include "BFC.Mem.Block.h"

// ============================================================================

namespace BFC {
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
/// \ingroup BFC_Mem

class BFC_Light_DLL Reader {

public :

	/// \brief Creates a new Reader, not attached to any Block.

	Reader(
	);

	/// \brief Creates a new Reader, attached to the Branch of Block
	///	\a pBlock, and using elements T of size \a pEltLength.
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
		const	Block &		pBlock,
		const	Uint32		pEltLength
	);

	/// \brief Creates a new Reader, attached to the Branch of Block
	///	\a pBlock, and using elements T of size \a pEltLength.
	///
	/// The Block is seen as containing \a pNbrOfRows rows of \a pNbrOfCols
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
		const	Block &		pBlock,
		const	Uint32		pEltLength,
		const	Uint32		pNbrOfCols,
		const	Uint32		pNbrOfRows
	);

	/// \brief Creates a new Reader, attached to the same Branch as
	///	\a pOther.
	///
	/// The element length and the dimensions are inherited from
	/// \a pOther, and the cursor position is set to the origin.

	Reader(
		const	Reader &	pOther
	);

	/// \brief Creates a new Reader, attached to the same Branch as
	///	\a pOther.
	///
	/// Settings are first inherited from \a pOther (element length
	/// and dimensions). Then the element
	/// length is set to \a pEltLength, and the number of columns
	/// is adapted accordingly. The cursor position is set to the
	/// origin.
	///
	/// \throws InvalidArgument
	///	If the new element length is incompatible with the settings
	///	of \a pOther (ess. different padding).

	Reader(
		const	Reader &	pOther,
		const	Uint32		pEltLength
	);

	/// \brief Destroys this object, freeing the data block if this reader
	///	was the last object referencing it.

	/* virtual */ ~Reader(
	);

	/// \brief Suppresses the reference to the current Branch (if any),
	///	and replaces it by the Branch referenced by \a pOther.

	Reader & operator = (
		const	Reader &	pOther
	);

	/// \brief Sets the dimensions.
	///
	/// The cursor is repositioned on the origin (0,0).
	///
	/// \param pNbrOfCols
	///	Number of data elements (each of size getSizeOfElement() bytes)
	///	per data row.
	///
	/// \param pNbrOfRows
	///	Number of data rows.

	void setDimensions(
		const	Uint32		pNbrOfCols,
		const	Uint32		pNbrOfRows
	);

	/// \brief Returns true iff the cursor is still in the
	///	readable area.

	operator Bool (
	) const {
		return ( mCursorIdx >= 0 && ( Uint32 )mCursorIdx < mNbrOfElts );
	}

	/// \brief Returns true iff the data read by this object is equal to
	///	the data read by \a pOther.
	///
	/// Unlike the Block::operator==(), this operator takes padding into
	/// account.

	Bool operator == (
		const	Reader &	pOther
	) const;

	/// \brief Returns true iff the data read by this object is not equal to
	///	the data read by \a pOther.
	///
	/// Unlike the Block::operator!=(), this operator takes padding into
	/// account.

	Bool operator != (
		const	Reader &	pOther
	) const {
		return ( ! operator == ( pOther ) );
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
	///	index is given by \a pIndex.
	///
	/// \param pIndex
	///	Index of the element. Must be <= getNbrElements().

	void gotoElement(
		const	Uint32		pIndex
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

	BASE_CLASS_GEN_EXCEPTION( "BFC.Mem.Reader" );

	CLASS_EXCEPTION( SegFault, "Segmentation fault" );

protected :

	/// \brief Pseudo copy operator, attaching this Reader to the same
	///	Branch as the other Reader \a pOther, and using elements T
	///	of size \a pEltLength.

	void doCopy(
		const	Reader &	pOther,
		const	Uint32		pEltLength
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
#if 0
// FIXME: Comment this test out in release mode!
		if ( x >= mNbrOfCols
		  || y >= mNbrOfRows ) {
			throw SegFault();
		}
#endif
		return ( const void * )(
			mBlkMinPtr
			+ y * mRowLength
			+ x * mEltLength );
	}

private :

	// Helpers...

	void initBlock(
			Branch		* branchPtr,
		const	Uint32		pEltLength
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
/// \ingroup BFC_Mem

class BFC_Light_DLL UcharReader : public Reader {

public :

	UcharReader(
	) {
	}

	UcharReader(
		const	Block &		pBlock
	) : Reader( pBlock, 1 ) {
	}

	UcharReader(
		const	UcharReader	& pOther
	) : Reader( pOther, 1 ) {
	}

	UcharReader(
		const	Reader &	pOther
	) : Reader( pOther, 1 ) {
	}

	UcharReader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 1 );
		return * this;
	}

	Uchar operator * (
	) const {
		return * ( const Uchar * )checkAndGetAddr();
	}

	Uchar operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Uchar * )checkAndGetAddr( pIndex );
	}

	Uchar operator () (
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
		return * ( const Uchar * )(
			( const Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ x );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Char elements (1 byte).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL CharReader : public Reader {

public :

	CharReader(
	) {
	}

	CharReader(
		const	Block &		pBlock
	) : Reader( pBlock, 1 ) {
	}

	CharReader(
		const	CharReader	& pOther
	) : Reader( pOther, 1 ) {
	}

	CharReader(
		const	Reader &	pOther
	) : Reader( pOther, 1 ) {
	}

	CharReader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 1 );
		return * this;
	}

	Char operator * (
	) const {
		return * ( const Char * )checkAndGetAddr();
	}

	Char operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Char * )checkAndGetAddr( pIndex );
	}

	Char operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Char * )(
			( const Char * )getBlockAddress()
			+ y * getStrideLength()
			+ x );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Uint16 elements (2 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint16Reader : public Reader {

public :

	Uint16Reader(
	) {
	}

	Uint16Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 2 ) {
	}

	Uint16Reader(
		const	Uint16Reader	& pOther
	) : Reader( pOther, 2 ) {
	}

	Uint16Reader(
		const	Reader &	pOther
	) : Reader( pOther, 2 ) {
	}

	Uint16Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 2 );
		return * this;
	}

	Uint16 operator * (
	) const {
		return * ( const Uint16 * )checkAndGetAddr();
	}

	Uint16 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Uint16 * )checkAndGetAddr( pIndex );
	}

	Uint16 operator () (
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
		return * ( const Uint16 * )(
			( const Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 1 ) );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Int16 elements (2 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int16Reader : public Reader {

public :

	Int16Reader(
	) {
	}

	Int16Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 2 ) {
	}

	Int16Reader(
		const	Int16Reader	& pOther
	) : Reader( pOther, 2 ) {
	}

	Int16Reader(
		const	Reader &	pOther
	) : Reader( pOther, 2 ) {
	}

	Int16Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 2 );
		return * this;
	}

	Int16 operator * (
	) const {
		return * ( const Int16 * )checkAndGetAddr();
	}

	Int16 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Int16 * )checkAndGetAddr( pIndex );
	}

	Int16 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Int16 * )(
			( const Char * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 1 ) );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with (logical) Uint24 elements
///	(3 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint24Reader : public Reader {

public :

	Uint24Reader(
	) {
	}

	Uint24Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 3 ) {
	}

	Uint24Reader(
		const	Uint24Reader	& pOther
	) : Reader( pOther, 3 ) {
	}

	Uint24Reader(
		const	Reader &	pOther
	) : Reader( pOther, 3 ) {
	}

	Uint24Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 3 );
		return * this;
	}

	Uint24 operator * (
	) const {
		return * ( const Uint24 * )checkAndGetAddr();
	}

	Uint24 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Uint24 * )checkAndGetAddr( pIndex );
	}

	Uint24 operator () (
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
		return * ( const Uint24 * )(
			( const Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ x * 3 );
	}

//	Uint32 operator * (
//	) const {
//		const Uchar * ptr = ( const Uchar * )checkAndGetAddr();
//		return ( ( ( Uint32 )*( const Uint16 * )( ptr     ) )       )
//		     | ( ( ( Uint32 )*                  ( ptr + 2 ) ) << 16 );
//	}
//
//	Uint32 operator [] (
//		const	Uint32		pIndex
//	) const {
//		const Uchar * ptr = ( const Uchar * )checkAndGetAddr( pIndex );
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

/// \brief Specialized Reader, working with (logical) Int24 elements
///	(3 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int24Reader : public Reader {

public :

	Int24Reader(
	) {
	}

	Int24Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 3 ) {
	}

	Int24Reader(
		const	Int24Reader	& pOther
	) : Reader( pOther, 3 ) {
	}

	Int24Reader(
		const	Reader &	pOther
	) : Reader( pOther, 3 ) {
	}

	Int24Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 3 );
		return * this;
	}

	Int24 operator * (
	) const {
		return * ( const Int24 * )checkAndGetAddr();
	}

	Int24 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Int24 * )checkAndGetAddr( pIndex );
	}

	Int24 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Int24 * )(
			( const Char * )getBlockAddress()
			+ y * getStrideLength()
			+ x * 3 );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Uint32 elements (4 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint32Reader : public Reader {

public :

	Uint32Reader(
	) {
	}

	Uint32Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 4 ) {
	}

	Uint32Reader(
		const	Uint32Reader	& pOther
	) : Reader( pOther, 4 ) {
	}

	Uint32Reader(
		const	Reader &	pOther
	) : Reader( pOther, 4 ) {
	}

	Uint32Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 4 );
		return * this;
	}

	Uint32 operator * (
	) const {
		return * ( const Uint32 * )checkAndGetAddr();
	}

	Uint32 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Uint32 * )checkAndGetAddr( pIndex );
	}

	Uint32 operator () (
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
		return * ( const Uint32 * )(
			( const Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 2 ) );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Int32 elements (4 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int32Reader : public Reader {

public :

	Int32Reader(
	) {
	}

	Int32Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 4 ) {
	}

	Int32Reader(
		const	Int32Reader	& pOther
	) : Reader( pOther, 4 ) {
	}

	Int32Reader(
		const	Reader &	pOther
	) : Reader( pOther, 4 ) {
	}

	Int32Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 4 );
		return * this;
	}

	Int32 operator * (
	) const {
		return * ( const Int32 * )checkAndGetAddr();
	}

	Int32 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Int32 * )checkAndGetAddr( pIndex );
	}

	Int32 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Int32 * )(
			( const Char * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 2 ) );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Uint64 elements (8 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Uint64Reader : public Reader {

public :

	Uint64Reader(
	) {
	}

	Uint64Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 8 ) {
	}

	Uint64Reader(
		const	Uint64Reader	& pOther
	) : Reader( pOther, 8 ) {
	}

	Uint64Reader(
		const	Reader &	pOther
	) : Reader( pOther, 8 ) {
	}

	Uint64Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 8 );
		return * this;
	}

	Uint64 operator * (
	) const {
		return * ( const Uint64 * )checkAndGetAddr();
	}

	Uint64 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Uint64 * )checkAndGetAddr( pIndex );
	}

	Uint64 operator () (
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
		return * ( const Uint64 * )(
			( const Uchar * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 3 ) );
	}

};

// ============================================================================

/// \brief Specialized Reader, working with Int64 elements (8 bytes).
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Int64Reader : public Reader {

public :

	Int64Reader(
	) {
	}

	Int64Reader(
		const	Block &		pBlock
	) : Reader( pBlock, 8 ) {
	}

	Int64Reader(
		const	Int64Reader	& pOther
	) : Reader( pOther, 8 ) {
	}

	Int64Reader(
		const	Reader &	pOther
	) : Reader( pOther, 8 ) {
	}

	Int64Reader & operator = (
		const	Reader &	pOther
	) {
		doCopy( pOther, 8 );
		return * this;
	}

	Int64 operator * (
	) const {
		return * ( const Int64 * )checkAndGetAddr();
	}

	Int64 operator [] (
		const	Uint32		pIndex
	) const {
		return * ( const Int64 * )checkAndGetAddr( pIndex );
	}

	Int64 operator () (
		const	Uint32		x,
		const	Uint32		y
	) const {
		return * ( const Int64 * )(
			( const Char * )getBlockAddress()
			+ y * getStrideLength()
			+ ( x << 3 ) );
	}

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_Reader_H_

// ============================================================================

