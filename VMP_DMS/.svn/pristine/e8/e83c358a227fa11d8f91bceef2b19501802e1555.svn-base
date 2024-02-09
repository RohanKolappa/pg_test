// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IProgressBar.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IProgressBar_H_
#define _VMP_UI_IProgressBar_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,IProgressBar)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract progress bar.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IProgressBar : virtual public IElement {

public :

	IProgressBar(
	);

	virtual ~IProgressBar(
	);

	void setSteps(
		const	BFC::Uint32		steps
	);

	BFC::Uint32 getSteps(
	) const;

	void setProgress(
		const	BFC::Uint32		progress
	);

	BFC::Uint32 getProgress(
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

protected :

	void setValueCB(
			BFC::Msg::EventCPtr	pEvent
	);

	virtual void showPropSteps(
	) = 0;

	virtual void showPropProgress(
	) = 0;

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Uint32		propSteps;
	BFC::Uint32		propProgress;

	static BFC::Buffer	propStepsTag;
	static BFC::Buffer	propProgressTag;

	// others...

	IProgressBar(
		const	IProgressBar&
	);

	IProgressBar& operator = (
		const	IProgressBar&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IProgressBar_H_

// ============================================================================

