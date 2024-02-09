// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Debug".
// 
// "TBFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Debug.ColoredBuffer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_ColoredBuffer_H_
#define _TBFC_Light_ColoredBuffer_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"

#include "TBFC.TL.Array.h"

#include "TBFC.Debug.ConsoleColor.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A buffer containing colored characters.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL ColoredBuffer {

public :

	ColoredBuffer(
	);

	ColoredBuffer(
		const	Buffer		& data,
		const	ConsoleColor	foreground = DefaultForeground,
		const	ConsoleColor	background = DefaultBackground
	);

	ColoredBuffer(
		const	ColoredBuffer	& data
	);

	virtual ~ColoredBuffer(
	);

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

	// internal state...

	struct ColorModif {
		ColorModif(
		) {
		}
		ColorModif(
			const	Uint32		__pos,
			const	ConsoleColor	__fg,
			const	ConsoleColor	__bg
		) : pos( __pos ), fg( __fg ), bg( __bg ) {
		}
		Uint32		pos;
		ConsoleColor	fg;
		ConsoleColor	bg;
	};

	typedef Array< ColorModif >	ColorsList;

	Buffer		buffer;
	ColorsList	colors;

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_ColoredBuffer_H_

// ============================================================================

