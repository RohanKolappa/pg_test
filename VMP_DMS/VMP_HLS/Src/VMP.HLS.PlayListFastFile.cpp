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
//	VMP.HLS.PlayListFastFile.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.IO.File.h"
#include "BFC.IO.MemoryOutputStream.h"

#include "BFC.VFS.FSNode.h"

#include "VMP.HLS.PlayListFastFile.h"

#include "VMP.SegUtils.SegmentFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, PlayListFastFile )

// ============================================================================

HLS::PlayListFastFile::PlayListFastFile(
	const	FSPath &	pOutptDir,
	const	Buffer &	pM3u8Name,
	const	Frac64 &	pTrgtDrtn,
	const	Buffer &	pHttpPrfx,
	const	Bool		pFakeClse,
	const	Bool		pSyncWrte ) :

	outptDir	( pOutptDir ),
	m3u8Name	( pM3u8Name ),
	m3u8Path	( pOutptDir + m3u8Name ),
	trgtDrtn	( pTrgtDrtn ),
	httpPrfx	( pHttpPrfx ),
	fakeClse	( pFakeClse ),
	syncWrte	( pSyncWrte ),
	nmbrWrtn	( 0 ),
	nmbrDltd	( 0 ) {

	setObjectName( "VMP.HLS.PlayListFastFile" );

}

HLS::PlayListFastFile::~PlayListFastFile() {

	try {
		if ( m3u8File ) {
			close();
		}
	}
	catch ( Exception & ) {
	}

}

// ============================================================================

void HLS::PlayListFastFile::create() {

	// Sanity checks...

	if ( m3u8File ) {
		throw AlreadyInitialized();
	}

	// Cleanup previous playlist file...

	FSNode	m3u8Node;

	m3u8Node.setPath( m3u8Path );

	if ( m3u8Node.exists() ) {
		Uint64	prevSize = m3u8Node.getTotalSize();
//		msgDbg( "writeFile(): overwriting previous playlist of " + Buffer( prevSize ) + " bytes!" );
		FastCSLocker locker( critSect );
		nmbrDltd += prevSize;
	}

	// Open new playlist file...

	m3u8File = new IO::File( m3u8Path.toBuffer(), IO::File::New | ( syncWrte ? IO::File::Sync : 0 ) );

	m3u8File->open();

	m3u8File->putBytes( "#EXTM3U\n" );

	BufferArray hderList = getHeaderList();

	for ( Uint32 i = 0 ; i < hderList.getSize() ; i++ ) {
		m3u8File->putBytes( "# " + hderList[ i ] + "\n" );
	}

	m3u8File->putBytes( "#EXT-X-TARGETDURATION:"
			+ Buffer( trgtDrtn.ceil().toInt64() ) + "\n" );
	m3u8File->putBytes( "#EXT-X-VERSION:3\n" );
	m3u8File->putBytes( "#EXT-X-ALLOW-CACHE:NO\n" );

	if ( fakeClse ) {
		m3u8File->putBytes( "#EXT-X-PLAYLIST-TYPE:VOD\n" );
	}
	else {
		eventPos = m3u8File->tell();
		m3u8File->putBytes( "#EXT-X-PLAYLIST-TYPE:EVENT\n" );
	}

	if ( fakeClse ) {
		Uint64 t = m3u8File->tell();
		m3u8File->putBytes( "#EXT-X-ENDLIST\n" );
		m3u8File->seek( t );
	}

	m3u8File->flush();

	// Update disk stats...

	Uint64	m3u8Size = m3u8File->tell();

//	msgDbg( "writeFile(): playlist is " + Buffer( tempSize ) + " bytes long!" );

	{
		FastCSLocker locker( critSect );
		nmbrWrtn += m3u8Size;
	}

}

void HLS::PlayListFastFile::update(
		SegUtils::SegmentFrameCPtr
				pSegmFrme ) {

	Uint64	prevSize = m3u8File->tell();

	m3u8File->putBytes( "#EXTINF:" + Buffer( pSegmFrme->getSegmentDuration().toDouble() ) + ",\n"
			+ httpPrfx + pSegmFrme->getFileName() + "\n" );

	if ( fakeClse ) {
		Uint64 t = m3u8File->tell();
		m3u8File->putBytes( "#EXT-X-ENDLIST\n" );
		m3u8File->seek( t );
	}

	m3u8File->flush();

	Uint64	currSize = m3u8File->tell();

//	msgDbg( "writeFile(): playlist is " + Buffer( tempSize ) + " bytes long!" );

	{
		FastCSLocker locker( critSect );
		nmbrWrtn += ( currSize - prevSize );
	}

}

void HLS::PlayListFastFile::close() {

	Uint64	prevSize = m3u8File->tell();

	m3u8File->putBytes( "#EXT-X-ENDLIST\n" );

	Uint64	currSize = m3u8File->tell();

	if ( ! fakeClse ) {
		try {
			m3u8File->seek( eventPos );
			m3u8File->putBytes( "#EXT-X-PLAYLIST-TYPE:VOD\n#\n" );
		}
		catch ( Exception & ) {
		}
	}

	m3u8File->flush();
	m3u8File->close();

	m3u8File.kill();

	{
		FastCSLocker locker( critSect );
		nmbrWrtn += ( currSize - prevSize );
	}

}

// ============================================================================

void HLS::PlayListFastFile::getDiskStats(
		Uint64 &	pNmbrWrtn,
		Uint64 &	pNmbrDltd ) {

	FastCSLocker locker( critSect );

	pNmbrWrtn = nmbrWrtn;
	pNmbrDltd = nmbrDltd;

}

// ============================================================================

