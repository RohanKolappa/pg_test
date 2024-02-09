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
//	BFC.IO.ByteOutputStream.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_IO_ByteOutputStream_H_
#define _BFC_IO_ByteOutputStream_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

#include "BFC.IO.ByteOutputStreamPtr.h"

// ============================================================================

#include "BFC.Base.Aliasing.h"
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
/// Beside the traditional putBytes() method, that is
/// exported by all Device objects, a ByteOutputStream provides a large
/// number of low-level byte-oriented writing methods, for example to
/// write Uint32, Double, and even FourCC objects.
///
/// \ingroup BFC_IO

class BFC_IO_DLL ByteOutputStream : public ProxyDevice {

public :

	/// \brief Constructs a new ByteOutputStream.

	ByteOutputStream(
			DevicePtr	pPeer
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

	// --------------------------------------------------------------------
	// N bits
	// --------------------------------------------------------------------

	/// \brief Writes \a pValue in Big Endian format, coded on \a pLength
	///	bytes, to the backend.
	///
	/// Only the first \a pLength Less Significant Bytes are actually
	/// written.

	template < typename T >
	void putBEUint(
		const	Uint32		pLength,
		const	T		pValue
	) {
		ensureAvailable( pLength );
		const Uint32	sze = ( Uint32 )sizeof( T );
		Uint32		len = pLength;
		Uchar *		dst = buffer + cursor;
		if ( len > sze ) {
			System::bzero( dst, len - sze );
			dst += len - sze;
			len = sze;
		}
		T		val = System::Host2BE( pValue );
		const Uchar *	src = ( const Uchar * )&val + sze - len;
		while ( len-- ) {
			*dst++ = *src++;
		}
		cursor += ( Uint64 )pLength;
	}

	template < typename T >
	void putBESint(
		const	Uint32		pLength,
		const	T		pValue
	) {
		ensureAvailable( pLength );
		const Uint32	sze = ( Uint32 )sizeof( T );
		Uint32		len = pLength;
		Uchar *		dst = buffer + cursor;
		if ( len > sze ) {
			Uchar pad = ( pValue < 0 ? 0xFF : 0x00 );
			System::memset( dst, pad, len - sze );
			dst += len - sze;
			len = sze;
		}
		T		val = System::Host2BE( pValue );
		const Uchar *	src = ( const Uchar * )&val + sze - len;
		while ( len-- ) {
			*dst++ = *src++;
		}
		cursor += ( Uint64 )pLength;
	}

	// --------------------------------------------------------------------
	// 8 bits: Uchar, Char, Uint8, Int8
	// --------------------------------------------------------------------

	/// \brief Writes an unsigned character (8 bits) to the backend.

	void putUchar(
		const	Uchar		c
	) {
		ensureAvailable( 1 );
		buffer[ cursor++ ] = c;
	}

	/// \brief Writes a signed character (8 bits) to the backend.

	void putChar(
		const	Char		c
	) {
		ensureAvailable( 1 );
		buffer[ cursor++ ] = ( Uchar )c;
	}

	/// \brief Writes an unsigned integer, coded on 8 bits, to the
	///	backend.

	void putUint8(
		const	Uint8		c
	) {
		putUchar( ( Uchar )c );
	}

	/// \brief Writes a signed integer, coded on 8 bits, to the
	///	backend.

	void putInt8(
		const	Int8		c
	) {
		putChar( ( Char )c );
	}

	// --------------------------------------------------------------------
	// 16 bits: Uint16, Int16 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Writes an unsigned integer, coded in Big Endian format on
	///	16 bits, to the backend.

	void putBEUint16(
		const	Uint16		v
	) {
		ensureAvailable( 2 );
		setValue( buffer + cursor, System::Host2BE( v ) );
		cursor += 2;
	}

	/// \brief Writes an unsigned integer, coded in Little Endian format on
	///	16 bits, to the backend.

	void putLEUint16(
		const	Uint16		v
	) {
		ensureAvailable( 2 );
		setValue( buffer + cursor, System::Host2LE( v ) );
		cursor += 2;
	}

	/// \brief Writes a signed integer, coded in Big Endian format on
	///	16 bits, to the backend.

	void putBEInt16(
		const	Int16		v
	) {
		putBEUint16( ( Uint16 )v );
	}

	/// \brief Writes a signed integer, coded in Little Endian format on
	///	16 bits, to the backend.

	void putLEInt16(
		const	Int16		v
	) {
		putLEUint16( ( Uint16 )v );
	}

	// --------------------------------------------------------------------
	// 24 bits: Uint64, Int64 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Writes an unsigned integer, coded in Big Endian format on
	///	24 bits, to the backend.

	void putBEUint24(
		const	Uint32		v
	) {
		ensureAvailable( 3 );
		buffer[ cursor     ] = ( Uchar )( v >> 16 );
		buffer[ cursor + 1 ] = ( Uchar )( v >>  8 );
		buffer[ cursor + 2 ] = ( Uchar )( v       );
		cursor += 3;
	}

	/// \brief Writes an unsigned integer, coded in Little Endian format on
	///	24 bits, to the backend.

	void putLEUint24(
		const	Uint32		v
	) {
		ensureAvailable( 3 );
		buffer[ cursor     ] = ( Uchar )( v       );
		buffer[ cursor + 1 ] = ( Uchar )( v >>  8 );
		buffer[ cursor + 2 ] = ( Uchar )( v >> 16 );
		cursor += 3;
	}

	/// \brief Writes a signed integer, coded in Big Endian format on
	///	24 bits, to the backend.

	void putBEInt24(
		const	Int32		v
	) {
		putBEUint24( ( Uint32 )v );
	}

	/// \brief Writes a signed integer, coded in Little Endian format on
	///	24 bits, to the backend.

	void putLEInt24(
		const	Int32		v
	) {
		putLEUint24( ( Uint32 )v );
	}

	// --------------------------------------------------------------------
	// 32 bits: Uint32, Int32 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Writes an unsigned integer, coded in Big Endian format on
	///	32 bits, to the backend.

	void putBEUint32(
		const	Uint32		v
	) {
		ensureAvailable( 4 );
		setValue( buffer + cursor, System::Host2BE( v ) );
		cursor += 4;
	}

	/// \brief Writes an unsigned integer, coded in Little Endian format on
	///	32 bits, to the backend.

	void putLEUint32(
		const	Uint32		v
	) {
		ensureAvailable( 4 );
		setValue( buffer + cursor, System::Host2LE( v ) );
		cursor += 4;
	}

	/// \brief Writes a signed integer, coded in Big Endian format on
	///	32 bits, to the backend.

	void putBEInt32(
		const	Int32		v
	) {
		putBEUint32( ( Uint32 )v );
	}

	/// \brief Writes a signed integer, coded in Little Endian format on
	///	32 bits, to the backend.

	void putLEInt32(
		const	Int32		v
	) {
		putLEUint32( ( Uint32 )v );
	}

	// --------------------------------------------------------------------
	// 64 bits: Uint64, Int64 / LE, BE
	// --------------------------------------------------------------------

	/// \brief Writes an unsigned integer, coded in Big Endian format on
	///	64 bits, to the backend.

	void putBEUint64(
		const	Uint64		v
	) {
		ensureAvailable( 8 );
		setValue( buffer + cursor, System::Host2BE( v ) );
		cursor += 8;
	}

	/// \brief Writes an unsigned integer, coded in Little Endian format on
	///	64 bits, to the backend.

	void putLEUint64(
		const	Uint64		v
	) {
		ensureAvailable( 8 );
		setValue( buffer + cursor, System::Host2LE( v ) );
		cursor += 8;
	}

	/// \brief Writes a signed integer, coded in Big Endian format on
	///	64 bits, to the backend.

	void putBEInt64(
		const	Int64		v
	) {
		putBEUint64( Uint64( v ) );
	}

	/// \brief Writes a signed integer, coded in Little Endian format on
	///	64 bits, to the backend.

	void putLEInt64(
		const	Int64		v
	) {
		putLEUint64( Uint64( v ) );
	}

	// --------------------------------------------------------------------
	// Floating point: Float, Double / LE, BE
	// --------------------------------------------------------------------

	/// \brief Writes a Double to the backend.
	///
	/// The Double has to be read back using the
	/// ByteInputStream::getDouble() method.

	void putDouble(
		const	Double		d
	);

	void putBEFloat(
		const	Float		f
	) {
		ensureAvailable( 4 );
		union {
			Float	f;
			Uint32	u;
		} u;
		u.f = System::Host2BE( f );
		setValue( buffer + cursor, u.u );
		cursor += 4;
	}

	void putLEFloat(
		const	Float		f
	) {
		ensureAvailable( 4 );
		union {
			Float	f;
			Uint32	u;
		} u;
		u.f = System::Host2LE( f );
		setValue( buffer + cursor, u.u );
		cursor += 4;
	}

	void putBEDouble(
		const	Double		f
	) {
		ensureAvailable( 8 );
		union {
			Double	f;
			Uint64	u;
		} u;
		u.f = System::Host2BE( f );
		setValue( buffer + cursor, u.u );
		cursor += 8;
	}

	void putLEDouble(
		const	Double		f
	) {
		ensureAvailable( 8 );
		union {
			Double	f;
			Uint64	u;
		} u;
		u.f = System::Host2LE( f );
		setValue( buffer + cursor, u.u );
		cursor += 8;
	}

	void putLEBuffer(
		const	Buffer &	pBuffer
	) {
		putLEUint32( pBuffer.getLength() );
		putBytes( pBuffer );
	}

	void putBEBuffer(
		const	Buffer &	pBuffer
	) {
		putBEUint32( pBuffer.getLength() );
		putBytes( pBuffer );
	}

	/// \brief Writes a FourCC (4 bytes) to the backend.

	void putFourCC(
		const	FourCC&		f
	) {
		ensureAvailable( 4 );
		setValue( buffer + cursor, ( Uint32 )f );
		cursor += 4;
	}

	/// \brief Writes a UUId (16 bytes) to the backend.

	void putUUId(
		const	UUId &		u
	) {
		putBytes( u.toCompactBuffer() );
	}

	virtual Uint64 seek(
		const	Uint64		offset
	);

	virtual Uint64 tell(
	) const;

	virtual Uint64 length(
	) const;

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
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_ByteOutputStream_H_

// ============================================================================

