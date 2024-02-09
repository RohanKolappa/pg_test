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
//	BFC.Dyn.VFloat.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VFloat_H_
#define _BFC_Dyn_VFloat_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VFloatPtr.h"

// ============================================================================

#include "BFC.Dyn.TFloat.h"
#include "BFC.Dyn.Var.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VFloat : public Var {

public :

	/// \brief Creates a new VFloat.

	VFloat(
		const	Double		pCurValue = ( Double )0,
		const	Double		pMinValue = - DoubleMax,
		const	Double		pMaxValue = DoubleMax
	);

	/// \brief Creates a new VFloat.

	VFloat(
		const	Buffer &	pVarName,
		const	Double		pCurValue = ( Double )0,
		const	Double		pMinValue = - DoubleMax,
		const	Double		pMaxValue = DoubleMax
	);

	/// \brief Creates a new VFloat.

	VFloat(
		const	TFloatPtr &	pTFloat
	);

	/// \brief Creates a copy of \a pOther.

	VFloat(
		const	VFloat &	pOther
	);

	/// \brief Destroys this object.

	virtual ~VFloat(
	);

	/// \brief Replaces the content of this object by a copy of \a pOther.

	VFloat & operator = (
		const	VFloat &	pOther
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

	Double getMinValue(
	) const {
		return minValue;
	}

	Double getMaxValue(
	) const {
		return maxValue;
	}

	Double getValue(
	) const {
		return curValue;
	}

	void setMinValue(
		const	Double		pValue
	);

	void setMaxValue(
		const	Double		pValue
	);

	void setValue(
		const	Double		pValue
	);

	void setValue(
		const	Buffer &	pBuffer
	);

	VFloat & operator = (
		const	Double		pValue
	) {
		setValue( pValue );
		return *this;
	}

	VFloat & operator = (
		const	Buffer &	pBuffer
	) {
		setValue( pBuffer );
		return *this;
	}

	operator Double() {
		return curValue;
	}

	virtual void syncObservers(
	) const;

protected :

	/// \brief Callback handling Msg::ValueEvent events (id = AnyId).
	///
	/// The Msg::ValueEvent event can be of any subtype (Buffer, Integer,
	/// ...).

	void setValueCB(
			Msg::EventCPtr	pEvent
	);

	void privSetMinValue(
		const	Double		pValue
	);

	void privSetMaxValue(
		const	Double		pValue
	);

	void privSetValue(
		const	Double		pValue
	);

	void privSetValue(
		const	Buffer &	pBuffer
	);

private :

	Double		curValue;	///< Current value.
	Double		minValue;	///< Minimum value.
	Double		maxValue;	///< Maximum value.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VFloat_H_

// ============================================================================

