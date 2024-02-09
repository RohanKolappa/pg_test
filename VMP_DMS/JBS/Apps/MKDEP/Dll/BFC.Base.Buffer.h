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
//	BFC.Base.Buffer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Buffer_H_
#define _BFC_Buffer_H_

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

class BufferData;

// ============================================================================

/// A simple buffer class (a.k.a. a string).

/// Buffer objects are used to handle sequences of 8-bits unsigned chars.
///
/// This class uses a copy-on-write mechanism in order to let multiple
/// Buffer objects share the same data until some object want to modify
/// some byte, in which case a 'personal' copy of the data is made.
///
/// Numerous constructors allow one to build Buffer objects representing
/// existing null-terminated (C-style) strings, integers, floats, ...
/// (those constructors are to be compared with the 'printf()' family of
/// traditional C functions).
///
/// Numerous methods allow one to convert between ASCII and numerical
/// representations of integers, floats, ... (those methods are to be
/// compared with the 'atoi', 'atod', ... C functions).
///
/// All accesses to the internal data are checked against possible 'out of
/// range' conditions. Note that in the case such a condition arise, no C++
/// exception is thrown.
///
/// See the documentation below for more information.
///
/// \ingroup Common_Base

class Buffer {

public :

	/// NULL-terminated string copy constructor.

	/// This constructor creates a deep copy of the NULL-terminated
	/// character string pointed to by \a ptr.
	/// If \a ptr is \b NULL, an empty object is created.
	/// Otherwise, bytes from \a ptr are copied until a NULL is found,
	/// which is NOT copied in this object!!!

	Buffer(
		const	Char	*ptr
	);

	/// String copy constructor.

	/// This constructor creates a deep copy of the
	/// byte string pointed to by \a ptr.
	/// If \a ptr is \b NULL, an empty object is created.
	/// Otherwise, \a length bytes are copied, the default value \b -1
	/// meaning that \a ptr points to a NULL-terminated byte string,
	/// which is entirely copied, as in the constructor above.

	Buffer(
		const	Uchar	*ptr = 0,
		const	Int32	length = -1
	);

	/// Single signed char copy constructor.

	/// This simple constructor creates a 1-byte long buffer whose
	/// content is a copy of the signed character \a c.

	Buffer(
		const	Char	c
	);

	/// Single unsigned char copy constructor.

	/// This simple constructor creates a 1-byte long buffer whose
	/// content is a copy of the unsigned character \a c.

	Buffer(
		const	Uchar	c
	);

	/// Signed integer to ASCII conversion constructor.

	/// This simple constructor creates an ASCII decimal representation
	/// of the 32-bits signed value \a i.

	Buffer(
		const	Int32	i
	);

	/// Enumerated list of bases that can be used in some constructors.

	enum Base {
		Base2	= 2,	///< Binary base (0..1)
		Base8	= 8,	///< Octal base (0..7)
		Base10	= 10,	///< Decimal base (0..9)
		Base16	= 16	///< Hexadecimal base (0..9, A..F)
	};

	/// Unsigned integer to ASCII conversion constructor.

	/// This simple constructor creates an ASCII decimal representation
	/// of the 32-bits unsigned value \a i.
	/// A base different than the decimal one can be used to perform
	/// the ASCII conversion.
	/// It is also possible to specify the minimum length the result
	/// should have.
	///
	/// Examples:
	/// - Buffer(1024) gives "1024"
	/// - Buffer(1024, Buffer::Base16) gives "0x400"
	/// - Buffer(1024, Buffer::Base2, 6) gives "0b10000000000"
	/// - Buffer(1024, Buffer::Base8, 6) gives "0o002000"
	/// - Buffer(1024, Buffer::Base10, 6) gives "001024"
	/// - Buffer(1024, Buffer::Base16, 6) gives "0x000400"

	Buffer(
		const	Uint32		integer,
		const	Base		base      = Base10,
		const	Uint32		minLength = 1
	);

	/// Signed integer to ASCII conversion constructor.

	/// This simple constructor creates an ASCII decimal representation
	/// of the 64-bits signed value \a i.

	Buffer(
		const	Int64		i
	);

	/// Unsigned integer to ASCII conversion constructor.

	/// This simple constructor creates an ASCII decimal representation
	/// of the 64-bits unsigned value \a i.

	Buffer(
		const	Uint64		i
	);

	/// Double to ASCII conversion constructor.

