// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IToolkit.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IToolkit_H_
#define _VMP_UI_IToolkit_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "VMP.UI.IToolkitPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.TL.Map.h"

#include "VMP.UI.IElementFactory.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief UI element factories registry.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IToolkit : virtual public BFC::DObject {

protected :

	IToolkit(
	);

public :

	IElementFactoryPtr getFactory(
		const	BFC::Buffer &		tagName
	) const;

	IElementPtr getElement(
		const	BFC::Buffer &		tagName,
			IElementPtr		parent,
		const	BFC::Buffer &		eltName
	) const;

	IElementPtr getElement(
		const	BFC::Buffer &		tagName,
			IElementPtr		parent,
		const	AttrTagValueArray &	props
	) const;

	void addFactory(
			IElementFactoryPtr	factory
	);

	void delFactory(
			IElementFactoryPtr	factory
	);

protected :

private :

	// internal state...

	BFC::Map< BFC::Buffer, IElementFactoryPtr >	factories;

	// others...

	IToolkit(
		const	IToolkit&
	);

	IToolkit& operator = (
		const	IToolkit&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IToolkit_H_

// ============================================================================

