// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ICheckBox.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ICheckBox_H_
#define _VMP_UI_ICheckBox_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, ICheckBox )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract check box.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ICheckBox : virtual public IElement {

public :

	ICheckBox(
	);

	virtual ~ICheckBox(
	);

	void setText(
		const	BFC::Buffer&		title
	);

	BFC::Buffer getText(
	) const;

	void setChecked(
		const	BFC::Bool		yesno
	);

	BFC::Bool isChecked(
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

	void setCheckedCB(
			BFC::Msg::EventCPtr	pEvent
	);

protected :

	virtual void showPropText(
	) = 0;

	virtual void showPropChecked(
	) = 0;

	void cachePropChecked(
		const	BFC::Bool		value
	);

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Buffer		propText;
	BFC::Bool		propChecked;

	static BFC::Buffer	propTextTag;
	static BFC::Buffer	propCheckedTag;

	// others...

	ICheckBox(
		const	ICheckBox&
	);

	ICheckBox& operator = (
		const	ICheckBox&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ICheckBox_H_

// ============================================================================

