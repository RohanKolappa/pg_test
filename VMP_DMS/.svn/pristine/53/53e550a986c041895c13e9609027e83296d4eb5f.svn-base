// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Image.ResolutionRegistry.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Resolution.h"
#include "BFC.Image.ResolutionRegistry.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Image, ResolutionRegistry )

// ============================================================================

Image::ResolutionRegistry::ResolutionRegistry() {

	// Populate...

	db.resize( Resolution::MaxFormat );

	db[ Resolution::Unknown	] = ResolutionManifest(
		UUId( "d44e7e93-51cb-4461-aaf3-1d3dcb3a3204" ),	0,	0,	"Unknown",		"n/a"		);
	db[ Resolution::SQCIF	] = ResolutionManifest(
		UUId( "4a56ab9f-417a-4a08-bd50-e83b6a5763d8" ),	128,	96,	"SQCIF",		"sqcif"		);
	db[ Resolution::QQVGA	] = ResolutionManifest(
		UUId( "3519be85-6bcd-4c84-b26c-d917893671b2" ),	160,	120,	"Quarter QVGA",		"qqvga"		);
	db[ Resolution::QCIF	] = ResolutionManifest(
		UUId( "0fa7fed9-00a8-43dc-9604-c4c83f13b1e2" ),	176,	144,	"QCIF",			"qcif"		);
	db[ Resolution::EVGA	] = ResolutionManifest(
		UUId( "fb6cab41-2b07-4ff8-b1d1-2b5d22540b8c" ),	240,	180,	"1/8 VGA",		"evga"		);
	db[ Resolution::QVGA	] = ResolutionManifest(
		UUId( "a4bfd0e8-cad6-47c7-a8cc-dac82c3adb1b" ),	320,	240,	"Quarter VGA",		"qvga"		);
	db[ Resolution::SIF	] = ResolutionManifest(
		UUId( "76bc8d64-7eca-48ab-84ff-6a1c47fba03b" ),	352,	240,	"SIF",			"sif"		);
	db[ Resolution::SIF_N	] = ResolutionManifest(
		UUId( "a2fcd287-d177-44ad-b945-4d15a834935d" ),	360,	240,	"SIF(NTSC)",		"sif_n"		);
	db[ Resolution::CIF	] = ResolutionManifest(
		UUId( "d82238de-45eb-4ea5-a27f-8651c89d2eee" ),	352,	288,	"CIF",			"cif"		);
	db[ Resolution::SIF_P	] = ResolutionManifest(
		UUId( "da8daac7-3540-438c-9680-79110190fe67" ),	360,	288,	"SIF(PAL)",		"sif_p"		);
	db[ Resolution::WVGA	] = ResolutionManifest(
		UUId( "4e4caa8c-64dc-4a3a-ab86-8056b7d24d49" ),	720,	400,	"Wide VGA",		"wvga"		);
	db[ Resolution::VGA	] = ResolutionManifest(
		UUId( "a2b3b5fd-4720-4070-ae73-64d2fc4f5147" ),	640,	480,	"VGA",			"vga"		);
	db[ Resolution::NTSC	] = ResolutionManifest(
		UUId( "3e20434c-b4c5-4a15-b1a0-c1326699a358" ),	720,	480,	"NTSC",			"ntsc"		);
	db[ Resolution::NTSC486	] = ResolutionManifest(
		UUId( "b8412f97-ca06-4a38-9500-9076e73300bb" ),	720,	486,	"NTSC 486",		"ntsc486"	);
	db[ Resolution::CIF4	] = ResolutionManifest(
		UUId( "5100044f-8639-4ec7-9510-ee799999f937" ),	704,	576,	"4CIF",			"cif4"		);
	db[ Resolution::PAL	] = ResolutionManifest(
		UUId( "220ea884-fdc9-4807-b68d-7dd1547b1091" ),	720,	576,	"PAL",			"pal"		);
	db[ Resolution::PALW	] = ResolutionManifest(
		UUId( "13548e11-c28c-493f-b7f5-81d801d45723" ),	1024,	576,	"PAL-Wide",		"palw"		);
	db[ Resolution::SVGA	] = ResolutionManifest(
		UUId( "5302e535-b334-49f6-8ee0-9a9da0799436" ),	800,	600,	"SVGA",			"svga"		);
	db[ Resolution::WSVGA	] = ResolutionManifest(
		UUId( "3ca1faa4-13e9-4f83-a5e3-3260fd5afbb5" ),	1024,	600,	"Wide SVGA",		"wsvga"		);
	db[ Resolution::HDTV720	] = ResolutionManifest(
		UUId( "9bb000cc-fab8-431c-a898-730d6ce8373b" ),	1280,	720,	"HDTV 720",		"720"		);
	db[ Resolution::XGA	] = ResolutionManifest(
		UUId( "f8e1c55e-d232-4eea-8c3f-8e1dc99b83dd" ),	1024,	768,	"XGA",			"xga"		);
	db[ Resolution::WXGA	] = ResolutionManifest(
		UUId( "9b76817c-9d00-4855-9bbc-187f58d710d0" ),	1280,	768,	"Wide XGA",		"wxga"		);
	db[ Resolution::CIF9	] = ResolutionManifest(
		UUId( "3ee85cec-f174-4d4f-bc0e-c88ba2d3b74a" ),	1056,	864,	"9CIF",			"cif9"		);
	db[ Resolution::SXGA	] = ResolutionManifest(
		UUId( "663955a2-844e-453b-a32b-a2153034da91" ),	1280,	1024,	"SXGA",			"sxga"		);
	db[ Resolution::WSXGA	] = ResolutionManifest(
		UUId( "4fba5a7e-6f8c-409d-bc28-10d90131193c" ),	1600,	1024,	"Wide SXGA",		"wsxga"		);
	db[ Resolution::HDTV1080] = ResolutionManifest(
		UUId( "750914cb-3541-45f1-96c8-c6c62f3408bd" ),	1920,	1080,	"HDTV 1080",		"1080"		);
//	db[ Resolution::] = ResolutionManifest(
//		UUId( "ae8b2102-58bd-40c1-8c0c-9a85557bacf6" ),	2048,	1080,	"DigCin 2K",		"dc2k"		);
	db[ Resolution::CIF16	] = ResolutionManifest(
		UUId( "203655fa-4556-4e3c-9071-6fbea4898eeb" ),	1408,	1152,	"16CIF",		"cif16"		);
	db[ Resolution::UXGA	] = ResolutionManifest(
		UUId( "d22b2d7f-9867-441a-a5fc-bdba98ec7850" ),	1600,	1200,	"Ultra XGA",		"uxga"		);
	db[ Resolution::WUXGA	] = ResolutionManifest(
		UUId( "22c22e46-2e6a-42fd-be7c-1e9613a7dbbd" ),	1920,	1200,	"Wide Ultra XGA",	"wuxga"		);
//	db[ Resolution::	] = ResolutionManifest(
//		UUId( "e94caf10-98b2-4291-b1ed-b647f49cc9f8" ),	????,	1440,	"HDTV 1440",		"1440"		);
	db[ Resolution::SUXGA	] = ResolutionManifest(
		UUId( "f00dafc5-457b-4d5e-bd00-c04f118f8cf9" ),	2048,	1536,	"Super UXGA",		"suxga"		);
	db[ Resolution::QXGA	] = ResolutionManifest(
		UUId( "c8b376ef-274a-4545-b708-0270ea67a353" ),	2048,	1536,	"Quad XGA",		"qxga"		);
//	db[ Resolution::] = ResolutionManifest(
//		UUId( "8356babe-5faf-450b-8a5a-9903396100c1" ),	4096,	2160,	"DigCin 4K",		"dc4k"		);
	db[ Resolution::QUXGA	] = ResolutionManifest(
		UUId( "65e82610-f8a4-49e9-bd14-5d6dd77bec22" ),	3200,	2400,	"Quad UXGA",		"quxga"		);
	db[ Resolution::QWUXGA	] = ResolutionManifest(
		UUId( "95441225-44ca-465f-bb81-127304947d96" ),	3840,	2400,	"Quad WUXGA",		"qwuxga"	);
	db[ Resolution::WQUXGA	] = ResolutionManifest(
		UUId( "051a1915-b110-4538-bbeb-721059be6089" ),	3840,	2400,	"Wide QUXGA",		"wquxga"	);
//	db[ Resolution::	] = ResolutionManifest(
//		"8ea37a84-c1fa-4d89-a35c-234ae70809a3" ),	7680,	4320,	"UHDV 4320p",		"4320"		);

}

Image::ResolutionRegistry::~ResolutionRegistry() {



}

// ============================================================================

Image::ResolutionRegistryPtr Image::ResolutionRegistry::instance() {

	static ResolutionRegistryPtr i = new ResolutionRegistry;

	return i;

}

// ============================================================================

Uint32 Image::ResolutionRegistry::getIndex(
	const	UUId &		uuid ) const {

	for ( Uint32 i = 0 ; i < db.getSize() ; i++ ) {
		if ( db[ i ].getUUId() == uuid ) {
			return i;
		}
	}

	throw NotDefined();

}

// ============================================================================

Uint32 Image::ResolutionRegistry::addManifest(
	const	ResolutionManifest &	params ) {

	if ( params.getUUId().isNull() ) {
		throw InvalidArgument( "No UUId!" );
	}

	for ( Uint32 i = 0 ; i < db.getSize() ; i++ ) {
		if ( db[ i ] == params ) {
			throw Duplicate();
		}
	}

	db += params;

	return ( db.getSize() - 1 );

}

// ============================================================================

