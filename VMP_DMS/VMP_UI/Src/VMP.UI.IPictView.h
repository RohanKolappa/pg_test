// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IPictView.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IPictView_H_
#define _VMP_UI_IPictView_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,IPictView)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "BFC.Image.Picture.h"
#include "BFC.Image.Plane.h"

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract VMP picture viewer.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IPictView : virtual public IElement {

public :

	/// \brief Enables/disables interactive mode.
	///
	/// In interactive mode, keyboard events are generated
	/// and sent to attached Msg::Observers.

	virtual void setInteractive(
		const	BFC::Bool	pYesNo
	);

	virtual void showPicture(
		const	BFC::Image::Picture &	pict
	) = 0;

	virtual void showPlane(
		const	BFC::Image::Plane &	pict,
		const	BFC::Uint32		component
	);

	virtual void hidePicture(
	) = 0;

protected :

	virtual void sendEvent(
	) const;

private :

	BFC::Image::Plane	black;

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IPictView_H_

// ============================================================================

