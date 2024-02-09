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
//	BFC.Image.PlaneFormatRegistry.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.PlaneFormat.h"
#include "BFC.Image.PlaneFormatRegistry.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Image, PlaneFormatRegistry )

// ============================================================================

Image::PlaneFormatRegistry::PlaneFormatRegistry() {

	db.resize( PlaneFormat::MaxFormat );

	db[ PlaneFormat::Unknown ] = PlaneFormatManifest(
		UUId( "6aecd0f1-2ca0-4533-859d-842b341ace38" )
	);

	db[ PlaneFormat::Gray_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "62c251ac-c077-4c92-b304-096c05640305" ),
		ColorChannel::Gray, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::Gray_12_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "8c7d98e8-68e4-45a5-a85a-5f822e264866" ),
		ColorChannel::Gray, 12, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::Gray_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "96a529a2-f1ba-42d6-b9ae-f8fe7336c4e0" ),
		ColorChannel::Gray, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_R_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "59e8f584-08b6-4d03-8355-023b449cb8f1" ),
		ColorChannel::RGB_R, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_G_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "548a3118-e627-4599-a5e2-1ea224c0f161" ),
		ColorChannel::RGB_G, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_B_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "11a90a43-6019-48d2-ad0f-5485a0bd60a0" ),
		ColorChannel::RGB_B, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_A_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "6a82a0d3-06d8-4fbc-af27-340d359e3ccc" ),
		ColorChannel::RGB_A, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_R_10_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "53f48c7d-408e-4307-a68e-b44e58006c14" ),
		ColorChannel::RGB_R, 10, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_G_10_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "8339c7b9-6ad0-48f9-bc45-ce6efa3a3720" ),
		ColorChannel::RGB_G, 10, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_B_10_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "9ee30e08-ec8b-446e-873f-0ed25cf8ce83" ),
		ColorChannel::RGB_B, 10, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_A_10_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "57892fab-0b48-4df1-ab17-6a99e0f3610d" ),
		ColorChannel::RGB_A, 10, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_R_12_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "39b3afe7-d9a6-4383-b48a-e735a315b9b3" ),
		ColorChannel::RGB_R, 12, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_G_12_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "491ce8f3-056c-47f7-acc8-74de9efcd21d" ),
		ColorChannel::RGB_G, 12, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_B_12_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "d95ba7c5-0dc5-47e1-8780-9e698976005d" ),
		ColorChannel::RGB_B, 12, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_A_12_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "40cafccd-b56c-47ad-b375-3d15c988cbc1" ),
		ColorChannel::RGB_A, 12, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_R_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "6bd838b0-8cb2-4097-8c9d-6e09ad10fbac" ),
		ColorChannel::RGB_R, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_G_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "e6ba7149-0201-4484-9869-2a6a112a039a" ),
		ColorChannel::RGB_G, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_B_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "b8c99585-98f6-4ce0-9048-161f849d55fa" ),
		ColorChannel::RGB_B, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_A_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "481dce48-4f68-450a-905a-aacea6f85979" ),
		ColorChannel::RGB_A, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_All_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "1a2ba8cd-3573-4009-b0f8-fe1731a29c0f" ),
		ColorChannel::RGB_All, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_All_8_1_1_1_2_2 ] = PlaneFormatManifest(
		UUId( "06e1c1fb-55e2-4feb-a1e7-76e70335b832" ),
		ColorChannel::RGB_All, 8, 1, 1, 1, 2, 2
	);

	db[ PlaneFormat::RGB_All_15_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "8820ed9c-6410-4f91-82dd-31af24d39c69" ),
		ColorChannel::RGB_All, 15, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_All_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "f1818349-9054-487e-80eb-3b68fdf84dc4" ),
		ColorChannel::RGB_All, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_All_24_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "c5d72132-92b7-4484-b51b-41cfb366957c" ),
		ColorChannel::RGB_All, 24, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_All_30_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "4aea3db8-0899-4725-85a2-f277be4aecf3" ),
		ColorChannel::RGB_All, 30, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_All_36_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "a518c920-62c9-4d38-9319-466127343f4e" ),
		ColorChannel::RGB_All, 36, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGB_All_48_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "d2c3d909-70bf-4357-933d-c2060b8f0ec0" ),
		ColorChannel::RGB_All, 48, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RGBA_All_32_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "78873d65-c7e4-48f4-a6c1-d089aab86fbc" ),
		ColorChannel::RGBA_All, 32, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::YUV_Y_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "dc0f7c5e-cf17-430b-aa9c-4232ae478fa3" ),
		ColorChannel::YUV_Y, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::YUV_U_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "eda410e2-9261-4844-9d6a-156b90e982f4" ),
		ColorChannel::YUV_U, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::YUV_U_8_1_2_1_2_1 ] = PlaneFormatManifest(
		UUId( "b1ccee39-f73a-433e-8a49-51b2630b9b36" ),
		ColorChannel::YUV_U, 8, 1, 2, 1, 2, 1
	);

	db[ PlaneFormat::YUV_U_8_1_4_1_4_1 ] = PlaneFormatManifest(
		UUId( "0a0eec7b-d3ac-47b5-890d-bf0e682bf73e" ),
		ColorChannel::YUV_U, 8, 1, 4, 1, 4, 1
	);

	db[ PlaneFormat::YUV_U_8_1_2_2_2_2 ] = PlaneFormatManifest(
		UUId( "b36e87b9-1144-4769-8bba-e9ede67441d2" ),
		ColorChannel::YUV_U, 8, 1, 2, 2, 2, 2
	);

	db[ PlaneFormat::YUV_U_8_1_4_4_4_4 ] = PlaneFormatManifest(
		UUId( "f2d0588d-466e-4965-bc0b-0ad4f092baed" ),
		ColorChannel::YUV_U, 8, 1, 4, 4, 4, 4
	);

	db[ PlaneFormat::YUV_V_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "bf8650fb-c775-4666-bfd4-0c247de62185" ),
		ColorChannel::YUV_V, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::YUV_V_8_1_2_1_2_1 ] = PlaneFormatManifest(
		UUId( "e62e3bbe-f879-4e02-80de-85d342531bc4" ),
		ColorChannel::YUV_V, 8, 1, 2, 1, 2, 1
	);

	db[ PlaneFormat::YUV_V_8_1_4_1_4_1 ] = PlaneFormatManifest(
		UUId( "9aba13e9-1f31-452f-9d99-d0c313a7f7ad" ),
		ColorChannel::YUV_V, 8, 1, 4, 1, 4, 1
	);

	db[ PlaneFormat::YUV_V_8_1_2_2_2_2 ] = PlaneFormatManifest(
		UUId( "f90436f5-45ab-4f10-9cd2-cb13ff4630b6" ),
		ColorChannel::YUV_V, 8, 1, 2, 2, 2, 2
	);

	db[ PlaneFormat::YUV_V_8_1_4_4_4_4 ] = PlaneFormatManifest(
		UUId( "64c0c0e1-65f4-416f-9cae-af02dbe9eeed" ),
		ColorChannel::YUV_V, 8, 1, 4, 4, 4, 4
	);

	db[ PlaneFormat::YUV_A_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "9a465d7b-14c8-4881-a6f3-f57008d5385d" ),
		ColorChannel::YUV_A, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::YUV_All_16_2_1_1_2_1 ] = PlaneFormatManifest(
		UUId( "12a26757-1ac3-4e53-8a24-0951e5b0f192" ),
		ColorChannel::YUV_All, 16, 2, 1, 1, 2, 1
	);

	db[ PlaneFormat::YUV_All_24_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "b0d54efa-e38c-48e9-9aba-cab899c631d1" ),
		ColorChannel::YUV_All, 24, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::YUVA_All_32_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "313abc77-7b01-4739-98b2-f9300a2d5a06" ),
		ColorChannel::YUVA_All, 32, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RYUV_Y_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "b05b4969-f09c-41df-9d5b-0514f05db7ca" ),
		ColorChannel::RYUV_Y, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RYUV_U_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "2e31a490-d227-4e98-adac-2883d9a1e8db" ),
		ColorChannel::RYUV_U, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RYUV_V_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "75e2fca0-d36c-4a29-a19b-6c5632dfb598" ),
		ColorChannel::RYUV_V, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::RYUV_A_16_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "78ce01c5-2cdb-455a-9dfc-d4c5ca2fede2" ),
		ColorChannel::RYUV_A, 16, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::JYUV_Y_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "a3dacaa8-93cc-4a1f-b342-922b6d523bef" ),
		ColorChannel::JYUV_Y, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::JYUV_U_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "0fd070a6-674c-494e-b24f-82b9b28045ce" ),
		ColorChannel::JYUV_U, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::JYUV_U_8_1_2_1_2_1 ] = PlaneFormatManifest(
		UUId( "d8435944-6ed3-4b58-855f-8d23c9895485" ),
		ColorChannel::JYUV_U, 8, 1, 2, 1, 2, 1
	);

	db[ PlaneFormat::JYUV_U_8_1_4_1_4_1 ] = PlaneFormatManifest(
		UUId( "02841ab1-48db-4ebb-875c-ee9dfa459f6b" ),
		ColorChannel::JYUV_U, 8, 1, 4, 1, 4, 1
	);

	db[ PlaneFormat::JYUV_U_8_1_2_2_2_2 ] = PlaneFormatManifest(
		UUId( "f5d5426f-df36-400e-8080-89fc10695ee3" ),
		ColorChannel::JYUV_U, 8, 1, 2, 2, 2, 2
	);

	db[ PlaneFormat::JYUV_U_8_1_4_4_4_4 ] = PlaneFormatManifest(
		UUId( "bb8634f6-18c8-411c-98af-853f535d723a" ),
		ColorChannel::JYUV_U, 8, 1, 4, 4, 4, 4
	);

	db[ PlaneFormat::JYUV_V_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "5bcd178b-6f07-4a37-9120-bd240dd238f6" ),
		ColorChannel::JYUV_V, 8, 1, 1, 1, 1, 1
	);

	db[ PlaneFormat::JYUV_V_8_1_2_1_2_1 ] = PlaneFormatManifest(
		UUId( "cf1fa1e4-d7e6-42cb-90fb-8c20e3d981fa" ),
		ColorChannel::JYUV_V, 8, 1, 2, 1, 2, 1
	);

	db[ PlaneFormat::JYUV_V_8_1_4_1_4_1 ] = PlaneFormatManifest(
		UUId( "0fbed30c-e53c-480e-82e6-c0374f0ee29c" ),
		ColorChannel::JYUV_V, 8, 1, 4, 1, 4, 1
	);

	db[ PlaneFormat::JYUV_V_8_1_2_2_2_2 ] = PlaneFormatManifest(
		UUId( "8b0e3e18-c8d8-46fe-a3df-f20b514bc654" ),
		ColorChannel::JYUV_V, 8, 1, 2, 2, 2, 2
	);

	db[ PlaneFormat::JYUV_V_8_1_4_4_4_4 ] = PlaneFormatManifest(
		UUId( "9eea7b85-ba7c-4928-8063-03424f620d9f" ),
		ColorChannel::JYUV_V, 8, 1, 4, 4, 4, 4
	);

	db[ PlaneFormat::JYUV_A_8_1_1_1_1_1 ] = PlaneFormatManifest(
		UUId( "3a493a8b-0ed9-41c3-9bf5-93ded9e76ca9" ),
		ColorChannel::JYUV_A, 8, 1, 1, 1, 1, 1
	);

}

