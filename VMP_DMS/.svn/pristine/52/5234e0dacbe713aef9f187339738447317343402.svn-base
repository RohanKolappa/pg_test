// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "JBS".
//
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	JBS.GenDep.MkDep.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include <iostream>

#include "TBFC.Base.BufferIO.h"
#include "TBFC.Base.BufferParser.h"
#include "TBFC.IO.File.h"

#include "JBS.GenDep.ExprBuilder.h"
#include "JBS.GenDep.MkDep.h"
#include "JBS.GenDep.PPFileParser.h"
#include "JBS.GenDep.TokenFinder.h"

// ============================================================================

using namespace std;
using namespace TBFC;
using namespace JBS;

// ============================================================================

GenDep::MkDep::MkDep() {



}

GenDep::MkDep::~MkDep() {



}

// ============================================================================

void GenDep::MkDep::getDepsList(
	const	Buffer&		filename,
	const	BufferArray&	array_defs,
	const	BufferArray&	array_dirs,
		BufferArray&	array_deps ) {

	DefsList	defs;
	PathsList	dirs;

	initWithStdDefs( defs );

	Uint32		i;

	for ( i = 0 ; i < array_defs.size() ; i++ ) {
		addDef( defs, array_defs[ i ] );
	}

	for ( i = 0 ; i < array_dirs.size() ; i++ ) {
		addPath( dirs, array_dirs[ i ] );
	}

	generateDeps( filename, defs, dirs );

	array_deps.kill();

	for ( PathsList::CstIterator it = deps.firstElt() ; it ; it++ ) {
		array_deps += (*it);
	}

}

// ============================================================================

void GenDep::MkDep::main(
		int	argc,
		char	*argv[] ) {

	DefsList	defs;
	PathsList	paths;

	initWithStdDefs( defs );

	argc--; argv++; // skip program name...

	while ( argc-- ) {
		Buffer b( *argv++ );
		if ( b.isEmpty() ) {
			cerr << "[WRN] Empty argument!" << endl;
		}
		else if ( b[0] != '-' ) {
			generateDeps( b, defs, paths );
			cout << convertExtension( b ) << " : " << b;
			for ( PathsList::CstIterator it = deps.firstElt() ; it ; it++ ) {
				cout << " \\\n\t" << (*it);
			}
			cout << endl << endl;
		}
		else if ( b == "-D" ) {
			if ( ! argc ) {
				cerr << "[WNR] Missing definition!" << endl;
			}
			else {
				argc--;
				Buffer a( *argv++ );
				addDef( defs, a );
			}
		}
		else if ( b == "-I" ) {
			if ( ! argc ) {
				cerr << "[WRN] Missing definition!" << endl;
			}
			else {
				argc--;
				Buffer a( *argv++ );
				addPath( paths, a );
			}
		}
		else {
			cerr << "[WRN] Unsupported flag \"" << b << "\"!" << endl;
		}
	}

}

// ============================================================================

void GenDep::MkDep::initWithStdDefs(
		DefsList&		defs ) {

#if defined( PLATFORM_WIN32 )

	addDef( defs, "_M_IX86" );
	addDef( defs, "_MSC_VER=1200" );

#elif defined( PLATFORM_LINUX )

	addDef( defs, "__GNUC__" );
	addDef( defs, "__unix__" );
	addDef( defs, "__i386__" );
	addDef( defs, "__linux" );
	addDef( defs, "linux" );
	addDef( defs, "__gnu_linux__" );
	addDef( defs, "i386" );

#endif

}

void GenDep::MkDep::addDef(
		DefsList&		defs,
	const	Buffer&		def ) const {

	// Split...

	BufferArray elts = BufferParser::tokenize( def, "=" );
	Buffer name;
	Buffer value;

	if ( elts.size() == 1 ) {
		name = def;
		value = "1";
	}
	else if ( elts.size() > 2 ) {
		throw InvalidArgument( "Multiple '=' in definition!" );
	}
	else {
		name = elts[0];
		value = elts[1];
	}

	try {
		defs.add( name, value );
	}
	catch ( Exception& ) {
//		cerr << "Duplicate #define: \"" << name << "\"!" << endl;
	}

}

