// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ITextEdit.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ITextEdit_H_
#define _VMP_UI_ITextEdit_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,ITextEdit)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract text editor.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ITextEdit : virtual public IElement {

public :

	ITextEdit(
	);

	void setText(
		const	BFC::Buffer&		text
	);

	void addText(
		const	BFC::Buffer&		text
	);

	BFC::Buffer getText(
	) const;

	void clearText(
	);

	void setReadOnly(
		const	BFC::Bool		yesno
	);

	BFC::Bool isReadOnly(
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

protected :

	virtual void showPropText(
	) = 0;

	virtual void showPropReadOnly(
	) = 0;

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Buffer		propText;
	BFC::Bool		propReadOnly;

	static BFC::Buffer	propTextTag;
	static BFC::Buffer	propReadOnlyTag;

	// others...

	ITextEdit(
		const	ITextEdit&
	);

	ITextEdit& operator = (
		const	ITextEdit&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ITextEdit_H_

// ============================================================================

