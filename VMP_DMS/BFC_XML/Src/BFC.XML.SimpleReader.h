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
//	BFC.XML.SimpleReader.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_SimpleReader_H_
#define _BFC_XML_SimpleReader_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.TL.Map.h"
#include "BFC.TL.Stack.h"

#include "BFC.XML.Attributes.h"
#include "BFC.XML.ContentHandler.h"
#include "BFC.XML.DTDHandler.h"
#include "BFC.XML.DeclHandler.h"
#include "BFC.XML.EntityResolver.h"
#include "BFC.XML.ErrorHandler.h"
#include "BFC.XML.LexicalHandler.h"
#include "BFC.XML.NamespaceSupport.h"
#include "BFC.XML.Reader.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief Implements a simple XML parser.
///
/// \ingroup BFC_XML

class BFC_XML_DLL SimpleReader : public Reader {

public :

	SimpleReader(
	);

	virtual ~SimpleReader(
	);

	virtual Bool getFeature(
		const	Buffer &	name
	) const;

	/// \brief Turns on the feature \a name if \a enable is true;
	///	otherwise turns it off.
	/// 
	/// The \a name parameter must be one of the following strings:
	/// -	\e http://xml.org/sax/features/namespaces:
	///	If enabled, namespaces are reported to the content handler
	///	(default: true).
	/// -	\e http://xml.org/sax/features/namespace-prefixes:
	/// 	If enabled, the original prefixed names and attributes used for
	///	namespace declarations are reported (default: false).
	/// -	\e http://trolltech.com/xml/features/report-whitespace-only-CharData:
	/// 	If enabled, CharData that consist of only whitespace characters
	///	are reported using ContentHandler::characters() (default: true).
	/// -	\e http://trolltech.com/xml/features/report-start-end-entity:
	/// 	If enabled, the parser reports ContentHandler::startEntity()
	///	and ContentHandler::endEntity() events, so character data might
	///	be reported in chunks. If disabled, the parser does not report
	///	these events, but silently substitutes the entities, and
	///	reports the character data in one chunk (default: false).

	virtual void setFeature(
		const	Buffer &	name,
			Bool		value
	);

	virtual Bool hasFeature(
		const	Buffer &	name
	) const;

	virtual void * getProperty(
		const	Buffer &	name
	) const;

	virtual void setProperty(
		const	Buffer &	name,
			void *		value
	);

	virtual Bool hasProperty(
		const	Buffer &	name
	) const;

	virtual void setEntityResolver(
			EntityResolverPtr	handler
	);

	virtual EntityResolverPtr getEntityResolver(
	) const;

	virtual void setDTDHandler(
			DTDHandlerPtr		handler
	);

	virtual DTDHandlerPtr getDTDHandler(
	) const;

	virtual void setContentHandler(
			ContentHandlerPtr	handler
	);

	virtual ContentHandlerPtr getContentHandler(
	) const;

	virtual void setErrorHandler(
			ErrorHandlerPtr		handler
	);

	virtual ErrorHandlerPtr getErrorHandler(
	) const;

	virtual void setLexicalHandler(
			LexicalHandlerPtr	handler
	);

	virtual LexicalHandlerPtr getLexicalHandler(
	) const;

	virtual void setDeclHandler(
			DeclHandlerPtr		handler
	);

	virtual DeclHandlerPtr getDeclHandler(
	) const;

	virtual void parse(
			InputSourcePtr		input
	);

	BASE_CLASS_GEN_EXCEPTION( "BFC.XML.SimpleReader" );

	CLASS_EXCEPTION( ParsingError, "Parsing error" );
	CLASS_EXCEPTION( UnexpectedCharacter, "Unexpected character" );
	CLASS_EXCEPTION( UnexpectedEOF, "Unexpected EOF" );

protected :

	void init(
			InputSourcePtr	inputDevice
	);

	Bool eat_ws(
	);

	Bool next_eat_ws(
	);

	void getNext(
	);

	Bool atEnd(
	) const;

	/// \brief Returns true if a entity with the name \a e exists, otherwise returns false.

	Bool entityExist(
		const	Buffer &
	) const;

	/// \brief Parses the prolog [ 22 ].

	Bool parseProlog(
	);

	/// \brief Parse an element [ 39 ].
	///
	/// Precondition: the opening '<' is already read.

	Bool parseElement(
	);

	/// \brief Helper to break down the size of the code in the case statement.

	Bool processElementEmptyTag(
	);

	/// \brief Helper to break down the size of the code in the case statement.

	Bool processElementETagBegin2(
	);

	/// \brief Helper to break down the size of the code in the case statement.

	Bool processElementAttribute(
	);

	/// \brief Parse Misc [ 27 ].

	Bool parseMisc(
	);

