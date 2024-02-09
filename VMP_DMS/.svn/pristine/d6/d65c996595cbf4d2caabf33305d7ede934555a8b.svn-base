// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UnitsIndex.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UnitsIndex_H_
#define _VMP_UnitsIndex_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.UnitsIndexPtr.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL( VMP_Libs_Base_DLL, CFLIndex )
BFC_PTR_DECL( VMP_Libs_Base_DLL, VFLIndex )

} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.IO.Serializable.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "BFC.TL.Uint32Array.h"
#include "BFC.TL.Uint64Array.h"

#include "VMP.KeyIndex.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief Abstract class associating byte offsets with start of units in
///	bytes streams.
///
/// This class is mainly interesting when used in conjunction with ByteSession
/// objects. In this case, it becomes possible to seek to the beginning of a
/// particular frame.
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL UnitsIndex :

	virtual public BFC::DL::Object,
	virtual public BFC::IO::Serializable {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates an empty Index object.

	UnitsIndex(
	);

	UnitsIndex(
		const	UnitsIndex &	other
	);

	virtual UnitsIndexPtr clone(
	) const = 0;

	/// \brief Returns the number of entries contained in this UnitsIndex.

	virtual BFC::Uint32 getSize(
	) const = 0;

	/// \brief Returns the unit position (in bytes) of frame
	///	\a unitIndex.

	virtual BFC::Uint64 getByteOffset(
		const	BFC::Uint32	unitIndex
	) = 0;

	/// \brief Returns the index of the unit containing byte at position
	///	\a byteIndex.

	virtual BFC::Uint32 getUnitOffset(
		const	BFC::Uint64	byteIndex
	) = 0;

	/// \brief Associates KeyIndex \a idx to this UnitsIndex.

	void setKeyIndex(
			KeyIndexPtr	idx
	) {
		keyIndex = idx;
	}

	/// \brief Deletes the KeyIndex associated to this UnitsIndex.

	void resetKeyIndex(
	) {
		keyIndex.kill();
	}

	/// \brief Returns whether or not this UnitsIndex has an associated KeyIndex.

	BFC::Bool hasKeyIndex(
	) const {
		return keyIndex;
	}

	/// \brief Returns the associated KeyIndex, or NULL if there is no
	///	associated KeyIndex.

	KeyIndexPtr getKeyIndex(
	) const {
		return keyIndex;
	}

	/// \brief [debug] Dumps up the first 5 elements of this UnitsIndex.

	virtual BFC::Buffer toBuffer(
	) const;

	/// \brief Serializes this UnitsIndex.

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	/// \brief Rebuilds this UnitsIndex from a serialized version.

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	// internal state...

	KeyIndexPtr	keyIndex;	///< Associated KeyIndex.

	// others...

	UnitsIndex& operator = (
		const	UnitsIndex&
	);

};

// ============================================================================

/// \brief Implements a UnitsIndex, where all frames have a constant length.
///
/// CFL stands of course for "Constant Frame Length".

class VMP_Libs_Base_DLL CFLIndex : public UnitsIndex {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a CFLIndex, for a sequence of \a size frames, each
	///	being \a length bytes long.

	CFLIndex(
		const	BFC::Uint32	size = 0,
		const	BFC::Uint32	length = 0
	);

	virtual UnitsIndexPtr clone(
	) const;

	virtual BFC::Uint32 getSize(
	) const;

	virtual BFC::Uint64 getByteOffset(
		const	BFC::Uint32	unitIndex
	);

	virtual BFC::Uint32 getUnitOffset(
		const	BFC::Uint64	byteIndex
	);

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	// internal state...

	BFC::Uint32	size;		///< number of frames
	BFC::Uint32	length;		///< frame length

	// others...

	CFLIndex(
		const	CFLIndex&
	);

	CFLIndex& operator = (
		const	CFLIndex&
	);

};

// ============================================================================

/// \brief Implements a UnitsIndex, where all frames have a different length.
///
/// VFL stands of course for "Variable Frame Length".

class VMP_Libs_Base_DLL VFLIndex : public UnitsIndex {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	VFLIndex(
	);

	VFLIndex(
		const	VFLIndex &	other
	);

	virtual UnitsIndexPtr clone(
	) const;

	/// \brief Sets this VFLIndex to contain \a size frames!

	void setSize(
		const	BFC::Uint32	size
	);

	virtual BFC::Uint32 getSize(
	) const;

	void setLength(
		const	BFC::Uint32	index,
		const	BFC::Uint32	length
	) {
		lengths[ index ] = length;
		isReady = false;
	}

	virtual BFC::Uint64 getByteOffset(
		const	BFC::Uint32	unitIndex
	);

	virtual BFC::Uint32 getUnitOffset(
		const	BFC::Uint64	byteIndex
	);

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

	BASE_CLASS_GEN_EXCEPTION( "VMP.UnitsIndex" );

	CLASS_EXCEPTION( Overflow, "Overflow" );

protected :

	void ensureHasOffsets(
	) {
		if ( ! isReady ) {
			computeOffsets();
		}
	}

	void computeOffsets(
	);

private :

	// internal state...

	BFC::Uint32Array	lengths;	///< Frame lengths.
	BFC::Uint64Array	offsets;	///< Byte indexes.
	BFC::Bool		isReady;	///< Byte indexes have been computed ?

	// others...

	VFLIndex& operator = (
		const	VFLIndex&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_UnitsIndex_H_

// ============================================================================

