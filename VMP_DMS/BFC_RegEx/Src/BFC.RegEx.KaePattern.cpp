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
//	BFC.RegEx.KaePattern.cpp
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
#include "BFC.RegEx.KaeMatcher.h"
#include "BFC.RegEx.KaePattern.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( RegEx, KaePattern )

// ============================================================================

RegEx::KaePattern::RegKaePatternMap RegEx::KaePattern::registeredKaePatterns;

// ============================================================================

RegEx::KaePattern::KaePattern() {

	matcher1		= 0;
	head		= 0;
	curInd		= 0;
	groupCount	= 0;
	nonCapCount	= 0;
	flags		= 0;

}

RegEx::KaePattern::~KaePattern() {

	kill();

}

// ============================================================================

void RegEx::KaePattern::kill() {

	if ( matcher1 ) {
		delete matcher1;
		matcher1 = 0;
	}

	KaeNFANode * tmpNode;

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

void RegEx::KaePattern::compile(
	const	Buffer &	__pattern,
	const	Uint32		__flags ) {

	kill();

	pattern	= __pattern;
	flags	= __flags;

	KaeNFANode *	end;

	if ( flags & SEARCH_LITERAL ) {
		head = new KaeNFAStartNode;
		if ( flags & SEARCH_CASE_INSENSITIVE ) {
			head->next = new KaeNFACIQuoteNode( pattern );
		}
		else {
			head->next = new KaeNFAQuoteNode( pattern );
		}
		head->next->next = new KaeNFAEndNode;
	}
	else {
		head = parse( 0, 0, &end );
		if ( ! head ) {
			throw SyntaxError();
		}
		if ( ! head->isStartOfInputNode() ) {
			KaeNFANode * n = new KaeNFAStartNode;
			n->next = head;
			head = n;
		}
		end->next = new KaeNFAEndNode;
	}

//	matcher = new KaeMatcher( this, "" );

}

Buffer RegEx::KaePattern::replace(
	const	Buffer &	originText,
	const	Buffer &	replaceText ) {

	Uint32	li = 0;
	Buffer	ret;
	
	BufferMatchStringPtr bufMatchString = new BufferMatchString(originText);

	getKaeMatcher()->setString( bufMatchString );

	while ( getKaeMatcher()->findNextMatch() ) {
		ret += originText( li, getKaeMatcher()->getStartingIndex() - li );
		ret += getKaeMatcher()->replaceWithGroups( replaceText );
		li = getKaeMatcher()->getEndingIndex();
	}

	ret += originText( li );

	return ret;

}

// ============================================================================

void RegEx::KaePattern::raiseError() {

	throw SyntaxError( "OK up to \"" + pattern + "\"!" );

}

// ============================================================================

Buffer RegEx::KaePattern::classUnion(
	const	Buffer &	s1,
	const	Buffer &	s2 ) const {

	return BufferManip::getUnion( s1, s2 );

}

Buffer RegEx::KaePattern::classIntersect(
	const	Buffer &	s1,
	const	Buffer &	s2 ) const {

	return BufferManip::getIntersection( s1, s2 );

}

Buffer RegEx::KaePattern::classNegate(
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
// KAE: remove \r and \n when not in EOL mode
	Bool useR = ( ( flags & SEARCH_UNIX_LINE_MODE  ) != 0 )
	         || ( ( flags & SEARCH_DOT_MATCHES_ALL ) != 0 );
	Bool useN = ( ( flags & SEARCH_DOT_MATCHES_ALL ) != 0 );
	if ( ! useN )
		map[ ( Uchar )'\n'] = 1;
	if ( ! useR )
		map[ ( Uchar )'\r'] = 1;
// END KAE

	len = 256;

	while ( len-- ) {
		if ( ! map[ len ] ) {
			out[ ind++ ] = ( Uchar )len;
		}
	}

	return Buffer( out, ind, true );

}

Buffer RegEx::KaePattern::classCreateRange(
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

int RegEx::KaePattern::getInt(int start, int end)
{
	int ret = 0;
	for (; start <= end; ++start) ret = ret * 10 + (pattern[start] - '0');
	return ret;
}
Bool RegEx::KaePattern::quantifyCurly(int & sNum, int & eNum)
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

RegEx::KaeNFANode * RegEx::KaePattern::quantifyGroup(
	RegEx::KaeNFANode * start, RegEx::KaeNFANode * stop, const int gn)
{
	KaeNFANode * newNode = 0;
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
			newNode = new KaeNFAGroupLoopPrologueNode(gn);
			newNode->next = new KaeNFAGroupLoopNode(start, MIN_QMATCH, MAX_QMATCH, gn, type);
			stop->next = newNode->next;
			return newNode;
		case '?':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; type = 1; break;
			case '+': ++curInd; type = 2; break;
			}
			newNode = new KaeNFAGroupLoopPrologueNode(gn);
			newNode->next = new KaeNFAGroupLoopNode(start, MIN_QMATCH, 1, gn, type);
			stop->next = newNode->next;
			return newNode;
		case '+':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; type = 1; break;
			case '+': ++curInd; type = 2; break;
			}
			newNode = new KaeNFAGroupLoopPrologueNode(gn);
			newNode->next = new KaeNFAGroupLoopNode(start, 1, MAX_QMATCH, gn, type);
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
					newNode = new KaeNFAGroupLoopPrologueNode(gn);
					newNode->next = new KaeNFAGroupLoopNode(start, s, e, gn, type);
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

RegEx::KaeNFANode * RegEx::KaePattern::quantify(RegEx::KaeNFANode * newNode)
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
			case '?': ++curInd; newNode = new KaeNFALazyQuantifierNode( newNode, MIN_QMATCH, MAX_QMATCH); break;
			case '+': ++curInd; newNode = new KaeNFAPossessiveQuantifierNode( newNode, MIN_QMATCH, MAX_QMATCH); break;
			default:						newNode = new KaeNFAGreedyQuantifierNode( newNode, MIN_QMATCH, MAX_QMATCH); break;
			}
			break;
		case '?':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; newNode = new KaeNFALazyQuantifierNode( newNode, MIN_QMATCH, 1); break;
			case '+': ++curInd; newNode = new KaeNFAPossessiveQuantifierNode( newNode, MIN_QMATCH, 1); break;
			default:						newNode = new KaeNFAGreedyQuantifierNode( newNode, MIN_QMATCH, 1); break;
			}
			break;
		case '+':
			++curInd;
			switch (ch)
			{
			case '?': ++curInd; newNode = new KaeNFALazyQuantifierNode( newNode, 1, MAX_QMATCH); break;
			case '+': ++curInd; newNode = new KaeNFAPossessiveQuantifierNode( newNode, 1, MAX_QMATCH); break;
			default:						newNode = new KaeNFAGreedyQuantifierNode( newNode, 1, MAX_QMATCH); break;
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
					case '?': ++curInd; newNode = new KaeNFALazyQuantifierNode( newNode, s, e); break;
					case '+': ++curInd; newNode = new KaeNFAPossessiveQuantifierNode( newNode, s, e); break;
					default:						newNode = new KaeNFAGreedyQuantifierNode( newNode, s, e); break;
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
Buffer RegEx::KaePattern::parseClass()
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
			if (flags & RegEx::KaePattern::SEARCH_CASE_INSENSITIVE) {
				ret += " ";
				ret[ret.getLength() - 1] = ( unsigned char )tolower(ch);
				ret += " ";
				ret[ret.getLength() - 1] = ( unsigned char )toupper(ch);
			} else {
				ret += " ";
				ret[ret.getLength() - 1] = ch;
			}

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
Buffer RegEx::KaePattern::parsePosix()
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
RegEx::KaeNFANode * RegEx::KaePattern::parseBackref()
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
		return new KaeNFAReferenceNode(-1);
	}
	curInd = ci;
	return new KaeNFAReferenceNode(ref);

	#undef is_dig
	#undef to_int
}
Buffer RegEx::KaePattern::parseOctal()
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
		s[0] = ( unsigned char )( fromoc(ch1) * 8 + fromoc(ch2) );
		if (isoc(ch3))
		{
			++curInd;
			s[0] = ( unsigned char )( s[0] * 8 + fromoc(ch3) );
		}
	}
	else if (isoc(ch1) && isoc(ch2))
	{
		curInd += 2;
		s[0] = ( unsigned char )( fromoc(ch1) * 8 + fromoc(ch2) );
	}
	else raiseError();

	return s;
	#undef islowoc
	#undef isoc
	#undef fromoc
}
Buffer RegEx::KaePattern::parseHex()
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
		s[0] = ( unsigned char )( (to_int(ch1) << 4 & 0xF0) | (to_int(ch2) & 0x0F) );
	}

	return s;
	#undef to_low
	#undef is_dig
	#undef is_hex
	#undef to_int
}
Buffer RegEx::KaePattern::parseEscape(Bool & inv, Bool & quo)
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

