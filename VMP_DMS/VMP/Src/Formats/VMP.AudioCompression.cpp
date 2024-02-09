// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.AudioCompression.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.AudioCompression.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

AudioCompression::ConvEntry AudioCompression::tbl[] = {

	{ WMA1,		"WMA1" },
	{ WMA2,		"WMA2" },

	{ VORBIS,	"vrbs" },
	{ SPEEX,	"spex" },
	{ FLAC,		"flac" },

	{ AAC,		"mp4a" },

	{ Unknown,	"????" }

};

const char *AudioCompression::nms[] = {

	"Unknown",
	"Uncompressed",

	"MP3",

	"WMA1",
	"WMA2",

	"Vorbis",
	"Speex",
	"FLAC",

	"AAC"

};

// ============================================================================

AudioCompression::AudioCompression(const BFC::FourCC& c) {

	BFC::Uint32 i = 0;

	while (tbl[i].format != Unknown && c != BFC::FourCC(tbl[i].fourCC)) {
		i++;
	}

	f = tbl[i].format;

}

BFC::FourCC AudioCompression::fourCC() const {

	BFC::Uint32 i = 0;

	while (tbl[i].format != f && tbl[i].format != Unknown)
		i++;

	return BFC::FourCC(tbl[i].fourCC);

}

// ============================================================================

void AudioCompression::doSerialize(
		BFC::IO::ByteOutputStreamPtr	out ) const {

	out->putLEUint32( ( Uint32 )f );

}

void AudioCompression::unSerialize(
		BFC::IO::ByteInputStreamPtr	in ) {

	f = ( Format )in->getLEUint32();

}

// ============================================================================

