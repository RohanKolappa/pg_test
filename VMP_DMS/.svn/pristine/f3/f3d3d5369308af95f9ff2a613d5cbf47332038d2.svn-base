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
//	PPFileParser.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.BufferIO.h"
#include "BFC.IO.File.h"

#include "JBS.Tools.MkDep.PPFileParser.h"

// ============================================================================

using namespace std;
using namespace BFC;
using namespace JBS::Tools::MkDep;

// ============================================================================

BFC_PTR_IMPL(PPFileParser)

// ============================================================================

PPFileParser::PPFileParser() {

	outbuf = 0;

}

PPFileParser::~PPFileParser() {

	if ( outbuf ) {
		delete[] outbuf;
	}

}

// ============================================================================

PPFilePtr PPFileParser::parse(
	const	BFC::Buffer&		filename ) {

	PathsList paths;

	return parse( filename, paths );

}

// ============================================================================

PPFilePtr PPFileParser::parse(
	const	BFC::Buffer&		filename,
	const	PathsList&		paths ) {

//	cerr << "[...] Parsing: " << filename << endl;

	PPFilePtr res = new PPFile;

	res->filename = filename;
	res->realname = filename;

	IO::FilePtr file = new IO::File( res->realname, IO::File::Read );

	try {
		file->open();
	}
	catch ( Exception& ) {
	}

	if ( ! file->isConnected() ) {
		for ( PathsList::CstIterator it = paths.firstElt()
		    ; it && ! file->isConnected() ; it++ ) {
			res->realname = *it;
			if ( res->realname[res->realname.length()-1] != '/' )
				res->realname += '/';
			res->realname += filename;
//			msgDbg( realname + " ?..." );
			file = new IO::File( res->realname, IO::File::Read );
			try {
				file->open();
			}
			catch ( Exception& ) {
			}
		}
	}

	if ( ! file->isConnected() ) {
//		cerr << "[!!!] Can't find \"" << filename << "\"!" << endl;
		res->realname.kill();
		return res;
//		throw InvalidArgument( filename );
	}

	// Dump file content...

	Buffer content = file->dumpToBuffer();

	if ( outbuf ) {
		delete[] outbuf;
	}

	outbuf = new Uchar[ content.length() ];

	Uint32 pos = 0;

	while ( pos < content.length() ) {
		try {
			readLine( content, pos );
		}
		catch ( Exception& e ) {
			cerr << "Can't parse file \"" << res->filename
				<< "\"!" << endl;
			cerr << e.what() << endl;
		}
		if ( ! outlen ) {
			continue;
		}
		DirectivePtr d = processLine();
		if ( ! d ) {
//			cerr << "[WRN] Can't understand: \""
//				<< line << "\"!" << endl;
		}
		else {
			res->directives += d;
		}
	}

	return res;

}

// ============================================================================

void PPFileParser::readLine(
	const	Buffer&			content,
		Uint32&			pos ) {

	Uint32 mode;
		// 0: normal
		// 1: string
		// 2: line comment
		// 3: block comment

	const Uint32 len = content.length();
	const Uchar *ptr = content.getBufferAddr();
	Uchar termc = ' '; // for string mode...
	Uchar *out;
	Uchar c;

	for (;;) {

		out = outbuf;
		mode = 0;

		while ( pos < len ) {
			c = ptr[pos++];
			if ( mode == 0 ) { // Normal...
				if ( c == '"' || c == '\'' ) {
					// Enter string mode...
					mode = 1;
					termc = c;
					*out++ = c;
				}
				else if ( c == '\r' && pos < len && ptr[pos] == '\n' ) {
					pos++;
					break;
				}
				else if ( c == '\n' ) {
					break;
				}
				else if ( out == outbuf
					&& ( c == ' ' || c == '\t' ) ) {
					// Skip all leading white spaces!
				}
				else if ( c == '/' && pos < len && ptr[pos] == '*' ) {
					pos++;
					mode = 3;
				}
				else if ( c == '/' && pos < len && ptr[pos] == '/' ) {
					pos++;
					mode = 2;
				}
				else if ( c == '\\'
						&& pos < len
						&& ptr[pos] == '\\' ) {
					pos++;
					*out++ = c;
					*out++ = c;
				}
				else if ( c == '\\'
						&& pos < len
						&& ptr[pos] == '\n' ) {
					pos++;
				}
				else if ( c == '\\'
						&& pos+1 < len
						&& ptr[pos] == '\r'
						&& ptr[pos+1] == '\n' ) {
					pos += 2;
				}
				else {
					*out++ = c;
				}
			}
			else if ( mode == 1 ) { // String...
				if ( c == '\n' ) {
					throw InvalidArgument(
						"Unterminated string (pos: "
						+ Buffer( pos )
						+ ")" );
				}
				else if ( c == '\\'
						&& pos < len
						&& ptr[pos] == '\\' ) {
					pos++;
					*out++ = c;
					*out++ = c;
				}
				else if ( c == '\\'
						&& pos < len
						&& ptr[pos] == termc ) {
					pos++;
					*out++ = '\\';
					*out++ = termc;
				}
				else if ( c == termc ) {
					mode = 0;
					*out++ = c;
				}
				else {
					*out++ = c;
				}
			}
			else if ( mode == 2 ) { // Line comment...
				if ( c == '\r' && pos < len && ptr[pos] == '\n' ) {
					pos++;
					mode = 0;
					break;
				}
				else if ( c == '\n' ) {
					mode = 0;
					break;
				}
			}
			else if ( mode == 3 ) { // Block comment...
				if ( c == '*' && pos < len && ptr[pos] == '/' ) {
					mode = 0;
					pos++;
					*out++ = ' ';
				}
			}
		}

		if ( pos > len ) {
			throw InvalidArgument( "Unexpected EOL!" );
		}

		// Skip trailing white spaces...

		while ( out > outbuf && ( out[-1] == ' ' || out[-1] == '\t' ) ) {
			out--;
		}

		// Directive ?

		Uchar *p;

		for ( p = outbuf
		    ; p < out && ( *p == ' ' || *p == '\t' )
		    ; p++ ) {
		}

		if ( ( p < out && *p == '#' ) || pos == len ) {
			break;
		}

	}

	outlen = ( out - outbuf );

}

