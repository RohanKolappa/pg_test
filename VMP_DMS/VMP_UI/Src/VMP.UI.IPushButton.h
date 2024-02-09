// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IPushButton.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IPushButton_H_
#define _VMP_UI_IPushButton_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, IPushButton )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract push button.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IPushButton : virtual public IElement {

public :

	IPushButton(
	);

	virtual ~IPushButton(
	);

	void setText(
		const	BFC::Buffer&		title
	);

	BFC::Buffer getText(
	) const;

	void setClicked(
	);

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

	enum {
		ServSetClickedId = AnyId
	};

	virtual void syncObservers(
	) const;

protected :

	void setClickedCB(
			BFC::Msg::EventCPtr	pEvent
	);

	virtual void showPropText(
	) = 0;

	void cachePropClicked(
	);

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Buffer		propText;
	mutable BFC::FastCS	propClickedCritSec;
	mutable BFC::Bool	propClickedSending;

	static BFC::Buffer	propTextTag;
	static BFC::Buffer	propClickedTag;

	// others...

	IPushButton(
		const	IPushButton&
	);

	IPushButton& operator = (
		const	IPushButton&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IPushButton_H_

// ============================================================================

