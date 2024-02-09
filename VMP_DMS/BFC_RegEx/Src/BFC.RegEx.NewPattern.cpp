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
//	BFC.RegEx.NewPattern.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <ctype.h>
#include <stdio.h>

#include "BFC.Base.System.h"
#include "BFC.Math.Utils.h"
#include "BFC.RegEx.BufferManip.h"
#include "BFC.RegEx.NewMatcher.h"
#include "BFC.RegEx.NewPattern.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( RegEx, NewPattern )

// ============================================================================

RegEx::NewPattern::RegNewPatternMap RegEx::NewPattern::registeredNewPatterns;

// ============================================================================

RegEx::NewPattern::NewPattern() {

	matcher		= 0;
	head		= 0;
	curInd		= 0;
	groupCount	= 0;
	nonCapCount	= 0;
	flags		= 0;

}

RegEx::NewPattern::~NewPattern() {

	kill();

}

// ============================================================================

void RegEx::NewPattern::kill() {

	if ( matcher ) {
		delete matcher;
		matcher = 0;
	}

	NewNFANode * tmpNode;

	while ( head ) {
		tmpNode = head->next;
		delete head;
		head = tmpNode;
	}

	pattern.kill();
	head		= 0;
	curInd		= 0;
	groupCount	= 0;
	nonCapCount	= 0;
	flags		= 0;

}

// ============================================================================

void RegEx::NewPattern::compile(
	const	Buffer &	__pattern,
	const	Uint32		__flags ) {

	kill();

	pattern	= __pattern;
	flags	= __flags;

	NewNFANode *	end;

	if ( flags & LITERAL ) {
		head = new NewNFAStartNode;
		if ( flags & CASE_INSENSITIVE ) {
			head->next = new NewNFACIQuoteNode( pattern );
		}
		else {
			head->next = new NewNFAQuoteNode( pattern );
		}
		head->next->next = new NewNFAEndNode;
	}
	else {
		head = parse( 0, 0, &end );
		if ( ! head ) {
			throw SyntaxError();
		}
		if ( ! head->isStartOfInputNode() ) {
			NewNFANode * n = new NewNFAStartNode;
			n->next = head;
			head = n;
		}
		end->next = new NewNFAEndNode;
	}

}

Buffer RegEx::NewPattern::replace(
	const	Buffer &	originText,
	const	Buffer &	replaceText ) {

	Uint32	li = 0;
	Buffer	ret;

	getNewMatcher()->setString( originText );

	while ( getNewMatcher()->findNextMatch() ) {
		ret += originText( li, getNewMatcher()->getStartingIndex() - li );
		ret += getNewMatcher()->replaceWithGroups( replaceText );
		li = getNewMatcher()->getEndingIndex();
	}

	ret += originText( li );

	return ret;

}

// ============================================================================

void RegEx::NewPattern::raiseError() {

	throw SyntaxError( "OK up to \"" + pattern + "\"!" );

}

// ============================================================================

Buffer RegEx::NewPattern::classUnion(
	const	Buffer &	s1,
	const	Buffer &	s2 ) const {

	return BufferManip::getUnion( s1, s2 );

}

Buffer RegEx::NewPattern::classIntersect(
	const	Buffer &	s1,
	const	Buffer &	s2 ) const {

	return BufferManip::getIntersection( s1, s2 );

}

Buffer RegEx::NewPattern::classNegate(
	const	Buffer &	s1 ) const {

	Uchar		out[ 256 ];
	Uint32		ind = 0;
	Uchar		map[ 256 ];
	const Uchar *	ptr = s1.getCstPtr();
	Uint32		len = s1.getLength();

	System::bzero( map, 256 );

	while ( len-- ) {
		map[ *ptr++ ] = 1;
	}

	len = 256;

	while ( len-- ) {
		if ( ! map[ len ] ) {
			out[ ind++ ] = ( Uchar )len;
		}
	}

	return Buffer( out, ind, true );

}

Buffer RegEx::NewPattern::classCreateRange(
		Uchar		low,
		Uchar		hi ) const {

	Uchar		out[ 256 ];
	Uint32		ind = 0;

	while ( low != hi ) {
		out[ ind++ ] = low++;
	}

	out[ ind++ ] = hi;

	return Buffer( out, ind, true );

}

// ============================================================================

int RegEx::NewPattern::getInt(int start, int end)
{
	int ret = 0;
	for (; start <= end; ++start) ret = ret * 10 + (pattern[start] - '0');
	return ret;
}
Bool RegEx::NewPattern::quantifyCurly(int & sNum, int & eNum)
{
	Bool	good = 1;
	Uint32	i, ci = curInd + 1;
	Uint32	commaInd = ci, endInd = ci;
	Uint32	len = pattern.getLength();

	sNum = eNum = 0;

	while ( endInd < len && pattern[ endInd ] != '}' )
		++endInd;
	while (commaInd < endInd	&& pattern[commaInd] != ',') ++commaInd;
	if (endInd >= len) { raiseError(); return 0; }
	for (i = ci; good && i < endInd; ++i) if (i != commaInd && !isdigit(pattern[i])) good = 0;
	if (!good && commaInd < endInd) { raiseError(); return 0; }
	if (!good) return 0;
	/* so now everything in here is either a comma (and there is at most one comma) or a digit */
	if (commaInd == ci) // {,*}
	{
		if (endInd == commaInd + 1)		 { sNum = MIN_QMATCH;								eNum = MAX_QMATCH;												} // {,} = *
		else													 { sNum = MIN_QMATCH;								eNum = getInt(commaInd + 1, endInd - 1);	} // {,+}
	}
	else if (commaInd == endInd - 1) { sNum = getInt(ci, commaInd - 1); eNum = MAX_QMATCH;												} // {+,}
	else if (commaInd == endInd)		 { sNum = getInt(ci, endInd - 1);		eNum = sNum;															} // {+}
	else														 { sNum = getInt(ci, commaInd - 1); eNum = getInt(commaInd + 1, endInd - 1);	} // {+,+}
	curInd = endInd + 1;
	return 1;
}