RegEx::KaeNFANode * RegEx::KaePattern::parseRegisteredKaePattern(
		KaeNFANode **	end ) {

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

	KaeNFANode *	ret = 0;

	Buffer s = pattern( curInd, i - curInd );

	if ( ! registeredKaePatterns.contains( s ) ) {
		throw SyntaxError( "Unknown registerer pattern \""
			+ s + "\"!" );
	}

	Uint32	oldFlags = flags;
	Buffer	oldPttrn = pattern;
	Uint32	oldIndex = i + 1;

	pattern = registeredKaePatterns[ s ].left();
	flags = registeredKaePatterns[ s ].right();
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
RegEx::KaeNFANode * RegEx::KaePattern::parseBehind(const Bool pos, RegEx::KaeNFANode ** end)
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
				return *end = new KaeNFACharNode(' ');
			}
			ch = pattern[curInd++];
		}
		t[t.getLength() - 1] = ch;
	}
	if (curInd >= pattern.getLength() || pattern[curInd] != ')') raiseError();
	else ++curInd;
	return *end = new KaeNFALookBehindNode(t, pos);
}
RegEx::KaeNFANode * RegEx::KaePattern::parseQuote()
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
	if ((flags & RegEx::KaePattern::SEARCH_CASE_INSENSITIVE) != 0) return new KaeNFACIQuoteNode(s);
	return new KaeNFAQuoteNode(s);
}

