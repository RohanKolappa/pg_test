// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.VBuffer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VBuffer_H_
#define _BFC_Dyn_VBuffer_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VBufferPtr.h"

// ============================================================================

#include "BFC.Dyn.TBuffer.h"
#include "BFC.Dyn.Var.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Var taking a Buffer value.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VBuffer : public Var {

public :

	/// \brief Creates a new VBuffer.

	VBuffer(
	);

	/// \brief Creates a copy of \a pBuffer.

	VBuffer(
		const	Buffer &	pBuffer
	);

	/// \brief Creates a copy of \a pBuffer.

	VBuffer(
		const	Buffer &	pVarName,
		const	Buffer &	pBuffer
	);

	/// \brief Creates a new VBuffer.

	VBuffer(
		const	TBufferPtr &	pTBuffer
	);

	/// \brief Creates a copy of \a pOther.

	VBuffer(
		const	VBuffer &	pOther
	);

	/// \brief Destroys this object.

	virtual ~VBuffer(
	);

	/// \brief Replaces the content of this object by a copy of \a pOther.

	VBuffer & operator = (
		const	VBuffer &	pOther
	);

	virtual void set(
		const	Buffer &	pTag,
		const	Buffer &	pVal
	);

	virtual Buffer get(
		const	Buffer &	pTag
	) const;

	virtual SPtr clone(
	) const;

	virtual void doSerialize(
			IO::ByteOutputStreamPtr	pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr	pIStream
	);

	virtual Buffer toBuffer(
	) const;

	virtual void fromBuffer(
		const	Buffer &	pBuffer
	);

	const Buffer & getValue(
	) const {
		return v;
	}

	operator const Buffer & () const {
		return v;
	}

	/// \brief Sets the value of this variable to \a pValue, generating a
	///	Msg::ValueEvent event if \a pValue is different than the replaced
	///	value.

	void setValue(
		const	Buffer &	pValue
	);

	virtual void syncObservers(
	) const;

protected :

	/// \brief Callback handling Msg::ValueEvent events.
	///
	/// The Msg::ValueEvent event can be of any subtype (Buffer, Uint32, ...).

	void setValueCB(
			Msg::EventCPtr	pEvent
	);

	void privSetValue(
		const	Buffer &	pValue
	);

private :

	Buffer		v;	///< Value of this variable.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VBuffer_H_

// ============================================================================

