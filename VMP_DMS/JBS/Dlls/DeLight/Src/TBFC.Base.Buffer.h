// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "TBFC::Light".
//
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	TBFC.Base.Buffer.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _TBFC_Base_Buffer_H_
#define _TBFC_Base_Buffer_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Mem.DataSharer.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A simple buffer class (a.k.a. a string).
///
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
/// range' conditions. Note that in the case such a condition arise, a C++
/// exception is thrown.
///
/// See the documentation below for more information.
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL Buffer : public Mem::DataSharer {

public :

	/// \brief Creates a new empty Buffer.

	Buffer(
	) {
	}

	/// \brief Creates a new Buffer and attaches it to the C-style
	///	NULL-terminated string pointed to by \a __ptr.
	///
	/// If \a __ptr is \b NULL, an empty Buffer is created.
	///
	/// If \a __copy is false (default), the new Buffer is directly
	/// attached to the NULL-terminated string. Otherwise, it is
	/// attached to a local copy of it.
	///
	/// If no copy is made,
	/// it is of uttermost importance that the memory zone pointed to
	/// by \a __ptr remains readable as long as this Buffer, or any
	/// copy of this Buffer, exists. If this condition could not be met for
	/// some obscure reason, you HAVE TO use a copy instead!
	///
	/// Example 1: attaching a Buffer to a static string (no copy needed).
	///
	/// \code
	///	// ...
	///
	///	Buffer buf( "Hello, world!" );
	///	// Using 'buf' is safe! And no copy was made!
	///
	///	// ...
	/// \endcode
	///
	/// Example 2: attaching a Buffer to a static string, and copying it
	///	immediatly after.
	///
	/// \code
	///	// ...
	///
	///	Buffer myfunc() {
	///		const Char str[] = "Hello, world!";
	///		Buffer buf( str, true ); // Copy NEEDED!!! 'str' will disappear soon!
	///		return buf;
	///	}
	///
	///	// ...
	/// \endcode

	Buffer(
		const	Char		* __ptr,
		const	Bool		__copy = false
	);

	/// \brief Attaches a new Buffer to the externally allocated
	///	string pointed to by \a __ptr, and whose length is \a __len.
	///	If \a __copy is true, then a copy is used instead.

	Buffer(
		const	Uchar		* __ptr,
		const	Uint32		__len,
		const	Bool		__copy = true
	);

	/// \brief Creates a 1-byte long buffer whose content is \a __char.

	Buffer(
		const	Char		__char
	);

	/// \brief Creates a 1-byte long buffer whose content is \a __char.

	Buffer(
		const	Uchar		__char
	);

	/// \brief Creates an ASCII representation of \a __sint.

	Buffer(
		const	Int32		__sint
	);

	/// \brief Enumerated list of numerical bases.

	enum Base {
		Base2	= 2,	///< Binary base (0..1)
		Base8	= 8,	///< Octal base (0..7)
		Base10	= 10,	///< Decimal base (0..9)
		Base16	= 16	///< Hexadecimal base (0..9, A..F)
	};

	/// \brief Creates an ASCII representation of \a __uint.
	///
	/// This constructor creates an ASCII representation
	/// of the 32-bits unsigned value \a __uint, in the base specified
	/// by \a __base (default: base 10), with an optional minimum length
	/// specified by \a __size, and using
	/// the filling character specified by \a __fill (default: '0')
	/// if the minimum length \a __size is greater
	/// than what is needed to completely represent \a __uint.
	///
	/// \param __uint
	///	The unsigned integer to represent as an ASCII string.
	///
	/// \param __base
	///	The numerical base to use (default: decimal base).
	///
	/// \param __size
	///	The minimum ASCII string length, including any base
	///	specific prefix ("0x" for hexadecimal base, ...) (default:
	///	no minimum length).
	///
	/// \param __fill
	///	A filling character to use if needed. If this character has to
	///	be used to fill a too short string, it is used at the right of
	///	the numerical base prefix (if any) if its value is '0', and at
	///	the left if its value is not '0'.
	///
	/// Examples:
	/// - Buffer( 1024 ) gives "1024"
	/// - Buffer( 1024, Buffer::Base2 , 8 ) gives "0b10000000000"
	/// - Buffer( 1024, Buffer::Base8 ) gives "0o2000"
	/// - Buffer( 1024, Buffer::Base8 , 8 ) gives "0o002000"
	/// - Buffer( 1024, Buffer::Base10, 8 ) gives "00001024"
	/// - Buffer( 1024, Buffer::Base10, 8, ' ' ) gives "    1024"
	/// - Buffer( 1024, Buffer::Base16 ) gives "0x400"
	/// - Buffer( 1024, Buffer::Base16, 8, ' ' ) gives "   0x400"
	/// - Buffer( 1024, Buffer::Base16, 8, '.' ) gives "...0x400"
	/// - Buffer( 1024, Buffer::Base16, 8 ) gives "0x000400"

	Buffer(
		const	Uint32		__uint,
		const	Base		__base	= Base10,
		const	Uint32		__size	= 1,
		const	Uchar		__fill	= '0'
	);

	/// \brief Creates an ASCII representation of \a __sint64.

	Buffer(
		const	Int64		__sint64
	);

	/// \brief Creates an ASCII representation of \a __uint64.
	///
	/// \param __uint64
	///	The unsigned integer to represent as an ASCII string.
	///
	/// \param __base
	///	The numerical base to use (default: decimal base).
	///
	/// \param __size
	///	The minimum ASCII string length, including any base
	///	specific prefix ("0x" for hexadecimal base, ...) (default:
	///	no minimum length).
	///
	/// \param __fill
	///	A filling character to use if needed. If this character has to
	///	be used to fill a too short string, it is used at the right of
	///	the numerical base prefix (if any) if its value is '0', and at
	///	the left if its value is not '0'.

	Buffer(
		const	Uint64		__uint64,
		const	Base		__base	= Base10,
		const	Uint32		__size	= 1,
		const	Uchar		__fill	= '0'
	);

	/// \brief Creates an ASCII representation of \a __double.
	///
	/// This constructor creates an ASCII representation
	/// of the double value \a __double, using a C-like format string
	/// \a __format.

	Buffer(
		const	Double		__double,
		const	Char		* __format = 0
	);

//	/// \brief Creates a Buffer, sharing a copy-on-write memory block
//	///	with \a __block.
//
//	Buffer(
//		const	Mem::Block	& __block
//	);

	/// \brief Creates a Buffer, associated with memory block \a __data.
	///
	/// This Buffer takes ownership of \a __data, so don't touch this
	/// after having called this constructor.

	Buffer(
			Mem::Data	* __data
	) :
		Mem::DataSharer( __data ) {
	}

	Buffer(
		const	Mem::DataSharer	& __other
	) :
		Mem::DataSharer( __other ) {
	}

	/// \brief Copy constructor.
	///
	/// This constructor creates a `copy-on-write' copy of the source
	/// object \a __other.

	Buffer(
		const	Buffer		& __other
	) :
		Mem::DataSharer( __other ) {
	}

	/// \brief Pseudo-copy constructor.
	///
	/// This constructor creates a `copy-on-write' copy of the substring
	/// of \a __other, starting at position \a __beg, up to the last
	/// character.

	Buffer(
		const	Buffer		& __other,
		const	Uint32		__beg
	) :
		Mem::DataSharer( __other, __beg ) {
	}

	/// \brief Pseudo-copy constructor.
	///
	/// This constructor creates a `copy-on-write' copy of the substring
	/// of \a __other, starting at position \a __beg, and of maximum
	/// length \a __len.

	Buffer(
		const	Buffer		& __other,
		const	Uint32		__beg,
		const	Uint32		__len
	) :
		Mem::DataSharer( __other, __beg, __len ) {
	}

//	/// \brief Destructor.
//	///
//	/// This destructor deallocates any resource used by this object,
//	/// if not shared with other Buffer objects.
//
//	/* virtual */ ~Buffer(
//	) {
//	}

	/// \brief Synonym of "deleteData()".

	void kill(
	) {
		deleteData();
	}

	/// \brief Pseudo-copy method.
	///
	/// This method replaces the actual content of this object by the
	/// byte string pointed to by \a ptr. The user should not try to
	/// access the memory area pointed to by \a ptr after
	/// calling this method, nor should he try to deallocate this area
	/// himself, as it will be automatically done by the destructor.
	///
	/// The memory pointed to by \a ptr should be allocated using
	/// the "new" operator: "new Uchar[len]" or "new Char[len]".
	///
	/// \deprecated You should avoid using this method. Use one of the
	///	constructors above, in conjunction with the classes of the
	///	new \ref TBFC_Mem, as in the example below.
	///
	/// \code
	///	// Create a buffer on the heap...
	///	Uint32    len = 1024;
	///	Uchar     * ptr = new Uchar[ len ];
	///	// Let a Mem::Data object adopt it...
	///	Mem::data * mem = new Mem::Data( ptr, len, Mem::StdCppAllocator::instance() );
	///	// Let a Buffer adopt the Mem::Data object...
	///	Buffer    buf( mem );
	///	// Now, the block will be automatically deallocated!
	/// \endcode

	void use(
			Uchar		* ptr,
		const	Uint32		len
	);

	/// \brief Creates a `copy-on-write' copy of \a __other.

	Buffer & operator = (
		const	Buffer		& __other
	);

	/// \brief Returns the concatenation of this object and \a __other.

	Buffer operator + (
		const	Buffer		& __other
	) const;

	/// \brief Appends \a __other at the end of this object.

	void operator += (
		const	Buffer		& __other
	);

	/// \brief "Equal-to" relational operator.
	///
	/// This operator returns \b true iff the content of this object
	/// is equal to the content of \a b.

	Bool operator == (
		const	Buffer		& b
	) const;

	/// \brief "Not-equal-to" relational operator.
	///
	/// This operator returns \b true iff the content of this object
	/// isn't equal to the content of \a b.

	Bool operator != (
		const	Buffer		& b
	) const;

	/// \brief "Less-than" relational operator.
	///
	/// This operator returns \b true iff the content of this object
	/// is less than the content of \a b.

	Bool operator < (
		const	Buffer		& b
	) const;

	/// \brief "Greater-than" relational operator.
	///
	/// This operator returns \b true iff the content of this object
	/// is greater than the content of \a b.

	Bool operator > (
		const	Buffer		& b
	) const;

	/// \brief "Less-than-or-equal-to" relational operator.
	///
	/// This operator returns \b true iff the content of this object
	/// is less than or equal to the content of \a b.

	Bool operator <= (
		const	Buffer		& b
	) const;

	/// \brief "Greater-than-or-equal-to" relational operator.
	///
	/// This operator returns \b true iff the content of this object
	/// is greater than or equal to the content of \a b.

	Bool operator >= (
		const	Buffer		& b
	) const;

	/// \brief Returns true iff this object is empty (no content).

	Bool isEmpty(
	) const {
		return ( getLength() == 0 );
	}

	/// \brief Returns true iff this object has some content.

	Bool hasContent(
	) const {
		return ( getLength() != 0 );
	}

	/// \brief Returns the number of bytes stored in this object.

	Uint32 getLength(
	) const {
		return getLogLength();
	}

	/// \brief Synonym of getLength().

	Uint32 length(
	) const {
		return getLogLength();
	}

	/// \brief Returns a real-only pointer to the first byte (pos. 0) in this
	///	object, or 0 if this object is empty.

	const Uchar * getCstPtr(
	) const {
		return getLogCstPtr();
	}

	/// \brief Returns a read-only pointer to the byte at position
	///	\a __offset in this object, or 0 if this object is empty.

	const Uchar * getCstPtr(
		const	Uint32		__offset
	) const {
		return getLogCstPtr( __offset );
	}

	/// \brief Synonym of getCstPtr().

	const Uchar * getBufferAddr(
	) const {
		return getLogCstPtr();
	}

	/// \brief Returns a pointer to the internal buffer, after having
	///	checked that this internal buffer is terminated by a NULL
	///	character.

	const Uchar * getCStringPtr(
	);

	/// \brief Synonym of getCStringPtr().

	const Uchar * getCStringAddress(
	) {
		return getCStringPtr();
	}

	/// \brief Returns a writeable pointer to the first byte (pos. 0) in this
	///	object, or 0 if this object is empty.

	Uchar * getVarPtr(
	) {
		return getLogVarPtr();
	}

	/// \brief Returns a writeable pointer to the byte at position
	///	\a __offset in this object, or 0 if this object is empty.

	Uchar * getVarPtr(
		const	Uint32		__offset
	) {
		return getLogVarPtr( __offset );
	}

	void ensureSingleUser(
	) {
		( void )getLogVarPtr();
	}

	/// \brief Returns a reference to the byte at position \a pos.
	///
	/// This method returns a reference to the byte at position \a pos
	/// in the internal buffer, thus allowing one to modify the content
	/// of this object.
	///
	/// If \a pos is outside the length of the internal buffer, a
	/// reference to a dummy char is returned.
	///
	/// Before returning, this method first makes sure the internal
	/// buffer is not shared with another Buffer object (copy-on-write).

	Uchar & operator [] (
		const	Uint32		pos
	) {
		return * getVarPtr( pos );
	}

	/// \brief Returns the byte at position \a pos.
	///
	/// This method returns the byte at position \a pos
	/// in the internal buffer, thus only allowing the consultation of
	/// the content of this object.
	///
	/// If \a pos is outside the length of the internal buffer, an
	/// exception is thrown.

	Uchar operator [] (
		const	Uint32		pos
	) const {
		return * getCstPtr( pos );
	}

	/// \brief Returns a substring of this object.
	///
	/// This method constructs a new Buffer object, containing only the
	/// right part of this object starting at position \a start.
	///
	/// If \a start is outside of the internal buffer, an empty object
	/// is returned.
	///
	/// If 'b' is a Buffer, calling 'b( N )' is equivalent to calling
	/// 'b( N, b.length() - N )'.

	Buffer operator () (
		const	Uint32		start
	) const;

	/// \brief Returns a substring of this object.
	///
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

	/// \brief Returns a copy of the string, with leading and trailing
	///	whitespace omitted.
	///
	/// If this object represents an empty character sequence, or the first
	/// and last characters of character sequence represented by this
	/// object both have codes greater than 0x20 (the space character),
	/// then a copy of this object is returned.
	///
	/// Otherwise, if there is no character with a code greater than 0x20
	/// in the string, then a new object representing an empty string is
	/// created and returned.
	///
	/// Otherwise, let k be the index of the first character in the string
	/// whose code is greater than 0x20, and let m be the index of the last
	/// character in the string whose code is greater than 0x20. A new
	/// object is created, representing the substring of this string that
	/// begins with the character at index k and ends with the character at
	/// index m - that is.
	///
	/// This method may be used to trim whitespace from the beginning and
	/// end of a string; in fact, it trims all ASCII control characters as
	/// well.
	///
	/// \returns
	///	A copy of this string with leading and trailing white space
	///	removed, or this string if it has no leading or trailing white
	///	space.

	Buffer trim(
	) const;

	/// \brief
	///	Replaces all occurrences
	///	of \a oldChar in this string with \a newChar.
	///
	/// If the character \a oldChar does not occur in the character
	/// sequence represented by this object, then nothing happens.
	///
	/// Otherwise, every occurrence of \a oldChar is
	/// replaced by an occurrence of \a newChar.
	///
	/// \param oldChar
	///	the old character.
	///
	/// \param newChar
	///	the new character.

	void replace(
		const	Uchar		oldChar,
		const	Uchar		newChar
	);

	/// \brief Replaces \a __len characters from position \a __pos with
	///	\a __sub.
	/// 
	/// Example:
	/// \code
	///	Buffer x = "Say yes!";
	///	Buffer y = "no";
	///	x.replace(4, 3, y);
	///	// x == "Say no!"
	/// \endcode

	void replace(
		const	Uint32		__pos,
		const	Uint32		__len,
		const	Buffer		& __sub
	);

	/// \brief Returns the last character of this object.

	Uchar lastChar(
	) const;

	/// \brief Creates a string of space (' ') characters.
	///
	/// This static method constructs a new Buffer object containing
	/// \a length times a space character.

	static Buffer spaces(
		const	Uint32		length
	);

	/// \brief Creates a string of \a __size '\a __char' characters.
	///
	/// This static method constructs a new Buffer object containing
	/// \a __size times the \a __char character.

	static Buffer chars(
		const	Uint32		__size,
		const	Uchar		__char
	);

	/// \brief ASCII to 32-bits signed integer conversion method.
	///
	/// This method converts a signed integer, given in ASCII,
	/// starting at position \a pos in this object, to its numeric
	/// value. It also increments \a pos with the number of bytes used
	/// by this ASCII representation.
	///
	/// If this method fails, it sets \a pos to 0 (NULL).

	Int32 getIntAt(
			Uint32		& pos
	) const;

	/// \brief ASCII to 32-bits unsigned integer conversion method.
	///
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
			Uint32		& pos
	) const;

	/// \brief ASCII to 64-bits unsigned integer conversion method.

	Uint64 getUlonglongAt(
			Uint32		& pos
	) const;

	/// \brief ASCII to double conversion method.

	Double getDoubleAt(
			Uint32		& pos
	) const;

	/// \brief ASCII to 16-bits unsigned integer conversion method.
	///
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

	Uint16 toUint16(
	) const;

	/// \brief ASCII to 32-bits unsigned integer conversion method.
	///
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

	Uint32 toUint32(
	) const;

	/// \brief ASCII to 64-bits unsigned integer conversion method.

	Uint64 toUint64(
	) const;

	/// \brief ASCII to 32-bits signed integer conversion method.

	Int32 toInt32(
	) const;

	/// \brief ASCII to double conversion method.

	Double toDouble(
	) const;

	/// \brief ASCII to bool conversion method.
	///
	/// This method converts a boolean, given in ASCII,
	/// starting at position 0 in this object, to its boolean
	/// value.
	///
	/// This method recognizes the 'true' value as one of the
	/// following strings: yes, y, YES, Y, true, TRUE, 1.

	Bool toBool(
	) const;

	/// \brief Extracts a line from a file dump.
	///
	/// This method reads the internal content, starting at position
	/// \a pos, until it finds a NL or a CR-NL, in which case all
	/// the characters read so far are returned in \a line (without
	/// the end-of-line character(s)).

	Bool getLineAt(
			Uint32&		pos,
			Buffer&		line
	) const;

	/// \brief Searches substring \a sub in this object, starting at \a pos.
	///
	/// This method tries to find the first occurrence of substring
	/// \a sub in this object, starting at position \a pos.
	/// If found, it returns `true' and updates \a pos to point to the
	/// beginning of the substring in this object.
	/// Otherwise, it returns `false'.

	Bool findBufferAt(
			Uint32		& pos,
		const	Buffer		& sub
	) const;

	/// \brief Replaces \a __toFind by \a __toCopy, starting at position
	///	\a __startPos, and stopping after the first replacement if
	///	\a __global is false.
	///
	/// \returns
	///	The number of replacements.

	Uint32 replaceBuffer(
		const	Buffer		& __toFind,
		const	Buffer		& __toCopy,
		const	Uint32		__startPos,
		const	Bool		__global
	);

	/// \brief Searches character \a c in this object.
	///
	/// This method returns \b true iff it finds an occurrence of
	/// character \a c in the internal buffer.

	Bool contains(
		const	Uchar		c
	) const;

	/// \brief Searches substring \a sub in this object.
	///
	/// This method returns \b true iff it finds an occurrence of
	/// substring \a sub in the internal buffer.

	Bool contains(
		const	Buffer		& subs
	) const;

	/// \brief Searches substring \a ext at beginning of this object.
	///
	/// This method returns \b true iff this object begins with an
	/// occurrence of substring \a ext.

	Bool beginsWith(
		const	Buffer		& ext
	) const;

	/// \brief Searches substring \a ext at end of this object.
	///
	/// This method returns \b true iff this object ends with an
	/// occurrence of substring \a ext.

	Bool endsWith(
		const	Buffer		& ext
	) const;

	/// \brief Finds position of closing bracket, parenthesis, ...
	///
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

	/// \brief Finds the position of the first occurrence of \a __char,
	///	starting at index \a __pos, and progressing to the right
	///	(increasing index).
	///
	/// If no occurrence is found, the length of this object is returned.

	Uint32 findFwd(
		const	Uchar		__char,
		const	Uint32		__pos = 0
	) const;

	/// \brief Finds the position of the first occurrence of any character
	///	in \a __chars,
	///	starting at index \a __pos, and progressing to the right
	///	(increasing index).
	///
	/// If no occurrence is found, the length of this object is returned.

	Uint32 findAnyFwd(
		const	Buffer		& __chars,
		const	Uint32		__pos = 0
	) const;

	/// \brief Searches substring \a __sub in this object, starting at
	///	position \a __pos.
	///
	/// \param __sub
	///	Substring to look for.
	///
	/// \param __pos
	///	Position where to start searching (forward).
	///
	/// \returns
	///	The position of __sub if found, or the length of this object
	///	if __sub was not found.

	Uint32 findFwd(
		const	Buffer		& __sub,
		const	Uint32		__pos = 0
	) const;

	/// \brief Finds the position of the first occurrence of \a __char,
	///	starting at index \a __pos, and progressing to the left
	///	(decreasing index).
	///
	/// If no occurrence is found, -1 is returned.

	Uint32 findRev(
		const	Uchar		__char,
		const	Uint32		__pos = ( Uint32 )-1
	) const;

	/// \brief Finds the position of the first occurrence of any character
	///	in \a __chars,
	///	starting at index \a __pos, and progressing to the left
	///	(decreasing index).
	///
	/// If no occurrence is found, -1 is returned.

	Uint32 findAnyRev(
		const	Buffer		& __chars,
		const	Uint32		__pos = ( Uint32 )-1
	) const;

	/// \brief ...

	Buffer toLower(
	) const;

	/// \brief ...

	Buffer toUpper(
	) const;

	/// \brief Dumps content to user-supplied buffer, with terminating NULL.
	///
	/// This method dumps the content of this object to the buffer
	/// of length \a allocated, pointed to by \a ptr, and append a
	/// closing NULL character.
	///
	/// \a allocated HAS TO BE greater than the length of this object,
	/// as given by 'length()'.

	void dumpToBuffer(
			Uchar		ptr[],
		const	Uint32		allocated
	) const;

	/// \brief Dumps content to user-supplied buffer, without terminating NULL.
	///
	/// This method dumps the content of this object to the buffer
	/// of length \a allocated, pointed to by \a ptr. No terminating
	/// NULL character is appended!!!
	///
	/// \a allocated HAS TO BE greater or equal to the length of this
	/// object, as given by 'length()'.

	void safeDumpToBuffer(
			Uchar		ptr[],
		const	Uint32		allocated
	) const;

	/// \brief Resizes internal buffer.
	///
	/// This method modifies the length of the internal buffer. The
	/// old content is copied to the new location.

	void resize(
		const	Uint32		newsize
	);

	/// \brief Removes the last char from this object, if any.

	void dropLastChar(
	);

	/// \brief Removes \a __len chars, starting at \a __pos.

	void pop(
		const	Uint32		__pos,
		const	Uint32		__len
	);

	/// \brief Synonym of pop().

	void remove(
		const	Uint32		__pos,
		const	Uint32		__len
	) {
		pop( __pos, __len );
	}

	/// \brief Removes every occurrence of \a __char from this object.

	void remove(
		const	Uchar		__char
	);

	/// \brief Removes every occurrence of \a __sub from this object.

	void remove(
		const	Buffer		& __sub
	);

	/// \brief Inserts substring \a __sub at position \a __pos.

	void insert(
		const	Uint32		__pos,
		const	Buffer		& __sub
	);

	/// \brief Gives a tip regarding the maximum length of this object.
	///
	/// This method gives a tip to the internal implementation, telling
	/// it how long this object is expected to become in the future.
	/// This  allows the internal implementation to preallocate a
	/// buffer large enough to accomodate \a maxsize bytes at one time.

	void supposedMaxSize(
		const	Uint32		maxsize
	);

	/// \brief Transfers ownership of the internal data to some
	///	external entity.

	Mem::AllocatorPtr releaseData(
			Uchar * &	__phyVarPtr,
			Uint32 &	__phyLength,
		const	Bool		__retrievable
	);

protected :

	Int64 convertToInt(
			Uint32		& pos
	) const;

	Double convertToDouble(
			Uint32		& pos
	) const;

private :

//	mutable FastCS	crt;		///< Internal state protector.

};

// ============================================================================

/// \brief Concatenation operator.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Buffer operator + (
	const	Char		* s1,
	const	Buffer		& s2
);

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_Buffer_H_

// ============================================================================

