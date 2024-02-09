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
//	BFC.RegEx.Matcher.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.RegEx.Matcher.h"
#include "BFC.RegEx.Pattern.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( RegEx, Matcher )

// ============================================================================

RegEx::Matcher::Matcher() {



}

RegEx::Matcher::Matcher(Pattern * pattern, const Buffer & text)
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

RegEx::Matcher::~Matcher() {

	delete [] (starts				- ncgc);
	delete [] (ends					- ncgc);
	delete [] (groups				- ncgc);
	delete [] (groupIndeces - ncgc);
	delete [] (groupPos			- ncgc);

}

// ============================================================================

const int RegEx::Matcher::MATCH_ENTIRE_STRING = 0x01;

/*
	Detailed documentation is provided in this class' header file

	@author		Jeffery Stuart
	@since		November 2004
	@version	1.07.00
*/

void RegEx::Matcher::clearGroups()
{
	int i;
	lm = 0;
	for (i = 0; i < gc; ++i)		groups[i] = starts[i] = ends[i] = -1;
	for (i = 1; i <= ncgc; ++i) groups[0 - i] = -1;
}
Buffer RegEx::Matcher::replaceWithGroups(const Buffer & str)
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
Uint32 RegEx::Matcher::getFlags() const
{
	return flags;
}
Buffer RegEx::Matcher::getText() const
{
	return str;
}

Bool RegEx::Matcher::matches() {

	flags = MATCH_ENTIRE_STRING;
	matchedSomething = false;
	clearGroups();
	lm = 0;

	return pat->getHead()->match(str, this, 0) == (int)str.getLength();

}

Bool RegEx::Matcher::findFirstMatch()
{
	starts[0] = 0;
	flags = 0;
	clearGroups();
	start = 0;
	lm = 0;
	ends[0] = pat->getHead()->match(str, this, 0);
	if (ends[0] >= 0)
	{
		matchedSomething = true;
		return 1;
	}
	return 0;
}
Bool RegEx::Matcher::findNextMatch()
{
	int s = starts[0], e = ends[0];

	if (!matchedSomething) return findFirstMatch();
	if (s == e) ++e;
	flags = 0;
	clearGroups();

	starts[0] = e;
	if (e >= (int)str.getLength()) return 0;
	start = e;
	lm = e;
	ends[0] = pat->getHead()->match(str, this, e);
	return ends[0] >= 0;
}
BufferArray RegEx::Matcher::findAll()
{
	BufferArray ret;
	reset();
	while (findNextMatch())
	{
		ret += getGroup();
	}
	return ret;
}

void RegEx::Matcher::reset()
{
	lm = 0;
	clearGroups();
	matchedSomething = false;
}

int RegEx::Matcher::getStartingIndex(const int groupNum) const
{
	if (groupNum < 0 || groupNum >= gc) return -1;
	return starts[groupNum];
}
int RegEx::Matcher::getEndingIndex(const int groupNum) const
{
	if (groupNum < 0 || groupNum >= gc) return -1;
	return ends[groupNum];
}
Buffer RegEx::Matcher::getGroup(const int groupNum) const
{
	if (groupNum < 0 || groupNum >= gc) return "";
	if (starts[groupNum] < 0 || ends[groupNum] < 0) return "";
	return str(starts[groupNum], ends[groupNum] - starts[groupNum]);
}
BufferArray RegEx::Matcher::getGroups(const Bool includeGroupZero) const
{
	int i, start = (includeGroupZero ? 0 : 1);
	BufferArray ret;

	for (i = start; i < gc; ++i) ret += getGroup(i);
	return ret;
}

// ============================================================================

