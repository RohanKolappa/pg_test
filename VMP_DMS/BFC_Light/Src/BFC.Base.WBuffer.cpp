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
//	BFC.Base.WBuffer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.WBuffer.h"

#include "BFC.Math.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

const WBuffer & WBuffer::getEmpty() {

	static WBuffer i;

	return i;

}

// ============================================================================

WBuffer::WBuffer(
	const	Buffer &	pBuffer ) {

	loadASCII( pBuffer );

}

WBuffer::WBuffer(
	const	char *		pBuffer ) {

	loadASCII( Buffer( pBuffer ) );

}

WBuffer::WBuffer(
	const	WChar		pWChar ) {

	b = Buffer( ( const Uchar * )&pWChar, 4, true );

}

// ============================================================================

Uint32 WBuffer::findFwd(
	const	WChar		pWChar,
	const	Uint32		pIndex ) const {

	Uint32		len = getLength();

	if ( pIndex >= len ) {
		return InvalidIndex;
	}

	const WChar *	ptr = getCstPtr( pIndex );

	for ( Uint32 i = pIndex ; i < len ; i++ ) {
		if ( *ptr++ == pWChar ) {
			return i;
		}
	}

	return InvalidIndex;

}

Uint32 WBuffer::findRev(
	const	WChar		pWChar,
	const	Uint32		pIndex ) const {

	Uint32 idx = ( pIndex == InvalidIndex ? getLength() - 1 : pIndex );

	if ( idx >= getLength() ) {
		return InvalidIndex;
	}

	const WChar *	ptr = getCstPtr( idx );

	for ( Int32 i = idx ; i >= 0 ; i-- ) {
		if ( *ptr-- == pWChar ) {
			return i;
		}
	}

	return InvalidIndex;

}

//int WBuffer::findRev(const WChar pWChar,
//						   const WChar* const toSearch,
//						   const Uint32    toSearchLen) {
//	const WChar* srcPtr = toSearch+toSearchLen;
//	while (srcPtr >= toSearch)
//		if (pWChar == *srcPtr--)
//			return ( int )(srcPtr + 1 - toSearch);
//	return -1;
//}
//
//int WBuffer::findRev( const   WChar* const    toSearch
//							, const WChar           pWChar
//							, const Uint32       fromIndex
//							, MemoryManager* const  manager) {
//	const Uint32 len = stringLen(toSearch);
//	if (fromIndex >= len)
//		ThrowXMLwithMemMgr(ArrayIndexOutOfBoundsException, ExceptionCode::Str_StartIndexPastEnd, manager);
//
//	const WChar* srcPtr = toSearch+fromIndex;
//	while (srcPtr >= toSearch)
//		if (pWChar == *srcPtr--)
//			return ( int )(srcPtr + 1 - toSearch);
//	return -1;
//}

// ============================================================================

//WBuffer WBuffer::replaceTokens(
//	const	WBuffer &	errText,
//	const	WBuffer &	text1,
//	const	WBuffer &	text2,
//	const	WBuffer &	text3,
//	const	WBuffer &	text4 ) {
//
//	Buffer	res = errText.b;
//	Buffer	pt0 = WBuffer( "{0}" ).b;
//	Buffer	pt1 = WBuffer( "{1}" ).b;
//	Buffer	pt2 = WBuffer( "{2}" ).b;
//	Buffer	pt3 = WBuffer( "{3}" ).b;
//
//	res.replaceBuffer( pt0, text1.b, 0, true );
//	res.replaceBuffer( pt1, text2.b, 0, true );
//	res.replaceBuffer( pt2, text3.b, 0, true );
//	res.replaceBuffer( pt3, text4.b, 0, true );
//
//	WBuffer t;
//
//	t.b = res;
//
//	return t;
//
//}

// ============================================================================

//Int64 WBuffer::compareIString(
//	const	WBuffer &	str1,
//	const	WBuffer &	str2 ) {
//
//	return TransService::instance()->compareIString( str1, str2 );
//
//}
//
//Int64 WBuffer::compareNIString(
//	const	WBuffer &	str1,
//	const	WBuffer &	str2,
//	const	Uint32		maxChars ) {
//
//	return TransService::instance()->compareNIString( str1, str2, maxChars );
//
//}

Int64 WBuffer::compareIStringASCII(
	const	WBuffer &	str1,
	const	WBuffer &	str2 ) {

	const Uint64	ln1 = str1.getLength();
	const Uint64	ln2 = str2.getLength();

	if ( ! ln1 || ! ln2 ) {
		return ( ln1 - ln2 );
	}

	const WChar *	pt1 = str1.getCstPtr();
	const WChar *	pt2 = str2.getCstPtr();
	const Uint64	len = Math::getMin( ln1, ln2 );

	for ( Uint64 i = 0 ; i < len ; i++ ) {

		Uint32 ch1 = *pt1++;
		Uint32 ch2 = *pt2++;

		if ( ch1 >= chLatin_A && ch1 <= chLatin_Z ) {
			ch1 += chLatin_a - chLatin_A;
		}

		if ( ch2 >= chLatin_A && ch2 <= chLatin_Z ) {
			ch2 += chLatin_a - chLatin_A;
		}

		if ( ch1 != ch2 ) {
			return ( ( Int64 )ch1 - ( Int64 )ch2 );
		}

	}

	return ( ln1 - ln2 );

}

Int64 WBuffer::compareString(
	const	WBuffer &	str1,
	const	WBuffer &	str2 ) {

	const Uint64	ln1 = str1.getLength();
	const Uint64	ln2 = str2.getLength();

	if ( ! ln1 || ! ln2 ) {
		return ( ln1 - ln2 );
	}

	const WChar *	pt1 = str1.getCstPtr();
	const WChar *	pt2 = str2.getCstPtr();
	const Uint64	len = Math::getMin( ln1, ln2 );

	for ( Uint64 i = 0 ; i < len ; i++ ) {

		Uint32 ch1 = *pt1++;
		Uint32 ch2 = *pt2++;

		if ( ch1 != ch2 ) {
			return ( ( Int64 )ch1 - ( Int64 )ch2 );
		}

	}

	return ( ln1 - ln2 );

}

