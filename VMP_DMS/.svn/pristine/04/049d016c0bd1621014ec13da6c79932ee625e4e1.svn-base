// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// FileName:
//	BFC.Plugins.File.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Plugins.File.h"

// ============================================================================

using namespace BFC;
using namespace Sys;

// ============================================================================

BFC_PTR_IMPL_NS( Plugins, File )

// ============================================================================

Plugins::File::File() {

	setObjectName("BFC.Plugins.File");

	data = 0;

}

Plugins::File::~File() {

	if ( data ) {
		msgWrn("Destroyed, but file still loaded...");
//		msgWrn("Going to get mad...");
	}

//	delete data;

}

// ============================================================================

void Plugins::File::setFileName(
	const	Buffer&		n) {

	Buffer nn;
	const Uint32 maxLen = 20;

	if ( n.length() > maxLen ) {
		nn = "..." + n(n.length() + 3 - maxLen, maxLen - 3);
	}
	else {
		nn = n;
	}

	setObjectName("BFC.Plugins.File[" + nn + "]");

	name = n;

}

const Buffer& Plugins::File::getFileName() const {

	return name;

}

// ============================================================================

void Plugins::File::load() {

	if ( name.isEmpty() ) {
		throw EmptyFileName();
	}

	if ( data ) {
		throw AlreadyLoaded();
	}

	Buffer f = name;

	data = DL::DllLoader::instance()->open(
			( const char * )f.getCStringPtr() );

	if ( ! data ) {
		throw CantOpenFile( DL::DllLoader::instance()->error() );
	}

}

void Plugins::File::unload() {

	if ( ! data ) {
		throw NotLoaded();
	}

	if ( ! DL::DllLoader::instance()->close( data ) ) { // autom. destroyed!
		throw CantCloseFile(Buffer(( const char* )DL::DllLoader::instance()->error()));
	}

}

// ============================================================================

