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
//	BFC.RegEx.KaeMatcher.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.RegEx.KaeMatcher.h"
#include "BFC.RegEx.KaePattern.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( RegEx, KaeMatcher )
BFC_PTR_IMPL_NS( RegEx, MatchString )
BFC_PTR_IMPL_NS( RegEx, BufferMatchString )

// ============================================================================

RegEx::KaeMatcher::KaeMatcher() {



}

RegEx::KaeMatcher::KaeMatcher(KaePattern * pattern, const RegEx::MatchStringPtr &  text)
{
	pat = pattern;
	str = text;
	gc = pattern->getGroupCount();
	ncgc = - (int)pattern->getNonCapGroupCount();
	flags = 0;
	matchedSomething = false;
	starts				= new int[gc + ncgc];
	ends					= new int[gc + ncgc];
	groups				= new int[gc + ncgc];
	groupPos			= new int[gc + ncgc];
	groupIndeces	= new int[gc + ncgc];
	starts				= starts				+ ncgc;
	ends					= ends					+ ncgc;
	groups				= groups				+ ncgc;
	groupPos			= groupPos			+ ncgc;
	groupIndeces	= groupIndeces	+ ncgc;
	for (int i = 0; i < gc; ++i) starts[i] = ends[i] = 0;
}

RegEx::KaeMatcher::~KaeMatcher() {

	delete [] (starts				- ncgc);
	delete [] (ends					- ncgc);
	delete [] (groups				- ncgc);
	delete [] (groupIndeces - ncgc);
	delete [] (groupPos			- ncgc);

}

// ============================================================================

const int RegEx::KaeMatcher::MATCH_ENTIRE_STRING = 0x01;

/*
	Detailed documentation is provided in this class' header file

	@author		Jeffery Stuart
	@since		November 2004
	@version	1.07.00
*/

void RegEx::KaeMatcher::clearGroups()
{
	int i;
	lm = 0;
	for (i = 0; i < gc; ++i)		groups[i] = starts[i] = ends[i] = -1;
	for (i = 1; i <= ncgc; ++i) groups[0 - i] = -1;
}
Buffer RegEx::KaeMatcher::replaceWithGroups(const Buffer & str, Bool parseInput)
{
	Buffer ret = "";

	Buffer t = str;
	while (t.getLength() > 0)
	{
		if (t[0] == '\\')
		{
			t = t(1);
			if (t.getLength() == 0)
			{
				ret += "\\";
				t = t(1);
			}
			else if (parseInput && t[0] == 'a') {
				ret += '\a';
				t = t(1);
			}
			else if (parseInput && t[0] == 'b') {
				ret += '\b';
				t = t(1);
			}
			else if (parseInput && t[0] == 'f') {
				ret += '\f';
				t = t(1);
			}
			else if (parseInput && t[0] == 'n') {
				ret += '\n';
				t = t(1);
			}
			else if (parseInput && t[0] == 'r') {
				ret += '\r';
				t = t(1);
			}
			else if (parseInput && t[0] == 't') {
				ret += '\t';
				t = t(1);
			}
			else if (parseInput && t[0] == 'v') {
				ret += '\v';
				t = t(1);
			}
			else if (t[0] < '0' || t[0] > '9')
			{
				ret += t(0, 1);
				t = t(1);
			}
			else
			{
				int gn = 0;
				while (t.getLength() > 0 && t[0] >= '0' && t[0] <= '9')
				{
					gn = gn * 10 + (t[0] - '0');
					t = t(1);
				}
				ret += getGroup(gn);
			}
		}
		else
		{
			ret += t(0, 1);
			t = t(1);
		}
	}

	return ret;
}
Uint32 RegEx::KaeMatcher::getFlags() const
{
	return flags;
}
const RegEx::MatchStringPtr& RegEx::KaeMatcher::getText() const
{
	return str;
}

Bool RegEx::KaeMatcher::matches() {

	flags = MATCH_ENTIRE_STRING;
	matchedSomething = false;
	clearGroups();
	lm = 0;
	
	// for speed? we create a normal pointer for the string
	const MatchString* msp = &(*str);
	
	Int32 endMatch = pat->getHead()->match(msp, this, 0);
	return endMatch >= 0 && (*msp)[endMatch] == 0; // full match

}

Bool RegEx::KaeMatcher::testing(MatchString* msp) {
	flags = MATCH_ENTIRE_STRING;
	matchedSomething = false;
	clearGroups();
	lm = 0;

	Int32 endMatch = pat->getHead()->match(msp, this, 0);
	return endMatch >= 0 && (*msp)[endMatch] == 0; // full match
}


Bool RegEx::KaeMatcher::findFirstMatch()
{
	starts[0] = 0;
	flags = 0;
	clearGroups();
	start = 0;
	lm = 0;
	// for speed? we create a normal pointer for the string
	const MatchString* msp = &(*str);
	ends[0] = pat->getHead()->match(msp, this, 0);
	if (ends[0] >= 0)
	{
		matchedSomething = true;
		return 1;
	}
	return 0;
}
Bool RegEx::KaeMatcher::findNextMatch()
{
	int s = starts[0], e = ends[0];

	if (!matchedSomething) return findFirstMatch();
	if (s == e) ++e;
	flags = 0;
	clearGroups();

	starts[0] = e;
	if ((*str)[e] == 0) return 0;
	start = e;
	lm = e;
	// for speed? we create a normal pointer for the string
	const MatchString* msp = &(*str);
	ends[0] = pat->getHead()->match(msp, this, e);
	return ends[0] >= 0;
}
BufferArray RegEx::KaeMatcher::findAll()
{
	BufferArray ret;
	reset();
	while (findNextMatch())
	{
		ret += getGroup();
	}
	return ret;
}

void RegEx::KaeMatcher::reset()
{
	lm = 0;
	clearGroups();
	matchedSomething = false;
}

int RegEx::KaeMatcher::getStartingIndex(const int groupNum) const
{
	if (groupNum < 0 || groupNum >= gc) return -1;
	return starts[groupNum];
}
int RegEx::KaeMatcher::getEndingIndex(const int groupNum) const
{
	if (groupNum < 0 || groupNum >= gc) return -1;
	return ends[groupNum];
}
Buffer RegEx::KaeMatcher::getGroup(const int groupNum) const
{
	if (groupNum < 0 || groupNum >= gc) return "";
	if (starts[groupNum] < 0 || ends[groupNum] < 0) return "";
	return (*str)(starts[groupNum], ends[groupNum] - starts[groupNum]);
}
BufferArray RegEx::KaeMatcher::getGroups(const Bool includeGroupZero) const
{
	int i, start = (includeGroupZero ? 0 : 1);
	BufferArray ret;

	for (i = start; i < gc; ++i) ret += getGroup(i);
	return ret;
}

// ============================================================================