//Int64 WBuffer::compareNString(
//	const	WBuffer &	str1,
//	const	WBuffer &	str2,
//	const	Uint32		maxChars ) {
//
//	const Uint32	ln1 = Math::getMin( str1.getLength(), maxChars );
//	const Uint32	ln2 = Math::getMin( str2.getLength(), maxChars );
//
//	if ( ! ln1 || ! ln2 ) {
//		return ( ln1 - ln2 );
//	}
//
//	const WChar *	pt1 = str1.getCstPtr();
//	const WChar *	pt2 = str2.getCstPtr();
//	const Uint32	len = Math::getMin( ln1, ln2 );
//
//	for ( Uint32 curCount = 0 ; curCount < len ; curCount++ ) {
//
//		WChar ch1 = *pt1++;
//		WChar ch2 = *pt2++;
//
//		if ( ch1 != ch2 ) {
//			return ( ( Int64 )ch1 - ( Int64 )ch2 );
//		}
//
//	}
//
//	return ( ln1 - ln2 );
//
//}
//
//// ============================================================================
//
//Bool WBuffer::equals(
//	const	WBuffer &	str1,
//	const	WBuffer &	str2 ) {
//
//	const Uint32	ln1 = str1.getLength();
//	const Uint32	ln2 = str2.getLength();
//
//	if ( ! ln1 || ! ln2 ) {
//		return ( ! ln1 && ! ln2 );
//	}
//
//	if ( ln1 != ln2 ) {
//		return false;
//	}
//
//	const WChar *	pt1 = str1.getCstPtr();
//	const WChar *	pt2 = str2.getCstPtr();
//
//	if ( pt1 == pt2 ) {
//		return true;
//	}
//
//	for ( Uint32 i = 0 ; i < ln1 ; i++ ) {
//		if ( *pt1++ != *pt2++ ) {
//			return false;
//		}
//	}
//
//	return true;
//
//}
//
//Bool WBuffer::equalsN(
//	const	WBuffer &	str1,
//	const	WBuffer &	str2,
//	const	Uint32		count ) {
//
//	const Uint32	ln1 = Math::getMin( str1.getLength(), count );
//	const Uint32	ln2 = Math::getMin( str2.getLength(), count );
//
//	if ( ! ln1 || ! ln2 ) {
//		return ( ! ln1 && ! ln2 );
//	}
//
//	if ( ln1 != ln2 ) {
//		return false;
//	}
//
//	const WChar *	pt1 = str1.getCstPtr();
//	const WChar *	pt2 = str2.getCstPtr();
//
//	if ( pt1 == pt2 ) {
//		return true;
//	}
//
//	for ( Uint32 i = 0 ; i < ln1 ; i++ ) {
//		if ( *pt1++ != *pt2++ ) {
//			return false;
//		}
//	}
//
//	return true;
//
//}

// ============================================================================

