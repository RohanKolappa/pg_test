// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::RegEx".
// 
// "BFC::RegEx" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::RegEx" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::RegEx"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.RegEx.SimpleRegEx.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_RegEx_H_
#define _BFC_RegEx_H_

// ============================================================================

#include "BFC.RegEx.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace RegEx {

BFC_PTR_DECL( BFC_RegEx_DLL, SimpleRegEx )

} // namespace RegEx
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace RegEx {

// ============================================================================

/*
 * Regular Expressions:
 *
 *      [1]     char    matches itself, unless it is a special
 *                      character (metachar): . \ [ ] * + ^ $
 *                      and ( ) if posix option.
 *
 *      [2]     .       matches any character.
 *
 *      [3]     \       matches the character following it, except:
 *                      - \a, \b, \f, \n, \t, \v match the
 *                      corresponding C escape char;
 *                      - if not in posix mode, when followed by a
 *                      left or right round bracket (see [7]);
 *                      - when followed by a digit 1 to 9 (see [8]);
 *                      - when followed by a left or right angle bracket
 *                      (see [9]).
 *                      It is used as an escape character for all
 *                      other meta-characters, and itself. When used
 *                      in a set ([4]), it is treated as an ordinary
 *                      character (except for escape chars).
 *
 *      [4]     [set]   matches one of the characters in the set.
 *                      If the first character in the set is "^",
 *                      it matches a character NOT in the set, i.e.
 *                      complements the set. A shorthand S-E (start-end)
 *                      is used to specify a set of characters S upto
 *                      E, inclusive. The special characters "]" and
 *                      "-" have no special meaning if they appear
 *                      as the first chars in the set. To include both,
 *                      put - first: [-]A-Z]:
 *                      [-]|] matches these 2 chars,
 *                      []-|] matches from ] to | chars.
 *                      examples:        match:
 *
 *                              [a-z]    any lowercase alpha
 *
 *                              [^-]]    any char except - and ]
 *
 *                              [^A-Z]   any char except uppercase
 *                                       alpha
 *
 *                              [a-zA-Z] any alpha
 *
 *      [5]     *       any regular expression form [1] to [4], followed by
 *                      closure char (*) matches zero or more matches of
 *                      that form.
 *
 *      [6]     +       same as [5], except it matches one or more.
 *
 *      [7]             a regular expression in the form [1] to [10], enclosed
 *                      as \(form\) (or (form) with posix flag) matches what
 *                      form matches. The enclosure creates a set of tags,
 *                      used for [8] and for pattern substitution.
 *                      The tagged forms are numbered starting from 1.
 *
 *      [8]             a \ followed by a digit 1 to 9 matches whatever a
 *                      previously tagged regular expression ([7]) matched.
 *
 *      [9]     \<      a regular expression starting with a \< construct
 *              \>      and/or ending with a \> construct, restricts the
 *                      pattern matching to the beginning of a word, and/or
 *                      the end of a word. A word is defined to be a character
 *                      string beginning and/or ending with the characters
 *                      A-Z a-z 0-9 and _. It must also be preceded and/or
 *                      followed by any character outside those mentioned.
 *
 *      [10]            a composite regular expression xy where x and y
 *                      are in the form [1] to [10] matches the longest
 *                      match of x followed by a match for y.
 *
 *      [11]    ^       a regular expression starting with a ^ character
 *              $       and/or ending with a $ character, restricts the
 *                      pattern matching to the beginning of the line,
 *                      or the end of line. [anchors] Elsewhere in the
 *                      pattern, ^ and $ are treated as ordinary characters.
 *
 * Examples:
 *
 *  pattern:    foo*.*
 *  compile:    CHR f CHR o CLO CHR o END CLO ANY END END
 *  matches:    fo foo fooo foobar fobar foxx ...
 *
 *  pattern:    fo[ob]a[rz]
 *  compile:    CHR f CHR o CCL bitset CHR a CCL bitset END
 *  matches:    fobar fooar fobaz fooaz
 *
 *  pattern:    foo\\+
 *  compile:    CHR f CHR o CHR o CHR \ CLO CHR \ END END
 *  matches:    foo\ foo\\ foo\\\  ...
 *
 *  pattern:    \(foo\)[1-3]\1  (same as foo[1-3]foo)
 *  compile:    BOT 1 CHR f CHR o CHR o EOT 1 CCL bitset REF 1 END
 *  matches:    foo1foo foo2foo foo3foo
 *
 *  pattern:    \(fo.*\)-\1
 *  compile:    BOT 1 CHR f CHR o CLO ANY END EOT 1 CHR - REF 1 END
 *  matches:    foo-foo fo-fo fob-fob foobar-foobar ...
 */

// ============================================================================

/// \brief Regular expressions compiler.
///
/// \ingroup BFC_RegEx

class BFC_RegEx_DLL SimpleRegEx : virtual public SObject {

public:

	SimpleRegEx(
	);

	virtual ~SimpleRegEx(
	);

	/// \brief Compiles a regular expression into a NFA (Non-Deterministic
	///	Finite-State Automata).
	///
	/// A non-deterministic finite-state automaton (NFA) consists of:
	/// -	An alphabet \f$\Sigma\f$ of input symbols
	/// -	A set of states, of which we distinguish:
	///	- a unique start state, usually numbered "0"
	///	- a set of final states
	/// -	A transition relation which, for any given state and symbol
	/// gives the (possibly empty) set of next states.
	///
	/// We can represent a NFA diagrammatically using a (labelled,
	/// directed) graph, where states are represented by nodes (circles)
	/// and transitions are represented by edges (arrows).
	///
	/// The purpose of an NFA is to model the process of reading in
	/// characters until we have formed one of the words that we are
	/// looking for.
	///
	/// How an NFA operates:
	/// -	We begin in the start state (usually labelled "0") and read the
	///	first character on the input.
	/// -	Each time we are in a state, reading a character from the input,
	///	we examine the outgoing transitions for this state, and look
	///	for one labelled with the current character. We then use this
	///	to move to a new state.
	///	-	There may be more than one possible transition, in
	///		which case we choose one at random.
	///	-	If at any stage there is an output transition labelled
	///		with the empty string, \f$\varepsilon\f$, we may take
	///		it without consuming any input.
	/// -	We keep going like this until we have no more input, or until
	///	we have reached one of the final states.
	/// -	If we are in a final state, with no input left, then we have
	///	succeeded in recognising a pattern.
	/// -	Otherwise we must backtrack to the last state in which we had
	///	to choose between two or more transitions, and try selecting a
	///	different one.
	///
	/// Basically, in order to match a pattern, we are trying to find a
	/// sequence of transitions that will take us from the start state to
	/// one of the finish states, consuming all of the input.

	void compile(
		const	Buffer		& pattern,
			Bool		caseSensitive = true
	);

	/// \brief Executes the NFA to match a pattern.
	///
	/// \returns
	///	True iff the \a __test string matches this regular
	///	expression.

	Bool execute(
		const	Buffer		& __test
	);

	const BufferArray & grabMatches(
	);

	/// \brief Substitutes the matched portions of \a src.
	///
	/// \param src
	///	A pattern containing any combination of characters and
	///	'\\digit' sequences, which substitute the subpattern with
	///	the given tag number (1 to 9: submatches, 0: entire match).
	///	The '\\' character has to be escaped!

	Buffer substitute(
		const	Buffer		& src
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.RegEx.SimpleRegEx" );

	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );
	CLASS_EXCEPTION( NotValid, "Not valid" );

protected :

	int PMatch(
		const	Buffer		& ci,
			int		minPos,
			int		maxPos,
		const	Uchar		* nfaPtr
	);

	void setMatch(
		const	Uint32		index,
		const	Buffer		& pattern
	);

	// Escape sequences...

	static Uchar escapeValue(
		const	Uchar		ch
	) {
		return ( escTable[ ch ] );
	}

	// Bit sets...

	static Uchar bytPos(
		const	Uint32		c
	) {
		return ( Uchar )( ( c & 0xFF ) >> 3 );
	}

	static Uchar bitPos(
		const	Uint32		c
	) {
		return ( Uchar )( 1 << ( c & 0x07 ) );
	}

	static void clrSet(
			Uchar		* setPtr
	);

	static void addSet(
			Uchar		* setPtr,
		const	Uint32		c
	) {
		Uchar	p = bytPos( c );
		setPtr[ p ] = ( Uchar )( setPtr[ p ] | bitPos( c ) );
	}

	static void addSet(
			Uchar		* setPtr,
		const	Uint32		c,
		const	Bool		caseSensitive
	) {
		if ( caseSensitive ) {
			addSet( setPtr, c );
		}
		else if ( c >= 'a' && c <= 'z' ) {
			addSet( setPtr, c );
			addSet( setPtr, c - 'a' + 'A' );
		}
		else if ( c >= 'A' && c <= 'Z' ) {
			addSet( setPtr, c );
			addSet( setPtr, c - 'A' + 'a' );
		}
		else {
			addSet( setPtr, c );
		}
	}

	static void invSet(
			Uchar		* setPtr
	) {
		Uint32 * p = ( Uint32 * )setPtr;
		for ( Uint32 i = 0 ; i < 8 ; i++ ) {
			p[ i ] ^= 0xFFFFFFFF;
		}
	}

	static Bool isInSet(
		const	Uchar		* x,
		const	Uchar		y
	) {
		return ( ( x[ bytPos( y ) ] & bitPos( y ) ) != 0 );
	}

	static Bool isDig(
		const	Uchar		c
	) {
		return ( c >= '0' && c <= '9' );
	}

	static Bool isWsp(
		const	Uchar		c
	) {
		return ( c == ' '
		      || c == '\f'
		      || c == '\n'
		      || c == '\r'
		      || c == '\t'
		      || c == '\v' );
	}

	static Bool isWrd(
		const	Uchar		c
	) {
		return ( ( c >= 'a' && c <= 'z' )
		      || ( c >= 'A' && c <= 'Z' )
		      || ( c >= '0' && c <= '9' )
		      || c == '_' );
	}

private:

	enum {
		MAXTAG		= 10,
		MAXNFA		= 2048,
		NOTFOUND	= -1
	};

	enum {

		OKP     = 1,
		NOP     = 0,

		CHR     = 1,
		ANY     = 2,
		CCL     = 3,
		BOL     = 4,
		EOL     = 5,
		BOT     = 6,
		EOT     = 7,
		BOW     = 8,
		EOW     = 9,
		REF     = 10,
		CLO     = 11,
		PCL	= 12,	///< Predefined set of characters.

		END     = 0,

		PCL_DIG	= 0,	///< 'd' [digit] set of characters.
		PCL_NDG = 1,	///< 'D' [non-digit] set of characters.
		PCL_WSP = 2,	///< 's' [whitespace] set of characters.
		PCL_NWS = 3,	///< 'S' [non-whitespace] set of characters.
		PCL_WRD = 4,	///< 'w' [word] set of characters (let, dig, '_').
		PCL_NWD = 5	///< 'W' [non-word] set of characters.

	};

	static Bool		tblInit;		///< Static tables initialized ?
	static Uchar		escTable[ 0x100 ];	///< Escape sequences.

	Buffer			lastCompile;		///< Last compiled pattern.
	Buffer			lastExecute;		///< Last checked string.
	BufferArray		lastMatches;		///< Last grabbed matches.

	int bopat[MAXTAG];
	int eopat[MAXTAG];

	Uint32			tagstk[ MAXTAG ];	///< Subpat tag stack.
	Uchar			nfa[ MAXNFA ];		///< Automaton.

	SimpleRegEx(
		const	SimpleRegEx		& other
	);

	SimpleRegEx & operator = (
		const	SimpleRegEx		& other
	);

};

// ============================================================================

} // namespace RegEx
} // namespace BFC

// ============================================================================

#endif // _BFC_RegEx_H_

// ============================================================================

