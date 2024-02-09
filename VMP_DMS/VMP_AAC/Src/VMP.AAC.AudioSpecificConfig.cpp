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
//	VMP.AAC.AudioSpecificConfig.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.ConfigInfoRegisterer.h"

#include "VMP.AAC.AudioSpecificConfig.h"
#include "VMP.AAC.BitStream.h"
#include "VMP.AAC.Defines.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AAC, AudioSpecificConfig )

// ============================================================================

const DL::TypeCPtr & AAC::AudioSpecificConfig::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "40caeb6a-9399-439d-8201-eff80720e5bb" ),
		"VMP.AAC.AudioSpecificConfig",
		ConfigInfo::getClassType() );

	return i;

}

// ============================================================================

AAC::AudioSpecificConfig::AudioSpecificConfig() :

	ConfigInfo( getClassType() ) {

}

// ============================================================================

void AAC::AudioSpecificConfig::decode(
	const	Buffer &	pData ) {

//	AudioSpecificConfig () {
//		audioObjectType = GetAudioObjectType();
//		samplingFrequencyIndex;				// 4 bslbf
//		if ( samplingFrequencyIndex == 0xf ) {
//			samplingFrequency;			// 24 uimsbf
//		}
//		channelConfiguration;				// 4 bslbf
//		sbrPresentFlag = -1;
//		if ( audioObjectType == 5 ) {
//			extensionAudioObjectType = audioObjectType;
//			sbrPresentFlag = 1;
//			extensionSamplingFrequencyIndex;	// 4 uimsbf
//			if ( extensionSamplingFrequencyIndex == 0xf )
//				extensionSamplingFrequency;	// 24 uimsbf
//			audioObjectType = GetAudioObjectType();
//		}
//		else {
//			extensionAudioObjectType = 0;
//		}
//		switch ( audioObjectType ) {
//		case 1:
//		case 2:
//		case 3:
//		case 4:
//		case 6:
//		case 7:
//		case 17:
//		case 19:
//		case 20:
//		case 21:
//		case 22:
//		case 23:
//			GASpecificConfig();
//			break:
//		case 8:
//			CelpSpecificConfig();
//			break;
//		case 9:
//			HvxcSpecificConfig();
//			break:
//		case 12:
//			TTSSpecificConfig();
//			break;
//		case 13:
//		case 14:
//		case 15:
//		case 16:
//			StructuredAudioSpecificConfig();
//			break;
//		case 24:
//			ErrorResilientCelpSpecificConfig();
//			break;
//		case 25:
//			ErrorResilientHvxcSpecificConfig();
//			break;
//		case 26:
//		case 27:
//			ParametricSpecificConfig();
//			break;
//		case 28:
//			SSCSpecificConfig();
//			break;
//		case 32:
//		case 33:
//		case 34:
//			MPEG_1_2_SpecificConfig();
//			break;
//		case 35:
//			DSTSpecificConfig();
//			break;
//		default:
//			/* reserved */
//		}
//		switch ( audioObjectType ) {
//		case 17:
//		case 19:
//		case 20:
//		case 21:
//		case 22:
//		case 23:
//		case 24:
//		case 25:
//		case 26:
//		case 27:
//			epConfig;				// 2 bslbf
//			if ( epConfig == 2 || epConfig == 3 ) {
//				ErrorProtectionSpecificConfig();
//			}
//			if ( epConfig == 3 ) {
//				directMapping;			// 1 bslbf
//				if ( ! directMapping ) {
//					/* tbd */
//				}
//			}
//		}
//		if ( extensionAudioObjectType != 5 && bits_to_decode() >= 16 ) {
//			syncExtensionType;			// 11 bslbf
//			if ( syncExtensionType == 0x2b7 ) {
//				extensionAudioObjectType = GetAudioObjectType();
//				if ( extensionAudioObjectType == 5 ) {
//					sbrPresentFlag;		// 1 uimsbf
//					if ( sbrPresentFlag == 1 ) {
//						extensionSamplingFrequencyIndex;
//								// 4 uimsbf
//						if ( extensionSamplingFrequencyIndex == 0xf )
//							extensionSamplingFrequency;
//								// 24 uimsbf
//					}
//				}
//			}
//		}
//	}

//	msgDbg( "decode(): " + Utils::toHex( pData ) );

	dataCopy = pData;

	BitStreamPtr	iBitStrm = new BitStream( pData );

	objtType = getAudioObjectType( iBitStrm );

	if ( objtType < AAC_main /* 1 */ || objtType > AAC_LC /* 2 */ ) {
		throw NotImplemented( "Only AAC main or LC is supported! (requested: "
			+ Buffer( objtType ) + ")" );
	}

	smplIndx = iBitStrm->read_u_v( 4 );
	smplFreq = ( smplIndx == 0xf ? iBitStrm->read_u_v( 24 ) : cSmplTbl[ smplIndx ] );

	if ( ! smplFreq ) {
		throw InternalError( "Invalid sampling freq!" );
	}

	chanCnfg = iBitStrm->read_u_v( 4 );

// FIXME: partial support...

	// Skip GASpecificConfig...

	if ( iBitStrm->read_u_v( 3 ) ) {
		throw NotImplemented( "Non-empty GASpecificConfig not supported!" );
	}

	if ( iBitStrm->tell() != iBitStrm->length() ) {
//		throw InternalError( "Junk at end!" );
		msgWrn( "decode(): Junk at end!" );
	}

}

