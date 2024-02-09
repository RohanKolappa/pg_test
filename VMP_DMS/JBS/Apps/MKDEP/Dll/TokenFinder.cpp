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
//	TokenFinder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "TokenFinder.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL(TokenFinder)

// ============================================================================

TokenFinder::TokenFinder() {



}

TokenFinder::~TokenFinder() {



}

// ============================================================================

void TokenFinder::process(
	const	Buffer&			line,
		TokenArray&		tokens ) const {

	// Loop:
	// - read next valid, or special EOS char
	// - if something in previous buffer
	//	- if new char == EOS
	//		- flush previous buffer
	//		- terminate loop
	//	- else if new char can be added to previous buffer
	//		- push new char in previous buffer
	//	- else
	//		- flush previous buffer
	//		- create new previous buffer with new char
	// - else
	//	- create new previous buffer with new char

	TokenList list;

	const Uchar *ptr = line.getBufferAddr();
	const Uint32 len = line.length();
	Uint32 cur = 0;
	Uint32 buf = 0;
	Context ctx = NOC;

	for (;;) {
		if ( cur == len ) {
			if ( buf < cur && ctx != SPA ) {
				list += createToken( ptr, buf, cur, ctx );
			}
			break;
		}
		// Can we add the current char to the previous buffer ?
		Uchar c = ptr[cur];
		Context newctx =
			( ctx >= PLU || c >= 0x80
			? INV
			: conv[ctx][c] );
		if ( buf < cur ) {
			if ( newctx == INV ) {
				if ( ctx != SPA ) {
					list += createToken( ptr, buf, cur, ctx );
				}
				buf = cur;
				ctx = NOC;
				if ( c < 0x80 ) {
					newctx = conv[ctx][c];
				}
			}
		}
		if ( newctx == INV ) {
			throw InvalidCharacter(
				"Char '" + Buffer( ( Uchar )c )
				+ "' at pos " + Buffer( cur )
				+ " in \""
				+ line + "\"" );
		}
		ctx = newctx;
		cur++;
	}

	tokens.kill();
	tokens.resize( list.size() );

	TokenList::Iterator it;
	Uint32 i;

	for ( it = list.firstElt(), i = 0 ; it ; it++, i++ ) {
		tokens[i] = *it;
	}

}

// ============================================================================

Token TokenFinder::createToken(
	const	Uchar		*ptr2,
	const	Uint32		start,
	const	Uint32		stop,
	const	Context		ctx ) const {

	const Uchar *ptr = ptr2 + start;
	const Uint32 len = stop - start;

	switch ( ctx ) {

	case STR : return Token( Buffer( ptr, len ) );
	case INT : return Token( Buffer( ptr, len ).toInt32() );
	case LES : return Token( Token::Less );
	case GRE : return Token( Token::Greater );
	case PLU : return Token( Token::Plus );
	case MIN : return Token( Token::Minus );
	case MUL : return Token( Token::Mult );
	case DIV : return Token( Token::Div );
	case MOD : return Token( Token::Mod );
	case LPA : return Token( Token::ParLeft );
	case RPA : return Token( Token::ParRight );
	case COM : return Token( Token::Comma );
	case DIF : return Token( Token::Not );
	case END :
		if ( len != 2 ) {
			throw InternalError( "Len != 2" );
		}
		if ( ptr[0] == '&' && ptr[1] == '&' )
			return Token( Token::And );
		if ( ptr[0] == '|' && ptr[1] == '|' )
			return Token( Token::Or );
		if ( ptr[0] == '<' && ptr[1] == '=' )
			return Token( Token::LessEqual );
		if ( ptr[0] == '=' && ptr[1] == '=' )
			return Token( Token::Equal );
		if ( ptr[0] == '!' && ptr[1] == '=' )
			return Token( Token::Diff );
		if ( ptr[0] == '>' && ptr[1] == '=' )
			return Token( Token::GreaterEqual );
		throw InternalError(
			"Pos == " + Buffer( start )
			+ ", partial token == \""
			+  Buffer( ptr, len )
			+ "\"" );

	case EQU :
	case BAR :
	case AMP :
	case INV :
		throw IncompleteToken( Buffer( ptr, len ) );

	case NOC :
	case SPA :
	default :
		throw InternalError( "Invalid context in createToken!" );

	}


}

// ============================================================================

const TokenFinder::Context TokenFinder::conv[9][128] = {
{ // NOC
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, SPA, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	SPA, DIF, INV, INV, INV, MOD, AMP, INV, /* 20  !"#$%&'  */
	LPA, RPA, MUL, PLU, COM, MIN, INV, DIV, /* 28 ()*+,-./  */
	INT, INT, INT, INT, INT, INT, INT, INT, /* 30 01234567  */
	INT, INT, INV, INV, LES, EQU, GRE, INV, /* 38 89:;<=>?  */
	INV, STR, STR, STR, STR, STR, STR, STR, /* 40 @ABCDEFG  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 48 HIJKLMNO  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 50 PQRSTUVW  */
	STR, STR, STR, INV, INV, INV, INV, STR, /* 58 XYZ[\]^_  */
	INV, STR, STR, STR, STR, STR, STR, STR, /* 60 `abcdefg  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 68 hijklmno  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 70 pqrstuvw  */
	STR, STR, STR, INV, BAR, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // STR
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 30 01234567  */
	STR, STR, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
	INV, STR, STR, STR, STR, STR, STR, STR, /* 40 @ABCDEFG  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 48 HIJKLMNO  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 50 PQRSTUVW  */
	STR, STR, STR, INV, INV, INV, INV, STR, /* 58 XYZ[\]^_  */
	INV, STR, STR, STR, STR, STR, STR, STR, /* 60 `abcdefg  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 68 hijklmno  */
	STR, STR, STR, STR, STR, STR, STR, STR, /* 70 pqrstuvw  */
	STR, STR, STR, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // INT
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	INT, INT, INT, INT, INT, INT, INT, INT, /* 30 01234567  */
	INT, INT, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
	INV, INV, INV, INV, INT, INV, INV, INV, /* 48 HIJKLMNO  */
	INV, INV, INV, INV, INV, INT, INV, INV, /* 50 PQRSTUVW  */
	INT, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
	INV, INV, INV, INV, INT, INV, INV, INV, /* 68 hijklmno  */
	INV, INV, INV, INV, INV, INT, INV, INV, /* 70 pqrstuvw  */
	INT, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // LES
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // GRE
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // EQU
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // DIF
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // AMP
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, END, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
},
{ // BAR
	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
	INV, INV, INV, INV, END, INV, INV, INV  /* 78 xyz{|}~   */
}
};

// ============================================================================