	/// This simple constructor creates an ASCII decimal representation
	/// of the double value \a d, using a C-like format string
	/// \a format (provided without the leading '%' and the terminating
	/// 'f').

	Buffer(
		const	Double		d,
		const	Char		*format = 0
	);

	/// Double to ASCII conversion constructor.

	Buffer(
		const	Double		d,
		const	Buffer&		format
	);

	/// Copy constructor.

	/// This constructor creates a `copy-on-write' copy of the source
	/// object \a b. That is, the internal representation of the data
	/// will be shared by both objects (\a this and \a b) the data has
	/// to be changed in some way, in which case a 'personal' copy is
	/// first made.

	Buffer(
		const	Buffer&		b
	);

	/// Destructor.

	/// This destructor deallocates any resource used by this object,
	/// if not shared with other Buffer objects.

	virtual ~Buffer();

	/// Pseudo-copy method.

	/// This method replaces the actual content of this object by the
	/// byte string pointed to by \a ptr. The user should not try to
	/// access the memory area pointed to by \a ptr after
	/// calling this method, nor should he try to deallocate this area
	/// himself, as it will be automatically done by the destructor.
	///
	/// The memory pointed to by \a ptr should be allocated using
	/// the "new" operator: "new Uchar[len]" or "new Char[len]".
	///
	/// If \a len is -1, \a ptr must point to a NULL-terminated string,
	/// whose length is first evaluated.
	///
	/// \deprecated You should avoid using this method. Use one of the
	///	constructors instead!

	void use(
			Uchar		*ptr,
			Int32		len = -1
	);

	/// Pseudo-copy method.

	/// This method replaces the actual content of this object by the
	/// byte string pointed to by \a ptr. The user should not try to
	/// access the memory area pointed to by \a ptr after
	/// calling this method, nor should he try to deallocate this area
	/// himself, as it will be automatically done by the destructor.
	///
	/// The memory pointed to by \a ptr should be allocated using
	/// the "new" operator: "new Uchar[len]" or "new Char[len]".
	///
	/// \a alloc gives the amount of memory allocated, which can be
	/// different from the actual length of the part used by the user.
	///
	/// \deprecated You should avoid using this method. Use one of the
	///	constructors instead!

	void use(
			Uchar		*ptr,
			Uint32		len,
			Uint32		alloc
	);

	/// Assignment operator.

	/// Creates a `copy-on-write' copy of \a b.

	Buffer& operator = (
		const	Buffer&		b
	);

	/// Concatenation operator.

	/// This operator creates a new Buffer object, which is the
	/// concatenation of this object and \a b.

	Buffer operator + (
		const	Buffer&		b
	) const;

	/// Concatenation operator.

	/// This operator appends the content of \b to this object.

	Buffer& operator += (
		const	Buffer&		b
	);

	/// Logical operator.

	/// This operator returns \b true iff the content of this object
	/// is equal to the content of \a b.

	Bool operator == (
		const	Buffer&		b
	) const;

	/// Logical operator.

	/// This operator returns \b true iff the content of this object
	/// isn't equal to the content of \a b.

	Bool operator != (
		const	Buffer&		b
	) const;

	/// Logical operator.

	/// This operator returns \b true iff the content of this object
	/// is lesser than the content of \a b (left-to-right, ASCII map).

	Bool operator < (
		const	Buffer&		b
	) const;

	/// Logical operator.

	/// This operator returns \b true iff the content of this object
	/// is greater than the content of \a b (left-to-right, ASCII map).

	Bool operator > (
		const	Buffer&	 b
	) const;

	/// Returns true iff this object is empty (no content).

	/// This method returns \b true iff this object is empty.

	Bool isEmpty(
	) const;

	/// Returns the length (in bytes) of this object.

	/// This method returns the length (in bytes) of this object.

	Uint32 length(
	) const;

	/// Consultation method.

	/// This method returns the base address of the memory location
	/// holding the content of this object. Use with caution!
	///
	/// \deprecated The implementation is subject to change without
	/// prior notice, so this method should NOT be relied upon!!!

	const Uchar *getBufferAddr(
	) const;

	/// Returns a reference to the byte at position \a pos.

	/// This method returns a reference to the byte at position \a pos
	/// in the internal buffer, thus allowing one to modify the content
	/// of this object.
	///
	/// If \a pos is outside the length of the internal buffer, a
	/// reference to a dummy char is returned.
	///
	/// Before returning, this method first makes sure the internal
	/// buffer is not shared with another Buffer object (copy-on-write).

