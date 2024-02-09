// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::IO".
// 
// "BFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.IO.ByteInputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_ByteInputStream_H_
#define _BFC_IO_ByteInputStream_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

#include "BFC.IO.ByteInputStreamPtr.h"

// ============================================================================

#include "BFC.Base.FourCC.h"
#include "BFC.Base.System.h"
#include "BFC.Base.UUId.h"

#include "BFC.IO.ProxyDevice.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

/// \brief Provides a Device with low-level byte-oriented methods.
///
/// Beside the traditional getBytes() method, that is
/// exported by all Device objects, a ByteInputStream provides a large
/// number of low-level byte-oriented reading methods, for example to
/// read Uint32, Double, and even FourCC objects.
///
/// \ingroup BFC_IO

class BFC_IO_DLL ByteInputStream : public ProxyDevice {

public :

	/// \brief Constructs a new ByteInputStream.

	ByteInputStream(
			DevicePtr	device
	);

	virtual void open(
	);

	virtual void close(
	);

	virtual Buffer getBytes(
	);

	/// \brief Peeks exactly (if \a pExact is true) or at most (if
	///	\a pExact is false) \a pLength bytes from the backend.
	///
	/// \returns The bytes that were read.
	///
	/// \throws NotConnected

	Buffer peekBytes(
		const	Uint32		pLength,
		const	Bool		pExact = true
	);

	/// \brief Reads exactly (if \a pExact is true) or at most (if
	///	\a pExact is false) \a pLength bytes from the backend.
	///
	/// \returns The bytes that were read.
	///
	/// \throws NotConnected

	Buffer getBytes(
		const	Uint32		pLength,
		const	Bool		pExact = true
	);

	/// \brief Peeks a whole line, stripping the NL or CR+NL char(s).
	///
	/// \returns The line, without the EOL character(s).
	///
	/// \throws NotConnected

	Buffer peekLine(
	);

	/// \brief Reads a whole line, stripping the NL or CR+NL char(s).
	///
	/// \returns The line, without the EOL character(s).
	///
	/// \throws NotConnected

	Buffer getLine(
	);

	/// \brief Reads a whole line, stripping the NL or CR+NL char(s).
	///
	/// \param concat
	///	If true, a line terminated by a backslash is concatenated by
	///	the following line.
	/// \param shellSyntax
	///	If true, a line starting with a '#' will be skipped
	///
	/// \returns The line, without the EOL character(s).
	///
	/// \throws NotConnected

	Buffer getLine(
		const	Bool		concat,
		const	Bool		shellSyntax = true
	);

	/// \brief Reads bytes until a NULL character is found.
	///
	/// \returns The bytes that were read, without the NULL character.
	///
	/// \throws NotConnected

	Buffer getAnsiString(
	);

	// --------------------------------------------------------------------
	// 8 bits: Uchar, Char, Uint8, Int8
	// --------------------------------------------------------------------

	/// \brief Peeks an unsigned character (8 bits) from the backend.

