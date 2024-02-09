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
//	BFC.XML.SimpleReader.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.RegEx.SimpleRegEx.h"

#include "BFC.Math.Utils.h"

#include "BFC.XML.DefaultHandler.h"
#include "BFC.XML.SimpleReader.h"
#include "BFC.XML.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

XML::SimpleReader::SimpleReader() {

	setObjectName( "BFC.XML.SimpleReader" );

	useNamespaces = true;
	useNamespacePrefixes = false;
	reportWhitespaceCharData = true;
	reportEntities = false;

}

XML::SimpleReader::~SimpleReader() {



}

// ============================================================================

const Buffer XML::SimpleReader::UseNamespacesName
	= "http://xml.org/sax/features/namespaces";
const Buffer XML::SimpleReader::UseNamespacePrefixesName
	= "http://xml.org/sax/features/namespace-prefixes";
const Buffer XML::SimpleReader::ReportWhitespaceCharDataName
	= "http://trolltech.com/xml/features/report-whitespace-only-CharData";
const Buffer XML::SimpleReader::ReportEntitiesName
	= "http://trolltech.com/xml/features/report-start-end-entity";

// ============================================================================

Bool XML::SimpleReader::getFeature(
	const	Buffer &	name ) const {

	if ( name == UseNamespacesName ) {
		return useNamespaces;
	}

	if ( name == UseNamespacePrefixesName ) {
		return useNamespacePrefixes;
	}

	if ( name == ReportWhitespaceCharDataName ) {
		return reportWhitespaceCharData;
	}

	if ( name == ReportEntitiesName ) {
		return reportEntities;
	}

	throw NoSuchFeature( name );

}

void XML::SimpleReader::setFeature(
	const	Buffer &	name,
		Bool		enable ) {

	if ( name == UseNamespacesName ) {
		useNamespaces = enable;
	}

	else if ( name == UseNamespacePrefixesName ) {
		useNamespacePrefixes = enable;
	}

	else if ( name == ReportWhitespaceCharDataName ) {
		reportWhitespaceCharData = enable;
	}

	else if ( name == ReportEntitiesName ) {
		reportEntities = enable;
	}

	else {
		msgWrn( "setFeature(): unknown feature " + name );
	}

}

Bool XML::SimpleReader::hasFeature(
	const	Buffer &	name ) const {

	return ( name == UseNamespacesName
	      || name == UseNamespacePrefixesName
	      || name == ReportWhitespaceCharDataName
	      || name == ReportEntitiesName );

}

// ============================================================================

void * XML::SimpleReader::getProperty(
	const	Buffer &	name ) const {

	throw NoSuchProperty( name );

}

void XML::SimpleReader::setProperty(
	const	Buffer &	name,
		void *		/* value */ ) {

	msgWrn( "setProperty(): unknown property " + name );

}

Bool XML::SimpleReader::hasProperty(
	const	Buffer &	/* name */ ) const {

	return false;

}

// ============================================================================

void XML::SimpleReader::setEntityResolver(
		EntityResolverPtr handler ) {

	entityRes = handler;

}

XML::EntityResolverPtr XML::SimpleReader::getEntityResolver() const {

	return entityRes;

}

// ============================================================================

void XML::SimpleReader::setDTDHandler(
		DTDHandlerPtr handler ) {

	dtdHandler = handler;

}

XML::DTDHandlerPtr XML::SimpleReader::getDTDHandler() const {

	return dtdHandler;

}

// ============================================================================

void XML::SimpleReader::setContentHandler(
		ContentHandlerPtr handler ) {

	cntHandler = handler;

}

XML::ContentHandlerPtr XML::SimpleReader::getContentHandler() const {

	return cntHandler;

}

// ============================================================================

void XML::SimpleReader::setErrorHandler(
		ErrorHandlerPtr handler ) {

	errHandler = handler;

}

XML::ErrorHandlerPtr XML::SimpleReader::getErrorHandler() const {

	return errHandler;

}

// ============================================================================

void XML::SimpleReader::setLexicalHandler(
		LexicalHandlerPtr handler ) {

	lexHandler = handler;

}

XML::LexicalHandlerPtr XML::SimpleReader::getLexicalHandler() const {

	return lexHandler;

}

// ============================================================================

void XML::SimpleReader::setDeclHandler(
		DeclHandlerPtr handler ) {

	decHandler = handler;

}

XML::DeclHandlerPtr XML::SimpleReader::getDeclHandler() const {

	return decHandler;

}

// ============================================================================

void XML::SimpleReader::parse(
		InputSourcePtr	input ) {

	try {

		init( input );
	
		if ( cntHandler ) {
			cntHandler->startDocument();
		}
	
		qt_xml_skipped_entity_in_content = false;
	
		parseProlog();
	
		parseElement();
	
		while ( ! atEnd() ) {
			parseMisc();
		}
	
		// is stack empty?
	
		if ( ! tags.isEmpty() ) {
			throw UnexpectedEOF();
		}
	
		if ( cntHandler ) {
			cntHandler->endDocument();
		}

	}
	catch ( Exception & e ) {
		if ( errHandler ) {
			errHandler->fatalError( e.what() );
		}
	}

}

// ============================================================================

void XML::SimpleReader::init(
		InputSourcePtr	i ) {

//	msgDbg( "init(): --->" );

	inputSource = i;

	gotEOF = false;

	xmlRefStack.kill();
	getNext();

	externParameterEntities.kill();
	parameterEntities.kill();
	externEntities.kill();
	entities.kill();

	tags.kill();

	doctype.kill();
	xmlVersion.kill();
	encoding.kill();
	standalone = Unknown;

//	msgDbg( "init(): <---" );

}

// ============================================================================

Bool XML::qt_xml_skipped_entity_in_content;

// ============================================================================

#define XMLERR_TAGMISMATCH			"tag mismatch"
#define XMLERR_WRONGVALUEFORSDECL		"wrong value for standalone declaration"
#define XMLERR_EDECLORSDDECLEXPECTED		"encoding declaration or standalone declaration expected while reading the XML declaration"
#define XMLERR_SDDECLEXPECTED			"standalone declaration expected while reading the XML declaration"
#define XMLERR_ERRORPARSINGDOCTYPE		"error occurred while parsing document type definition"
#define XMLERR_LETTEREXPECTED			"letter is expected"
#define XMLERR_ERRORPARSINGCOMMENT		"error occurred while parsing comment"
#define XMLERR_ERRORPARSINGREFERENCE		"error occurred while parsing reference"
#define XMLERR_INTERNALGENERALENTITYINDTD	"internal general entity reference not allowed in DTD"
#define XMLERR_EXTERNALGENERALENTITYINAV	"external parsed general entity reference not allowed in attribute value"
#define XMLERR_EXTERNALGENERALENTITYINDTD	"external parsed general entity reference not allowed in DTD"
#define XMLERR_UNPARSEDENTITYREFERENCE		"unparsed entity reference in wrong context"
#define XMLERR_RECURSIVEENTITIES		"recursive entities"
#define XMLERR_ERRORINTEXTDECL			"error in the text declaration of an external entity"

static const Char cltWS		=  0; // white space
static const Char cltPer	=  1; // %
static const Char cltAmp	=  2; // &
static const Char cltGt		=  3; // >
static const Char cltLt		=  4; // <
static const Char cltSla	=  5; // /
static const Char cltQm		=  6; // ?
static const Char cltEm		=  7; // !
static const Char cltDsh	=  8; // -
static const Char cltCB		=  9; // ]
static const Char cltOB		= 10; // [
static const Char cltEq		= 11; // =
static const Char cltDq		= 12; // ""
static const Char cltSq		= 13; // '
static const Char cltUnk	= 14;

static const Char charLookupTable[ 256 ] = {
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
//		'\t'	'\n'			'\r'
	cltUnk, cltWS,	cltWS,	cltUnk,	cltUnk,	cltWS,	cltUnk,	cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
//	' '	'!'	'"'			'%'	'&'	'''
	cltWS,	cltEm,	cltDq,	cltUnk,	cltUnk,	cltPer,	cltAmp,	cltSq,
//						'-'		'/'
	cltUnk,	cltUnk,	cltUnk,	cltUnk,	cltUnk,	cltDsh,	cltUnk,	cltSla,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
//					'<'	'='	'>'	'?'
	cltUnk,	cltUnk,	cltUnk,	cltUnk,	cltLt,	cltEq,	cltGt,	cltQm,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
//				'['		']'
	cltUnk,	cltUnk,	cltUnk,	cltOB,	cltUnk,	cltCB,	cltUnk,	cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk,
	cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk, cltUnk
};

static Bool is_S(
	const	Uchar		uc ) {

	return ( uc == ' ' || uc == '\t' || uc == '\n' || uc == '\r' );

}

// ============================================================================

Buffer XML::SimpleReader::stripTextDecl(
	const	Buffer &	str ) {

	static const Buffer textDeclStart = "<?xml";

	if ( ! str.beginsWith( textDeclStart ) ) {
		return str;
	}

	RegEx::SimpleRegEx regex;

	regex.compile(
		"^(<\\?xml\\s+"
		"(version\\s*=\\s*((['\"])[-a-zA-Z0-9_.:]+\\4))?"
		"\\s*"
		"(encoding\\s*=\\s*((['\"])[A-Za-z][-a-zA-Z0-9_.]*\\7))?"
		"\\s*\\?>)" );

	regex.execute( str );

	Buffer res = str;

	res.replaceBuffer( regex.grabMatches()[ 1 ], "", 0, false );

	return res;

}

// ============================================================================

Bool XML::SimpleReader::atEnd() const {

	return gotEOF;

}

// ============================================================================

//
// The following private parse functions have another semantics for the return
// value: They return true iff parsing has finished successfully ( i.e. the end
// of the XML file must be reached! ). If one of these functions return false,
// there is only an error when d->error.isNULL() is also false.
//

/*
  For the incremental parsing, it is very important that the parse...()
  functions have a certain structure. Since it might be hard to understand how
  they work, here is a description of the layout of these functions:

	Bool XML::SimpleReader::parse...()
	{
( 1 )		   const Char Init			  = 0;
		...

( 2 )		   const Char Inp...			  = 0;
		...

( 3 )		   static Char table[ 3 ][ 2   ] = {
		...
		};
		Char state;
		Char input;

( 4 )		...

		for ( ; ; ) {
( 5 )			   switch ( state ) {
			...
			}

( 6 )
( 6a )			if ( atEnd() ) {
				unexpectedEof( &XML::SimpleReader::parseNmtoken, state );
				return false;
			}
( 6b )			if ( getCharCategory( c ) != NotName ) {
			...
			}
( 7 )			   state = table[ state ][ input ];

( 8 )			   switch ( state ) {
			...
			}
		}
	}

  Explanation:
  ad 1: constants for the states ( used in the transition table )
  ad 2: constants for the input ( used in the transition table )
  ad 3: the transition table for the state machine
  ad 4: do inititalizations
  ad 5: Do some actions according to the state; from the logical execution
		order, this code belongs after 8 ( see there for an explanation )
  ad 6: Check the character that is at the actual "cursor" position:
		a ) If we reached the EOF, report either error or push the state ( in the
		   case of incremental parsing ).
		b ) Otherwise, set the input character constant for the transition
		   table.
  ad 7: Get the new state according to the input that was read.
  ad 8: Do some actions according to the state. The last line in every case
		statement reads new data ( i.e. it move the cursor ). This can also be
		done by calling another parse...() funtion. If you need processing for
		this state after that, you have to put it into the switch statement 5.
		This ensures that you have a well defined re-entry point, when you ran
		out of data.
*/

