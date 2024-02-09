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
//	BFC.RegEx.Pattern.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_RegEx_Pattern_H_
#define _BFC_RegEx_Pattern_H_

// ============================================================================

#include "BFC.RegEx.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace RegEx {

BFC_PTR_DECL( BFC_RegEx_DLL, Pattern )

} // namespace RegEx
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.TL.BufferArray.h"
#include "BFC.TL.Map.h"
#include "BFC.TL.Pair.h"

#include "BFC.RegEx.Matcher.h"

// ============================================================================

namespace BFC {
namespace RegEx {

// ============================================================================

//class Matcher;
class NFANode;

// ============================================================================

/// \brief Represents an immutable regular expression.
///
/// This pattern class is very similar in functionality to Java's
/// java.util.regex.Pattern class. The pattern class represents an immutable
/// regular expression object. Instead of having a single object contain both the
/// regular expression object and the matching object, instead the two objects are
/// split apart. The {@link Matcher Matcher} class represents the maching
/// object.
/// 
/// The Pattern class works primarily off of "compiled" patterns. A typical
/// instantiation of a regular expression looks like:
/// 
/// <pre>
/// Pattern * p = Pattern::compile("a*b");
/// Matcher * m = p->createMatcher("aaaaaab");
/// if (m->matches()) ...
/// </pre>
/// 
/// However, if you do not need to use a pattern more than once, it is often times
/// okay to use the Pattern's static methods insteads. An example looks like this:
/// 
/// <pre>
/// if (Pattern::matches("a*b", "aaaab")) { ... }
/// </pre>
/// 
/// This class does not currently support unicode. The unicode update for this
/// class is coming soon.
/// 
/// This class is partially immutable. It is completely safe to call createMatcher
/// concurrently in different threads, but the other functions (e.g. split) should
/// not be called concurrently on the same <code>Pattern</code>.
/// 
/// <table border="0" cellpadding="1" cellspacing="0">
/// 	<tr align="left" bgcolor="#CCCCFF">
/// 		<td>
/// 			<b>Construct</b>
/// 		</td>
/// 		<td>
/// 			<b>Matches</b>
/// 		</th>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 		&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Characters</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x</i></code>
/// 		</td>
/// 		<td>
/// 			The character <code><i>x</i></code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\\\\</code>
/// 		</td>
/// 		<td>
/// 			The character <code>\\</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\\0<i>nn</i></code>
/// 		</td>
/// 		<td>
/// 			The character with octal ASCII value <code><i>nn</i></code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\0<i>nnn</i></code>
/// 		</td>
/// 		<td>
/// 			The character with octal ASCII value <code><i>nnn</i></code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\x<i>hh</i></code>
/// 		</td>
/// 		<td>
/// 			The character with hexadecimal ASCII value <code><i>hh</i></code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\\t</code>
/// 		</td>
/// 		<td>
/// 			A tab character
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\\r</code>
/// 		</td>
/// 		<td>
/// 			A carriage return character
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\\n</code>
/// 		</td>
/// 		<td>
/// 			A new-line character
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<b>Character Classes</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[abc]</code>
/// 		</td>
/// 		<td>
/// 			Either <code>a</code>, <code>b</code>, or <code>c</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[^abc]</code>
/// 		</td>
/// 		<td>
/// 			Any character but <code>a</code>, <code>b</code>, or <code>c</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[a-zA-Z]</code>
/// 		</td>
/// 		<td>
/// 			Any character ranging from <code>a</code> thru <code>z</code>, or
/// 			<code>A</code> thru <code>Z</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[^a-zA-Z]</code>
/// 		</td>
/// 		<td>
/// 			Any character except those ranging from <code>a</code> thru
/// 			<code>z</code>, or <code>A</code> thru <code>Z</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[a\-z]</code>
/// 		</td>
/// 		<td>
/// 			Either <code>a</code>, <code>-</code>, or <code>z</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[a-z[A-Z]]</code>
/// 		</td>
/// 		<td>
/// 			Same as <code>[a-zA-Z]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[a-z&&[g-i]]</code>
/// 		</td>
/// 		<td>
/// 			Any character in the intersection of <code>a-z</code> and
/// 			<code>g-i</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>[a-z&&[^g-i]]</code>
/// 		</td>
/// 		<td>
/// 			Any character in <code>a-z</code> and not in <code>g-i</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Prefefined character classes</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><b>.</b></code>
/// 		</td>
/// 		<td>
/// 			Any character. Multiline matching must be compiled into the pattern for
/// 			<code><b>.</b></code> to match a <code>\r</code> or a <code>\n</code>.
/// 			Even if multiline matching is enabled, <code><b>.</b></code> will not
/// 			match a <code>\r\n</code>, only a <code>\r</code> or a <code>\n</code>.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\d</code>
/// 		</td>
/// 		<td>
/// 			<code>[0-9]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\D</code>
/// 		</td>
/// 		<td>
/// 			<code>[^\d]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\s</code>
/// 		</td>
/// 		<td>
/// 			<code>[&nbsp;\t\r\n\x0B]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\S</code>
/// 		</td>
/// 		<td>
/// 			<code>[^\s]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\w</code>
/// 		</td>
/// 		<td>
/// 			<code>[a-zA-Z0-9_]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\W</code>
/// 		</td>
/// 		<td>
/// 			<code>[^\w]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>POSIX character classes
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{Lower}</code>
/// 		</td>
/// 		<td>
/// 			<code>[a-z]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{Upper}</code>
/// 		</td>
/// 		<td>
/// 			<code>[A-Z]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{ASCII}</code>
/// 		</td>
/// 		<td>
/// 			<code>[\x00-\x7F]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{Alpha}</code>
/// 		</td>
/// 		<td>
/// 			<code>[a-zA-Z]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{Digit}</code>
/// 		</td>
/// 		<td>
/// 			<code>[0-9]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{Alnum}</code>
/// 		</td>
/// 		<td>
/// 			<code>[\w&&[^_]]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{Punct}</code>
/// 		</td>
/// 		<td>
/// 			<code>[!"#$%&'()*+,-./:;&lt;=&gt;?@[\]^_`{|}~]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\p{XDigit}</code>
/// 		</td>
/// 		<td>
/// 			<code>[a-fA-F0-9]</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Boundary Matches</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>^</code>
/// 		</td>
/// 		<td>
/// 			The beginning of a line. Also matches the beginning of input.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>$</code>
/// 		</td>
/// 		<td>
/// 			The end of a line. Also matches the end of input.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\b</code>
/// 		</td>
/// 		<td>
/// 			A word boundary
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\B</code>
/// 		</td>
/// 		<td>
/// 			A non word boundary
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\A</code>
/// 		</td>
/// 		<td>
/// 			The beginning of input
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\G</code>
/// 		</td>
/// 		<td>
/// 			The end of the previous match. Ensures that a "next" match will only
/// 			happen if it begins with the character immediately following the end of
/// 			the "current" match.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\Z</code>
/// 		</td>
/// 		<td>
/// 			The end of input. Will also match if there is a single trailing
/// 			<code>\r\n</code>, a single trailing <code>\r</code>, or a single
/// 			trailing <code>\n</code>.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\z</code>
/// 		</td>
/// 		<td>
/// 			The end of input
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Greedy Quantifiers</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x?</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, either zero times or one time
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x*</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, zero or more times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x+</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, one or more times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n}</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, exactly n times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n,}</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at least <code><i>n</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{,m}</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at most <code><i>m</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n,m}</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at least <code><i>n</i></code> times and at most
/// 			<code><i>m</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Possessive Quantifiers</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x?+</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, either zero times or one time
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x*+</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, zero or more times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x++</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, one or more times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n}+</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, exactly n times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n,}+</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at least <code><i>n</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{,m}+</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at most <code><i>m</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n,m}+</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at least <code><i>n</i></code> times and at most
/// 			<code><i>m</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Reluctant Quantifiers</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x??</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, either zero times or one time
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x*?</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, zero or more times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x+?</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, one or more times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n}?</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, exactly n times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n,}?</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at least <code><i>n</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{,m}?</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at most <code><i>m</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x{n,m}?</i></code>
/// 		</td>
/// 		<td>
/// 			<i>x</i>, at least <code><i>n</i></code> times and at most
/// 			<code><i>m</i></code> times
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Operators</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>xy</i></code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code> then <code><i>y</i></code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code><i>x</i></code>|<code><i>y</i></code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code> or <code><i>y</i></code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>(<i>x</i>)</code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code> as a capturing group
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Quoting</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\Q</code>
/// 		</td>
/// 		<td>
/// 			Nothing, but treat every character (including \s) literally until a
/// 			matching <code>\E</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>\E</code>
/// 		</td>
/// 		<td>
/// 			Nothing, but ends its matching <code>\Q</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Special Constructs</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>(?:<i>x</i>)</code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code>, but not as a capturing group
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>(?=<i>x</i>)</code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code>, via positive lookahead. This means that the
/// 			expression will match only if it is trailed by <code><i>x</i></code>.
/// 			It will not "eat" any of the characters matched by
/// 			<code><i>x</i></code>.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>(?!<i>x</i>)</code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code>, via negative lookahead. This means that the
/// 			expression will match only if it is not trailed by
/// 			<code><i>x</i></code>. It will not "eat" any of the characters
/// 			matched by <code><i>x</i></code>.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>(?<=<i>x</i>)</code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code>, via positive lookbehind. <code><i>x</i></code>
/// 			cannot contain any quantifiers.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>(?<!<i>x</i>)</code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i></code>, via negative lookbehind. <code><i>x</i></code>
/// 			cannot contain any quantifiers.
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>(?><i>x</i>)</code>
/// 		</td>
/// 		<td>
/// 			<code><i>x</i>{1}+</code>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			&nbsp;
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td colspan="2">
/// 			<b>Registered Expression Matching</b>
/// 		</td>
/// 	</tr>
/// 	<tr>
/// 		<td>
/// 			<code>{<i>x</i>}</code>
/// 		</td>
/// 		<td>
/// 			The registered pattern <code><i>x</i></code>
/// 		</td>
/// 	</tr>
/// </table>
/// 
/// <hr>
/// 
///  <i>Begin Text Extracted And Modified From java.util.regex.Pattern documentation</i>
/// 
///  <h4> Backslashes, escapes, and quoting </h4>
/// 
///  <p> The backslash character (<tt>'\'</tt>) serves to introduce escaped
///  constructs, as defined in the table above, as well as to quote characters
///  that otherwise would be interpreted as unescaped constructs.	 Thus the
///  expression <tt>\\</tt> matches a single backslash and <tt>\{</tt> matches a
///  left brace.
/// 
///  <p> It is an error to use a backslash prior to any alphabetic character that
///  does not denote an escaped construct; these are reserved for future
///  extensions to the regular-expression language.	 A backslash may be used
///  prior to a non-alphabetic character regardless of whether that character is
///  part of an unescaped construct.
/// 
///  <p>It is necessary to double backslashes in string literals that represent
///  regular expressions to protect them from interpretation by a compiler.	 The
///  string literal <tt>"&#92;b"</tt>, for example, matches a single backspace
///  character when interpreted as a regular expression, while
///  <tt>"&#92;&#92;b"</tt> matches a word boundary.	The string litera
/// 	<tt>"&#92;(hello&#92;)"</tt> is illegal and leads to a compile-time error;
/// 	in order to match the string <tt>(hello)</tt> the string literal
/// 	<tt>"&#92;&#92;(hello&#92;&#92;)"</tt> must be used.
/// 
///  <h4> Character Classes </h4>
/// 
/// 		<p> Character classes may appear within other character classes, and
/// 		may be composed by the union operator (implicit) and the intersection
/// 		operator (<tt>&amp;&amp;</tt>).
/// 		The union operator denotes a class that contains every character that is
/// 		in at least one of its operand classes.	 The intersection operator
/// 		denotes a class that contains every character that is in both of its
/// 		operand classes.
/// 
/// 		<p> The precedence of character-class operators is as follows, from
/// 		highest to lowest:
/// 
/// 		<blockquote><table border="0" cellpadding="1" cellspacing="0"
/// 								 summary="Precedence of character class operators.">
/// 
/// 			<tr><th>1&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 					<td>Literal escape&nbsp;&nbsp;&nbsp;&nbsp;</td>
/// 					<td><tt>\x</tt></td></tr>
/// 			<tr><th>2&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 					<td>Range</td>
/// 					<td><tt>a-z</tt></td></tr>
/// 			<tr><th>3&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 					<td>Grouping</td>
/// 					<td><tt>[...]</tt></td></tr>
/// 			<tr><th>4&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 					<td>Intersection</td>
/// 					<td><tt>[a-z&&[aeiou]]</tt></td></tr>
/// 			<tr><th>5&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 					<td>Union</td>
/// 					<td><tt>[a-e][i-u]<tt></td></tr>
/// 		</table></blockquote>
/// 
/// 		<p> Note that a different set of metacharacters are in effect inside
/// 		a character class than outside a character class. For instance, the
/// 		regular expression <tt>.</tt> loses its special meaning inside a
/// 		character class, while the expression <tt>-</tt> becomes a range
/// 		forming metacharacter.
/// 
///  <a name="lt">
/// 
///  <a name="cg">
///  <h4> Groups and capturing </h4>
/// 
///  <p> Capturing groups are numbered by counting their opening parentheses from
///  left to right.	 In the expression <tt>((A)(B(C)))</tt>, for example, there
///  are four such groups: </p>
/// 
///  <blockquote><table cellpadding=1 cellspacing=0 summary="Capturing group numberings">
/// 
///  <tr><th>1&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 		 <td><tt>((A)(B(C)))</tt></td></tr>
///  <tr><th>2&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 		 <td><tt>(A)</tt></td></tr>
///  <tr><th>3&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 		 <td><tt>(B(C))</tt></td></tr>
/// 
///  <tr><th>4&nbsp;&nbsp;&nbsp;&nbsp;</th>
/// 		 <td><tt>(C)</tt></td></tr>
///  </table></blockquote>
/// 
///  <p> Group zero always stands for the entire expression.
/// 
///  <p> Capturing groups are so named because, during a match, each subsequence
///  of the input sequence that matches such a group is saved.	The captured
///  subsequence may be used later in the expression, via a back reference, and
///  may also be retrieved from the matcher once the match operation is complete.
/// 
///  <p> The captured input associated with a group is always the subsequence
///  that the group most recently matched.	If a group is evaluated a second time
///  because of quantification then its previously-captured value, if any, will
///  be retained if the second evaluation fails.	Matching the string
///  <tt>"aba"</tt> against the expression <tt>(a(b)?)+</tt>, for example, leaves
///  group two set to <tt>"b"</tt>.	 All captured input is discarded at the
///  beginning of each match.
/// 
///  <p> Groups beginning with <tt>(?</tt> are pure, <i>non-capturing</i> groups
///  that do not capture text and do not count towards the group total.
/// 
/// 
///  <h4> Unicode support </h4>
/// 
///  <p> Coming Soon.
/// 
///  <h4> Comparison to Perl 5 </h4>
/// 
///  <p>The <code>Pattern</code> engine performs traditional NFA-based matching
///  with ordered alternation as occurs in Perl 5.
/// 
///  <p> Perl constructs not supported by this class: </p>
/// 
///  <ul>
/// 
/// 		<li><p> The conditional constructs <tt>(?{</tt><i>X</i><tt>})</tt> and
/// 		<tt>(?(</tt><i>condition</i><tt>)</tt><i>X</i><tt>|</tt><i>Y</i><tt>)</tt>,
/// 		</p></li>
/// 
/// 		<li><p> The embedded code constructs <tt>(?{</tt><i>code</i><tt>})</tt>
/// 		and <tt>(??{</tt><i>code</i><tt>})</tt>,</p></li>
/// 
/// 		<li><p> The embedded comment syntax <tt>(?#comment)</tt>, and </p></li>
/// 
/// 		<li><p> The preprocessing operations <tt>\l</tt> <tt>&#92;u</tt>,
/// 		<tt>\L</tt>, and <tt>\U</tt>.	 </p></li>
/// 
/// 		<li><p> Embedded flags</p></li>
/// 
///  </ul>
/// 
///  <p> Constructs supported by this class but not by Perl: </p>
/// 
///  <ul>
/// 
/// 		<li><p> Possessive quantifiers, which greedily match as much as they can
/// 		and do not back off, even when doing so would allow the overall match to
/// 		succeed.	</p></li>
/// 
/// 		<li><p> Character-class union and intersection as described
/// 		above.</p></li>
/// 
///  </ul>
/// 
///  <p> Notable differences from Perl: </p>
/// 
///  <ul>
/// 
/// 		<li><p> In Perl, <tt>\1</tt> through <tt>\9</tt> are always interpreted
/// 		as back references; a backslash-escaped number greater than <tt>9</tt> is
/// 		treated as a back reference if at least that many subexpressions exist,
/// 		otherwise it is interpreted, if possible, as an octal escape.	 In this
/// 		class octal escapes must always begin with a zero. In this class,
/// 		<tt>\1</tt> through <tt>\9</tt> are always interpreted as back
/// 		references, and a larger number is accepted as a back reference if at
/// 		least that many subexpressions exist at that point in the regular
/// 		expression, otherwise the parser will drop digits until the number is
/// 		smaller or equal to the existing number of groups or it is one digit.
/// 		</p></li>
/// 
/// 		<li><p> Perl uses the <tt>g</tt> flag to request a match that resumes
/// 		where the last match left off.	This functionality is provided implicitly
/// 		by the <CODE>Matcher</CODE> class: Repeated invocations of the
/// 		<code>find</code> method will resume where the last match left off,
/// 		unless the matcher is reset.	</p></li>
/// 
/// 		<li><p> Perl is forgiving about malformed matching constructs, as in the
/// 		expression <tt>*a</tt>, as well as dangling brackets, as in the
/// 		expression <tt>abc]</tt>, and treats them as literals.	This
/// 		class also strict and will not compile a pattern when dangling characters
/// 		are encountered.</p></li>
/// 
///  </ul>
/// 
/// 
///  <p> For a more precise description of the behavior of regular expression
///  constructs, please see <a href="http://www.oreilly.com/catalog/regex2/">
///  <i>Mastering Regular Expressions, 2nd Edition</i>, Jeffrey E. F. Friedl,
///  O'Reilly and Associates, 2002.</a>
///  </p>
/// <P>
/// 
/// <i>End Text Extracted And Modified From java.util.regex.Pattern documentation</i>
/// 
/// <hr>
///
/// \ingroup BFC_RegEx

