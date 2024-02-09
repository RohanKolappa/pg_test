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
//	BFC.Image.PictureFormatRegistry.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.PictureFormat.h"
#include "BFC.Image.PictureFormatRegistry.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Image, PictureFormatRegistry )

// ============================================================================

Image::PictureFormatRegistry::PictureFormatRegistry() {

	// Make sure the plane manifests get loaded first!

	PlaneFormatRegistryPtr planeReg = PlaneFormatRegistry::instance();

	// Now, populate...

	db.resize( PictureFormat::MaxFormat );

	// ============
	// Unknown type
	// ============

	db[ PictureFormat::Unknown ] = PictureFormatManifest(
		UUId( "8b6bd502-1c2e-496f-8409-2b0ed29711c2" )
	);

	// ============
	// Planar types
	// ============

	// ... 64 bpp

	db[ PictureFormat::RYUVA444P ] = PictureFormatManifest(
		UUId( "dde50e4d-7f54-4c91-8a59-8750e8ed5800" ),
		"",
		"RYUVA444P",
		"RYUVA444P (planar)",
		ColorSpace::RYUV,
		PlaneFormat::RYUV_Y_16_1_1_1_1_1,
		PlaneFormat::RYUV_U_16_1_1_1_1_1,
		PlaneFormat::RYUV_V_16_1_1_1_1_1,
		PlaneFormat::RYUV_A_16_1_1_1_1_1
	);

	// ... 48 bpp

	db[ PictureFormat::RGB48P ] = PictureFormatManifest(
		UUId( "066c6895-c2b7-4f90-96b7-57e5babdab20" ),
		"",
		"RGB48P",
		"RGB48 (planar)",
		ColorSpace::RGB,
		PlaneFormat::RGB_R_16_1_1_1_1_1,
		PlaneFormat::RGB_G_16_1_1_1_1_1,
		PlaneFormat::RGB_B_16_1_1_1_1_1
	);

	db[ PictureFormat::RYUV444P ] = PictureFormatManifest(
		UUId( "011bda47-e429-489a-a2a6-d86e0fbda205" ),
		"",
		"RYUV444P",
		"RYUV444P (planar)",
		ColorSpace::RYUV,
		PlaneFormat::RYUV_Y_16_1_1_1_1_1,
		PlaneFormat::RYUV_U_16_1_1_1_1_1,
		PlaneFormat::RYUV_V_16_1_1_1_1_1
	);

	// ... 36 bpp

	db[ PictureFormat::RGB36P ] = PictureFormatManifest(
		UUId( "e2858744-27c0-4a5f-9d76-dd02ed8869f3" ),
		"",
		"RGB36P",
		"RGB36 (planar)",
		ColorSpace::RGB,
		PlaneFormat::RGB_R_12_1_1_1_1_1,
		PlaneFormat::RGB_G_12_1_1_1_1_1,
		PlaneFormat::RGB_B_12_1_1_1_1_1
	);

	// ... 32 bpp

	db[ PictureFormat::RGBA ] = PictureFormatManifest(
		UUId( "587f91ba-1854-470e-a69b-5fb930c17795" ),
		"",
		"RGBA",
		"RGBA (planar)",
		ColorSpace::RGB,
		PlaneFormat::RGB_R_8_1_1_1_1_1,
		PlaneFormat::RGB_G_8_1_1_1_1_1,
		PlaneFormat::RGB_B_8_1_1_1_1_1,
		PlaneFormat::RGB_A_8_1_1_1_1_1
	);

	db[ PictureFormat::BGRA ] = PictureFormatManifest(
		UUId( "4df85fa3-2d89-4a87-9f4a-6d6adf9f1787" ),
		"",
		"BGRA",
		"BGRA (planar)",
		ColorSpace::RGB,
		PlaneFormat::RGB_B_8_1_1_1_1_1,
		PlaneFormat::RGB_G_8_1_1_1_1_1,
		PlaneFormat::RGB_R_8_1_1_1_1_1,
		PlaneFormat::RGB_A_8_1_1_1_1_1
	);

	db[ PictureFormat::YUVA444P ] = PictureFormatManifest(
		UUId( "154e48b7-6a43-478d-be21-07e4353c2657" ),
		"",
		"YUVA444P",
		"YUVA444P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_1_1_1_1,
		PlaneFormat::YUV_A_8_1_1_1_1_1
	);

	db[ PictureFormat::YVUA444P ] = PictureFormatManifest(
		UUId( "59282041-05aa-434d-b2fc-dbbdaba44062" ),
		"",
		"YVUA444P",
		"YVUA444P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_1_1_1_1,
		PlaneFormat::YUV_A_8_1_1_1_1_1
	);

	// ... 30 bpp

	db[ PictureFormat::RGB30P ] = PictureFormatManifest(
		UUId( "937686fd-abaa-4d0b-9d0c-fbcbddbaf497" ),
		"",
		"RGB30P",
		"RGB30 (planar)",
		ColorSpace::RGB,
		PlaneFormat::RGB_R_10_1_1_1_1_1,
		PlaneFormat::RGB_G_10_1_1_1_1_1,
		PlaneFormat::RGB_B_10_1_1_1_1_1
	);

	// ... 24 bpp

	db[ PictureFormat::RGB ] = PictureFormatManifest(
		UUId( "ce4eb30f-be1c-4b35-a936-1df07b54f743" ),
		"",
		"RGB",
		"RGB (planar)",
		ColorSpace::RGB,
		PlaneFormat::RGB_R_8_1_1_1_1_1,
		PlaneFormat::RGB_G_8_1_1_1_1_1,
		PlaneFormat::RGB_B_8_1_1_1_1_1
	);

	db[ PictureFormat::BGR ] = PictureFormatManifest(
		UUId( "2e5ce7fe-e24d-444c-a33a-87ebb183dae8" ),
		"",
		"BGR",
		"BGR (planar)",
		ColorSpace::RGB,
		PlaneFormat::RGB_B_8_1_1_1_1_1,
		PlaneFormat::RGB_G_8_1_1_1_1_1,
		PlaneFormat::RGB_R_8_1_1_1_1_1
	);

	db[ PictureFormat::YUV444P ] = PictureFormatManifest(
		UUId( "0ed822f8-4ef4-4712-91d4-ffe2a96f2dfa" ),
		"",
		"YUV444P",
		"YUV444P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_1_1_1_1
	);

	db[ PictureFormat::YVU444P ] = PictureFormatManifest(
		UUId( "17f95054-a733-46d4-9567-99cca4b4e11f" ),
		"",
		"YVU444P",
		"YVU444P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_1_1_1_1
	);

	db[ PictureFormat::JYUV444P ] = PictureFormatManifest(
		UUId( "7cabdabb-f894-4e78-aaba-7ddeed02931b" ),
		"",
		"JYUV444P",
		"JYUV444P (planar)",
		ColorSpace::JYUV,
		PlaneFormat::JYUV_Y_8_1_1_1_1_1,
		PlaneFormat::JYUV_U_8_1_1_1_1_1,
		PlaneFormat::JYUV_V_8_1_1_1_1_1
	);

	// ... 16 bpp

	db[ PictureFormat::YUV422P ] = PictureFormatManifest(
		UUId( "a3112303-fa32-47fb-a59c-9b6fecdb5727" ),
		"YU16",
		"YUV422P",
		"YUV422P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_2_1_2_1,
		PlaneFormat::YUV_V_8_1_2_1_2_1
	);

	db[ PictureFormat::YVU422P ] = PictureFormatManifest(
		UUId( "0e7a3be8-7007-452d-977a-5f0945d76efa" ),
		"YV16",
		"YVU422P",
		"YVU422P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_2_1_2_1,
		PlaneFormat::YUV_U_8_1_2_1_2_1
	);

	db[ PictureFormat::JYUV422P ] = PictureFormatManifest(
		UUId( "7936c81c-f493-494a-9831-955e4a13d484" ),
		"",
		"JYUV422P",
		"JYUV422P (planar)",
		ColorSpace::JYUV,
		PlaneFormat::JYUV_Y_8_1_1_1_1_1,
		PlaneFormat::JYUV_U_8_1_2_1_2_1,
		PlaneFormat::JYUV_V_8_1_2_1_2_1
	);

	// ... 12 bpp

	db[ PictureFormat::YUV411P ] = PictureFormatManifest(
		UUId( "6743f90a-5d5c-4227-b706-9ba40f9531af" ),
		"",
		"YUV411P",
		"YUV411P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_4_1_4_1,
		PlaneFormat::YUV_V_8_1_4_1_4_1
	);

	db[ PictureFormat::YVU411P ] = PictureFormatManifest(
		UUId( "fc6d2e12-48ad-403e-8f8f-856fc0097cc4" ),
		"",
		"YVU411P",
		"YVU411P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_4_1_4_1,
		PlaneFormat::YUV_U_8_1_4_1_4_1
	);

	db[ PictureFormat::YUV420P ] = PictureFormatManifest(
		UUId( "0217f3a4-9609-4897-acc5-59df9656b75e" ),
		"I420,IYUV",
		"YUV420P",
		"YUV420P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_2_2_2_2,
		PlaneFormat::YUV_V_8_1_2_2_2_2
	);

	db[ PictureFormat::YVU420P ] = PictureFormatManifest(
		UUId( "9ffcd0a5-0187-4c5a-8217-92ecfcb610fd" ),
		"YV12",
		"YVU420P",
		"YVU420P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_2_2_2_2,
		PlaneFormat::YUV_U_8_1_2_2_2_2
	);

	db[ PictureFormat::JYUV420P ] = PictureFormatManifest(
		UUId( "fe5b464d-761e-43a3-a16b-f74b65649e03" ),
		"",
		"JYUV420P",
		"JYUV420P (planar)",
		ColorSpace::JYUV,
		PlaneFormat::JYUV_Y_8_1_1_1_1_1,
		PlaneFormat::JYUV_U_8_1_2_2_2_2,
		PlaneFormat::JYUV_V_8_1_2_2_2_2
	);

	// ... 9 bpp

	db[ PictureFormat::YUV410P ] = PictureFormatManifest(
		UUId( "ed380cd5-d48d-4c28-b7a1-d719848cb43a" ),
		"YUV9",
		"YUV410P",
		"YUV410P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_U_8_1_4_4_4_4,
		PlaneFormat::YUV_V_8_1_4_4_4_4
	);

	db[ PictureFormat::YVU410P ] = PictureFormatManifest(
		UUId( "ef88189e-3a4a-404c-861f-5a852c2eaba8" ),
		"YVU9",
		"YVU410P",
		"YVU410P (planar)",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1,
		PlaneFormat::YUV_V_8_1_4_4_4_4,
		PlaneFormat::YUV_U_8_1_4_4_4_4
	);

	// ============
	// Packed types
	// ============

	// ... 48 bpp

	db[ PictureFormat::RGB48 ] = PictureFormatManifest(
		UUId( "2bf635fc-6b77-4be3-ae8f-b38b951cfba8" ),
		"",
		"RGB48",
		"RGB48 (packed, RAM: R G B)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_48_1_1_1_1_1
	);

	db[ PictureFormat::BGR48 ] = PictureFormatManifest(
		UUId( "a5cfd472-0ff3-4860-a90d-0c8f7cedc545" ),
		"",
		"BGR48",
		"BGR48 (packed, RAM: B G R)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_48_1_1_1_1_1
	);

	// ... 36 bpp

	db[ PictureFormat::RGB36 ] = PictureFormatManifest(
		UUId( "e20d53e0-d8e9-4f66-a540-bb38190c81c8" ),
		"",
		"RGB36",
		"RGB36 (packed, RAM: R G B)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_48_1_1_1_1_1
	);

	db[ PictureFormat::BGR36 ] = PictureFormatManifest(
		UUId( "6439eafe-7e24-4b75-97ee-dedac979157f" ),
		"",
		"BGR36",
		"BGR36 (packed, RAM: B G R)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_48_1_1_1_1_1
	);

	// ... 32 bpp

	db[ PictureFormat::RGBA32 ] = PictureFormatManifest(
		UUId( "bc150925-4d17-434b-9b61-9e0e1b22b0f1" ),
		"RGBA",
		"RGBA32",
		"RGBA32 (packed, RAM: B G R A)",
		ColorSpace::RGB,
		PlaneFormat::RGBA_All_32_1_1_1_1_1
	);

	db[ PictureFormat::BGRA32 ] = PictureFormatManifest(
		UUId( "741cf9a9-8dab-414c-b72d-816de28891df" ),
		"",
		"BGRA32",
		"BGRA32 (packed, RAM: R G B A)",
		ColorSpace::RGB,
		PlaneFormat::RGBA_All_32_1_1_1_1_1
	);

	db[ PictureFormat::AYUV ] = PictureFormatManifest(
		UUId( "6f8ef6a1-a299-4e28-8310-7a30c8c97020" ),
		"AYUV",
		"AYUV",
		"AYUV (packed, RAM: A Y U V)",
		ColorSpace::YUV,
		PlaneFormat::YUVA_All_32_1_1_1_1_1
	);

	// ... 30 bpp

	db[ PictureFormat::RGB30 ] = PictureFormatManifest(
		UUId( "6293690c-f093-4372-9502-e41832f34b60" ),
		"",
		"RGB30",
		"RGB30 (packed, RAM: R G B)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_48_1_1_1_1_1
	);

	db[ PictureFormat::BGR30 ] = PictureFormatManifest(
		UUId( "947368ed-3e1e-4cf6-b774-9d58e6076555" ),
		"",
		"BGR30",
		"BGR30 (packed, RAM: B G R)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_48_1_1_1_1_1
	);

	// ... 24 bpp

	db[ PictureFormat::RGB24 ] = PictureFormatManifest(
		UUId( "f07df197-f2a9-4cae-9d4b-a318bf605547" ),
		"",
		"RGB24",
		"RGB24 (packed, RAM: R G B)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_24_1_1_1_1_1
	);

	db[ PictureFormat::BGR24 ] = PictureFormatManifest(
		UUId( "95d7c469-166a-48af-8dc9-7852b489faa9" ),
		"",
		"BGR24",
		"BGR24 (packed, RAM: B G R)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_24_1_1_1_1_1
	);

	db[ PictureFormat::IYU2 ] = PictureFormatManifest(
		UUId( "f565996d-e5ee-478c-897f-0c790782fa83" ),
		"IYU2",
		"IYU2",
		"IYU2 (packed, RAM: U Y V)",
		ColorSpace::RGB,
		PlaneFormat::YUV_All_24_1_1_1_1_1
	);

	// ... 16 bpp

	db[ PictureFormat::RGB565 ] = PictureFormatManifest(
		UUId( "21e7ddc4-963c-44e8-a675-8dcebdae121b" ),
		"RGB2",
		"RGB565",
		"RGB565 (packed, W16: R G B)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_16_1_1_1_1_1
	);

	db[ PictureFormat::UYVY ] = PictureFormatManifest(
		UUId( "1aed130d-fa44-45e0-9083-cdef160dccb7" ),
		"UYVY,Y422,UYNV",
		"YUV422",
		"YUV422 (packed, RAM: U Y V Y)",
		ColorSpace::YUV,
		PlaneFormat::YUV_All_16_2_1_1_2_1
	);

	db[ PictureFormat::YUYV ] = PictureFormatManifest(
		UUId( "478b1101-bdce-4fca-b715-6d14e2808481" ),
		"YUYV,YUY2,YUNV",
		"UVY422",
		"UVY422 (packed, RAM: Y U Y V)",
		ColorSpace::YUV,
		PlaneFormat::YUV_All_16_2_1_1_2_1
	);

	db[ PictureFormat::YVYU ] = PictureFormatManifest(
		UUId( "55a5c386-66d8-4fad-aeaf-06a1473f8bb9" ),
		"YVYU",
		"YVYU",
		"YVYU (packed, RAM: Y V Y U)",
		ColorSpace::YUV,
		PlaneFormat::YUV_All_16_2_1_1_2_1
	);

	// ... 15 bpp

	db[ PictureFormat::RGB555 ] = PictureFormatManifest(
		UUId( "323d2b8b-215e-4b64-9c6e-eb8dcef4bb23" ),
		"RGBT",
		"RGB555",
		"RGB555 (packed, W16: R G B)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_15_1_1_1_1_1
	);

	// ... 12 bpp

	// ... 8 bpp

	db[ PictureFormat::RGB323 ] = PictureFormatManifest(
		UUId( "781d543d-3428-4ded-864a-1e5c4dbf13c3" ),
		"",
		"RGB323",
		"RGB323 (packed, W8: R G B)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_8_1_1_1_1_1
	);

	db[ PictureFormat::SGBRG8 ] = PictureFormatManifest(
		UUId( "deceb828-a7f8-4c84-ac1f-38617bd2e4d8" ),
		"GBRG",
		"SGBRG8",
		"SGBRG8 (Bayer, L0 W16: G B, L1 W16: R G)",
		ColorSpace::RGB,
		PlaneFormat::RGB_All_8_1_1_1_2_2
	);

	// ===========
	// Other types
	// ===========

	// ... 16 bpp

	db[ PictureFormat::GRAY16 ] = PictureFormatManifest(
		UUId( "7434a40b-03e1-44b1-b930-5d74cc24c240" ),
		"",
		"GRAY16",
		"GRAY16 (16 bpp)",
		ColorSpace::Gray,
		PlaneFormat::Gray_16_1_1_1_1_1
	);

	// ... 12 bpp

	db[ PictureFormat::GRAY12 ] = PictureFormatManifest(
		UUId( "6550be6e-fd79-48b9-8af1-fcce3732ee4c" ),
		"",
		"GRAY12",
		"GRAY12 (12 bpp)",
		ColorSpace::Gray,
		PlaneFormat::Gray_12_1_1_1_1_1
	);

	// ... 8 bpp

	db[ PictureFormat::GRAY8 ] = PictureFormatManifest(
		UUId( "70260b44-a3b4-4b36-a4df-af7434da7e1b" ),
		"",
		"GRAY8",
		"GRAY8 (8 bpp)",
		ColorSpace::Gray,
		PlaneFormat::Gray_8_1_1_1_1_1
	);

	db[ PictureFormat::Y800 ] = PictureFormatManifest(
		UUId( "f7348b9b-6289-45b1-91ba-7c054b3287c4" ),
		"Y800,Y8  ,GREY",
		"YUV400",
		"YUV400",
		ColorSpace::YUV,
		PlaneFormat::YUV_Y_8_1_1_1_1_1
	);

	db[ PictureFormat::JYUV400 ] = PictureFormatManifest(
		UUId( "cf47acb2-6dd9-46c3-8b49-a5c5d4132a4a" ),
		"",
		"JYUV400",
		"JYUV400",
		ColorSpace::JYUV,
		PlaneFormat::JYUV_Y_8_1_1_1_1_1
	);

}

