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
//	Expr.cpp
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

#include "Expr.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

BFC_PTR_IMPL(Expr)

// ============================================================================

Expr::Expr(
	const	Expr&		e ) : SObject() {

	type = e.type;
	string = e.string;
	integer = e.integer;
	if ( e.left ) { left = new Expr( *e.left ); }
	if ( e.right ) { right = new Expr( *e.right ); }

}

Expr& Expr::operator = (
	const	Expr&		e ) {

	if ( this != &e ) {
		type = e.type;
		string = e.string;
		integer = e.integer;
		if ( e.left ) { left = new Expr( *e.left ); }
		if ( e.right ) { right = new Expr( *e.right ); }
	}

	return *this;

}

// ============================================================================

Int32 Expr::eval(
	const	DefsList&		defs ) const {

	Int32 res;

	switch ( type ) {

	case None :
		throw NotInitialized();

	case OpBinaryPlus :
		res =( left->eval( defs ) + right->eval( defs ) ); break;

	case OpBinaryMinus :
		res =( left->eval( defs ) - right->eval( defs ) ); break;

	case OpBinaryMult :
		res =( left->eval( defs ) * right->eval( defs ) ); break;

	case OpBinaryDiv :
		res =( left->eval( defs ) / right->eval( defs ) ); break;

	case OpBinaryMod :
		res =( left->eval( defs ) % right->eval( defs ) ); break;

	case OpUnaryPlus :
		res =( right->eval( defs ) ); break;

	case OpUnaryMinus :
		res =( - right->eval( defs ) ); break;

	case OpLogicalOr :
		res =( left->eval( defs ) ||  right->eval( defs ) ? 1 : 0 ); break;

	case OpLogicalAnd :
		res =( left->eval( defs ) &&  right->eval( defs ) ? 1 : 0 ); break;

	case OpLogicalNot :
		res =( right->eval( defs ) ? 0 : 1 ); break;

	case OpLogicalLess :
		res =( left->eval( defs ) < right->eval( defs ) ? 1 : 0 ); break;

	case OpLogicalLessEqual :
		res =( left->eval( defs ) <= right->eval( defs ) ? 1 : 0 ); break;

	case OpLogicalEqual :
		res =( left->eval( defs ) == right->eval( defs ) ? 1 : 0 ); break;

	case OpLogicalDiff :
		res =( left->eval( defs ) != right->eval( defs ) ? 1 : 0 ); break;

	case OpLogicalGreaterEqual :
		res =( left->eval( defs ) >= right->eval( defs ) ? 1 : 0 ); break;

	case OpLogicalGreater :
		res =( left->eval( defs ) > right->eval( defs ) ? 1 : 0 ); break;

	case CstInteger :
		res =integer; break;

	case CstString :
		res =interpret( string, defs ); break;

	case OpFnDefined :
		res =( defs.contains( right->string ) ? 1 : 0 ); break;

	default :
		throw NotInitialized();

	}

//	cerr << res << " == " << toBuffer() << endl;

	return res;

}

// ============================================================================

Int32 Expr::interpret(
	const	Buffer&		token,
	const	DefsList&	defs ) const {

	Buffer t = token;

	while ( isFirstTokenChar( t[0] ) ) {
		try {
			t = defs[t];
		}
		catch ( Exception& ) {
//			cerr << "Can't find value of #define \""
//				<< t << "\"!" << endl;
			return 0;
		}
	}

	return t.toInt32();

}

// ============================================================================

Buffer Expr::toBuffer() const {

	Buffer res;

	if ( left ) {
		if ( left->left || left->right ) {
			res += "( ";
		}
		res += left->toBuffer();
		if ( left->left || left->right ) {
			res += " )";
		}
	}

	switch ( type ) {
	case None : res += "<NONE>"; break;
	case CstString : res += string; break;
	case CstInteger : res += integer; break;
	case OpBinaryPlus : res += " + "; break;
	case OpBinaryMinus : res += " - "; break;
	case OpBinaryMult : res += " * "; break;
	case OpBinaryDiv : res += " / "; break;
	case OpBinaryMod : res += " % "; break;
	case OpUnaryPlus : res += "+ "; break;
	case OpUnaryMinus : res += "- "; break;
	case OpFnDefined : res += "defined "; break;
	case OpLogicalNot : res += "NOT "; break;
	case OpLogicalAnd : res += " AND "; break;
	case OpLogicalOr : res += " OR "; break;
	case OpLogicalLess : res += " < "; break;
	case OpLogicalLessEqual : res += " <= "; break;
	case OpLogicalEqual : res += " == "; break;
	case OpLogicalDiff : res += " != "; break;
	case OpLogicalGreaterEqual : res += " >= "; break;
	case OpLogicalGreater : res += " > "; break;
	}

	if ( right ) {
		if ( right->left || right->right ) {
			res += "( ";
		}
		res += right->toBuffer();
		if ( right->left || right->right ) {
			res += " )";
		}
	}

	return res;

}

// ============================================================================