class BFC_RegEx_DLL Pattern : virtual public SObject {

public :

	/// \brief Creates a new Pattern.

	Pattern(
	);

	/// \brief Destroys this object.

	virtual ~Pattern(
	);

	/// \brief Resets this object.

	void kill(
	);

	enum {
		/// \brief Case insensitive matching.
		CASE_INSENSITIVE	= 0x01,
		/// \brief Implicitly quoted.
		LITERAL			= 0x02,
		/// \brief '.' is [\\x00-\\x7F].
		DOT_MATCHES_ALL		= 0x04,
		/// \brief '^' and '$' anchor to the beginning and ending of
		///	lines, not all input.
		MULTILINE_MATCHING	= 0x08,
		/// \brief Only '\\n' is line terminator.
		UNIX_LINE_MODE		= 0x10
	};

	enum {
		/// Absolute minimum number of matches a quantifier can match.
		MIN_QMATCH		= 0x00000000,
		/// Absolute maximum number of matches a quantifier can match.
		MAX_QMATCH		= 0x7FFFFFFF
	};

	/// \brief Compiles the regular expression given by \a pattern.
	///
	/// Special values can be assigned to \a mode when certain non-standard
	/// behaviors are expected from the pattern.
	///
	/// \param pattern
	///	The regular expression to compile.
	///
	/// \param mode
	///	A bitwise OR of flags signaling what special behaviors are
	///	wanted from the pattern.

