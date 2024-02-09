// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ISlider.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ISlider_H_
#define _VMP_UI_ISlider_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,ISlider)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract slider.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ISlider : virtual public IElement {

public :

	ISlider(
	);

	virtual ~ISlider(
	);

	// MinValue, MaxValue, Value

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

	// LineStep, PageStep

	void setLineStep(
		const	BFC::Uint32		lineStep
	);

	void setPageStep(
		const	BFC::Uint32		pageStep
	);

	BFC::Uint32 getLineStep(
	) const;

	BFC::Uint32 getPageStep(
	) const;

	// TickMarks, TickInterval

	enum TickSetting {
		NoMarks	= 0,
		Both	= 1,
		Above	= 2,
		Below	= 3,
		Left	= 4,
		Right	= 5
	};

	void setTickMarks(
		const	TickSetting		setting
	);

	void setTickInterval(
		const	BFC::Uint32		interval
	);

	TickSetting getTickMarks(
	) const;

	BFC::Uint32 getTickInterval(
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

	enum {
		ServSetMinValueId,
		ServSetMaxValueId,
		ServSetCurValueId = AnyId
	};

protected :

	void setMinValueCB(
			BFC::Msg::EventCPtr	pEvent
	);

	void setMaxValueCB(
			BFC::Msg::EventCPtr	pEvent
	);

	void setCurValueCB(
			BFC::Msg::EventCPtr	pEvent
	);

	TickSetting convertSetting(
		const	BFC::Buffer&		buffer
	) const;

	BFC::Buffer convertSetting(
		const	TickSetting		setting
	) const;

	virtual void showPropMinValue(
	) = 0;

	virtual void showPropMaxValue(
	) = 0;

	virtual void showPropValue(
	) = 0;

	virtual void showPropLineStep(
	) = 0;

	virtual void showPropPageStep(
	) = 0;

	virtual void showPropTickMarks(
	) = 0;

	virtual void showPropTickInterval(
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
	BFC::Uint32		propLineStep;
	BFC::Uint32		propPageStep;
	TickSetting		propTickMarks;
	BFC::Uint32		propTickInterval;

	static BFC::Buffer	propMinValueTag;
	static BFC::Buffer	propMaxValueTag;
	static BFC::Buffer	propValueTag;
	static BFC::Buffer	propLineStepTag;
	static BFC::Buffer	propPageStepTag;
	static BFC::Buffer	propTickMarksTag;
	static BFC::Buffer	propTickIntervalTag;

	// others...

	ISlider(
		const	ISlider&
	);

	ISlider& operator = (
		const	ISlider&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ISlider_H_

// ============================================================================

