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
//	VMP.AVC.NALU2AUFramer.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.AVC.CodecConfig.h"
#include "VMP.AVC.Defines.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.NALU2AUFramer.h"
#include "VMP.AVC.Frame.h"
#include "VMP.AVC.Session.h"
#include "VMP.AVC.SequenceParameterSet.h"
#include "VMP.AVC.SliceHeader.h"
#include "VMP.AVC.VUIParameters.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, NALU2AUFramer )

// ============================================================================

const DL::TypeCPtr & AVC::NALU2AUFramer::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "c61f950b-07cd-4406-b914-c8db6e34cd50" ),
		"VMP.AVC.NALU2AUFramer",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"NALU2AUFramer",
			"Utils",
			"Concatenates multiple NALU's to form Access Units."
		)
	);

	return i;

}

// ============================================================================

AVC::NALU2AUFramer::NALU2AUFramer() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, Session::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	static const Uchar aud[] = { 0x09, 0xE0 };

	audCache = new NALU( Buffer( aud, 2, true ) );

}

AVC::NALU2AUFramer::~NALU2AUFramer() {

}

// ============================================================================

void AVC::NALU2AUFramer::putSessionCallback(
		VMP::SessionPtr	pSession ) {

//	msgDbg( "putSession(): " + pSession->toBuffer() );

	iBytSess			= pSession;
	iVidStrm			= pSession->getStream();

	pictWdth			= iVidStrm->getWidth();
	pictHght			= iVidStrm->getHeight();
	frmeRate			= iVidStrm->getFrameRate();
	frmeScle			= iVidStrm->getFrameScale();

	num_units_in_tick		= 0;
	time_scale			= 0;
	fixed_frame_rate_flag		= 0;
	separate_colour_plane_flag	= false;
	log2_max_frame_num		= 5;
	frame_mbs_only_flag		= true;
	prevNalu.kill();
	naluList.kill();
	keyFrame			= false;
	aUntCntr			= 0;

	spsCache.kill();
	ppsCache.kill();

	if ( iVidStrm->getExtraData().hasContent() ) {
//		msgDbg( "putSession(): got SPS/PPS from cfg_rec!" );
		CodecConfigPtr cfg = new CodecConfig;
		cfg->decode( iVidStrm->getExtraData() );
		if ( cfg->getNbrSPSs() != 1
		  || cfg->getNbrPPSs() != 1 ) {
			throw InternalError( "Missing SPS/PPS!" );
		}
		spsCache = cfg->getSPS( 0 );
		decodeSPS( spsCache );
		ppsCache = cfg->getPPS( 0 );
	}
	else {
//		msgDbg( "putSession(): no SPS/PPS in cfg_rec! Waiting for some inline..." );
	}

	oBytSess = iBytSess->clone();
	oVidStrm = oBytSess->getStream();

	oVidStrm->setWidth( pictWdth );
	oVidStrm->setHeight( pictHght );
	oVidStrm->setFrameRate( frmeRate );
	oVidStrm->setFrameScale( frmeScle );

	getPeerEngine( 0 )->putSessionCallback( oBytSess );

	setInSession( true );

}

void AVC::NALU2AUFramer::endSessionCallback() {

//	msgDbg( "endSession()" );

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

	iBytSess.kill();
	iVidStrm.kill();
	oBytSess.kill();
	oVidStrm.kill();

}

