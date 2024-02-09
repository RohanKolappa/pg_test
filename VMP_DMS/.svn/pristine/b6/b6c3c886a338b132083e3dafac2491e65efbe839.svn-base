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
//	JBS.GenDep.TokenFinder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.GenDep.TokenFinder.h"

// ============================================================================

using namespace TBFC;
using namespace JBS;

// ============================================================================

TBFC_PTR_IMPL_NS( GenDep, TokenFinder )

// ============================================================================

GenDep::TokenFinder::TokenFinder() {



}

GenDep::TokenFinder::~TokenFinder() {



}

// ============================================================================

void GenDep::TokenFinder::process(
	const	Buffer &	line,
		TokenArray &	list ) const {

	list.kill();

	const Uint32	len = line.getLength();
//	Uint32		buf = 0;
//	Context		ctx = NOC;

	for ( Uint32 cur = 0 ; cur < len ; ) {

		Uchar c = line[ cur++ ];

		if ( c == '(' ) {
			list += Token( Token::ParLeft );
		}
		else if ( c == ')' ) {
			list += Token( Token::ParRight );
		}
		else if ( c == ',' ) {
			list += Token( Token::Comma );
		}
		else if ( c == '+' ) {
			list += Token( Token::Plus );
		}
		else if ( c == '-' ) {
			list += Token( Token::Minus );
		}
		else if ( c == '*' ) {
			list += Token( Token::Mult );
		}
		else if ( c == '/' ) {
			list += Token( Token::Div );
		}
		else if ( c == '%' ) {
			list += Token( Token::Mod );
		}
		else if ( c == '~' ) {
			list += Token( Token::BitCompl );
		}
		else if ( c == '^' ) {
			list += Token( Token::BitXor );
		}
		else if ( c == '<' ) {
			Uchar d = line[ cur ];
			if ( d == '<' ) {
				list += Token( Token::LShift ); cur++;
			}
			else if ( d == '=' ) {
				list += Token( Token::LessEqual ); cur++;
			}
			else {
				list += Token( Token::Less );
			}
		}
		else if ( c == '>' ) {
			Uchar d = line[ cur ];
			if ( d == '>' ) {
				list += Token( Token::RShift ); cur++;
			}
			else if ( d == '=' ) {
				list += Token( Token::GreaterEqual ); cur++;
			}
			else {
				list += Token( Token::Greater );
			}
		}
		else if ( c == '&' ) {
			if ( line[ cur ] == '&' ) {
				list += Token( Token::And ); cur++;
			}
			else {
				list += Token( Token::BitAnd );
			}
		}
		else if ( c == '|' ) {
			if ( line[ cur ] == '|' ) {
				list += Token( Token::Or ); cur++;
			}
			else {
				list += Token( Token::BitOr );
			}
		}
		else if ( c == '!' ) {
			if ( line[ cur ] == '=' ) {
				list += Token( Token::Diff ); cur++;
			}
			else {
				list += Token( Token::Not );
			}
		}
		else if ( c == '=' ) {
			if ( line[ cur ] == '=' ) {
				list += Token( Token::Equal ); cur++;
			}
			else {
				throw InvalidCharacter();
			}
		}
		else if ( c >= '0' && c <= '9' ) {
			Uint32 pos = cur - 1;
			Int64 val = line.getIntAt( pos );
			cur = pos;
			list += Token( val );
		}
		else if ( isAlpha( c ) ) {
			Uint32 pos = cur - 1;
			while ( cur < len && isAlpha( line[ cur ] ) ) {
				cur++;
			}
			list += Token( line( pos, cur - pos ) );
		}
		else {
			throw InvalidCharacter(
				"Char '" + Buffer( ( Uchar )c )
				+ "' at pos " + Buffer( cur )
				+ " in line \""
				+ line + "\"" );
		}

//		Context newctx =
//			( ctx >= PLU || c >= 0x80
//			? INV
//			: conv[ctx][c] );
//
//		if ( buf < cur ) {
//			if ( newctx == INV ) {
//				if ( ctx != SPA ) {
//					list += createToken( line, buf, cur, ctx );
//				}
//				buf = cur;
//				ctx = NOC;
//				if ( c < 0x80 ) {
//					newctx = conv[ctx][c];
//				}
//			}
//		}
//
//		if ( newctx == INV ) {
//			throw InvalidCharacter(
//				"Char '" + Buffer( ( Uchar )c )
//				+ "' at pos " + Buffer( cur )
//				+ " in line \""
//				+ line + "\"" );
//		}
//
//		ctx = newctx;

	}

//	if ( buf < len && ctx != SPA ) {
//		list += createToken( line, buf, cur, ctx );
//	}

}

// ============================================================================