RegEx::KaeNFANode * RegEx::KaePattern::parse(
	const	Bool			inParen,
	const	Bool			inOr,
		RegEx::KaeNFANode **	end ) {

	KaeNFANode *	start;
	KaeNFANode *	cur;
	KaeNFANode *	next = 0;
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
							flags &= ~KaePattern::SEARCH_CASE_INSENSITIVE;
							break;
						case 'd':
							flags &= ~KaePattern::SEARCH_UNIX_LINE_MODE;
							break;
						case 'm':
							flags &= ~KaePattern::SEARCH_MULTILINE_MATCHING;
							break;
						case 's':
							flags &= ~KaePattern::SEARCH_DOT_MATCHES_ALL;
							break;
						case ':':
							done = true;
							break;
						case ')':
							++curInd;
							*end = new KaeNFALookBehindNode("", true);
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
							flags |= RegEx::KaePattern::SEARCH_CASE_INSENSITIVE;
							break;
						case 'd':
							flags |= RegEx::KaePattern::SEARCH_UNIX_LINE_MODE;
							break;
						case 'm':
							flags |= RegEx::KaePattern::SEARCH_MULTILINE_MATCHING;
							break;
						case 's':
							flags |= RegEx::KaePattern::SEARCH_DOT_MATCHES_ALL;
							break;
						case ':':
							done = true;
							break;
						case '-':
							negate = true;
							break;
						case ')':
							++curInd;
							*end = new KaeNFALookBehindNode("", true);
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
				cur = start = new KaeNFAGroupHeadNode(grc);
			else
				cur = start = new KaeNFASubStartNode;
		}
		else cur = start = new KaeNFAGroupHeadNode(grc);
	}
	else {
		cur = start = new KaeNFASubStartNode;
	}

	while (curInd < pattern.getLength()) {
		char ch = pattern[curInd++];
		next = 0;
		switch ( ch ) {
		case '^':
			if ((flags & KaePattern::SEARCH_MULTILINE_MATCHING) != 0)
				next = new KaeNFAStartOfLineNode;
			else
				next = new KaeNFAStartOfInputNode;
			break;
		case '$':
			if ((flags & KaePattern::SEARCH_MULTILINE_MATCHING) != 0)
				next = new KaeNFAEndOfLineNode;
			else
				next = new KaeNFAEndOfInputNode(0);
			break;
		case '|':
			--groupCount;
			cur->next = new KaeNFAAcceptNode;
			cur = start = new KaeNFAOrNode(start, parse(inParen, 1));
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
					++curInd; next = new KaeNFAStartOfInputNode;
					break;
				case 'B':
					++curInd; next = new KaeNFAWordBoundaryNode(0);
					break;
				case 'b':
					++curInd; next = new KaeNFAWordBoundaryNode(1);
					break;
				case 'G':
					++curInd; next = new KaeNFAEndOfMatchNode;
					break;
				case 'Z':
					eoi = 1;
				case 'z':
					++curInd; next = new KaeNFAEndOfInputNode(eoi);
					break;
				default:
					t = parseEscape(inv, quo);
					if (!quo)
					{
						if (t.getLength() > 1 || inv)
						{
							if ((flags & RegEx::KaePattern::SEARCH_CASE_INSENSITIVE) != 0)
								next = new KaeNFACIClassNode(t, inv);
							else
								next = new KaeNFAClassNode(t, inv);
						}
						else
						{
							next = new KaeNFACharNode(t[0]);
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
			if ((flags & RegEx::KaePattern::SEARCH_CASE_INSENSITIVE) == 0)
			{
				KaeNFAClassNode * clazz = new KaeNFAClassNode();
				Buffer s = parseClass();
				for (int i = 0; i < (int)s.getLength(); ++i)
					clazz->vals[s[i]] = 0xFF;
				next = clazz;
			}
			else
			{
				KaeNFACIClassNode * clazz = new KaeNFACIClassNode();
				Buffer s = parseClass();
				for (int i = 0; i < (int)s.getLength(); ++i)
					clazz->vals[tolower(s[i])] = 0xFF;
				next = clazz;
			}
			break;
		case '.':
			{
			Bool useR = ( ( flags & SEARCH_UNIX_LINE_MODE  ) != 0 )
			         || ( ( flags & SEARCH_DOT_MATCHES_ALL ) != 0 );
			Bool useN = ( ( flags & SEARCH_DOT_MATCHES_ALL ) != 0 );
			KaeNFAClassNode * clazz = new KaeNFAClassNode( true );
			if ( ! useN )
				clazz->vals[ ( Uchar )'\n'] = 0x00;
			if ( ! useR )
				clazz->vals[ ( Uchar )'\r'] = 0x00;
			next = clazz;
			}
			break;
		case '(':
			{
				KaeNFANode * tmp, * t1, * t2;
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
				cur = cur->next = new KaeNFAAcceptNode;
				flags = oldFlags;
				return start;
			}
			else
			{
				if (ahead)
				{
					cur = cur->next = new KaeNFAAcceptNode;
					flags = oldFlags;
					return *end = new KaeNFALookAheadNode(start, pos);
				}
				else if (indep)
				{
					cur = cur->next = new KaeNFAAcceptNode;
					flags = oldFlags;
					return *end = new KaeNFAPossessiveQuantifierNode( start, 1, 1);
				}
				else // capping or noncapping, it doesnt matter
				{
					*end = cur = cur->next = new KaeNFAGroupTailNode(grc);
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
			cur->next = parseRegisteredKaePattern(&next);
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
			if ((flags & RegEx::KaePattern::SEARCH_CASE_INSENSITIVE) != 0)
				next = new KaeNFACICharNode(ch);
			else
				next = new KaeNFACharNode(ch);
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
		if (inOr) cur = cur->next = new KaeNFAAcceptNode;
		if (end) *end = cur;
	}

	flags = oldFlags;
	return start;
}

// ============================================================================

BufferArray RegEx::KaePattern::split(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Bool		keepEmptys,
	const	Uint32		limit,
	const	Uint32		mode ) {

	KaePattern p;

	p.compile( pattern, mode );
	
	return p.split( str, keepEmptys, limit );

}

BufferArray RegEx::KaePattern::findAll(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		mode ) {

	KaePattern p;

	p.compile( pattern, mode );
	
	BufferMatchStringPtr bms = new BufferMatchString(str);

	return p.findAll( bms );

}

Bool RegEx::KaePattern::matches(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		mode ) {

	KaePattern p;

	p.compile( pattern, mode );
	
	BufferMatchStringPtr bms = new BufferMatchString(str);

	return p.matches( bms );

}

// ============================================================================

void RegEx::KaePattern::registerKaePattern(
	const	Buffer &	name,
	const	Buffer &	pattern,
	const	Uint32		mode ) {

	KaePattern p;

	p.compile( pattern, mode );

	registeredKaePatterns[ name ] = RegKaePatternPair( pattern, mode );

}

void RegEx::KaePattern::unregisterKaePatterns() {

	registeredKaePatterns.kill();

}

// ============================================================================

Buffer RegEx::KaePattern::findNthMatch(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		matchNum,
		Uint32 &	startPos ) {

	Uint32	mode = 0;

	return findNthMatch( pattern, str, matchNum, mode, startPos );

}

Buffer RegEx::KaePattern::findNthMatch(
	const	Buffer &	pattern,
	const	Buffer &	str,
	const	Uint32		matchNum,
	const	Uint32		mode,
		Uint32 &	startPos ) {

	KaePattern p;

	p.compile( pattern, mode );

	BufferMatchStringPtr bms = new BufferMatchString(str);

	Buffer	ret;

	int i = -1;

	p.getKaeMatcher()->setString( bms );

	while ( i < ( int )matchNum && p.getKaeMatcher()->findNextMatch() ) {
		++i;
	}

	if ( i == ( int )matchNum && p.getKaeMatcher()->getStartingIndex() >= 0 ) {
		ret = p.getKaeMatcher()->getGroup( 0 );
		startPos = ( Uint32 )p.getKaeMatcher()->getStartingIndex();
	}

	return ret;

}

BufferArray RegEx::KaePattern::split(const Buffer& str, const Bool keepEmptys, const Uint32 limit)
{
	Uint32 lim = (limit == 0 ? ( Uint32 )MAX_QMATCH : limit);
	int li = 0;
	BufferArray ret;

	BufferMatchStringPtr bms = new BufferMatchString(str);
	getKaeMatcher()->setString(bms);

	while ( getKaeMatcher()->findNextMatch() && ret.getSize() < lim ) {
		if (getKaeMatcher()->getStartingIndex() == 0 && keepEmptys)
			ret += "";
		if ((getKaeMatcher()->getStartingIndex() != getKaeMatcher()->getEndingIndex()) || keepEmptys)
		{
			if (li != getKaeMatcher()->getStartingIndex() || keepEmptys)
			{
				ret += str(li, getKaeMatcher()->getStartingIndex() - li);
			}
			li = getKaeMatcher()->getEndingIndex();
		}
	}
	if ((*bms)[li] != 0)
		ret += str(li);

	return ret;
}
BufferArray RegEx::KaePattern::findAll(const RegEx::MatchStringPtr& str)
{
	getKaeMatcher()->setString(str);
	return getKaeMatcher()->findAll();
}
BufferArray RegEx::KaePattern::findAll(const Buffer& str)
{
	BufferMatchStringPtr bms = new BufferMatchString(str);
	return findAll(bms);
}
Bool RegEx::KaePattern::matches(const RegEx::MatchStringPtr& str)
{
	getKaeMatcher()->setString(str);
	return getKaeMatcher()->matches();
}
Bool RegEx::KaePattern::testing(RegEx::MatchString* str)
{
	getKaeMatcher()->reset(); // is done in setString
	return getKaeMatcher()->testing(str);
}
Bool RegEx::KaePattern::matches(const Buffer & str)
{
	BufferMatchStringPtr bms = new BufferMatchString(str);
	return matches(bms);
}
Uint32 RegEx::KaePattern::getFlags() const
{
	return flags;
}
Buffer RegEx::KaePattern::getKaePattern() const
{
	return pattern;
}

RegEx::KaeMatcher * RegEx::KaePattern::createKaeMatcher(const RegEx::MatchStringPtr& str)
{
	return new KaeMatcher(this, str);
}

// ============================================================================

int RegEx::KaeNFACharNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if ((*str)[curInd] == ch) return next->match(str, matcher, curInd + 1);
	return -1;
}

// ============================================================================

RegEx::KaeNFACICharNode::KaeNFACICharNode(
	const	char		c ) {

	ch = ( char )tolower(c);

}

int RegEx::KaeNFACICharNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (tolower((*str)[curInd]) == ch) return next->match(str, matcher, curInd + 1);
	return -1;
}

// ============================================================================

int RegEx::KaeNFAStartNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int ret = -1, ci = curInd;

	matcher->starts[0] = curInd;
	if ((matcher->getFlags() & KaeMatcher::MATCH_ENTIRE_STRING) == (unsigned int)KaeMatcher::MATCH_ENTIRE_STRING)
	{
		if (curInd != 0)
		{
			matcher->starts[0] = -1;
			return -1;
		}
		return next->match(str, matcher, 0);
	}
	while ((ret = next->match(str, matcher, ci)) == -1 && (*str)[ci] != 0)
	{
		matcher->clearGroups();
		matcher->starts[0] = ++ci;
	}
	if (ret < 0) matcher->starts[0] = -1;
	return ret;
}

// ============================================================================

int RegEx::KaeNFAEndNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	matcher->ends[0] = curInd;
	if ((matcher->getFlags() & KaeMatcher::MATCH_ENTIRE_STRING) != 0)
	{
		if ( (*str)[curInd] == 0 )
			return curInd;
		matcher->ends[0] = -1;
		return -1;
	}
	return curInd;
}