RegEx::NewNFANode * RegEx::NewPattern::quantifyGroup(
	RegEx::NewNFANode * start, RegEx::NewNFANode * stop, const int gn)
{
	NewNFANode * newNode = 0;
	int type = 0;

	if (curInd < pattern.getLength())
	{
		char ch = ( char )( (curInd + 1 >= pattern.getLength()) ? -1 : pattern[curInd + 1] );
		switch (pattern[curInd])
		{
		case '*':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; type = 1; break;
			case '+': ++curInd; type = 2; break;
			}
			newNode = new NewNFAGroupLoopPrologueNode(gn);
			newNode->next = new NewNFAGroupLoopNode(start, MIN_QMATCH, MAX_QMATCH, gn, type);
			stop->next = newNode->next;
			return newNode;
		case '?':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; type = 1; break;
			case '+': ++curInd; type = 2; break;
			}
			newNode = new NewNFAGroupLoopPrologueNode(gn);
			newNode->next = new NewNFAGroupLoopNode(start, MIN_QMATCH, 1, gn, type);
			stop->next = newNode->next;
			return newNode;
		case '+':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; type = 1; break;
			case '+': ++curInd; type = 2; break;
			}
			newNode = new NewNFAGroupLoopPrologueNode(gn);
			newNode->next = new NewNFAGroupLoopNode(start, 1, MAX_QMATCH, gn, type);
			stop->next = newNode->next;
			return newNode;
		case '{':
			{
				int s, e;
				if (quantifyCurly(s, e))
				{
					ch = ( char )( (curInd < pattern.getLength()) ? pattern[curInd] : -1 );
					switch (ch)
					{
					case '?': ++curInd; type = 1; break;
					case '+': ++curInd; type = 2; break;
					}
					newNode = new NewNFAGroupLoopPrologueNode(gn);
					newNode->next = new NewNFAGroupLoopNode(start, s, e, gn, type);
					stop->next = newNode->next;
					return newNode;
				}
			}
		default:
			break;
		}
	}
	return 0;
}

