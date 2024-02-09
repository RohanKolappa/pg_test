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
//	BFC.Base.WBuffer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_WBuffer_H_
#define _BFC_Base_WBuffer_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.WChar.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Buffer of WChar.
///
/// \ingroup BFC_Base

class BFC_Light_DLL WBuffer {

public :

	static const Uint32 InvalidIndex = ( Uint32 )-1;

	static const WBuffer & getEmpty(
	);

	/// \brief Creates a new WBuffer.

	WBuffer(
	) {
	}

	/// \brief Creates a new WBuffer.

	WBuffer(
		const	Buffer &	pBuffer
	);

	/// \brief Creates a new WBuffer.

	WBuffer(
		const	char *		pBuffer
	);

	/// \brief Creates a new WBuffer.

	WBuffer(
		const	WChar		pWChar
	);

	/// \brief Copy constructor.

	WBuffer(
		const	WBuffer &	pOther
	) :
		b	( pOther.b )
	{
	}

	/// \brief Copy operator.

	WBuffer & operator = (
		const	WBuffer &	pOther
	) {
		b = pOther.b;
		return *this;
	}

	void kill(
	) {
		b.kill();
	}

	void chop(
		const	Uint32		pOffset
	) {
		b = b( 0, pOffset * 4 );
	}

	Bool operator == (
		const	WBuffer &	pOther
	) const {
		return ( b == pOther.b );
	}

	Bool operator != (
		const	WBuffer &	pOther
	) const {
		return ( b != pOther.b );
	}

	Bool isEmpty(
	) const {
		return ( b.isEmpty() );
	}

	Bool hasContent(
	) const {
		return ( b.hasContent() ) ;
	}

	Uint32 getLength(
	) const {
		return ( b.getLength() / 4 );
	}

	const WChar * getCstPtr(
		const	Uint32		pIndex = 0
	) const {
		if ( pIndex >= getLength() ) {
			throw InvalidArgument();
		}
		return ( const WChar * )b.getCstPtr() + pIndex;
	}

	WChar * getVarPtr(
		const	Uint32		pIndex = 0
	) {
		if ( pIndex >= getLength() ) {
			throw InvalidArgument();
		}
		return ( WChar * )b.getVarPtr() + pIndex;
	}

	WChar operator [] (
		const	Uint32		pIndex
	) const {
		return *getCstPtr( pIndex );
	}

	WChar & operator [] (
		const	Uint32		pIndex
	) {
		return *getVarPtr( pIndex );
	}

	WBuffer operator () (
		const	Uint32		pOffset
	) const {
		WBuffer res;
		res.b = b( pOffset * 4 );
		return res;
	}

	WBuffer operator () (
		const	Uint32		pOffset,
		const	Uint32		pLength
	) const {
		WBuffer res;
		res.b = b( pOffset * 4, pLength * 4 );
		return res;
	}

	operator Buffer(
	) const {
		return saveASCII();
	}

	Bool contains(
		const	WChar		pCh
	) const {
		return ( findFwd( pCh ) != InvalidIndex );
	}

	WBuffer & operator += (
		const	WBuffer &	pOther
	) {
		b += pOther.b;
		return *this;
	}

	Uint32 findFwd(
		const	WChar		pWChar,
		const	Uint32		pIndex = 0
	) const;

	Uint32 findRev(
		const	WChar		pWChar,
		const	Uint32		pIndex = InvalidIndex
	) const;

//	WBuffer replaceTokens(
//		const	WBuffer &	pRepl1,
//		const	WBuffer &	pRepl2,
//		const	WBuffer &	pRepl3,
//		const	WBuffer &	pRepl4
//	);
//
//	static Int64 compareIString(
//		const	WBuffer &	str1,
//		const	WBuffer &	str2
//	);
//
//	static Int64 compareNIString(
//		const	WBuffer &	str1,
//		const	WBuffer &	str2,
//		const	Uint64		count
//	);

	static Int64 compareIStringASCII(
		const	WBuffer &	str1,
		const	WBuffer &	str2
	);