	Uchar peekUchar(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 1 + pOffset );
		return buffer[ cursor + pOffset ];
	}

	/// \brief Reads an unsigned character (8 bits) from the backend.

	Uchar getUchar(
	) {
		ensureAvailable( 1 );
		return buffer[ cursor++ ];
	}

	/// \brief Peeks a signed character (8 bits) from the backend.

	Char peekChar(
		const	Uint32		pOffset = 0
	) {
		return ( Char )peekUchar( pOffset );
	}

	/// \brief Reads a signed character (8 bits) from the backend.

	Char getChar(
	) {
		return ( Char )getUchar();
	}

	/// \brief Peeks an unsigned integer, coded on 8 bits, from the
	///	backend.

	Uint8 peekUint8(
		const	Uint32		pOffset = 0
	) {
		return ( Uint8 )peekUchar( pOffset );
	}

	/// \brief Reads an unsigned integer, coded on 8 bits, from the
	///	backend.

	Uint8 getUint8(
	) {
		return ( Uint8 )getUchar();
	}

	/// \brief Peeks a signed integer, coded on 8 bits, from the
	///	backend.

	Int8 peekInt8(
		const	Uint32		pOffset = 0
	) {
		return ( Int8 )peekUchar( pOffset );
	}

	/// \brief Reads a signed integer, coded on 8 bits, from the
	///	backend.

	Int8 getInt8(
	) {
		return ( Int8 )getUchar();
	}

	// --------------------------------------------------------------------
	// 16 bits: Uint16, Int16 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Peeks an unsigned integer, coded in Big Endian format
	///	on 16 bits, from the backend.

	Uint16 peekBEUint16(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 2 + pOffset );
		return System::Host2BE( *( Uint16 * )( buffer + cursor + pOffset ) );
	}

	/// \brief Reads an unsigned integer, coded in Big Endian format
	///	on 16 bits, from the backend.

	Uint16 getBEUint16(
	) {
		Uint16 res = peekBEUint16();
		cursor += 2;
		return res;
	}

	/// \brief Peeks an unsigned integer, coded in Little Endian format
	///	on 16 bits, from the backend.

	Uint16 peekLEUint16(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 2 + pOffset );
		return System::Host2LE( *( Uint16 * )( buffer + cursor + pOffset ) );
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 16 bits, from the backend.

	Uint16 getLEUint16(
	) {
		Uint16 res = peekLEUint16();
		cursor += 2;
		return res;
	}

	/// \brief Peeks a signed integer, coded in Big Endian format
	///	on 16 bits, from the backend.

	Int16 peekBEInt16(
		const	Uint32		pOffset = 0
	) {
		return ( Int16 )peekBEUint16( pOffset );
	}

	/// \brief Reads a signed integer, coded in Big Endian format
	///	on 16 bits, from the backend.

	Int16 getBEInt16(
	) {
		return ( Int16 )getBEUint16();
	}

	/// \brief Peeks a signed integer, coded in Little Endian format
	///	on 16 bits, from the backend.

	Int16 peekLEInt16(
		const	Uint32		pOffset = 0
	) {
		return ( Int16 )peekLEUint16( pOffset );
	}

	/// \brief Reads a signed integer, coded in Little Endian format
	///	on 16 bits, from the backend.

	Int16 getLEInt16(
	) {
		return ( Int16 )getLEUint16();
	}

	// --------------------------------------------------------------------
	// 24 bits: Uint64, Int64 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Peeks an unsigned integer, coded in Big Endian format
	///	on 24 bits, from the backend.

	Uint32 peekBEUint24(
	) {
		ensureAvailable( 3 );
		return ( ( ( Uint32 )( buffer[ cursor     ] ) << 16 )
		       | ( ( Uint32 )( buffer[ cursor + 1 ] ) <<  8 )
		       | ( ( Uint32 )( buffer[ cursor + 2 ] )       ) );
	}

	/// \brief Reads an unsigned integer, coded in Big Endian format
	///	on 24 bits, from the backend.

	Uint32 getBEUint24(
	) {
		Uint32 val = peekBEUint24();
		cursor += 3;
		return val;
	}

	/// \brief Peeks an unsigned integer, coded in Little Endian format
	///	on 24 bits, from the backend.

	Uint32 peekLEUint24(
	) {
		ensureAvailable( 3 );
		return ( ( ( Uint32 )( buffer[ cursor     ] )       )
		       | ( ( Uint32 )( buffer[ cursor + 1 ] ) <<  8 )
		       | ( ( Uint32 )( buffer[ cursor + 2 ] ) << 16 ) );
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 24 bits, from the backend.

	Uint32 getLEUint24(
	) {
		Uint32 val = peekLEUint24();
		cursor += 3;
		return val;
	}

	/// \brief Peeks a signed integer, coded in Big Endian format
	///	on 24 bits, from the backend.

	Int32 peekBEInt24(
	) {
		ensureAvailable( 3 );
		Uint32 v = ( ( ( Uint32 )( buffer[ cursor     ] ) << 16 )
		           | ( ( Uint32 )( buffer[ cursor + 1 ] ) <<  8 )
		           | ( ( Uint32 )( buffer[ cursor + 2 ] )       ) );
		if ( v & 0x00800000 ) {
			v |= 0xFF000000;
		}
		return ( Int32 )v;
	}

	/// \brief Reads a signed integer, coded in Big Endian format
	///	on 24 bits, from the backend.

	Int32 getBEInt24(
	) {
		Int32 val = peekBEInt24();
		cursor += 3;
		return val;
	}

	/// \brief Peeks a signed integer, coded in Little Endian format
	///	on 24 bits, from the backend.

	Int32 peekLEInt24(
	) {
		ensureAvailable( 3 );
		Uint32 v = ( ( ( Uint32 )( buffer[ cursor     ] )       )
		           | ( ( Uint32 )( buffer[ cursor + 1 ] ) <<  8 )
		           | ( ( Uint32 )( buffer[ cursor + 2 ] ) << 16 ) );
		if ( v & 0x00800000 ) {
			v |= 0xFF000000;
		}
		return ( Int32 )v;
	}

	/// \brief Reads a signed integer, coded in Little Endian format
	///	on 24 bits, from the backend.

	Int32 getLEInt24(
	) {
		Int32 val = peekLEInt24();
		cursor += 3;
		return val;
	}

	// --------------------------------------------------------------------
	// 32 bits: Uint32, Int32 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Peeks an unsigned integer, coded in Big Endian format
	///	on 32 bits, from the backend.

	Uint32 peekBEUint32(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 4 + pOffset );
		return System::Host2BE( *( Uint32 * )( buffer + cursor + pOffset ) );
	}

	/// \brief Reads an unsigned integer, coded in Big Endian format
	///	on 32 bits, from the backend.

	Uint32 getBEUint32(
	) {
		ensureAvailable( 4 );
		Uint32	val = *( Uint32 * )( buffer + cursor );
		cursor += 4;
		return System::Host2BE( val );
	}

	/// \brief Peeks an unsigned integer, coded in Little Endian format
	///	on 32 bits, from the backend.

	Uint32 peekLEUint32(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 4 + pOffset );
		return System::Host2LE( *( Uint32 * )( buffer + cursor + pOffset ) );
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 32 bits, from the backend.

	Uint32 getLEUint32(
	) {
		ensureAvailable( 4 );
		Uint32 val = *( Uint32 * )( buffer + cursor );
		cursor += 4;
		return System::Host2LE( val );
	}

	/// \brief Peeks a signed integer, coded in Big Endian format
	///	on 32 bits, from the backend.

	Int32 peekBEInt32(
		const	Uint32		pOffset = 0
	) {
		return ( Int32 )peekBEUint32( pOffset );
	}

	/// \brief Reads a signed integer, coded in Big Endian format
	///	on 32 bits, from the backend.

	Int32 getBEInt32(
	) {
		return ( Int32 )getBEUint32();
	}

	/// \brief Peeks a signed integer, coded in Little Endian format
	///	on 32 bits, from the backend.

	Int32 peekLEInt32(
		const	Uint32		pOffset = 0
	) {
		return ( Int32 )peekLEUint32( pOffset );
	}

	/// \brief Reads a signed integer, coded in Little Endian format
	///	on 32 bits, from the backend.

	Int32 getLEInt32(
	) {
		return ( Int32 )getLEUint32();
	}

	// --------------------------------------------------------------------
	// 64 bits: Uint64, Int64 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Peeks an unsigned integer, coded in Big Endian format
	///	on 64 bits, from the backend.

	Uint64 peekBEUint64(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 8 + pOffset );
		return System::Host2BE( *( Uint64 * )( buffer + cursor + pOffset ) );
	}

	/// \brief Reads an unsigned integer, coded in Big Endian format
	///	on 64 bits, from the backend.

	Uint64 getBEUint64(
	) {
		ensureAvailable( 8 );
		Uint64	val = *( Uint64 * )( buffer + cursor );
		cursor += 8;
		return System::Host2BE( val );
	}

	/// \brief Peeks an unsigned integer, coded in Little Endian format
	///	on 64 bits, from the backend.

	Uint64 peekLEUint64(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 8 + pOffset );
		return System::Host2LE( *( Uint64 * )( buffer + cursor + pOffset ) );
	}

	/// \brief Reads an unsigned integer, coded in Little Endian format
	///	on 64 bits, from the backend.

	Uint64 getLEUint64(
	) {
		ensureAvailable( 8 );
		Uint64 val = *( Uint64 * )( buffer + cursor );
		cursor += 8;
		return System::Host2LE( val );
	}

	/// \brief Peeks a signed integer, coded in Big Endian format
	///	on 64 bits, from the backend.

	Int64 peekBEInt64(
		const	Uint32		pOffset = 0
	) {
		return ( Int64 )peekBEUint64( pOffset );
	}

	/// \brief Reads a signed integer, coded in Big Endian format
	///	on 64 bits, from the backend.

	Int64 getBEInt64(
	) {
		return ( Int64 )getBEUint64();
	}

	/// \brief Peeks a signed integer, coded in Little Endian format
	///	on 64 bits, from the backend.

	Int64 peekLEInt64(
		const	Uint32		pOffset = 0
	) {
		return ( Int64 )peekLEUint64( pOffset );
	}

	/// \brief Reads a signed integer, coded in Little Endian format
	///	on 64 bits, from the backend.

	Int64 getLEInt64(
	) {
		return ( Int64 )getLEUint64();
	}

	/// \brief Reads an unsigned integer, coded on \a length bytes, from
	///	the backend.
	///
	/// \param length
	///	The number of bytes used to store the integer. Must be between
	///	1 and 4.
	///
	/// \returns The integer.
	///
	/// \throws InvalidArgument

	Uint32 getBEUint(
			Uint32	length
	);

	/// \brief Peeks a FourCC (4 bytes) from the backend.

	FourCC peekFourCC(
		const	Uint32		pOffset = 0
	) {
		ensureAvailable( 4 + pOffset );
		Uint32 val = *( const Uint32 * )( buffer + cursor + pOffset );
		return FourCC( val );
	}

	/// \brief Reads a FourCC (4 bytes) from the backend.

	FourCC getFourCC(
	) {
		ensureAvailable( 4 );
		Uint32 val = *(const Uint32 *)(buffer + cursor);
		cursor += 4;
		return FourCC(val);
	}

	/// \brief Peeks a UUId (16 bytes) from the backend.

	UUId peekUUId(
		const	Uint32		pOffset = 0
	) {
		UUId res;
		res.fromCompactBuffer( peekBytes( pOffset + 16, true )( pOffset ) );
		return res;
	}

	/// \brief Reads a UUId (16 bytes) from the backend.

	UUId getUUId(
	) {
		UUId res;
		res.fromCompactBuffer( getBytes( 16 ) );
		return res;
	}

	/// \brief Reads a Double from the backend.
	///
	/// The Double had to be written using the
	/// ByteOutputStream::putDouble() method.

	Double getDouble(
	);

	// --------------------------------------------------------------------
	// Floating point: Float, Double / LE, BE
	// --------------------------------------------------------------------

	Float getBEFloat(
	) {
		ensureAvailable( 4 );
		Float	f;
		f = System::Host2BE( *( Float * )( buffer + cursor ) );
		cursor += 4;
		return f;
	}

	Float getLEFloat(
	) {
		ensureAvailable( 4 );
		Float	f;
		f = System::Host2LE( *( Float * )( buffer + cursor ) );
		cursor += 4;
		return f;
	}

	Double getBEDouble(
	) {
		ensureAvailable( 8 );
		Double	d;
		d = System::Host2BE( *( Double * )( buffer + cursor ) );
		cursor += 8;
		return d;
	}

	Double getLEDouble(
	) {
		ensureAvailable( 8 );
		Double	d;
		d = System::Host2LE( *( Double * )( buffer + cursor ) );
		cursor += 8;
		return d;
	}

	Buffer getLEBuffer(
	) {
		return getBytes( getLEUint32() );
	}

	Buffer getBEBuffer(
	) {
		return getBytes( getBEUint32() );
	}

	virtual Uint64 seek(
		const	Uint64		offset
	);

	virtual Uint64 skip(
		const	Uint64		offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

	virtual Bool atEOS(
	) const {
		return ( cursor == filled && getPeer()->atEOS() );
	}

protected :

	/// \brief Refills the internal cache, by reading more bytes from the
	///	backend.

	void ensureAvailable(
		const	Uint32		atLeast,
		const	Bool		pExact = true
	) {
		if ( cursor + atLeast > filled ) {
			refill( atLeast, pExact );
		}
	}

	void refill(
		const	Uint32		atLeast,
		const	Bool		pExact
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

	Buffer		cache;

	const Uchar *	buffer;		///< First byte in "cache".
	Uint32		cursor;		///< Position in "cache".
	Uint32		filled;		///< How many bytes in "cache".
	Uint64		offset;		///< Pos of "cache" in stream.

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
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_ByteInputStream_H_

// ============================================================================

