// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.XMLParser.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_XMLParser_H_
#define _VMP_UI_XMLParser_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, XMLParser )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Debug.DObject.h"

#include "VMP.UI.IElement.h"
#include "VMP.UI.IToolkit.h"

// ============================================================================

namespace BFC {
namespace DOM {

class Element;

}
}

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief UI manifest parser.
///
/// \ingroup VMP_UI

class VMP_UI_DLL XMLParser : virtual public BFC::DObject {

public :

	XMLParser(
	);

	IElementPtr parse(
		const	BFC::Buffer &	pManifest,
			IToolkitPtr	pToolkit
	) const;

	IElementPtr parse(
		const	BFC::Buffer &	pManifest,
			IToolkitPtr	pToolkit,
			IElementPtr	pParent
	) const;

	BASE_CLASS_GEN_EXCEPTION( "VMP.UI.XMLParser" );

	CLASS_EXCEPTION( CantParseManifest, "Can't parse manifest" );

protected :

	IElementPtr parse(
			IElementPtr		parent,
		const	BFC::DOM::Element &	element,
			IToolkitPtr		toolkitFactories
	) const;

private :

	// others...

	XMLParser(
		const	XMLParser			&
	);

	XMLParser& operator = (
		const	XMLParser			&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_XMLParser_H_

// ============================================================================