// ============================================================================

Image::PlaneFormatRegistryPtr Image::PlaneFormatRegistry::instance() {

	static PlaneFormatRegistryPtr i = new PlaneFormatRegistry;

	return i;

}

// ============================================================================

Uint32 Image::PlaneFormatRegistry::getIndex(
	const	UUId &		uuid ) const {

	for ( Uint32 i = 0 ; i < db.getSize() ; i++ ) {
		if ( db[ i ].getUUId() == uuid ) {
			return i;
		}
	}

	throw NotDefined();

}

Uint32 Image::PlaneFormatRegistry::getIndex(
	const	ColorChannel &	pColorChannel,
	const	Uint32		pBitsPerPixel,
	const	Uint32		pPixelsPerElt,
	const	Uint32		pMinXDownsampling,
	const	Uint32		pMinYDownsampling,
	const	Uint32		pMaxXDownsampling,
	const	Uint32		pMaxYDownsampling ) const {

	for ( Uint32 i = 0 ; i < db.getSize() ; i++ ) {
		if ( db[ i ].colorChannel == pColorChannel
		  && db[ i ].bitsPerPixel == pBitsPerPixel
		  && db[ i ].pixelsPerElt == pPixelsPerElt
		  && db[ i ].minXDownsmpl == pMinXDownsampling
		  && db[ i ].minYDownsmpl == pMinYDownsampling
		  && db[ i ].maxXDownsmpl == pMaxXDownsampling
		  && db[ i ].maxYDownsmpl == pMaxYDownsampling ) {
			return i;
		}
	}

	throw NotDefined();

}

// ============================================================================

Uint32 Image::PlaneFormatRegistry::addManifest(
	const	PlaneFormatManifest &	params ) {

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