//static WChar                gNullStr[] = {
//	chOpenCurly, chLatin_n, chLatin_u, chLatin_l, chLatin_l, chCloseCurly, chNull
//};
//
//void WBuffer::sizeToText(  const  Uint32           toFormat
//							 ,       char* const          toFill
//							 , const Uint32            maxChars
//							 , const unsigned int         radix
//							 , MemoryManager* const       manager) {
//	static const char digitList[16] = {
//		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
//		, 'A', 'B', 'C', 'D', 'E', 'F'
//	};
//
//	if (!maxChars)
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_ZeroSizedTargetBuf, manager);
//
//	// Handle special case
//	if (!toFormat) {
//		toFill[0] = '0';
//		toFill[1] = 0;
//		return;
//	}
//
//	// This is used to fill the temp buffer
//	Uint32 tmpIndex = 0;
//
//	// A copy of the conversion value that we can modify
//	Uint32 tmpVal = toFormat;
//
//	//
//	//  Convert into a temp buffer that we know is large enough. This avoids
//	//  having to check for overflow in the inner loops, and we have to flip
//	//  the resulting WBuffer anyway.
//	//
//	char   tmpBuf[128];
//
//	//
//	//  For each radix, do the optimal thing. For bin and hex, we can special
//	//  case them and do shift and mask oriented stuff. For oct and decimal
//	//  there isn't much to do but bull through it with divides.
//	//
//	if (radix == 2) {
//		while (tmpVal) {
//			if (tmpVal & 0x1UL)
//				tmpBuf[tmpIndex++] = '1';
//			else
//				tmpBuf[tmpIndex++] = '0';
//			tmpVal >>= 1;
//		}
//	} else if (radix == 16) {
//		while (tmpVal) {
//			const Uint32 charInd = (tmpVal & 0xFUL);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal >>= 4;
//		}
//	} else if ((radix == 8) || (radix == 10)) {
//		while (tmpVal) {
//			const Uint32 charInd = (tmpVal % radix);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal /= radix;
//		}
//	} else {
//		ThrowXMLwithMemMgr(RuntimeException, ExceptionCode::Str_UnknownRadix, manager);
//	}
//
//	// See if have enough room in the caller's buffer
//	if (tmpIndex > maxChars) {
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_TargetBufTooSmall, manager);
//	}
//
//	// Reverse the tmp buffer into the caller's buffer
//	Uint32 outIndex = 0;
//	for (; tmpIndex > 0; tmpIndex--)
//		toFill[outIndex++] = tmpBuf[tmpIndex-1];
//
//	// And cap off the caller's buffer
//	toFill[outIndex] = char(0);
//}
//
//void WBuffer::binToText(  const   unsigned long   toFormat
//							,       char* const     toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	static const char digitList[16] = {
//		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
//		, 'A', 'B', 'C', 'D', 'E', 'F'
//	};
//
//	if (!maxChars)
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_ZeroSizedTargetBuf, manager);
//
//	// Handle special case
//	if (!toFormat) {
//		toFill[0] = '0';
//		toFill[1] = 0;
//		return;
//	}
//
//	// This is used to fill the temp buffer
//	Uint32 tmpIndex = 0;
//
//	// A copy of the conversion value that we can modify
//	unsigned long tmpVal = toFormat;
//
//	//
//	//  Convert into a temp buffer that we know is large enough. This avoids
//	//  having to check for overflow in the inner loops, and we have to flip
//	//  the resulting WBuffer anyway.
//	//
//	char   tmpBuf[128];
//
//	//
//	//  For each radix, do the optimal thing. For bin and hex, we can special
//	//  case them and do shift and mask oriented stuff. For oct and decimal
//	//  there isn't much to do but bull through it with divides.
//	//
//	if (radix == 2) {
//		while (tmpVal) {
//			if (tmpVal & 0x1UL)
//				tmpBuf[tmpIndex++] = '1';
//			else
//				tmpBuf[tmpIndex++] = '0';
//			tmpVal >>= 1;
//		}
//	} else if (radix == 16) {
//		while (tmpVal) {
//			const unsigned long charInd = (tmpVal & 0xFUL);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal >>= 4;
//		}
//	} else if ((radix == 8) || (radix == 10)) {
//		while (tmpVal) {
//			const unsigned long charInd = (tmpVal % radix);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal /= radix;
//		}
//	} else {
//		ThrowXMLwithMemMgr(RuntimeException, ExceptionCode::Str_UnknownRadix, manager);
//	}
//
//	// See if have enough room in the caller's buffer
//	if (tmpIndex > maxChars) {
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_TargetBufTooSmall, manager);
//	}
//
//	// Reverse the tmp buffer into the caller's buffer
//	Uint32 outIndex = 0;
//	for (; tmpIndex > 0; tmpIndex--)
//		toFill[outIndex++] = tmpBuf[tmpIndex-1];
//
//	// And cap off the caller's buffer
//	toFill[outIndex] = char(0);
//}
//
//void WBuffer::binToText(  const   unsigned int    toFormat
//							,       char* const     toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	// Just call the unsigned long version
//	binToText((unsigned long)toFormat, toFill, maxChars, radix, manager);
//}
//
//void WBuffer::binToText(  const   long            toFormat
//							,       char* const     toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	//
//	//  If its negative, then put a negative sign into the output and flip
//	//  the sign of the local temp value.
//	//
//	Uint32 startInd = 0;
//	unsigned long actualVal;
//	if (toFormat < 0) {
//		toFill[0] = '-';
//		startInd++;
//		// Signed integers can represent one extra negative value
//		// compared to the positive values. If we simply do (v * -1)
//		// we will overflow on that extra value.
//		//
//		long v = toFormat;
//		v++;
//		actualVal = (unsigned long)(v * -1);
//		actualVal++;
//	} else {
//		actualVal = (unsigned long)(toFormat);
//	}
//
//	// And now call the unsigned long version
//	binToText(actualVal, &toFill[startInd], maxChars, radix, manager);
//}
//
//void WBuffer::binToText(  const   int             toFormat
//							,       char* const     toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	// Just call the long version
//	binToText((long)toFormat, toFill, maxChars, radix, manager);
//}
//
//void WBuffer::cut(        WChar* const    toCutFrom
//							, const Uint32       count) {
//
//	// If count is zero, then nothing to do
//	if (!count)
//		return;
//
//	WChar* targetPtr = toCutFrom;
//	WChar* srcPtr = toCutFrom + count;
//	while (*srcPtr)
//		*targetPtr++ = *srcPtr++;
//
//	// Cap it off at the new end
//	*targetPtr = 0;
//}
//
//
//
//
//
//
//
//
//void WBuffer::trim(char* const toTrim) {
//	const Uint32 len = strlen(toTrim);
//
//	Uint32 skip, scrape;
//	for (skip = 0; skip < len; skip++) {
//		if (! isspace(toTrim[skip]))
//			break;
//	}
//
//	for (scrape = len; scrape > skip; scrape--) {
//		if (! isspace(toTrim[scrape - 1] ))
//			break;
//	}
//
//	// Cap off at the scrap point
//	if (scrape != len)
//		toTrim[scrape] = 0;
//
//	if (skip) {
//		// Copy the chars down
//		Uint32 index = 0;
//		while (toTrim[skip])
//			toTrim[index++] = toTrim[skip++];
//
//		toTrim[index] = 0;
//	}
//}
//
//
//Bool WBuffer::isValidNOTATION(const WChar*         const name
//								,       MemoryManager* const manager ) {
//	//
//	//  NOTATATION: <URI>:<localPart>
//	//  where URI is optional
//	//        ':' and localPart must be present
//	//
//	Uint32 nameLen = name.getLength();
//	int colPos = WBuffer::findRev(name, chColon);
//
//	if ((colPos == 0)         ||      // no ':'
//			(colPos == (( int )nameLen) - 1)  )     // <URI>':'
//		return false;
//
//
//	// Examine localpart
//	if (!XML10::isValidNCName(&name[colPos+1], nameLen - colPos -1)) {
//		return false;
//	} else if (colPos == -1) {
//		return true;
//	} else {
//		// Examine URI
//		WChar* const temp =
//			(WChar*) manager->allocate((colPos + 1) * sizeof(WChar));
//		const ArrayJanitor<WChar> jan(temp, manager);
//
//		copyNString(temp, name, colPos);
//		temp[colPos] = 0;
//
//		try {
//			XMLUri  newURI(temp, manager); // no relative uri support here
//		} catch (const MalformedURLException&) {
//			return false;
//		}
//
//		return true;
//	}
//}
//
///**
//  * isValidEncName
//  *
//  * [80] EncName ::= [A-Za-z] ([A-Za-z0-9._] | '-')*
//  *
//  */
//Bool WBuffer::isValidEncName(const WChar* const name) {
//	if (name == 0 || *name == 0)
//		return false;
//
//	const WChar* tempName = name;
//	WChar firstChar = *tempName++;
//
//	if (!isAlpha(firstChar))
//		return false;
//
//	while(*tempName) {
//		if (( !isAlpha(*tempName))       &&
//				( !isDigit(*tempName))       &&
//				( *tempName != chPeriod)     &&
//				( *tempName != chUnderscore) &&
//				( *tempName != chDash)        )
//			return false;
//
//		tempName++;
//	}
//
//	return true;
//}
//
//Bool WBuffer::isAlpha(WChar const theChar) {
//	if ((( theChar >= chLatin_a ) && ( theChar <= chLatin_z )) ||
//			(( theChar >= chLatin_A ) && ( theChar <= chLatin_Z )) )
//		return true;
//
//	return false;
//}

//Bool WBuffer::isDigit(
//	const	WChar		theChar ) {
//
//	return ( ( theChar >= chDigit_0 ) && ( theChar <= chDigit_9 ) );
//
//}