	void compile(
		const	Buffer &	pattern,
		const	Uint32		mode = 0
	);

	/// \brief Searches through \a originText and replaces all substrings
	///	matched by this pattern with \a replaceText.
	///
	/// \a replaceText may contain backreferences (e.g. "\1") to capture groups.
	///
	/// A typical invocation looks like:
	/// \code
	///	Pattern p;
	///	p.compile( "(a+)b(c+)" );
	///	BFC::Buffer r = p.replace( "abcccbbabcbabc", "\\2b\\1" );
	/// \endcode
	/// which would replace "abcccbbabcbabc" with
	/// "cccbabbcbabcba".
	///
	/// \param originText
	///	The original text to parse.
	///
	/// \param replaceText
	///	The replacement text.
	///
	/// \returns
	///	The text with the replacement string substituted where necessary.

	Buffer replace(
		const	Buffer &	originText,
		const	Buffer &	replaceText
	);

	/**
	Splits the specified string over occurrences of the specified pattern.
	Empty strings can be optionally ignored. The number of strings returned is
	configurable. A typical invocation looks like:
	<p>
	<code>
	Buffer str(strSize, '\0');<br>
	FILE * fp = fopen(fileName, "r");<br>
	fread((char*)str.data(), strSize, 1, fp);<br>
	fclose(fp);<br>
	<br>
	std::vector&lt;Buffer&gt; lines = Pattern::split("[\r\n]+", str, true);<br>
	<br>
	</code>

	\param pattern	The regular expression.
	\param replace	The string to split.
	\param keepEmptys	Whether or not to keep empty strings.
	\param limit	The maximum number of splits to make.
	\param mode	The special mode requested of the <code>Pattern</code> 											during the split process
	\returns All substrings of <code>str</code> split across <code>pattern</code>.
	 */