RegEx::NewNFANode * RegEx::NewPattern::quantify(RegEx::NewNFANode * newNode)
{
	if (curInd < pattern.getLength())
	{
		char ch = ( char )( (curInd + 1 >= pattern.getLength()) ? -1 : pattern[curInd + 1] );
		switch (pattern[curInd])
		{
		case '*':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; newNode = new NewNFALazyQuantifierNode( newNode, MIN_QMATCH, MAX_QMATCH); break;
			case '+': ++curInd; newNode = new NewNFAPossessiveQuantifierNode( newNode, MIN_QMATCH, MAX_QMATCH); break;
			default:						newNode = new NewNFAGreedyQuantifierNode( newNode, MIN_QMATCH, MAX_QMATCH); break;
			}
			break;
		case '?':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; newNode = new NewNFALazyQuantifierNode( newNode, MIN_QMATCH, 1); break;
			case '+': ++curInd; newNode = new NewNFAPossessiveQuantifierNode( newNode, MIN_QMATCH, 1); break;
			default:						newNode = new NewNFAGreedyQuantifierNode( newNode, MIN_QMATCH, 1); break;
			}
			break;
		case '+':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; newNode = new NewNFALazyQuantifierNode( newNode, 1, MAX_QMATCH); break;
			case '+': ++curInd; newNode = new NewNFAPossessiveQuantifierNode( newNode, 1, MAX_QMATCH); break;
			default:						newNode = new NewNFAGreedyQuantifierNode( newNode, 1, MAX_QMATCH); break;
			}
			break;
		case '{':
			{
				int s, e;
				if (quantifyCurly(s, e))
				{
					ch = ( char )( (curInd < pattern.getLength()) ? pattern[curInd] : -1 );
					switch (ch)
					{
					case '?': ++curInd; newNode = new NewNFALazyQuantifierNode( newNode, s, e); break;
					case '+': ++curInd; newNode = new NewNFAPossessiveQuantifierNode( newNode, s, e); break;
					default:						newNode = new NewNFAGreedyQuantifierNode( newNode, s, e); break;
					}
				}
			}
			break;
		default:
			break;
		}
	}
	return newNode;
}
Buffer RegEx::NewPattern::parseClass()
{
	Buffer t, ret = "";
	char ch, c1, c2;
	Bool inv = 0, neg = 0, quo = 0;

	if (curInd < pattern.getLength() && pattern[curInd] == '^')
	{
		++curInd;
		neg = 1;
	}
	while (curInd < pattern.getLength() && pattern[curInd] != ']')
	{
		ch = pattern[curInd++];
		if (ch == '[')
		{
			t = parseClass();
			ret = classUnion(ret, t);
		}
		/*else if (ch == '-')
		{
			raiseError();
			curInd = pattern.getLength();
		}*/
		else if (ch == '&' && curInd < pattern.getLength() && pattern[curInd] == '&')
		{
			if (pattern[++curInd] != '[')
			{
				raiseError();
				curInd = pattern.getLength();
			}
			else
			{
				++curInd;
				t = parseClass();
				ret = classIntersect(ret, t);
			}
		}
		else if (ch == '\\')
		{
			t = parseEscape(inv, quo);
			if (quo)
			{
				raiseError();
				curInd = pattern.getLength();
			}
			else if (inv || t.getLength() > 1) // cant be part of a range (a-z)
			{
				if (inv) t = classNegate(t);
				ret = classUnion(ret, t);
			}
			else if (curInd < pattern.getLength() && pattern[curInd] == '-') // part of a range (a-z)
			{
				c1 = t[0];
				++curInd;
				if (curInd >= pattern.getLength()) raiseError();
				else
				{
					c2 = pattern[curInd++];
					if (c2 == '\\')
					{
						t = parseEscape(inv, quo);
						if (quo)
						{
							raiseError();
							curInd = pattern.getLength();
						}
						else if (inv || t.getLength() > 1) raiseError();
						else ret = classUnion(ret, classCreateRange(c1, c2));
					}
					else if (c2 == '[' || c2 == ']' || c2 == '-' || c2 == '&')
					{
						raiseError();
						curInd = pattern.getLength();
					}
					else ret = classUnion(ret, classCreateRange(c1, c2));
				}
			}
			else
			{
				ret = classUnion(ret, t);
			}
		}
		else if (curInd < pattern.getLength() && pattern[curInd] == '-')
		{
			c1 = ch;
			++curInd;
			if (curInd >= pattern.getLength()) raiseError();
			else
			{
				c2 = pattern[curInd++];
				if (c2 == '\\')
				{
					t = parseEscape(inv, quo);
					if (quo)
					{
						raiseError();
						curInd = pattern.getLength();
					}
					else if (inv || t.getLength() > 1) raiseError();
					else ret = classUnion(ret, classCreateRange(c1, c2));
				}
				else if (c2 == '[' || c2 == ']' || c2 == '-' || c2 == '&')
				{
					raiseError();
					curInd = pattern.getLength();
				}
				else
				{
					ret = classUnion(ret, classCreateRange(c1, c2));
				}
			}
		}
		else
		{
			ret += " ";
			ret[ret.getLength() - 1] = ch;
		}
	}
	if (curInd >= pattern.getLength() || pattern[curInd] != ']')
	{
		raiseError();
		ret = "";
	}
	else
	{
		++curInd;
		if (neg) ret = classNegate(ret);
	}
	return ret;
}
Buffer RegEx::NewPattern::parsePosix()
{
	Buffer s7 = pattern(curInd, 7);
	if (s7 == "{Lower}")	{ curInd += 7; return "abcdefghijklmnopqrstuvwxyz";																																				}
	if (s7 == "{Upper}")	{ curInd += 7; return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";																																				}
	if (s7 == "{Alpha}")	{ curInd += 7; return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";																							}
	if (s7 == "{Digit}")	{ curInd += 7; return "0123456789";																																												}
	if (s7 == "{Alnum}")	{ curInd += 7; return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";																		}
	if (s7 == "{Punct}")	{ curInd += 7; return "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";																																}
	if (s7 == "{Graph}")	{ curInd += 7; return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"; }
	if (s7 == "{Print}")	{ curInd += 7; return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"; }
	if (s7 == "{Blank}")	{ curInd += 7; return " \t";																																															}
	if (s7 == "{Space}")	{ curInd += 7; return " \t\n\x0B\f\r";																																										}
	if (s7 == "{Cntrl}")
	{
		int i;
		Buffer s = " ";

		for (i = 0; i < 5; ++i) s += s;
		s += " ";
		for (i = 0; i <= 0x1F; ++i) s[i] = ( Uchar )i;
		s[0x20] = 0x7F;
		curInd += 7;
		return s;
	}
	if (s7 == "{ASCII}")
	{
		Buffer s(0x80, ' ');
		for (int i = 0; i < 0x80; ++i) s[i] = ( Uchar )i;
		curInd += 7;
		return s;
	}
	if (pattern(curInd, 8) == "{XDigit}") { curInd += 8; return "abcdefABCDEF0123456789"; }
	raiseError();
	return "";
}
RegEx::NewNFANode * RegEx::NewPattern::parseBackref()
{
	#define is_dig(x) ((x) >= '0' && (x) <= '9')
	#define to_int(x) ((x) - '0')
	Uint32	ci = curInd;
	Uint32	ref = 0;

	while (ci < pattern.getLength() && is_dig(pattern[ci]) && (ref < 10 || ref < groupCount))
	{
		ref = ref * 10 + to_int(pattern[ci++]);
	}
	if (ci == pattern.getLength())
	{
		++ci;
	}
	if ( /* oldRef < 0 || */ ci <= curInd)
	{
		raiseError();
		return new NewNFAReferenceNode(-1);
	}
	curInd = ci;
	return new NewNFAReferenceNode(ref);

	#undef is_dig
	#undef to_int
}
Buffer RegEx::NewPattern::parseOctal()
{
	#define islowoc(x)	((x) >= '0' && (x) <= '3')
	#define isoc(x)			((x) >= '0' && (x) <= '7')
	#define fromoc(x)		((x) - '0')
	Uint32 ci = curInd;
	char ch1 = ( char )( (ci + 0 < pattern.getLength()) ? pattern[ci + 0] : -1 );
	char ch2 = ( char )( (ci + 1 < pattern.getLength()) ? pattern[ci + 1] : -1 );
	char ch3 = ( char )( (ci + 2 < pattern.getLength()) ? pattern[ci + 2] : -1 );
	Buffer s = " ";

	if (islowoc(ch1) && isoc(ch2))
	{
		curInd += 2;
		s[0] = ( Uchar )( fromoc(ch1) * 8 + fromoc(ch2) );
		if (isoc(ch3))
		{
			++curInd;
			s[0] = ( Uchar )( s[0] * 8 + fromoc(ch3) );
		}
	}
	else if (isoc(ch1) && isoc(ch2))
	{
		curInd += 2;
		s[0] = ( Uchar )( fromoc(ch1) * 8 + fromoc(ch2) );
	}
	else raiseError();

	return s;
	#undef islowoc
	#undef isoc
	#undef fromoc
}
Buffer RegEx::NewPattern::parseHex()
{
	#define to_low(x)		(((x) >= 'A' && (x) <= 'Z') ? ((x) - 'A' + 'a') : (x))
	#define is_dig(x)		((x) >= '0' && (x) <= '9')
	#define is_hex(x)		(is_dig(x) || (to_low(x) >= 'a' && to_low(x) <= 'f'))
	#define to_int(x)		((is_dig(x)) ? ((x) - '0') : (to_low(x) - 'a' + 10))

	Uint32 ci = curInd;
	char ch1 = ( char )( (ci + 0 < pattern.getLength()) ? pattern[ci + 0] : -1 );
	char ch2 = ( char )( (ci + 1 < pattern.getLength()) ? pattern[ci + 1] : -1 );
	Buffer s = " ";

	if (is_hex(ch1) && is_hex(ch2))
	{
		curInd += 2;
		s[0] = ( Uchar )( (to_int(ch1) << 4 & 0xF0) | (to_int(ch2) & 0x0F) );
	}

	return s;
	#undef to_low
	#undef is_dig
	#undef is_hex
	#undef to_int
}
Buffer RegEx::NewPattern::parseEscape(Bool & inv, Bool & quo)
{
	char ch = pattern[curInd++];
	Buffer classes = "";

	if (curInd > pattern.getLength())
	{
		raiseError();
		return 0;
	}

	quo = 0;
	inv = 0;
	switch (ch)
	{
	case 'p': classes = parsePosix();																													break;
	case 'P': classes = "!!"; classes += parsePosix();																				break;
	case 'd': classes = "0123456789";																													break;
	case 'D': classes = "!!0123456789";																												break;
	case 's': classes = " \t\r\n\f";																													break;
	case 'S': classes = "!! \t\r\n\f";																												break;
	case 'w': classes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";		break;
	case 'W': classes = "!!abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";	break;
	case '0': classes = parseOctal(); break;
	case 'x': classes = parseHex();		break;

	case 'Q': quo = 1;				break;
	case 't': classes = "\t"; break;
	case 'r': classes = "\r"; break;
	case 'n': classes = "\n"; break;
	case 'f': classes = "\f"; break;
	case 'a': classes = "\a"; break;
	case 'e': classes = "\r"; break;
	default:	classes = " "; classes[0] = ch; break;
	}
	if (classes(0, 2) == "!!")
	{
		classes = classes(2);
		inv = 1;
	}
	return classes;
}

RegEx::NewNFANode * RegEx::NewPattern::parseRegisteredNewPattern(
		NewNFANode **	end ) {

	Uint32 i = curInd;

	while ( i < pattern.getLength() && pattern[ i ] != '}' ) {
		++i;
	}

	if ( i >= pattern.getLength() ) {
		raiseError();
	}

	if ( i == curInd + 1 ) {
		raiseError();
	}

	for ( Uint32 j = curInd ; j < i ; ++j ) {
		Uchar c = pattern[ j ];
		if ( ! ( ( c >= 'a' && c <= 'z' )
		      || ( c >= 'A' && c <= 'Z' )
		      || ( c >= '0' && c <= '9' && j > curInd )
		      || ( c == '_' ) ) ) {
			raiseError();
		}
	}

	NewNFANode *	ret = 0;

	Buffer s = pattern( curInd, i - curInd );

	if ( ! registeredNewPatterns.contains( s ) ) {
		throw SyntaxError( "Unknown registerer pattern \""
			+ s + "\"!" );
	}

	Uint32	oldFlags = flags;
	Buffer	oldPttrn = pattern;
	Uint32	oldIndex = i + 1;

	pattern = registeredNewPatterns[ s ].left();
	flags = registeredNewPatterns[ s ].right();
	curInd = 0;

	--groupCount;

	ret = parse( 0, 0, end );

	pattern = oldPttrn;
	curInd = oldIndex;
	flags = oldFlags;

	return ret;

}

// look behind should interpret everything as a literal (except \\) since the
// pattern must have a concrete length
RegEx::NewNFANode * RegEx::NewPattern::parseBehind(const Bool pos, RegEx::NewNFANode ** end)
{
	Buffer t = "";
	while (curInd < pattern.getLength() && pattern[curInd] != ')')
	{
		char ch = pattern[curInd++];
		t += " ";
		if (ch == '\\')
		{
			if (curInd + 1 >= pattern.getLength())
			{
				raiseError();
				return *end = new NewNFACharNode(' ');
			}
			ch = pattern[curInd++];
		}
		t[t.getLength() - 1] = ch;
	}
	if (curInd >= pattern.getLength() || pattern[curInd] != ')') raiseError();
	else ++curInd;
	return *end = new NewNFALookBehindNode(t, pos);
}
RegEx::NewNFANode * RegEx::NewPattern::parseQuote()
{
	Bool done = 0;
	Buffer s = "";

	while (!done)
	{
		if			(curInd >= pattern.getLength())
		{
			raiseError();
			done = 1;
		}
		else if (pattern(curInd, 2) == "\\E")
		{
			curInd += 2;
			done = 1;
		}
		else if (pattern[curInd] == '\\')
		{
			s += " ";
			s[s.getLength() - 1] = pattern[++curInd];
			++curInd;
		}
		else
		{
			s += " ";
			s[s.getLength() - 1] = pattern[curInd++];
		}
	}
	if ((flags & RegEx::NewPattern::CASE_INSENSITIVE) != 0) return new NewNFACIQuoteNode(s);
	return new NewNFAQuoteNode(s);
}

RegEx::NewNFANode * RegEx::NewPattern::parse(
	const	Bool			inParen,
	const	Bool			inOr,
		RegEx::NewNFANode **	end ) {

	NewNFANode *	start;
	NewNFANode *	cur;
	NewNFANode *	next = 0;
	Buffer		t;
	int		grc = groupCount++;
	Bool		inv;
	Bool		quo;
	Bool		ahead = 0;
	Bool		pos = 0;
	Bool		noncap = 0;
	Bool		indep = 0;
	Uint32		oldFlags = flags;

	if ( inParen ) {
		if ( pattern[ curInd ] == '?' ) {
			++curInd;
			--groupCount;
			if ( pattern[curInd] == ':' ) {
				noncap = 1;
				++curInd;
				grc = --nonCapCount;
			}
			else if ( pattern[ curInd ] == '=' ) {
				++curInd;
				ahead = 1;
				pos = 1;
			}
			else if ( pattern[ curInd ] == '!' ) {
				++curInd;
				ahead = 1;
				pos = 0;
			}
			else if ( pattern( curInd, 2 ) == "<=" ) {
				curInd += 2;
				return parseBehind( 1, end );
			}
			else if ( pattern( curInd, 2 ) == "<!" ) {
				curInd += 2;
				return parseBehind( 0, end );
			}
			else if ( pattern[ curInd ] == '>' ) {
				++curInd;
				indep = 1;
			}
			else {
				Bool	negate = false;
				Bool	done = false;
				while ( ! done ) {
					if ( curInd >= pattern.getLength() ) {
						raiseError();
						return 0;
					}
					else if ( negate ) {
						switch ( pattern[ curInd ] ) {
						case 'i':
							flags &= ~NewPattern::CASE_INSENSITIVE;
							break;
						case 'd':
							flags &= ~NewPattern::UNIX_LINE_MODE;
							break;
						case 'm':
							flags &= ~NewPattern::MULTILINE_MATCHING;
							break;
						case 's':
							flags &= ~NewPattern::DOT_MATCHES_ALL;
							break;
						case ':':
							done = true;
							break;
						case ')':
							++curInd;
							*end = new NewNFALookBehindNode("", true);
							return *end;
						case '-':
						default:
							raiseError();
							return 0;
						}
					}
					else {
						switch ( pattern[ curInd ] ) {
						case 'i':
							flags |= RegEx::NewPattern::CASE_INSENSITIVE;
							break;
						case 'd':
							flags |= RegEx::NewPattern::UNIX_LINE_MODE;
							break;
						case 'm':
							flags |= RegEx::NewPattern::MULTILINE_MATCHING;
							break;
						case 's':
							flags |= RegEx::NewPattern::DOT_MATCHES_ALL;
							break;
						case ':':
							done = true;
							break;
						case '-':
							negate = true;
							break;
						case ')':
							++curInd;
							*end = new NewNFALookBehindNode("", true);
							return *end;
						default:
							raiseError();
							return 0;
						}
					}
					++curInd;
				}
				noncap = 1;
				grc = --nonCapCount;
			}
			if (noncap)
				cur = start = new NewNFAGroupHeadNode(grc);
			else
				cur = start = new NewNFASubStartNode;
		}
		else cur = start = new NewNFAGroupHeadNode(grc);
	}
	else {
		cur = start = new NewNFASubStartNode;
	}

	while (curInd < pattern.getLength()) {
		char ch = pattern[curInd++];
		next = 0;
		switch ( ch ) {
		case '^':
			if ((flags & NewPattern::MULTILINE_MATCHING) != 0)
				next = new NewNFAStartOfLineNode;
			else
				next = new NewNFAStartOfInputNode;
			break;
		case '$':
			if ((flags & NewPattern::MULTILINE_MATCHING) != 0)
				next = new NewNFAEndOfLineNode;
			else
				next = new NewNFAEndOfInputNode(0);
			break;
		case '|':
			--groupCount;
			cur->next = new NewNFAAcceptNode;
			cur = start = new NewNFAOrNode(start, parse(inParen, 1));
			break;
		case '\\':
			if (curInd < pattern.getLength()) {
				Bool eoi = 0;
				switch ( pattern[curInd] ) {
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					next = parseBackref();
					break;
				case 'A':
					++curInd; next = new NewNFAStartOfInputNode;
					break;
				case 'B':
					++curInd; next = new NewNFAWordBoundaryNode(0);
					break;
				case 'b':
					++curInd; next = new NewNFAWordBoundaryNode(1);
					break;
				case 'G':
					++curInd; next = new NewNFAEndOfMatchNode;
					break;
				case 'Z':
					eoi = 1;
				case 'z':
					++curInd; next = new NewNFAEndOfInputNode(eoi);
					break;
				default:
					t = parseEscape(inv, quo);
					if (!quo)
					{
						if (t.getLength() > 1 || inv)
						{
							if ((flags & RegEx::NewPattern::CASE_INSENSITIVE) != 0)
								next = new NewNFACIClassNode(t, inv);
							else
								next = new NewNFAClassNode(t, inv);
						}
						else
						{
							next = new NewNFACharNode(t[0]);
						}
					}
					else
					{
						next = parseQuote();
					}
				}
			}
			else
				raiseError();
			break;
		case '[':
			if ( hasFlag( CASE_INSENSITIVE ) ) {
				next = new NewNFACIClassNode( parseClass(), false );
			}
			else {
				next = new NewNFAClassNode( parseClass(), false );
			}
			break;
		case '.':
			next = new NewNFADotNode(
				hasFlag( UNIX_LINE_MODE ) || hasFlag( DOT_MATCHES_ALL ),
				hasFlag( DOT_MATCHES_ALL ) );
			break;
		case '(':
			{
				NewNFANode * tmp, * t1, * t2;
				t1 = parse(1, 0, &tmp);
				if (!t1) raiseError();
				else if (t1->isGroupHeadNode() && (t2 = quantifyGroup(t1, tmp, grc)) != 0)
				{
					cur->next = t2;
					cur = t2->next;
				}
				else
				{
					cur->next = t1;
					cur = tmp;
				}
			}
			break;
		case ')':
			if (!inParen) raiseError();
			else if (inOr)
			{
				--curInd;
				cur = cur->next = new NewNFAAcceptNode;
				flags = oldFlags;
				return start;
			}
			else
			{
				if (ahead)
				{
					cur = cur->next = new NewNFAAcceptNode;
					flags = oldFlags;
					return *end = new NewNFALookAheadNode(start, pos);
				}
				else if (indep)
				{
					cur = cur->next = new NewNFAAcceptNode;
					flags = oldFlags;
					return *end = new NewNFAPossessiveQuantifierNode( start, 1, 1);
				}
				else // capping or noncapping, it doesnt matter
				{
					*end = cur = cur->next = new NewNFAGroupTailNode(grc);
					next = quantifyGroup(start, *end, grc);
					if (next)
					{
						start = next;
						*end = next->next;
					}
					flags = oldFlags;
					return start;
				}
			}
			break;
		case '{': // registered pattern
			cur->next = parseRegisteredNewPattern(&next);
			if (cur->next) cur = next;
			break;
		case '*':
		case '+':
		case '?':
		case '}':
		case ']':
			raiseError();
			break;
		default:
			if ((flags & RegEx::NewPattern::CASE_INSENSITIVE) != 0)
				next = new NewNFACICharNode(ch);
			else
				next = new NewNFACharNode(ch);
			break;
		}
		if (next)
		{
			cur = cur->next = quantify(next);
		}
	}
	if (inParen) raiseError();
	else
	{
		if (inOr) cur = cur->next = new NewNFAAcceptNode;
		if (end) *end = cur;
	}

	flags = oldFlags;
	return start;
}

// ============================================================================

BufferArray RegEx::NewPattern::split(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Bool		keepEmptys,
	const	Uint32		limit,
	const	Uint32		mode ) {

	NewPattern p;

	p.compile( pattern, mode );

	return p.split( str, keepEmptys, limit );

}

BufferArray RegEx::NewPattern::findAll(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		mode ) {

	NewPattern p;

	p.compile( pattern, mode );

	return p.findAll( str );

}

Bool RegEx::NewPattern::matches(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		mode ) {

	NewPattern p;

	p.compile( pattern, mode );

	return p.matches( str );

}

// ============================================================================

void RegEx::NewPattern::registerNewPattern(
	const	Buffer &	name,
	const	Buffer &	pattern,
	const	Uint32		mode ) {

	NewPattern p;

	p.compile( pattern, mode );

	registeredNewPatterns[ name ] = RegNewPatternPair( pattern, mode );

}

void RegEx::NewPattern::unregisterNewPatterns() {

	registeredNewPatterns.kill();

}

// ============================================================================

Buffer RegEx::NewPattern::findNthMatch(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		matchNum,
		Uint32 &	startPos ) {

	Uint32	mode = 0;

	return findNthMatch( pattern, str, matchNum, mode, startPos );

}

Buffer RegEx::NewPattern::findNthMatch(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		matchNum,
	const	Uint32		mode,
		Uint32 &	startPos ) {

	NewPattern p;

	p.compile( pattern, mode );

	Buffer	ret;

	int i = -1;

	p.getNewMatcher()->setString( str );

	while ( i < ( int )matchNum && p.getNewMatcher()->findNextMatch() ) {
		++i;
	}

	if ( i == ( int )matchNum && p.getNewMatcher()->getStartingIndex() >= 0 ) {
		ret = p.getNewMatcher()->getGroup( 0 );
		startPos = ( Uint32 )p.getNewMatcher()->getStartingIndex();
	}

	return ret;

}

BufferArray RegEx::NewPattern::split(const Buffer & str, const Bool keepEmptys, const Uint32 limit)
{
	Uint32 lim = (limit == 0 ? ( Uint32 )MAX_QMATCH : limit);
	int li = 0;
	BufferArray ret;

	getNewMatcher()->setString(str);

	while ( getNewMatcher()->findNextMatch() && ret.getSize() < lim ) {
		if (getNewMatcher()->getStartingIndex() == 0 && keepEmptys)
			ret += "";
		if ((getNewMatcher()->getStartingIndex() != getNewMatcher()->getEndingIndex()) || keepEmptys)
		{
			if (li != getNewMatcher()->getStartingIndex() || keepEmptys)
			{
				ret += str(li, getNewMatcher()->getStartingIndex() - li);
			}
			li = getNewMatcher()->getEndingIndex();
		}
	}
	if (li < (int)str.getLength())
		ret += str(li);

	return ret;
}
BufferArray RegEx::NewPattern::findAll(const Buffer & str)
{
	getNewMatcher()->setString(str);
	return getNewMatcher()->findAll();
}
Bool RegEx::NewPattern::matches(const Buffer & str)
{
	getNewMatcher()->setString(str);
	return getNewMatcher()->matches();
}
Uint32 RegEx::NewPattern::getFlags() const
{
	return flags;
}
Buffer RegEx::NewPattern::getNewPattern() const
{
	return pattern;
}

// ============================================================================

int RegEx::NewNFACharNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( curInd >= str.getLength() ) {
		return -1;
	}

	if ( str[ curInd ] != ch ) {
		return -1;
	}

	return next->match( str, matcher, curInd + 1 );

}

// ============================================================================

RegEx::NewNFACICharNode::NewNFACICharNode(
	const	Uchar		c ) {

	if ( c >= 'A' && c <= 'Z' ) {
		chl = ( Uchar )( c - 'A' + 'a' );
		chu = c;
	}
	else if ( c >= 'a' && c <= 'z' ) {
		chl = c;
		chu = ( Uchar )( c - 'a' + 'A' );
	}
	else {
		chl = chu = c;
	}

}

int RegEx::NewNFACICharNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( curInd >= str.getLength() ) {
		return -1;
	}

	Uchar c = str[ curInd ];

	if ( c == chl || c == chu ) {
		return next->match( str, matcher, curInd + 1 );
	}

	return -1;

}

// ============================================================================

int RegEx::NewNFAStartNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	int	ret = -1;
	Uint32	ci = curInd;

	matcher->starts[ 0 ] = curInd;

	if ( matcher->hasFlag( NewMatcher::MATCH_ENTIRE_STRING ) ) {
		if (curInd != 0)
		{
			matcher->starts[0] = -1;
			return -1;
		}
		return next->match( str, matcher, 0 );
	}

	while ( ( ret = next->match( str, matcher, ci ) ) == -1
	     && ci < str.getLength() ) {
		matcher->clearGroups();
		matcher->starts[ 0 ] = ++ci;
	}

	if ( ret < 0 ) {
		matcher->starts[0] = -1;
	}

	return ret;

}

// ============================================================================

int RegEx::NewNFAEndNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	matcher->ends[ 0 ] = curInd;

	if ( matcher->hasFlag( NewMatcher::MATCH_ENTIRE_STRING ) ) {
		if ( curInd == str.getLength() )
			return ( int )curInd;
		matcher->ends[0] = -1;
		return -1;
	}

	return ( int )curInd;

}

// ============================================================================

RegEx::NewNFAQuantifierNode::NewNFAQuantifierNode(
		NewNFANode *	internal,
	const	Uint32		minMatch,
	const	Uint32		maxMatch ) {

	inner = internal;
	inner->next = new NewNFAAcceptNode;
	min = Math::getMax( ( Uint32 )NewPattern::MIN_QMATCH, minMatch );
	max = Math::getMin( ( Uint32 )NewPattern::MAX_QMATCH, maxMatch );

}

int RegEx::NewNFAQuantifierNode::commonMatch(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	int	i0 = curInd;
	int	i1 = curInd;
	Uint32	i2 = 0;

	while ( i2 < min ) {
		++i2;
		i1 = inner->match( str, matcher, i0 );
		if (i1 <= i0) return i1; // i1 < i0 means i1 is -1
		i0 = i1;
	}

	return i1;

}

// ============================================================================

int RegEx::NewNFAGreedyQuantifierNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int t = commonMatch(str, matcher, curInd);
	if (t != -1) return matchInternal(str, matcher, t, min);
	return t;
}
int RegEx::NewNFAGreedyQuantifierNode::matchInternal(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd,
	const	Uint32		soFar ) const {

	if (soFar >= max) return next->match(str, matcher, curInd);

	int i, j;

	i = inner->match(str, matcher, curInd);
	if (i != -1)
	{
		j = matchInternal(str, matcher, i, soFar + 1);
		if (j != -1) return j;
	}
	return next->match(str, matcher, curInd);
}

