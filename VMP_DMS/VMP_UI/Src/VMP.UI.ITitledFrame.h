// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ITitledFrame.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ITitledFrame_H_
#define _VMP_UI_ITitledFrame_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,ITitledFrame)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract titled frame.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ITitledFrame : virtual public IElement {

public :

	ITitledFrame(
	);

	void setTitle(
		const	BFC::Buffer&		title
	);

	BFC::Buffer getTitle(
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

protected :

	virtual void showPropTitle(
	) = 0;

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Buffer		propTitle;

	static BFC::Buffer	propTitleTag;

	// others...

	ITitledFrame(
		const	ITitledFrame&
	);

	ITitledFrame& operator = (
		const	ITitledFrame&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ITitledFrame_H_

// ============================================================================