	static BufferArray split(
		const	Buffer &	pattern,
		const	Buffer &	str,
		const	Bool		keepEmptys = 0,
		const	Uint32		limit = 0,
		const	Uint32		mode = 0
	);

	/**
	Finds all the instances of the specified pattern within the string. You
	should be careful to only pass patterns with a minimum length of one. For
	example, the pattern <code>a*</code> can be matched by an empty string, so
	instead you should pass <code>a+</code> since at least one character must
	be matched. A typical invocation of <code>findAll</code> looks like:
	<p>
	<code>
	std::vector&lt;td::string&gt; numbers = Pattern::findAll("\\d+", string);
	</code>
	<p>

	\param pattern	The pattern for which to search
	\param str			The string to search
	\param mode			The special mode requested of the <code>Pattern</code>
									during the find process
	\returns All instances of <code>pattern</code> in <code>str</code>
	 */

	static BufferArray findAll(
		const	Buffer &	pattern,
		const	Buffer &	str,
		const	Uint32		mode = 0
	);

	/**
	Determines if an entire string matches the specified pattern

	\param pattern	The pattern for to match
	\param str			The string to match
	\param mode			The special mode requested of the <code>Pattern</code>
									during the replacement process
	\returns True if <code>str</code> is recognized by <code>pattern</code>
	 */

