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
//	VMP.HLS.ServerConfig.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "VMP.HLS.ServerConfig.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, ServerConfig )

// ============================================================================

HLS::ServerConfig::ServerConfig() :

	iVidPort	( 0 ),
	iAudPort	( 0 ),
	vrblSSRC	( false ),
	pvrLngth	( 0 ),
	sncWrite	( false ),
	fakeClse	( false ) {

	setObjectName( "VMP.HLS.ServerConfig" );

}

// ============================================================================

void HLS::ServerConfig::loadArgs(
	const	BufferArray &	pCmndLine ) {

	Uint32 i = 0;

	progName	= pCmndLine[ i++ ];

	while ( i < pCmndLine.getSize() ) {
		Buffer	arg = pCmndLine[ i++ ];
		if ( i >= pCmndLine.getSize() ) {
			throw InvalidArgument( "Missing value for argument \"" + arg + "\"!" );
		}
		Buffer	val = pCmndLine[ i++ ];
		if ( arg == "--pid_file" ) {
			pidFName = val;
		}
		else if ( arg == "--input_sdp_file" ) {
			iSDPFile = val;
		}
		else if ( arg == "--video_addr" ) {
			iVidAddr = val;
		}
		else if ( arg == "--video_port" ) {
			iVidPort = val.toUint16();
		}
		else if ( arg == "--audio_addr" ) {
			iAudAddr = val;
		}
		else if ( arg == "--audio_port" ) {
			iAudPort = val.toUint16();
		}
		else if ( arg == "--output_formats" ) {
			outpFrmt = BufferParser::tokenize( val, "," );
		}
		else if ( arg == "--segment_duration" ) {
			segmDrtn = val.toDouble();
		}
		else if ( arg == "--output_dir" ) {
			outptDir = val;
		}
		else if ( arg == "--pvr_length" ) {
			pvrLngth = val.toUint32();
		}
		else if ( arg == "--base_url" ) {
			httpPrfx = val;
		}
		else if ( arg == "--base_indexfile_name" ) {
			m3u8Name = val;
		}
		else if ( arg == "--base_segmentfile_name" ) {
			segmPrfx = val;
		}
		else if ( arg == "--sync_write" ) {
			sncWrite = val.toBool();
		}
		else if ( arg == "--fake_close" ) {
			fakeClse = val.toBool();
		}
		else {
			throw InvalidArgument( "Unsupported argument \"" + arg + "\"!" );
		}
	}

//	if ( iSDPFile.isEmpty() ) {
//		throw InternalError( "Missing SDP file path!" );
//	}

	if ( outpFrmt.isEmpty() ) {
		throw InternalError( "Missing output format list!" );
	}

//	if ( outptDir.isEmpty() ) {
//		outptDir = ".";
//		msgWrn( "Using default output path: \"" + outptDir + "\"!" );
//	}

	if ( segmPrfx.isEmpty() ) {
		segmPrfx	= "segment_";
//		msgWrn( "Using default segment prefix: \"" + segmPrfx + "\"!" );
	}

	if ( ! segmDrtn ) {
		segmDrtn	= 1.0;
//		msgWrn( "Using default segment duration: \"" + Buffer( segmDrtn ) + "\"!" );
	}

	if ( m3u8Name.isEmpty() ) {
		throw InternalError( "Missing playlist file name!" );
	}

}

// ============================================================================

Buffer HLS::ServerConfig::getUsage() const {

	return	"Usage: " + progName + " <options>\n"
		"where <options> are:\n"
		"  --pid_file <path>              : Path to file where to write the PID.\n"
		"  --input_sdp_file <path>        : Path to SDP file to use as reference.\n"
		"  --output_dir <path>            : Directory where converted media files are\n"
		"                                   to be created.\n"
		"  --video_addr <mcast>           : IP multicast address of video data.\n"
		"  --video_port <port>            : UDP port to listen on for streaming RTP\n"
		"                                   video data.\n"
		"  --audio_addr <mcast>           : IP multicast address of audio data.\n"
		"  --audio_port <port>            : UDP port to listen on for streaming RTP\n"
		"                                   audio data.\n"
		"  --output_formats <list>        : Comma-separated list of output formats.\n"
		"                                   Supported formats are: HLS, DASH.\n"
		"  --segment_duration <secs>      : Segment duration (floating point, in secs).\n"
		"  --pvr_length <duration>        : The number of seconds of media to hold in\n"
		"                                   the PVR buffer. Media segments older than\n"
		"                                   these will be deleted. Example: 600 means\n"
		"                                   hold 10 minutes worth of media. A special\n"
		"                                   value of zero would mean never delete any\n"
		"                                   files (infinite PVR duration).\n"
		"  --base_url <url>               : The base URL to be plugged into the index\n"
		"                                   file. The index file will be munged at file\n"
		"                                   serving time with the actual IP address,\n"
		"                                   media ID and authentication parameters.\n"
		"  --base_indexfile_name <name>   : Base name to use for the index file\n"
		"                                   (e.g. \"output.m3u8\").\n"
		"  --base_segmentfile_name <name> : Base name to use for the segment files\n"
		"                                   (e.g. \"segment-\").\n"
		"  --sync_write <bool>            : Use synchronous disk writes ?\n"
		"  --fake_close <bool>            : Write complete playlists (with ENDLIST tag) ?";

}

// ============================================================================

