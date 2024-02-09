// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Debug".
// 
// "BFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Debug.DObject.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_DObject_H_
#define _BFC_Light_DObject_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtrDecl.h"

// ============================================================================

namespace BFC {

BFC_PTR_DECL( BFC_Light_DLL, DObject )

} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "BFC.Debug.ColoredBuffer.h"
#include "BFC.Debug.ConsoleColor.h"
#include "BFC.Debug.DebuggerPtr.h"
#include "BFC.Debug.ThreadFinderPtr.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Generates a debug message.
///
/// \ingroup BFC_Light

BFC_Light_DLL void msgDbg(
	const	Buffer &	pMsg
);

/// \brief Generates an info message.
///
/// \ingroup BFC_Light

BFC_Light_DLL void msgInf(
	const	Buffer &	pMsg
);

/// \brief Generates a warning message.
///
/// \ingroup BFC_Light

BFC_Light_DLL void msgWrn(
	const	Buffer &	pMsg
);

/// \brief Generates an error message.
///
/// \ingroup BFC_Light

BFC_Light_DLL void msgErr(
	const	Buffer &	pMsg
);

/// \brief Generates an exception message.
///
/// \ingroup BFC_Light

BFC_Light_DLL void msgExc(
		Exception &	pExc
);

/// \brief Generates an exception message.
///
/// \ingroup BFC_Light

BFC_Light_DLL void msgExc(
	const	Exception &	pExc,
	const	Buffer &	pMsg
);

// ============================================================================

/// \brief Base class of all objects with debugging capabilities.
///
/// \ingroup BFC_Light

class BFC_Light_DLL DObject : virtual public SObject {

public :

	/// \brief Enumerated list of log levels.

	enum LogLevel {
		QuietLevel	= 0,		///< No output.
		ErrorLevel	= 1,		///< Error message.
		WarningLevel	= 2,		///< Warning message.
		InfoLevel	= 3,		///< Informative message.
		DebugLevel	= 4,		///< Simple debug message.

		MinLevel	= QuietLevel,	///< Minimum allowed level (incl.).
		MaxLevel	= DebugLevel	///< Maximum allowed level (incl.).
	};

	/// \brief Enumerated list of output message parts.

	enum Part {
		HeaderPart = 0,	///< Time/date.
		ThreadPart,	///< Thread name (if any).
		ObjectPart,	///< Class/object name.
		MessagePart,	///< User message.
		ExceptionPart	///< Exception string.
	};

	/// \brief Creates a new DObject.
	///
	/// The generation of output messages is automatically turned on.

	DObject(
	) :
		objLevel( DebugLevel ) {
	}

	/// \brief Creates a new DObject named \a pName.

	DObject(
		const	Buffer &	pName
	) :
		objLevel( DebugLevel ),
		name	( pName )
	{
	}

	/// \brief Creates a copy of \a pOther.

	DObject(
		const	DObject &	pOther
	) :
		SObject	( pOther ),
		objLevel( pOther.objLevel ),
		name	( pOther.name ) {
	}

	/// \brief Returns the name of this object.

	const Buffer & getObjectName(
	) const {
		return name;
	}

	/// \brief Sets the name of this object to \a pNme.
	///
	/// \param pNme
	///	The new name of this object.

	void setObjectName(
		const	Buffer &	pNme
	) {
		name = pNme;
	}

	/// \brief Adds \a pXtraName to the name of this object.
	///
	/// \param pXtraName
	///	The suffix to add.

	void addObjectName(
		const	Buffer &	pXtraName
	) {
		name += pXtraName;
	}

	/// \brief Turns generation of debugging messages on or off.
	///
	/// \param pFlg
	///	Turns on (DebugLevel) or off (false).

	void setDebug(
		const	Bool		pFlg
	) {
		objLevel = ( pFlg ? DebugLevel : QuietLevel );
	}

	/// \brief Returns true iff generation of debugging messages has been
	///	turned on.

	Bool getDebug(
	) const {
		return ( objLevel != QuietLevel );
	}

	/// \brief Sets the object logging level to \a pObjLevel.

	void setLogLevel(
		const	LogLevel	pObjLevel
	) {
		objLevel = pObjLevel;
	}

	/// \brief Returns the object logging level.

	LogLevel getLogLevel(
	) const {
		return objLevel;
	}

	/// \brief Sets the global logging level to \a pObjLevel.

	static void setGlobalLogLevel(
		const	LogLevel	pGlbLevel
	) {
		glbLevel = pGlbLevel;
	}

	/// \brief Returns the global logging level.

	static LogLevel getGlobalLogLevel(
	) {
		return glbLevel;
	}

	/// \brief Turns the generation of long data/time prefixes on or off.

	static void setLongFormat(
		const	Bool		pFlg
	) {
		longFormat = pFlg;
	}

	/// \brief Sets the foreground color used for part \a pPrt of messages of
	///	level \a pLvl to \a pClr.

