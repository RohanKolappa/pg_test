// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IPictView.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.IPictView.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, IPictView )

// ============================================================================

void UI::IPictView::setInteractive(
	const	Bool		/*pYesNo*/ ) {

}

// ============================================================================

void UI::IPictView::showPlane(
	const	Image::Plane &	plane,
	const	Uint32		color ) {

	Uint32 w = plane.getWidth();
	Uint32 h = plane.getHeight();
	Uint32 d = plane.getBitsPerSample();

	Image::Picture dpic;

	if ( d == 32 ) {
		dpic = Image::Picture( w, h, Image::PictureFormat::RGBA32, plane );
	}
	else if ( d == 8 ) {
		if ( black.getWidth() != plane.getWidth()
		  || black.getHeight() != plane.getHeight() ) {
			black = plane;
			black.getDataWriter().clearAll();
		}
		if (color == 0) {
			dpic = Image::Picture(w, h, Image::PictureFormat::RGB, plane, plane, plane);
		}
		else if (color == 1) {
			dpic = Image::Picture(w, h, Image::PictureFormat::RGB, plane, black, black);
		}
		else if (color == 2) {
			dpic = Image::Picture(w, h, Image::PictureFormat::RGB, black, plane, black);
		}
		else if (color == 3) {
			dpic = Image::Picture(w, h, Image::PictureFormat::RGB, black, black, plane);
		}
	}
	else {
		msgWrn( "Invalid plane depth!" );
		return;
	}

	showPicture( dpic );

}

// ============================================================================

void UI::IPictView::sendEvent() const {

}

// ============================================================================