// ============================================================================

RegEx::KaeNFAQuantifierNode::KaeNFAQuantifierNode(
		KaeNFANode *	internal,
	const	Uint32		minMatch,
	const	Uint32		maxMatch ) {

	inner = internal;
	inner->next = new KaeNFAAcceptNode;
	min = Math::getMax( ( Uint32 )KaePattern::MIN_QMATCH, minMatch );
	max = Math::getMin( ( Uint32 )KaePattern::MAX_QMATCH, maxMatch );

}

int RegEx::KaeNFAQuantifierNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const {

	Int32 i0, i1, i2 = 0;

	i0 = i1 = curInd;
	while (i2 < (Int32)min) {

		++i2;
		i1 = inner->match(str, matcher, i0);
		if (i1 <= i0) return i1; // i1 < i0 means i1 is -1
		i0 = i1;
	}

	return i1;
}

// ============================================================================

int RegEx::KaeNFAGreedyQuantifierNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int t = KaeNFAQuantifierNode::match(str, matcher, curInd);
	if (t != -1) return matchInternal(str, matcher, t, min);
	return t;
}
int RegEx::KaeNFAGreedyQuantifierNode::matchInternal(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
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

int RegEx::KaeNFALazyQuantifierNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int j, m = KaeNFAQuantifierNode::match(str, matcher, curInd);

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

int RegEx::KaeNFAPossessiveQuantifierNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int j, m = KaeNFAQuantifierNode::match(str, matcher, curInd);

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

int RegEx::KaeNFAAcceptNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (!next) return curInd;
	else return next->match(str, matcher, curInd);
}