//Bool WBuffer::isAlphaNum(WChar const theChar) {
//	return (isAlpha(theChar) || isDigit(theChar));
//}
//
//Bool WBuffer::isHex(WChar const theChar) {
//	return (isDigit(theChar) ||
//			(theChar >= chLatin_a && theChar <= chLatin_f) ||
//			(theChar >= chLatin_A && theChar <= chLatin_F));
//}
//
//// ---------------------------------------------------------------------------
////  Wide char versions of most of the string methods
//// ---------------------------------------------------------------------------
//
//void WBuffer::sizeToText(  const  Uint32            toFormat
//							 ,       WChar* const         toFill
//							 , const Uint32            maxChars
//							 , const unsigned int         radix
//							 , MemoryManager* const       manager) {
//	static const WChar digitList[16] = {
//		chDigit_0, chDigit_1, chDigit_2, chDigit_3, chDigit_4, chDigit_5
//		,   chDigit_6, chDigit_7, chDigit_8, chDigit_9, chLatin_A, chLatin_B
//		,   chLatin_C, chLatin_D, chLatin_e, chLatin_F
//	};
//
//	if (!maxChars)
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_ZeroSizedTargetBuf, manager);
//
//	// Handle special case
//	if (!toFormat) {
//		toFill[0] = chDigit_0;
//		toFill[1] = chNull;
//		return;
//	}
//
//	// This is used to fill the temp buffer
//	Uint32 tmpIndex = 0;
//
//	// A copy of the conversion value that we can modify
//	Uint32 tmpVal = toFormat;
//
//	//
//	//  Convert into a temp buffer that we know is large enough. This avoids
//	//  having to check for overflow in the inner loops, and we have to flip
//	//  the resulting sring anyway.
//	//
//	WChar   tmpBuf[128];
//
//	//
//	//  For each radix, do the optimal thing. For bin and hex, we can special
//	//  case them and do shift and mask oriented stuff. For oct and decimal
//	//  there isn't much to do but bull through it with divides.
//	//
//	if (radix == 2) {
//		while (tmpVal) {
//			if (tmpVal & 0x1UL)
//				tmpBuf[tmpIndex++] = chDigit_1;
//			else
//				tmpBuf[tmpIndex++] = chDigit_0;
//			tmpVal >>= 1;
//		}
//	} else if (radix == 16) {
//		while (tmpVal) {
//			const Uint32 charInd = (tmpVal & 0xFUL);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal >>= 4;
//		}
//	} else if ((radix == 8) || (radix == 10)) {
//		while (tmpVal) {
//			const Uint32 charInd = (tmpVal % radix);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal /= radix;
//		}
//	} else {
//		ThrowXMLwithMemMgr(RuntimeException, ExceptionCode::Str_UnknownRadix, manager);
//	}
//
//	// See if have enough room in the caller's buffer
//	if (tmpIndex > maxChars) {
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_TargetBufTooSmall, manager);
//	}
//
//	// Reverse the tmp buffer into the caller's buffer
//	Uint32 outIndex = 0;
//	for (; tmpIndex > 0; tmpIndex--)
//		toFill[outIndex++] = tmpBuf[tmpIndex-1];
//
//	// And cap off the caller's buffer
//	toFill[outIndex] = chNull;
//}
//
//void WBuffer::binToText(  const   unsigned long   toFormat
//							,       WChar* const    toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	static const WChar digitList[16] = {
//		chDigit_0, chDigit_1, chDigit_2, chDigit_3, chDigit_4, chDigit_5
//		,   chDigit_6, chDigit_7, chDigit_8, chDigit_9, chLatin_A, chLatin_B
//		,   chLatin_C, chLatin_D, chLatin_e, chLatin_F
//	};
//
//	if (!maxChars)
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_ZeroSizedTargetBuf, manager);
//
//	// Handle special case
//	if (!toFormat) {
//		toFill[0] = chDigit_0;
//		toFill[1] = chNull;
//		return;
//	}
//
//	// This is used to fill the temp buffer
//	Uint32 tmpIndex = 0;
//
//	// A copy of the conversion value that we can modify
//	unsigned long tmpVal = toFormat;
//
//	//
//	//  Convert into a temp buffer that we know is large enough. This avoids
//	//  having to check for overflow in the inner loops, and we have to flip
//	//  the resulting sring anyway.
//	//
//	WChar   tmpBuf[128];
//
//	//
//	//  For each radix, do the optimal thing. For bin and hex, we can special
//	//  case them and do shift and mask oriented stuff. For oct and decimal
//	//  there isn't much to do but bull through it with divides.
//	//
//	if (radix == 2) {
//		while (tmpVal) {
//			if (tmpVal & 0x1UL)
//				tmpBuf[tmpIndex++] = chDigit_1;
//			else
//				tmpBuf[tmpIndex++] = chDigit_0;
//			tmpVal >>= 1;
//		}
//	} else if (radix == 16) {
//		while (tmpVal) {
//			const unsigned long charInd = (tmpVal & 0xFUL);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal >>= 4;
//		}
//	} else if ((radix == 8) || (radix == 10)) {
//		while (tmpVal) {
//			const unsigned long charInd = (tmpVal % radix);
//			tmpBuf[tmpIndex++] = digitList[charInd];
//			tmpVal /= radix;
//		}
//	} else {
//		ThrowXMLwithMemMgr(RuntimeException, ExceptionCode::Str_UnknownRadix, manager);
//	}
//
//	// See if have enough room in the caller's buffer
//	if (tmpIndex > maxChars) {
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_TargetBufTooSmall, manager);
//	}
//
//	// Reverse the tmp buffer into the caller's buffer
//	Uint32 outIndex = 0;
//	for (; tmpIndex > 0; tmpIndex--)
//		toFill[outIndex++] = tmpBuf[tmpIndex-1];
//
//	// And cap off the caller's buffer
//	toFill[outIndex] = chNull;
//}
//
//void WBuffer::binToText(  const   unsigned int    toFormat
//							,       WChar* const    toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	// Just call the unsigned long version
//	binToText((unsigned long)toFormat, toFill, maxChars, radix, manager);
//}
//
//void WBuffer::binToText(  const   long            toFormat
//							,       WChar* const    toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	//
//	//  If its negative, then put a negative sign into the output and flip
//	//  the sign of the local temp value.
//	//
//	Uint32 startInd = 0;
//	unsigned long actualVal;
//	if (toFormat < 0) {
//		toFill[0] = chDash;
//		startInd++;
//		// Signed integers can represent one extra negative value
//		// compared to the positive values. If we simply do (v * -1)
//		// we will overflow on that extra value.
//		//
//		long v = toFormat;
//		v++;
//		actualVal = (unsigned long)(v * -1);
//		actualVal++;
//	} else {
//		actualVal = (unsigned long)(toFormat);
//	}
//
//	// And now call the unsigned long version
//	binToText(actualVal, &toFill[startInd], maxChars, radix, manager);
//}
//
//void WBuffer::binToText(  const   int             toFormat
//							,       WChar* const    toFill
//							, const Uint32       maxChars
//							, const unsigned int    radix
//							, MemoryManager* const  manager) {
//	// Just call the long version
//	binToText((long)toFormat, toFill, maxChars, radix, manager);
//}
//
//
//
//WChar*
//WBuffer::makeUName(const WChar* const pszURI, const WChar* const pszName) {
//	//
//	//  If there is a URI, then format out the full name in the {uri}name
//	//  form. Otherwise, just set it to the same thing as the base name.
//	//
//	WChar* pszRet = 0;
//	const Uint32 uriLen = stringLen(pszURI);
//	if (uriLen) {
//		pszRet = new WChar[uriLen + stringLen(pszName) + 3];
//
//		WChar szTmp[2];
//		szTmp[1] = 0;
//
//		szTmp[0] = chOpenCurly;
//		copyString(pszRet, szTmp);
//		catString(pszRet, pszURI);
//		szTmp[0] = chCloseCurly;
//		catString(pszRet, szTmp);
//		catString(pszRet, pszName);
//	} else {
//		pszRet = replicate(pszName);
//	}
//	return pszRet;
//}
//
//
//Bool WBuffer::textToBin(const WChar* const toConvert, unsigned int& toFill
//						  , MemoryManager* const manager) {
//	toFill = 0;
//
//	// If no string, then its a failure
//	if ((!toConvert) || (!*toConvert))
//		return false;
//
//	WChar* trimmedStr = WBuffer::replicate(toConvert, manager);
//	ArrayJanitor<WChar> jan1(trimmedStr, manager);
//	WBuffer::trim(trimmedStr);
//	Uint32 trimmedStrLen = trimmedStr.getLength();
//
//	if ( !trimmedStrLen )
//		return false;
//
//	// we don't allow '-' sign
//	if (WBuffer::findFwd(trimmedStr, chDash, 0, manager) != -1)
//		return false;
//
//	//the errno set by previous run is NOT automatically cleared
//	errno = 0;
//
//	char *nptr = WBuffer::transcode(trimmedStr, manager);
//	ArrayJanitor<char> jan2(nptr, manager);
//
//	char *endptr;
//	//
//	// REVISIT: conversion of (unsigned long) to (unsigned int)
//	//          may truncate value on IA64
//	toFill = (unsigned int) strtoul(nptr, &endptr, 10);
//
//	// check if all chars are valid char
//	// check if overflow/underflow occurs
//	if ( ( (endptr - nptr) != ( int ) trimmedStrLen) ||
//			(errno == ERANGE)                      )
//		return false;
//
//	return true;
//}
//
//int WBuffer::parseInt(const WChar* const toConvert
//						, MemoryManager* const manager) {
//	// If no string, or empty string, then it is a failure
//	if ((!toConvert) || (!*toConvert))
//		ThrowXMLwithMemMgr(NumberFormatException, ExceptionCode::XMLNUM_null_ptr, manager);
//
//	WChar* trimmedStr = WBuffer::replicate(toConvert, manager);
//	ArrayJanitor<WChar> jan1(trimmedStr, manager);
//	WBuffer::trim(trimmedStr);
//	Uint32 trimmedStrLen = trimmedStr.getLength();
//
//	if ( !trimmedStrLen )
//		ThrowXMLwithMemMgr(NumberFormatException, ExceptionCode::XMLNUM_null_ptr, manager);
//
//	//the errno set by previous run is NOT automatically cleared
//	errno = 0;
//
//	char *nptr = WBuffer::transcode(trimmedStr, manager);
//	ArrayJanitor<char> jan2(nptr, manager);
//
//	char *endptr;
//	long retVal = strtol(nptr, &endptr, 10);
//
//	// check if all chars are valid char
//	if ( (endptr - nptr) != ( int ) trimmedStrLen)
//		ThrowXMLwithMemMgr(NumberFormatException, ExceptionCode::XMLNUM_Inv_chars, manager);
//
//	// check if overflow/underflow occurs
//	if (errno == ERANGE)
//		ThrowXMLwithMemMgr(NumberFormatException, ExceptionCode::Str_ConvertOverflow, manager);
//
//	//
//	// REVISIT: conversion of (long) to ( int )
//	//          may truncate value on IA64
//	return ( int ) retVal;
//}
//
//
//void WBuffer::trim(WChar* const toTrim) {
//	const Uint32 len = stringLen(toTrim);
//
//	Uint32 skip, scrape;
//	for (skip = 0; skip < len; skip++) {
//		if (!XML10::isWhitespace(toTrim[skip]))
//			break;
//	}
//
//	for (scrape = len; scrape > skip; scrape--) {
//		if (!XML10::isWhitespace(toTrim[scrape - 1]))
//			break;
//	}
//
//	// Cap off at the scrap point
//	if (scrape != len)
//		toTrim[scrape] = 0;
//
//	if (skip) {
//		// Copy the chars down
//		Uint32 index = 0;
//		while (toTrim[skip])
//			toTrim[index++] = toTrim[skip++];
//
//		toTrim[index] = 0;
//	}
//}
//
//
//void WBuffer::upperCase(WChar* const toUpperCase) {
//	// Refer this one to the transcoding service
//	TransService::instance()->upperCase(toUpperCase);
//}
//
//void WBuffer::upperCaseASCII(WChar* const toUpperCase) {
//	WChar* pt1 = toUpperCase;
//
//	if (!pt1)
//		return;
//
//	while (*pt1) {
//		if (*pt1 >= chLatin_a && *pt1 <= chLatin_z)
//			*pt1 = *pt1 - chLatin_a + chLatin_A;
//
//		pt1++;
//	}
//}
//
//
//void WBuffer::lowerCase(WChar* const toLowerCase) {
//	// Refer this one to the transcoding service
//	TransService::instance()->lowerCase(toLowerCase);
//}
//
//void WBuffer::lowerCaseASCII(WChar* const toLowerCase) {
//	WChar* pt1 = toLowerCase;
//
//	if (!pt1)
//		return;
//
//	while (*pt1) {
//		if (*pt1 >= chLatin_A && *pt1 <= chLatin_Z)
//			*pt1 = *pt1 - chLatin_A + chLatin_a;
//
//		pt1++;
//	}
//}
//
//BaseRefVectorOf<WChar>* WBuffer::tokenizeString(const WChar*      const   tokenizeSrc
//		,  MemoryManager*    const   manager) {
//	WChar* orgText = replicate(tokenizeSrc, manager);
//	ArrayJanitor<WChar> janText(orgText, manager);
//	WChar* tokenizeStr = orgText;
//
//	RefArrayVectorOf<WChar>* tokenStack = new (manager) RefArrayVectorOf<WChar>(16, true, manager);
//
//	Uint32 len = stringLen(tokenizeStr);
//	Uint32 skip;
//	Uint32 index = 0;
//
//	while (index != len) {
//		// find the first non-space character
//		for (skip = index; skip < len; skip++) {
//			if (!XML10::isWhitespace(tokenizeStr[skip]))
//				break;
//		}
//		index = skip;
//
//		// find the delimiter (space character)
//		for (; skip < len; skip++) {
//			if (XML10::isWhitespace(tokenizeStr[skip]))
//				break;
//		}
//
//		// we reached the end of the string
//		if (skip == index)
//			break;
//
//		// these tokens are adopted in the RefVector and will be deleted
//		// when the vector is deleted by the caller
//		WChar* token = (WChar*) manager->allocate
//					   (
//						   (skip+1-index) * sizeof(WChar)
//					   );//new WChar[skip+1-index];
//
//		WBuffer::subString(token, tokenizeStr, index, skip, len, manager);
//		tokenStack->addElement(token);
//		index = skip;
//	}
//	return tokenStack;
//}
//
////
////  This method is called when we get a notation or enumeration type attribute
////  to validate. We have to confirm that the passed value to find is one of
////  the values in the passed list. The list is a space separated string of
////  values to match against.
////
//Bool WBuffer::isInList(const WChar* const toFind, const WChar* const enumList) {
//	//
//	//  We loop through the values in the list via this outer loop. We end
//	//  when we hit the end of the enum list or get a match.
//	//
//	const WChar* listPtr = enumList;
//	const Uint32 findLen = toFind.getLength();
//	while (*listPtr) {
//		Uint32 testInd;
//		for (testInd = 0; testInd < findLen; testInd++) {
//			//
//			//  If they don't match, then reset and try again. Note that
//			//  hitting the end of the current item will cause a mismatch
//			//  because there can be no spaces in the toFind string.
//			//
//			if (listPtr[testInd] != toFind[testInd])
//				break;
//		}
//
//		//
//		//  If we went the distance, see if we matched. If we did, the current
//		//  list character has to be null or space.
//		//
//		if (testInd == findLen) {
//			if ((listPtr[testInd] == chSpace) || !listPtr[testInd])
//				return true;
//		}
//
//		// Run the list pointer up to the next substring
//		while ((*listPtr != chSpace) && *listPtr)
//			listPtr++;
//
//		// If we hit the end, then we failed
//		if (!*listPtr)
//			return false;
//
//		// Else move past the space and try again
//		listPtr++;
//	}
//
//	// We never found it
//	return false;
//}
//
////
//// a string is whitespace:replaced, is having no
////    #xD  Carriage Return
////    #xA  Line Feed
////    #x9  TAB
////
//Bool WBuffer::isWSReplaced(const WChar* const toCheck) {
//	// If no string, then its a OK
//	if (( !toCheck ) || ( !*toCheck ))
//		return true;
//
//	const WChar* startPtr = toCheck;
//	while ( *startPtr ) {
//		if ( ( *startPtr == chCR) ||
//				( *startPtr == chLF) ||
//				( *startPtr == chHTab))
//			return false;
//
//		startPtr++;
//	}
//
//	return true;
//}
//
////
////   to replace characters listed below to #x20
////    #xD  Carriage Return
////    #xA  Line Feed
////    #x9  TAB
////
//void WBuffer::replaceWS(WChar* toConvert, MemoryManager* const) {
//	// If no string, then its a OK
//	if (( !toConvert ) || ( !*toConvert ))
//		return;
//
//	WChar* cursorPtr = toConvert;
//	while ( *cursorPtr ) {
//		if ( ( *cursorPtr == chCR) ||
//				( *cursorPtr == chLF) ||
//				( *cursorPtr == chHTab))
//			*cursorPtr = chSpace;
//
//		cursorPtr++;
//	}
//}
//
////
//// a string is whitespace:collapsed, is whitespace::replaced
//// and no
////    leading space (#x20)
////    trailing space
////    no contiguous sequences of spaces
////
//Bool WBuffer::isWSCollapsed(const WChar* const toCheck) {
//	if (( !toCheck ) || ( !*toCheck ))
//		return true;
//
//	// shall be whitespace::replaced first
//	if ( !isWSReplaced(toCheck) )
//		return false;
//
//	// no leading or trailing space
//	if ((*toCheck == chSpace) ||
//			(toCheck[toCheck.getLength()-1] == chSpace))
//		return false;
//
//	const WChar* startPtr = toCheck;
//	WChar theChar;
//	Bool  inSpace = false;
//	while ( (theChar = *startPtr) != 0 ) {
//		if ( theChar == chSpace) {
//			if (inSpace)
//				return false;
//			else
//				inSpace = true;
//		} else
//			inSpace = false;
//
//		startPtr++;
//
//	}
//
//	return true;
//}
//
////
//// no leading and/or trailing spaces
//// no continuous sequences of spaces
////
//void WBuffer::collapseWS(WChar* toConvert
//						   , MemoryManager* const  manager) {
//	// If no string, then its a failure
//	if (( !toConvert ) || ( !*toConvert ))
//		return;
//
//	// replace whitespace first
//	if(!isWSReplaced(toConvert))
//		replaceWS(toConvert, manager);
//
//	// remove leading spaces
//	WChar* startPtr = toConvert;
//	while ( *startPtr == chSpace )
//		startPtr++;
//
//	if (!*startPtr) {
//		*toConvert = chNull;
//		return;
//	}
//
//	// remove trailing spaces
//	WChar* endPtr = toConvert + stringLen(toConvert);
//	while (*(endPtr - 1) == chSpace)
//		endPtr--;
//	*endPtr = chNull;
//
//	// move data to beginning only if there were spaces in front
//	if(startPtr != toConvert)
//		WBuffer::moveChars(toConvert, startPtr, endPtr - startPtr + 1);
//
//	if(!isWSCollapsed(toConvert)) {
//		//
//		//  Work through what remains and chop continuous spaces
//		//
//		WChar* retPtr = toConvert;
//		startPtr = toConvert;
//		Bool inSpace = false;
//		while (*startPtr) {
//			if ( *startPtr == chSpace) {
//				// copy a single space, then ignore subsequent
//				if (!inSpace) {
//					inSpace = true;
//					*retPtr++ = chSpace;
//				}
//			} else {
//				inSpace = false;
//				*retPtr++ = *startPtr;
//			}
//
//			startPtr++;
//		}
//
//		*retPtr = chNull;
//	}
//}
//
////
//// remove whitespace
////
//void WBuffer::removeWS(WChar* toConvert, MemoryManager* const) {
//	// If no string, then its a failure
//	if (( !toConvert ) || ( !*toConvert ))
//		return;
//
//	WChar* retPtr = toConvert;
//	WChar* startPtr = toConvert;
//
//	while (*startPtr) {
//		if ( ( *startPtr != chCR)    &&
//				( *startPtr != chLF)    &&
//				( *startPtr != chHTab)  &&
//				( *startPtr != chSpace)  ) {
//			*retPtr++ = *startPtr;
//		}
//
//		startPtr++;
//	}
//
//	*retPtr = chNull;
//}
//
//void WBuffer::removeChar(const WChar*     const srcString
//						   , const WChar&           toRemove
//						   ,       XMLBuffer&       dstBuffer) {
//	if(!srcString) return;
//	const WChar* iPtr = srcString;
//	WChar c;
//
//	dstBuffer.reset();
//
//	while ((c=*iPtr++)!=0) {
//		if (c != toRemove)
//			dstBuffer.append(c);
//	}
//}
//
///**
// * Fixes a platform dependent absolute path filename to standard URI form.
// * 1. Windows: fix 'x:' to 'file:///x:' and convert any backslash to forward slash
// * 2. UNIX: fix '/blah/blahblah' to 'file:///blah/blahblah'
// */
//void WBuffer::fixURI(const WChar* const str, WChar* const target) {
//	if (!str || !*str)
//		return;
//
//	int colonIdx = WBuffer::findFwd(str, chColon);
//
//	// If starts with a '/' we assume
//	// this is an absolute (UNIX) file path and prefix it with file://
//	if (colonIdx == -1 && WBuffer::findFwd(str, chForwardSlash) == 0) {
//		unsigned index = 0;
//		target[index++] = chLatin_f;
//		target[index++] = chLatin_i;
//		target[index++] = chLatin_l;
//		target[index++] = chLatin_e;
//		target[index++] = chColon;
//		target[index++] = chForwardSlash;
//		target[index++] = chForwardSlash;
//
//		// copy the string
//		const WChar* inPtr = str;
//		while (*inPtr)
//			target[index++] = *inPtr++;
//
//		target[index] = chNull;
//	} else if (colonIdx == 1 && WBuffer::isAlpha(*str)) {
//		// If starts with a driver letter 'x:' we assume
//		// this is an absolute (Windows) file path and prefix it with file:///
//		unsigned index = 0;
//		target[index++] = chLatin_f;
//		target[index++] = chLatin_i;
//		target[index++] = chLatin_l;
//		target[index++] = chLatin_e;
//		target[index++] = chColon;
//		target[index++] = chForwardSlash;
//		target[index++] = chForwardSlash;
//		target[index++] = chForwardSlash;
//
//		// copy the string and fix any backward slash
//		const WChar* inPtr = str;
//		while (*inPtr) {
//			if (*inPtr == chYenSign ||
//					*inPtr == chWonSign ||
//					*inPtr == chBackSlash)
//				target[index++] = chForwardSlash;
//			else
//				target[index++] = *inPtr;
//			inPtr++;
//		}
//
//		// cap it with null
//		target[index] = chNull;
//	} else {
//		// not specific case, so just copy the string over
//		copyString(target, str);
//	}
//}

