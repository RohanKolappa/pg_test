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
//	BFC.Debug.ColoredBuffer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_ColoredBuffer_H_
#define _BFC_Light_ColoredBuffer_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.TL.Array.h"

#include "BFC.Debug.ConsoleColor.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A buffer containing colored characters.
///
/// \ingroup BFC_Light

class BFC_Light_DLL ColoredBuffer {

public :

	class ColorModif {

	public :

		ColorModif(
		) {
		}

		ColorModif(
			const	Uint32		pStartPos,
			const	ConsoleColor	pForeColr,
			const	ConsoleColor	pBackColr
		) :
			pos	( pStartPos ),
			fg	( pForeColr ),
			bg	( pBackColr )
		{
		}

		Uint32		pos;
		ConsoleColor	fg;
		ConsoleColor	bg;

	};

	typedef Array< ColorModif >	ColorsList;

	ColoredBuffer(
	) {
	}

	ColoredBuffer(
		const	Buffer &	data,
		const	ConsoleColor	foreground = DefaultForeground,
		const	ConsoleColor	background = DefaultBackground
	);

	ColoredBuffer(
		const	Buffer &	pTextMesg,
		const	ColorsList &	pColrModf
	) :
		buffer	( pTextMesg ),
		colors	( pColrModf )
	{
	}

	ColoredBuffer(
		const	ColoredBuffer &	pOther
	) :
		buffer	( pOther.buffer ),
		colors	( pOther.colors )
	{
	}

	const Buffer & getBuffer(
	) const {
		return buffer;
	}

	Uint32 getLength(
	) const {
		return buffer.length();
	}

	Uint32 getColorsNumber(
	) const {
		return colors.size();
	}

	Uint32 getStartPosition(
		const	Uint32		i
	) const {
		return colors[ i ].pos;
	}

	ConsoleColor getForeground(
		const	Uint32		i
	) const {
		return colors[ i ].fg;
	}

	ConsoleColor getBackground(
		const	Uint32		i
	) const {
		return colors[ i ].bg;
	}

	ColoredBuffer operator () (
		const	Uint32		beg,
		const	Uint32		end
	) const;

	ColoredBuffer & operator = (
		const	ColoredBuffer	& buf
	);

	ColoredBuffer & operator += (
		const	ColoredBuffer	& buf
	);

	ColoredBuffer operator + (
		const	ColoredBuffer	& buf
	);

	void setForegroundColor(
		const	ConsoleColor	color
	);

	void setBackgroundColor(
		const	ConsoleColor	color
	);

protected :

	void popRedundant(
	);

private :

	Buffer		buffer;
	ColorsList	colors;

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Light_ColoredBuffer_H_

// ============================================================================