void GenDep::MkDep::addPath(
		PathsList&		paths,
	const	Buffer&		path ) const {

	for ( PathsList::Iterator it = paths.firstElt() ; it ; it++ ) {
		if ( *it == path ) {
			return;
		}
	}

	paths += path;

}

// ============================================================================

void GenDep::MkDep::generateDeps(
	const	Buffer&		filename,
	const	DefsList&		__defs,
	const	PathsList&		__paths ) {

	DefsList defs;
	PathsList paths;

	defs = __defs;
	paths = __paths;

	deps.kill();

	try {
		parseFile( filename, defs, paths, 0 );
	}
	catch ( Exception& e ) {
		cerr << "Failed to generate deps for \"" << filename << "\"!" << endl;
		cerr << e.what() << endl;
	}

}

void GenDep::MkDep::parseFile(
	const	Buffer&		filename,
		DefsList&		defs,
	const	PathsList&		paths,
	const	Uint32		recLevel ) {

	if ( recLevel > 50 ) {
		throw SyntaxError( "Too many nested includes!" );
	}

	// Find file in cache...

	PPFilePtr ppfile;

	for ( PPFileList::Iterator pit = ppfiles.firstElt() ; pit ; pit++ ) {
		if ( (*pit)->filename == filename ) {
			// Cache hit!
			ppfile = *pit;
			break;
		}
	}

	if ( ! ppfile ) {
		// Need to parse this file!
		PPFileParser parser;
		ppfile = parser.parse( filename, paths );
		ppfiles += ppfile;
	}

	// Found file!

	if ( recLevel
	  && ! ppfile->realname.isEmpty()
	  && ! deps.contains( ppfile->realname ) ) {
//		cout << " \\\n\t" << ppfile->realname;
		deps += ppfile->realname;
	}

	// Process directives...

	IfsList ifs;

	for ( DirectivesList::Iterator it = ppfile->directives.firstElt() ; it ; it++ ) {
		DirectivePtr d = *it;
		processDirective( d, defs, paths, ifs, recLevel );
	}

}