	Uchar& operator [] (
		const	Uint32		pos
	);

	/// Returns the byte at position \a pos.

	/// This method returns the byte at position \a pos
	/// in the internal buffer, thus only allowing the consultation of
	/// the content of this object.
	///
	/// If \a pos is outside the length of the internal buffer, a
	/// dummy char is returned.
	///
	/// The copy-on-write mechanism doesn't apply here!

	Uchar operator [] (
		const	Uint32		pos
	) const;

	/// Returns a substring of this object.

	/// This method constructs a new Buffer object, containing only the
	/// part of this object starting at position \a start, and of length
	/// \a length.
	///
	/// If \a start is outside of the internal buffer, an empty object
	/// is returned.
	///
	/// If there are less than \a length bytes to be copied in the
	/// internal buffer, the new object will contain all bytes starting
	/// from position \a start, up to the last byte in the internal
	/// buffer.

	Buffer operator () (
		const	Uint32		start,
		const	Uint32		length
	) const;

	/// Creates a string of space (' ') characters.

	/// This static method constructs a new Buffer object containing
	/// \a length times a space character.

	static Buffer spaces(
		const	Uint32		length
	);

	/// Creates a string of \a car characters.

	/// This static method constructs a new Buffer object containing
	/// \a length times the \a car character.

	static Buffer chars(
		const	Uint32	length,
		const	Uchar	car
	);

	/// ASCII to 32-bits signed integer conversion method.

	/// This method converts a signed integer, given in ASCII,
	/// starting at position \a pos in this object, to its numeric
	/// value. It also increments \a pos with the number of bytes used
	/// by this ASCII representation.
	///
	/// If this method fails, it sets \a pos to 0 (NULL).

	Int32 getIntAt(
		Uint32&		pos
	) const;

	/// ASCII to 32-bits unsigned integer conversion method.

	/// This method converts a unsigned integer, given in ASCII,
	/// starting at position \a pos in this object, to its numeric
	/// value. It also increments \a pos with the number of bytes used
	/// by this ASCII representation.
	///
	/// This method recognizes integers given in decimal notation OR
	/// in hexadecimal notation. In this last case, the integer has to
	/// be supplied with a "0x" or "0X" prefix.
	///
	/// If this method fails, it sets \a pos to 0 (NULL).

	Uint32 getUintAt(
		Uint32&		pos
	) const;

	/// ASCII to 64-bits unsigned integer conversion method.

	Uint64 getUlonglongAt(
		Uint32&		pos
	) const;

	/// ASCII to 16-bits unsigned integer conversion method.

	/// This method converts a unsigned integer, given in ASCII,
	/// starting at position 0 in this object, to its numeric
	/// value.
	///
	/// This method recognizes integers given in decimal notation OR
	/// in hexadecimal notation. In this last case, the integer has to
	/// be supplied with a "0x" or "0X" prefix.
	///
	/// If this method fails, it returns 0.
	///
	/// Example:
	/// \code
	/// cerr << Buffer("0x010000").toUint16() << endl; // prints 0
	/// \endcode

	Uint16 toUint16() const;

	/// ASCII to 32-bits unsigned integer conversion method.

	/// This method converts a unsigned integer, given in ASCII,
	/// starting at position 0 in this object, to its numeric
	/// value.
	///
	/// This method recognizes integers given in decimal notation OR
	/// in hexadecimal notation. In this last case, the integer has to
	/// be supplied with a "0x" or "0X" prefix.
	///
	/// If this method fails, it returns 0.
	///
	/// Example:
	/// \code
	/// cerr << Buffer("0x010000").toUint32() << endl; // prints 65536
	/// \endcode

	Uint32 toUint32() const;

	/// ASCII to 64-bits unsigned integer conversion method.

	Uint64 toUint64() const;

	/// ASCII to 32-bits signed integer conversion method.

	Int32 toInt32() const;

	/// ASCII to double conversion method.

	Double getDoubleAt(
		Uint32&		pos
	) const;

	/// ASCII to double conversion method.

	Double toDouble() const;

	/// ASCII to bool conversion method.

	/// This method converts a boolean, given in ASCII,
	/// starting at position 0 in this object, to its boolean
	/// value.
	///
	/// This method recognizes the 'true' value as one of the
	/// following strings: yes, y, YES, Y, true, TRUE, 1.

	Bool toBool(
	) const;

	/// Extracts a line from a file dump.

