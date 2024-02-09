// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.Mem.Allocator.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Mem_Allocator_H_
#define _BFC_Mem_Allocator_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Mem.AllocatorPtr.h"

// ============================================================================

#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Mem {

// ============================================================================

/// \brief Interface implemented by all memory allocators.
///
/// This interface should be implemented by all memory allocators
/// used to allocate and/or manage memory using the BFC_Mem framework.
///
/// \ingroup BFC_Mem

class BFC_Light_DLL Allocator /* : virtual public SObject */ {

protected :

	/// \brief Creates a new Allocator.

	Allocator(
		const	Bool		pRetrieve
	);

public :

	/// \brief Destroys this object.

	virtual ~Allocator(
	);

	static Allocator * chooseBest(
		const	Uint32		pLen
	);

	static Allocator * retrieveAllocator(
		const	Uchar *		pPtr
	);

	static Uint32 retrieveLength(
		const	Uchar *		pPtr
	);

	Bool isRetrievable(
	) const {
		return retrieve;
	}

	/// \brief Allocates a new block of memory.
	///
	/// This method allocates a new block of contiguous memory, using
	/// some specific method implemented by the subclass.
	///
	/// The allocated block will be AT LEAST \a pLen bytes long. The exact
	/// length that was allocated is stored in \a pPhy, and the base
	/// address of the block is stored in \a pPtr.
	///
	/// If \a pLen is 0, then no memory is allocated.
	///
	/// \param pLen
	///	[in] Requested minimum length.
	///
	/// \param pPtr
	///	[out] Base address of the newly allocated memory.
	///
	/// \param pPhy
	///	[out] Exact length that was allocated.
	///
	/// \throws OutOfMemory
	///	If the request could not be fulfilled. The \a pPtr and
	///	\a pPhy are first set to 0 before throwing this exception.

	virtual void createBlock(
		const	Uint32		pLen,
			Uchar * &	pPtr,
			Uint32 &	pPhy
	);

	/// \brief Resizes a previously allocated block.
	///
	/// This method tries to resize a block that was previously allocated
	/// by this same object, and whose base address was \a pPtr, and whose
	/// real physical length was \a pPhy.
	///
	/// If the requested new length \a pLen is <= \a pPhy, then this
	/// method does nothing!
	///
	/// The new block will be AT LEAST \a pLen bytes long, and the old
	/// content is preserved.
	///
	/// \param pLen
	///	[in] Requested minimum length.
	///
	/// \param pPtr
	///	[in/out] Base address of the old block, which is then
	///	overwritten with the address of the new block. Both address may
	///	be identical, but this is not guaranteed!
	///
	/// \param pPhy
	///	[in/out] Physical length of the old block, which is then
	///	overwritten with the physical length of the new block.
	///
	/// \throws OutOfMemory
	///	If the request could not be fulfilled. The \a pPtr and
	///	\a pPhy parameters are left untouched, and the old memory
	///	block is not freed or moved!

	virtual void resizeBlock(
		const	Uint32		pLen,
			Uchar * &	pPtr,
			Uint32 &	pPhy
	);

	/// \brief Destroys a previously allocated block.
	///
	/// This method releases the memory block pointed to by \a pPtr, and
	/// whose physical length is \a pPhy.
	///
	/// \param pPtr
	///	The start address of the block to delete.
	///
	/// \param pPhy
	///	The physical length of the block to delete.

	virtual void deleteBlock(
			Uchar *		pPtr,
			Uint32		pPhy
	);

	/// \brief Returns the total amount of memory this Allocator had
	///	to allocate.

	Uint64 getTotalAllocated(
	) const;

	/// \brief Returns the current amount of memory this Allocator had
	///	to allocate.

	Uint64 getCurrentAllocated(
	) const;

protected :

	virtual void implCreateBlock(
		const	Uint32		pLen,
			Uchar * &	pPtr,
			Uint32 &	pPhy
	) = 0;

	virtual void implResizeBlock(
		const	Uint32		pLen,
			Uchar * &	pPtr,
			Uint32 &	pPhy
	) = 0;

	virtual void implDeleteBlock(
			Uchar *		pPtr,
		const	Uint32		pPhy
	) = 0;

private :

	static Uint32 getMagicString(
	) {
		return *( const Uint32 * )"BFC!";
	}

	mutable FastCS		allocCrt;
	Bool			retrieve;
	Uint64			totAlloc;
	Uint64			curAlloc;

	void addAllocated(
		const	Uint32		pLength
	);

	void delAllocated(
		const	Uint32		pLength
	);

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Allocator(
	);

	Allocator(
		const	Allocator	&
	);

	Allocator& operator = (
		const	Allocator	&
	);

};

// ============================================================================

/// \brief Allocator using "::malloc()", "::resize()", and "::free()".
///
/// \ingroup BFC_Mem

class BFC_Light_DLL StdCAllocator : public Allocator {

protected :

	StdCAllocator(
		const	Bool		pRetrieve
	);

public :

	static Allocator * instance(
		const	Bool		pRetrieve
	);

protected :

	virtual void implCreateBlock(
		const	Uint32		pLen,
			Uchar * &	pPtr,
			Uint32 &	pPhy
	);

	virtual void implResizeBlock(
		const	Uint32		pLen,
			Uchar *	&	pPtr,
			Uint32 &	pPhy
	);

	virtual void implDeleteBlock(
			Uchar *		pPtr,
		const	Uint32		pPhy
	);

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	StdCAllocator(
		const	StdCAllocator&
	);

	StdCAllocator& operator = (
		const	StdCAllocator&
	);

};

// ============================================================================

/// \brief Allocator using "new Uchar[]", and "delete[]".
///
/// \ingroup BFC_Mem

class BFC_Light_DLL StdCppAllocator : public Allocator {

protected :

	StdCppAllocator(
		const	Bool		pRetrieve
	);

public :

	static Allocator * instance(
		const	Bool		pRetrieve
	);

protected :

	virtual void implCreateBlock(
		const	Uint32		pLen,
			Uchar * &	pPtr,
			Uint32 &	pPhy
	);

	virtual void implResizeBlock(
		const	Uint32		pLen,
			Uchar * &	pPtr,
			Uint32 &	pPhy
	);

	virtual void implDeleteBlock(
			Uchar *		pPtr,
		const	Uint32		pPhy
	);

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	StdCppAllocator(
		const	StdCppAllocator&
	);

	StdCppAllocator& operator = (
		const	StdCppAllocator&
	);

};

// ============================================================================

} // namespace Mem
} // namespace BFC

// ============================================================================

#endif // _BFC_Mem_Allocator_H_

// ============================================================================

