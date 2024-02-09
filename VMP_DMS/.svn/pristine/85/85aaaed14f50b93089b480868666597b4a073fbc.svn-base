// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IElementRegisterer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.IElementRegisterer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

UI::IElementRegisterer::IElementRegisterer(
		IToolkitPtr	registry,
		IElementFactoryPtr		factory ) {

	reg = registry;
	fac = factory;

	try {
		if ( reg && fac ) {
			reg->addFactory( fac );
		}
	}
	catch ( Exception & ) {
		reg.kill();
		fac.kill();
	}

}

UI::IElementRegisterer::~IElementRegisterer() {

	try {
		if ( reg && fac ) {
			reg->delFactory( fac );
		}
	}
	catch ( Exception & ) {
		reg.kill();
		fac.kill();
	}

}

// ============================================================================

