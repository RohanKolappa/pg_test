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
//	TBFC.IO.ByteInputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_ByteInputStream_H_
#define _TBFC_Light_ByteInputStream_H_

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

TBFC_PTR_DECL( TBFC_Light_DLL, ByteInputStream )

// ============================================================================

/// \brief Provides a Device with low-level byte-oriented methods.
///
/// Beside the traditional getBytes() method, that is
/// exported by all Device objects, a ByteInputStream provides a large
/// number of low-level byte-oriented reading methods, for example to
/// read Uint32, Double, and even FourCC objects.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL ByteInputStream : public ProxyDevice {

public :

	/// \brief Constructs a new ByteInputStream.

	ByteInputStream(
			DevicePtr	device
	);

	/// \brief Destroys this ByteInputStream.

	virtual ~ByteInputStream(
	);

	virtual void open(
	);

	virtual void close(
	);

	virtual Buffer getBytes(
	);

	/// \brief Reads exactly \a length bytes from the backend, but DOES NOT
	///	remove them!

	Buffer peekBytes(
		const	Uint32		length
	);

	/// \brief Reads exactly \a length bytes from the backend.
	///
	/// \returns The bytes that were read.
	///
	/// \throws NotConnected

	Buffer getBytes(
		const	Uint32		length
	);

	/// \brief Reads a whole line, stripping the NL or CR+NL char(s).
	///
	/// \param concat
	///	If true, a line terminated by a backslash is concatenated by
	///	the following line.
	///
	/// \returns The line, without the EOL character(s).
	///
	/// \throws NotConnected

	Buffer getLine(
		const	Bool		concat = false
	);

	/// \brief Reads bytes until a NULL character is found.
	///
	/// \returns The bytes that were read, without the NULL character.
	///
	/// \throws NotConnected

	Buffer getAnsiString(
	);

	/// \brief Reads an unsigned character (8 bits) from the backend.

	Uchar getUchar(
	) {
		ensureAvailable(1);
		return buffer[cursor++];
	}

	/// \brief Reads a FourCC (4 bytes) from the backend.

	FourCC getFourCC(
	) {
		ensureAvailable(4);
		Uint32 v = *(const Uint32 *)(buffer + cursor);
		cursor += 4;
		return FourCC(v);
	}

	/// \brief Reads a UUId (16 bytes) from the backend.

	UUId getUUId(
	) {
		UUId res;
		res.fromCompactBuffer( getBytes( 16 ) );
		return res;
	}