	/// \brief Parse a content [ 43 ].
	///
	/// A content is only used between tags. If a end tag is found the < is
	/// already read and the head stand on the '/' of the end tag '</name>'.

	Bool parseContent(
	);

	/// \brief Parse a processing instruction [ 16 ].
	/// 
	/// If xmldec is true, it tries to parse a PI or a XML declaration [ 23 ].
	/// 
	/// Precondition: the beginning '<' of the PI is already read and the head stand
	/// on the '?' of '<?'.
	/// 
	/// If this funktion was successful, the head-position is on the first
	/// character after the PI.

	Bool parsePI(
	);

	/// \brief Parse a document type definition ( doctypedecl [ 28 ] ).
	/// 
	/// Precondition: the beginning '<!' of the doctype is already read the head
	/// stands on the 'D' of '<!DOCTYPE'.
	/// 
	/// If this funktion was successful, the head-position is on the first
	/// character after the document type definition.

	Bool parseDoctype(
	);

	/// \brief Parse a comment [ 15 ].
	/// 
	/// Precondition: the beginning '<!' of the comment is already read and the head
	/// stands on the first '-' of '<!--'.
	/// 
	/// If this funktion was successful, the head-position is on the first
	/// character after the comment.

	Bool parseComment(
	);

	/// \brief Parse a Name [ 5 ] and store the name in name or ref ( if useRef is true ).

	Bool parseName(
	);

	/// \brief Parse a Nmtoken [ 7 ] and store the name in name.

	Bool parseNmtoken(
	);

	/// \brief Parse an Attribute [ 41 ].
	/// 
	/// Precondition: the head stands on the first character of the name
	/// of the attribute ( i.e. all whitespaces are already parsed ).
	/// 
	/// The head stand on the next character after the end quotes. The
	/// variable name contains the name of the attribute and the variable
	/// string contains the value of the attribute.

	Bool parseAttribute(
	);

	/// \brief Parse a Reference [ 67 ].
	/// 
	/// parseReference_charDataRead is set to true if the reference must not be
	/// parsed. The character( s ) which the reference mapped to are appended to
	/// string. The head stands on the first character after the reference.
	/// 
	/// parseReference_charDataRead is set to false if the reference must be parsed.
	/// The charachter( s ) which the reference mapped to are inserted at the reference
	/// position. The head stands on the first character of the replacement ).

	Bool parseReference(
	);

	/// \brief Helper function for parseReference().

	Bool processReference(
	);

	/// \brief Parse a ExternalID [ 75 ].
	/// 
	/// If allowPublicID is true parse ExternalID [ 75   ] or PublicID [ 83 ].

	Bool parseExternalID(
	);

	/// \brief Parse a PEReference [ 69 ].

	Bool parsePEReference(
	);

	/// \brief Parse a markupdecl [ 29 ].

	Bool parseMarkupdecl(
	);

	/// \brief Parse a AttlistDecl [ 52 ].
	/// 
	/// Precondition: the beginning '<!' is already read and the head
	/// stands on the 'A' of '<!ATTLIST'.

	Bool parseAttlistDecl(
	);

	/// \brief Parse a AttType [ 54 ].

	Bool parseAttType(
	);

	/// \brief Parse a AttValue [ 10 ].
	/// 
	/// Precondition: the head stands on the beginning " or '
	/// 
	/// If this function was successful, the head stands on the first
	/// character after the closing " or ' and the value of the attribute
	/// is in string().

	Bool parseAttValue(
	);

	/// \brief Parse a elementdecl [ 45 ].
	/// 
	/// Precondition: the beginning '<!E' is already read and the head
	/// stands on the 'L' of '<!ELEMENT'.

	Bool parseElementDecl(
	);

	/// \brief Parse a NotationDecl [ 82 ].
	/// 
	/// Precondition: the beginning '<!' is already read and the head
	/// stands on the 'N' of '<!NOTATION'.

	Bool parseNotationDecl(
	);

	/// \brief Parse choice [ 49 ] or seq [ 50 ].
	/// 
	/// Precondition: the beginning '('S? is already read and the head
	/// stands on the first non-whitespace character after it.

	Bool parseChoiceSeq(
	);

	//// \brief Parse a EntityDecl [ 70 ].
	//// 
	//// Precondition: the beginning '<!E' is already read and the head
	//// stand on the 'N' of '<!ENTITY'.

	Bool parseEntityDecl(
	);

	/// \brief Parse a EntityValue [ 9 ].

	Bool parseEntityValue(
	);

	/// \brief Parses over a simple string.
	/// 
	/// After the string was successfully parsed, the head is on the first
	/// character after the string.

	void parseString(
		const	Buffer &	str
	);

	/// This private function inserts and reports an entity substitution.
	///
	/// The
	/// substituted string is \a data and the name of the entity reference is \a
	/// name. If \a inLiteral is true, the entity is IncludedInLiteral ( i.e., " and '
	/// must be quoted. Otherwise they are not quoted.
	/// 
	/// This function returns false on error.

