// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_MuxedStream_H_
#define _VMP_MuxedStream_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.MuxedStreamPtr.h"

// ============================================================================

#include "VMP.Stream.h"

#include "VMP.MuxedFormat.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL MuxedStream : public Stream {

public :

	/// \brief Unique DL::Type object identifying this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new MuxedStream.
	///
	/// \param pFormat
	///	MuxedFormat of this MuxedStream.

	MuxedStream(
		const	MuxedFormat	pFormat = MuxedFormat::Unknown
	) :
		BFC::DL::Object	( getClassType() ),
		format		( pFormat ) {
	}

	/// \brief Creates a copy of \a pOther.
	///
	/// \param pOther
	///	MuxedStream to copy.

	MuxedStream(
		const	MuxedStream &	pOther
	);

	virtual StreamPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			StreamCPtr	pStream
	) const;

	/// \brief Returns the format of this MuxedStream.

	MuxedFormat getMuxedFormat(
	) const {
		return format;
	}

	/// \brief Sets the number of elementary streams contained in this
	///	MuxedStream.
	///
	/// All (if any) previously added elementary streams are deleted first.

	void setNbrComponents(
		const	BFC::Uint32		s
	) {
		streams.kill();
		streams.resize( s );
	}

	/// \brief Returns the number of elementary streams contained in this
	///	MuxedStream.

	BFC::Uint32 getNbrComponents(
	) const {
		return streams.getSize();
	}

	/// \brief Sets the definition of the \a pIndex-th sub stream
	///	to \a pStream.

	void setComponent(
		const	BFC::Uint32		pIndex,
			StreamPtr		pStream
	) {
		streams[ pIndex ] = pStream;
	}

	/// \brief Returns the definition of the \a pIndex-th elementary
	///	stream contained in this MuxedStream.

	StreamPtr getComponent(
		const	BFC::Uint32		pIndex
	) const {
		return streams[ pIndex ];
	}

	/// \brief Returns the first child stream whose definition inherits
	///	from the type specified by \a pClassType.
	///
	/// \returns
	///	The first stream matching \a pClassType, or a NULL pointer if
	///	there is no such stream.

	StreamPtr findComponent(
		const	BFC::DL::TypeCPtr &	pClassType
	) const;

	/// \brief Adds a new elementary stream definition to this
	///	MuxedStream.
	///
	/// \param pStream
	///	The new component to add.
	///
	/// \returns
	///	The index of this new component (to be used with getComponent()
	///	for example).

	BFC::Uint32 addComponent(
			StreamPtr		pStream
	) {
		streams += pStream;
		return ( streams.getSize() - 1 );
	}

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

private :

	// internal state...

	MuxedFormat		format;
	BFC::Array< StreamPtr >	streams;

	// others...

	MuxedStream& operator = (
		const	MuxedStream&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_MuxedStream_H_

// ============================================================================