// FIXME: in ALL the following methods, we assume we're using a LE host...

	/// \brief Reads an unsigned integer, coded on \a length bytes, from
	///	the backend.
	///
	/// \param length
	///	The number of bytes used to store the integer. Must be between
	///	1 and 3.
	///
	/// \returns The integer.
	///
	/// \throws InvalidArgument

	Uint32 getBEUint(
			Uint32	length
	);

	/// \brief Reads an unsigned integer, coded on 8 bits, from the
	///	backend.

	Uint8 getBEUint8(
	) {
		return (Uint8)getUchar();
	}

	/// \brief Reads an unsigned integer, coded in Big Endian format
	///	on 16 bits, from the backend.

	Uint16 getBEUint16(
	) {
		ensureAvailable(2);
		Uint16	v;
		Uchar	*dst = (Uchar *)&v;
		const Uchar	*src = buffer + cursor + 1;
		*dst++ = *src--;
		*dst   = *src  ;
		cursor += 2;
		return v;
	}

	/// \brief Reads an unsigned integer, coded in Big Endian format
	///	on 32 bits, from the backend.

	Uint32 getBEUint32(
	) {
		ensureAvailable(4);
		Uint32	v;
		Uchar	*dst = (Uchar *)&v;
		const Uchar	*src = buffer + cursor + 3;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst   = *src  ;
		cursor += 4;
		return v;
	}

	/// \brief Reads an unsigned integer, coded in Big Endian format
	///	on 64 bits, from the backend.

	Uint64 getBEUint64(
	) {
		ensureAvailable(8);
		Uint64	v;
		Uchar	*dst = (Uchar *)&v;
		const Uchar	*src = buffer + cursor + 7;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst++ = *src--;
		*dst   = *src  ;
		cursor += 8;
		return v;
	}

	/// \brief Reads a signed integer, coded on 8 bits, from the
	///	backend.

	Int8 getBEInt8(
	) {
		return (Int8)getUchar();
	}

	/// \brief Reads a signed integer, coded in Big Endian format
	///	on 16 bits, from the backend.

	Int16 getBEInt16(
	) {
		return (Int16)getBEUint16();
	}

	/// \brief Reads a signed integer, coded in Big Endian format
	///	on 32 bits, from the backend.

	Int32 getBEInt32(
	) {
		return (Int32)getBEUint32();
	}

	/// \brief Reads a signed integer, coded in Big Endian format
	///	on 64 bits, from the backend.

	Int64 getBEInt64(
	) {
		return (Int64)getBEUint64();
	}

	/// \brief Reads an unsigned integer, coded on 8 bits, from the
	///	backend.

	Uint8 getLEUint8(
	) {
		return (Uint8)getUchar();
	}

	Uint16 peekLEUint16(
	) {
		ensureAvailable( 2 );
		return *(const Uint16 *)( buffer + cursor );
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 16 bits, from the backend.

	Uint16 getLEUint16(
	) {
		ensureAvailable( 2 );
		Uint16 v = *(const Uint16 *)( buffer + cursor );
		cursor += 2;
		return v;
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 24 bits, from the backend.

	Uint32 getLEUint24(
	) {
		ensureAvailable(3);
		Uint32 v = (((Uint32)*(const Uint16 *)(buffer + cursor    )      )
		         |  ((Uint32)*(const Uchar  *)(buffer + cursor + 2) << 16));
		cursor += 3;
		return v;
	}

	Uint32 peekLEUint32(
	) {
		ensureAvailable( 4 );
		return *(const Uint32 *)( buffer + cursor );
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 32 bits, from the backend.

	Uint32 getLEUint32(
	) {
		ensureAvailable(4);
		Uint32 v = *(const Uint32 *)(buffer + cursor);
		cursor += 4;
		return v;
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 64 bits, from the backend.

	Uint64 getLEUint64(
	) {
		ensureAvailable(8);
		Uint64 v = *(const Uint64 *)(buffer + cursor);
		cursor += 8;
		return v;
	}

	/// \brief Reads a signed integer, coded on 8 bits, from the
	///	backend.

	Int8 getLEInt8(
	) {
		return (Int8)getUchar();
	}

	/// \brief Reads a signed integer, coded in Little Endian format
	///	on 16 bits, from the backend.

	Int16 getLEInt16(
	) {
		return (Int16)getLEUint16();
	}

	/// \brief Reads a signed integer, coded in Little Endian format
	///	on 32 bits, from the backend.

	Int32 getLEInt32(
	) {
		return (Int32)getLEUint32();
	}

	/// \brief Reads a signed integer, coded in Little Endian format
	///	on 64 bits, from the backend.

	Int64 getLEInt64(
	) {
		return (Int64)getLEUint64();
	}

	/// \brief Reads a Double from the backend.
	///
	/// The Double had to be written using the
	/// ByteOutputStream::putDouble() method.

	Double getDouble(
	);

	virtual Uint64 seek(
		const	Uint64		offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

	virtual Bool atEOS(
	) const {
		return ( ! device || ( cursor == filled && device->atEOS() ) );
	}

protected :

	/// \brief Refills the internal cache, by reading more bytes from the
	///	backend.

	void ensureAvailable(
		const	Uint32		atLeast
	);

private :

	// internal state...

	//                           |<--------- CACHED -------->|
	//                           |                           |
	// +-------------------------+-------------------+-------+
	// |        NOT CACHED       |    Already read   |       |
	// +-------------------------+-------------------+-------+
	//                           |                   |       |
	//                           |<----- cursor ---->|       |
	//                           |<--------- filled -------->|
	// |<------- offset -------->|
	// |<----------------- engine->tell() ------------------>|

	Buffer			cache;

	const Uchar		* buffer;	///< First byte in "cache".
	Uint32			cursor;		///< Position in "cache".
	Uint32			filled;		///< How many bytes in "cache".
	Uint64			offset;		///< Pos of "cache" in stream.

	// others...

	ByteInputStream(
		const	ByteInputStream&
	);

	ByteInputStream& operator = (
		const	ByteInputStream&
	);

};

// ============================================================================

} // namespace IO
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_ByteInputStream_H_

// ============================================================================

