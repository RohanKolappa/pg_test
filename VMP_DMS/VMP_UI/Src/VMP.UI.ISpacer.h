// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ISpacer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ISpacer_H_
#define _VMP_UI_ISpacer_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,ISpacer)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract inter-element space adder.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ISpacer : virtual public IElement {

public :

	ISpacer(
	);

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

protected :

	virtual void sendEvent(
	) const;

private :

	// others...

	ISpacer(
		const	ISpacer&
	);

	ISpacer& operator = (
		const	ISpacer&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ISpacer_H_

// ============================================================================

