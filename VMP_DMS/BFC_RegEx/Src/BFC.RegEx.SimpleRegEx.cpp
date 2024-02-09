// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::RegEx".
// 
// "BFC::RegEx" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::RegEx" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::RegEx"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.RegEx.SimpleRegEx.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <ctype.h>

#include "BFC.Base.System.h"

#include "BFC.RegEx.SimpleRegEx.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( RegEx, SimpleRegEx )

// ============================================================================

RegEx::SimpleRegEx::SimpleRegEx() {

	if ( ! tblInit ) {

		for ( Uint32 ch = 0 ; ch < 0x100 ; ch++ ) {
			escTable[ ch ] = ( Uchar )ch;
		}

		escTable[ ( Uchar )'a' ] = '\a';
		escTable[ ( Uchar )'b' ] = '\b';
		escTable[ ( Uchar )'f' ] = '\f';
		escTable[ ( Uchar )'n' ] = '\n';
		escTable[ ( Uchar )'r' ] = '\r';
		escTable[ ( Uchar )'t' ] = '\t';
		escTable[ ( Uchar )'v' ] = '\v';

		tblInit = true;

	}

}

RegEx::SimpleRegEx::~SimpleRegEx() {



}

// ============================================================================

void RegEx::SimpleRegEx::compile(
	const	Buffer		& pattern,
		Bool		caseSensitive ) {

	if ( pattern.isEmpty() ) {
		throw InvalidArgument();
	}

	lastCompile = pattern;
	lastExecute.kill();
	lastMatches.kill();

	const Uchar	* patBeg = lastCompile.getCstPtr();
	const Uint32	patLen = lastCompile.getLength();
	const Uchar	* patEnd = patBeg + patLen;

	Uchar		* nfaCur = nfa;
	const Uchar	* nfaMax = nfa + MAXNFA - 32 - 10;
	Uchar		* nfaTmp;
	Uchar		* nfaOld = nfa;

	Uint32		tagi = 0;	/* tag stack index */
	Uint32		tagc = 1;	/* actual tag count*/

	for ( const Uchar * patCur = patBeg ; patCur < patEnd ; patCur++ ) {

		if ( nfaCur > nfaMax ) {
			throw InvalidArgument( "Pattern too long" );
		}

		nfaTmp = nfaCur;

		switch ( patCur[ 0 ] ) {

		case '.' :	// Any char...
			*nfaCur++ = ANY;
			break;

		case '^' :	// Beginning of line...
			if ( patCur == patBeg ) {
				*nfaCur++ = BOL;
			}
			else {
				*nfaCur++ = CHR;
				*nfaCur++ = *patCur;
			}
			break;

		case '$' :	// End of line...
			if ( patCur + 1 == patEnd ) {
				*nfaCur++ = EOL;
			}
			else {
				*nfaCur++ = CHR;
				*nfaCur++ = *patCur;
			}
			break;

		case '[' : {	// Char class...

			* nfaCur++ = CCL;

			clrSet( nfaCur );

			patCur++;

			Uchar mask = 0;

			if ( patCur[ 0 ] == '^' ) {
				mask = 0xFF;
				patCur++;
			}
			if ( patCur[ 0 ] == '-' ) {
				addSet( nfaCur, '-' );
				patCur++;
			}
			if ( patCur[ 0 ] == ']' ) {
				addSet( nfaCur, ']' );
				patCur++;
			}
			while ( patCur < patEnd && patCur[ 0 ] != ']' ) {
				if ( patCur[ 0 ] == '-'
				  && patCur + 1 < patEnd
				  && patCur[ 1 ] != ']' ) {
					Uint32 c1 = patCur[ -1 ] + 1;
					Uint32 c2 = patCur[  1 ];
					while ( c1 <= c2 ) {
						addSet( nfaCur, c1++, caseSensitive );
					}
					patCur += 2;
				}
				else if ( patCur[ 0 ] == '\\'
				       && patCur + 1 < patEnd ) {
					addSet( nfaCur, escapeValue( patCur[ 1 ] ), caseSensitive );
					patCur += 2;
				}
				else {
					addSet( nfaCur, * patCur, caseSensitive );
					patCur++;
				}
			}
			if ( patCur >= patEnd ) {
				throw InvalidArgument("Missing ]");
			}
			if ( mask ) {
				invSet( nfaCur );
			}
			nfaCur += 32;
			break;
			}

		case '*':	// 0 or more...
		case '+':	// 1 or more...

			if ( patCur == patBeg ) {
				throw InvalidArgument("Empty closure");
			}

			nfaTmp = nfaOld;	// previous opcode

			if ( * nfaTmp == CLO ) {
				break;
			}

			switch ( * nfaTmp ) {
			case BOL:
			case BOT:
			case EOT:
			case BOW:
			case EOW:
			case REF:
				throw InvalidArgument("Illegal closure");
			default:
				break;
			}

			if ( * patCur == '+' ) {
				// Copy previous opcode...
				Uint32 delta = nfaCur - nfaTmp;
				System::memcpy(
					nfaCur,
					nfaTmp,
					delta );
				nfaCur += delta;
				nfaTmp += delta;
			}

			*nfaCur++ = END;
			*nfaCur++ = END;

			nfaOld = nfaCur;

			while ( --nfaCur > nfaTmp ) {
				*nfaCur = nfaCur[-1];
			}

			*nfaCur = CLO;
			nfaCur = nfaOld;

			break;

		case '\\' :	// Tags, backrefs, ...

			patCur++;

			switch ( patCur[ 0 ] ) {

			case '<':
				* nfaCur ++ = BOW;
				break;

			case '>':
				if ( * nfaOld == BOW ) {
					throw InvalidArgument("Null pattern inside \\<\\>");
				}
				* nfaCur ++ = EOW;
				break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				Uint32 n = patCur[ 0 ] - '0';
				if ( tagi > 0 && tagstk[ tagi ] == n ) {
					throw InvalidArgument("Cyclical reference");
				}
				if ( tagc > n ) {
					* nfaCur ++ = REF;
					* nfaCur ++ = ( Uchar )n;
				}
				else {
					throw InvalidArgument("Undetermined reference");
				}
				break;
				}

			case 'a':
			case 'b':
			case 'n':
			case 'f':
			case 'r':
			case 't':
			case 'v':
				* nfaCur ++ = CHR;
				* nfaCur ++ = escapeValue( * patCur );
				break;

			case 'd': *nfaCur++ = PCL; *nfaCur++ = PCL_DIG; break;
			case 'D': *nfaCur++ = PCL; *nfaCur++ = PCL_NDG; break;
			case 's': *nfaCur++ = PCL; *nfaCur++ = PCL_WSP; break;
			case 'S': *nfaCur++ = PCL; *nfaCur++ = PCL_NWS; break;
			case 'w': *nfaCur++ = PCL; *nfaCur++ = PCL_WRD; break;
			case 'W': *nfaCur++ = PCL; *nfaCur++ = PCL_NWD; break;

			default:
				*nfaCur++ = CHR;
				*nfaCur++ = *patCur;
			}

			break;

		case '(' :

			if ( tagc >= MAXTAG ) {
				throw InvalidArgument("Too many () pairs");
			}

			tagstk[ ++ tagi ] = tagc;
			* nfaCur++ = BOT;
			* nfaCur++ = ( Uchar )tagc++;

			break;

		case ')' :

			if ( * nfaOld == BOT ) {
				throw InvalidArgument("Null pattern inside ()");
			}

			if ( tagi == 0 ) {
				throw InvalidArgument("Unmatched )");
			}

			* nfaCur++ = EOT;
			* nfaCur++ = ( Uchar )tagstk[ tagi-- ];

			break;

		default :	// Ordinary char...

			if ( caseSensitive ) {
				* nfaCur++ = CHR;
				* nfaCur++ = * patCur;
			}
			else {
				* nfaCur++ = CCL;
				clrSet( nfaCur );
				addSet( nfaCur, * patCur, false );
				nfaCur += 32;
			}
			break;
		}
		nfaOld = nfaTmp;
	}

	if ( tagi > 0 ) {
		throw InvalidArgument( "Unmatched (" );
	}

	*nfaCur = END;

}