Buffer AAC::AudioSpecificConfig::encode() const {

	Buffer res;

	res += ( Uchar )( ( objtType << 3 ) | ( smplIndx >> 1 ) );
	res += ( Uchar )( ( smplIndx << 7 ) | ( chanCnfg << 3 ) );

	return res;

}

// ============================================================================

void AAC::AudioSpecificConfig::setSamplingFrequency(
	const	Uint32		pSmplFreq ) {

	for ( Uint32 i = 0 ; i < 16 ; i++ ) {
		if ( cSmplTbl[ i ] == pSmplFreq ) {
			smplIndx = i;
			smplFreq = pSmplFreq;
			return;
		}
	}

}

// ============================================================================

Buffer AAC::AudioSpecificConfig::toBuffer() const {

	return "ObjtType: " + Buffer( objtType )
		+ ", smplIndx; " + Buffer( smplIndx )
		+ ", smplFreq: " + Buffer( smplFreq )
		+ ", chanCnfg: " + Buffer( chanCnfg );

}

// ============================================================================

Uint32 AAC::AudioSpecificConfig::getSamplingFrequency(
	const	Uint32		pSmplIndx ) {

	if ( pSmplIndx >= 16 ) {
		throw InvalidArgument();
	}

	return cSmplTbl[ pSmplIndx ];

}

Uint32 AAC::AudioSpecificConfig::getSamplingIndex(
	const	Uint32		pSmplFreq ) {

	for ( Uint32 i = 0 ; i < 16 ; i++ ) {
		if ( cSmplTbl[ i ] == pSmplFreq ) {
			return i;
		}
	}

	throw InvalidArgument();

}

// ============================================================================

Uint32 AAC::AudioSpecificConfig::getAudioObjectType(
		BitStreamPtr	pIBitStrm ) {

//	GetAudioObjectType() {
//		audioObjectType;				// 5 uimsbf
//		if ( audioObjectType == 31 ) {
//			audioObjectType = 32 + audioObjectTypeExt;
//								// 6 uimsbf
//		}
//		return audioObjectType;
//	}

	Uint32 res = pIBitStrm->read_u_v( 5 );

	if ( res == 31 ) {
		res = 32 + pIBitStrm->read_u_v( 6 );
	}

	return res;

}

// ============================================================================

const Uint32 AAC::AudioSpecificConfig::cSmplTbl[ 16 ] = {
	96000,		// 0x0: 
	88200,		// 0x1: 
	64000,		// 0x2: 
	48000,		// 0x3: 
	44100,		// 0x4: 
	32000,		// 0x5: 
	24000,		// 0x6: 
	22050,		// 0x7: 
	16000,		// 0x8: 
	12000,		// 0x9: 
	11025,		// 0xa: 
	 8000,		// 0xb: 
	 7350,		// 0xc: 
	    0,		// 0xd: reserved
	    0,		// 0xe: reserved
	    0,		// 0xf: escape value

};

// ============================================================================

static ConfigInfoRegisterer< AAC::AudioSpecificConfig > registerer;

// ============================================================================

