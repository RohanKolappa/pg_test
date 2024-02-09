// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.Event.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_Event_H_
#define _BFC_Dyn_Event_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Dyn {

BFC_PTR_DECL( BFC_Dyn_DLL, ItemsDeleted )
BFC_PTR_DECL( BFC_Dyn_DLL, ItemAppended )

} // namespace Dyn
} // namespace BFC

// ============================================================================

#include "BFC.Msg.Event.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Generated when all items have been deleted from a VEnum variable.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL ItemsDeleted : public Msg::Event {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new ItemsDeleted.

	ItemsDeleted(
	) :
		Msg::Event	( classUUId() ) {
	}

	virtual Buffer toBuffer(
	) const;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL ItemAppended : public Msg::Event {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new ItemAppended.

	ItemAppended(
	) :
		Msg::Event	( classUUId() ) {
	}

	/// \brief Creates a new ItemAppended.

	ItemAppended(
		const	Buffer &	pItemName
	) :
		Msg::Event	( classUUId() ),
		itemName	( pItemName ) {
	}

	virtual Buffer toBuffer(
	) const;

	const Buffer & getItemName(
	) const {
		return itemName;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr		pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr		pIStream
	);

private :

	Buffer	itemName;

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_Event_H_

// ============================================================================

