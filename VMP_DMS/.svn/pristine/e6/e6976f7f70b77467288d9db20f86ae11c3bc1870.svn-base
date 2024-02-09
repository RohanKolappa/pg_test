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
//	BFC.RegEx.Matcher.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_RegEx_Matcher_H_
#define _BFC_RegEx_Matcher_H_

// ============================================================================

#include "BFC.RegEx.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace RegEx {

BFC_PTR_DECL( BFC_RegEx_DLL, Matcher )

} // namespace RegEx
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.TL.BufferArray.h"

// ============================================================================

namespace BFC {
namespace RegEx {

// ============================================================================

class NFANode;
class NFAStartNode;
class NFAEndNode;
class NFAGroupHeadNode;
class NFAGroupLoopNode;
class NFAGroupLoopPrologueNode;
class NFAGroupTailNode;
class NFALookBehindNode;
class NFAStartOfLineNode;
class NFAEndOfLineNode;
class NFAEndOfMatchNode;
class NFAReferenceNode;

class Pattern;

// ============================================================================

/// \brief Scans strings using a given Pattern.
///
/// A matcher is a non thread-safe object used to scan strings using a given
/// {@link Pattern Pattern} object. Using a <code>Matcher</code> is the preferred
/// method for scanning strings. Matchers are not thread-safe. Matchers require
/// very little dynamic memory, hence one is encouraged to create several
/// instances of a matcher when necessary as opposed to sharing a single instance
/// of a matcher.
/// <p>
/// The most common methods needed by the matcher are <code>matches</code>,
/// <code>findNextMatch</code>, and <code>getGroup</code>. <code>matches</code>
/// and <code>findNextMatch</code> both return success or failure, and further
/// details can be gathered from their documentation.
/// <p>
/// Unlike Java's <code>Matcher</code>, this class allows you to change the string
/// you are matching against. This provides a small optimization, since you no
/// longer need multiple matchers for a single pattern in a single thread.
/// <p>
/// This class also provides an extremely handy method for replacing text with
/// captured data via the <code>replaceWithGroups</code> method. A typical
/// invocation looks like:
/// <pre>
/// char buf[10000];
/// Buffer str = "\\5 (user name \\1) uses \\7 for his/her shell and \\6 is their home directory";
/// FILE * fp = fopen("/etc/passwd", "r");
/// Pattern::registerPattern("entry", "[^:]+");
/// Pattern * p = Pattern::compile("^({entry}):({entry}):({entry}):({entry}):({entry}):({entry}):({entry})$",
/// &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Pattern::MULTILINE_MATCHING | Pattern::UNIX_LINE_MODE);
/// Matcher * m = p->createMatcher("");
/// while (fgets(buf, 9999, fp))
/// {
/// &nbsp;&nbsp;m->setString(buf);
/// &nbsp;&nbsp;if (m->matches())
/// &nbsp;&nbsp;{
/// &nbsp;&nbsp;&nbsp;&nbsp;printf("%s\n", m->replaceWithGroups(str).c_str());
/// &nbsp;&nbsp;}
/// }
/// fclose(fp);
/// 
/// </pre>
/// Calling any of the following functions before first calling
/// <code>matches</code>, <code>findFirstMatch</code>, or
/// <code>findNextMatch</code> results in undefined behavior and may cause your
/// program to crash.
/// <code>
/// <ul>
/// 	<li>replaceWithGroups</li>
/// 	<li>getStartingIndex</li>
/// 	<li>getEndingIndex</li>
/// 	<li>getGroup</li>
/// 	<li>getGroups</li>
/// </ul>
/// </code>
/// <p>
/// The function <code>findFirstMatch</code> will attempt to find the first match
/// in the input string. The same results can be obtained by first calling
/// <code>reset</code> followed by <code>findNextMatch</code>.
/// <p>
/// To eliminate the necessity of looping through a string to find all the
/// matching substrings, <code>findAll</code> was created. The function will find
/// all matching substrings and return them in a <code>vector</code>. If you need
/// to examine specific capture groups within the substrings, then this method
/// should not be used.
/// 
///
/// \ingroup BFC_RegEx

class BFC_RegEx_DLL Matcher : virtual public SObject {

public :

	/// \brief Creates a new Matcher.

	Matcher(
	);

	/// \brief Destroys this object.

	virtual ~Matcher(
	);

	friend class NFANode;
	friend class NFAStartNode;
	friend class NFAEndNode;
	friend class NFAGroupHeadNode;
	friend class NFAGroupLoopNode;
	friend class NFAGroupLoopPrologueNode;
	friend class NFAGroupTailNode;
	friend class NFALookBehindNode;
	friend class NFAStartOfLineNode;
	friend class NFAEndOfLineNode;
	friend class NFAEndOfMatchNode;
	friend class NFAReferenceNode;
	friend class Pattern;

protected:

	/// The pattern we use to match
	Pattern * pat;
	/// The string in which we are matching
	Buffer str;
	/// The starting point of our match
	int start;
	/// An array of the starting positions for each group
	int * starts;
	/// An array of the ending positions for each group
	int * ends;
	/// An array of private data used by NFANodes during matching
	int * groups;
	/// An array of private data used by NFANodes during matching
	int * groupIndeces;
	/// An array of private data used by NFANodes during matching
	int * groupPos;
	/// The ending index of the last match
	int lm;
	/// The number of capturing groups we have
	int gc;
	/// The number of non-capturing groups we havew
	int ncgc;
	/// Whether or not we have matched something (used only by findFirstMatch and findNextMatch)
	int matchedSomething;
	/// The flags with which we were made
	Uint32 flags;
	/// Called by reset to clear the group arrays
	void clearGroups();
public:

	/// Used internally by match to signify we want the entire string matched

	static const int MATCH_ENTIRE_STRING;

public:
	/**
		Replaces the contents of <code>str</code> with the appropriate captured
		text. <code>str</code> should have at least one back reference, otherwise
		this function does nothing.
		@param str The string in which to replace text
		@return A string with all backreferences appropriately replaced
	 */
	Buffer replaceWithGroups(const Buffer & str);
	/**
		The flags currently being used by the matcher.
		@return Zero
	 */
	Uint32 getFlags() const;
	/**
		The text being searched by the matcher.
		@return the text being searched by the matcher.
	 */
	Buffer getText() const;

	/**
		Scans the string from start to finish for a match. The entire string must
		match for this function to return success. Group variables are
		appropriately set and can be queried after this function returns.

		@return Success if and only if the entire string matches the pattern
	 */
	Bool matches();
	/**
		Scans the string for the first substring matching the pattern. The entire
		string does not necessarily have to match for this function to return
		success. Group variables are appropriately set and can be queried after
		this function returns.

		@return Success if any substring matches the specified pattern
	 */
	Bool findFirstMatch();
	/**
		Scans the string for the next substring matching the pattern. If no calls
		have been made to findFirstMatch of findNextMatch since the last call to
		reset, matches, or setString, then this function's behavior results to
		that of findFirstMatch.

		@return Success if another substring can be found that matches the pattern
	 */
	Bool findNextMatch();
	/**
		Returns a vector of every substring in order which matches the given
		pattern.

		@return Every substring in order which matches the given pattern
	 */
	BufferArray findAll();
	/**
		Resets the internal state of the matcher
	 */
	void reset();
	/**
		Same as getText. Left n for backwards compatibilty with old source code
		@return Returns the string that is currently being used for matching
	 */

	Buffer getString(
	) const {
		return str;
	}

	/**
		Sets the string to scan
		@param newStr The string to scan for subsequent matches
	 */

	void setString(
		const	Buffer &		newStr
	) {
		str = newStr;
		reset();
	}

	/**
		Returns the starting index of the specified group.
		@param groupNum The group to query
		@return The starting index of the group if it was matched, -1 for an
						invalid group or if the group was not matched
	 */
	int getStartingIndex(const int groupNum = 0) const;
	/**
		Returns the ending index of the specified group.
		@param groupNum The group to query
		@return The ending index of the group if it was matched, -1 for an
						invalid group or if the group was not matched
	 */
	int getEndingIndex(const int groupNum = 0) const;
	/**
		Returns the specified group. An empty string ("") does not necessarily
		mean the group was not matched. A group such as (a*b?) could be matched by
		a zero length. If an empty string is returned, getStartingIndex can be
		called to determine if the group was actually matched.
		@param groupNum The group to query
		@return The text of the group
	 */
	Buffer getGroup(const int groupNum = 0) const;
	/**
		Returns every capture group in a vector

		@param includeGroupZero Whether or not include capture group zero
		@return Every capture group
	 */
	BufferArray getGroups(const Bool includeGroupZero = 0) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.RegEx.Matcher" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected :

	/// \brief Creates a new matcher object against <code>text</code> using
	///	<code>pattern</code>.
	/// 
	/// \param pattern
	///	The pattern with which to search.
	///
	/// \param text
	///	The text in which to search.

	Matcher(
			Pattern *	pattern,
		const	Buffer &	text
	);

private :

	/// \brief Forbidden copy constructor.

	Matcher(
		const	Matcher&
	);

	/// \brief Forbidden copy operator.

	Matcher& operator = (
		const	Matcher&
	);

};

// ============================================================================

} // namespace RegEx
} // namespace BFC

// ============================================================================

#endif // _BFC_RegEx_Matcher_H_

// ============================================================================