// ============================================================================

Bool RegEx::SimpleRegEx::execute(
	const	Buffer		& __ci ) {

	if ( lastCompile.isEmpty() ) {
		throw NotValid();
	}

	lastExecute = __ci;
	lastMatches.kill();

	int	minPos = 0;
	int	maxPos = ( int )__ci.getLength();

	for ( int i=0; i<MAXTAG; i++ ) {
		bopat[i] = NOTFOUND;
		eopat[i] = NOTFOUND;
	}

	char c;
	int	endPos = NOTFOUND;
	const Uchar	* ap = nfa;

	switch ( *ap ) {

	case BOL :
		endPos = PMatch(
			lastExecute,
			minPos,
			maxPos,
			ap );
		break;

	case EOL :
		if ( ap[ 1 ] != END ) {
			return false;
		}
		minPos = maxPos;
		endPos = minPos;
		break;

	case CHR :
		c = ap[ 1 ];
		while ( minPos < maxPos && lastExecute[ minPos ] != c ) {
			minPos++;
		}
		if ( minPos >= maxPos ) {
			return false;
		}

	default :
		while ( minPos < maxPos ) {
			endPos = PMatch(
				lastExecute,
				minPos,
				maxPos,
				ap );
			if ( endPos != NOTFOUND ) {
				break;
			}
			minPos++;
		}
		break;

	case END :
		return false;

	}

	if ( endPos == NOTFOUND ) {
		return false;
	}

//	setMatch( 0, ci( minPos, endPos - minPos ) );

	bopat[ 0 ] = minPos;
	eopat[ 0 ] = endPos;

	return true;

}