	static void setForegroundColor(
		const	LogLevel	pLvl,
		const	Part		pPrt,
		const	ConsoleColor	pClr
	) {
		if ( pLvl != QuietLevel ) {
			partColor[ pLvl ][ 0 ][ pPrt ] = pClr;
		}
	}

	/// \brief Returns the foreground color used for part \a pPrt of messages
	///	of level \a pLvl.

	static ConsoleColor getForegroundColor(
		const	LogLevel	pLvl,
		const	Part		pPrt
	) {
		return ( pLvl != QuietLevel
			? partColor[ pLvl ][ 0 ][ pPrt ]
			: Black );
	}

	/// \brief Sets the background color used for part \a pPrt of messages of
	///	level \a pLvl to \a pClr.

	static void setBackgroundColor(
		const	LogLevel	pLvl,
		const	Part		pPrt,
		const	ConsoleColor	pClr
	) {
		if ( pLvl != QuietLevel ) {
			partColor[ pLvl ][ 1 ][ pPrt ] = pClr;
		}
	}

	/// \brief Returns the background color used for part \a pPrt of messages
	///	of level \a pLvl.

	static ConsoleColor getBackgroundColor(
		const	LogLevel	pLvl,
		const	Part		pPrt
	) {
		return ( pLvl != QuietLevel
			? partColor[ pLvl ][ 1 ][ pPrt ]
			: Black );
	}

	/// \brief Replaces the current global thread name finder by
	///	\a pFinder.

	static void setThreadFinder(
			ThreadFinderPtr	pFinder
	) {
		getThreadFinder() = pFinder;
	}

	/// \brief Returns the current global thread name finder (by reference,
	///	so you can tweak it).

	static ThreadFinderPtr & getThreadFinder(
	);

	/// \brief Generates a debug message.

	static void staticMsgDbg(
		const	Buffer &	pMsg
	);

	/// \brief Generates an info message.

	static void staticMsgInf(
		const	Buffer &	pMsg
	);

	/// \brief Generates a warning message.

	static void staticMsgWrn(
		const	Buffer &	pMsg
	);

	/// \brief Generates an error message.

	static void staticMsgErr(
		const	Buffer &	pMsg
	);

	/// \brief Generates a message.

	static void staticMsg(
		const	Buffer &	pMsg,
		const	LogLevel	pLvl = InfoLevel
	);

	/// \brief Generates an exception message.

	static void staticMsgExc(
		const	Exception &	pExc
	);

	/// \brief Generates an exception message.

	static void staticMsgExc(
		const	Exception &	pExc,
		const	Buffer &	pMsg
	);

	static void setDebugger(
			DebuggerPtr	pDebugger
	) {
		debugger = pDebugger;
	}

protected :

	/// \brief Generates a debug message.

	void msgDbg(
		const	Buffer &	pMsg
	) const;

	/// \brief Generates an info message.

	void msgInf(
		const	Buffer &	pMsg
	) const;

	/// \brief Generates a warning message.

	void msgWrn(
		const	Buffer &	pMsg
	) const;

	/// \brief Generates an error message.

	void msgErr(
		const	Buffer &	pMsg
	) const;

	/// \brief Generates a message.

	void msg(
		const	Buffer &	pMsg,
		const	LogLevel	pLvl = InfoLevel
	) const;

	/// \brief Generates an exception message.

	void msgExc(
			Exception &	pExc
	) const;

	/// \brief Generates an exception message.

	void msgExc(
			Exception &	pExc,
		const	Buffer &	pMsg
	) const;

	/// \brief Returns a string representing the current data/time.

	static ColoredBuffer getDateTimePart(
		const	LogLevel	pLvl
	);

	/// \brief Returns a string representing the current thread.

	static ColoredBuffer getThreadPart(
		const	LogLevel	pLvl
	);

	/// \brief Returns a string representing the current object.

	ColoredBuffer getObjectPart(
		const	LogLevel	pLvl
	) const;

	/// \brief Returns the string that should prefix the user message.

	ColoredBuffer getMessagePrefix(
		const	LogLevel	pLvl
	) const;

	/// \brief Formatting method.

	static ColoredBuffer prefixedBy(
		const	ColoredBuffer &	prefix,
		const	Bool		inter,
		const	ColoredBuffer &	multiLine
	);

	/// \brief Output method.

	static void sendMsg(
		const	ColoredBuffer &	prefix,
		const	Bool		inter,
		const	ColoredBuffer &	multiline
	);

private :

	static DebuggerPtr	debugger;
	static Bool		longFormat;			///< Always use long format ?
	static LogLevel		glbLevel;			///< Global logging level.
	static ConsoleColor	partColor[ 5 ][ 2 ][ 5 ];	///< Color palette.
	LogLevel		objLevel;			///< Object logging level.
	Buffer			name;				///< Object name.

	/// \brief Forbidden copy operator.

	DObject & operator = (
		const	DObject &
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Light_DObject_H_

// ============================================================================