void AVC::NALU2AUFramer::putFrameCallback(
		VMP::FramePtr	pFrame ) {

//	msgDbg( "putFrameCallback(): " + pFrame->toBuffer() );

	FrameCPtr	currFrme = pFrame;
	NALUCPtr	currNalu = currFrme->getNALU();
	Uchar		currType = currNalu->getType();

//	msgDbg( "putFrameCallback(): type: " + Buffer( ( Uint32 )currType, Buffer::Base16, 2 )
//		+ ", time: " + currFrme->getTimeStamp().dateTimeToBuffer( true, true ) );

	if ( currType == NALU::TypeSPS ) {
		if ( ! spsCache ) {
			spsCache = currNalu;
			decodeSPS( currNalu );
		}
		else if ( spsCache->getData() != currNalu->getData() ) {
			msgWrn( "putFrameCallback(): mutating SPS!" );
			msgWrn( "putFrameCallback(): Old SPS: " + Utils::toHex( spsCache->getData() ) );
			msgWrn( "putFrameCallback(): New SPS: " + Utils::toHex( currNalu->getData() ) );
// FIXME: SPS mutation should generate a new session!
// FIXME: Let multiple SPS coexist if using different id's!
			// Use new version to prevent further warnings, but do not decode it.
			spsCache = currNalu;
		}
	}
	else if ( currType == NALU::TypePPS ) {
		if ( ! ppsCache ) {
			ppsCache = currNalu;
		}
		else if ( ppsCache->getData() != currNalu->getData() ) {
			msgWrn( "putFrameCallback(): mutating PPS!" );
			msgWrn( "putFrameCallback(): Old PPS: " + Utils::toHex( ppsCache->getData() ) );
			msgWrn( "putFrameCallback(): New PPS: " + Utils::toHex( currNalu->getData() ) );
// FIXME: PPS mutation should generate a new session!
// FIXME: Let multiple PPS coexist if using different id's!
			// Use new version to prevent further warnings, but do not decode it.
			ppsCache = currNalu;
		}
	}
	else if ( ! spsCache || ! ppsCache ) {
		msgDbg( "putFrameCallback(): Missing SPS and/or PPS. Dropping Frame!" );
		return;
	}

	Bool	flushAUn;		// Need to flush previous access unit ?

	if ( ! prevNalu ) {
//		msgDbg( "putFrameCallback(): no prev..." );
		flushAUn = false;	// Nothing to flush!
	}
	else if ( prevNalu->isNonVCL() ) {
//		msgDbg( "putFrameCallback(): prev non vcl..." );
		flushAUn = false;	// Add whatever we get here...
	}
	else if ( currNalu->isNonVCL() ) {
//		msgDbg( "putFrameCallback(): curr non vcl..." );
		flushAUn = true;	// Some non-VCL following some VCL --> flush!
	}
	else if ( currNalu->isIDR() != prevNalu->isIDR() ) {
//		msgDbg( "putFrameCallback(): idr != idr..." );
		flushAUn = true;	// IDR + Slice, or Slice + IDR --> flush!
	}
	else if ( currNalu->getRefIdc() != prevNalu->getRefIdc()
	       && ( currNalu->getRefIdc() == 0
		 || prevNalu->getRefIdc() == 0 ) ) {
//		msgDbg( "putFrameCallback(): ref != ref..." );
		flushAUn = true;
	}
	else if ( ( ! prevNalu->isSlice() && ! prevNalu->isDPA() && ! prevNalu->isIDR() )
	       || ( ! currNalu->isSlice() && ! currNalu->isDPA() && ! currNalu->isIDR() ) ) {
//		msgDbg( "putFrameCallback(): huh ?..." );
		flushAUn = false;	// No slice header to parse... Don't know what to do :-(
	}
	else {
//		msgDbg( "putFrameCallback(): comparing slices..." );
		SliceHeaderPtr	prvHeadr = new SliceHeader;
		SliceHeaderPtr	curHeadr = new SliceHeader;
		prvHeadr->decode( prevNalu, separate_colour_plane_flag, log2_max_frame_num, frame_mbs_only_flag, pic_order_cnt_type, log2_max_pic_order_cnt_lsb );
		curHeadr->decode( currNalu, separate_colour_plane_flag, log2_max_frame_num, frame_mbs_only_flag, pic_order_cnt_type, log2_max_pic_order_cnt_lsb );
//		msgDbg( "putFrameCallback(): comparing prev: " + prvHeadr->toBuffer() );
//		msgDbg( "putFrameCallback(): comparing curr: " + curHeadr->toBuffer() );
		if ( curHeadr->frame_num != prvHeadr->frame_num ) {
//			msgDbg( "putFrameCallback(): comparing: ! frame_nul" );
			flushAUn = true;
		}
		else if ( curHeadr->pic_parameter_set_id != prvHeadr->pic_parameter_set_id ) {
//			msgDbg( "putFrameCallback(): comparing: ! pps id" );
			flushAUn = true;
		}
		else if ( curHeadr->field_pic_flag != prvHeadr->field_pic_flag ) {
//			msgDbg( "putFrameCallback(): comparing: ! field pic flag" );
			flushAUn = true;
		}
		else if ( curHeadr->bottom_field_flag != prvHeadr->bottom_field_flag ) {
//			msgDbg( "putFrameCallback(): comparing: ! bottom field flag" );
			flushAUn = true;
		}
		else if ( currNalu->isIDR() && curHeadr->idr_pic_id != prvHeadr->idr_pic_id ) {
//			msgDbg( "putFrameCallback(): comparing: ! idr_pic_id" );
			flushAUn = true;
		}
		else if ( seqParSt && seqParSt->pic_order_cnt_type == 0
		       && curHeadr->pic_order_cnt_lsb != prvHeadr->pic_order_cnt_lsb ) {
//			msgDbg( "putFrameCallback(): comparing: ! pic_order_cnt_lsb" );
			flushAUn = true;
		}
		else {
//			msgDbg( "putFrameCallback(): comparing: no flush!!!" );
			flushAUn = false;
		}

	}

	if ( flushAUn ) {

//		msgDbg( "putFrameCallback(): flushing..." );

		verify( naluList );

		BytesFramePtr	bFrm = new BytesFrame;

		bFrm->setData( collect( naluList ) );
		bFrm->setPTS( prevsPTS );
		bFrm->setDTS( prevsDTS );
		bFrm->setUnitOffset( aUntCntr++ );
		bFrm->setKeyFrame( keyFrame );

		naluList.kill();
		keyFrame = false;

		getPeerEngine( 0 )->putFrameCallback( bFrm );

	}

	prevNalu = currNalu;
	prevsPTS = currFrme->getPTS();
	prevsDTS = currFrme->getDTS();
	naluList += currNalu;

	if ( currNalu->isIDR() ) {
//		if ( ! spsCache || ! ppsCache ) {
//			setInSession( false );
//			throw L2::BrokenSession( "putFrameCallback(): no SPS/PPS before IDR!" );
//		}
		keyFrame = true;
	}

}

