// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ISpinBox.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ISpinBox_H_
#define _VMP_UI_ISpinBox_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, ISpinBox )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract spin box.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ISpinBox : virtual public IElement {

public :

	ISpinBox(
	);

	virtual ~ISpinBox(
	);

	void setMinValue(
		const	BFC::Int32		minValue
	);

	void setMaxValue(
		const	BFC::Int32		maxValue
	);

	void setValue(
		const	BFC::Int32		value
	);

	BFC::Int32 getMinValue(
	) const;

	BFC::Int32 getMaxValue(
	) const;

	BFC::Int32 getValue(
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

	virtual void showPropMinValue(
	) = 0;

	virtual void showPropMaxValue(
	) = 0;

	virtual void showPropValue(
	) = 0;

	void cachePropValue(
		const	BFC::Int32		value
	);

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Int32		propMinValue;
	BFC::Int32		propMaxValue;
	BFC::Int32		propValue;

	static BFC::Buffer	propMinValueTag;
	static BFC::Buffer	propMaxValueTag;
	static BFC::Buffer	propValueTag;

	// others...

	ISpinBox(
		const	ISpinBox&
	);

	ISpinBox& operator = (
		const	ISpinBox&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ISpinBox_H_

// ============================================================================

