// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IItemContainer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IItemContainer_H_
#define _VMP_UI_IItemContainer_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,IItemContainer)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Base class of all items containers.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IItemContainer : virtual public IElement {

public :

	virtual BFC::Uint32 size(
	) const = 0;

	virtual BFC::Uint32 add(
		const	BFC::Buffer &		text
	) = 0;

	virtual BFC::Buffer getText(
		const	BFC::Uint32		position
	) const = 0;

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IItemContainer_H_

// ============================================================================

