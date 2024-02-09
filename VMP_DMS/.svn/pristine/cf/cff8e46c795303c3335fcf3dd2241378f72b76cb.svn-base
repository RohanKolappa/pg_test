// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::IO".
// 
// "TBFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.IO.ByteOutputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_ByteOutputStream_H_
#define _TBFC_Light_ByteOutputStream_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.FourCC.h"
#include "TBFC.Base.SPtr.h"
#include "TBFC.Base.UUId.h"

#include "TBFC.IO.ProxyDevice.h"

// ============================================================================

namespace TBFC {
namespace IO {

// ============================================================================

TBFC_PTR_DECL( TBFC_Light_DLL, ByteOutputStream )

// ============================================================================

/// \brief Provides a Device with low-level byte-oriented methods.
///
/// Beside the traditional putBytes() method, that is
/// exported by all Device objects, a ByteOutputStream provides a large
/// number of low-level byte-oriented writing methods, for example to
/// write Uint32, Double, and even FourCC objects.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL ByteOutputStream : public ProxyDevice {

public :

	/// \brief Constructs a new ByteOutputStream.

	ByteOutputStream(
			DevicePtr	device
	);

	/// \brief Destroys this ByteOutputStream.

	virtual ~ByteOutputStream(
	);

	virtual void open(
	);

	virtual void putBytes(
		const	Buffer&		buf
	);

	/// \brief Puts buffer \a buf to the backend, appending a NULL character.

	void putAnsiString(
		const	Buffer&		buf
	);

	virtual void flush(
	);

	/// \brief Writes an unsigned character (8 bits) to the backend.

	void putUchar(
		const	Uchar		c
	) {
		ensureAvailable(1);
		buffer[cursor++] = c;
	}

	/// \brief Writes a FourCC (4 bytes) to the backend.

	void putFourCC(
		const	FourCC&		f
	) {
		ensureAvailable( 4 );
		union { Uchar * pb; Uint32 * p4; } p;
		p.pb = buffer + cursor;
		*p.p4 = (Uint32)f;
		cursor += 4;
	}

	/// \brief Writes a UUId (16 bytes) to the backend.

	void putUUId(
		const	UUId &		u
	) {
		putBytes( u.toCompactBuffer() );
	}

// FIXME: in ALL the following methods, we assume we're using a LE host...

	/// \brief Writes \a value in Big Endian format, coded on \a length
	///	bytes, to the backend.

	void putBEUint(
			Uint32		length,
		const	Uint32		value
	) {
		ensureAvailable(length);
		const Uchar	*src = (const Uchar *)&value + length - 1;
		Uchar	*dst = buffer + cursor;
		cursor += Uint64( length );
		while (length--)
			*dst++ = *src--;
	}

	/// \brief Writes an unsigned integer, coded on 8 bits, to the
	///	backend.

	void putBEUint8(
		const	Uint8		c
	) {
		putUchar((const Uchar)c);
	}

	/// \brief Writes an unsigned integer, coded in Big Endian format on
	///	16 bits, to the backend.

	void putBEUint16(
		const	Uint16		v
	) {
		ensureAvailable(2);
		const Uchar	*src = (const Uchar *)&v + 1;
		Uchar	*dst = buffer + cursor;
		*dst++ = *src--;
		*dst++ = *src--;
		cursor += 2;
	}

	/// \brief Writes an unsigned integer, coded in Big Endian format on
	///	32 bits, to the backend.

	void putBEUint32(
		const	Uint32		v
	) {
		ensureAvailable(4);
		const Uchar	*src = (const Uchar *)&v + 3;
		Uchar	*dst = buffer + cursor;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		cursor += 4;
	}

	/// \brief Writes an unsigned integer, coded in Big Endian format on
	///	64 bits, to the backend.

	void putBEUint64(
		const	Uint64		v
	) {
		ensureAvailable(8);
		const Uchar	*src = (const Uchar *)&v + 7;
		Uchar	*dst = buffer + cursor;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		cursor += 8;
	}