// ---------------------------------------------------------------------------
//  WBuffer: Private static methods
// ---------------------------------------------------------------------------

// ============================================================================

// ============================================================================

// ============================================================================

//Bool WBuffer::regionMatches(const   WChar* const	str1
//							  , const int			offset1
//							  , const WChar* const	str2
//							  , const int			offset2
//							  , const Uint32     charCount) {
//	if (!validateRegion(str1, offset1,str2, offset2, charCount))
//		return false;
//
//	if (compareNString(str1+offset1, str2+offset2, charCount) != 0)
//		return false;
//
//	return true;
//}
//
//Bool WBuffer::regionIMatches(const   WChar* const	str1
//							   , const int			offset1
//							   , const WChar* const	str2
//							   , const int			offset2
//							   , const Uint32    charCount) {
//	if (!validateRegion(str1, offset1,str2, offset2, charCount))
//		return false;
//
//	if (compareNIString(str1+offset1, str2+offset2, charCount) != 0)
//		return false;
//
//	return true;
//}

// ============================================================================

//Uint32 WBuffer::hash(   const   char* const    tohash
//							 , const Uint32       hashModulus) {
//	Uint32 hashVal = 0;
//	if (tohash) {
//		const char* curCh = tohash;
//		while (*curCh) {
//			Uint32 top = hashVal >> 24;
//			hashVal += (hashVal * 37) + top + (Uint32)(*curCh);
//			curCh++;
//		}
//	}
//
//	// Divide by modulus
//	return hashVal % hashModulus;
//}

