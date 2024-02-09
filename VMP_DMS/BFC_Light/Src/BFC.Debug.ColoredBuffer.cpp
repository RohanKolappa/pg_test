// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Debug".
// 
// "BFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Debug.ColoredBuffer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Debug.ColoredBuffer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

ColoredBuffer::ColoredBuffer(
	const	Buffer &	data,
	const	ConsoleColor	foreground,
	const	ConsoleColor	background ) {

	buffer = data;

	if ( ! buffer.isEmpty() ) {
		colors += ColorModif( 0, foreground, background );
	}

}

// ============================================================================

ColoredBuffer ColoredBuffer::operator () (
	const	Uint32		b,
	const	Uint32		e ) const {

	ColoredBuffer res;

	if (e <= b) {
		return res;
	}

	Uint32 c = 0;

	while (c+1 < colors.size() && colors[c+1].pos <= b) {
		c++;
	}

	while (c < colors.size() && colors[c].pos < e) {
		res.colors += colors[c++];
	}

	c = 0;

	for (c = 0 ; c < res.colors.size() ; c++) {
		if (res.colors[c].pos < b) {
			res.colors[c].pos = 0;
		}
		else {
			res.colors[c].pos -= b;
		}
	}

	res.buffer = buffer(b, e - b);

	return res;

}

// ============================================================================

ColoredBuffer& ColoredBuffer::operator = (
	const	ColoredBuffer&	o) {

	if (this != &o) {
		buffer = o.buffer;
		colors = o.colors;
	}

	return *this;

}

ColoredBuffer& ColoredBuffer::operator += (
	const	ColoredBuffer&	o) {

	if (o.buffer.isEmpty()) {
		return *this;
	}

	if (buffer.isEmpty()) {
		buffer = o.buffer;
		colors = o.colors;
		return *this;
	}

	Uint32 l = buffer.length();
	Uint32 p = colors.size();

	buffer += o.buffer;
	colors += o.colors;

	for (Uint32 i = p ; i < colors.size() ; i++) {
		colors[i].pos += l;
	}

	return *this;

}

ColoredBuffer ColoredBuffer::operator + (
	const	ColoredBuffer&	o) {

	ColoredBuffer r(*this);
	r += o;

	return r;

}

// ============================================================================

void ColoredBuffer::setForegroundColor(
	const	ConsoleColor	c) {

	for (Uint32 i = 0 ; i < colors.size() ; i++) {
		colors[i].fg = c;
	}

	popRedundant();

}

void ColoredBuffer::setBackgroundColor(
	const	ConsoleColor	c) {

	for (Uint32 i = 0 ; i < colors.size() ; i++) {
		colors[i].bg = c;
	}

	popRedundant();

}

void ColoredBuffer::popRedundant() {

	Uint32 i = 0;

	while (i+1 < colors.size()) {
		if (colors[i].fg == colors[i+1].fg
		 && colors[i].bg == colors[i+1].bg) {
			colors.pop(i+1);
		}
		else {
			i++;
		}
	}

}

// ============================================================================

