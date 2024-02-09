// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::UI".
// 
// "VMP::UI" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::UI" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::UI"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IStackedFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_UI_IStackedFrame_H_
#define _VMP_UI_IStackedFrame_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, IStackedFrame )
BFC_PTR_DECL( VMP_UI_DLL, IStackedItem )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Provides a stack of IStackedItem objects, of which only the top
///	IStackedItem is user-visible.
///
/// As a BFC::Msg::Observer, an IStackedFrame can consume BFC::Msg::ValueEvent
/// objects on service IStackedFrame::ServRaiseId, which must contain a valid
/// BFC::Uint32 giving the index of a child to raise.
///
/// As a BFC::Msg::Observable, an IStackedFrame generates a new
/// BFC::Msg::ValueEvent every time the top level item changes, containing the
/// index of the newly visible item.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IStackedFrame : virtual public IElement {

public :

	/// \brief Creates a new IStackedFrame.

	IStackedFrame(
	);

	/// \brief Destroys this object.

	virtual ~IStackedFrame(
	);

	/// \brief Returns the number of items if this object.

	BFC::Uint32 getNbrItems(
	) const {
		return getNbrChildren();
	}

	/// \brief Returns the item whose index is \a pIndex.

	IStackedItemPtr getItem(
		const	BFC::Uint32	pIndex
	) const {
		return IStackedItemPtr( getChildByIndex( pIndex ) );
	}

	/// \brief Brings item \a pIndex to the top.

	void raiseItem(
		const	BFC::Uint32	pIndex
	);

	/// \brief Returns the index of the currently visible item.

	BFC::Uint32 getRaisedItemIndex(
	) const;

	/// \brief Returns the currently visible item.

	IStackedItemPtr getRaisedItem(
	) const;

	/// \brief List of services for this Observer.
	///
	/// An IStackedFrame can observe on service \a ServRaiseId any
	/// Observable that generates Msg::ValueEvent objects, which must
	/// provide an Uint32 that gives the index of the child to raise.

	enum {
		ServRaiseId = AnyId	///< Default service.
	};

protected :

	/// \brief Internal callback method, attached to service \a ServRaiseId.

	void raiseCB(
			BFC::Msg::EventCPtr	pEvent
	);

	/// \brief Internal helper method used by subclasses to update the
	///	internal state.
	///
	/// This method simply warn Observers by sending a new ValueEvent
	/// holding \a pIndex.

	void cachePropRaised(
		const	BFC::Uint32	pIndex
	);

	virtual void sendEvent(
	) const;

};

// ============================================================================

/// \brief Item of an IStackedFrame.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IStackedItem : virtual public IElement {

public :

	virtual void raise(
	) = 0;

	virtual BFC::Bool isRaised(
	) const = 0;

protected :

	virtual void sendEvent(
	) const;

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IStackedFrame_H_

// ============================================================================

