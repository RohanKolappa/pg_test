// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.KeyIndex.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_KeyIndex_H_
#define _VMP_KeyIndex_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.KeyIndexPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.IO.Serializable.h"

#include "BFC.TL.Uint32Array.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief Provides a index of key frames in an existing UnitsIndex.
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL KeyIndex : virtual public BFC::IO::Serializable {

public :

	/// \brief Constructs an empty KeyIndex object.

	KeyIndex(
	);

	KeyIndex(
		const	KeyIndex &	other
	);

	/// \brief Destroys this KeyIndex object.

	virtual ~KeyIndex(
	);

	/// \brief Returns the number of entries (keyframes) in this object.

	BFC::Uint32 getSize(
	) const {
		return indexes.size();
	}

	/// \brief Returns the frame index (not the byte offset) of keyframe
	///	\a index.
	///
	/// The returned value typically gives the index that should be used
	/// in a subsequent call to UnitsIndex::getByteOffset().

	BFC::Uint32 getIndex(
		const	BFC::Uint32	index
	) const {
		return indexes[index];
	}

	/// \brief Returns the frame index (not the byte offset) of the
	///	the keyframe with the greatest index which is not greater
	///	than \a index.
	///
	/// Example: if this object contains { 0, 50, 100 }, it would return:
	/// -	getBefore( 0 ) == 0
	/// -	getBefore( 10 ) == 0
	/// -	getBefore( 60 ) == 50
	/// -	getBefore( 10000 ) == 100

	BFC::Uint32 getBefore(
		const	BFC::Uint32	index
	) const;

	/// \brief Resizes this index to contain \a size elements.

	void setSize(
		const	BFC::Uint32	size
	) {
		indexes.resize( size );
	}

	/// \brief Sets keyframe \a i to be frame \a o.

	void setIndex(
		const	BFC::Uint32	i,
		const	BFC::Uint32	o
	) {
		indexes[i] = o;
	}

	/// \brief Appends a new frame index at the end of the actual index.

	void append(
		const	BFC::Uint32	o
	) {
		indexes += o;
	}

	/// \brief [debug] Dumps the content of this KeyIndex.

	BFC::Buffer toBuffer(
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

	BFC::Uint32Array		indexes;

	// others...

	KeyIndex& operator = (
		const	KeyIndex&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_KeyIndex_H_

// ============================================================================