	Bool insertXmlRef(
		const	Buffer &,
		const	Buffer &,
			Bool
	);

	Bool reportEndEntities(
	);

	/// \brief This method strips the TextDecl [ 77 ] ( "<?xml ...?>" )
	///	from the string \a str.

	Buffer stripTextDecl(
		const	Buffer &	str
	);

private :

	struct ExternParameterEntity {

		ExternParameterEntity(
		) {
		}

		ExternParameterEntity(
			const	Buffer &	p,
			const	Buffer &	s
		) : publicId( p ), systemId( s ) {
		}

		Bool operator == (
			const	ExternParameterEntity &	o
		) const {
			return ( publicId == o.publicId
			      && systemId == o.systemId );
		}

		Buffer publicId;
		Buffer systemId;

	};

	struct ExternEntity {

		ExternEntity(
		) {
		}

		ExternEntity(
			const	Buffer &	p,
			const	Buffer &	s,
			const	Buffer &	n
		) : publicId( p ), systemId( s ), notation( n ) {
		}

		Bool operator == (
			const	ExternEntity	& o
		) const {
			return ( publicId == o.publicId
			      && systemId == o.systemId
			      && notation == o.notation );
		}

		Buffer publicId;
		Buffer systemId;
		Buffer notation;

	};

	struct XmlRef {

		XmlRef(
		) : index( 0 ) {
		}

		XmlRef(
			const	Buffer &	pName,
			const	Buffer &	pValue
		) : name( pName ), value( pValue ), index( 0 ) {
		}

		Bool isEmpty(
		) const {
			return ( index == value.getLength() );
		}

		Uchar getNext(
		) {
			return value[ index++ ];
		}

		Buffer	name;
		Buffer	value;
		Uint32	index;

	};

	Map< Buffer, ExternParameterEntity >	externParameterEntities;
	Map< Buffer, Buffer >			parameterEntities;
	Map< Buffer, ExternEntity >		externEntities;
	Map< Buffer, Buffer >			entities;

	Stack< Buffer >		tags;		///< used to determine if elements are correctly nested
	Stack< XmlRef >		xmlRefStack;

	Buffer			doctype;	///< only used for the doctype
	Buffer			xmlVersion;	///< only used to store the version information
	Buffer			encoding;	///< only used to store the encoding

	enum Standalone {
		Yes,
		No,
		Unknown
	};

	Standalone		standalone;	///< used to store the value of the standalone declaration

	Buffer			publicId;	// used by parseExternalID() to store the public ID
	Buffer			systemId;	// used by parseExternalID() to store the system ID
	Buffer			attDeclEName;	// use by parseAttlistDecl()
	Buffer			attDeclAName;	// use by parseAttlistDecl()

	// Features...

	static const Buffer	UseNamespacesName;
	static const Buffer	UseNamespacePrefixesName;
	static const Buffer	ReportWhitespaceCharDataName;
	static const Buffer	ReportEntitiesName;

	Bool			useNamespaces;
	Bool			useNamespacePrefixes;
	Bool			reportWhitespaceCharData;
	Bool			reportEntities;

	Attributes		attList;
	Bool			contentCharDataRead;
	NamespaceSupport	namespaceSupport;
//	Buffer			error;

	enum EntityRecognitionCtx {
		InContent,
		InAttributeValue,
		InEntityValue,
		InDTD
	};

	Bool			parsePI_xmldecl;
	Bool			parseName_useRef;
	Bool			parseReference_charDataRead;
	EntityRecognitionCtx	parseReference_context;
	Bool			parseExternalID_allowPublicID;
	EntityRecognitionCtx	parsePEReference_context;
	Buffer			parseString_s;

	Bool			xmldecl_possible;
	Bool			doctype_read;
	Bool			startDTDwasReported;
	Char			Done;

	ContentHandlerPtr	cntHandler;
	ErrorHandlerPtr		errHandler;
	DTDHandlerPtr		dtdHandler;
	EntityResolverPtr	entityRes;
	LexicalHandlerPtr	lexHandler;
	DeclHandlerPtr		decHandler;

	InputSourcePtr		inputSource;

	Uchar			c;		///< Current character.
	Bool			gotEOF;

	Buffer			nameValue;	// only used for names
	Buffer			refValue;	// only used for references
	Buffer			stringValue;	// used for any other strings that are parsed

	Buffer			emptyStr;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	SimpleReader(
		const	SimpleReader &
	);

	SimpleReader & operator = (
		const	SimpleReader &
	);

};

// ============================================================================

BFC_XML_DLL extern Bool qt_xml_skipped_entity_in_content;

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_SimpleReader_H_

// ============================================================================