void AVC::NALU2AUFramer::flushSessionCallback() {

//	msgDbg( "rstSession()" );

	if ( naluList.hasContent() ) {

		verify( naluList );

		BytesFramePtr	bFrm = new BytesFrame;

		bFrm->setData( collect( naluList ) );
		bFrm->setPTS( prevsPTS );
		bFrm->setDTS( prevsDTS );
		bFrm->setUnitOffset( aUntCntr++ );
		bFrm->setKeyFrame( keyFrame );

		naluList.kill();
		keyFrame = false;

		getPeerEngine( 0 )->putFrameCallback( bFrm );

	}

	prevNalu.kill();

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

void AVC::NALU2AUFramer::verify(
		Array< NALUCPtr > &
				pNaluList ) const {

//	msgDbg( "verify():" );

	Uint32	i = 0;

	// Find first data unit...

	while ( i < pNaluList.getSize() && pNaluList[ i ]->isNonVCL() ) {
		if ( pNaluList[ i ]->isSEI() ) {
			pNaluList.pop( i );
		}
		else {
			i++;
		}
	}

	if ( i >= pNaluList.getSize() ) {
		// Only metadata ?
		msgWrn( "verify(): Only non-VCL NALU's in this AU!" );
//		throw InternalError();
	}

	Bool	gotIDR = ( i < pNaluList.getSize() && pNaluList[ i ]->getType() == NALU::TypeIDR );

	Bool	gotAUD = false;
	Bool	gotSPS = false;
	Bool	gotPPS = false;

	for ( Uint32 j = 0 ; j < i ; j++ ) {
		if ( pNaluList[ j ]->getType() == NALU::TypeAUD ) {
			gotAUD = true;
		}
		else if ( pNaluList[ j ]->getType() == NALU::TypeSPS ) {
			gotSPS = true;
		}
		else if ( pNaluList[ j ]->getType() == NALU::TypePPS ) {
			gotPPS = true;
		}
	}

	if ( gotIDR && ! gotPPS ) {
//		msgDbg( "verify(): inserting PPS..." );
		pNaluList.insert( 0, ppsCache );
	}

	if ( gotIDR && ! gotSPS ) {
//		msgDbg( "verify(): inserting SPS..." );
		pNaluList.insert( 0, spsCache );
	}

	if ( ! gotAUD ) {
		pNaluList.insert( 0, audCache );
	}

}

Buffer AVC::NALU2AUFramer::collect(
	const	Array< NALUCPtr > &
				pNaluList ) const {

	Buffer	dbugMesg;

	for ( Uint32 i = 0 ; i < pNaluList.getSize() ; i++ ) {
		dbugMesg += " " + NALU::getTypeName( pNaluList[ i ]->getType() );
	}

//	msgDbg( "                              " + dbugMesg );

	static const Buffer prefix = Buffer( ( const Uchar * )"\x00\x00\x00\x01", 4, true );

	Uint32	totlSize = 0;

	for ( Uint32 i = 0 ; i < pNaluList.getSize() ; i++ ) {
		totlSize += prefix.getLength() + pNaluList[ i ]->getData().getLength();
	}

	Buffer	accsUnit;

	accsUnit.supposedMaxSize( totlSize );

	for ( Uint32 i = 0 ; i < pNaluList.getSize() ; i++ ) {
		accsUnit += prefix + pNaluList[ i ]->getData();
	}

	return accsUnit;

}

// ============================================================================

void AVC::NALU2AUFramer::decodeSPS(
		NALUCPtr	pNalu ) {

	SequenceParameterSetPtr sps = new SequenceParameterSet;

	try {
		sps->decode( pNalu );
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't decode SPS!" );
		msgErr( Utils::toHex( pNalu->getData() ) );
		msgErr( Utils::toHex( pNalu->getRBSP() ) );
		throw InternalError( e );
	}

	seqParSt = sps;

	num_units_in_tick	= 0;
	time_scale		= 0;
	fixed_frame_rate_flag	= 0;

	separate_colour_plane_flag
		= ( ( sps->profile_idc == FREXT_HP		// 100
		   || sps->profile_idc == FREXT_Hi10P		// 110
		   || sps->profile_idc == FREXT_Hi422		// 122
		   || sps->profile_idc == FREXT_Hi444		// 244
		   || sps->profile_idc == FREXT_CAVLC444	// 44
		   || sps->profile_idc == SCALABLE_BASELINE	// 83
		   || sps->profile_idc == SCALABLE_HIGH		// 86
		   || sps->profile_idc == MVC_HIGH		// 118
		   || sps->profile_idc == STEREO_HIGH )		// 128
		 && sps->chroma_format_idc == YUV444		// 3
		 && sps->separate_colour_plane_flag != 0 );

	log2_max_frame_num = sps->log2_max_frame_num_minus4 + 4;
	frame_mbs_only_flag = sps->frame_mbs_only_flag;
	pic_order_cnt_type = sps->pic_order_cnt_type;
	log2_max_pic_order_cnt_lsb = sps->log2_max_pic_order_cnt_lsb_minus4 + 4;

	if ( sps->vui_parameters_present_flag
	  && sps->vui_seq_parameters->timing_info_present_flag ) {
		num_units_in_tick	= sps->vui_seq_parameters->num_units_in_tick;
		time_scale		= sps->vui_seq_parameters->time_scale;
		fixed_frame_rate_flag	= sps->vui_seq_parameters->fixed_frame_rate_flag;
//		msgDbg( "decodeSPS(): num_units_in_tick: " + Buffer( num_units_in_tick ) );
//		msgDbg( "decodeSPS(): time_scale       : " + Buffer( time_scale ) );
//		msgDbg( "decodeSPS(): fixed_frame_rate : " + Buffer( fixed_frame_rate_flag ) );
	}

	if ( time_scale > 0 && num_units_in_tick > 0 ) {

		Uint32	tempRate = time_scale / 2;
		Uint32	tempScle = num_units_in_tick;

		if ( ! frmeRate ) {
			frmeRate = tempRate;
			frmeScle = tempScle;
		}
		else if ( frmeRate * tempScle != frmeScle * tempRate ) {
			msgWrn( "decodeSPS(): frmeRate/frmeScle mismatch! Sess: "
				+ Buffer( frmeRate ) + "/" + Buffer( frmeScle )
				+ ", sps: "
				+ Buffer( tempRate ) + "/" + Buffer( tempScle ) );
			frmeRate = tempRate;
			frmeScle = tempScle;
		}

//		msgDbg( "decodeSPS(): frame rate       : " + Buffer( frmeRate ) + "/"
//			+ Buffer( frmeScle ) );

	}

	Uint32	tempWdth = sps->getPictWidth();
	Uint32	tempHght = sps->getPictHeight();

	if ( ! pictWdth ) {
		pictWdth = tempWdth;
	}
	else if ( pictWdth != tempWdth ) {
		msgWrn( "decodeSPS(): pictWdth mismatch! Sess: "
			+ Buffer( pictWdth ) + ", sps: "
			+ Buffer( tempWdth ) );
		pictWdth = tempWdth;
	}

	if ( ! pictHght ) {
		pictHght = tempHght;
	}
	else if ( pictHght != tempHght ) {
		msgWrn( "decodeSPS(): pictHght mismatch! Sess: "
			+ Buffer( pictHght ) + ", sps: "
			+ Buffer( tempHght ) );
		pictHght = tempHght;
	}

//	msgDbg( "decodeSPS(): pictWdth: " + Buffer( pictWdth ) );
//	msgDbg( "decodeSPS(): pictHght: " + Buffer( pictHght ) );

}

// ============================================================================