	/// This method reads the internal content, starting at position
	/// \a pos, until it finds a NL or a CR-NL, in which case all
	/// the characters read so far are returned in \a line (without
	/// the end-of-line character(s)).

	Bool getLineAt(
			Uint32&		pos,
			Buffer&		line
	) const;

	/// Searches substring \a sub in this object, starting at \a pos.

	/// This method tries to find the first occurrence of substring
	/// \a sub in this object, starting at position \a pos.
	/// If found, it returns `true' and updates \a pos to point to the
	/// beginning of the substring in this object.
	/// Otherwise, it returns `false'.

	Bool findBufferAt(
			Uint32&		pos,
		const	Buffer&		sub
	) const;

	/// Searches character \a c in this object.

	/// This method returns \b true iff it finds an occurrence of
	/// character \a c in the internal buffer.

	Bool contains(
		const	Uchar		c
	) const;

	/// Searches substring \a sub in this object.

	/// This method returns \b true iff it finds an occurrence of
	/// substring \a sub in the internal buffer.

	Bool contains(
		const	Buffer&		subs
	) const;

	/// Searches substring \a ext at beginning of this object.

	/// This method returns \b true iff this object begins with an
	/// occurrence of substring \a ext.

	Bool beginsWith(
		const	Buffer&		ext
	) const;

	/// Searches substring \a ext at end of this object.

	/// This method returns \b true iff this object ends with an
	/// occurrence of substring \a ext.

	Bool endsWith(
		const	Buffer&		ext
	) const;

	/// Finds position of closing bracket, parenthesis, ...

	/// This method tries to find the closing character corresponding
	/// to the character at position \a pos.
	///
	/// The following opening characters are recognized: (, [, and {.
	///
	/// If this method fails, it returns 0. Otherwise, it returns the
	/// position of the closing char.

	Uint32 findClosingChar(
		const	Uint32		pos
	) const;

	/// ...

	Uint32 findFwd(
		const	Uchar		c,
		const	Uint32		pos = 0
	) const;

	/// ...

	Uint32 findRev(
		const	Uchar		c,
		const	Uint32		pos = (Uint32)-1
	) const;

	/// ...

	Buffer toLower(
	) const;

	/// ...

	Buffer toUpper(
	) const;

	/// Dumps content to user-supplied buffer, with terminating NULL.

	/// This method dumps the content of this object to the buffer
	/// of length \a allocated, pointed to by \a ptr, and append a
	/// closing NULL character.
	///
	/// \a allocated HAS TO BE greater than the length of this object,
	/// as given by 'length()'.

	Bool dumpToBuffer(
			Uchar		ptr[],
		const	Uint32		allocated
	) const;

	/// Dumps content to user-supplied buffer, without terminating NULL.

	/// This method dumps the content of this object to the buffer
	/// of length \a allocated, pointed to by \a ptr. No terminating
	/// NULL character is appended!!!
	///
	/// \a allocated HAS TO BE greater or equal to the length of this
	/// object, as given by 'length()'.

	Bool safeDumpToBuffer(
			Uchar		ptr[],
		const	Uint32		allocated
	) const;

	/// Resizes internal buffer.

	/// This method modifies the length of the internal buffer. The
	/// old content is copied to the new location, and padding NULLs
	/// are appended, if needed.

	void resize(
		const	Uint32		newsize
	);

	/// Gives a tip regarding the maximum length of this object.

	/// This method gives a tip to the internal implementation, telling
	/// it how long this object is expected to become in the future.
	/// This  allows the internal implementation to preallocate a
	/// buffer large enough to accomodate \a maxsize bytes at one time.

	void supposedMaxSize(
		const	Uint32		maxsize
	);

	/// Deletes the content of this object.

	/// This method irrevocably deletes the content of this object.
	///
	/// If this content was shared, no actual memory deallocation
	/// occurs.

	void kill(
	);

	/// Ensures the content of this object isn't shared.

	void ensureSingleUser(
	);

protected :

	Uint32 supremum(const	Uint32 i) {
		return ((i & 0xFFFFFF00) + 0x100);
	}

	static Uchar dummy_char;

private :

	BufferData	*ptr;

};

Buffer operator + (const	Char *s1, const	Buffer& s2);

#if 0
std::ostream& operator << (std::ostream&, const	Buffer&);
std::istream& operator >> (std::istream&,        Buffer&);
#endif

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Buffer_H_

// ============================================================================