// ============================================================================

RegEx::KaeNFAClassNode::KaeNFAClassNode(
	const	Bool		invert ) {

	if ( invert ) {
		System::memset( vals, 0xFF, 0x100 );
	}
	else {
		System::bzero( vals, 0x100 );
	}

}

RegEx::KaeNFAClassNode::KaeNFAClassNode(
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

int RegEx::KaeNFAClassNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( (*str)[curInd] != 0 && vals[ (*str)[ curInd ] ] ) {
		return next->match( str, matcher, curInd + 1 );
	}

	return -1;

}

// ============================================================================

RegEx::KaeNFACIClassNode::KaeNFACIClassNode(
	const	Bool		invert ) {

	if ( invert ) {
		System::memset( vals, 0xFF, 0x100 );
	}
	else {
		System::bzero( vals, 0x100 );
	}

}

RegEx::KaeNFACIClassNode::KaeNFACIClassNode(
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
//	Uchar		c;

	while ( len-- ) {
		vals[ tolower(*ptr++) ] = mask;
		//c = *ptr++;
		//if ( c >= 'A' && c <= 'Z' ) {
		//	vals[ c ] = vals[ c - 'A' + 'a' ] = mask;
		//}
		//else if ( c >= 'a' && c <= 'z' ) {
		//	vals[ c ] = vals[ c - 'a' + 'A' ] = mask;
		//}
		//else {
		//	vals[ c ] = mask;
		//}
	}

}