// ============================================================================

const BufferArray & RegEx::SimpleRegEx::grabMatches() {

	if ( lastCompile.isEmpty() ) {
		throw NotValid();
	}

	if ( lastExecute.isEmpty() ) {
		return lastMatches;
	}

	if ( lastMatches.isEmpty() ) {
		for ( Uint32 i = 0 ; i < MAXTAG ; i++ ) {
			if ( bopat[ i ] != NOTFOUND && eopat[ i ] != NOTFOUND ) {
				setMatch(
					i,
					lastExecute( bopat[ i ], eopat[ i ] - bopat[ i ] ) );
			}
		}
	}

	return lastMatches;

}

// ============================================================================

Buffer RegEx::SimpleRegEx::substitute(
	const	Buffer		& src ) {

	grabMatches();

	if ( lastMatches.isEmpty() ) {
		throw NotValid();
	}

	if ( src.isEmpty() ) {
		return Buffer();
	}

	const Uchar	* p = src.getCstPtr();
	const	Uint32	l = src.getLength();
	Uint32		i;
	Buffer		r;
	Uchar		c;

	for ( i = 0 ; i < l ; i++ ) {
		c = *p++;
		if ( c == '\\' ) {
			if ( i + 1 == l ) {
				throw InvalidArgument();
			}
			c = *p++;
			if ( c >= '0' && c <= '9' ) {
				if ( ( Uint32 )c < lastMatches.getSize() ) {
					r += lastMatches[ ( Uint32 )c ];
				}
			}
			else {
				r += c;
			}
		}
		else {
			r += c;
		}
	}

	return r;

}

// ============================================================================

#define ANYSKIP 2 	/* [CLO] ANY END*/
#define CHRSKIP 3	/* [CLO] CHR chr END*/
#define CCLSKIP 34	/* [CLO] CCL 32 bytes END */