	static Bool matches(
		const	Buffer &	pattern,
		const	Buffer &	str,
		const	Uint32		mode = 0
	);

	/**
	Registers a pattern under a specific name for use in later compilations.
	A typical invocation and later use looks like:
	<p>
	<code>
	Pattern::registerPattern("ip", "(?:\\d{1,3}\\.){3}\\d{1,3}");<br>
	Pattern * p1 = Pattern::compile("{ip}:\\d+");<br>
	Pattern * p2 = Pattern::compile("Connection from ({ip}) on port \\d+");<br>
	</code>
	<p>
	Multiple calls to <code>registerPattern</code> with the same
	<code>name</code> will result in the pattern getting overwritten.

	\param name			The name to give to the pattern
	\param pattern	The pattern to register
	\param mode			Any special flags to use when compiling pattern
	\returns Success/Failure. Fails only if <code>pattern</code> has invalid
					syntax
	 */

	static void registerPattern(
		const	Buffer &	name,
		const	Buffer &	pattern,
		const	Uint32		mode = 0
	);

	/// \brief Clears the pattern registry.

	static void unregisterPatterns(
	);

	/// \brief Searches through a string for the \a matchNum <sup>th</sup>
	///	match of the given pattern in the string.
	///
	/// Match indeces start at zero, not one.
	///
	/// A typical invocation looks like this:
	/// \code
	///	Uint32	pos;
	///	Buffer	match = Pattern::findNthMatch(
	///		"\\d{1,3}",
	///		"192.168.1.101:22",
	///		1,
	///		pos );
	///	cerr << pos << endl;
	/// \endcode
	///
	/// Output: 4
	/// 
	/// \param pattern	The pattern for which to search
	/// \param str			The string to search
	/// \param matchNum Which match to find
	/// \param mode			Any special flags to use during the matching process
	/// \returns A string and an integer. The string is the string matched. The
	/// 				integer is the starting location of the matched string in
	/// 				<code>str</code>. You can check for success/failure by making sure
	/// 				that the integer returned is greater than or equal to zero.

