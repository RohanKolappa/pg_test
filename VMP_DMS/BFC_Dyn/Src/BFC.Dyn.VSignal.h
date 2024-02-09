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
//	BFC.Dyn.VSignal.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VSignal_H_
#define _BFC_Dyn_VSignal_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VSignalPtr.h"

// ============================================================================

#include "BFC.Dyn.TSignal.h"
#include "BFC.Dyn.Var.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Var being triggered.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VSignal : public Var {

public :

	/// \brief Creates a new VSignal.

	VSignal(
	);

	/// \brief Creates a new VSignal.

	VSignal(
		const	Buffer &	pVarName
	);

	VSignal(
		const	TSignalPtr &	pTSignal
	);

	/// \brief Creates a copy of \a pOther.

	VSignal(
		const	VSignal &	pOther
	);

	/// \brief Destroys this object.

	virtual ~VSignal(
	);

	/// \brief Replaces the content of this object by a copy of \a pOther.

	VSignal & operator = (
		const	VSignal &	pOther
	);

	virtual SPtr clone(
	) const;

	virtual Buffer toBuffer(
	) const;

	virtual void fromBuffer(
		const	Buffer &	pBuffer
	);

	/// \brief Triggers this object.
	///
	/// This method warns all observers, then exits.
	///
	/// Care is taken not to enter a recursive loop, for example if an
	/// observer calls this method (directly or indirectly) from its
	/// callback.

	void trigger(
	);

protected :

	/// \brief Unique observer callback, that progagates the incoming
	///	event by calling trigger().
	///
	/// \param pEvent
	///	Ignored.

	void triggerCB(
			Msg::EventCPtr	pEvent
	);

	void sendEvent(
	);

private :

	FastCS	critSec;	///< Protects the 'sending' variable.
	Bool	sending;	///< Are we warning observers ?

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VSignal_H_

// ============================================================================

