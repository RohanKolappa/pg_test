// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "JBS".
// 
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	JBS.Tools.Qui2Vui.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _JBS_Tools_Qui2Vui_H_
#define _JBS_Tools_Qui2Vui_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace JBS {
namespace Tools {

BFC_PTR_DECL( JBS_DLL_NONE, Qui2Vui )

} // namespace Tools
} // namespace JBS

// ============================================================================

#include <qdom.h>

#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"

#include "JBS.Atoms.VuiAtom.h"

// ============================================================================

namespace JBS {
namespace Tools {

// ============================================================================

class JBS_DLL_NONE Qui2Vui : virtual public BFC::DObject {

public :

	Qui2Vui(
	);

	virtual ~Qui2Vui(
	);

	void generate(
			Atoms::Ui2VuiAtomPtr	vuiAtom
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "JBS.Tools.Qui2Vui" );

	CLASS_EXCEPTION( Failed, "Failed" );

protected :

	BFC::Buffer parse(
		const	BFC::Buffer&		content
	);

private :

	QDomDocument parse(
			QDomDocument		doc
	);

	QDomElement parseContainer(
			QDomDocument		oDoc,
			QDomElement		iElement,
		const	BFC::Buffer&			tagName,
		const	BFC::Buffer&			properties
	);

	void parseBox(
			QDomDocument		oDoc,
			QDomElement		oElement,
			QDomElement		iElement
	);

	QDomElement parseSimpleWidget(
			QDomDocument		oDoc,
			QDomElement		iElement,
		const	BFC::Buffer&			tagName,
		const	BFC::Buffer&			properties,
		const	BFC::Buffer&			attributes
	);

	QDomElement parseComboBox(
			QDomDocument		oDoc,
			QDomElement		iElement
	);

	QDomElement parseButtonGroup(
			QDomDocument		oDoc,
			QDomElement		iElement
	);

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Qui2Vui(
		const	Qui2Vui&
	);

	Qui2Vui& operator = (
		const	Qui2Vui&
	);

};

// ============================================================================

} // namespace Tools
} // namespace JBS

// ============================================================================

#endif // _JBS_Tools_Qui2Vui_H_

// ============================================================================