// ============================================================================

int RegEx::NewNFALazyQuantifierNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int j, m = commonMatch(str, matcher, curInd);

	Uint32 i;

	if (m == -1) return -1;

	for (i = min; i < max; ++i)
	{
		j = next->match(str, matcher, m);
		if (j == -1)
		{
			j = inner->match(str, matcher, m);
			// if j < m, then j is -1, so we bail.
			// if j == m, then we would just go and call next->match on the same index,
			// but it already failed trying to match right there, so we know we can
			// just bail
			if (j <= m) return -1;
			m = j;
		}
		else return j;
	}
	return next->match(str, matcher, m);
}

// ============================================================================

int RegEx::NewNFAPossessiveQuantifierNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int j, m = commonMatch(str, matcher, curInd);

	Uint32 i;

	if (m == -1) return -1;
	for (i = min; i < max; ++i)
	{
		j = inner->match(str, matcher, m);
		if (j <= m) return next->match(str, matcher, m);
		m = j;
	}
	return next->match(str, matcher, m);
}

// ============================================================================

int RegEx::NewNFAAcceptNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (!next) return curInd;
	else return next->match(str, matcher, curInd);
}

// ============================================================================

int RegEx::NewNFADotNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( curInd >= str.getLength() ) {
		return -1;
	}

	Uchar c = str[ curInd ];

	if ( ( c == '\n' && ! useN )
	  || ( c == '\r' && ! useR ) ) {
		return -1;
	}

	return next->match( str, matcher, curInd + 1 );

}

