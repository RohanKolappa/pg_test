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
//	VMP.HLS.PlayListSlowFile.cpp
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

#include "VMP.HLS.PlayListSlowFile.h"

#include "VMP.SegUtils.SegmentFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, PlayListSlowFile )

// ============================================================================

HLS::PlayListSlowFile::PlayListSlowFile(
	const	FSPath &	pOutptDir,
	const	Buffer &	pM3u8Name,
	const	Uint32		pListSize,
	const	Frac64 &	pTrgtDrtn,
	const	Buffer &	pHttpPrfx,
	const	Bool		pFakeClse,
	const	Bool		pSyncWrte ) :

	outptDir	( pOutptDir ),
	m3u8Name	( pM3u8Name ),
	m3u8Path	( pOutptDir + m3u8Name ),
	tempName	( pM3u8Name + ".tmp" ),
	tempPath	( pOutptDir + tempName ),
	listSize	( pListSize ),
	trgtDrtn	( pTrgtDrtn ),
	httpPrfx	( pHttpPrfx ),
	fakeClse	( pFakeClse ),
	syncWrte	( pSyncWrte ),
	nmbrWrtn	( 0 ),
	nmbrDltd	( 0 ) {

	setObjectName( "VMP.HLS.PlayListSlowFile" );

}

HLS::PlayListSlowFile::~PlayListSlowFile() {

	try {
		close();
	}
	catch ( Exception & ) {
	}

}

// ============================================================================

void HLS::PlayListSlowFile::create() {

	segmList.kill();
	drtyList.kill();

	writeFile( fakeClse );

}

void HLS::PlayListSlowFile::update(
		SegUtils::SegmentFrameCPtr
				pSegmFrme ) {

// FIXME: Not compliant with draft 12:

	// "The server MUST NOT remove a media segment from the Playlist
	// file if the duration of the Playlist file minus the duration of
	// the segment is less than three times the target duration."

	segmList += pSegmFrme;

	adjustWindow();

	writeFile( fakeClse );

	// "When the server removes a media segment from the Playlist, the
	// corresponding media URI SHOULD remain available to clients for
	// a period of time equal to the duration of the segment plus the
	// duration of the longest Playlist file distributed by the server
	// containing that segment."

	// BUT:
	// Issue VNG0001-1596: Need to ensure that the extra segments kept
	// outside of the sliding window should not be more than few seconds
	// extra rather than double the sliding window size (as suggested by
	// the HLS spec).

	while ( drtyList.getSize() > 3 ) {
		eraseFile( outptDir + drtyList.popFirst()->getFileName() );
	}

}

void HLS::PlayListSlowFile::close() {

	writeFile( true );

	while ( drtyList.hasContent() ) {
		eraseFile( outptDir + drtyList.popFirst()->getFileName() );
	}

}

// ============================================================================

void HLS::PlayListSlowFile::getDiskStats(
		Uint64 &	pNmbrWrtn,
		Uint64 &	pNmbrDltd ) {

	FastCSLocker locker( critSect );

	pNmbrWrtn = nmbrWrtn;
	pNmbrDltd = nmbrDltd;

}

// ============================================================================

