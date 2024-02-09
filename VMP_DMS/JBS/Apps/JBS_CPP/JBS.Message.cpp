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
//	JBS.Message.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "BFC.Debug.ConsoleColor.h"

#include "JBS.Context.h"
#include "JBS.Message.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

struct TblEntry {
	TblEntry(
		const	char			*__prefix,
		const	ConsoleColor	__foreground,
		const	ConsoleColor	__background,
		const	Message::Type		__priority
	) : prefix( __prefix )
	, foreground( __foreground )
	, background( __background )
	, priority( __priority ) {
	}
	const	char			*prefix;
	const	ConsoleColor	foreground;
	const	ConsoleColor	background;
	const	Message::Type		priority;
};

static TblEntry tbl[] = {
TblEntry( "[BUILD'G]", Yellow,		Blue,			Message::Normal ),
TblEntry( "[MOC'ING]", LightBlue,		DefaultBackground,	Message::Normal ),
TblEntry( "[UIC'ING]", LightMagenta,	DefaultBackground,	Message::Normal ),
TblEntry( "[COMPILE]", LightGreen,	DefaultBackground,	Message::Normal ),
TblEntry( "[LNK LIB]", Yellow,		Brown,			Message::Normal ),
TblEntry( "[LNK EXE]", Yellow,		Red,			Message::Normal ),
TblEntry( "[GEN BSC]", LightBlue,		Blue,			Message::Normal ),
TblEntry( "[STRIP'G]", LightRed,		DefaultBackground,	Message::Normal ),
TblEntry( "[GEN DEP]", LightGreen,	DefaultBackground,	Message::Normal ),
TblEntry( "[GEN DEF]", LightCyan,		DefaultBackground,	Message::Normal ),
TblEntry( "[PACK'NG]", LightCyan,		DefaultBackground,	Message::Normal ),
TblEntry( "[IMP VUI]", LightCyan,		Cyan,			Message::Normal ),
TblEntry( "[DEC VUI]", LightCyan,		Cyan,			Message::Normal ),
TblEntry( "[QUI VUI]", LightMagenta,	Magenta,			Message::Normal ),
TblEntry( "[DEL'ING]", LightMagenta,	Magenta,			Message::Normal ),
TblEntry( "[TOUCH'G]", LightMagenta,	DefaultBackground,	Message::Normal ),
TblEntry( "[MAK DIR]", LightGreen,	DefaultBackground,	Message::Normal ),
TblEntry( "[DEL DIR]", LightMagenta,	Magenta,			Message::Normal ),
TblEntry( "[ENTER'G]", White,		Yellow,			Message::Debug ),
TblEntry( "[CLEAN'G]", Yellow,		Blue,			Message::Normal ),
TblEntry( "[ANALYZE]", White,		Yellow,			Message::Debug ),
TblEntry( "[DBGINFO]", LightGreen,	DefaultBackground,	Message::Debug ),
TblEntry( "[MESSAGE]", Yellow,		DefaultBackground,	Message::Normal ),
TblEntry( "[WARNING]", Black,		Yellow,			Message::Normal ),
TblEntry( "[PROBLEM]", Brown,		LightRed,			Message::Warning )
};

static struct PrefEntry {
	PrefEntry(
		const	ConsoleColor	__foreground,
		const	ConsoleColor	__background
	) : foreground( __foreground )
	, background( __background ) {
	}
	const	ConsoleColor	foreground;
	const	ConsoleColor	background;
} pref[] = {
PrefEntry( LightGreen,	DefaultBackground ),
PrefEntry( White,		DefaultBackground ),
PrefEntry( LightRed,	DefaultBackground )
};

// ============================================================================

Message::Message(
	const	Action		action,
	const	FSPath&		path ) {

	Buffer m = path.getLastComponent();

	if ( path.size() > 1 ) {
		m += " (" + path.toBuffer() + ")";
	}

	context->output->msg(
		tbl[ action ].prefix,
		m,
		pref[ tbl[ action ].priority ].foreground,
		pref[ tbl[ action ].priority ].background,
		tbl[ action ].foreground,
		tbl[ action ].background,
		tbl[ action ].priority );

}

Message::Message(
	const	Action		action,
	const	Buffer&		message ) {

	context->output->msg(
		tbl[ action ].prefix,
		message,
		pref[ tbl[ action ].priority ].foreground,
		pref[ tbl[ action ].priority ].background,
		tbl[ action ].foreground,
		tbl[ action ].background,
		tbl[ action ].priority );

}

Message::~Message() {



}

// ============================================================================