// ============================================================================

Image::PictureFormatRegistryPtr Image::PictureFormatRegistry::instance() {

	static PictureFormatRegistryPtr i = new PictureFormatRegistry;

	return i;

}

// ============================================================================

Uint32 Image::PictureFormatRegistry::getIndex(
	const	UUId &		uuid ) const {

	for ( Uint32 i = 0 ; i < db.getSize() ; i++ ) {
		if ( db[ i ].getUUId() == uuid ) {
			return i;
		}
	}

	throw NotDefined();

}

Uint32 Image::PictureFormatRegistry::getIndex(
	const	FourCC &	fourcc ) const {

	for ( Uint32 i = 0 ; i < db.getSize() ; i++ ) {
		if ( db[ i ].getFourCCs().contains( fourcc ) ) {
			return i;
		}
	}

	throw NotDefined();

}

Uint32 Image::PictureFormatRegistry::getIndex(
	const	CompArray< Uint32 > &	pPlaneIndexes ) const {

	for ( Uint32 i = 0 ; i < db.getSize() ; i++ ) {
		if ( db[ i ].planes == pPlaneIndexes ) {
			return i;
		}
	}

	throw NotDefined();

}

// ============================================================================

Uint32 Image::PictureFormatRegistry::addManifest(
	const	PictureFormatManifest &	params ) {

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

