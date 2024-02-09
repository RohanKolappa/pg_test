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
//	BFC.Base.Time.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================


#include <stdio.h>
#include <time.h>
#include "BFC.Base.Types.h"
#include "BFC.Base.Buffer.h"
#include "BFC.Base.Time.h"


// ============================================================================

using namespace BFC;

// ============================================================================

Time::Time(
	const int ye,
	const int mo,
	const int da,
	const int ho,
	const int mi,
	const int se) {

	year   = ye;
	month  = mo;
	day    = da;
	hour   = ho;
	minute = mi;
	second = se;

	DoCorrections();

}

Time::Time(const Time& src) {

	year   = src.year;
	month  = src.month;
	day    = src.day;
	hour   = src.hour;
	minute = src.minute;
	second = src.second;

}

Time::Time(const Buffer& src) {

	buildFromBuffer(src);

}

Time::~Time() {



}

Time& Time::operator = (const Time& src) {

	if (this != &src) {
		year   = src.year;
		month  = src.month;
		day    = src.day;
		hour   = src.hour;
		minute = src.minute;
		second = src.second;
	}

	return *this;

}

bool Time::operator == (const Time& src)  const {

	return (year   == src.year
	     && month  == src.month
	     && day    == src.day
	     && hour   == src.hour
	     && minute == src.minute
	     && second == src.second);

}

bool Time::operator != (const Time& src)  const {

	return (year   != src.year
	     || month  != src.month
	     || day    != src.day
	     || hour   != src.hour
	     || minute != src.minute
	     || second != src.second);

}

bool Time::operator < (const Time& src)  const {

	if (year   < src.year  ) return true;
	if (year   > src.year  ) return false;
	if (month  < src.month ) return true;
	if (month  > src.month ) return false;
	if (day    < src.day   ) return true;
	if (day    > src.day   ) return false;
	if (hour   < src.hour  ) return true;
	if (hour   > src.hour  ) return false;
	if (minute < src.minute) return true;
	if (minute > src.minute) return false;

	return (second < src.second);

}

bool Time::operator <= (const Time& src)  const {

	if (year   < src.year  ) return true;
	if (year   > src.year  ) return false;
	if (month  < src.month ) return true;
	if (month  > src.month ) return false;
	if (day    < src.day   ) return true;
	if (day    > src.day   ) return false;
	if (hour   < src.hour  ) return true;
	if (hour   > src.hour  ) return false;
	if (minute < src.minute) return true;
	if (minute > src.minute) return false;

	return (second <= src.second);

}

bool Time::operator > (const Time& src)  const {

	if (year   > src.year  ) return true;
	if (year   < src.year  ) return false;
	if (month  > src.month ) return true;
	if (month  < src.month ) return false;
	if (day    > src.day   ) return true;
	if (day    < src.day   ) return false;
	if (hour   > src.hour  ) return true;
	if (hour   < src.hour  ) return false;
	if (minute > src.minute) return true;
	if (minute < src.minute) return false;

	return (second > src.second);

}

bool Time::operator >= (const Time& src)  const {

	if (year   > src.year  ) return true;
	if (year   < src.year  ) return false;
	if (month  > src.month ) return true;
	if (month  < src.month ) return false;
	if (day    > src.day   ) return true;
	if (day    < src.day   ) return false;
	if (hour   > src.hour  ) return true;
	if (hour   < src.hour  ) return false;
	if (minute > src.minute) return true;
	if (minute < src.minute) return false;

	return (second >= src.second);

}

Time& Time::operator += (const Time& src) {

	year   += src.year;
	month  += src.month;
	day    += src.day;
	hour   += src.hour;
	minute += src.minute;
	second += src.second;

	DoCorrections();

	return *this;

}

Time Time::operator + (const Time& src) const {

	return Time(
		year   + src.year,
		month  + src.month,
		day    + src.day,
		hour   + src.hour,
		minute + src.minute,
		second + src.second);

}

Time& Time::operator -= (const Time& src) {

	year   -= src.year;
	month  -= src.month;
	day    -= src.day;
	hour   -= src.hour;
	minute -= src.minute;
	second -= src.second;

	DoCorrections();

	return *this;

}