int RegEx::SimpleRegEx::PMatch(
	const	Buffer		& ci,
		int		minPos,
		int		maxPos,
	const	Uchar		* nfaPtr ) {

	Uchar	c;
	int op, n;
	int e;		/* extra pointer for CLO*/
	int begPos;		/* beginning of subpat... */
	int endPos;		/* ending of subpat...*/
	int are;	/* to save the line ptr.*/

	while ( ( op = *nfaPtr++ ) != END ) {


		switch ( op ) {

		case CHR:
			if ( minPos >= maxPos ) {
				return NOTFOUND;
			}
			if ( ci[ minPos++ ] != *nfaPtr++ ) {
				return NOTFOUND;
			}
			break;

		case ANY:
			if ( minPos >= maxPos ) {
				return NOTFOUND;
			}
			minPos++;
			break;

		case CCL:
			if ( minPos >= maxPos ) {
				return NOTFOUND;
			}
			if ( ! isInSet( nfaPtr, ci[ minPos++ ] ) )
				return NOTFOUND;
			nfaPtr += 32;
			break;

		case BOL:
			if ( minPos ) {
				return NOTFOUND;
			}
			break;

		case EOL:
			if ( minPos < maxPos ) {
				return NOTFOUND;
			}
			break;

		case BOT:
			bopat[ ( int )*nfaPtr++ ] = minPos;
			break;

		case EOT:
			eopat[ ( int )*nfaPtr++ ] = minPos;
			break;

 		case BOW:
			if ( minPos >= maxPos ) {
				return NOTFOUND;
			}
			if ( ( minPos && isWrd( ci[ minPos - 1 ] ) )
			  || ! isWrd( ci[ minPos ] ) ) {
				return NOTFOUND;
			}
			break;

		case EOW:
			if ( minPos > maxPos ) {
				return NOTFOUND;
			}
			if ( minPos == maxPos ) {
				break;
			}
			if ( ! minPos
			  || ! isWrd( ci[ minPos - 1 ] )
			  || isWrd( ci[ minPos ] ) ) {
				return NOTFOUND;
			}
			break;

		case REF:
			n = *nfaPtr++;
			begPos = bopat[ n ];
			endPos = eopat[ n ];
			while ( begPos < endPos ) {
				if ( minPos >= maxPos ) {
					return NOTFOUND;
				}
				if ( ci[ begPos++ ] != ci[ minPos++ ] ) {
					return NOTFOUND;
				}
			}
			break;

		case CLO:
			are = minPos;
			switch ( *nfaPtr ) {
			case ANY:
				while ( minPos < maxPos ) {
					minPos++;
				}
				n = ANYSKIP;
				break;
			case CHR:
				c = *(nfaPtr+1);
				while ( minPos < maxPos && c == ci[ minPos ] ) {
					minPos++;
				}
				n = CHRSKIP;
				break;
			case CCL:
				while ( minPos < maxPos && isInSet( nfaPtr + 1, ci[ minPos ] ) ) {
					minPos++;
				}
				n = CCLSKIP;
				break;
			default:
				return NOTFOUND;
			}
			nfaPtr += n;
			while ( minPos >= are ) {
				if ( ( e = PMatch( ci, minPos, maxPos, nfaPtr ) ) != NOTFOUND ) {
					return e;
				}
				--minPos;
			}
			return NOTFOUND;

		case PCL: {
			if ( minPos >= maxPos ) {
				return NOTFOUND;
			}
			Uint32	p = *nfaPtr++;
			Uchar	c2 = ci[ minPos++ ];

			switch ( p ) {
			case PCL_DIG : if ( ! isDig( c2 ) ) return NOTFOUND; break;
			case PCL_NDG : if ( isDig( c2 ) ) return NOTFOUND; break;
			case PCL_WSP : if ( ! isWsp( c2 ) ) return NOTFOUND; break;
			case PCL_NWS : if ( isWsp( c2 ) ) return NOTFOUND; break;
			case PCL_WRD : if ( ! isWrd( c2 ) ) return NOTFOUND; break;
			case PCL_NWD : if ( isWrd( c2 ) ) return NOTFOUND; break;
			}
			break;
			}

		default:
			return NOTFOUND;

		}

	}

	return minPos;

}

// ============================================================================

void RegEx::SimpleRegEx::setMatch(
	const	Uint32		index,
	const	Buffer		& pattern ) {

	if ( index >= lastMatches.getSize() ) {
		lastMatches.resize( index + 1 );
	}

	lastMatches[ index ] = pattern;

}

// ============================================================================

void RegEx::SimpleRegEx::clrSet(
		Uchar		* p ) {

	System::bzero( p, 32 );

}

// ============================================================================

Bool	RegEx::SimpleRegEx::tblInit = false;
Uchar	RegEx::SimpleRegEx::escTable[ 0x100 ];

// ============================================================================

