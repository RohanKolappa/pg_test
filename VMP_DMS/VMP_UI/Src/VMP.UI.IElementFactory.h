// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IElementFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IElementFactory_H_
#define _VMP_UI_IElementFactory_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,IElementFactory)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Debug.DObject.h"

#include "VMP.UI.Attribute.h"
#include "VMP.UI.IElement.h"
#include "VMP.UI.IToolkitPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract UI element factory.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IElementFactory : virtual public BFC::DObject {

public :

	IElementFactory(
		const	BFC::Buffer &		tagName
	);

	const BFC::Buffer & getTagName(
	) const {
		return tagName;
	}

	IElementPtr createNew(
			IElementPtr		parent,
		const	AttrTagValueArray &	props
	);

	virtual IToolkitPtr getToolkit(
	) const = 0;

	BASE_CLASS_GEN_EXCEPTION("VMP.UI.IElementFactory");

	CLASS_EXCEPTION(NoSuchAttribute, "No such attribute");

protected :

	void addSupportedAttribute(
		const	BFC::Buffer &		attribute
	);

	BFC::Buffer getAttribute(
		const	AttrTagValueArray &	props,
		const	BFC::Buffer &		name
	) const;

	virtual IElementPtr getNew(
			IElementPtr		parent,
		const	AttrTagValueArray &	attrs
	) const = 0;

private :

	// internal state...

	BFC::Buffer			tagName;
	BFC::BufferArray		attributes;

	// others...

	IElementFactory(
	);

	IElementFactory(
		const	IElementFactory&
	);

	IElementFactory& operator = (
		const	IElementFactory&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IElementFactory_H_

// ============================================================================

