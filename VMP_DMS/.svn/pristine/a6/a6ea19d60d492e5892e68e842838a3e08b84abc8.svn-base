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
//	BFC.Base.Time.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================


#ifndef _MY_TIME_H
#define _MY_TIME_H


// ============================================================================


#include "BFC.Base.Types.h"

#include "BFC.TL.Array.h"
#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"


// ============================================================================

namespace BFC {

// ============================================================================

/// Records a timestamp in year, month, ... second.

///
/// \ingroup Common_Base

class Time {

public :

	Time(
		const int ye = 0,
		const int mo = 0,
		const int da = 0,
		const int ho = 0,
		const int mi = 0,
		const int se = 0
	);

	Time(
		const Time& src
	);

	Time(
		const Buffer& src
	);

	~Time();

	Time& operator = (const Time& src);

	bool operator == (const Time& src) const;
	bool operator != (const Time& src) const;
	bool operator <  (const Time& src) const;
	bool operator >  (const Time& src) const;
	bool operator <= (const Time& src) const;
	bool operator >= (const Time& src) const;

#if 0
	class Diff {
	public :
		Diff(Y=0,M=0,D=0,h=0,m=0,s=0);
		Diff(const Diff&);
		~Diff();
		Diff& operator = (const Diff&);
		static Time one_year  () {return Diff(1);}
		static Time one_month () {return Diff(0,1);}
		static Time one_day   () {return Diff(0,0,1);}
		static Time one_hour  () {return Diff(0,0,0,1);}
		static Time one_minute() {return Diff(0,0,0,0,1);}
		static Time one_second() {return Diff(0,0,0,0,0,1);}
		Diff& operator += (const Diff&);
		Diff  operator +  (const Diff&) const;
		Diff& operator -= (const Diff&);
		Diff  operator -  (const Diff&) const;
		Diff& operator *= (...
	protected :
	private :
	};
#endif

	Time& operator += (const Time& src);
	Time  operator +  (const Time& src) const;
	Time& operator -= (const Time& src);
	Time  operator -  (const Time& src) const;

	//
	//	bool buildFrom(const Buffer& src, const Buffer& pattern);
	//
	//	Tries to build a Time object from `src', which must match
	//	`pattern', which in turn may contain the followin chars:
	//	- `s' : seconds -    0..59
	//	- `m' : minutes -    0..59
	//	- `h' : hours   -    0..23
	//	- `D' : day     -    1..31
	//	- `M' : month   -    1..12
	//	- `Y' : year    - 1900..???
	//	Exemples::
	//	- "2010-12-12"		matches "Y-M-D"
	//	- "2010/12/12/0/0/0"	matches "Y/M/D/h/m/s"
	//	- "01-12-1910   12.00"	matches "D-M-Y   h.m"
	//	Note: all fields not present in the pattern are set to 0.
	//

	void buildFrom(
		const Buffer& src,
		const Buffer& pattern
	);

	//
	//	bool buildFromBuffer(const Buffer& src);
	//
	//	`src' must be formatted as the output of the `longOutput()'
	//	and `shortOutput()' is.
	//

	void buildFromBuffer(
		const Buffer& src
	);

	void buildFromSecondsSinceEpoch(
		const Uint32 seconds
	);

	Time& setYear(const int new_year);	// 1900...
	Time& setMonth(const int new_month);	// 0..11
	Time& setDay(const int new_day);	// 0..30
	Time& setHour(const int new_hour);	// 0..23
	Time& setMinute(const int new_minute);// 0..59
	Time& setSecond(const int new_second);// 0..59

	int getYear() const;
	int getMonth() const;
	int getDay() const;
	int getHour() const;
	int getMinute() const;
	int getSecond() const;

	Uint32 getSecondsSinceEpoch() const;
	Uint32 getDaysSinceJanuary1() const;  // 0...365

	//
	//	Buffer shortOutputToBuffer() const;
	//
	//	Returns a buffer matching "YYYY-MM-DD".
	//

	Buffer shortOutputToBuffer() const;

	Buffer hourToBuffer() const;

	//
	//	Buffer longOutputToBuffer() const;
	//
	//	Returns a buffer matching "YYYY-MM-DD_hh:mm:ss".
	//

	Buffer longOutputToBuffer() const;

	static Time now();

	static Time one_year  (const int i = 1) {return Time(i);}
	static Time one_month (const int i = 1) {return Time(0,i);}
	static Time one_day   (const int i = 1) {return Time(0,0,i);}
	static Time one_hour  (const int i = 1) {return Time(0,0,0,i);}
	static Time one_minute(const int i = 1) {return Time(0,0,0,0,i);}
	static Time one_second(const int i = 1) {return Time(0,0,0,0,0,i);}

	static Time endOfTimes() {return Time(2010);}

	static int numberOfDaysInMonth(	// ---> 1..31
		const	int	month,		// 0..11
		const	int	year		// 1900..
	);

	static int numberOfDaysInYear(	// ---> 365..366
		const	int	year		// 1900..
	);

	BASE_CLASS_GEN_EXCEPTION("Time");

	CLASS_EXCEPTION(BufferUnderflow, "Buffer underflow");
	CLASS_EXCEPTION(InvalidPattern, "Invalid pattern");
	CLASS_EXCEPTION(InvalidDay, "Invalid `day' field");
	CLASS_EXCEPTION(InvalidHour, "Invalid `hour' field");
	CLASS_EXCEPTION(InvalidMinute, "Invalid `minute' field");
	CLASS_EXCEPTION(InvalidMonth, "Invalid `month' field");
	CLASS_EXCEPTION(InvalidSecond, "Invalid `second' field");
	CLASS_EXCEPTION(InvalidSeparator, "Invalid separator field");
	CLASS_EXCEPTION(InvalidSource, "Invalid source");
	CLASS_EXCEPTION(InvalidYear, "Invalid `year' field");
	CLASS_EXCEPTION(JunkAtEnd, "Junk at end");
	CLASS_EXCEPTION(MkTime, "Can't system mktime");

private :

	void DoCorrections();
	static bool IsBissext(const int year);

	int	year,		// 1900 -- ???
		month,		// 0 -- 11
		day,		// 0 -- 30
		hour,		// 0 -- 23
		minute,		// 0 -- 59
		second;		// 0 -- 59

};

// ============================================================================

} // namespace BFC

// ============================================================================


#endif // _MY_TIME_H


// ============================================================================

