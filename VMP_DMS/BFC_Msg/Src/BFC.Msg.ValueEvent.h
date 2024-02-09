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
//	BFC.Msg.ValueEvent.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_ValueEvent_H_
#define _BFC_Msg_ValueEvent_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

BFC_PTR_DECL( BFC_Msg_DLL, ValueEvent )
BFC_PTR_DECL( BFC_Msg_DLL, BoundedValueEvent )

} // namespace Msg
} // namespace BFC

// ============================================================================

#include "BFC.Msg.Event.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL ValueEvent : public Event {

public :

	static const UUId & classUUId(
	);

	enum Type {
		TypeBoolean,
		TypeInteger,
		TypeFloating,
		TypeBuffer
	};

	ValueEvent(
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( 0 ) {
	}

	ValueEvent(
			Bool		pBool
	) :
		Event	( classUUId() ),
		mType	( TypeBoolean ),
		mBoolean( pBool ) {
	}

	ValueEvent(
			Uchar		pUchar
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( ( Int64 )( Uint64 )pUchar ) {
	}

	ValueEvent(
			Char		pChar
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( ( Int64 )pChar ) {
	}

	ValueEvent(
			Uint16		pUint16
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( ( Int64 )( Uint64 )pUint16 ) {
	}

	ValueEvent(
			Int16		pInt16
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( ( Int64 )pInt16 ) {
	}

	ValueEvent(
			Uint32		pUint32
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( ( Int64 )( Uint64 )pUint32 ) {
	}

	ValueEvent(
			Int32		pInt32
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( ( Int64 )pInt32 ) {
	}

	ValueEvent(
			Uint64		pUint64
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( ( Int64 )pUint64 ) {
	}

	ValueEvent(
			Int64		pInt64
	) :
		Event	( classUUId() ),
		mType	( TypeInteger ),
		mInteger( pInt64 ) {
	}

protected :

	ValueEvent(
			Int64		pInt64,
		const	UUId &		pUUId
	) :
		Event	( pUUId ),
		mType	( TypeInteger ),
		mInteger( pInt64 ) {
	}

public :

	/// \brief Creates a new ValueEvent storing Float \a pFloat.

	ValueEvent(
			Float		pFloat
	) :
		Event	( classUUId() ),
		mType	( TypeFloating ),
		mDouble	( ( Double )pFloat ) {
	}

	/// \brief Creates a new ValueEvent storing Double \a pDouble.

	ValueEvent(
			Double		pDouble
	) :
		Event	( classUUId() ),
		mType	( TypeFloating ),
		mDouble	( pDouble ) {
	}

protected :

	ValueEvent(
			Double		pDouble,
		const	UUId &		pUUId
	) :
		Event	( pUUId ),
		mType	( TypeFloating ),
		mDouble	( pDouble ) {
	}

public :

	/// \brief Creates a new ValueEvent storing Buffer \a pBuffer.

	ValueEvent(
		const	Buffer &	pBuffer
	) :
		Event	( classUUId() ),
		mType	( TypeBuffer ),
		mBuffer	( pBuffer ) {
	}

	virtual Buffer toBuffer(
	) const;

	Type getType(
	) const {
		return mType;
	}

	Bool getBool(
	) const {
		if ( mType != TypeBoolean ) {
			throwException();
		}
		return mBoolean;
	}

	Uchar getUchar(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return ( Uchar )( Char )mInteger;
	}

	Char getChar(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return ( Char )mInteger;
	}

	Uint16 getUint16(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return ( Uint16 )( Int16 )mInteger;
	}

	Int16 getInt16(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return ( Int16 )mInteger;
	}

	Uint32 getUint32(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return ( Uint32 )( Int32 )mInteger;
	}

	Int32 getInt32(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return ( Int32 )mInteger;
	}

	Uint64 getUint64(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return ( Uint64 )mInteger;
	}

	Int64 getInt64(
	) const {
		if ( mType != TypeInteger ) {
			throwException();
		}
		return mInteger;
	}

	Float getFloat(
	) const {
		if ( mType != TypeFloating ) {
			throwException();
		}
		return ( Float )mDouble;
	}

	Double getDouble(
	) const {
		if ( mType != TypeFloating ) {
			throwException();
		}
		return mDouble;
	}

	const Buffer & getBuffer(
	) const {
		if ( mType != TypeBuffer ) {
			throwException();
		}
		return mBuffer;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr		pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr		pIStream
	);

protected :

	void throwException(
	) const;

private :

	Type		mType;
	union {
		Bool		mBoolean;
		Int64		mInteger;
		Double		mDouble;
	};
	Buffer		mBuffer;

	/// \brief Forbidden copy constructor.

	ValueEvent(
		const	ValueEvent &
	);

	/// \brief Forbidden copy operator.

	ValueEvent & operator = (
		const	ValueEvent &
	);

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL BoundedValueEvent : public ValueEvent {

public :

	static const UUId & classUUId(
	);

	BoundedValueEvent(
	) :
		ValueEvent	( ( Int64 )0, classUUId() ),
		mMinInteger	( ( Int64 )0 ),
		mMaxInteger	( ( Int64 )0 ) {
	}

	BoundedValueEvent(
		const	Int64		pCurValue,
		const	Int64		pMinValue,
		const	Int64		pMaxValue
	) :
		ValueEvent	( pCurValue, classUUId() ),
		mMinInteger	( pMinValue ),
		mMaxInteger	( pMaxValue ) {
	}

	BoundedValueEvent(
		const	Double		pCurValue,
		const	Double		pMinValue,
		const	Double		pMaxValue
	) :
		ValueEvent	( pCurValue, classUUId() ),
		mMinDouble	( pMinValue ),
		mMaxDouble	( pMaxValue ) {
	}

	Int64 getInt64MinValue(
	) const {
		if ( getType() != TypeInteger ) {
			throwException();
		}
		return mMinInteger;
	}

	Int64 getInt64MaxValue(
	) const {
		if ( getType() != TypeInteger ) {
			throwException();
		}
		return mMaxInteger;
	}

	Double getDoubleMinValue(
	) const {
		if ( getType() != TypeFloating ) {
			throwException();
		}
		return mMinDouble;
	}

	Double getDoubleMaxValue(
	) const {
		if ( getType() != TypeFloating ) {
			throwException();
		}
		return mMaxDouble;
	}

	virtual Buffer toBuffer(
	) const;

	virtual void doSerialize(
			IO::ByteOutputStreamPtr		pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr		pIStream
	);

protected :

private :

	union {
		Int64		mMinInteger;
		Double		mMinDouble;
	};

	union {
		Int64		mMaxInteger;
		Double		mMaxDouble;
	};

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_ValueEvent_H_

// ============================================================================

