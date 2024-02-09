// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.UI.IElement.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _VMP_UI_IElement_H_
#define _VMP_UI_IElement_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#if defined( PLATFORM_WIN32 )

// Disable warning C4121: 'alignment of a member was sensitive to packing'

//#pragma warning ( disable : 4121 )

// Disable warning C4250: inheritance via dominance

#pragma warning ( disable : 4250 )

// Disable warning C4702: unreachable code

#pragma warning ( disable : 4702 )

#endif

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Msg.ValueEvent.h"
#include "BFC.Msg.Observable.h"
#include "BFC.Msg.Observer.h"

#include "BFC.TL.Array.h"
#include "BFC.TL.BufferArray.h"

#include "VMP.UI.IElementPtr.h"
#include "VMP.UI.IToolkitPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Layout direction.
///
/// \ingroup VMP_UI

enum LayoutDirection {
	HLayout,
	VLayout
};

// ============================================================================

/// \brief Base class of all UI elements.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IElement :

	virtual public BFC::DObject,
	public BFC::Msg::Observable,
	public BFC::Msg::Observer {

	FIX_FOR_WIN32_COMP_PROBLEM(IElement)

public :

	IElement(
			IElementPtr		eltParent,
		const	BFC::Buffer &		eltName,
		const	BFC::Buffer &		typeName,
		const	BFC::Bool		typeData
	);

	virtual ~IElement(
	);

	virtual IToolkitPtr getToolkit(
	) const = 0;

	const BFC::Buffer & getElementName(
	) const {
		return eltName;
	}

	IElementPtr getChildByName(
		const	BFC::Buffer &		elementName
	) const;

	IElementPtr findChildByName(
		const	BFC::Buffer &		elementName
	) const;

	void delAllElements(
	);

	BFC::Uint32 getNbrChildren(
	) const {
		return children.getSize();
	}

	IElementPtr getChildByIndex(
		const	BFC::Uint32		index
	) const {
		IElementPtr res = children[ index ];
		res.setMaster( false );
		return res;
	}

	BFC::Bool isData(
	) const {
		return ( typeData );
	}

	BFC::Bool isContainer(
	) const {
		return ( ! typeData );
	}

	// To modify Element properties...

	void setCaption(
		const	BFC::Buffer &		caption
	);

	BFC::Buffer getCaption(
	) const;

	virtual void resize(
	) = 0;

	virtual BFC::Int32 getXPos(
	) const = 0;

	virtual BFC::Int32 getYPos(
	) const = 0;

	virtual void setXYPos(
		const	BFC::Int32		xpos,
		const	BFC::Int32		ypos
	) = 0;

	void setHidden(
		const	BFC::Bool		yesno
	);

	BFC::Bool getHidden(
	) const;

	void setEnabled(
		const	BFC::Bool		yesno
	);

	BFC::Bool getEnabled(
	) const;

	virtual void setProperty(
		const	BFC::Buffer &		propertyName,
		const	BFC::Buffer &		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer &		propertyName
	) const;

	const BFC::Buffer & getPropertyTag(
		const	BFC::Uint32		propertyIndex
	) const;

	BFC::Buffer getPropertyVal(
		const	BFC::Uint32		propertyIndex
	) const;

	BFC::Uint32 getNbrProperties(
	) const;

	virtual void syncObservers(
	) const;

protected :

	void addProperty(
		const	BFC::Buffer &		propertyName
	);

	void delAllProperties(
	);

	IElementPtr getChildByName(
		const	BFC::BufferArray &	names,
		const	BFC::Uint32		index
	) const;

	IElementPtr findChildByName(
		const	BFC::BufferArray &	names,
		const	BFC::Uint32		index
	) const;

	IElementPtr getParent(
	) const {
		return parent;
	}

	BFC::Bool hasParent(
	) const {
		return ( BFC::Bool )parent;
	}

	// As the IElement is virtually multiply inherited, we have to
	// provide an empty constructor to keep the compiler happy, even if
	// we disable it!

	IElement(
	);

	virtual void showPropEnabled(
	) = 0;

	virtual void showPropHidden(
	) = 0;

	virtual void showPropCaption(
	) = 0;

	void cachePropEnabled(
		const	BFC::Bool		yesno
	);

	void cachePropHidden(
		const	BFC::Bool		yesno
	);

	virtual void sendEvent(
	) const = 0;

private :

	// properties...

	BFC::Bool			propEnabled;
	BFC::Bool			propHidden;
	BFC::Buffer			propCaption;

	BFC::Bool			typeData;
	IElementPtr			parent;		// SLAVE!
	BFC::Array< IElementPtr >	children;
	BFC::Array< BFC::Buffer >	properties;

	BFC::Buffer			eltName;

	static BFC::Buffer		propEnabledTag;
	static BFC::Buffer		propHiddenTag;
	static BFC::Buffer		propCaptionTag;

	/// \brief Forbidden copy constructor.

	IElement(
		const	IElement &
	);

	/// \brief Forbidden copy operator.

	IElement& operator = (
		const	IElement &
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IElement_H_

// ============================================================================