//GenDep::Token GenDep::TokenFinder::createToken(
//	const	Uchar		*ptr2,
//	const	Uint32		start,
//	const	Uint32		stop,
//	const	Context		ctx ) const {
//
//	const Uchar *ptr = ptr2 + start;
//	const Uint32 len = stop - start;
//
//	switch ( ctx ) {
//
//	case STR : return Token( Buffer( ptr, len ) );
//	case INT : return Token( Buffer( ptr, len ).toInt64() );
//	case LES : return Token( Token::Less );
//	case GRE : return Token( Token::Greater );
//	case PLU : return Token( Token::Plus );
//	case MIN : return Token( Token::Minus );
//	case MUL : return Token( Token::Mult );
//	case DIV : return Token( Token::Div );
//	case MOD : return Token( Token::Mod );
//	case LPA : return Token( Token::ParLeft );
//	case RPA : return Token( Token::ParRight );
//	case COM : return Token( Token::Comma );
//	case DIF : return Token( Token::Not );
//	case END :
//		if ( len != 2 ) {
//			throw InternalError( "Len != 2" );
//		}
//		if ( ptr[0] == '&' && ptr[1] == '&' )
//			return Token( Token::And );
//		if ( ptr[0] == '|' && ptr[1] == '|' )
//			return Token( Token::Or );
//		if ( ptr[0] == '<' && ptr[1] == '=' )
//			return Token( Token::LessEqual );
//		if ( ptr[0] == '=' && ptr[1] == '=' )
//			return Token( Token::Equal );
//		if ( ptr[0] == '!' && ptr[1] == '=' )
//			return Token( Token::Diff );
//		if ( ptr[0] == '>' && ptr[1] == '=' )
//			return Token( Token::GreaterEqual );
//		throw InternalError(
//			"Pos == " + Buffer( start )
//			+ ", partial token == \""
//			+  Buffer( ptr, len )
//			+ "\"" );
//
//	case EQU :
//	case BAR :
//	case AMP :
//	case INV :
//		throw IncompleteToken( Buffer( ptr, len ) );
//
//	case NOC :
//	case SPA :
//	default :
//		throw InternalError( "Invalid context in createToken!" );
//
//	}
//
//
//}

// ============================================================================

//const GenDep::TokenFinder::Context GenDep::TokenFinder::conv[9][128] = {
//{ // NOC
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, SPA, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	SPA, DIF, INV, INV, INV, MOD, AMP, INV, /* 20  !"#$%&'  */
//	LPA, RPA, MUL, PLU, COM, MIN, INV, DIV, /* 28 ()*+,-./  */
//	INT, INT, INT, INT, INT, INT, INT, INT, /* 30 01234567  */
//	INT, INT, INV, INV, LES, EQU, GRE, INV, /* 38 89:;<=>?  */
//	INV, STR, STR, STR, STR, STR, STR, STR, /* 40 @ABCDEFG  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 48 HIJKLMNO  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 50 PQRSTUVW  */
//	STR, STR, STR, INV, INV, INV, HAT, STR, /* 58 XYZ[\]^_  */
//	INV, STR, STR, STR, STR, STR, STR, STR, /* 60 `abcdefg  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 68 hijklmno  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 70 pqrstuvw  */
//	STR, STR, STR, INV, BAR, INV, TIL, INV  /* 78 xyz{|}~   */
//},
//{ // STR
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 30 01234567  */
//	STR, STR, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
//	INV, STR, STR, STR, STR, STR, STR, STR, /* 40 @ABCDEFG  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 48 HIJKLMNO  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 50 PQRSTUVW  */
//	STR, STR, STR, INV, INV, INV, INV, STR, /* 58 XYZ[\]^_  */
//	INV, STR, STR, STR, STR, STR, STR, STR, /* 60 `abcdefg  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 68 hijklmno  */
//	STR, STR, STR, STR, STR, STR, STR, STR, /* 70 pqrstuvw  */
//	STR, STR, STR, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
//},
//{ // INT
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	INT, INT, INT, INT, INT, INT, INT, INT, /* 30 01234567  */
//	INT, INT, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
//	INV, INV, INV, INV, INT, INV, INV, INV, /* 48 HIJKLMNO  */
//	INV, INV, INV, INV, INV, INT, INV, INV, /* 50 PQRSTUVW  */
//	INT, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
//	INV, INV, INV, INV, INT, INV, INV, INV, /* 68 hijklmno  */
//	INV, INV, INV, INV, INV, INT, INV, INV, /* 70 pqrstuvw  */
//	INT, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
//},
//{ // LES
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
//	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
//	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
//},
//{ // GRE
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
//	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
//	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
//},
//{ // EQU
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
//	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
//	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
//},
//{ // DIF
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
//	INV, INV, INV, INV, INV, END, INV, INV, /* 38 89:;<=>?  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
//	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
//},
//{ // AMP
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, END, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
//	INV, INV, INV, INV, INV, INV, INV, INV  /* 78 xyz{|}~   */
//},
//{ // BAR
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 00           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 08           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 10           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 18           */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 20  !"#$%&'  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 28 ()*+,-./  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 30 01234567  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 38 89:;<=>?  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 40 @ABCDEFG  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 48 HIJKLMNO  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 50 PQRSTUVW  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 58 XYZ[\]^_  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 60 `abcdefg  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 68 hijklmno  */
//	INV, INV, INV, INV, INV, INV, INV, INV, /* 70 pqrstuvw  */
//	INV, INV, INV, INV, END, INV, INV, INV  /* 78 xyz{|}~   */
//}
//};

// ============================================================================