//inline Uint32 WBuffer::hash(const   WChar* const   tohash
//								 , const Uint32          hashModulus) {
//	if (tohash == 0 || *tohash == 0)
//		return 0;
//
//	const WChar* curCh = tohash;
//	Uint32 hashVal = (Uint32)(*curCh++);
//
//	while (*curCh)
//		hashVal = (hashVal * 38) + (hashVal >> 24) + (Uint32)(*curCh++);
//
//	// Divide by modulus
//	return hashVal % hashModulus;
//}
//
//inline Uint32 WBuffer::hashN(const   WChar* const   tohash
//								  , const Uint32       n
//								  , const Uint32       hashModulus) {
//	if (tohash == 0 || n == 0)
//		return 0;
//
//	const WChar* curCh = tohash;
//	Uint32 hashVal = (Uint32)(*curCh++);
//
//	for(Uint32 i=0; i<n; i++)
//		hashVal = (hashVal * 38) + (hashVal >> 24) + (Uint32)(*curCh++);
//
//	// Divide by modulus
//	return hashVal % hashModulus;
//}

// ============================================================================

// ============================================================================

//inline void WBuffer::moveChars(       WChar* const targetStr
//										, const	WBuffer &	srcStr
//										, const Uint32    count) {
//	memcpy(targetStr, srcStr, count * sizeof(WChar));
//}

