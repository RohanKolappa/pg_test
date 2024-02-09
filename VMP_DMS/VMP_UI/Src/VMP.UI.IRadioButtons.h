// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IRadioButtons.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IRadioButtons_H_
#define _VMP_UI_IRadioButtons_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, IRadioButtons )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IItemContainer.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract group of radio buttons.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IRadioButtons : virtual public IItemContainer {

public :

	IRadioButtons(
	);

	virtual ~IRadioButtons(
	);

	void setTitle(
		const	BFC::Buffer&		title
	);

	BFC::Buffer getTitle(
	) const;

	void setChecked(
		const	BFC::Uint32		position
	);

	BFC::Uint32 getChecked(
	) const;

	BFC::Bool isChecked(
		const	BFC::Uint32		position
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

protected :

	void setCheckedCB(
			BFC::Msg::EventCPtr	pEvent
	);

	virtual void showPropTitle(
	) = 0;

	virtual void showPropChecked(
	) = 0;

	void cachePropChecked(
		const	BFC::Uint32		position
	);

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Buffer		propTitle;
	BFC::Uint32		propChecked;

	static BFC::Buffer	propTitleTag;
	static BFC::Buffer	propCheckedTag;

	// others...

	IRadioButtons(
		const	IRadioButtons&
	);

	IRadioButtons& operator = (
		const	IRadioButtons&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IRadioButtons_H_

// ============================================================================