	static Buffer findNthMatch(
		const	Buffer &	pattern,
		const	Buffer &	str,
		const	Uint32		matchNum,
			Uint32 &	startPos
	);

	static Buffer findNthMatch(
		const	Buffer &	pattern,
		const	Buffer &	str,
		const	Uint32		matchNum,
		const	Uint32		mode,
			Uint32 &	startPos
	);

	BufferArray split(
		const	Buffer &	str,
		const	Bool		keepEmptys = 0,
		const	Uint32		limit = 0
	);

	BufferArray findAll(
		const	Buffer &	str
	);

	Bool matches(
		const	Buffer &	str
	);

	/// \brief Returns the flags used during compilation of this pattern.

	Uint32 getFlags(
	) const;

	/// \brief Returns the regular expression this pattern represents.

	Buffer getPattern(
	) const;

	/// \brief Creates a matcher object using the specified string and
	///	this pattern.

	Matcher * createMatcher(
		const	Buffer &	str
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.RegEx.Pattern" );

	CLASS_EXCEPTION( SyntaxError, "Syntax error" );

protected :

	/// \brief Raises an error during compilation.

	void raiseError(
	);

	/// \brief Calculates the union of two strings.

	Buffer classUnion(
		const	Buffer &	s1,
		const	Buffer &	s2
	) const;

	/// \brief Calculates the intersection of two strings.

	Buffer classIntersect(
		const	Buffer &	s1,
		const	Buffer &	s2
	) const;

	/// \brief Calculates the negation of a string.

	Buffer classNegate(
		const	Buffer &	s1
	) const;

	/// \brief Creates a new "class" representing the range from
	///	\a low thru \a hi.
	///
	/// This function will wrap if /a low &gt; \a hi. This is a feature,
	/// not a bug. Sometimes it is useful to be able to say [\x70-\x10]
	/// instead of [\x70-\x7F\x00-\x10].

	Buffer classCreateRange(
			Uchar		low,
			Uchar		hi
	) const;

	/// \brief Extracts a decimal number from the pattern.

	int getInt(
			int		start,
			int		end
	);

	/// \brief Parses a {n,m} string out of the pattern.
	///
	/// Stores the result in sNum and eNum.
	///
	/// \param sNum
	///	Output parameter. The minimum number of matches required
	///	by the curly quantifier are stored here.
	///
	/// \param eNum
	///	Output parameter. The maximum number of matches allowed
	///	by the curly quantifier are stored here.
	///
	/// \returns
	///	Success/Failure. Fails when the curly does not have the proper
	///	syntax.

	Bool quantifyCurly(
			int &		sNum,
			int &		eNum
	);

	/// \brief Tries to quantify the currently parsed group.
	///
	/// If the group being parsed is indeed quantified in the pattern,
	/// then the NFA is modified accordingly.
	///
	/// \param start
	///	The starting node of the current group being parsed.
	///
	/// \param stop
	///	The ending node of the current group being parsed.
	///
	/// \param gn
	///	The group number of the current group being parsed.
	///
	/// \returns
	///	The node representing the starting node of the group. If the
	///	group becomes quantified, then this node is not necessarily
	/// 	a GroupHead node.

	NFANode * quantifyGroup(
			NFANode *	start,
			NFANode *	stop,
		const	int		gn
	);

	/// \brief Tries to quantify the last parsed expression.
	///
	/// If the character was indeed quantified, then the NFA is modified
	/// accordingly.
	///
	/// \param newNode
	///	The recently created expression node.
	///
	/// \returns
	///	The node representing the last parsed expression. If the
	///	expression was quantified, return value != newNode.

	NFANode * quantify(
			NFANode *	newNode
	);

	/// \brief Parses the current class being examined in pattern.
	///
	/// \returns
	///	A string of unique characters contained in the current class
	///	being parsed.

	Buffer parseClass(
	);

	/// \brief Parses the current POSIX class being examined in pattern.
	///
	/// \returns
	///	A string of unique characters representing the POSIX class
	///	being parsed.

	Buffer parsePosix(
	);

	/// \brief Returns a string containing the octal character being parsed.
	///
	/// \returns
	///	The string contained the octal value being parsed.

	Buffer parseOctal(
	);

	/// \brief Returns a string containing the hex character being parsed.
	///
	/// \returns The string contained the hex value being parsed.

	Buffer parseHex(
	);

	/// \brief Returns a new node representing the back reference being parsed.
	///
	/// \returns The new node representing the back reference being parsed.

	NFANode * parseBackref(
	);

	/**
		Parses the escape sequence currently being examined. Determines if the
		escape sequence is a class, a single character, or the beginning of a
		quotation sequence.
		\param inv Output parameter. Whether or not to invert the returned class
		\param quo Output parameter. Whether or not this sequence starts a
							 quotation.
		\returns The characters represented by the class
	 */

	Buffer parseEscape(
			Bool &		inv,
			Bool &		quo
	);

	/**
		Parses a supposed registered pattern currently under compilation. If the
		sequence of characters does point to a registered pattern, then the
		registered pattern is appended to <code>*end<code>. The registered pattern
		is parsed with the current compilation flags.
		\param end The ending node of the thus-far compiled pattern
		\returns The new end node of the current pattern
	 */

	NFANode * parseRegisteredPattern(
			NFANode **	end
	);

	/**
		Parses a lookbehind expression. Appends the necessary nodes
		<code>*end</code>.
		\param pos Positive or negative look behind
		\param end The ending node of the current pattern
		\returns The new end node of the current pattern
	 */

	NFANode * parseBehind(
		const	Bool		pos,
			NFANode **	end
	);

	/**
		Parses the current expression and tacks on nodes until a \E is found.
		\returns The end of the current pattern
	 */

	NFANode * parseQuote(
	);

	/**
		Parses <code>{@link pattern pattern}</code>. This function is called
		recursively when an or (<code>|</code>) or a group is encountered.
		\param inParen Are we currently parsing inside a group
		\param inOr Are we currently parsing one side of an or (<code>|</code>)
		\param end The end of the current expression
		\returns The starting node of the NFA constructed from this parse
	 */

	NFANode * parse(
		const	Bool		inParen = 0,
		const	Bool		inOr = 0,
			NFANode **	end = 0
	);

public :

	Uint32 getGroupCount(
	) const {
		return groupCount;
	}

	Uint32 getNonCapGroupCount(
	) const {
		return nonCapCount;
	}

	NFANode * getHead(
	) const {
		return head;
	}

public :

	Matcher * getMatcher(
	) {
		if ( ! matcher1 ) {
			matcher1 = new Matcher( this, "" );
		}
		return matcher1;
	}

private :

	typedef Pair< Buffer, Uint32 >		RegPatternPair;
	typedef Map< Buffer, RegPatternPair >	RegPatternMap;

	/// Holds all of the registered patterns as strings. Due to certain problems
	/// with compilation of patterns, especially with capturing groups, this seemed
	/// to be the best way to do it.

	static RegPatternMap	registeredPatterns;

	Matcher *	matcher1;	///< Used when methods like split are called.
	NFANode *	head;		///< The front node of the NFA.
	Buffer		pattern;	///< The actual regular expression we represent.
	Uint32		curInd;		///< Current index into the pattern.
	Uint32		groupCount;	///< The number of capture groups this contains.
	Uint32		nonCapCount;	///< The number of non-capture groups this contains.
	Uint32		flags;		///< Flags.

	/// \brief Forbidden copy constructor.

	Pattern(
		const	Pattern&
	);

	/// \brief Forbidden copy operator.

	Pattern& operator = (
		const	Pattern&
	);

};

// ============================================================================

class NFANode {

public :