// ============================================================================

RegEx::NewNFAClassNode::NewNFAClassNode(
	const	Bool		invert ) {

	if ( invert ) {
		System::memset( vals, 0xFF, 0x100 );
	}
	else {
		System::bzero( vals, 0x100 );
	}

}

RegEx::NewNFAClassNode::NewNFAClassNode(
	const	Buffer &	clazz,
	const	Bool		invert ) {

	Uchar mask;

	if ( invert ) {
		System::memset( vals, 0xFF, 0x100 );
		mask = 0x00;
	}
	else {
		System::bzero( vals, 0x100 );
		mask = 0xFF;
	}

	Uint32		len = clazz.getLength();
	const Uchar *	ptr = clazz.getCstPtr();

	while ( len-- ) {
		vals[ *ptr++ ] = mask;
	}

}

int RegEx::NewNFAClassNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( curInd < str.getLength() && vals[ str[ curInd ] ] ) {
		return next->match( str, matcher, curInd + 1 );
	}

	return -1;

}

// ============================================================================

RegEx::NewNFACIClassNode::NewNFACIClassNode(
	const	Bool		invert ) {

	if ( invert ) {
		System::memset( vals, 0xFF, 0x100 );
	}
	else {
		System::bzero( vals, 0x100 );
	}

}

