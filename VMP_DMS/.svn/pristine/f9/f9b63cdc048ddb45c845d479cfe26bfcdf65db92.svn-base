// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IComboBox.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IComboBox_H_
#define _VMP_UI_IComboBox_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, IComboBox )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IItemContainer.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract combo box.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IComboBox : virtual public IItemContainer {

public :

	IComboBox(
	);

	virtual ~IComboBox(
	);

	// Implementation of IItemContainer interface.

	virtual BFC::Uint32 size(
	) const;

	virtual BFC::Uint32 add(
		const	BFC::Buffer&		text
	);

	virtual BFC::Buffer getText(
		const	BFC::Uint32		position
	) const;

	virtual void clear(
	);

	void setSelected(
		const	BFC::Uint32		position
	);

	void setSelected(
		const	BFC::Buffer&		item
	);

	BFC::Uint32 getSelected(
	) const;

	BFC::Bool isSelected(
		const	BFC::Uint32		position
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

	void dynEventCB(
			BFC::Msg::EventCPtr	pEvent
	);

protected :

	virtual void showItemsAdd(
		const	BFC::Buffer&		text
	) = 0;

	virtual void showItemsClear(
	) = 0;

	virtual void showPropSelected(
	) = 0;

	void cachePropSelected(
		const	BFC::Uint32		position
	);

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Array< BFC::Buffer >	textCache;

	BFC::Uint32			propSelected;

	static BFC::Buffer		propSelectedTag;

	// others...

	IComboBox(
		const	IComboBox&
	);

	IComboBox& operator = (
		const	IComboBox&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IComboBox_H_

// ============================================================================