	static Int64 compareString(
		const	WBuffer &	str1,
		const	WBuffer &	str2
	);

//	static Int64 compareNString(
//		const	WBuffer &	str1,
//		const	WBuffer &	str2,
//		const	Uint64		count
//	);
//
//	static Bool equals(
//		const	WBuffer &	str1,
//		const	WBuffer &	str2
//	);
//
//	static Bool equalsN(
//		const	WBuffer &	str1,
//		const	WBuffer &	str2,
//		const	Uint64		count
//	);

//	static Bool regionMatches(
//		const	WBuffer &	str1,
//		const	Int64		offset1,
//		const	WBuffer &	str2,
//		const	Int64		offset2,
//		const	Uint64		charCount
//	);
//
//	static Bool regionIMatches(
//		const	WBuffer &	str1,
//		const	Int64		offset1,
//		const	WBuffer &	str2,
//		const	Int64		offset2,
//		const	Uint64		charCount
//	);
//
//	static Uint64 hash(
//		const	Buffer &	toHash,
//		const	Uint64		hashModulus
//	);
//
//	static Uint64 hash(
//		const	WBuffer &	toHash,
//		const	Uint64		hashModulus
//	);
//
//	static Uint64 hashN(
//		const	WBuffer &	toHash,
//		const	Uint64		numChars,
//		const	Uint64		hashModulus
//	);
//
//	static void moveChars(
//			WBuffer &	targetStr,
//		const	WBuffer &	srcStr,
//		const	Uint64		count
//	);
//
//	static void subString(
//			Buffer &	targetStr,
//		const	Buffer &	srcStr,
//		const	Uint64		startIndex,
//		const	Uint64		endIndex
//	);
//
//	static void subString(
//			WBuffer &	targetStr,
//		const	WBuffer &	srcStr,
//		const	Uint64		startIndex,
//		const	Uint64		endIndex
//	);
//
//	static void subString(
//			WBuffer &	targetStr,
//		const	WBuffer &	srcStr,
//		const	Uint64		startIndex,
//		const	Uint64		endIndex,
//		const	Uint64		srcStrLength
//	);
//
//	static Buffer replicate(
//		const	Buffer &	toRep
//	);
//
//	static WBuffer replicate(
//		const	WBuffer &	toRep
//	);
//
//	static Bool startsWith(
//		const	Buffer &	toTest,
//		const	Buffer &	prefix
//	);
//
//	static Bool startsWith(
//		const	WBuffer &	toTest,
//		const	WBuffer &	prefix
//	);
//
//	static Bool startsWithI(
//		const	Buffer &	toTest,
//		const	Buffer &	prefix
//	);
//
//	static Bool startsWithI(
//		const	WBuffer &	toTest,
//		const	WBuffer &	prefix
//	);
//
//	static Bool endsWith(
//		const	WBuffer &	toTest,
//		const	WBuffer &	suffix
//	);
//
//	static Uint64 findAny(
//		const	WBuffer &	toSearch,
//		const	WBuffer &	searchList
//	);
//
////	static WChar* findAny
////	(
////		WChar* const    toSearch,
////		const	WBuffer &	searchList
////	);
//
//	static int patternMatch(
//		const	WBuffer &	toSearch,
//		const	WBuffer &	pattern
//	);
//
//	static Bool isValidNOTATION(
//		const	WBuffer &	name
//	);
//
//	static Bool isValidEncName(
//		const	WBuffer &	name
//	);
//
//	static Bool isAlpha(
//		const	WChar		theChar
//	);
//
//	static Bool isDigit(
//		const	WChar		theChar
//	);
//
//	static Bool isAlphaNum(
//		const	WChar		theChar
//	);
//
//	static Bool isHex(
//		const	WChar		theChar
//	);
//
//	static Bool isInList(
//		const	WBuffer &	toFind,
//		const	WBuffer &	enumList
//	);
//
//	static void sizeToText(
//		const   Uint64           toFormat
//		,       char* const         toFill,
//		const	Uint64		    maxChars
//		, const unsigned int        radix
//	);
//
//	static void sizeToText
//	(
//		const   Uint64           toFormat
//		,       WChar* const        toFill,
//		const	Uint64		    maxChars
//		, const unsigned int        radix
//	);
//
//	static void binToText
//	(
//		const   unsigned int    toFormat
//		,       char* const     toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static void binToText
//	(
//		const   unsigned int    toFormat
//		,       WChar* const    toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static void binToText
//	(
//		const   unsigned long   toFormat
//		,       char* const     toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static void binToText
//	(
//		const   unsigned long   toFormat
//		,       WChar* const    toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static void binToText
//	(
//		const   int             toFormat
//		,       char* const     toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static void binToText
//	(
//		const   int             toFormat
//		,       WChar* const    toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static void binToText
//	(
//		const   long            toFormat
//		,       char* const     toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static void binToText
//	(
//		const   long            toFormat
//		,       WChar* const    toFill,
//		const	Uint64		maxChars
//		, const unsigned int    radix
//	);
//
//	static Bool textToBin
//	(
//		const	WBuffer &	toConvert
//		,       unsigned int&   toFill
//	);
//
//	static int parseInt
//	(
//		const	WBuffer &	toConvert
//	);
//
//	static void cut
//	(
//		WChar* const    toCutFrom,
//		const	Uint64		count
//	);
//
//	static void trim(char* const toTrim);
//
//	static void trim(WChar* const toTrim);
//
////	static BaseRefVectorOf<WChar>* tokenizeString(const	WBuffer &	tokenizeSrc);
//
//	static WChar* makeUName
//	(
//		const	WBuffer &	pszURI,
//		const	WBuffer &	pszName
//	);
//
//	static Uint64 replaceTokens
//	(
//		WChar* const    errText,
//		const	Uint64		maxChars,
//		const	WBuffer &	text1,
//		const	WBuffer &	text2,
//		const	WBuffer &	text3,
//		const	WBuffer &	text4
//	);
//
//	static void upperCase(WChar* const toUpperCase);
//
//	static void upperCaseASCII(WChar* const toUpperCase);
//
//	static void lowerCase(WChar* const toLowerCase);
//
//	static void lowerCaseASCII(WChar* const toLowerCase);
//
//	static Bool isWSReplaced(const	WBuffer &	toCheck);
//
//	static Bool isWSCollapsed(const	WBuffer &	toCheck);
//
//	static void replaceWS(WChar* toConvert);
//
//	static void collapseWS(WChar* toConvert);
//
//	static void removeWS(WChar* toConvert);
//
//
////	static void removeChar(const WChar*     const srcString
////						   , const WChar&           toRemove
////						   ,       XMLBuffer&       dstBuffer);
//
//	static void fixURI(const	WBuffer &	str, WChar* const target);

protected :

//	static Bool validateRegion(const	WBuffer &	str1, const int offset1,
//							   const	WBuffer &	str2, const int offset2,
//							   const Uint64 charCount);

	void loadASCII(
		const	Buffer &	pSystem
	);

	Buffer saveASCII(
	) const;

private :

	Buffer		b;

};

// ============================================================================

/// \brief Concatenation operator.
///
/// \ingroup BFC_Base

BFC_Light_DLL WBuffer operator + (
	const	WBuffer &	s1,
	const	WBuffer &	s2
);

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_WBuffer_H_

// ============================================================================