int RegEx::KaeNFACIClassNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( (*str)[curInd] != 0 && vals[ tolower((*str)[ curInd ]) ] ) {
		return next->match( str, matcher, curInd + 1 );
	}

	return -1;

}

// ============================================================================

int RegEx::KaeNFASubStartNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	return next->match(str, matcher, curInd);
}

// ============================================================================

int RegEx::KaeNFAOrNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
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

int RegEx::KaeNFAQuoteNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if ((*str)[curInd + qStr.getLength()-1] == 0)			 return -1;
	if ((*str)(curInd, qStr.getLength()) != qStr) return -1;
	return next->match(str, matcher, curInd + qStr.getLength());
}

// ============================================================================

int RegEx::KaeNFACIQuoteNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const {

	if ( (*str)[curInd+qStr.getLength()-1] == 0 ) {
		return -1;
	}

	if ( (*str)( curInd, qStr.getLength() ).toLower() != qStr.toLower() ) {
		return -1;
	}

	return next->match( str, matcher, qStr.getLength() );

}

// ============================================================================

int RegEx::KaeNFALookAheadNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	return ((inner->match(str, matcher, curInd) == -1) ^ pos) ? next->match(str, matcher, curInd) : -1;
}

// ============================================================================

int RegEx::KaeNFALookBehindNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (pos)
	{
		if (curInd < mStr.getLength()) return -1;
		if ((*str)(curInd - mStr.getLength(), mStr.getLength()) == mStr) return next->match(str, matcher, curInd);
	}
	else
	{
		if (curInd < mStr.getLength()) return next->match(str, matcher, curInd);
		if ((*str)(curInd - mStr.getLength(), mStr.getLength()) == mStr) return -1;
		 return next->match(str, matcher, curInd);
	}
	return -1;
}

