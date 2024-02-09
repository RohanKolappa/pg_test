// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ILineEdit.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_ILineEdit_H_
#define _VMP_UI_ILineEdit_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, ILineEdit )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract single line editor.
///
/// \ingroup VMP_UI

class VMP_UI_DLL ILineEdit : virtual public IElement {

public :

	ILineEdit(
	);

	virtual ~ILineEdit(
	);

	void setText(
		const	BFC::Buffer&		title
	);

	/// \brief Displays the ASCII representation of integer \a pValue,
	///	using the format set via setOutputFormat(), setOutputSize(),
	///	and setOutputChar().

	void setValue(
		const	BFC::Uint64		pValue
	);

	BFC::Buffer getText(
	) const;

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

	void setOutputBase(
		const	BFC::Buffer::Base	pBase
	) {
		outBase = pBase;
	}

	void setOutputSize(
		const	BFC::Uint32		pSize
	) {
		outSize = pSize;
	}

	void setOutputFillChar(
		const	BFC::Uchar		pChar
	) {
		outChar = pChar;
	}

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

	void cachePropText(
		const	BFC::Buffer		text
	);

	virtual void sendEvent(
	) const;

private :

	// internal state...

	BFC::Buffer		propText;
	Alignment		propAlign;

	BFC::Buffer::Base	outBase;	///< Base for ASCII output.
	BFC::Uint32		outSize;	///< Minimum size for ASCII output.
	BFC::Uchar		outChar;	///< Fill character for ASCII ouptut.

	static BFC::Buffer	propTextTag;
	static BFC::Buffer	propAlignTag;

	static BFC::Buffer::Base
				dfltOutBase;	///< Base for ASCII output.
	static BFC::Uint32	dfltOutSize;	///< Minimum size for ASCII output.
	static BFC::Uchar	dfltOutChar;	///< Fill character for ASCII ouptut.

	// others...

	ILineEdit(
		const	ILineEdit&
	);

	ILineEdit& operator = (
		const	ILineEdit&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_ILineEdit_H_

// ============================================================================