// ============================================================================

//void WBuffer::subString(char* const targetStr, const char* const srcStr
//						  , const Uint32 startIndex, const Uint32 endIndex
//						  , MemoryManager* const manager) {
//	if (targetStr == 0)
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_ZeroSizedTargetBuf, manager);
//
//	const Uint32 srcLen = strlen(srcStr);
//
//	// Make sure the start index is within the WBuffer bounds
//	if (startIndex > endIndex || endIndex > srcLen)
//		ThrowXMLwithMemMgr(ArrayIndexOutOfBoundsException, ExceptionCode::Str_StartIndexPastEnd, manager);
//
//	const Uint32 copySize = endIndex - startIndex;
//
//	for (Uint32 i= startIndex; i < endIndex; i++) {
//		targetStr[i-startIndex] = srcStr[i];
//	}
//
//	targetStr[copySize] = 0;
//}
//
//void WBuffer::subString(WChar* const targetStr, const WChar* const srcStr
//						  , const Uint32 startIndex, const Uint32 endIndex
//						  , MemoryManager* const manager) {
//	subString(targetStr, srcStr, startIndex, endIndex, stringLen(srcStr), manager);
//}
//
//void WBuffer::subString(WChar* const targetStr, const WChar* const srcStr
//						  , const Uint32 startIndex, const Uint32 endIndex
//						  , const Uint32 srcStrLength
//						  , MemoryManager* const manager) {
//	//if (startIndex < 0 || endIndex < 0)
//	//    ThrowXMLwithMemMgr(ArrayIndexOutOfBoundsException, ExceptionCode::Str_NegativeIndex);
//
//	if (targetStr == 0)
//		ThrowXMLwithMemMgr(IllegalArgumentException, ExceptionCode::Str_ZeroSizedTargetBuf, manager);
//
//	// Make sure the start index is within the WBuffer bounds
//	if (startIndex > endIndex || endIndex > srcStrLength)
//		ThrowXMLwithMemMgr(ArrayIndexOutOfBoundsException, ExceptionCode::Str_StartIndexPastEnd, manager);
//
//	const Uint32 copySize = endIndex - startIndex;
//
//	for (Uint32 i= startIndex; i < endIndex; i++) {
//		targetStr[i-startIndex] = srcStr[i];
//	}
//
//	targetStr[copySize] = 0;
//}