RegEx::NewNFACIClassNode::NewNFACIClassNode(
	const	Buffer &	clazz,
	const	Bool		invert ) {

	Uchar mask;

	if ( invert ) {
		System::memset( vals, 0xFF, 0x100 );
		mask = 0x00;
	}
	else {
		System::bzero( vals, 0x100 );
		mask = 0xFF;
	}

	Uint32		len = clazz.getLength();
	const Uchar *	ptr = clazz.getCstPtr();
	Uchar		c;

	while ( len-- ) {
		c = *ptr++;
		if ( c >= 'A' && c <= 'Z' ) {
			vals[ c ] = vals[ c - 'A' + 'a' ] = mask;
		}
		else if ( c >= 'a' && c <= 'z' ) {
			vals[ c ] = vals[ c - 'a' + 'A' ] = mask;
		}
		else {
			vals[ c ] = mask;
		}
	}

}

int RegEx::NewNFACIClassNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( curInd < str.getLength() && vals[ str[ curInd ] ] ) {
		return next->match( str, matcher, curInd + 1 );
	}

	return -1;

}

// ============================================================================

int RegEx::NewNFASubStartNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	return next->match(str, matcher, curInd);
}

// ============================================================================

int RegEx::NewNFAOrNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int ci = one->match(str, matcher, curInd);

	if (ci != -1) ci = next->match(str, matcher, ci);
	if (ci != -1) return ci;
	if (ci == -1) ci = two->match(str, matcher, curInd);
	if (ci != -1) ci = next->match(str, matcher, ci);
	return ci;
}

