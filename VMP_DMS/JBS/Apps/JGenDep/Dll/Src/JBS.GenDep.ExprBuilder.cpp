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
//	JBS.GenDep.ExprBuilder.cpp
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

#include "JBS.GenDep.ExprBuilder.h"

// ============================================================================

using namespace TBFC;
using namespace JBS;

// ============================================================================

TBFC_PTR_IMPL_NS( GenDep, ExprBuilder )

// ============================================================================

GenDep::ExprBuilder::ExprBuilder() {



}

GenDep::ExprBuilder::~ExprBuilder() {



}

// ============================================================================

GenDep::ExprPtr GenDep::ExprBuilder::process(
	const	TokenArray &	tokens ) const {

	Uint32	pos = 0;
	ExprPtr	res = process( tokens, pos );

	if ( pos < tokens.size() ) {
		throw JunkAtEnd( "Starting at token at pos. "
			+ Buffer( pos ) );
	}

	return res;

}

// ============================================================================

GenDep::ExprPtr GenDep::ExprBuilder::process(
	const	TokenArray &	tokens,
		Uint32 &	pos ) const {

//	cerr << "Processing tokens, starting at " << pos << endl;

	ExprArray	subs;
	Bool		waitingOper = true;
	Bool		eot = false;

	for ( ; pos < tokens.size() && ! eot ; pos++ ) {

		if ( waitingOper ) {
			switch ( tokens[pos].type ) {
			case Token::CstString :
				if ( tokens[pos].string == "defined" ) {
					subs += new Expr( Expr::OpFnDefined );
				}
				else {
					subs += new Expr( tokens[pos].string );
					waitingOper = false;
				}
				break;
			case Token::CstInteger :
				subs += new Expr( ( Int32 )tokens[pos].integer );
				waitingOper = false;
				break;
			case Token::ParLeft :
				pos++;
				subs += process( tokens, pos );
				if ( tokens[pos].type != Token::ParRight ) {
					throw SyntaxError( "Missing right parenthesis!" );
				}
				waitingOper = false;
				break;
			case Token::Plus :
				subs += new Expr( Expr::OpUnaryPlus );
				break;
			case Token::Minus :
				subs += new Expr( Expr::OpUnaryMinus );
				break;
			case Token::Not :
				subs += new Expr( Expr::OpLogicalNot );
				break;
			case Token::None :
				throw InternalError();
			default :
				throw SyntaxError(
					"Invalid token \""
					+ tokens[pos].toBuffer()
					+ "\" at pos. " + Buffer( pos ) );
			}
		}
		else { // Waiting operator || end of tokens
			switch ( tokens[pos].type ) {
			case Token::ParRight :
				pos--;
				eot = true;
				break;
			case Token::Plus :
				subs += new Expr( Expr::OpBinaryPlus );
				waitingOper = true;
				break;
			case Token::Minus :
				subs += new Expr( Expr::OpBinaryMinus );
				waitingOper = true;
				break;
			case Token::Mult :
				subs += new Expr( Expr::OpBinaryMult );
				waitingOper = true;
				break;
			case Token::Div :
				subs += new Expr( Expr::OpBinaryDiv );
				waitingOper = true;
				break;
			case Token::Mod :
				subs += new Expr( Expr::OpBinaryMod );
				waitingOper = true;
				break;
			case Token::And :
				subs += new Expr( Expr::OpLogicalAnd );
				waitingOper = true;
				break;
			case Token::Or :
				subs += new Expr( Expr::OpLogicalOr );
				waitingOper = true;
				break;
			case Token::Less :
				subs += new Expr( Expr::OpLogicalLess );
				waitingOper = true;
				break;
			case Token::LessEqual :
				subs += new Expr( Expr::OpLogicalLessEqual );
				waitingOper = true;
				break;
			case Token::Equal :
				subs += new Expr( Expr::OpLogicalEqual );
				waitingOper = true;
				break;
			case Token::Diff :
				subs += new Expr( Expr::OpLogicalDiff );
				waitingOper = true;
				break;
			case Token::GreaterEqual :
				subs += new Expr( Expr::OpLogicalGreaterEqual );
				waitingOper = true;
				break;
			case Token::Greater :
				subs += new Expr( Expr::OpLogicalGreater );
				waitingOper = true;
				break;
			case Token::None :
				throw InternalError();
			case Token::CstString :
			case Token::CstInteger :
			case Token::Not :
			case Token::ParLeft :
				throw SyntaxError( "Missing operator!" );
			case Token::Comma :
				throw SyntaxError( "Invalid comma operator!" );
			case Token::LShift :
			case Token::RShift :
			case Token::BitAnd :
			case Token::BitOr :
			case Token::BitCompl :
			case Token::BitXor :
				throw SyntaxError( "Unsupported operator!" );
			}
		}

//		cerr << "Partial expressions..." << endl;
//		for ( Uint32 i = 0 ; i < subs.size() ; i++ ) {
//			cerr << i << ": " << subs[i]->toBuffer() << endl;
//		}

	} // for

	if ( waitingOper ) {
		throw SyntaxError( "Missing operand!" );
	}

	if ( subs.isEmpty() ) {
		throw SyntaxError( "No content!" );
	}

	Uint32 i;

	// Now, reduce expressions...

	// ... right to left: unary operators...

//	cerr << "Reducing unary operators..." << endl;

	for ( i = subs.size() - 1 ; i ; i-- ) {
		if ( subs[i-1]->isUnaryOp() ) {
//			cerr << "Merging:" << endl;
//			cerr << "[Unary op]: " << subs[i-1]->toBuffer() << endl;
//			cerr << "[Expressi]: " << subs[i]->toBuffer() << endl;
			subs[i-1]->right = subs[i];
			subs.pop(i);
//			cerr << "Reduced: " << subs[i-1]->toBuffer() << endl;
		}
	}

	// ... left to right: binary operators...

//	cerr << "Reducing binary operators (size == "
//		<< subs.size() << ")..." << endl;

	for ( i = 0 ; i + 2 < subs.size() ; ) {
		if ( ( i + 4 >= subs.size() )
		  || ( prio[subs[i+1]->type] >= prio[subs[i+3]->type] ) ) {
			subs[i+1]->left = subs[i];
			subs[i+1]->right = subs[i+2];
			subs.pop(i+2);
			subs.pop(i);
//			cerr << "Reduced: " << subs[i]->toBuffer() << endl;
			if ( i >= 2 ) {
				i -= 2;
			}
		}
		else {
			i += 2;
		}
	}

	if ( subs.size() != 1 ) {
		throw InternalError( "Can't reduce completely!" );
	}

	return subs[0];

}

// ============================================================================

Uint32 GenDep::ExprBuilder::prio[] = {

	0,	// None = 0,
	0,	// CstString,
	0,	// CstInteger,
	5,	// OpBinaryPlus,
	5,	// OpBinaryMinus,
	6,	// OpBinaryMult,
	6,	// OpBinaryDiv,
	6,	// OpBinaryMod,
	0,	// OpUnaryPlus,
	0,	// OpUnaryMinus,
	0,	// OpFnDefined,
	0,	// OpLogicalNot,
	2,	// OpLogicalAnd,
	1,	// OpLogicalOr,
	4,	// OpLogicalLess,
	4,	// OpLogicalLessEqual,
	3,	// OpLogicalEqual,
	3,	// OpLogicalDiff,
	4,	// OpLogicalGreaterEqual,
	4	// OpLogicalGreater

};

// ============================================================================

