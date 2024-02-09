// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.DataStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_DataStream_H_
#define _VMP_DataStream_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.DataStreamPtr.h"

// ============================================================================

#include "VMP.ElementaryStream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL DataStream : public ElementaryStream {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new DataStream.

	DataStream(
	);

	DataStream(
		const	DataStream &	pOther
	);

	DataStream & operator = (
		const	DataStream &	pOther
	);

	virtual StreamPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			StreamCPtr	pStream
	) const;

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

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_DataStream_H_

// ============================================================================

