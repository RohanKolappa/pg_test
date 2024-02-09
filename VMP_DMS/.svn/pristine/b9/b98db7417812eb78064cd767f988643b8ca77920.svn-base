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
//	VMP.RTP.SEStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.TL.BufferArray.h"

#include "VMP.RTP.NSession.h"
#include "VMP.RTP.SBandwidth.h"
#include "VMP.RTP.SConnInfo.h"
#include "VMP.RTP.SEStream.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, SEStream )

// ============================================================================

Buffer RTP::SEStream::toBuffer() const {

	Buffer	res;

	res += "--------";
	res += "\nSEStream";
	res += "\n--------";

	res += "\nMeta type       : " + getMetaType();
	res += "\nStream port     : " + Buffer( getStreamPort() )
			+ ( nbrPorts > 1 ? "/" + Buffer( nbrPorts ) : Buffer() );
	res += "\nStream profile  : " + getStreamProfile();
	res += "\nData type       : " + Buffer( getDataType() );

	if ( hasEStreamInfo() ) {
		res += "\nStream info     : " + getEStreamInfo();
	}

	if ( hasConnectionInfo() ) {
		res += "\nConnection info : " + getConnectionInfo()->encode();
	}

	if ( hasBandwidth() ) {
		res += "\nBandwidth       : " + getBandwidth()->encode();
	}

	if ( hasEncoding() ) {
		res += "\nEncoding        : " + getEncoding();
	}

	if ( hasFormat() ) {
		res += "\nFormat          : " + getFormat();
	}

	if ( hasFrameRate() ) {
		res += "\nFramerate       : " + getFrameRate().toBuffer( false );
	}

	return res;

}

// ============================================================================

StreamPtr RTP::SEStream::toStream() const {

	StreamPtr		outpStrm;
	BufferArray		codcList = BufferParser::tokenize( strmCodc, "/" );

	if ( codcList.getSize() < 2 ) {
		throw InternalError();
	}

	if ( metaType == "video"
	  && codcList[ 0 ].toLower() == "h264" ) {
		VideoStreamPtr		oVidStrm = new VideoStream;
		oVidStrm->setCompression( VideoCompression::H264 );
		outpStrm = oVidStrm;
	}
	else if ( metaType == "audio"
	  && ( codcList[ 0 ].toLower() == "mpeg4-generic"
	    || codcList[ 0 ].toLower() == "mp4a-latm" ) ) {
		AudioStreamPtr		oAudStrm = new AudioStream;
		oAudStrm->setCompression( AudioCompression::AAC );
		outpStrm = oAudStrm;
	}
	else {
		throw InternalError( "Unsupported stream!" );
	}

	return outpStrm;

}

// ============================================================================