// ============================================================================

DirectivePtr PPFileParser::processLine() const {

	Uint32 pos = 1; // skip '#'

	Buffer directive = extractToken( pos );

	if ( directive == "if" ) {
		return new Directive(
			Directive::If,
			buildExpr( extractRestOfLine( pos ) ) );
	}

	if ( directive == "elif" ) {
		return new Directive(
			Directive::Elif,
			buildExpr( extractRestOfLine( pos ) ) );
	}

	if ( directive == "else" ) {
		return new Directive(
			Directive::Else );
	}

	if ( directive == "endif" ) {
		return new Directive(
			Directive::Endif );
	}

	if ( directive == "ifdef" ) {
		return new Directive(
			Directive::Ifdef,
			extractRestOfLine( pos ) );
	}

	if ( directive == "ifndef" ) {
		return new Directive(
			Directive::Ifndef,
			extractRestOfLine( pos ) );
	}

	if ( directive == "define" ) {
		Buffer name = extractToken( pos );
		Buffer value = extractRestOfLine( pos );
		if ( value.isEmpty() ) {
			value = "1";
		}
		return new Directive(
			Directive::Define,
			name, value );
	}

	if ( directive == "undef" ) {
		return new Directive(
			Directive::Undef,
			extractRestOfLine( pos ) );
	}

	if ( directive == "include" ) {
		return new Directive(
			Directive::Include,
			extractRestOfLine( pos ) );
	}

	if ( directive == "warning" ) {
		return new Directive(
			Directive::Warning,
			extractRestOfLine( pos ) );
	}

	if ( directive == "error" ) {
		return new Directive(
			Directive::Error,
			extractRestOfLine( pos ) );
	}

	return DirectivePtr();

}

// ============================================================================

Buffer PPFileParser::extractToken(
		Uint32&		pos ) const {

	const Uchar *ptr = outbuf + pos;
	Uint32 len = outlen - pos;

	while ( len && ( *ptr == ' ' || *ptr == '\t' ) ) {
		pos++;
		ptr++;
		len--;
	}

	if ( ! len ) {
		return Buffer();
	}

	const Uchar *beg = ptr;

	while ( len && *ptr != ' ' && *ptr != '\t' ) {
		pos++;
		ptr++;
		len--;
	}

	return Buffer( beg, ptr - beg );

}

Buffer PPFileParser::extractRestOfLine(
	const	Uint32		pos ) const {

	const Uchar *ptr = outbuf + pos;
	Uint32 len = outlen - pos;

	while ( len && ( *ptr == ' ' || *ptr == '\t' ) ) {
		ptr++;
		len--;
	}

	if ( ! len ) {
		return Buffer();
	}

	return Buffer( ptr, len );

}

// ============================================================================

ExprPtr PPFileParser::buildExpr(
	const	Buffer&		line ) const {

	TokenArray tokens;

	lexer.process( line, tokens );

	return builder.process( tokens );

}

// ============================================================================

