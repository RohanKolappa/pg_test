// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ILCDNumber.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ILCDNumber_H_
#define _VMP_UI_ILCDNumber_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,ILCDNumber)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract LCD number.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ILCDNumber : virtual public IElement {

public :

	ILCDNumber(
	);

	virtual ~ILCDNumber(
	);

	void setValue(
		const	BFC::Int32		value
	);

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

	virtual void showPropValue(
	) = 0;

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Int32		propValue;

	static BFC::Buffer	propValueTag;

	// others...

	ILCDNumber(
		const	ILCDNumber&
	);

	ILCDNumber& operator = (
		const	ILCDNumber&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ILCDNumber_H_

// ============================================================================

