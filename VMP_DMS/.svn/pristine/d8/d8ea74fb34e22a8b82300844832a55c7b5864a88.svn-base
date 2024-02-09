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
//	Directive.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _Directive_H_
#define _Directive_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

// not using any namespace {

BFC_PTR_DECL(JBS_DLL_NONE,Directive)

// } // not using any namespace

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.TL.List.h"
#include "BFC.Base.SObject.h"

#include "Expr.h"

// ============================================================================

// not using any namespace {

// ============================================================================

class JBS_DLL_NONE Directive : virtual public BFC::SObject {

public :

	enum Type {
		None,
		If,
		Elif,
		Else,
		Endif,
		Ifdef,
		Ifndef,
		Define,
		Undef,
		Include,
		Warning,
		Error
	};

	Directive(
		const	Type		t = None
	) : type( t ) {
	}

	Directive(
		const	Type		t,
			ExprPtr		e
	) : type( t ), expr( e ) {
	}

	Directive(
		const	Type		t,
		const	BFC::Buffer&	s
	) : type( t ), str1( s ) {
	}

	Directive(
		const	Type		t,
		const	BFC::Buffer&	s1,
		const	BFC::Buffer&	s2
	) : type( t ), str1( s1 ), str2( s2 ) {
	}

	Directive(
		const	Directive&	d
	) : SObject(), type( d.type ), expr( d.expr ), str1( d.str1 ), str2( d.str2 ) {
	}

	virtual ~Directive(
	) {
	}

	Directive& operator = (
		const	Directive&
	);

	Type		type;
	ExprPtr		expr;
	BFC::Buffer	str1;
	BFC::Buffer	str2;

protected :

private :

};

typedef BFC::List< DirectivePtr > DirectivesList;

// ============================================================================

// } // not using any namespace

// ============================================================================

#endif // _Directive_H_

// ============================================================================