// ============================================================================

int RegEx::KaeNFAStartOfLineNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd == 0 || (*str)[curInd - 1] == '\n' || (*str)[curInd - 1] == '\r')
	{
		return next->match(str, matcher, curInd);
	}
	return -1;
}

// ============================================================================

int RegEx::KaeNFAEndOfLineNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if ((*str)[curInd] == 0 || (*str)[curInd] == '\n' || (*str)[curInd] == '\r')
	{
		return next->match(str, matcher, curInd);
	}
	return -1;
}

// ============================================================================

int RegEx::KaeNFAReferenceNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int len = matcher->ends[gi] - matcher->starts[gi];
	int ni = -1;
	if			(gi < 1 || matcher->ends[gi] < matcher->starts[gi] || len == 0)		ni = curInd;
	else if ((*str)[curInd+len-1] == 0)																	return -1;
	else if ((*str)(curInd, len) != (*str)(matcher->starts[gi], len)) return -1;
	else																																			ni = curInd + len;

	return next->match(str, matcher, ni);
}

// ============================================================================

int RegEx::KaeNFAStartOfInputNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd == 0) return next->match(str, matcher, curInd);
	return -1;
}

// ============================================================================

int RegEx::KaeNFAEndOfInputNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if			((*str)[curInd] == 0) return next->match(str, matcher, curInd);
	else if (term)
	{
		if			((*str)[curInd+1] == 0 && ((*str)[curInd] == '\r' || (*str)[curInd] == '\n'))
		{
			return next->match(str, matcher, curInd);
		}
		else if ((*str)[curInd+2] == 0 && (*str)(curInd, 2) == "\r\n")
		{
			return next->match(str, matcher, curInd);
		}
	}
	return -1;
}

