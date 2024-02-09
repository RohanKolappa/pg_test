// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Common".
// 
// "TBFC::Common" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Common" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Common"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Serializable.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _TBFC_Serializable_H_
#define _TBFC_Serializable_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.IO.ByteInputStream.h"
#include "TBFC.IO.ByteOutputStream.h"

// ============================================================================

namespace TBFC {

// ============================================================================

class TBFC_Light_DLL Serializable {

public :

	Serializable(
	) {
	}

	virtual ~Serializable(
	) {
	}

	/// \brief Serializes this object, and writes the result to the output
	///	stream \a out.
	///
	/// \throws CantSerialize
	///	If something went wrong. 

	virtual void doSerialize(
			IO::ByteOutputStreamPtr		out
	) const = 0;

	/// \brief Deserializes this object, by reading a previously serialized
	///	representation from the input stream \a in.
	///
	/// \throws CantUnserialize
	///	If something went wrong.

	virtual void unSerialize(
			IO::ByteInputStreamPtr		in
	) = 0;

	BASE_CLASS_GEN_EXCEPTION( "TBFC.Serializable" );

	CLASS_EXCEPTION( CantSerialize, "Can't serialize" );
	CLASS_EXCEPTION( CantUnserialize, "Can't unserialize" );

protected :

private :

	// others...

	Serializable(
		const	Serializable&
	);

	Serializable& operator = (
		const	Serializable&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Serializable_H_

// ============================================================================