// ============================================================================

int RegEx::NewNFAQuoteNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd + qStr.getLength() > str.getLength()) return -1;
	if (str(curInd, qStr.getLength()) != qStr) return -1;
	return next->match(str, matcher, curInd + qStr.getLength());
}

// ============================================================================

int RegEx::NewNFACIQuoteNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( curInd + qStr.getLength() > str.getLength() ) {
		return -1;
	}

	if ( str( curInd, qStr.getLength() ).toLower() != qStr.toLower() ) {
		return -1;
	}

	return next->match( str, matcher, qStr.getLength() );

}

// ============================================================================

int RegEx::NewNFALookAheadNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	return ((inner->match(str, matcher, curInd) == -1) ^ pos)
		? next->match(str, matcher, curInd)
		: -1;
}

// ============================================================================

int RegEx::NewNFALookBehindNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (pos)
	{
		if (curInd < mStr.getLength()) return -1;
		if (str(curInd - mStr.getLength(), mStr.getLength()) == mStr) return next->match(str, matcher, curInd);
	}
	else
	{
		if (curInd < mStr.getLength()) return next->match(str, matcher, curInd);
		if (str(curInd - mStr.getLength(), mStr.getLength()) == mStr) return -1;
		 return next->match(str, matcher, curInd);
	}
	return -1;
}

// ============================================================================

int RegEx::NewNFAStartOfLineNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd == 0 || str[curInd - 1] == '\n' || str[curInd - 1] == '\r')
	{
		return next->match(str, matcher, curInd);
	}
	return -1;
}

// ============================================================================

int RegEx::NewNFAEndOfLineNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd >= str.getLength() || str[curInd] == '\n' || str[curInd] == '\r')
	{
		return next->match(str, matcher, curInd);
	}
	return -1;
}

