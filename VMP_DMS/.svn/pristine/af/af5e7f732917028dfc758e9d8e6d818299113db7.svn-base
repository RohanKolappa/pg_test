// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Debug".
// 
// "TBFC::Debug" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Debug" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Debug"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Debug.DObject.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Light_DObject_H_
#define _TBFC_Light_DObject_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SObject.h"

#include "TBFC.Debug.ColoredBuffer.h"
#include "TBFC.Debug.ConsoleColor.h"
#include "TBFC.Debug.ThreadFinderPtr.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Generates a debug message.
///
/// \ingroup TBFC_Light

TBFC_Light_DLL void msgDbg(
	const	TBFC::Buffer	& __msg
);

/// \brief Generates a warning message.
///
/// \ingroup TBFC_Light

TBFC_Light_DLL void msgWrn(
	const	TBFC::Buffer	& __msg
);

/// \brief Generates an error message.
///
/// \ingroup TBFC_Light

TBFC_Light_DLL void msgErr(
	const	TBFC::Buffer	& __msg
);

/// \brief Generates an exception message.
///
/// \ingroup TBFC_Light

TBFC_Light_DLL void msgExc(
		TBFC::Exception	& __exception
);

/// \brief Generates an exception message.
///
/// \ingroup TBFC_Light

TBFC_Light_DLL void msgExc(
	const	TBFC::Exception	& __exception,
	const	TBFC::Buffer	& __msg
);

// ============================================================================

/// \brief Base class of all objects with debugging capabilities.
///
/// \ingroup TBFC_Light

class TBFC_Light_DLL DObject : virtual public SObject {

public :

	/// \brief Creates a new DObject.
	///
	/// The generation of output messages is automatically turned on.

	DObject(
	) : flag( true ) {
	}

	/// \brief Destroys this object.

	virtual ~DObject(
	);

	/// \brief Returns the name of this object.

	const Buffer & getObjectName(
	) const {
		return name;
	}

	/// \brief Sets the name of this object to \a __objName.
	///
	/// \param __objName
	///	The new name of this object.

	void setObjectName(
		const	Buffer		& __objName
	) {
		name = __objName;
	}

	/// \brief Turns generation of debugging messages on or off.
	///
	/// \param __onoff
	///	Turns on (true) or off (false).

	void setDebug(
		const	Bool		__onoff
	) {
		flag = __onoff;
	}

	/// \brief Returns true iff generation of debugging messages has been
	///	turned on.

	Bool getDebug(
	) const {
		return flag;
	}

	/// \brief Enumerated list of importance levels.

	enum Level {
		DebugLevel = 0,	///< Simple debug message.
		WarningLevel,	///< Warning message.
		ErrorLevel,	///< Error message.
		UserLevel	///< Reserved for the user.
	};

	/// \brief Enumerated list of output message parts.

	enum Part {
		HeaderPart = 0,	///< Time/date.
		ThreadPart,	///< Thread name (if any).
		ObjectPart,	///< Class/object name.
		MessagePart,	///< User message.
		ExceptionPart	///< Exception string.
	};

	/// \brief Turns the generation of long data/time prefixes on or off.

	static void setLongFormat(
		const	Bool		__yesno
	) {
		longFormat = __yesno;
	}

	/// \brief Sets the foreground color used for part \a __part of messages of
	///	level \a __level to \a __color.

	static void setForegroundColor(
		const	Level		__level,
		const	Part		__part,
		const	ConsoleColor	__color
	) {
		partColor[ __level ][ 0 ][ __part ] = __color;
	}

	/// \brief Returns the foreground color used for part \a __part of messages
	///	of level \a __level.

	static ConsoleColor getForegroundColor(
		const	Level		__level,
		const	Part		__part
	) {
		return partColor[ __level ][ 0 ][ __part ];
	}

	/// \brief Sets the background color used for part \a __part of messages of
	///	level \a __level to \a __color.

	static void setBackgroundColor(
		const	Level		__level,
		const	Part		__part,
		const	ConsoleColor	__color
	) {
		partColor[ __level ][ 1 ][ __part ] = __color;
	}

	/// \brief Returns the background color used for part \a __part of messages
	///	of level \a __level.

	static ConsoleColor getBackgroundColor(
		const	Level		__level,
		const	Part		__part
	) {
		return partColor[ __level ][ 1 ][ __part ];
	}

	/// \brief Replaces the current global thread name finder by
	///	\a __finder.

	static void setThreadFinder(
			ThreadFinderPtr	__finder
	) {
		finder = __finder;
	}

	/// \brief Generates a debug message.

	static void staticMsgDbg(
		const	Buffer		& __msg
	);

	/// \brief Generates a warning message.

	static void staticMsgWrn(
		const	Buffer		& __msg
	);

	/// \brief Generates an error message.

	static void staticMsgErr(
		const	Buffer		& __msg
	);

	/// \brief Generates a message.

	static void staticMsg(
		const	Buffer		& __msg,
		const	Level		__level = UserLevel
	);

	/// \brief Generates an exception message.

	static void staticMsgExc(
		const	Exception	& __exception
	);

	/// \brief Generates an exception message.

	static void staticMsgExc(
		const	Exception	& __exception,
		const	Buffer		& __msg
	);

protected :

	/// \brief Generates a debug message.

	void msgDbg(
		const	Buffer		& __msg
	) const;

	/// \brief Generates a warning message.

	void msgWrn(
		const	Buffer		& __msg
	) const;

	/// \brief Generates an error message.

	void msgErr(
		const	Buffer		& __msg
	) const;

	/// \brief Generates a message.

	void msg(
		const	Buffer		& __msg,
		const	Level		__level = UserLevel
	) const;

	/// \brief Generates an exception message.

	void msgExc(
			Exception	& __exception
	) const;

	/// \brief Generates an exception message.

	void msgExc(
			Exception	& __exception,
		const	Buffer		& __msg
	) const;

private :

	Bool			flag;		///< Generate output ?
	Buffer			name;		///< Object name.
	static Bool		longFormat;	///< Always use long format ?
	static ConsoleColor	partColor[ 4 ][ 2 ][ 5 ];
						///< Color palette.
	static ThreadFinderPtr	finder;		///< Thread name finder.

	/// \brief Returns a string representing the current data/time.

	static ColoredBuffer getDateTimePart(
		const	Level		__level
	);

	/// \brief Returns a string representing the current thread.

	static ColoredBuffer getThreadPart(
		const	Level		__level
	);

	/// \brief Returns a string representing the current object.

	ColoredBuffer getObjectPart(
		const	Level		__level
	) const;

	/// \brief Returns the string that should prefix the user message.

	ColoredBuffer getMessagePrefix(
		const	Level		__level
	) const;

	/// \brief Formatting method.

	static ColoredBuffer prefixedBy(
		const	ColoredBuffer	& prefix,
		const	Bool		inter,
		const	ColoredBuffer	& multiLine
	);

	/// \brief Output method.

	static void sendMsg(
		const	ColoredBuffer	& prefix,
		const	Bool		inter,
		const	ColoredBuffer	& multiline
	);

	/// \brief Forbidden copy constructor.

	DObject(
		const	DObject		&
	);

	/// \brief Forbidden copy operator.

	DObject & operator = (
		const	DObject		&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Light_DObject_H_

// ============================================================================

