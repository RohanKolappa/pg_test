// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	TBFC.Mem.Allocator.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Mem_Allocator_H_
#define _TBFC_Mem_Allocator_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Mem.AllocatorPtr.h"

// ============================================================================

#include "TBFC.Base.FastCS.h"
#include "TBFC.Base.SObject.h"

// ============================================================================

namespace TBFC {
namespace Mem {

// ============================================================================

/// \brief Interface implemented by all memory allocators.
///
/// This interface should be implemented by all memory allocators
/// used to allocate and/or manage memory using the TBFC_Mem framework.
///
/// \ingroup TBFC_Mem

class TBFC_Light_DLL Allocator : virtual public SObject {

protected :

	/// \brief Creates a new Allocator.

	Allocator(
		const	Bool		__retrieve
	);

public :

	/// \brief Destroys this object.

	virtual ~Allocator(
	);

	static AllocatorPtr chooseBest(
		const	Uint32		__len
	);

	static AllocatorPtr retrieveAllocator(
		const	Uchar		* __ptr
	);

	static Uint32 retrieveLength(
		const	Uchar		* __ptr
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
	/// The allocated block will be AT LEAST \a __len bytes long. The exact
	/// length that was allocated is stored in \a __phy, and the base
	/// address of the block is stored in \a __ptr.
	///
	/// If \a __len is 0, then no memory is allocated.
	///
	/// \param __len
	///	[in] Requested minimum length.
	///
	/// \param __ptr
	///	[out] Base address of the newly allocated memory.
	///
	/// \param __phy
	///	[out] Exact length that was allocated.
	///
	/// \throws OutOfMemory
	///	If the request could not be fulfilled. The \a __ptr and
	///	\a __phy are first set to 0 before throwing this exception.

	virtual void createBlock(
		const	Uint32		__len,
			Uchar		* & __ptr,
			Uint32		& __phy
	);

	/// \brief Resizes a previously allocated block.
	///
	/// This method tries to resize a block that was previously allocated
	/// by this same object, and whose base address was \a __ptr, and whose
	/// real physical length was \a __phy.
	///
	/// If the requested new length \a __len is <= \a __phy, then this
	/// method does nothing!
	///
	/// The new block will be AT LEAST \a __len bytes long, and the old
	/// content is preserved.
	///
	/// \param __len
	///	[in] Requested minimum length.
	///
	/// \param __ptr
	///	[in/out] Base address of the old block, which is then
	///	overwritten with the address of the new block. Both address may
	///	be identical, but this is not guaranteed!
	///
	/// \param __phy
	///	[in/out] Physical length of the old block, which is then
	///	overwritten with the physical length of the new block.
	///
	/// \throws OutOfMemory
	///	If the request could not be fulfilled. The \a __ptr and
	///	\a __phy parameters are left untouched, and the old memory
	///	block is not freed or moved!

	virtual void resizeBlock(
		const	Uint32		__len,
			Uchar		* & __ptr,
			Uint32		& __phy
	);

	/// \brief Destroys a previously allocated block.
	///
	/// This method releases the memory block pointed to by \a __ptr, and
	/// whose physical length is \a __phy.
	///
	/// \param __ptr
	///	The start address of the block to delete.
	///
	/// \param __phy
	///	The physical length of the block to delete.

	virtual void deleteBlock(
			Uchar		* __ptr,
			Uint32		__phy
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
		const	Uint32		__len,
			Uchar		* & __ptr,
			Uint32		& __phy
	) = 0;

	virtual void implResizeBlock(
		const	Uint32		__len,
			Uchar		* & __ptr,
			Uint32		& __phy
	) = 0;

	virtual void implDeleteBlock(
			Uchar		* __ptr,
		const	Uint32		__phy
	) = 0;

private :

	static Uint32 getMagicString(
	) {
		return *( const Uint32 * )"TBFC!";
	}

	mutable FastCS		allocCrt;
	Bool			retrieve;
	Uint64			totAlloc;
	Uint64			curAlloc;

	void addAllocated(
		const	Uint32		__length
	);

	void delAllocated(
		const	Uint32		__length
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
/// \ingroup TBFC_Mem

class TBFC_Light_DLL StdCAllocator : public Allocator {

protected :

	StdCAllocator(
		const	Bool		__retrieve
	);

public :

	virtual ~StdCAllocator(
	);

	static AllocatorPtr instance(
		const	Bool		__retrieve
	);

protected :

	virtual void implCreateBlock(
		const	Uint32		__len,
			Uchar		*&__ptr,
			Uint32		&__phy
	);

	virtual void implResizeBlock(
		const	Uint32		__len,
			Uchar		*&__ptr,
			Uint32		&__phy
	);

	virtual void implDeleteBlock(
			Uchar		*__ptr,
		const	Uint32		__phy
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
/// \ingroup TBFC_Mem

class TBFC_Light_DLL StdCppAllocator : public Allocator {

protected :

	StdCppAllocator(
		const	Bool		__retrieve
	);

public :

	virtual ~StdCppAllocator(
	);

	static AllocatorPtr instance(
		const	Bool		__retrieve
	);

protected :

	virtual void implCreateBlock(
		const	Uint32		__len,
			Uchar		*&__ptr,
			Uint32		&__phy
	);

	virtual void implResizeBlock(
		const	Uint32		__len,
			Uchar		*&__ptr,
			Uint32		&__phy
	);

	virtual void implDeleteBlock(
			Uchar		*__ptr,
		const	Uint32		__phy
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
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Mem_Allocator_H_

// ============================================================================