// ============================================================================

int RegEx::KaeNFAWordBoundaryNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	#define is_alpha(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))

	Bool ok = 0;
	char c1 = (*str)[curInd-1];
	char c2 = (*str)[curInd];

	if			((*str)[curInd] == 0) return next->match(str, matcher, curInd);
	if			(is_alpha(c1) ^ is_alpha(c2)) ok = 1;
	if (ok && pos) return next->match(str, matcher, curInd);
	return -1;

	#undef is_alpha
}

// ============================================================================

int RegEx::KaeNFAEndOfMatchNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	if (curInd == ( Uint32 )matcher->lm) return next->match(str, matcher, curInd);
	return -1;
}

// ============================================================================

int RegEx::KaeNFAGroupHeadNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int ret, o = matcher->starts[gi];

	matcher->starts[gi] = curInd;
	ret = next->match(str, matcher, curInd);
	if (ret < 0) matcher->starts[gi] = o;

	return ret;
}

// ============================================================================

int RegEx::KaeNFAGroupTailNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	int ret, o = matcher->ends[gi];

	matcher->ends[gi] = curInd;
	ret = next->match(str, matcher, curInd);
	if (ret < 0) matcher->ends[gi] = o;

	return ret;
}

// ============================================================================

int RegEx::KaeNFAGroupLoopPrologueNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
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

int RegEx::KaeNFAGroupLoopNode::match(
	const	RegEx::MatchString *	str,
		KaeMatcher *	matcher,
	const	Uint32		curInd ) const
{
	Bool b = ( (Int32)curInd > matcher->groupIndeces[gi] );

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
int RegEx::KaeNFAGroupLoopNode::matchGreedy(const RegEx::MatchString * str, KaeMatcher * matcher, const Uint32 curInd) const
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
int RegEx::KaeNFAGroupLoopNode::matchLazy(const RegEx::MatchString * str, KaeMatcher * matcher, const Uint32 curInd) const
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
int RegEx::KaeNFAGroupLoopNode::matchPossessive(const RegEx::MatchString * str, KaeMatcher * matcher, const Uint32 curInd) const
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