void HLS::PlayListSlowFile::writeFile(
	const	Bool		pLastShot ) {

	FSNode		tempNode;

	tempNode.setPath( tempPath );

	if ( tempNode.exists() ) {
		msgWrn( "writeFile(): overwriting temp file!" );
		FastCSLocker locker( critSect );
		nmbrDltd += tempNode.getTotalSize();
	}

	IO::MemoryOutputStreamPtr	m3u8Strm = new IO::MemoryOutputStream;

	m3u8Strm->open();

	m3u8Strm->putBytes( "#EXTM3U\n" );

	BufferArray hderList = getHeaderList();

	for ( Uint32 i = 0 ; i < hderList.getSize() ; i++ ) {
		m3u8Strm->putBytes( "# " + hderList[ i ] + "\n" );
	}

	m3u8Strm->putBytes( "#EXT-X-TARGETDURATION:"
			+ Buffer( trgtDrtn.ceil().toInt64() ) + "\n" );
	m3u8Strm->putBytes( "#EXT-X-VERSION:3\n" );
	m3u8Strm->putBytes( "#EXT-X-ALLOW-CACHE:NO\n" );

	if ( pLastShot ) {
		m3u8Strm->putBytes( "#EXT-X-PLAYLIST-TYPE:VOD\n" );
	}
	else if ( ! listSize ) {
		m3u8Strm->putBytes( "#EXT-X-PLAYLIST-TYPE:EVENT\n" );
	}

	// "If the Media Playlist contains the EXT-X-MEDIA-SEQUENCE tag, the
	// client SHOULD assume that each media segment in it will become
	// unavailable at the time that the Playlist file was loaded plus the
	// duration of the Playlist file."

	if ( listSize ) {
		m3u8Strm->putBytes( "#EXT-X-MEDIA-SEQUENCE:"
			+ Buffer( segmList.hasContent() ?
				segmList[ 0 ]->getSegmentIndex() : 0 )
			+ "\n" );
	}

	for ( Uint32 i = 0 ; i < segmList.getSize() ; i++ ) {
		m3u8Strm->putBytes( "#EXTINF:" );
		m3u8Strm->putBytes( Buffer( segmList[ i ]->getSegmentDuration().toDouble() ) );
		m3u8Strm->putBytes( ",\n" );
		m3u8Strm->putBytes( httpPrfx );
		m3u8Strm->putBytes( segmList[ i ]->getFileName() );
		m3u8Strm->putBytes( "\n" );
	}

	if ( pLastShot ) {
		m3u8Strm->putBytes( "#EXT-X-ENDLIST\n" );
	}

	m3u8Strm->flush();

	Buffer	m3u8Data = m3u8Strm->getContent();

	m3u8Strm->close();

	m3u8Strm.kill();

	IO::FilePtr	m3u8File = new IO::File( tempPath.toBuffer(), IO::File::New | ( syncWrte ? IO::File::Sync : 0 ) );

	m3u8File->open();
	m3u8File->putBytes( m3u8Data );
	m3u8File->close();

	tempNode.refresh();

	Uint64	tempSize = tempNode.getTotalSize();

//	msgDbg( "writeFile(): playlist is " + Buffer( tempSize ) + " bytes long!" );

	{
		FastCSLocker locker( critSect );
		nmbrWrtn += tempSize;
	}

	FSNode	prevNode;

	prevNode.setPath( m3u8Path );

	if ( prevNode.exists() ) {
		Uint64	prevSize = prevNode.getTotalSize();
//		msgDbg( "writeFile(): overwriting previous playlist of " + Buffer( prevSize ) + " bytes!" );
		FastCSLocker locker( critSect );
		nmbrDltd += prevSize;
	}

	tempNode.rename( m3u8Name );

}

void HLS::PlayListSlowFile::eraseFile(
	const	FSPath &	pFilePath ) {

	FSNode	fileNode;

	fileNode.setPath( pFilePath );

	Uint64	fileSize = fileNode.getTotalSize();

	try {
		fileNode.remove();
	}
	catch ( ThreadStopped & ) {
		throw;
	}
	catch ( Exception & ) {
		return;
	}

	FastCSLocker locker( critSect );

	nmbrDltd += fileSize;

}

// ============================================================================

void HLS::PlayListSlowFile::adjustWindow() {

	if ( ! listSize ) {
		return;
	}

	// Compute duration of playlist...

	Double	currSize = 0.0;

	for ( Uint32 i = 0 ; i < segmList.getSize() ; i++ ) {
		currSize += segmList[ i ]->getSegmentDuration().toDouble();
	}

//	msgDbg( "adjustWindow(): playlist duration: " + Buffer( currSize ) );

	// Remove first segments until playlist duration fits into expected
	// window size...

	for (;;) {

		SegUtils::SegmentFrameCPtr	s = segmList.getFirst();
		Double				d = s->getSegmentDuration().toDouble();

		if ( currSize - d < ( Double )listSize ) {
			break;
		}

		segmList.popFirst();

		currSize -= d;
		drtyList += s;

//		msgDbg( "adjustWindow(): removed: " + Buffer( d )
//			+ " ---> new duration: " + Buffer( currSize ) );

	}

}

// ============================================================================

