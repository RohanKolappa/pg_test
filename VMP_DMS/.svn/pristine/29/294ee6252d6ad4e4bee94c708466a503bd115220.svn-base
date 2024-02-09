// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IElementRegisterer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_UI_IElementRegisterer_H_
#define _VMP_UI_IElementRegisterer_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "VMP.UI.IElementFactory.h"
#include "VMP.UI.IToolkit.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Helper class used to register UI elements to some
///	UI element factories repository.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IElementRegisterer {

public :

	/// \brief Creates a new IElementRegisterer, registering
	///	\a factory to \a registry.

	IElementRegisterer(
			IToolkitPtr	registry,
			IElementFactoryPtr		factory
	);

	/// \brief Destroys this object, unregistering the previously registered
	///	factory.

	/* virtual */ ~IElementRegisterer(
	);

	IToolkitPtr getRegistry(
	) const {
		return reg;
	}

	IElementFactoryPtr getFactory(
	) const {
		return fac;
	}

protected :

private :

	IToolkitPtr	reg;
	IElementFactoryPtr		fac;

	/// \brief Forbidden copy constructor.

	IElementRegisterer(
		const	IElementRegisterer&
	);

	/// \brief Forbidden copy operator.

	IElementRegisterer& operator = (
		const	IElementRegisterer&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IElementRegisterer_H_

// ============================================================================