// ============================================================================

int RegEx::NewNFAReferenceNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int len = matcher->ends[gi] - matcher->starts[gi];
	int ni = -1;
	if			(gi < 1 || matcher->ends[gi] < matcher->starts[gi] || len == 0)		ni = curInd;
	else if (curInd + len > str.getLength())																	return -1;
	else if (str(curInd, len) != str(matcher->starts[gi], len)) return -1;
	else																																			ni = curInd + len;

	return next->match(str, matcher, ni);
}

// ============================================================================

int RegEx::NewNFAStartOfInputNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd == 0) return next->match(str, matcher, curInd);
	return -1;
}

// ============================================================================

int RegEx::NewNFAEndOfInputNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	Uint32 len = str.getLength();
	if			(curInd == len) return next->match(str, matcher, curInd);
	else if (term)
	{
		if			(curInd == len - 1 && (str[curInd] == '\r' || str[curInd] == '\n'))
		{
			return next->match(str, matcher, curInd);
		}
		else if (curInd == len - 2 && str(curInd, 2) == "\r\n")
		{
			return next->match(str, matcher, curInd);
		}
	}
	return -1;
}

// ============================================================================

int RegEx::NewNFAWordBoundaryNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	#define is_alpha(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))

	Uint32 len = str.getLength();
	Bool ok = 0;
	char c1 = ( char )( (curInd - 1 < len) ? str[curInd - 1] : -1 );
	char c2 = ( char )( (curInd     < len) ? str[curInd    ] : -1 );

	if			(curInd == len) return next->match(str, matcher, curInd);
	if			(is_alpha(c1) ^ is_alpha(c2)) ok = 1;
	if (ok && pos) return next->match(str, matcher, curInd);
	return -1;

	#undef is_alpha
}

// ============================================================================

int RegEx::NewNFAEndOfMatchNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd == ( Uint32 )matcher->lm) return next->match(str, matcher, curInd);
	return -1;
}

// ============================================================================

int RegEx::NewNFAGroupHeadNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int ret, o = matcher->starts[gi];

	matcher->starts[gi] = curInd;
	ret = next->match(str, matcher, curInd);
	if (ret < 0) matcher->starts[gi] = o;

	return ret;
}

// ============================================================================

int RegEx::NewNFAGroupTailNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int ret, o = matcher->ends[gi];

	matcher->ends[gi] = curInd;
	ret = next->match(str, matcher, curInd);
	if (ret < 0) matcher->ends[gi] = o;

	return ret;
}

// ============================================================================

int RegEx::NewNFAGroupLoopPrologueNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int ret, o1 = matcher->groups[gi], o2 = matcher->groupPos[gi], o3 = matcher->groupIndeces[gi];

	matcher->groups[gi] = 0;
	matcher->groupPos[gi] = 0;
	matcher->groupIndeces[gi] = -1;
	ret = next->match(str, matcher, curInd);
	if (ret < 0)
	{
		matcher->groups[gi] = o1;
		matcher->groupPos[gi] = o2;
		matcher->groupIndeces[gi] = o3;
	}

	return ret;
}

// ============================================================================

int RegEx::NewNFAGroupLoopNode::match(
	const	Buffer &	str,
		NewMatcher *	matcher,
	const	Uint32		curInd ) const
{
	Bool b = ( curInd > ( Uint32 )matcher->groupIndeces[gi] );

	if (b && matcher->groups[gi] < my_min)
	{
		++matcher->groups[gi];
		int o = matcher->groupIndeces[gi];
		matcher->groupIndeces[gi] = curInd;
		int ret = inner->match(str, matcher, curInd);
		if (ret < 0)
		{
			matcher->groupIndeces[gi] = o;
			--matcher->groups[gi];
		}
		return ret;
	}
	else if (!b || matcher->groups[gi] >= my_max)
	{
		return next->match(str, matcher, curInd);
	}
	else
	{
		switch (type)
		{
		case 0: return matchGreedy(str, matcher, curInd);
		case 1: return matchLazy(str, matcher, curInd);
		case 2: return matchPossessive(str, matcher, curInd);
		}
	}
	return -1;
}
int RegEx::NewNFAGroupLoopNode::matchGreedy(const Buffer & str, NewMatcher * matcher, const Uint32 curInd) const
{
	int o = matcher->groupIndeces[gi];						// save our info for backtracking
	matcher->groupIndeces[gi] = curInd;						// move along
	++matcher->groups[gi];
	int ret = inner->match(str, matcher, curInd); // match internally
	if (ret < 0)
	{																							// if we failed, then restore info and match next
		--matcher->groups[gi];
		matcher->groupIndeces[gi] = o;
		ret = next->match(str, matcher, curInd);
	}
	return ret;
}
int RegEx::NewNFAGroupLoopNode::matchLazy(const Buffer & str, NewMatcher * matcher, const Uint32 curInd) const
{
	int ret = next->match(str, matcher, curInd);	// be lazy, just go on
	if (ret < 0)
	{
		int o = matcher->groupIndeces[gi];					// save info for backtracking
		matcher->groupIndeces[gi] = curInd;					// advance our position
		++matcher->groups[gi];
		ret = inner->match(str, matcher, curInd);		// match our internal stuff
		if (ret < 0)																// if we failed, then restore the info
		{
			--matcher->groups[gi];
			matcher->groupIndeces[gi] = o;
		}
	}
	return ret;
}
int RegEx::NewNFAGroupLoopNode::matchPossessive(const Buffer & str, NewMatcher * matcher, const Uint32 curInd) const
{
	int o = matcher->groupIndeces[gi];						// save info for backtracking
	matcher->groupPos[gi] = matcher->groups[gi];	// set a flag stating we have matcher at least this much
	matcher->groupIndeces[gi] = curInd;						// move along
	++matcher->groups[gi];
	int ret = inner->match(str, matcher, curInd); // try and match again
	if (ret < 0)
	{																							// if we fail, back off, but to an extent
		--matcher->groups[gi];
		matcher->groupIndeces[gi] = o;
		if (matcher->groups[gi] == matcher->groupPos[gi]) ret = next->match(str, matcher, curInd);
	}
	return ret;
}

// ============================================================================