	NFANode(
	) : next( 0 ), groupHeadNode( false ), startOfInput( false ) {
	}

	virtual ~NFANode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const = 0;

	Bool isGroupHeadNode(
	) const {
		return groupHeadNode;
	}

	Bool isStartOfInputNode(
	) const {
		return startOfInput;
	}

	NFANode *	next;

protected :

	Bool		groupHeadNode;
	Bool		startOfInput;

};

// ============================================================================

class NFACharNode : public NFANode {

public :

	NFACharNode(
		const	char		c
	) : ch( c ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

protected :

	char ch;

};

// ============================================================================

class NFACICharNode : public NFANode {

public :

	NFACICharNode(
		const	char		c
	);

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

protected :

	char ch;

};

// ============================================================================

class NFAStartNode : public NFANode {

public :

	NFAStartNode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAEndNode : public NFANode {

public :

	NFAEndNode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAQuantifierNode : public NFANode {

public :

	NFAQuantifierNode(
			NFANode *	internal,
		const	Uint32		minMatch = Pattern::MIN_QMATCH,
		const	Uint32		maxMatch = Pattern::MAX_QMATCH
	);

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Uint32		min;
	Uint32		max;
	NFANode *	inner;

};

// ============================================================================

class NFAGreedyQuantifierNode : public NFAQuantifierNode {

public :

	NFAGreedyQuantifierNode(
			NFANode *	internal,
		const	Uint32		minMatch = Pattern::MIN_QMATCH,
		const	Uint32		maxMatch = Pattern::MAX_QMATCH
	) : NFAQuantifierNode( internal, minMatch, maxMatch ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	virtual int matchInternal(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd,
		const	Uint32		soFar
	) const;

};

// ============================================================================

class NFALazyQuantifierNode : public NFAQuantifierNode {

public :

	NFALazyQuantifierNode(
			NFANode *	internal,
		const	Uint32		minMatch = Pattern::MIN_QMATCH,
		const	Uint32		maxMatch = Pattern::MAX_QMATCH
	) : NFAQuantifierNode( internal, minMatch, maxMatch ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAPossessiveQuantifierNode : public NFAQuantifierNode {

public :

	NFAPossessiveQuantifierNode(
			NFANode *	internal,
		const	Uint32		minMatch = Pattern::MIN_QMATCH,
		const	Uint32		maxMatch = Pattern::MAX_QMATCH
	) : NFAQuantifierNode( internal, minMatch, maxMatch ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAAcceptNode : public NFANode {

public :

	NFAAcceptNode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAClassNode : public NFANode {

public :

	NFAClassNode(
		const	Bool		invert = false
	);

	NFAClassNode(
		const	Buffer &	clazz,
		const	Bool		invert
	);

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Uchar		vals[ 0x100 ];

};

// ============================================================================

class NFACIClassNode : public NFANode {

public :

	NFACIClassNode(
		const	Bool		invert = false
	);

	NFACIClassNode(
		const	Buffer &	clazz,
		const	Bool		invert
	);

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Uchar		vals[ 0x100 ];

};

// ============================================================================

class NFASubStartNode : public NFANode {

public :

	NFASubStartNode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAOrNode : public NFANode {

public :

	NFAOrNode(
			NFANode *	first,
			NFANode *	second
	) : one( first ), two( second ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	NFANode *	one;
	NFANode *	two;

};

// ============================================================================

class NFAQuoteNode : public NFANode {

public :

	NFAQuoteNode(
		const	Buffer &	quoted
	) : qStr( quoted ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Buffer qStr;

};

// ============================================================================

class NFACIQuoteNode : public NFANode {

public :

	NFACIQuoteNode(
		const	Buffer &	quoted
	) : qStr( quoted ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Buffer qStr;

};

// ============================================================================

class NFALookAheadNode : public NFANode {

public :

	NFALookAheadNode(
			NFANode *	internal,
		const	Bool		positive
	) : pos( positive ), inner( internal ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Bool		pos;
	NFANode *	inner;

};

// ============================================================================

class NFALookBehindNode : public NFANode {

public :

	NFALookBehindNode(
		const	Buffer &	str,
		const	Bool		positive
	) : mStr( str ), pos( positive ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Buffer		mStr;
	Bool		pos;

};

// ============================================================================

class NFAStartOfLineNode : public NFANode {

public :

	NFAStartOfLineNode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAEndOfLineNode : public NFANode {

public :

	NFAEndOfLineNode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAReferenceNode : public NFANode {

public :

	NFAReferenceNode(
		const	int		groupIndex
	) : gi( groupIndex ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	int gi;

};

// ============================================================================

class NFAStartOfInputNode : public NFANode {

public :

	NFAStartOfInputNode(
	) {
		startOfInput = true;
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAEndOfInputNode : public NFANode {

public :

	NFAEndOfInputNode(
		const	Bool		lookForTerm
	) : term( lookForTerm ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Bool term;

};

// ============================================================================

class NFAWordBoundaryNode : public NFANode {

public :

	NFAWordBoundaryNode(
		const	Bool		positive
	) : pos( positive ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	Bool pos;

};

// ============================================================================

class NFAEndOfMatchNode : public NFANode {

public :

	NFAEndOfMatchNode(
	) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

};

// ============================================================================

class NFAGroupHeadNode : public NFANode {

public :

	NFAGroupHeadNode(
		const	int		groupIndex
	) : gi( groupIndex ) {
		groupHeadNode = true;
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	int gi;

};

// ============================================================================

class NFAGroupTailNode : public NFANode {

public :

	NFAGroupTailNode(
		const	int		groupIndex
	) : gi( groupIndex ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	int gi;

};

// ============================================================================

class NFAGroupLoopPrologueNode : public NFANode {

public :

	NFAGroupLoopPrologueNode(
		const	int		groupIndex
	) : gi( groupIndex ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	int gi;

};

// ============================================================================

class NFAGroupLoopNode : public NFANode {

public :

	NFAGroupLoopNode(
			NFANode *	internal,
		const	Uint32		minMatch,
		const	Uint32		maxMatch,
		const	int		groupIndex,
		const	int		matchType
	) :
		inner( internal ),
		my_min( minMatch ),
		my_max( maxMatch ),
		gi( groupIndex ),
		type( matchType ) {
	}

	virtual int match(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	int matchGreedy(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	int matchLazy(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	int matchPossessive(
		const	Buffer &	str,
			Matcher *	matcher,
		const	Uint32		curInd = 0
	) const;

	NFANode *	inner;
	int		my_min;
	int		my_max;
	int		gi;
	int		type;

};

// ============================================================================

} // namespace RegEx
} // namespace BFC

// ============================================================================

#endif // _BFC_RegEx_Pattern_H_

// ============================================================================

