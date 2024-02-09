// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ICheckBoxes.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ICheckBoxes_H_
#define _VMP_UI_ICheckBoxes_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,ICheckBoxes)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IItemContainer.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract group of check boxes.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ICheckBoxes : virtual public IItemContainer {

public :

	ICheckBoxes(
	);

	virtual ~ICheckBoxes(
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

	ICheckBoxes(
		const	ICheckBoxes&
	);

	ICheckBoxes& operator = (
		const	ICheckBoxes&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ICheckBoxes_H_

// ============================================================================

