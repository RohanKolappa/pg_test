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
//	BFC.Dyn.VInt.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VInt_H_
#define _BFC_Dyn_VInt_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VIntPtr.h"

// ============================================================================

#include "BFC.Dyn.TInt.h"
#include "BFC.Dyn.Var.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VInt : public Var {

public :

	/// \brief Creates a new VInt.

	VInt(
		const	Int64		pCurValue = ( Int64 )0,
		const	Int64		pMinValue = Int64Min,
		const	Int64		pMaxValue = Int64Max
	);

	/// \brief Creates a new VInt.

	VInt(
		const	Buffer &	pVarName,
		const	Int64		pCurValue = ( Int64 )0,
		const	Int64		pMinValue = Int64Min,
		const	Int64		pMaxValue = Int64Max
	);

	/// \brief Creates a new VInt.

	VInt(
		const	TIntPtr &	pTInt
	);

	/// \brief Creates a copy of \a pOther.

	VInt(
		const	VInt &		pOther
	);

	/// \brief Destroys this object.

	virtual ~VInt(
	);

	/// \brief Replaces the content of this object by a copy of \a pOther.

	VInt & operator = (
		const	VInt &		pOther
	) {
		Var::operator = ( pOther );
		privSetValue( pOther.curValue );
		return *this;
	}

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

	Int64 getMinValue(
	) const {
		return minValue;
	}

	Int64 getMaxValue(
	) const {
		return maxValue;
	}

	Int64 getValue(
	) const {
		return curValue;
	}

	void setMinValue(
		const	Int64		pValue
	);

	void setMaxValue(
		const	Int64		pValue
	);

	void setValue(
		const	Int64		pValue
	);

	void setValue(
		const	Buffer &	pBuffer
	);

	void decValue(
	);

	void incValue(
	);

	VInt & operator = (
		const	Int64		pValue
	) {
		setValue( pValue );
		return *this;
	}

	VInt & operator = (
		const	Buffer &	pBuffer
	) {
		setValue( pBuffer );
		return *this;
	}

	operator Int64() {
		return curValue;
	}

	virtual void syncObservers(
	) const;

	void setOutputBase(
		const	Buffer::Base	pBase
	) {
		convBase = pBase;
	}

	void setOutputSize(
		const	Uint16		pSize
	) {
		convSize = pSize;
	}

	void setOutputFillChar(
		const	Uchar		pChar
	) {
		convChar = pChar;
	}

protected :

	/// \brief Callback handling Msg::ValueEvent events (id = AnyId).
	///
	/// The Msg::ValueEvent event can be of any subtype (Buffer, Integer,
	/// ...).

	void setValueCB(
			Msg::EventCPtr	pEvent
	);

	void privSetMinValue(
		const	Int64		pValue
	);

	void privSetMaxValue(
		const	Int64		pValue
	);

	void privSetValue(
		const	Int64		pValue
	);

	void privSetValue(
		const	Buffer &	pBuffer
	);

private :

	Int64		curValue;	///< Current value.
	Int64		minValue;	///< Minimum value.
	Int64		maxValue;	///< Maximum value.
	Buffer::Base	convBase;	///< Base for ASCII output.
	Uint16		convSize;	///< Minimum size for ASCII output.
	Uchar		convChar;	///< Fill character for ASCII ouptut.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VInt_H_

// ============================================================================