Time Time::operator - (const Time& src) const {

	return Time(
		year   - src.year,
		month  - src.month,
		day    - src.day,
		hour   - src.hour,
		minute - src.minute,
		second - src.second);

}

void Time::buildFrom(const Buffer& src, const Buffer& pattern) {

	Uint32	srcidx = 0,
		patidx = 0,
		pos,
		val;
	char	c;

	second =
	minute =
	hour   =
	day    =
	month  =
	year   = 0;

	while (patidx < pattern.length()) {

		c = pattern[patidx++];

		if (c == 's' || c == 'm' || c == 'h'
		 || c == 'D' || c == 'M' || c == 'Y') {
			pos = srcidx;
			val = src.getUintAt(pos);
			if (pos == srcidx)
				throw InvalidSource(src);
			srcidx = pos;
			switch (c) {
			case 's' :
				if (val >= 60)
					throw InvalidSecond();
				second = (int)val;
				break;
			case 'm' :
				if (val >= 60)
					throw InvalidMinute();
				minute = (int)val;
				break;
			case 'h' :
				if (val >= 24)
					throw InvalidHour();
				hour = (int)val;
				break;
			case 'D' :
				if (val == 0 || val >= 32)
					throw InvalidDay();
				day = (int)val - 1;
				break;
			case 'M' :
				if (val == 0 || val >= 13)
					throw InvalidMonth();
				month = (int)val - 1;
				break;
			case 'Y' :
				if (val < 1900)
					throw InvalidYear();
				year = (int)val;
				break;
			}
		}

		else if (srcidx >= src.length())

			throw BufferUnderflow();

		else if (src[srcidx++] != c)

			throw InvalidSeparator();

	}

	if (srcidx != src.length())
		throw JunkAtEnd();

}

void Time::buildFromBuffer(const Buffer& src) {

	Uint32	pos = 0;

	year = (int)src.getUintAt(pos);
	if (!pos || src[pos++] != '-')
		throw InvalidYear();

	month = (int)src.getUintAt(pos) - 1;
	if (!pos || src[pos++] != '-')
		throw InvalidMonth();

	day = (int)src.getUintAt(pos) - 1;
	if (!pos)
		throw InvalidDay();

	if (pos == src.length()) {
		hour   =
		minute =
		second = 0;
		return;
	}

	if (src[pos++] != '_')
		throw InvalidSource(src);

	hour = (int)src.getUintAt(pos);
	if (!pos || src[pos++] != ':')
		throw InvalidHour();

	minute = (int)src.getUintAt(pos);
	if (!pos || src[pos++] != ':')
		throw InvalidMinute();

	second = (int)src.getUintAt(pos);
	if (pos != src.length())
		throw InvalidSecond();

	DoCorrections();

}

void Time::buildFromSecondsSinceEpoch(
	const Uint32 seconds) {

	time_t t = (time_t)seconds;

	struct tm *gmt = gmtime(&t);

	year   = gmt->tm_year + 1900;	// NOT 1970!!!
	month  = gmt->tm_mon;
	day    = gmt->tm_mday - 1;
	hour   = gmt->tm_hour;
	minute = gmt->tm_min;
	second = gmt->tm_sec;

}

Time& Time::setYear(const int y) {

	year = y;

	DoCorrections();

	return *this;

}

Time& Time::setMonth(const int m) {

	month = m;

	DoCorrections();

	return *this;

}

Time& Time::setDay(const int d) {

	day = d;

	DoCorrections();

	return *this;

}

Time& Time::setHour(const int h) {

	hour = h;

	DoCorrections();

	return *this;

}

Time& Time::setMinute(const int m) {

	minute = m;

	DoCorrections();

	return *this;

}

Time& Time::setSecond(const int s) {

	second = s;

	DoCorrections();

	return *this;

}

int Time::getYear() const {

	return year;

}

int Time::getMonth() const {

	return month;

}

int Time::getDay() const {

	return day;

}

int Time::getHour() const {

	return hour;

}

int Time::getMinute() const {

	return minute;

}

