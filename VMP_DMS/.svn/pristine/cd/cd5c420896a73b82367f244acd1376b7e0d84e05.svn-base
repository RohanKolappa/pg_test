// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ITextLabel.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ITextLabel_H_
#define _VMP_UI_ITextLabel_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,ITextLabel)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract text label.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ITextLabel : virtual public IElement {

public :

	static BFC::Buffer	propTextTag;
	static BFC::Buffer	propAlignTag;

	ITextLabel(
	);

	virtual ~ITextLabel(
	);

	void setText(
		const	BFC::Buffer&		title
	);

	void setNum(
		const	BFC::Uint32		v
	);

	void setNum(
		const	BFC::Int32		v
	);

	void setNum(
		const	BFC::Double		v
	);

	BFC::Buffer getText(
	) const;

	void clearText(
	);

	enum Alignment {
		AlignLeft	= 0,
		AlignRight	= 1,
		AlignCenter	= 2
	};

	void setAlignment(
		const	Alignment		value
	);

	Alignment getAlignment(
	) const;

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

	enum {
		ServSetTextId = AnyId	///< Default service.
	};

protected :

	void setTextCB(
			BFC::Msg::EventCPtr	pEvent
	);

	virtual Alignment alignBufToInt(
		const	BFC::Buffer&		value
	) const = 0;

	virtual BFC::Buffer alignIntToBuf(
		const	Alignment		value
	) const = 0;

	virtual void showPropText(
	) = 0;

	virtual void showPropAlign(
	) = 0;

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Buffer		propText;
	Alignment		propAlign;

	// others...

	ITextLabel(
		const	ITextLabel&
	);

	ITextLabel& operator = (
		const	ITextLabel&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ITextLabel_H_

// ============================================================================