Bool XML::SimpleReader::parseProlog()
{
	const Char Init			   = 0;
	const Char EatWS			   = 1; // eat white spaces
	const Char Lt			   = 2; // '<' read
	const Char Em			   = 3; // '!' read
	const Char DocType		   = 4; // read doctype
	const Char Comment		   = 5; // read comment
	const Char CommentR		   = 6; // same as Comment, but already reported
	const Char PInstr		   = 7; // read PI
	const Char PInstrR		   = 8; // same as PInstr, but already reported
	const Char Done			   = 9;

	const Char InpWs			   = 0;
	const Char InpLt			   = 1; // <
	const Char InpQm			   = 2; // ?
	const Char InpEm			   = 3; // !
	const Char InpD			   = 4; // D
	const Char InpDash		   = 5; // -
	const Char InpUnknown	   = 6;

	static const Char table[ 9 ][ 7   ] = {
	 /*	 InpWs	 InpLt	InpQm  InpEm  InpD		InpDash	 InpUnknown */
		{ EatWS,  Lt,	 -1,	-1,	   -1,		 -1,	   -1	   }, // Init
		{ -1,	  Lt,	 -1,	-1,	   -1,		 -1,	   -1	   }, // EatWS
		{ -1,	  -1,	 PInstr,Em,	   Done,	 -1,	   Done	   }, // Lt
		{ -1,	  -1,	 -1,	-1,	   DocType,	 Comment,  -1	   }, // Em
		{ EatWS,  Lt,	 -1,	-1,	   -1,		 -1,	   -1	   }, // DocType
		{ EatWS,  Lt,	 -1,	-1,	   -1,		 -1,	   -1	   }, // Comment
		{ EatWS,  Lt,	 -1,	-1,	   -1,		 -1,	   -1	   }, // CommentR
		{ EatWS,  Lt,	 -1,	-1,	   -1,		 -1,	   -1	   }, // PInstr
		{ EatWS,  Lt,	 -1,	-1,	   -1,		 -1,	   -1	   }  // PInstrR
	};
	int state;
	int input;

	xmldecl_possible = true;
	doctype_read = false;
	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case DocType:
				if ( doctype_read ) {
					throw ParsingError( "More than one doctype!" );
					return false;
				} else {
					doctype_read = false;
				}
				break;
			case Comment:
				if ( lexHandler ) {
					lexHandler->comment( stringValue );
				}
				state = CommentR;
				break;
			case PInstr:
				// call the handler
				if ( cntHandler ) {
					if ( xmldecl_possible && !xmlVersion.isEmpty() ) {
						Buffer value = "version='";
						value += xmlVersion;
						value += "'";
						if ( !encoding.isEmpty() ) {
							value += " encoding='";
							value += encoding;
							value += "'";
						}
						if ( standalone == XML::SimpleReader::Yes ) {
							value += " standalone='yes'";
						} else if ( standalone == XML::SimpleReader::No ) {
							value += " standalone='no'";
						}
						cntHandler->processingInstruction( "xml", value );
					} else {
						cntHandler->processingInstruction( nameValue, stringValue );
					}
				}
				// XML declaration only on first position possible
				xmldecl_possible = false;
				state = PInstrR;
				break;
			case Done:
				return true;
			case -1:
				throw ParsingError( "Invalid element!" );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '<' ) {
			input = InpLt;
		} else if ( c == '?' ) {
			input = InpQm;
		} else if ( c == '!' ) {
			input = InpEm;
		} else if ( c == 'D' ) {
			input = InpD;
		} else if ( c == '-' ) {
			input = InpDash;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case EatWS:
				// XML declaration only on first position possible
				xmldecl_possible = false;
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Lt:
				getNext();
				break;
			case Em:
				// XML declaration only on first position possible
				xmldecl_possible = false;
				getNext();
				break;
			case DocType:
				if ( !parseDoctype() ) {
					return false;
				}
				break;
			case Comment:
			case CommentR:
				if ( !parseComment() ) {
					return false;
				}
				break;
			case PInstr:
			case PInstrR:
				parsePI_xmldecl = xmldecl_possible;
				if ( !parsePI() ) {
					return false;
				}
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseElement()
{
	const int Init			   =  0;
	const int ReadName		   =  1;
	const int Ws1			   =  2;
	const int STagEnd		   =  3;
	const int STagEnd2		   =  4;
	const int ETagBegin		   =  5;
	const int ETagBegin2	   =  6;
	const int Ws2			   =  7;
	const int EmptyTag		   =  8;
	const int Attrib		   =  9;
	const int AttribPro		   = 10; // like Attrib, but processAttribute was already called
	const int Ws3			   = 11;
	const int Done			   = 12;

	const int InpWs			   = 0; // whitespace
	const int InpNameBe		   = 1; // BegName
	const int InpGt			   = 2; // >
	const int InpSlash		   = 3; // /
	const int InpUnknown	   = 4;

	static const int table[ 12 ][ 5   ] = {
	 /*	 InpWs		InpNameBe	 InpGt		  InpSlash	   InpUnknown */
		{ -1,		 ReadName,	-1,		   -1,		-1		}, // Init
		{ Ws1,		 Attrib,	STagEnd,	   EmptyTag,	-1		}, // ReadName
		{ -1,		 Attrib,	STagEnd,	   EmptyTag,	-1		}, // Ws1
		{ STagEnd2,	 STagEnd2,	STagEnd2,	   STagEnd2,	STagEnd2	}, // STagEnd
		{ -1,		 -1,		-1,		   ETagBegin,	-1		}, // STagEnd2
		{ -1,		 ETagBegin2,	-1,		   -1,		-1		}, // ETagBegin
		{ Ws2,		 -1,		Done,		   -1,		-1		}, // ETagBegin2
		{ -1,		 -1,		Done,		   -1,		-1		}, // Ws2
		{ -1,		 -1,		Done,		   -1,		-1		}, // EmptyTag
		{ Ws3,		 Attrib,	STagEnd,	   EmptyTag,	-1		}, // Attrib
		{ Ws3,		 Attrib,	STagEnd,	   EmptyTag,	-1		}, // AttribPro
		{ -1,		 Attrib,	STagEnd,	   EmptyTag,	-1		}	 // Ws3
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case ReadName:
				// store it on the stack
				tags.push( nameValue );
				// empty the attributes
				attList.kill();
				if ( useNamespaces )
					namespaceSupport.pushContext();
				break;
			case ETagBegin2:
				if ( !processElementETagBegin2() )
					return false;
				break;
			case Attrib:
				if ( !processElementAttribute() )
					return false;
				state = AttribPro;
				break;
			case Done:
				return true;
			case -1:
				throw ParsingError( "Invalid element!" );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( getCharCategory( c ) == BegName ) {
			input = InpNameBe;
		} else if ( c == '>' ) {
			input = InpGt;
		} else if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '/' ) {
			input = InpSlash;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case ReadName:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Ws1:
			case Ws2:
			case Ws3:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case STagEnd:
				// call the handler
				if ( cntHandler ) {
					Buffer tagsTop = tags.top();
					if ( useNamespaces ) {
						Buffer uri, lname;
						namespaceSupport.processName( tagsTop, false, uri, lname );
						cntHandler->startElement( uri, lname, tagsTop, attList );
					} else {
						cntHandler->startElement( Buffer(), Buffer(), tagsTop, attList );
					}
				}
				getNext();
				break;
			case STagEnd2:
				if ( !parseContent() ) {
					return false;
				}
				break;
			case ETagBegin:
				getNext();
				break;
			case ETagBegin2:
				// get the name of the tag
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case EmptyTag:
				if	( tags.isEmpty() ) {
					throw ParsingError( XMLERR_TAGMISMATCH );
					return false;
				}
				if ( !processElementEmptyTag() )
					return false;
				getNext();
				break;
			case Attrib:
			case AttribPro:
				// get name and value of attribute
				if ( !parseAttribute() ) {
					return false;
				}
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::processElementEmptyTag()
{
	Buffer uri, lname;
	// pop the stack and call the handler
	if ( cntHandler ) {
		if ( useNamespaces ) {
			// report startElement first...
			namespaceSupport.processName( tags.top(), false, uri, lname );
			cntHandler->startElement( uri, lname, tags.top(), attList );
			// ... followed by endElement...
			cntHandler->endElement( uri, lname, tags.pop() );
			// ... followed by endPrefixMapping
			BufferArray prefixesBefore, prefixesAfter;
			if ( cntHandler ) {
				prefixesBefore = namespaceSupport.getPrefixes();
			}
			namespaceSupport.popContext();
			// call the handler for prefix mapping
			prefixesAfter = namespaceSupport.getPrefixes();
			for ( Uint32 i = 0 ; i < prefixesBefore.getSize() ; i++ ) {
				if ( ! prefixesAfter.contains( prefixesBefore[ i ] ) ) {
					cntHandler->endPrefixMapping( prefixesBefore[ i ] );
				}
			}
		} else {
			// report startElement first...
			cntHandler->startElement( Buffer(), Buffer(), tags.top(), attList );
			// ... followed by endElement
			cntHandler->endElement( Buffer(), Buffer(), tags.pop() );
		}
	} else {
		tags.pop();
		namespaceSupport.popContext();
	}
	return true;
}

Bool XML::SimpleReader::processElementETagBegin2()
{
	const Buffer &name = XML::SimpleReader::nameValue;

	// pop the stack and compare it with the name
	if ( tags.pop() != name ) {
		throw ParsingError( XMLERR_TAGMISMATCH );
		return false;
	}
	// call the handler
	if ( cntHandler ) {
		Buffer uri, lname;

		if ( useNamespaces )
			namespaceSupport.processName( name, false, uri, lname );
		cntHandler->endElement( uri, lname, name );
	}
	if ( useNamespaces ) {
		NamespaceMap prefixesBefore, prefixesAfter;
		if ( cntHandler )
			prefixesBefore = namespaceSupport.getNs();

		namespaceSupport.popContext();
		// call the handler for prefix mapping
		if ( cntHandler ) {
			prefixesAfter = namespaceSupport.getNs();
			if ( prefixesBefore.getSize() != prefixesAfter.getSize() ) {
				for ( Uint32 i = 0 ; i < prefixesBefore.getSize() ; i++ ) {
					if ( ! prefixesBefore.getKey( i ).isEmpty()
					  && ! prefixesAfter.contains( prefixesBefore.getKey( i ) ) ) {
						cntHandler->endPrefixMapping( prefixesBefore.getKey( i ) );
					}
				}
			}
		}
	}
	return true;
}

Bool XML::SimpleReader::processElementAttribute()
{
	Buffer uri, lname, prefix;
	const Buffer &name = XML::SimpleReader::nameValue;
	const Buffer &string = XML::SimpleReader::stringValue;

	// add the attribute to the list
	if ( useNamespaces ) {
		// is it a namespace declaration?
		namespaceSupport.splitName( name, prefix, lname );
		if ( prefix == "xmlns" ) {
			// namespace declaration
			namespaceSupport.setPrefix( lname, string );
			if ( useNamespacePrefixes ) {
				// according to http://www.w3.org/2000/xmlns/, the "prefix"
				// xmlns maps to the namespace name
				// http://www.w3.org/2000/xmlns/
				attList.append( name, "http://www.w3.org/2000/xmlns/", lname, string );
			}
			// call the handler for prefix mapping
			if ( cntHandler ) {
				cntHandler->startPrefixMapping( lname, string );
			}
		} else {
			// no namespace delcaration
			namespaceSupport.processName( name, true, uri, lname );
			attList.append( name, uri, lname, string );
		}
	} else {
		// no namespace support
		attList.append( name, uri, lname, string );
	}
	return true;
}

Bool XML::SimpleReader::parseContent()
{
	const Char Init			   =  0;
	const Char ChD			   =  1; // CharData
	const Char ChD1			   =  2; // CharData help state
	const Char ChD2			   =  3; // CharData help state
	const Char Ref			   =  4; // Reference
	const Char Lt			   =  5; // '<' read
	const Char PInstr		   =  6; // PI
	const Char PInstrR		   =  7; // same as PInstr, but already reported
	const Char Elem			   =  8; // Element
	const Char Em			   =  9; // '!' read
	const Char Com			   = 10; // Comment
	const Char ComR			   = 11; // same as Com, but already reported
	const Char CDS			   = 12; // CDSect
	const Char CDS1			   = 13; // read a CDSect
	const Char CDS2			   = 14; // read a CDSect ( help state )
	const Char CDS3			   = 15; // read a CDSect ( help state )
	const Char Done			   = 16; // finished reading content

	const Char InpLt			   = 0; // <
	const Char InpGt			   = 1; // >
	const Char InpSlash		   = 2; // /
	const Char InpQMark		   = 3; // ?
	const Char InpEMark		   = 4; // !
	const Char InpAmp		   = 5; // &
	const Char InpDash		   = 6; // -
	const Char InpOpenB		   = 7; // [ 
	const Char InpCloseB		   = 8; // ]
	const Char InpUnknown	   = 9;

	static const Char mapCLT2FSMChar[] = {
		InpUnknown, // white space
		InpUnknown, // %
		InpAmp,		// &
		InpGt,		// >
		InpLt,		// <
		InpSlash,	// /
		InpQMark,	// ?
		InpEMark,	// !
		InpDash,	// -
		InpCloseB,	// ]
		InpOpenB,	// [ 
		InpUnknown, // =
		InpUnknown, // "
		InpUnknown, // '
		InpUnknown	// unknown
	};

	static const Char table[ 16 ][ 10   ] = {
	 /*	 InpLt	InpGt  InpSlash	 InpQMark  InpEMark	 InpAmp	 InpDash  InpOpenB	InpCloseB  InpUnknown */
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD1,		ChD	 }, // Init
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD1,		ChD	 }, // ChD
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD2,		ChD	 }, // ChD1
		{ Lt,	 -1,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD2,		ChD	 }, // ChD2
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD,		ChD	 }, // Ref ( same as Init )
		{ -1,	 -1,	Done,	  PInstr,	Em,		  -1,	  -1,	   -1,		 -1,		Elem }, // Lt
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD,		ChD	 }, // PInstr ( same as Init )
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD,		ChD	 }, // PInstrR
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD,		ChD	 }, // Elem ( same as Init )
		{ -1,	 -1,	-1,		  -1,		-1,		  -1,	  Com,	   CDS,		 -1,		-1	 }, // Em
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD,		ChD	 }, // Com ( same as Init )
		{ Lt,	 ChD,	ChD,	  ChD,		ChD,	  Ref,	  ChD,	   ChD,		 ChD,		ChD	 }, // ComR
		{ CDS1,	 CDS1,	CDS1,	  CDS1,		CDS1,	  CDS1,	  CDS1,	   CDS1,	 CDS2,		CDS1 }, // CDS
		{ CDS1,	 CDS1,	CDS1,	  CDS1,		CDS1,	  CDS1,	  CDS1,	   CDS1,	 CDS2,		CDS1 }, // CDS1
		{ CDS1,	 CDS1,	CDS1,	  CDS1,		CDS1,	  CDS1,	  CDS1,	   CDS1,	 CDS3,		CDS1 }, // CDS2
		{ CDS1,	 Init,	CDS1,	  CDS1,		CDS1,	  CDS1,	  CDS1,	   CDS1,	 CDS3,		CDS1 }	// CDS3
	};
	int state;
	int input;

	contentCharDataRead = false;
	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Ref:
				if ( !contentCharDataRead )
					contentCharDataRead = parseReference_charDataRead;
				break;
			case PInstr:
				if ( cntHandler ) {
					cntHandler->processingInstruction( nameValue,stringValue );
				}
				state = PInstrR;
				break;
			case Com:
				if ( lexHandler ) {
					lexHandler->comment( stringValue );
				}
				state = ComR;
				break;
			case CDS:
				stringValue.kill();
				break;
			case CDS2:
				if ( !atEnd() && c != ']' )
					stringValue += ']';
				break;
			case CDS3:
				// test if this skipping was legal
				if ( !atEnd() ) {
					if ( c == '>' ) {
						// the end of the CDSect
						if ( lexHandler ) {
							lexHandler->startCDATA();
						}
						if ( cntHandler ) {
							cntHandler->characters( stringValue );
						}
						if ( lexHandler ) {
							lexHandler->endCDATA();
						}
					} else if ( c == ']' ) {
						// three or more ']'
						stringValue += ']';
					} else {
						// after ' ] ]' comes another character
						stringValue += ']';
						stringValue += ']';
					}
				}
				break;
			case Done:
				// call the handler for CharData
				if ( cntHandler ) {
					if ( contentCharDataRead ) {
						if ( reportWhitespaceCharData || !stringValue.trim().isEmpty() ) {
							cntHandler->characters( stringValue );
						}
					}
				}
				// Done
				return true;
			case -1:
				// Error
				throw ParsingError( "Invalid content!" );
				return false;
		}

		// get input ( use lookup-table instead of nested ifs for performance
		// reasons )
		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( 0 ) {
			input = InpUnknown;
		} else {
			input = mapCLT2FSMChar[ ( int )charLookupTable[ c ] ];
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Init:
				// skip the ending '>' of a CDATASection
				getNext();
				break;
			case ChD:
				// on first call: clear string
				if ( !contentCharDataRead ) {
					contentCharDataRead = true;
					stringValue.kill();
				}
				stringValue += c;
				if ( reportEntities ) {
					if ( !reportEndEntities() )
						return false;
				}
				getNext();
				break;
			case ChD1:
				// on first call: clear string
				if ( !contentCharDataRead ) {
					contentCharDataRead = true;
					stringValue.kill();
				}
				stringValue += c;
				if ( reportEntities ) {
					if ( !reportEndEntities() )
						return false;
				}
				getNext();
				break;
			case ChD2:
				stringValue += c;
				if ( reportEntities ) {
					if ( !reportEndEntities() )
						return false;
				}
				getNext();
				break;
			case Ref:
				if ( !contentCharDataRead ) {
					// reference may be CharData; so clear string to be safe
					stringValue.kill();
					parseReference_context = InContent;
					if ( !parseReference() ) {
						return false;
					}
				} else {
					if ( reportEntities ) {
						// report character data in chunks
						if ( cntHandler ) {
							if ( reportWhitespaceCharData || !stringValue.trim().isEmpty() ) {
								cntHandler->characters( stringValue );
							}
						}
						stringValue.kill();
					}
					parseReference_context = InContent;
					if ( !parseReference() ) {
						return false;
					}
				}
				break;
			case Lt:
				// call the handler for CharData
				if ( cntHandler ) {
					if ( contentCharDataRead ) {
						if ( reportWhitespaceCharData || !stringValue.trim().isEmpty() ) {
							cntHandler->characters( stringValue );
						}
					}
				}
				contentCharDataRead = false;
				getNext();
				break;
			case PInstr:
			case PInstrR:
				parsePI_xmldecl = false;
				if ( !parsePI() ) {
					return false;
				}
				break;
			case Elem:
				if ( !parseElement() ) {
					return false;
				}
				break;
			case Em:
				getNext();
				break;
			case Com:
			case ComR:
				if ( !parseComment() ) {
					return false;
				}
				break;
			case CDS:
				parseString( "[CDATA[" );
				break;
			case CDS1:
				stringValue += c;
				getNext();
				break;
			case CDS2:
				// skip ']'
				getNext();
				break;
			case CDS3:
				// skip ']'...
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::reportEndEntities()
{
	int count = ( int )xmlRefStack.getSize();
	while ( count != 0 && xmlRefStack.top().isEmpty() ) {
		if ( cntHandler ) {
			if ( reportWhitespaceCharData || !stringValue.trim().isEmpty() ) {
				cntHandler->characters( stringValue );
			}
		}
		stringValue.kill();
		if ( lexHandler ) {
			lexHandler->endEntity( xmlRefStack.top().name );
		}
		xmlRefStack.pop();
		count--;
	}
	return true;
}

Bool XML::SimpleReader::parseMisc()
{
	const Char Init			   = 0;
	const Char Lt			   = 1; // '<' was read
	const Char Comment		   = 2; // read comment
	const Char eatWS			   = 3; // eat whitespaces
	const Char PInstr		   = 4; // read PI
	const Char Comment2		   = 5; // read comment

	const Char InpWs			   = 0; // S
	const Char InpLt			   = 1; // <
	const Char InpQm			   = 2; // ?
	const Char InpEm			   = 3; // !
	const Char InpUnknown	   = 4;

	static const Char table[ 3 ][ 5   ] = {
	 /*	 InpWs	 InpLt	InpQm  InpEm	 InpUnknown */
		{ eatWS,  Lt,	 -1,	-1,		  -1		}, // Init
		{ -1,	  -1,	 PInstr,Comment,  -1		}, // Lt
		{ -1,	  -1,	 -1,	-1,		  Comment2	}  // Comment
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case eatWS:
				return true;
			case PInstr:
				if ( cntHandler ) {
					cntHandler->processingInstruction( nameValue,stringValue );
				}
				return true;
			case Comment2:
				if ( lexHandler ) {
					lexHandler->comment( stringValue );
				}
				return true;
			case -1:
				throw UnexpectedCharacter( "In parseMisc(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '<' ) {
			input = InpLt;
		} else if ( c == '?' ) {
			input = InpQm;
		} else if ( c == '!' ) {
			input = InpEm;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case eatWS:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Lt:
				getNext();
				break;
			case PInstr:
				parsePI_xmldecl = false;
				if ( !parsePI() ) {
					return false;
				}
				break;
			case Comment:
				getNext();
				break;
			case Comment2:
				if ( !parseComment() ) {
					return false;
				}
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parsePI()
{
	const Char Init			   =  0;
	const Char QmI			   =  1; // ? was read
	const Char Name			   =  2; // read Name
	const Char XMLDecl		   =  3; // read XMLDecl
	const Char Ws1			   =  4; // eat ws after "xml" of XMLDecl
	const Char PInstr		   =  5; // read PI
	const Char Ws2			   =  6; // eat ws after Name of PI
	const Char Version		   =  7; // read versionInfo
	const Char Ws3			   =  8; // eat ws after versionInfo
	const Char EorSD			   =  9; // read EDecl or SDDecl
	const Char Ws4			   = 10; // eat ws after EDecl or SDDecl
	const Char SD			   = 11; // read SDDecl
	const Char Ws5			   = 12; // eat ws after SDDecl
	const Char ADone			   = 13; // almost done
	const Char Car			   = 14; // Char was read
	const Char Qm			   = 15; // Qm was read
	const Char Done			   = 16; // finished reading content

	const Char InpWs			   = 0; // whitespace
	const Char InpNameBe		   = 1; // BegName
	const Char InpGt			   = 2; // >
	const Char InpQm			   = 3; // ?
	const Char InpUnknown	   = 4;

	static const Char table[ 16 ][ 5   ] = {
	 /*	 InpWs,	 InpNameBe	InpGt  InpQm   InpUnknown  */
		{ -1,	  -1,		 -1,	QmI,	-1	   }, // Init
		{ -1,	  Name,		 -1,	-1,		-1	   }, // QmI
		{ -1,	  -1,		 -1,	-1,		-1	   }, // Name ( this state is left not through input )
		{ Ws1,	  -1,		 -1,	-1,		-1	   }, // XMLDecl
		{ -1,	  Version,	 -1,	-1,		-1	   }, // Ws1
		{ Ws2,	  -1,		 -1,	Qm,		-1	   }, // PInstr
		{ Car,	  Car,		 Car,	Qm,		Car   }, // Ws2
		{ Ws3,	  -1,		 -1,	ADone,	-1	   }, // Version
		{ -1,	  EorSD,	 -1,	ADone,	-1	   }, // Ws3
		{ Ws4,	  -1,		 -1,	ADone,	-1	   }, // EorSD
		{ -1,	  SD,		 -1,	ADone,	-1	   }, // Ws4
		{ Ws5,	  -1,		 -1,	ADone,	-1	   }, // SD
		{ -1,	  -1,		 -1,	ADone,	-1	   }, // Ws5
		{ -1,	  -1,		 Done,	-1,		-1	   }, // ADone
		{ Car,	  Car,		 Car,	Qm,		Car   }, // Char
		{ Car,	  Car,		 Done,	Qm,		Car   }, // Qm
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Name:
				// test what name was read and determine the next state
				// ( not very beautiful, I admit )
				if ( nameValue.toLower() == "xml" ) {
					if ( parsePI_xmldecl && nameValue=="xml" ) {
						state = XMLDecl;
					} else {
						throw ParsingError( "Invalid name for processing instruction!" );
						return false;
					}
				} else {
					state = PInstr;
					stringValue.kill();
				}
				break;
			case Version:
				// get version ( syntax like an attribute )
				if ( nameValue != "version" ) {
					throw ParsingError( "Missing version!" );
					return false;
				}
				xmlVersion = stringValue;
				break;
			case EorSD:
				// get the EDecl or SDDecl ( syntax like an attribute )
				if		  ( nameValue == "standalone" ) {
					if ( stringValue=="yes" ) {
						standalone = XML::SimpleReader::Yes;
					} else if ( stringValue=="no" ) {
						standalone = XML::SimpleReader::No;
					} else {
						throw ParsingError( XMLERR_WRONGVALUEFORSDECL );
						return false;
					}
				} else if ( nameValue == "encoding" ) {
					encoding = stringValue;
				} else {
					throw ParsingError( XMLERR_EDECLORSDDECLEXPECTED );
					return false;
				}
				break;
			case SD:
				if ( nameValue != "standalone" ) {
					throw ParsingError( XMLERR_SDDECLEXPECTED );
					return false;
				}
				if ( stringValue=="yes" ) {
					standalone = XML::SimpleReader::Yes;
				} else if ( stringValue=="no" ) {
					standalone = XML::SimpleReader::No;
				} else {
					throw ParsingError( XMLERR_WRONGVALUEFORSDECL );
					return false;
				}
				break;
			case Qm:
				// test if the skipping was legal
				if ( !atEnd() && c != '>' )
					stringValue += '?';
				break;
			case Done:
				return true;
			case -1:
				throw UnexpectedCharacter( "In parsePI(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( getCharCategory( c ) == BegName ) {
			input = InpNameBe;
		} else if ( c == '>' ) {
			input = InpGt;
		} else if ( c == '?' ) {
			input = InpQm;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case QmI:
				getNext();
				break;
			case Name:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Ws1:
			case Ws2:
			case Ws3:
			case Ws4:
			case Ws5:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Version:
				if ( !parseAttribute() ) {
					return false;
				}
				break;
			case EorSD:
				if ( !parseAttribute() ) {
					return false;
				}
				break;
			case SD:
				// get the SDDecl ( syntax like an attribute )
				if ( standalone != XML::SimpleReader::Unknown ) {
					throw UnexpectedCharacter( "In parsePI(): already parsed standalone declaration!" );
				}
				if ( !parseAttribute() ) {
					return false;
				}
				break;
			case ADone:
				getNext();
				break;
			case Car:
				stringValue += c;
				getNext();
				break;
			case Qm:
				// skip the '?'
				getNext();
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseDoctype()
{
	const Char Init			   =  0;
	const Char Doctype		   =  1; // read the doctype
	const Char Ws1			   =  2; // eat_ws
	const Char Doctype2		   =  3; // read the doctype, part 2
	const Char Ws2			   =  4; // eat_ws
	const Char Sys			   =  5; // read SYSTEM or PUBLIC
	const Char Ws3			   =  6; // eat_ws
	const Char MP			   =  7; // markupdecl or PEReference
	const Char MPR			   =  8; // same as MP, but already reported
	const Char PER			   =  9; // PERReference
	const Char Mup			   = 10; // markupdecl
	const Char Ws4			   = 11; // eat_ws
	const Char MPE			   = 12; // end of markupdecl or PEReference
	const Char Done			   = 13;

	const Char InpWs			   = 0;
	const Char InpD			   = 1; // 'D'
	const Char InpS			   = 2; // 'S' or 'P'
	const Char InpOB			   = 3; // [ 
	const Char InpCB			   = 4; // ]
	const Char InpPer		   = 5; // %
	const Char InpGt			   = 6; // >
	const Char InpUnknown	   = 7;

	static const Char table[ 13 ][ 8   ] = {
	 /*	 InpWs,	 InpD		InpS	   InpOB  InpCB	 InpPer InpGt  InpUnknown */
		{ -1,	  Doctype,	 -1,		-1,	   -1,	  -1,	 -1,	-1		  }, // Init
		{ Ws1,	  -1,		 -1,		-1,	   -1,	  -1,	 -1,	-1		  }, // Doctype
		{ -1,	  Doctype2,	 Doctype2,	-1,	   -1,	  -1,	 -1,	Doctype2  }, // Ws1
		{ Ws2,	  -1,		 Sys,		MP,	   -1,	  -1,	 Done,	-1		  }, // Doctype2
		{ -1,	  -1,		 Sys,		MP,	   -1,	  -1,	 Done,	-1		  }, // Ws2
		{ Ws3,	  -1,		 -1,		MP,	   -1,	  -1,	 Done,	-1		  }, // Sys
		{ -1,	  -1,		 -1,		MP,	   -1,	  -1,	 Done,	-1		  }, // Ws3
		{ -1,	  -1,		 -1,		-1,	   MPE,	  PER,	 -1,	Mup		  }, // MP
		{ -1,	  -1,		 -1,		-1,	   MPE,	  PER,	 -1,	Mup		  }, // MPR
		{ Ws4,	  -1,		 -1,		-1,	   MPE,	  PER,	 -1,	Mup		  }, // PER
		{ Ws4,	  -1,		 -1,		-1,	   MPE,	  PER,	 -1,	Mup		  }, // Mup
		{ -1,	  -1,		 -1,		-1,	   MPE,	  PER,	 -1,	Mup		  }, // Ws4
		{ -1,	  -1,		 -1,		-1,	   -1,	  -1,	 Done,	-1		  }	 // MPE
	};
	int state;
	int input;

	startDTDwasReported = false;
	systemId.kill();
	publicId.kill();
	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Doctype2:
				doctype = nameValue;
				break;
			case MP:
				if ( !startDTDwasReported && lexHandler ) {
					startDTDwasReported = true;
					lexHandler->startDTD( doctype, publicId, systemId );
				}
				state = MPR;
				break;
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_ERRORPARSINGDOCTYPE );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == 'D' ) {
			input = InpD;
		} else if ( c == 'S' ) {
			input = InpS;
		} else if ( c == 'P' ) {
			input = InpS;
		} else if ( c == '[' ) {
			input = InpOB;
		} else if ( c == ']' ) {
			input = InpCB;
		} else if ( c == '%' ) {
			input = InpPer;
		} else if ( c == '>' ) {
			input = InpGt;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Doctype:
				parseString( "DOCTYPE" );
				break;
			case Ws1:
			case Ws2:
			case Ws3:
			case Ws4:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Doctype2:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Sys:
				parseExternalID_allowPublicID = false;
				if ( !parseExternalID() ) {
					return false;
				}
				break;
			case MP:
			case MPR:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case PER:
				parsePEReference_context = InDTD;
				if ( !parsePEReference() ) {
					return false;
				}
				break;
			case Mup:
				if ( !parseMarkupdecl() ) {
					return false;
				}
				break;
			case MPE:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case Done:
				if ( lexHandler ) {
					if ( !startDTDwasReported ) {
						startDTDwasReported = true;
						lexHandler->startDTD( doctype, publicId, systemId );
					}
					lexHandler->endDTD();
				}
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseExternalID()
{
	const Char Init			   =  0;
	const Char Sys			   =  1; // parse 'SYSTEM'
	const Char SysWS			   =  2; // parse the whitespace after 'SYSTEM'
	const Char SysSQ			   =  3; // parse SystemLiteral with '
	const Char SysSQ2		   =  4; // parse SystemLiteral with '
	const Char SysDQ			   =  5; // parse SystemLiteral with "
	const Char SysDQ2		   =  6; // parse SystemLiteral with "
	const Char Pub			   =  7; // parse 'PUBLIC'
	const Char PubWS			   =  8; // parse the whitespace after 'PUBLIC'
	const Char PubSQ			   =  9; // parse PubidLiteral with '
	const Char PubSQ2		   = 10; // parse PubidLiteral with '
	const Char PubDQ			   = 11; // parse PubidLiteral with "
	const Char PubDQ2		   = 12; // parse PubidLiteral with "
	const Char PubE			   = 13; // finished parsing the PubidLiteral
	const Char PubWS2		   = 14; // parse the whitespace after the PubidLiteral
	const Char PDone			   = 15; // done if allowPublicID is true
	const Char Done			   = 16;

	const Char InpSQ			   = 0; // '
	const Char InpDQ			   = 1; // "
	const Char InpS			   = 2; // S
	const Char InpP			   = 3; // P
	const Char InpWs			   = 4; // white space
	const Char InpUnknown	   = 5;

	static const Char table[ 15 ][ 6   ] = {
	 /*	 InpSQ	  InpDQ	   InpS		InpP	 InpWs	   InpUnknown */
		{ -1,	   -1,		Sys,	 Pub,	  -1,		-1		}, // Init
		{ -1,	   -1,		-1,		 -1,	  SysWS,	-1		}, // Sys
		{ SysSQ,   SysDQ,	-1,		 -1,	  -1,		-1		}, // SysWS
		{ Done,	   SysSQ2,	SysSQ2,	 SysSQ2,  SysSQ2,	SysSQ2	}, // SysSQ
		{ Done,	   SysSQ2,	SysSQ2,	 SysSQ2,  SysSQ2,	SysSQ2	}, // SysSQ2
		{ SysDQ2,  Done,	SysDQ2,	 SysDQ2,  SysDQ2,	SysDQ2	}, // SysDQ
		{ SysDQ2,  Done,	SysDQ2,	 SysDQ2,  SysDQ2,	SysDQ2	}, // SysDQ2
		{ -1,	   -1,		-1,		 -1,	  PubWS,	-1		}, // Pub
		{ PubSQ,   PubDQ,	-1,		 -1,	  -1,		-1		}, // PubWS
		{ PubE,	   -1,		PubSQ2,	 PubSQ2,  PubSQ2,	PubSQ2	}, // PubSQ
		{ PubE,	   -1,		PubSQ2,	 PubSQ2,  PubSQ2,	PubSQ2	}, // PubSQ2
		{ -1,	   PubE,	PubDQ2,	 PubDQ2,  PubDQ2,	PubDQ2	}, // PubDQ
		{ -1,	   PubE,	PubDQ2,	 PubDQ2,  PubDQ2,	PubDQ2	}, // PubDQ2
		{ PDone,   PDone,	PDone,	 PDone,	  PubWS2,	PDone	}, // PubE
		{ SysSQ,   SysDQ,	PDone,	 PDone,	  PDone,	PDone	}  // PubWS2
	};
	int state;
	int input;

	systemId.kill();
	publicId.kill();
	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case PDone:
				if ( parseExternalID_allowPublicID ) {
					publicId = stringValue;
					return true;
				} else {
					throw UnexpectedCharacter( "In parseExternalID(): no public ID!" );
				}
			case Done:
				return true;
			case -1:
				throw UnexpectedCharacter( "In parseExternalID(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '\'' ) {
			input = InpSQ;
		} else if ( c == '"' ) {
			input = InpDQ;
		} else if ( c == 'S' ) {
			input = InpS;
		} else if ( c == 'P' ) {
			input = InpP;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Sys:
				parseString( "SYSTEM" );
				break;
			case SysWS:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case SysSQ:
			case SysDQ:
				stringValue.kill();
				getNext();
				break;
			case SysSQ2:
			case SysDQ2:
				stringValue += c;
				getNext();
				break;
			case Pub:
				parseString( "PUBLIC" );
				break;
			case PubWS:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case PubSQ:
			case PubDQ:
				stringValue.kill();
				getNext();
				break;
			case PubSQ2:
			case PubDQ2:
				stringValue += c;
				getNext();
				break;
			case PubE:
				getNext();
				break;
			case PubWS2:
				publicId = stringValue;
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Done:
				systemId = stringValue;
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseMarkupdecl()
{
	const Char Init			   = 0;
	const Char Lt			   = 1; // < was read
	const Char Em			   = 2; // ! was read
	const Char CE			   = 3; // E was read
	const Char Qm			   = 4; // ? was read
	const Char Dash			   = 5; // - was read
	const Char CA			   = 6; // A was read
	const Char CEL			   = 7; // EL was read
	const Char CEN			   = 8; // EN was read
	const Char CN			   = 9; // N was read
	const Char Done			   = 10;

	const Char InpLt			   = 0; // <
	const Char InpQm			   = 1; // ?
	const Char InpEm			   = 2; // !
	const Char InpDash		   = 3; // -
	const Char InpA			   = 4; // A
	const Char InpE			   = 5; // E
	const Char InpL			   = 6; // L
	const Char InpN			   = 7; // N
	const Char InpUnknown	   = 8;

	static const Char table[ 4 ][ 9   ] = {
	 /*	 InpLt	InpQm  InpEm  InpDash  InpA	  InpE	 InpL	InpN   InpUnknown */
		{ Lt,	 -1,	-1,	   -1,		-1,	   -1,	  -1,	 -1,	-1	   }, // Init
		{ -1,	 Qm,	Em,	   -1,		-1,	   -1,	  -1,	 -1,	-1	   }, // Lt
		{ -1,	 -1,	-1,	   Dash,	CA,	   CE,	  -1,	 CN,	-1	   }, // Em
		{ -1,	 -1,	-1,	   -1,		-1,	   -1,	  CEL,	 CEN,	-1	   }  // CE
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Qm:
				if ( cntHandler ) {
					cntHandler->processingInstruction( nameValue,stringValue );
				}
				return true;
			case Dash:
				if ( lexHandler ) {
					lexHandler->comment( stringValue );
				}
				return true;
			case CA:
				return true;
			case CEL:
				return true;
			case CEN:
				return true;
			case CN:
				return true;
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_LETTEREXPECTED );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( c == '<' ) {
			input = InpLt;
		} else if ( c == '?' ) {
			input = InpQm;
		} else if ( c == '!' ) {
			input = InpEm;
		} else if ( c == '-' ) {
			input = InpDash;
		} else if ( c == 'A' ) {
			input = InpA;
		} else if ( c == 'E' ) {
			input = InpE;
		} else if ( c == 'L' ) {
			input = InpL;
		} else if ( c == 'N' ) {
			input = InpN;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Lt:
				getNext();
				break;
			case Em:
				getNext();
				break;
			case CE:
				getNext();
				break;
			case Qm:
				parsePI_xmldecl = false;
				if ( !parsePI() ) {
					return false;
				}
				break;
			case Dash:
				if ( !parseComment() ) {
					return false;
				}
				break;
			case CA:
				if ( !parseAttlistDecl() ) {
					return false;
				}
				break;
			case CEL:
				if ( !parseElementDecl() ) {
					return false;
				}
				break;
			case CEN:
				if ( !parseEntityDecl() ) {
					return false;
				}
				break;
			case CN:
				if ( !parseNotationDecl() ) {
					return false;
				}
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parsePEReference()
{
	const Char Init			   = 0;
	const Char Next			   = 1;
	const Char Name			   = 2;
	const Char NameR			   = 3; // same as Name, but already reported
	const Char Done			   = 4;

	const Char InpSemi		   = 0; // ;
	const Char InpPer		   = 1; // %
	const Char InpUnknown	   = 2;

	static const Char table[ 4 ][ 3   ] = {
	 /*	 InpSemi  InpPer  InpUnknown */
		{ -1,	   Next,   -1	 }, // Init
		{ -1,	   -1,	   Name	 }, // Next
		{ Done,	   -1,	   -1	 }, // Name
		{ Done,	   -1,	   -1	 }	// NameR
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
		case Name:
			{
			Bool skipIt = true;
			Buffer xmlRefString;

			// Map<Buffer,Buffer>::Iterator it;
			// it = parameterEntities.find( refValue );

			Uint32 pos;

			if ( parameterEntities.exists( refValue, pos ) ) {
				skipIt = false;
				xmlRefString = parameterEntities.getValue( pos );
			} else if ( entityRes ) {
				// Map<Buffer,XML::SimpleReader::ExternParameterEntity>::Iterator it2;
				// it2 = externParameterEntities.find( refValue );
				InputSourcePtr ret;
				if ( externParameterEntities.exists( refValue, pos ) ) {
				// if ( it2 != externParameterEntities.end() ) {
					XML::SimpleReader::ExternParameterEntity it2
						= externParameterEntities.getValue( pos );
					entityRes->resolveEntity( it2.publicId, it2.systemId, ret );
					if ( ret ) {
						xmlRefString = ret->getData();
						if ( stripTextDecl( xmlRefString ).isEmpty() ) {
							throw ParsingError( XMLERR_ERRORINTEXTDECL );
							return false;
						}
						skipIt = false;
					}
				}
			}

			if ( skipIt ) {
				if ( cntHandler ) {
					cntHandler->skippedEntity( Buffer( "%" ) + refValue );
				}
			} else {
				if ( parsePEReference_context == InEntityValue ) {
					// Included in literal
					if ( !insertXmlRef( xmlRefString, refValue, true ) )
						return false;
				} else if ( parsePEReference_context == InDTD ) {
					// Included as PE
					if ( !insertXmlRef( Buffer( " " )+xmlRefString+Buffer( " " ), refValue, false ) )
						return false;
				}
			}
			}
			state = NameR;
			break;
		case Done:
			return true;
		case -1:
			// Error
			throw ParsingError( XMLERR_LETTEREXPECTED );
			return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if		  ( c == ';' ) {
			input = InpSemi;
		} else if ( c == '%' ) {
			input = InpPer;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Next:
				getNext();
				break;
			case Name:
			case NameR:
				parseName_useRef = true;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseAttlistDecl()
{
	const Char Init			   =  0;
	const Char Attlist		   =  1; // parse the string "ATTLIST"
	const Char Ws			   =  2; // whitespace read
	const Char Name			   =  3; // parse name
	const Char Ws1			   =  4; // whitespace read
	const Char Attdef		   =  5; // parse the AttDef
	const Char Ws2			   =  6; // whitespace read
	const Char Atttype		   =  7; // parse the AttType
	const Char Ws3			   =  8; // whitespace read
	const Char DDecH			   =  9; // DefaultDecl with #
	const Char DefReq		   = 10; // parse the string "REQUIRED"
	const Char DefImp		   = 11; // parse the string "IMPLIED"
	const Char DefFix		   = 12; // parse the string "FIXED"
	const Char Attval		   = 13; // parse the AttValue
	const Char Ws4			   = 14; // whitespace read
	const Char Done			   = 15;

	const Char InpWs			   = 0; // white space
	const Char InpGt			   = 1; // >
	const Char InpHash		   = 2; // #
	const Char InpA			   = 3; // A
	const Char InpI			   = 4; // I
	const Char InpF			   = 5; // F
	const Char InpR			   = 6; // R
	const Char InpUnknown	   = 7;

	static const Char table[ 15 ][ 8   ] = {
	 /*	 InpWs	  InpGt	   InpHash	InpA	  InpI	   InpF		InpR	 InpUnknown */
		{ -1,	   -1,		-1,		 Attlist,  -1,		-1,		 -1,	  -1	  }, // Init
		{ Ws,	   -1,		-1,		 -1,	   -1,		-1,		 -1,	  -1	  }, // Attlist
		{ -1,	   -1,		-1,		 Name,	   Name,	Name,	 Name,	  Name	  }, // Ws
		{ Ws1,	   Done,	Attdef,	 Attdef,   Attdef,	Attdef,	 Attdef,  Attdef  }, // Name
		{ -1,	   Done,	Attdef,	 Attdef,   Attdef,	Attdef,	 Attdef,  Attdef  }, // Ws1
		{ Ws2,	   -1,		-1,		 -1,	   -1,		-1,		 -1,	  -1	  }, // Attdef
		{ -1,	   Atttype, Atttype, Atttype,  Atttype, Atttype, Atttype, Atttype }, // Ws2
		{ Ws3,	   -1,		-1,		 -1,	   -1,		-1,		 -1,	  -1	  }, // Attype
		{ -1,	   Attval,	DDecH,	 Attval,   Attval,	Attval,	 Attval,  Attval  }, // Ws3
		{ -1,	   -1,		-1,		 -1,	   DefImp,	DefFix,	 DefReq,  -1	  }, // DDecH
		{ Ws4,	   Ws4,		-1,		 -1,	   -1,		-1,		 -1,	  -1	  }, // DefReq
		{ Ws4,	   Ws4,		-1,		 -1,	   -1,		-1,		 -1,	  -1	  }, // DefImp
		{ Ws3,	   -1,		-1,		 -1,	   -1,		-1,		 -1,	  -1	  }, // DefFix
		{ Ws4,	   Ws4,		-1,		 -1,	   -1,		-1,		 -1,	  -1	  }, // Attval
		{ -1,	   Done,	Attdef,	 Attdef,   Attdef,	Attdef,	 Attdef,  Attdef  }	 // Ws4
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Name:
				attDeclEName = nameValue;
				break;
			case Attdef:
				attDeclAName = nameValue;
				break;
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_LETTEREXPECTED );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '>' ) {
			input = InpGt;
		} else if ( c == '#' ) {
			input = InpHash;
		} else if ( c == 'A' ) {
			input = InpA;
		} else if ( c == 'I' ) {
			input = InpI;
		} else if ( c == 'F' ) {
			input = InpF;
		} else if ( c == 'R' ) {
			input = InpR;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Attlist:
				parseString( "ATTLIST" );
				break;
			case Ws:
			case Ws1:
			case Ws2:
			case Ws3:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Name:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Attdef:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Atttype:
				if ( !parseAttType() ) {
					return false;
				}
				break;
			case DDecH:
				getNext();
				break;
			case DefReq:
				parseString( "REQUIRED" );
				break;
			case DefImp:
				parseString( "IMPLIED" );
				break;
			case DefFix:
				parseString( "FIXED" );
				break;
			case Attval:
				if ( !parseAttValue() ) {
					return false;
				}
				break;
			case Ws4:
				if ( decHandler ) {
					// ### not all values are computed yet...
					decHandler->attributeDecl( attDeclEName, attDeclAName, "", "", "" );
				}
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseAttType()
{
	const Char Init			   =  0;
	const Char ST			   =  1; // StringType
	const Char TTI			   =  2; // TokenizedType starting with 'I'
	const Char TTI2			   =  3; // TokenizedType helpstate
	const Char TTI3			   =  4; // TokenizedType helpstate
	const Char TTE			   =  5; // TokenizedType starting with 'E'
	const Char TTEY			   =  6; // TokenizedType starting with 'ENTITY'
	const Char TTEI			   =  7; // TokenizedType starting with 'ENTITI'
	const Char N				   =  8; // N read ( TokenizedType or Notation )
	const Char TTNM			   =  9; // TokenizedType starting with 'NM'
	const Char TTNM2			   = 10; // TokenizedType helpstate
	const Char NO			   = 11; // Notation
	const Char NO2			   = 12; // Notation helpstate
	const Char NO3			   = 13; // Notation helpstate
	const Char NOName		   = 14; // Notation, read name
	const Char NO4			   = 15; // Notation helpstate
	const Char EN			   = 16; // Enumeration
	const Char ENNmt			   = 17; // Enumeration, read Nmtoken
	const Char EN2			   = 18; // Enumeration helpstate
	const Char ADone			   = 19; // almost done ( make next and accept )
	const Char Done			   = 20;

	const Char InpWs			   =  0; // whitespace
	const Char InpOp			   =  1; // ( 
	const Char InpCp			   =  2; // )
	const Char InpPipe		   =  3; // |
	const Char InpC			   =  4; // C
	const Char InpE			   =  5; // E
	const Char InpI			   =  6; // I
	const Char InpM			   =  7; // M
	const Char InpN			   =  8; // N
	const Char InpO			   =  9; // O
	const Char InpR			   = 10; // R
	const Char InpS			   = 11; // S
	const Char InpY			   = 12; // Y
	const Char InpUnknown	   = 13;

	static const Char table[ 19 ][ 14   ] = {
	 /*	 InpWs	  InpOp	   InpCp	InpPipe	 InpC	  InpE	   InpI		InpM	 InpN	  InpO	   InpR		InpS	 InpY	  InpUnknown */
		{ -1,	   EN,		-1,		 -1,	  ST,	   TTE,		TTI,	 -1,	  N,	   -1,		-1,		 -1,	  -1,	   -1	  }, // Init
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done	  }, // ST
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	TTI2,	 Done,	  Done,	   Done	  }, // TTI
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 TTI3,	  Done,	   Done	  }, // TTI2
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done	  }, // TTI3
		{ -1,	   -1,		-1,		 -1,	  -1,	   -1,		TTEI,	 -1,	  -1,	   -1,		-1,		 -1,	  TTEY,	   -1	  }, // TTE
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done	  }, // TTEY
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done	  }, // TTEI
		{ -1,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 TTNM,	  -1,	   NO,		-1,		 -1,	  -1,	   -1	  }, // N
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 TTNM2,	  Done,	   Done	  }, // TTNM
		{ Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done,	Done,	 Done,	  Done,	   Done	  }, // TTNM2
		{ NO2,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1	  }, // NO
		{ -1,	   NO3,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1	  }, // NO2
		{ NOName,  NOName,	NOName,	 NOName,  NOName,  NOName,	NOName,	 NOName,  NOName,  NOName,	NOName,	 NOName,  NOName,  NOName }, // NO3
		{ NO4,	   -1,		ADone,	 NO3,	  -1,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1	  }, // NOName
		{ -1,	   -1,		ADone,	 NO3,	  -1,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1	  }, // NO4
		{ -1,	   -1,		ENNmt,	 -1,	  ENNmt,   ENNmt,	ENNmt,	 ENNmt,	  ENNmt,   ENNmt,	ENNmt,	 ENNmt,	  ENNmt,   ENNmt  }, // EN
		{ EN2,	   -1,		ADone,	 EN,	  -1,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1	  }, // ENNmt
		{ -1,	   -1,		ADone,	 EN,	  -1,	   -1,		-1,		 -1,	  -1,	   -1,		-1,		 -1,	  -1,	   -1	  }	 // EN2
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case ADone:
				return true;
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_LETTEREXPECTED );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '(' ) {
			input = InpOp;
		} else if ( c == ')' ) {
			input = InpCp;
		} else if ( c == '|' ) {
			input = InpPipe;
		} else if ( c == 'C' ) {
			input = InpC;
		} else if ( c == 'E' ) {
			input = InpE;
		} else if ( c == 'I' ) {
			input = InpI;
		} else if ( c == 'M' ) {
			input = InpM;
		} else if ( c == 'N' ) {
			input = InpN;
		} else if ( c == 'O' ) {
			input = InpO;
		} else if ( c == 'R' ) {
			input = InpR;
		} else if ( c == 'S' ) {
			input = InpS;
		} else if ( c == 'Y' ) {
			input = InpY;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case ST:
				parseString( "CDATA" );
				break;
			case TTI:
				parseString( "ID" );
				break;
			case TTI2:
				parseString( "REF" );
				break;
			case TTI3:
				getNext(); // S
				break;
			case TTE:
				parseString( "ENTIT" );
				break;
			case TTEY:
				getNext(); // Y
				break;
			case TTEI:
				parseString( "IES" );
				break;
			case N:
				getNext(); // N
				break;
			case TTNM:
				parseString( "MTOKEN" );
				break;
			case TTNM2:
				getNext(); // S
				break;
			case NO:
				parseString( "OTATION" );
				break;
			case NO2:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case NO3:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case NOName:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case NO4:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case EN:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case ENNmt:
				if ( !parseNmtoken() ) {
					return false;
				}
				break;
			case EN2:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case ADone:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseAttValue()
{
	const Char Init			   = 0;
	const Char Dq			   = 1; // double quotes were read
	const Char DqRef			   = 2; // read references in double quotes
	const Char DqC			   = 3; // signed character read in double quotes
	const Char Sq			   = 4; // single quotes were read
	const Char SqRef			   = 5; // read references in single quotes
	const Char SqC			   = 6; // signed character read in single quotes
	const Char Done			   = 7;

	const Char InpDq			   = 0; // "
	const Char InpSq			   = 1; // '
	const Char InpAmp		   = 2; // &
	const Char InpLt			   = 3; // <
	const Char InpUnknown	   = 4;

	static const Char table[ 7 ][ 5   ] = {
	 /*	 InpDq	InpSq  InpAmp  InpLt InpUnknown */
		{ Dq,	 Sq,	-1,		-1,	  -1	}, // Init
		{ Done,	 DqC,	DqRef,	-1,	  DqC	}, // Dq
		{ Done,	 DqC,	DqRef,	-1,	  DqC	}, // DqRef
		{ Done,	 DqC,	DqRef,	-1,	  DqC	}, // DqC
		{ SqC,	 Done,	SqRef,	-1,	  SqC	}, // Sq
		{ SqC,	 Done,	SqRef,	-1,	  SqC	}, // SqRef
		{ SqC,	 Done,	SqRef,	-1,	  SqC	}  // SqRef
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Done:
				return true;
			case -1:
				throw UnexpectedCharacter( "In parseAttValue(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( c == '"' ) {
			input = InpDq;
		} else if ( c == '\'' ) {
			input = InpSq;
		} else if ( c == '&' ) {
			input = InpAmp;
		} else if ( c == '<' ) {
			input = InpLt;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Dq:
			case Sq:
				stringValue.kill();
				getNext();
				break;
			case DqRef:
			case SqRef:
				parseReference_context = InAttributeValue;
				if ( !parseReference() ) {
					return false;
				}
				break;
			case DqC:
			case SqC:
				stringValue += c;
				getNext();
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseElementDecl()
{
	const Char Init			   =  0;
	const Char Elem			   =  1; // parse the beginning string
	const Char Ws1			   =  2; // whitespace required
	const Char Nam			   =  3; // parse Name
	const Char Ws2			   =  4; // whitespace required
	const Char Empty			   =  5; // read EMPTY
	const Char Any			   =  6; // read ANY
	const Char Cont			   =  7; // read contentspec ( except ANY or EMPTY )
	const Char Mix			   =  8; // read Mixed
	const Char Mix2			   =  9; //
	const Char Mix3			   = 10; //
	const Char MixN1			   = 11; //
	const Char MixN2			   = 12; //
	const Char MixN3			   = 13; //
	const Char MixN4			   = 14; //
	const Char Cp			   = 15; // parse cp
	const Char Cp2			   = 16; //
	const Char WsD			   = 17; // eat whitespace before Done
	const Char Done			   = 18;

	const Char InpWs			   =  0;
	const Char InpGt			   =  1; // >
	const Char InpPipe		   =  2; // |
	const Char InpOp			   =  3; // ( 
	const Char InpCp			   =  4; // )
	const Char InpHash		   =  5; // #
	const Char InpQm			   =  6; // ?
	const Char InpAst		   =  7; // *
	const Char InpPlus		   =  8; // +
	const Char InpA			   =  9; // A
	const Char InpE			   = 10; // E
	const Char InpL			   = 11; // L
	const Char InpUnknown	   = 12;

	static const Char table[ 18 ][ 13   ] = {
	 /*	 InpWs	 InpGt	InpPipe	 InpOp	InpCp	InpHash	 InpQm	InpAst	InpPlus	 InpA	 InpE	 InpL	 InpUnknown */
		{ -1,	  -1,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  Elem,	  -1	 }, // Init
		{ Ws1,	  -1,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Elem
		{ -1,	  -1,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  Nam,	  Nam,	  Nam,	  Nam	 }, // Ws1
		{ Ws2,	  -1,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Nam
		{ -1,	  -1,	 -1,	  Cont,	 -1,	 -1,	  -1,	 -1,	 -1,	  Any,	  Empty,  -1,	  -1	 }, // Ws2
		{ WsD,	  Done,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Empty
		{ WsD,	  Done,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Any
		{ -1,	  -1,	 -1,	  Cp,	 Cp,	 Mix,	  -1,	 -1,	 -1,	  Cp,	  Cp,	  Cp,	  Cp	 }, // Cont
		{ Mix2,	  -1,	 MixN1,	  -1,	 Mix3,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Mix
		{ -1,	  -1,	 MixN1,	  -1,	 Mix3,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Mix2
		{ WsD,	  Done,	 -1,	  -1,	 -1,	 -1,	  -1,	 WsD,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Mix3
		{ -1,	  -1,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  MixN2,  MixN2,  MixN2,  MixN2	 }, // MixN1
		{ MixN3,  -1,	 MixN1,	  -1,	 MixN4,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // MixN2
		{ -1,	  -1,	 MixN1,	  -1,	 MixN4,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // MixN3
		{ -1,	  -1,	 -1,	  -1,	 -1,	 -1,	  -1,	 WsD,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // MixN4
		{ WsD,	  Done,	 -1,	  -1,	 -1,	 -1,	  Cp2,	 Cp2,	 Cp2,	  -1,	  -1,	  -1,	  -1	 }, // Cp
		{ WsD,	  Done,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }, // Cp2
		{ -1,	  Done,	 -1,	  -1,	 -1,	 -1,	  -1,	 -1,	 -1,	  -1,	  -1,	  -1,	  -1	 }	// WsD
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Done:
				return true;
			case -1:
				throw UnexpectedCharacter( "In parseElementDecl(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '>' ) {
			input = InpGt;
		} else if ( c == '|' ) {
			input = InpPipe;
		} else if ( c == '(' ) {
			input = InpOp;
		} else if ( c == ')' ) {
			input = InpCp;
		} else if ( c == '#' ) {
			input = InpHash;
		} else if ( c == '?' ) {
			input = InpQm;
		} else if ( c == '*' ) {
			input = InpAst;
		} else if ( c == '+' ) {
			input = InpPlus;
		} else if ( c == 'A' ) {
			input = InpA;
		} else if ( c == 'E' ) {
			input = InpE;
		} else if ( c == 'L' ) {
			input = InpL;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Elem:
				parseString( "LEMENT" );
				break;
			case Ws1:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Nam:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Ws2:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Empty:
				parseString( "EMPTY" );
				break;
			case Any:
				parseString( "ANY" );
				break;
			case Cont:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case Mix:
				parseString( "#PCDATA" );
				break;
			case Mix2:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Mix3:
				getNext();
				break;
			case MixN1:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case MixN2:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case MixN3:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case MixN4:
				getNext();
				break;
			case Cp:
				if ( !parseChoiceSeq() ) {
					return false;
				}
				break;
			case Cp2:
				getNext();
				break;
			case WsD:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseNotationDecl()
{
	const Char Init			   = 0;
	const Char Not			   = 1; // read NOTATION
	const Char Ws1			   = 2; // eat whitespaces
	const Char Nam			   = 3; // read Name
	const Char Ws2			   = 4; // eat whitespaces
	const Char ExtID			   = 5; // parse ExternalID
	const Char ExtIDR		   = 6; // same as ExtID, but already reported
	const Char Ws3			   = 7; // eat whitespaces
	const Char Done			   = 8;

	const Char InpWs			   = 0;
	const Char InpGt			   = 1; // >
	const Char InpN			   = 2; // N
	const Char InpUnknown	   = 3;

	static const Char table[ 8 ][ 4   ] = {
	 /*	 InpWs	 InpGt	InpN	InpUnknown */
		{ -1,	  -1,	 Not,	 -1		}, // Init
		{ Ws1,	  -1,	 -1,	 -1		}, // Not
		{ -1,	  -1,	 Nam,	 Nam	}, // Ws1
		{ Ws2,	  Done,	 -1,	 -1		}, // Nam
		{ -1,	  Done,	 ExtID,	 ExtID	}, // Ws2
		{ Ws3,	  Done,	 -1,	 -1		}, // ExtID
		{ Ws3,	  Done,	 -1,	 -1		}, // ExtIDR
		{ -1,	  Done,	 -1,	 -1		}  // Ws3
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case ExtID:
				// call the handler
				if ( dtdHandler ) {
					dtdHandler->notationDecl( nameValue, publicId, systemId );
				}
				state = ExtIDR;
				break;
			case Done:
				return true;
			case -1:
				throw UnexpectedCharacter( "In parseNotationDecl(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '>' ) {
			input = InpGt;
		} else if ( c == 'N' ) {
			input = InpN;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Not:
				parseString( "NOTATION" );
				break;
			case Ws1:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Nam:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Ws2:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case ExtID:
			case ExtIDR:
				parseExternalID_allowPublicID = true;
				if ( !parseExternalID() ) {
					return false;
				}
				break;
			case Ws3:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseChoiceSeq()
{
	const Char Init			   = 0;
	const Char Ws1			   = 1; // eat whitespace
	const Char CoS			   = 2; // choice or set
	const Char Ws2			   = 3; // eat whitespace
	const Char More			   = 4; // more cp to read
	const Char Name			   = 5; // read name
	const Char Done			   = 6; //

	const Char InpWs			   = 0; // S
	const Char InpOp			   = 1; // ( 
	const Char InpCp			   = 2; // )
	const Char InpQm			   = 3; // ?
	const Char InpAst		   = 4; // *
	const Char InpPlus		   = 5; // +
	const Char InpPipe		   = 6; // |
	const Char InpComm		   = 7; // ,
	const Char InpUnknown	   = 8;

	static const Char table[ 6 ][ 9   ] = {
	 /*	 InpWs	 InpOp	InpCp  InpQm  InpAst  InpPlus  InpPipe	InpComm	 InpUnknown */
		{ -1,	  Ws1,	 -1,	-1,	   -1,	   -1,		-1,		 -1,	  Name	}, // Init
		{ -1,	  CoS,	 -1,	-1,	   -1,	   -1,		-1,		 -1,	  CoS	}, // Ws1
		{ Ws2,	  -1,	 Done,	Ws2,   Ws2,	   Ws2,		More,	 More,	  -1	}, // CS
		{ -1,	  -1,	 Done,	-1,	   -1,	   -1,		More,	 More,	  -1	}, // Ws2
		{ -1,	  Ws1,	 -1,	-1,	   -1,	   -1,		-1,		 -1,	  Name	}, // More ( same as Init )
		{ Ws2,	  -1,	 Done,	Ws2,   Ws2,	   Ws2,		More,	 More,	  -1	}  // Name ( same as CS )
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Done:
				return true;
			case -1:
				throw UnexpectedCharacter( "In parseChoiceSeq(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '(' ) {
			input = InpOp;
		} else if ( c == ')' ) {
			input = InpCp;
		} else if ( c == '?' ) {
			input = InpQm;
		} else if ( c == '*' ) {
			input = InpAst;
		} else if ( c == '+' ) {
			input = InpPlus;
		} else if ( c == '|' ) {
			input = InpPipe;
		} else if ( c == ',' ) {
			input = InpComm;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Ws1:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case CoS:
				if ( !parseChoiceSeq() ) {
					return false;
				}
				break;
			case Ws2:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case More:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case Name:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseEntityDecl()
{
	const Char Init			   =  0;
	const Char Ent			   =  1; // parse "ENTITY"
	const Char Ws1			   =  2; // white space read
	const Char Name			   =  3; // parse name
	const Char Ws2			   =  4; // white space read
	const Char EValue		   =  5; // parse entity value
	const Char EValueR		   =  6; // same as EValue, but already reported
	const Char ExtID			   =  7; // parse ExternalID
	const Char Ws3			   =  8; // white space read
	const Char Ndata			   =  9; // parse "NDATA"
	const Char Ws4			   = 10; // white space read
	const Char NNam			   = 11; // parse name
	const Char NNamR			   = 12; // same as NNam, but already reported
	const Char PEDec			   = 13; // parse PEDecl
	const Char Ws6			   = 14; // white space read
	const Char PENam			   = 15; // parse name
	const Char Ws7			   = 16; // white space read
	const Char PEVal			   = 17; // parse entity value
	const Char PEValR		   = 18; // same as PEVal, but already reported
	const Char PEEID			   = 19; // parse ExternalID
	const Char PEEIDR		   = 20; // same as PEEID, but already reported
	const Char WsE			   = 21; // white space read
	const Char Done			   = 22;
	const Char EDDone		   = 23; // done, but also report an external, unparsed entity decl

	const Char InpWs			   = 0; // white space
	const Char InpPer		   = 1; // %
	const Char InpQuot		   = 2; // " or '
	const Char InpGt			   = 3; // >
	const Char InpN			   = 4; // N
	const Char InpUnknown	   = 5;

	static const Char table[ 22 ][ 6   ] = {
	 /*	 InpWs	InpPer	InpQuot	 InpGt	InpN	InpUnknown */
		{ -1,	 -1,	 -1,	  -1,	 Ent,	 -1		 }, // Init
		{ Ws1,	 -1,	 -1,	  -1,	 -1,	 -1		 }, // Ent
		{ -1,	 PEDec,	 -1,	  -1,	 Name,	 Name	 }, // Ws1
		{ Ws2,	 -1,	 -1,	  -1,	 -1,	 -1		 }, // Name
		{ -1,	 -1,	 EValue,  -1,	 -1,	 ExtID	 }, // Ws2
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // EValue
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // EValueR
		{ Ws3,	 -1,	 -1,	  EDDone,-1,	 -1		 }, // ExtID
		{ -1,	 -1,	 -1,	  EDDone,Ndata,	 -1		 }, // Ws3
		{ Ws4,	 -1,	 -1,	  -1,	 -1,	 -1		 }, // Ndata
		{ -1,	 -1,	 -1,	  -1,	 NNam,	 NNam	 }, // Ws4
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // NNam
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // NNamR
		{ Ws6,	 -1,	 -1,	  -1,	 -1,	 -1		 }, // PEDec
		{ -1,	 -1,	 -1,	  -1,	 PENam,	 PENam	 }, // Ws6
		{ Ws7,	 -1,	 -1,	  -1,	 -1,	 -1		 }, // PENam
		{ -1,	 -1,	 PEVal,	  -1,	 -1,	 PEEID	 }, // Ws7
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // PEVal
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // PEValR
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // PEEID
		{ WsE,	 -1,	 -1,	  Done,	 -1,	 -1		 }, // PEEIDR
		{ -1,	 -1,	 -1,	  Done,	 -1,	 -1		 }	// WsE
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case EValue:
				if ( !entityExist( nameValue ) ) {
					entities.add( nameValue, stringValue );
					if ( decHandler ) {
						decHandler->internalEntityDecl( nameValue, stringValue );
					}
				}
				state = EValueR;
				break;
			case NNam:
				if ( !entityExist( nameValue ) ) {
					externEntities.add( nameValue, XML::SimpleReader::ExternEntity( publicId, systemId, refValue ) );
					if ( dtdHandler ) {
						dtdHandler->unparsedEntityDecl( nameValue, publicId, systemId, refValue );
					}
				}
				state = NNamR;
				break;
			case PEVal:
				if ( !entityExist( nameValue ) ) {
					parameterEntities.add( nameValue, stringValue );
					if ( decHandler ) {
						decHandler->internalEntityDecl( Buffer( "%" )+nameValue, stringValue );
					}
				}
				state = PEValR;
				break;
			case PEEID:
				if ( !entityExist( nameValue ) ) {
					externParameterEntities.add( nameValue, XML::SimpleReader::ExternParameterEntity( publicId, systemId ) );
					if ( decHandler ) {
						decHandler->externalEntityDecl( Buffer( "%" )+nameValue, publicId, systemId );
					}
				}
				state = PEEIDR;
				break;
			case EDDone:
				if ( !entityExist( nameValue ) ) {
					externEntities.add( nameValue, XML::SimpleReader::ExternEntity( publicId, systemId, Buffer() ) );
					if ( decHandler ) {
						decHandler->externalEntityDecl( nameValue, publicId, systemId );
					}
				}
				return true;
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_LETTEREXPECTED );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( is_S( c ) ) {
			input = InpWs;
		} else if ( c == '%' ) {
			input = InpPer;
		} else if ( c == '"' || c == '\'' ) {
			input = InpQuot;
		} else if ( c == '>' ) {
			input = InpGt;
		} else if ( c == 'N' ) {
			input = InpN;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Ent:
				parseString( "NTITY" );
				break;
			case Ws1:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Name:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Ws2:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case EValue:
			case EValueR:
				if ( !parseEntityValue() ) {
					return false;
				}
				break;
			case ExtID:
				parseExternalID_allowPublicID = false;
				if ( !parseExternalID() ) {
					return false;
				}
				break;
			case Ws3:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Ndata:
				parseString( "NDATA" );
				break;
			case Ws4:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case NNam:
			case NNamR:
				parseName_useRef = true;
				if ( !parseName() ) {
					return false;
				}
				break;
			case PEDec:
				getNext();
				break;
			case Ws6:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case PENam:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Ws7:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case PEVal:
			case PEValR:
				if ( !parseEntityValue() ) {
					return false;
				}
				break;
			case PEEID:
			case PEEIDR:
				parseExternalID_allowPublicID = false;
				if ( !parseExternalID() ) {
					return false;
				}
				break;
			case WsE:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case EDDone:
				getNext();
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseEntityValue()
{
	const Char Init			   = 0;
	const Char Dq			   = 1; // EntityValue is double quoted
	const Char DqC			   = 2; // signed character
	const Char DqPER			   = 3; // PERefence
	const Char DqRef			   = 4; // Reference
	const Char Sq			   = 5; // EntityValue is double quoted
	const Char SqC			   = 6; // signed character
	const Char SqPER			   = 7; // PERefence
	const Char SqRef			   = 8; // Reference
	const Char Done			   = 9;

	const Char InpDq			   = 0; // "
	const Char InpSq			   = 1; // '
	const Char InpAmp		   = 2; // &
	const Char InpPer		   = 3; // %
	const Char InpUnknown	   = 4;

	static const Char table[ 9 ][ 5   ] = {
	 /*	 InpDq	InpSq  InpAmp  InpPer  InpUnknown */
		{ Dq,	 Sq,	-1,		-1,		-1	  }, // Init
		{ Done,	 DqC,	DqRef,	DqPER,	DqC	  }, // Dq
		{ Done,	 DqC,	DqRef,	DqPER,	DqC	  }, // DqC
		{ Done,	 DqC,	DqRef,	DqPER,	DqC	  }, // DqPER
		{ Done,	 DqC,	DqRef,	DqPER,	DqC	  }, // DqRef
		{ SqC,	 Done,	SqRef,	SqPER,	SqC	  }, // Sq
		{ SqC,	 Done,	SqRef,	SqPER,	SqC	  }, // SqC
		{ SqC,	 Done,	SqRef,	SqPER,	SqC	  }, // SqPER
		{ SqC,	 Done,	SqRef,	SqPER,	SqC	  }	 // SqRef
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_LETTEREXPECTED );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( c == '"' ) {
			input = InpDq;
		} else if ( c == '\'' ) {
			input = InpSq;
		} else if ( c == '&' ) {
			input = InpAmp;
		} else if ( c == '%' ) {
			input = InpPer;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Dq:
			case Sq:
				stringValue.kill();
				getNext();
				break;
			case DqC:
			case SqC:
				stringValue += c;
				getNext();
				break;
			case DqPER:
			case SqPER:
				parsePEReference_context = InEntityValue;
				if ( !parsePEReference() ) {
					return false;
				}
				break;
			case DqRef:
			case SqRef:
				parseReference_context = InEntityValue;
				if ( !parseReference() ) {
					return false;
				}
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseComment()
{
	const Char Init			   = 0;
	const Char Dash1			   = 1; // the first dash was read
	const Char Dash2			   = 2; // the second dash was read
	const Char Com			   = 3; // read comment
	const Char Com2			   = 4; // read comment ( help state )
	const Char ComE			   = 5; // finished reading comment
	const Char Done			   = 6;

	const Char InpDash		   = 0; // -
	const Char InpGt			   = 1; // >
	const Char InpUnknown	   = 2;

	static const Char table[ 6 ][ 3   ] = {
	 /*	 InpDash  InpGt	 InpUnknown */
		{ Dash1,   -1,	  -1  }, // Init
		{ Dash2,   -1,	  -1  }, // Dash1
		{ Com2,	   Com,	  Com }, // Dash2
		{ Com2,	   Com,	  Com }, // Com
		{ ComE,	   Com,	  Com }, // Com2
		{ -1,	   Done,  -1  }	 // ComE
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Dash2:
				stringValue.kill();
				break;
			case Com2:
				// if next character is not a dash than don't skip it
				if ( !atEnd() && c != '-' )
					stringValue += '-';
				break;
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_ERRORPARSINGCOMMENT );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( c == '-' ) {
			input = InpDash;
		} else if ( c == '>' ) {
			input = InpGt;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case Dash1:
				getNext();
				break;
			case Dash2:
				getNext();
				break;
			case Com:
				stringValue += c;
				getNext();
				break;
			case Com2:
				getNext();
				break;
			case ComE:
				getNext();
				break;
			case Done:
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseAttribute()
{
	const int Init			   = 0;
	const int PName			   = 1; // parse name
	const int Ws			   = 2; // eat ws
	const int Eq			   = 3; // the '=' was read
	const int Quotes		   = 4; // " or ' were read

	const int InpNameBe		   = 0;
	const int InpEq			   = 1; // =
	const int InpDq			   = 2; // "
	const int InpSq			   = 3; // '
	const int InpUnknown	   = 4;

	static const int table[ 4 ][ 5   ] = {
	 /*	 InpNameBe	InpEq  InpDq	InpSq	 InpUnknown */
		{ PName,	 -1,	-1,		 -1,	  -1	}, // Init
		{ -1,		 Eq,	-1,		 -1,	  Ws	}, // PName
		{ -1,		 Eq,	-1,		 -1,	  -1	}, // Ws
		{ -1,		 -1,	Quotes,	 Quotes,  -1	}  // Eq
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Quotes:
				// Done
				return true;
			case -1:
				throw UnexpectedCharacter( "In parseAttribute(): invalid state!" );
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( getCharCategory( c ) == BegName ) {
			input = InpNameBe;
		} else if ( c == '=' ) {
			input = InpEq;
		} else if ( c == '"' ) {
			input = InpDq;
		} else if ( c == '\'' ) {
			input = InpSq;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case PName:
				parseName_useRef = false;
				if ( !parseName() ) {
					return false;
				}
				break;
			case Ws:
				if ( !eat_ws() ) {
					return false;
				}
				break;
			case Eq:
				if ( !next_eat_ws() ) {
					return false;
				}
				break;
			case Quotes:
				if ( !parseAttValue() ) {
					return false;
				}
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseName()
{
	const int Init			   = 0;
	const int Name1			   = 1; // parse first character of the name
	const int Name			   = 2; // parse name
	const int Done			   = 3;

	static const int table[ 3 ][ 3   ] = {
	 /*	 InpNameBe	InpNameCh  InpUnknown */
		{ Name1,	 -1,		-1	  }, // Init
		{ Name,		 Name,		Done  }, // Name1
		{ Name,		 Name,		Done  }	 // Name
	};
	int state;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_LETTEREXPECTED );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}

		// we can safely do the ( int ) cast thanks to the Q_ASSERTs earlier in this function
		state = table[ state ][ ( int )getCharCategory( c ) ];

		switch ( state ) {
			case Name1:
				if ( parseName_useRef ) {
					refValue.kill();
					refValue += c;
				} else {
					nameValue.kill();
					nameValue += c;
				}
				getNext();
				break;
			case Name:
				if ( parseName_useRef ) {
					refValue += c;
				} else {
					nameValue += c;
				}
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseNmtoken()
{
	const Char Init			   = 0;
	const Char NameF			   = 1;
	const Char Name			   = 2;
	const Char Done			   = 3;

	const Char InpNameCh		   = 0; // NameChar without InpNameBe
	const Char InpUnknown	   = 1;

	static const Char table[ 3 ][ 2   ] = {
	 /*	 InpNameCh	InpUnknown */
		{ NameF,	 -1	   }, // Init
		{ Name,		 Done  }, // NameF
		{ Name,		 Done  }  // Name
	};
	int state;
	int input;

	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case Done:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_LETTEREXPECTED );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( getCharCategory( c ) == NotName ) {
			input = InpUnknown;
		} else {
			input = InpNameCh;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case NameF:
				nameValue.kill();
				nameValue += c;
				getNext();
				break;
			case Name:
				nameValue += c;
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::parseReference()
{
	// temporary variables ( only used in very local context, so they don't
	// interfere with incremental parsing
	Uint32 tmp;
	Bool ok = false;

	const Char Init			   =  0;
	const Char SRef			   =  1; // start of a reference
	const Char ChRef			   =  2; // parse CharRef
	const Char ChDec			   =  3; // parse CharRef decimal
	const Char ChHexS		   =  4; // start CharRef hexadecimal
	const Char ChHex			   =  5; // parse CharRef hexadecimal
	const Char Name			   =  6; // parse name
	const Char DoneD			   =  7; // done CharRef decimal
	const Char DoneH			   =  8; // done CharRef hexadecimal
	const Char DoneN			   =  9; // done EntityRef

	const Char InpAmp		   = 0; // &
	const Char InpSemi		   = 1; // ;
	const Char InpHash		   = 2; // #
	const Char InpX			   = 3; // x
	const Char InpNum		   = 4; // 0-9
	const Char InpHex		   = 5; // a-f A-F
	const Char InpUnknown	   = 6;

	static const Char table[ 8 ][ 7   ] = {
	 /*	 InpAmp	 InpSemi  InpHash  InpX		InpNum	InpHex	InpUnknown */
		{ SRef,	  -1,	   -1,		-1,		 -1,	 -1,	 -1	   }, // Init
		{ -1,	  -1,	   ChRef,	Name,	 Name,	 Name,	 Name  }, // SRef
		{ -1,	  -1,	   -1,		ChHexS,	 ChDec,	 -1,	 -1	   }, // ChRef
		{ -1,	  DoneD,   -1,		-1,		 ChDec,	 -1,	 -1	   }, // ChDec
		{ -1,	  -1,	   -1,		-1,		 ChHex,	 ChHex,	 -1	   }, // ChHexS
		{ -1,	  DoneH,   -1,		-1,		 ChHex,	 ChHex,	 -1	   }, // ChHex
		{ -1,	  DoneN,   -1,		-1,		 -1,	 -1,	 -1	   }  // Name
	};
	int state;
	int input;

	parseReference_charDataRead = false;
	state = Init;

	for ( ;; ) {
		switch ( state ) {
			case DoneD:
				return true;
			case DoneH:
				return true;
			case DoneN:
				return true;
			case -1:
				// Error
				throw ParsingError( XMLERR_ERRORPARSINGREFERENCE );
				return false;
		}

		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( 0 ) {
			input = InpUnknown;
		} else if ( c == '&' ) {
			input = InpAmp;
		} else if ( c == ';' ) {
			input = InpSemi;
		} else if ( c == '#' ) {
			input = InpHash;
		} else if ( c == 'x' ) {
			input = InpX;
		} else if ( '0' <= c && c <= '9' ) {
			input = InpNum;
		} else if ( 'a' <= c && c <= 'f' ) {
			input = InpHex;
		} else if ( 'A' <= c && c <= 'F' ) {
			input = InpHex;
		} else {
			input = InpUnknown;
		}
		state = table[ state ][ input ];

		switch ( state ) {
			case SRef:
				refValue.kill();
				getNext();
				break;
			case ChRef:
				getNext();
				break;
			case ChDec:
				refValue += c;
				getNext();
				break;
			case ChHexS:
				getNext();
				break;
			case ChHex:
				refValue += c;
				getNext();
				break;
			case Name:
				// read the name into the ref
				parseName_useRef = true;
				if ( !parseName() ) {
					return false;
				}
				break;
			case DoneD:
				tmp = refValue.toUint32();
				if ( ok ) {
					stringValue += Uchar( tmp );
				} else {
					throw ParsingError( XMLERR_ERRORPARSINGREFERENCE );
					return false;
				}
				parseReference_charDataRead = true;
				getNext();
				break;
			case DoneH:
// FIXME: toUint32() with hexa... Is there a 0x before ?
				tmp = refValue.toUint32();
				if ( ok ) {
					stringValue += Uchar( tmp );
				} else {
					throw ParsingError( XMLERR_ERRORPARSINGREFERENCE );
					return false;
				}
				parseReference_charDataRead = true;
				getNext();
				break;
			case DoneN:
				if ( !processReference() )
					return false;
				getNext();
				break;
		}
	}
	return false;
}

Bool XML::SimpleReader::processReference()
{
	Buffer reference = refValue;
	if ( reference == "amp" ) {
		if ( parseReference_context == InEntityValue ) {
			// Bypassed
			stringValue += '&'; stringValue += 'a'; stringValue += 'm'; stringValue += 'p'; stringValue += ';';
		} else {
			// Included or Included in literal
			stringValue += '&';
		}
		parseReference_charDataRead = true;
	} else if ( reference == "lt" ) {
		if ( parseReference_context == InEntityValue ) {
			// Bypassed
			stringValue += '&'; stringValue += 'l'; stringValue += 't'; stringValue += ';';
		} else {
			// Included or Included in literal
			stringValue += '<';
		}
		parseReference_charDataRead = true;
	} else if ( reference == "gt" ) {
		if ( parseReference_context == InEntityValue ) {
			// Bypassed
			stringValue += '&'; stringValue += 'g'; stringValue += 't'; stringValue += ';';
		} else {
			// Included or Included in literal
			stringValue += '>';
		}
		parseReference_charDataRead = true;
	} else if ( reference == "apos" ) {
		if ( parseReference_context == InEntityValue ) {
			// Bypassed
			stringValue += '&'; stringValue += 'a'; stringValue += 'p'; stringValue += 'o'; stringValue += 's'; stringValue += ';';
		} else {
			// Included or Included in literal
			stringValue += '\'';
		}
		parseReference_charDataRead = true;
	} else if ( reference == "quot" ) {
		if ( parseReference_context == InEntityValue ) {
			// Bypassed
			stringValue += '&'; stringValue += 'q'; stringValue += 'u'; stringValue += 'o'; stringValue += 't'; stringValue += ';';
		} else {
			// Included or Included in literal
			stringValue += '"';
		}
		parseReference_charDataRead = true;
	} else {
		// Map<Buffer,Buffer>::Iterator it;
		// it = entities.find( reference );
		Uint32 posRef;
		// if ( it != entities.end() ) {
		if ( entities.exists( reference, posRef ) ) {
			Buffer it = entities.getValue( posRef );
			// "Internal General"
			switch ( parseReference_context ) {
				case InContent:
					// Included
					if ( !insertXmlRef( it, reference, false ) )
						return false;
					parseReference_charDataRead = false;
					break;
				case InAttributeValue:
					// Included in literal
					if ( !insertXmlRef( it, reference, true ) )
						return false;
					parseReference_charDataRead = false;
					break;
				case InEntityValue:
					{
						// Bypassed
						stringValue += '&';
						for ( int i=0; i<( int )reference.getLength(); i++ ) {
							stringValue += reference[ i ];
						}
						stringValue += ';';
						parseReference_charDataRead = true;
					}
					break;
				case InDTD:
					// Forbidden
					parseReference_charDataRead = false;
					throw ParsingError( XMLERR_INTERNALGENERALENTITYINDTD );
					return false;
			}
		}
		else {
			// Map<Buffer,XML::SimpleReader::ExternEntity>::Iterator itExtern;
			// itExtern = externEntities.find( reference );
			Uint32 posRef2;
			// if ( itExtern == externEntities.end() ) {
			if ( ! externEntities.exists( reference, posRef2 ) ) {
				// entity not declared
				// ### check this case for conformance
				if ( parseReference_context == InEntityValue ) {
					// Bypassed
					stringValue += '&';
					for ( int i=0; i<( int )reference.getLength(); i++ ) {
						stringValue += reference[ i ];
					}
					stringValue += ';';
					parseReference_charDataRead = true;
				} else {
					// if we have some char data read, report it now
					if ( parseReference_context == InContent ) {
						if ( contentCharDataRead ) {
							if ( reportWhitespaceCharData || !stringValue.trim().isEmpty() ) {
								if ( cntHandler ) {
									cntHandler->characters( stringValue );
								}
							}
							stringValue.kill();
							contentCharDataRead = false;
						}
					}

					if ( cntHandler ) {
						qt_xml_skipped_entity_in_content = parseReference_context == InContent;
						cntHandler->skippedEntity( reference );
						qt_xml_skipped_entity_in_content = false;
					}
				}
			} else if ( externEntities.getValue( posRef2 ).notation.isEmpty() ) {
				XML::SimpleReader::ExternEntity itExtern = externEntities.getValue( posRef2 );
				// "External Parsed General"
				switch ( parseReference_context ) {
					case InContent:
						{
							// Included if validating
							Bool skipIt = true;
							if ( entityRes ) {
								InputSourcePtr ret;
								entityRes->resolveEntity( itExtern.publicId, itExtern.systemId, ret );
								if ( ret ) {
									Buffer xmlRefString = ret->getData();
									if ( stripTextDecl( xmlRefString ).isEmpty() ) {
										throw ParsingError( XMLERR_ERRORINTEXTDECL );
										return false;
									}
									if ( !insertXmlRef( xmlRefString, reference, false ) )
										return false;
									skipIt = false;
								}
							}
							if ( skipIt && cntHandler ) {
								qt_xml_skipped_entity_in_content = true;
								cntHandler->skippedEntity( reference );
								qt_xml_skipped_entity_in_content = false;
							}
							parseReference_charDataRead = false;
						} break;
					case InAttributeValue:
						// Forbidden
						parseReference_charDataRead = false;
						throw ParsingError( XMLERR_EXTERNALGENERALENTITYINAV );
						return false;
					case InEntityValue:
						{
							// Bypassed
							stringValue += '&';
							for ( int i=0; i<( int )reference.getLength(); i++ ) {
								stringValue += reference[ i ];
							}
							stringValue += ';';
							parseReference_charDataRead = true;
						}
						break;
					case InDTD:
						// Forbidden
						parseReference_charDataRead = false;
						throw ParsingError( XMLERR_EXTERNALGENERALENTITYINDTD );
						return false;
				}
			} else {
				// "Unparsed"
				// ### notify for "Occurs as Attribute Value" missing ( but this is no refence, anyway )
				// Forbidden
				parseReference_charDataRead = false;
				throw ParsingError( XMLERR_UNPARSEDENTITYREFERENCE );
				return false; // error
			}
		}
	}
	return true; // no error
}

void XML::SimpleReader::parseString(
	const	Buffer &	parseString_s ) {

	Uint32	len = parseString_s.getLength();

	for ( Uint32 pos = 0 ; pos < len ; pos++ ) {
		if ( atEnd() ) {
			throw UnexpectedEOF();
		}
		if ( c != parseString_s[ pos ] ) {
			throw UnexpectedCharacter( "In parseString(): char '"
				+ Buffer( ( Uchar )c )
				+ "' at pos "
				+ Buffer( pos )
				+ " in string \""
				+ parseString_s
				+ "\"!" );
		}
		getNext();
	}

	Done = len;

}

Bool XML::SimpleReader::insertXmlRef(
	const	Buffer		& data,
	const	Buffer		& name,
		Bool		inLiteral )
{
	if ( inLiteral ) {
		Buffer tmp = data;
		tmp.replaceBuffer( "\"", "&quot;", 0, true );
		tmp.replaceBuffer( "'", "&apos;", 0, true );
		xmlRefStack.push( XmlRef( name, tmp ) );
	} else {
		xmlRefStack.push( XmlRef( name, data ) );
	}
	Uint32 n = Math::getMax( parameterEntities.getSize(), entities.getSize() );
	if ( xmlRefStack.getSize() > n+1 ) {
		// recursive entities
		throw ParsingError( XMLERR_RECURSIVEENTITIES );
		return false;
	}
	if ( reportEntities && lexHandler ) {
		lexHandler->startEntity( name );
	}
	return true;
}

// ============================================================================

void XML::SimpleReader::getNext() {

	while ( ! xmlRefStack.isEmpty() ) {
		if ( ! xmlRefStack.top().isEmpty() ) {
			c = xmlRefStack.top().getNext();
			return;
		}
		xmlRefStack.pop();
	}

	try {
		c = inputSource->getNext();
	}
	catch ( InputSource::EndOfStream & ) {
		gotEOF = true;
	}

}

Bool XML::SimpleReader::eat_ws() {

	while ( ! atEnd() ) {
		if ( ! is_S( c ) ) {
			return true;
		}
		getNext();
	}

	return true;

}

Bool XML::SimpleReader::next_eat_ws() {

	getNext();

	return eat_ws();

}

// ============================================================================

Bool XML::SimpleReader::entityExist( const Buffer& e ) const
{
	if ( ! parameterEntities.contains( e )
	  && ! externParameterEntities.contains( e )
	  && ! externEntities.contains( e )
	  && ! entities.contains( e ) ) {
		return false;
	} else {
		return true;
	}
}

// ============================================================================