// ============================================================================

//char* WBuffer::replicate( const char* const    toRep
//							, MemoryManager* const manager) {
//	// If a null string, return a null string
//	if (!toRep)
//		return 0;
//
//	//
//	//  Get the len of the source and allocate a new buffer. Make sure to
//	//  account for the nul terminator.
//	//
//	const Uint32 srcLen = strlen(toRep);
//	char* ret = (char*) manager->allocate((srcLen+1) * sizeof(char)); //new char[srcLen+1];
//
//	// Copy over the text, adjusting for the size of a char
//	memcpy(ret, toRep, (srcLen+1) * sizeof(char));
//	return ret;
//}
//
//inline WChar* WBuffer::replicate(const	WBuffer &	toRep,
//								   MemoryManager* const manager) {
//	// If a null string, return a null string!
//	WChar* ret = 0;
//	if (toRep) {
//		const Uint32 len = stringLen(toRep);
//		ret = (WChar*) manager->allocate((len+1) * sizeof(WChar)); //new WChar[len + 1];
//		memcpy(ret, toRep, (len + 1) * sizeof(WChar));
//	}
//	return ret;
//}

// ============================================================================

//Bool WBuffer::startsWith(const char* const toTest, const char* const prefix) {
//	return (strncmp(toTest, prefix, strlen(prefix)) == 0);
//}
//
//
//Bool WBuffer::startsWithI(const   char* const toTest
//							, const char* const prefix) {
//	return (strnicmp(toTest, prefix, strlen(prefix)) == 0);
//}
//
//inline Bool WBuffer::startsWith(  const	WBuffer &	toTest
//									, const	WBuffer &	   prefix) {
//	return (compareNString(toTest, prefix, stringLen(prefix)) == 0);
//}
//
//inline Bool WBuffer::startsWithI( const	WBuffer &	toTest
//									, const	WBuffer &	   prefix) {
//	return (compareNIString(toTest, prefix, stringLen(prefix)) == 0);
//}
//
//inline Bool WBuffer::endsWith(const	WBuffer &	toTest,
//								const	WBuffer &	suffix) {
//
//	Uint32 suffixLen = suffix.getLength();
//
//	return regionMatches(toTest, ( int )(toTest.getLength() - suffixLen),
//						 suffix, 0, suffixLen);
//}

// ============================================================================

//const WChar* WBuffer::findAny(const   WChar* const    toSearch
//								, const WChar* const    searchList) {
//	const WChar* srcPtr = toSearch;
//	while (*srcPtr) {
//		const WChar* listPtr = searchList;
//		const WChar  curCh = *srcPtr;
//
//		while (*listPtr) {
//			if (curCh == *listPtr++)
//				return srcPtr;
//		}
//		srcPtr++;
//	}
//	return 0;
//}
//
//WChar* WBuffer::findAny(          WChar* const    toSearch
//									, const WChar* const    searchList) {
//	WChar* srcPtr = toSearch;
//	while (*srcPtr) {
//		const WChar* listPtr = searchList;
//		const WChar  curCh = *srcPtr;
//
//		while (*listPtr) {
//			if (curCh == *listPtr++)
//				return srcPtr;
//		}
//		srcPtr++;
//	}
//	return 0;
//}

// ============================================================================

//int WBuffer::patternMatch(  const WChar* const    toSearch
//							  , const WChar* const    pattern) {
//	if (!toSearch || !*toSearch )
//		return -1;
//
//	const Uint32 patnLen = pattern.getLength();
//	if ( !patnLen )
//		return -1;
//
//	const WChar* srcPtr    = toSearch;
//	const WChar* patnStart = toSearch;
//	Uint32 patnIndex = 0;
//
//	while (*srcPtr) {
//		if ( !(*srcPtr++ == pattern[patnIndex])) {
//			patnIndex = 0;
//			srcPtr = ++patnStart;
//		} else {
//			if (++patnIndex == patnLen)
//				// full pattern match found
//				return ( int )(srcPtr - patnLen - toSearch);
//
//		}
//	}
//
//	return -1;
//}

// ============================================================================

//inline Bool WBuffer::validateRegion(const	WBuffer &	str1,
//									  const int offset1,
//									  const	WBuffer &	str2,
//									  const int offset2,
//									  const Uint32 charCount) {
//
//	if (offset1 < 0 || offset2 < 0 ||
//			(offset1 + charCount) > str1.getLength() ||
//			(offset2 + charCount) > str2.getLength() )
//		return false;
//
//	return true;
//}

// ============================================================================

void WBuffer::loadASCII(
	const	Buffer &	pSystem ) {

	if ( pSystem.isEmpty() ) {
		kill();
		return;
	}

	Uint32		len = pSystem.getLength();

	b.resize( len * 4 );

	const Uchar *	src = pSystem.getCstPtr();
	WChar *		dst = getVarPtr();

	for ( Uint32 i = len ; i ; i-- ) {
		*dst++ = ( WChar )*src++;
	}

}

Buffer WBuffer::saveASCII() const {

	if ( isEmpty() ) {
		return Buffer();
	}

	Buffer res;

	Uint32		len = getLength();

	res.resize( len );

	const WChar *	src = getCstPtr();
	Uchar *		dst = res.getVarPtr();

	for ( Uint32 i = len ; i ; i-- ) {
		*dst++ = ( Uchar )*src++;
	}

	return res;

}

// ============================================================================

WBuffer BFC::operator + (
	const	WBuffer &	s1,
	const	WBuffer &	s2 ) {

	WBuffer res = s1;
	res += s2;
	return res;

}

// ============================================================================