void GenDep::MkDep::processDirective(
		DirectivePtr		directive,
		DefsList&		defs,
	const	PathsList&		paths,
		IfsList&		ifs,
	const	Uint32		recLevel ) {

	Bool isCompiling = ( ifs.isEmpty() || ifs.top() == 1 );

	switch ( directive->type ) {

	case Directive::If :
//		msgDbg( "Processing #if..." );
		if ( ! isCompiling ) {
			ifs.push( 2 );
		}
		else if ( directive->expr->eval( defs ) ) {
			ifs.push( 1 );
//			msgDbg( "OK: #if " + expr->toBuffer() );
		}
		else {
			ifs.push( 0 );
//			msgDbg( "KO: #if " + expr->toBuffer() );
		}
		break;

	case Directive::Elif :
		if ( ifs.isEmpty() ) {
			throw SyntaxError( "elif without if!" );
		}
		if ( ifs.top() >= 1 ) {
			ifs.top() = 2;
		}
		else if ( directive->expr->eval( defs ) ) {
			ifs.top() = 1;
//			msgDbg( "OK: #elif " + expr->toBuffer() );
		}
		else {
//			msgDbg( "KO: #elif " + expr->toBuffer() );
		}
		break;

	case Directive::Else :
		if ( ifs.isEmpty() ) {
			throw SyntaxError( "else without if!" );
		}
		if ( ifs.top() >= 1 ) {
			ifs.top() = 2;
		}
		else {
			ifs.top() = 1;
		}
		break;

	case Directive::Endif :
		if ( ifs.isEmpty() ) {
			throw SyntaxError( "endif without if!" );
		}
		ifs.pop();
		break;

	case Directive::Ifdef :
		if ( ! isCompiling ) {
			ifs.push( 2 );
		}
		else if ( defs.contains( directive->str1 ) ) {
			ifs.push( 1 );
		}
		else {
			ifs.push( 0 );
		}
		break;

	case Directive::Ifndef :
		if ( ! isCompiling ) {
			ifs.push( 2 );
		}
		else if ( ! defs.contains( directive->str1 ) ) {
			ifs.push( 1 );
		}
		else {
			ifs.push( 0 );
		}
		break;

	case Directive::Define :
		if ( isCompiling ) {
			Buffer name = directive->str1;
			Buffer value = directive->str2;
//			cerr << "[DEF] " << name << " == " << value << endl;
			try {
				defs.add( name, value );
			}
			catch ( Exception& ) {
//				cerr << "Duplicate #define: \"" << name
//					<< "\"!" << endl;
//				cerr << "... old value: " << defs[name] << endl;
//				cerr << "... new value: " << value << endl;
			}
		}
		break;

	case Directive::Undef :
		if ( isCompiling ) {
			Buffer name = directive->str1;
//			cerr << "[UND] " << name << endl;
			try {
				defs.del( name );
			}
			catch ( Exception& ) {
//				cerr << "Can't undef \"" << name << "\"!" << endl;
			}
		}
		break;

	case Directive::Include :
		if ( isCompiling ) {
			Buffer name = directive->str1;
//			cerr << "[INC] " << name << endl;
//			if ( name[0] == '"' ) {
				parseFile(
					name( 1, name.length()-2 ),
					defs,
					paths,
					recLevel + 1 );
//			}
		}
		break;

	case Directive::Warning :
		if ( isCompiling ) {
			cerr << "WARNING: " << directive->str1 << endl;
		}
		break;

	case Directive::Error :
		if ( isCompiling ) {
			cerr << "ERROR: " << directive->str1 << endl;
			throw SyntaxError( "Found #error directive!" );
		}
		break;

	default :
		throw SyntaxError( "Unrecognized directive!" );

	} // switch(type)

}

// ============================================================================

Buffer GenDep::MkDep::convertExtension(
	const	Buffer&		filename ) const {

	if ( filename.endsWith( ".cpp" )
	  || filename.endsWith( ".CPP" ) ) {
#if defined( PLATFORM_WIN32 )
		return filename( 0, filename.length() - 4 ) + ".obj";
#else
		return filename( 0, filename.length() - 4 ) + ".o";
#endif
	}

	if ( filename.endsWith( ".c" )
	  || filename.endsWith( ".C" ) ) {
#if defined( PLATFORM_WIN32 )
		return filename( 0, filename.length() - 2 ) + ".obj";
#else
		return filename( 0, filename.length() - 2 ) + ".o";
#endif
	}

	return filename;

}

// ============================================================================

Buffer GenDep::MkDep::extractToken(
	const	Buffer&		line,
		Uint32&		pos ) const {

	const Uint32 len = line.length();

	while ( pos < len && ( line[pos] == ' ' || line[pos] == '\t' ) ) {
		pos++;
	}

	if ( pos >= len ) {
		return Buffer();
	}

	Uint32 beg = pos;

	while ( pos < len && line[pos] != ' ' && line[pos] != '\t' ) {
		pos++;
	}

	return line( beg, pos - beg );

}

Buffer GenDep::MkDep::extractRestOfLine(
	const	Buffer&		line,
	const	Uint32		__pos ) const {

	const Uint32 len = line.length();
	Uint32 pos = __pos;

	while ( pos < len && ( line[pos] == ' ' || line[pos] == '\t' ) ) {
		pos++;
	}

	if ( pos >= len ) {
		return Buffer();
	}

	return line( pos, len - pos );

}

// ============================================================================

GenDep::ExprPtr GenDep::MkDep::buildExpr(
	const	Buffer&		line ) const {

	TokenArray tokens;
	TokenFinder lexer;
	lexer.process( line, tokens );
	ExprBuilder builder;
	return builder.process( tokens );

}

// ============================================================================