int Time::getSecond() const {

	return second;

}

Uint32 Time::getSecondsSinceEpoch() const {

	struct tm t;

	t.tm_sec = second;
	t.tm_min = minute;
	t.tm_hour = hour;
	t.tm_mday = day + 1;
	t.tm_mon  = month;
	t.tm_year = year - 1900;

	t.tm_isdst = 0;

	time_t res = mktime(&t);

	if (res == (time_t)-1)
		throw MkTime();

	return (Uint32)res;

}

Uint32 Time::getDaysSinceJanuary1() const {

	struct tm t;

	t.tm_sec = second;
	t.tm_min = minute;
	t.tm_hour = hour;
	t.tm_mday = day + 1;
	t.tm_mon  = month;
	t.tm_year = year - 1900;

	t.tm_isdst = 0;

	time_t res = mktime(&t);

	if (res == (time_t)-1)
		throw MkTime();

	return (Uint32)t.tm_yday;

}

Buffer Time::shortOutputToBuffer()  const {

	char buf[32];

	sprintf(buf,
	        "%4d-%02d-%02d",
	        year,
	        month + 1,
	        day   + 1);

	return Buffer(buf);

}

Buffer Time::hourToBuffer()  const {

	char buf[32];

	sprintf(buf,
	        "%02d:%02d:%02d",
	        hour,
	        minute,
	        second);

	return Buffer(buf);

}

Buffer Time::longOutputToBuffer()  const {

	char buf[32];

	sprintf(buf,
	        "%4d-%02d-%02d_%02d:%02d:%02d",
	        year,
	        month + 1,
	        day   + 1,
	        hour,
	        minute,
	        second);

	return Buffer(buf);

}

Time Time::now() {

	time_t t = time((time_t *)0);
			// returns the time since the Epoch (00:00:00 UTC,
			// January 1, 1970), measured in seconds.

	struct tm *gmt = gmtime(&t);

	Time result;

	result.year   = gmt->tm_year + 1900;	// NOT 1970!!!
	result.month  = gmt->tm_mon;
	result.day    = gmt->tm_mday - 1;
	result.hour   = gmt->tm_hour;
	result.minute = gmt->tm_min;
	result.second = gmt->tm_sec;

	return result;

}

int Time::numberOfDaysInMonth(
	const	int	month,
	const	int	year) {

	if (month < 0 || month > 11)
		return 0;

	int tbl1[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	    tbl2[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	    *tbl;

	tbl = (IsBissext(year) ? tbl2 : tbl1);

	return tbl[month];

}

int Time::numberOfDaysInYear(
	const	int	year) {

	return (IsBissext(year) ? 366 : 365);



}

void Time::DoCorrections() {

// META : Time.C
// META : DoCorrections()
// META : Find a way to trust modulo-arithmetic...

	// for the seconds

	while (second < 0) {
		second += 60;
		minute--;
	}

	minute += (second / 60);
	second %= 60;

	// for the minutes

	while (minute < 0) {
		minute += 60;
		hour--;
	}

	hour += (minute / 60);
	minute %= 60;

	// for the hours

	while (hour < 0) {
		hour += 24;
		day--;
	}

	day += (hour / 24);
	hour %= 24;

	// for the days... We do a first correction on the month

	while (month < 0) {
		month += 12;
		year--;
	}
	year += (month / 12);
	month %= 12;

	int tbl1[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	    tbl2[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	    *tbl;

	tbl = (IsBissext(year) ? tbl2 : tbl1);

	while (day < 0) {
		if (month == 0) {
			day += 31;
			month = 11;
			year--;
			tbl = (IsBissext(year) ? tbl2 : tbl1);
		}
		else {
			day += tbl[month - 1];
			month--;
		}
	}

	while (day >= tbl[month]) {
		day -= tbl[month];
		month++;
		if (month == 12) {
			month = 0;
			year++;
			tbl = (IsBissext(year) ? tbl2 : tbl1);
		}
	}


}

bool Time::IsBissext(const int year) {

	if (year % 4)
		return false;

	if (year % 100)
		return true;

	return (year % 400 == 0);

}

// ============================================================================


