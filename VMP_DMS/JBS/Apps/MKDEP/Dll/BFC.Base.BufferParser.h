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
//	BFC.Base.BufferParser.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_BufferParser_H_
#define _BFC_BufferParser_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {

BFC_PTR_DECL(JBS_DLL_NONE,BufferParser)

} // namespace BFC

// ============================================================================

#include "BFC.TL.Array.h"
#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// Provides static methods to parse Buffer objects.

class JBS_DLL_NONE BufferParser : virtual public BFC::SObject {

public :

	BufferParser(
	);

	virtual ~BufferParser(
	);

	// ...

	/// Lightweight version of sscanf().

	/// Splits \a source in multiple tokens according to \a format.
	///
	/// \a format is a sequence of format elements. A format element
	/// is either a literal character ('A', '=', ...) or a 'wildcard'
	/// sequence, which is a 2-characters string, beginning with a '%',
	/// and followed by either a '?' or a 'd'.
	/// The literal '%' itself is represented by the pseudo wildcard
	/// sequence "%%".
	///
	/// This method uses the format elements found in \a format to
	/// parse \a source. Both \a format and \a source are read from
	/// left to right (from element 0 upwards). When a new format
	/// element \b elt is read, this method does the following:
	/// -	if \b elt is some literal character \b c, then
	///	the next character in \a source has to be \b c, which is
	///	then skipped.
	/// -	if \b elt is the wildcard sequence "%?", then the next
	///	character in \a source can be any character, which is also
	///	skipped.
	/// -	if \b elt is the wildcard sequence "%d",
	///	\a source has to contain 1 or more ASCII digits (characters
	///	between '0' and '9'), which are put in a new Buffer object
	///	that is appended to the array of Buffer objects this method
	///	returns.
	///
	/// \throw InvalidFormat
	/// -	Invalid second character in a wildcard sequence.
	/// \throw Overflow
	/// -	\a format is too short to describe all elements of \a
	///	source.
	/// \throw Underflow
	/// -	\a format is too long to describe all elements of \a
	///	source.
	/// -	\a format ends with a single '%'.
	/// \throw WrongMatch
	/// -	A literal character in format has not been found in \a
	///	source.
	/// -	No digit to match a "%d" wildcard sequence.
	///
	/// Example:
	/// \code
	/// using namespace BFC;
	/// ...
	/// Buffer source = "truc[%512] = 1024";
	/// Buffer format = "truc[%%%d] = %d";
	/// BufferArray tokens = BufferParser::scan(source, format);
	/// // tokens[0] == "512"
	/// // tokens[1] == "1024"
	/// \endcode

	static BufferArray scan(
		const	Buffer&		source,
		const	Buffer&		format
	);

	/// Tokenize \a source using token separators in \a separators.

	/// This method considers \a source as a sequence of tokens
	/// separated by token separators (given in \a separators), and
	/// returns an Array of Buffer objects representing those tokens.
	///
	/// If a '"' character is found, it is considered as the opening
	/// character of a string, and the closing '"' is then searched
	/// for, blindly skipping all characters, even those that would
	/// otherwise be separators. Such a string is appended to the
	/// result Array without the '"' characters.
	///
	/// \note To insert a '"' into a string surrounded by '"', escape
	///	it with a '\\'.
	///
	/// \throw InvalidArgument
	/// -	\a separators is empty.
	/// -	unmatched opening '"'.
	///
	/// Example:
	/// \code
	/// using namespace BFC;
	/// using namespace BFC::IO;
	/// ...
	/// File file;
	/// file.open("somefile.txt", File::Read);
	/// Buffer seps = "\n\r";
	/// BufferArray lines = BufferParser::tokenize(
	///		file.dumpToBuffer(), seps);
	///
	/// // Now lines contains all single lines of file, without
	/// // the trailing NL (Unix convention) or CR-NL (DOS convention).
	/// \endcode

	static BufferArray tokenize(
		const	Buffer&		source,
		const	Buffer&		separators
	);

	/// Tokenize \a source using token separators in \a separators.

	/// This method differs only from the above one by the parameters
	/// it takes.
	///
	/// Example: executing the following application:
	/// \code
	/// #include "BFC.Base.BufferIO.h"
	/// #include "BFC.Base.BufferParser.h"
	/// 
	/// using namespace std;
	/// using namespace BFC;
	/// 
	/// int main() {
	/// 	Buffer line = "\"First \\\"Var\\\"\"   =    file.txt";
	/// 	BufferArray tokens = BufferParser::tokenize(line, " \t");
	/// 	for (Uint32 i = 0 ; i < tokens.size() ; i++)
	/// 		cerr << "[" << tokens[i] << "]" << endl;
	/// 	return 0;
	/// }
	/// \endcode
	/// would print the following on the console:
	/// \code
	/// [First "Var"]
	/// [=]
	/// [file.txt]
	/// \endcode

	static BufferArray tokenize(
		const	Buffer&		source,
		const	Char		*separators = " \t"
	) {
		return tokenize(source, Buffer(separators));
	}

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("BFC.Base.BufferParser");

	CLASS_EXCEPTION(InvalidArgument, "Invalid argument");
	CLASS_EXCEPTION(InvalidFormat, "Invalid format");
	CLASS_EXCEPTION(Overflow, "Overflow");
	CLASS_EXCEPTION(Underflow, "Underflow");
	CLASS_EXCEPTION(WrongMatch, "Wrong match");

protected :

private :

	// others...

	BufferParser(
		const	BufferParser&
	);

	BufferParser& operator = (
		const	BufferParser&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_BufferParser_H_

// ============================================================================

