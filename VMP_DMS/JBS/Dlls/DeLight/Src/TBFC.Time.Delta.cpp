// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Time.Delta.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Time.Delta.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

const Time::Delta Time::OneYear  ( 60 * 60 * 24 * 365, 0 );
const Time::Delta Time::OneBYear ( 60 * 60 * 24 * 366, 0 );
const Time::Delta Time::OneDay   ( 60 * 60 * 24, 0 );
const Time::Delta Time::OneHour  ( 60 * 60, 0 );
const Time::Delta Time::OneMinute( 60, 0 );
const Time::Delta Time::OneSecond( 1, 0 );

// ============================================================================