	/// \brief Writes a signed integer, coded on 8 bits, to the
	///	backend.

	void putBEInt8(
		const	Int8		v
	) {
		putBEUint8(v);
	}

	/// \brief Writes a signed integer, coded in Big Endian format on
	///	16 bits, to the backend.

	void putBEInt16(
		const	Int16		v
	) {
		putBEUint16(v);
	}

	/// \brief Writes a signed integer, coded in Big Endian format on
	///	32 bits, to the backend.

	void putBEInt32(
		const	Int32		v
	) {
		putBEUint32(v);
	}

	/// \brief Writes a signed integer, coded in Big Endian format on
	///	64 bits, to the backend.

	void putBEInt64(
		const	Int64		v
	) {
		putBEUint64( Uint64( v ) );
	}

	/// \brief Writes an unsigned integer, coded on 8 bits, to the
	///	backend.

	void putLEUint8(
		const	Uint8		c
	) {
		putUchar((const Uchar)c);
	}

	/// \brief Writes an unsigned integer, coded in Little Endian format on
	///	16 bits, to the backend.

	void putLEUint16(
		const	Uint16		v
	) {
		ensureAvailable( 2 );
		union { Uchar * pb; Uint16 * p2; } p;
		p.pb = buffer + cursor;
		*p.p2 = v;
		cursor += 2;
	}

	/// \brief Writes an unsigned integer, coded in Little Endian format on
	///	32 bits, to the backend.

	void putLEUint32(
		const	Uint32		v
	) {
		ensureAvailable( 4 );
		union { Uchar * pb; Uint32 * p4; } p;
		p.pb = buffer + cursor;
		*p.p4 = v;
		cursor += 4;
	}

	/// \brief Writes an unsigned integer, coded in Little Endian format on
	///	64 bits, to the backend.

	void putLEUint64(
		const	Uint64		v
	) {
		ensureAvailable( 8 );
		union { Uchar * pb; Uint64 * p8; } p;
		p.pb = buffer + cursor;
		*p.p8 = v;
		cursor += 8;
	}

	/// \brief Writes a signed integer, coded on 8 bits, to the
	///	backend.

	void putLEInt8(
		const	Int8		v
	) {
		putLEUint8(v);
	}

	/// \brief Writes a signed integer, coded in Little Endian format on
	///	16 bits, to the backend.

	void putLEInt16(
		const	Int16		v
	) {
		putLEUint16(v);
	}

	/// \brief Writes a signed integer, coded in Little Endian format on
	///	32 bits, to the backend.

	void putLEInt32(
		const	Int32		v
	) {
		putLEUint32(v);
	}

	/// \brief Writes a signed integer, coded in Little Endian format on
	///	64 bits, to the backend.

	void putLEInt64(
		const	Int64		v
	) {
		putLEUint64( Uint64( v ) );
	}

	/// \brief Writes a Double to the backend.
	///
	/// The Double has to be read back using the
	/// ByteInputStream::getDouble() method.

	void putDouble(
		const	Double		d
	);

	virtual Uint64 seek(
		const	Uint64		offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

	BASE_CLASS_GEN_EXCEPTION("TBFC.IO.ByteOutputStream");

	CLASS_EXCEPTION(CantFlushDevice, "Can't flush device");

protected :

	/// \brief Flushed the internal cache, until at least \a l bytes
	///	can be written in it.

	void ensureAvailable(
		const	Uint32		l
	);

private :

	// internal state

	enum {
		BlockSize	= 0x1000
	};

	Uchar		buffer[BlockSize];
	Uint64		cursor;

	// others...

	ByteOutputStream(
		const	ByteOutputStream&
	);

	ByteOutputStream& operator = (
		const	ByteOutputStream&
	);

};

// ============================================================================

} // namespace IO
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_ByteOutputStream_H_

// ============================================================================

