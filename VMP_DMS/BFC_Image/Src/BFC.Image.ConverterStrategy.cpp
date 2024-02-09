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
//	BFC.Image.ConverterStrategy.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferIO.h"

#include "BFC.Image.Converter.h"
#include "BFC.Image.ConverterStrategy.h"
#include "BFC.Image.Picture.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::ConverterStrategy::ConverterStrategy() {

	iFormat = PictureFormat::Unknown;

}

Image::ConverterStrategy::ConverterStrategy(
	const	PictureFormat&	__iFormat) {

	iFormat = __iFormat;

}

Image::ConverterStrategy::ConverterStrategy(
	const	ConverterStrategy&	s)
	: SObject() {

	iFormat = s.iFormat;
	entries = s.entries;

}

Image::ConverterStrategy::ConverterStrategy(
	const	ConverterStrategyPtr	& __other,
	const	ConverterPtr		& __converter ) {

	iFormat = __other->iFormat;
	entries = __other->entries;

	Entry e;

	e.forw = ( getOFormat() == __converter->getIFormat() );
	e.conv = __converter;

	entries.insertLast( e );

}

// ============================================================================

Image::ConverterStrategy& Image::ConverterStrategy::operator = (
	const	ConverterStrategy&	s) {

	if ( this != &s ) {
		iFormat = s.iFormat;
		entries = s.entries;
	}

	return *this;

}

// ============================================================================

Bool Image::ConverterStrategy::operator == (
	const	ConverterStrategy&	s) const {

	return ( getLength() == s.getLength()
	      && getIFormat() == s.getIFormat()
	      && getOFormat() == s.getOFormat() );

}

Bool Image::ConverterStrategy::operator != (
	const	ConverterStrategy&	s) const {

	return ( ! ( *this == s ) );

}

// ============================================================================

Image::PictureFormat Image::ConverterStrategy::getIFormat() const {

	return iFormat;

}

Image::PictureFormat Image::ConverterStrategy::getOFormat() const {

	if ( entries.isEmpty() ) {
		return iFormat;
	}

	return ( entries.getLast().forw
		? entries.getLast().conv->getOFormat()
		: entries.getLast().conv->getIFormat() );

}

Bool Image::ConverterStrategy::isEmpty() const {

	return ( entries.isEmpty() );

}

Uint32 Image::ConverterStrategy::getLength() const {

	return ( entries.size() );

}

Bool Image::ConverterStrategy::canAdd(
		ConverterPtr	c) const {

	// One of the candidate's formats must match the actual oformat!

	PictureFormat tmpFormat = getOFormat();

	if ( tmpFormat != c->getIFormat()
	  && tmpFormat != c->getOFormat() ) {
		return false;
	}

	// No loop should be created!

	PictureFormat newFormat =
		( c->getIFormat() == tmpFormat
		? c->getOFormat()
		: c->getIFormat() );

	if ( newFormat == iFormat ) {
		return false;
	}

	for ( EntryList::CstIterator it = entries.firstElt() ; it ; it++ ) {
		if ( it->conv->getIFormat() == newFormat
		  || it->conv->getOFormat() == newFormat ) {
			return false;
		}
	}

#if 0

	// No intermediary downscaling should be allowed! As a side-effect,
	// this ensures that in order to be sure that a strategy can
	// convert a picture, it's enough to test if the initial & final
	// formats can represent the picture (can cope with its dims)...

	Uint32 idownx = iFormat.getMaxXDownsampling();
	Uint32 idowny = iFormat.getMaxYDownsampling();
	Uint32 odownx = newFormat.getMaxXDownsampling();
	Uint32 odowny = newFormat.getMaxYDownsampling();

	Uint32 mdownx = ( idownx > odownx ? idownx : odownx );
	Uint32 mdowny = ( idowny > odowny ? idowny : odowny );

	for ( i = 0 ; i < entries.size() ; i++ ) {
		PictureFormat ifmt = entries[i].conv->getIFormat();
		PictureFormat ofmt = entries[i].conv->getOFormat();
		if ( ifmt.getMaxXDownsampling() > mdownx
		  || ifmt.getMaxYDownsampling() > mdowny
		  || ofmt.getMaxXDownsampling() > mdownx
		  || ofmt.getMaxYDownsampling() > mdowny ) {
			cerr << "Rejecting: beg: " << iFormat.getShortName()
				<< ", new: " << newFormat.getShortName() << endl;
			for ( Uint32 j = 0 ; j < entries.size() ; j++ ) {
				cerr << j << ": " << entries[j].conv->getIFormat().getShortName()
					<< " - " << entries[j].conv->getOFormat().getShortName()
					<< endl;
			}
//			throw InternalError();
			return false;
		}
	}

#endif

	return true;

}

// ============================================================================

Image::Picture Image::ConverterStrategy::applyTo(
	const	Picture&		iPic) {

	Picture res = iPic;

	for ( EntryList::Iterator it = entries.firstElt() ; it ; it++ ) {
		res = ( it->forw
		      ? it->conv->convertForward(res)
		      : it->conv->convertBackward(res) );
	}

	return res;

}

void Image::ConverterStrategy::applyBetween(
	const	Picture &	iPic,
		Picture &	oPic ) {

	Picture	tPic = iPic;

	for ( EntryList::Iterator it = entries.firstElt() ; it ; it++ ) {
		if ( it->forw ) {
			it->conv->convertForward( tPic, oPic );
		}
		else {
			it->conv->convertBackward( tPic, oPic );
		}
		tPic = oPic;
	}

}

// ============================================================================

Buffer Image::ConverterStrategy::toBuffer() const {

	Buffer res;

	res = Buffer( iFormat.getShortName() );

	for ( EntryList::CstIterator it = entries.firstElt() ; it ; it++ ) {

		res += ", (";
		res += Buffer( it->conv->getIFormat().getShortName() );
		res += Buffer( it->forw ? " -> " : " <- " );
		res += Buffer( it->conv->getOFormat().getShortName() );
		res += ")";

	}

	return res;

}

// ============================================================================

