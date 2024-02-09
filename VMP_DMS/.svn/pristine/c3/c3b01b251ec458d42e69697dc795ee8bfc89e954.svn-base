// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Image".
// 
// "BFC::Image" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Image" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Image"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Image.ConverterManager.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

//#include <iostream>

#include "BFC.Base.BufferIO.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.SPtrImpl.h"

#include "BFC.Image.ConverterManager.h"
#include "BFC.Image.ConverterStrategy.h"
#include "BFC.Image.PictureFormat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::ConverterManager::ConverterManager() :

	convertersChanged( false ) {

	Uint32 nbrFormats = PictureFormat::MaxFormat - PictureFormat::MinFormat;

	strategies.resize( nbrFormats, nbrFormats );

}

// ============================================================================

Image::ConverterManagerPtr Image::ConverterManager::instance() {

	static ConverterManagerPtr i = new ConverterManager;

	return i;

}

// ============================================================================

void Image::ConverterManager::addConverter(
		ConverterPtr	converter,
	const	Bool		pPreferred ) {

	FastCSLocker	locker( lockable );

	if ( pPreferred ) {
		converters.insertFirst( converter );
	}
	else {
		converters.insertLast( converter );
	}

	convertersChanged = true;

}

// ============================================================================

Image::ConverterStrategyPtr Image::ConverterManager::getStrategyFor(
	const	PictureFormat &	iFormat,
	const	PictureFormat &	oFormat ) {

	FastCSLocker	locker( lockable );

	Uint32	col = iFormat.getPredefined() - PictureFormat::MinFormat;
	Uint32	row = oFormat.getPredefined() - PictureFormat::MinFormat;

	if ( convertersChanged ) {
		computeStrategies();
		convertersChanged = false;
	}

	return strategies( col, row );

}

void Image::ConverterManager::computeStrategies() {

//	std::cerr << "computeStrategies(): got "
//		<< converters.getSize()
//		<< " converters!" << std::endl;
//	for ( Uint32 i = 0 ; i < converters.getSize() ; i++ ) {
//		std::cerr << converters[ i ]->getIFormat().getShortName()
//			<< "\t"
//			<< converters[ i ]->getOFormat().getShortName()
//			<< std::endl;
//	}

	// 1. Resize matrix of strategies...

	Uint32 nbrFormats = PictureFormat::MaxFormat - PictureFormat::MinFormat;
	Uint32 fmtsToFind = nbrFormats * ( nbrFormats - 1 );

	strategies.kill();
	strategies.resize( nbrFormats, nbrFormats );

	ConverterStrategyList	candidates;

	// 1. Create list of empty strategies, one for each iformat. Those
	//    strategies, of length 0, are basically no-op.

	for ( Uint32 f = PictureFormat::MinFormat
	    ; f < PictureFormat::MaxFormat
	    ; f++ ) {
		candidates.insertLast(
			new ConverterStrategy( ( PictureFormat::Format )f ) );
	}

	for (;;) { // [almost] infinite loop...

		// If no more candidates, exit!

		if ( candidates.isEmpty() ) {
			break;
		}

		// Take all candidates, remove them from the list of
		// candidates, and append all possible new strategies formed
		// by concatenating those candidates with compatible
		// converters.

		Uint32 nbr = candidates.size();

		while ( nbr-- ) {
			ConverterStrategyPtr currCand = candidates.popFirst();
			for ( ConverterList::Iterator it = converters.firstElt()
			    ; it
			    ; it++ ) {
				if ( currCand->canAdd( *it ) ) {
					candidates.insertLast(
						new ConverterStrategy( currCand, *it ) );
				}
			}
		}

		// Fill strategies matrix with new paths, if any! And remove
		// duplicates/longer ones...

		for ( ConverterStrategyList::Iterator it = candidates.firstElt()
		    ; it
		    ; ) {

			Uint32	col = (*it)->getIFormat().getPredefined()
				    - PictureFormat::MinFormat;
			Uint32	row = (*it)->getOFormat().getPredefined()
				    - PictureFormat::MinFormat;
			if ( strategies( col, row ) ) {
				// There is already a shorter route!
				candidates.pop( it );
			}
			else {
				strategies( col, row ) = *it;
				fmtsToFind--;
				it++;
			}

		}

		if ( ! fmtsToFind ) {
			break;
		}

	}

	// 3. By now, all entries in 'strategies' should have been filled!

	if ( fmtsToFind ) {
		throw InternalError( "Image::ConverterManager: unconnected graph of converters!!!" );
	}

}

// ============================================================================

void Image::ConverterManager::forcePopulate() {

	FastCSLocker	locker( lockable );

	computeStrategies();

}

// ============================================================================

Image::PictureFormat Image::ConverterManager::selectNearestFormat(
	const	PictureFormat &	pInputFormat,
	const	Array< PictureFormat > &
				pCandidates ) {

	if ( pCandidates.isEmpty() ) {
		throw InvalidArgument();
	}

	for ( Uint32 i = 0 ; i < pCandidates.getSize() ; i++ ) {
		if ( pInputFormat == pCandidates[ i ] ) {
			return pInputFormat;
		}
	}

	PictureFormat	res = pCandidates[ 0 ];
	Uint32		len = getStrategyFor( pInputFormat, res )->getLength();

	for ( Uint32 i = 1 ; len && i < pCandidates.getSize() ; i++ ) {
		PictureFormat	newRes = pCandidates[ i ];
		Uint32		newLen = getStrategyFor( pInputFormat, newRes )->getLength();
		if ( newLen < len ) {
			len = newLen;
			res = newRes;
		}
	}

	return res;

}

// ============================================================================

