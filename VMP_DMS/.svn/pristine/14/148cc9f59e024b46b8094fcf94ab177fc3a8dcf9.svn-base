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
//	BFC.RegEx.NewMatcher.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_RegEx_NewMatcher_H_
#define _BFC_RegEx_NewMatcher_H_

// ============================================================================

#include "BFC.RegEx.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace RegEx {

BFC_PTR_DECL( BFC_RegEx_DLL, NewMatcher )

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

class NewNFANode;
class NewNFAStartNode;
class NewNFAEndNode;
class NewNFAGroupHeadNode;
class NewNFAGroupLoopNode;
class NewNFAGroupLoopPrologueNode;
class NewNFAGroupTailNode;
class NewNFALookBehindNode;
class NewNFAStartOfLineNode;
class NewNFAEndOfLineNode;
class NewNFAEndOfMatchNode;
class NewNFAReferenceNode;

class NewPattern;

// ============================================================================

/// \brief Scans strings using a given NewPattern.
///
/// A matcher is a non thread-safe object used to scan strings using a given
/// {@link NewPattern NewPattern} object. Using a <code>NewMatcher</code> is the preferred
/// method for scanning strings. NewMatchers are not thread-safe. NewMatchers require
/// very little dynamic memory, hence one is encouraged to create several
/// instances of a matcher when necessary as opposed to sharing a single instance
/// of a matcher.
/// <p>
/// The most common methods needed by the matcher are <code>matches</code>,
/// <code>findNextMatch</code>, and <code>getGroup</code>. <code>matches</code>
/// and <code>findNextMatch</code> both return success or failure, and further
/// details can be gathered from their documentation.
/// <p>
/// Unlike Java's <code>NewMatcher</code>, this class allows you to change the string
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
/// NewPattern::registerNewPattern("entry", "[^:]+");
/// NewPattern * p = NewPattern::compile("^({entry}):({entry}):({entry}):({entry}):({entry}):({entry}):({entry})$",
/// &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;NewPattern::MULTILINE_MATCHING | NewPattern::UNIX_LINE_MODE);
/// NewMatcher * m = p->createNewMatcher("");
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

class BFC_RegEx_DLL NewMatcher : virtual public SObject {

//	friend class NewNFANode;
//	friend class NewNFAStartNode;
//	friend class NewNFAEndNode;
//	friend class NewNFAGroupHeadNode;
//	friend class NewNFAGroupLoopNode;
//	friend class NewNFAGroupLoopPrologueNode;
//	friend class NewNFAGroupTailNode;
//	friend class NewNFALookBehindNode;
//	friend class NewNFAStartOfLineNode;
//	friend class NewNFAEndOfLineNode;
//	friend class NewNFAEndOfMatchNode;
//	friend class NewNFAReferenceNode;
//	friend class NewPattern;

public :

	/// \brief Creates a new NewMatcher.

	NewMatcher(
	);

	/// \brief Creates a new NewMatcher.
	/// 
	/// \param pattern
	///	The pattern with which to search.
	///
	/// \param text
	///	The text in which to search.

	NewMatcher(
			NewPattern *	pattern,
		const	Buffer &	text = Buffer()
	);

public :

	/// \brief Destroys this object.

	virtual ~NewMatcher(
	);

	enum {
		MATCH_ENTIRE_STRING	= 0x01
	};

	/// \brief Returns the flags currently being used by the matcher.

	Uint32 getFlags(
	) const;

	/// \brief Returns true iff this Matcher uses flag \a mask.

	Bool hasFlag(
		const	Uint32		mask
	) const {
		return ( ( flags & mask ) != 0 );
	}

	/// \brief Returns the text being searched by this object.

	Buffer getText(
	) const;

	/// \brief Replaces the content of \a str with the appropriate
	///	captured text.
	///
	/// \a str should have at least one back reference, otherwise this
	/// function does nothing.
	///
	/// \param str
	///	The string in which to replace text.
	///
	/// \returns
	///	A string with all backreferences appropriately replaced.

	Buffer replaceWithGroups(
		const	Buffer &	str
	);

	/// \brief Scans the string from start to finish for a match.
	///
	/// The entire string must match for this function to return success.
	///
	/// Group variables are appropriately set and can be queried after
	/// this function returns.
	///
	/// \returns
	///	True iff the entire string matches the pattern.

	Bool matches(
	);

	/// \brief Scans the string for the first substring matching the
	///	pattern.
	///
	/// The entire string does not necessarily have to match for this
	/// function to return success.
	///
	/// Group variables are appropriately set and can be queried after
	/// this function returns.
	///
	/// \returns
	///	Success if any substring matches the specified pattern.

	Bool findFirstMatch(
	);

	/// \brief Scans the string for the next substring matching the
	///	pattern.
	///
	/// If no calls have been made to findFirstMatch() of findNextMatch()
	/// since the last call to reset(), matches(), or setString(), then
	/// this function's behavior results to that of findFirstMatch().
	///
	/// \returns
	///	True iff another substring can be found that matches the
	///	pattern.

	Bool findNextMatch();

	/**
		Returns a vector of every substring in order which matches the given
		pattern.

		\returns Every substring in order which matches the given pattern
	 */
	BufferArray findAll();
	/**
		Resets the internal state of the matcher
	 */
	void reset();
	/**
		Same as getText. Left n for backwards compatibilty with old source code
		\returns Returns the string that is currently being used for matching
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
		\returns The starting index of the group if it was matched, -1 for an
						invalid group or if the group was not matched
	 */
	int getStartingIndex(const int groupNum = 0) const;
	/**
		Returns the ending index of the specified group.
		@param groupNum The group to query
		\returns The ending index of the group if it was matched, -1 for an
						invalid group or if the group was not matched
	 */
	int getEndingIndex(const int groupNum = 0) const;
	/**
		Returns the specified group. An empty string ("") does not necessarily
		mean the group was not matched. A group such as (a*b?) could be matched by
		a zero length. If an empty string is returned, getStartingIndex can be
		called to determine if the group was actually matched.
		@param groupNum The group to query
		\returns The text of the group
	 */
	Buffer getGroup(const int groupNum = 0) const;
	/**
		Returns every capture group in a vector

		@param includeGroupZero Whether or not include capture group zero
		\returns Every capture group
	 */
	BufferArray getGroups(const Bool includeGroupZero = 0) const;

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.RegEx.NewMatcher" );

	CLASS_EXCEPTION( SampleException, "Sample exception" );

protected:

public :

	NewPattern *	pat;	///< The pattern we use to match.
	Buffer		str;	///< The string in which we are matching.
	int		start;	///< The starting point of our match.
	int *		starts;	///< Starting position for each group.
	int *		ends;		///< Ending position for each group.
	int *		groups;		///< Private data.
	int *		groupIndeces;	///< Private data.
	int *		groupPos;	///< Private data.
	int		lm;	///< The ending index of the last match.
	int		gc;	///< The number of capturing groups we have.
	int		ncgc;	///< The number of non-capturing groups we have.
	int		matchedSomething;	///< Whether or not we have matched something (used only by findFirstMatch and findNextMatch).
	Uint32		flags;	///< The flags with which we were made.

	/// Called by reset to clear the group arrays.

	void clearGroups(
	);

private :

	/// \brief Forbidden copy constructor.

	NewMatcher(
		const	NewMatcher&
	);

	/// \brief Forbidden copy operator.

	NewMatcher& operator = (
		const	NewMatcher&
	);

};

// ============================================================================

} // namespace RegEx
} // namespace BFC

// ============================================================================

#endif // _BFC_RegEx_NewMatcher_H_

// ============================================================================

