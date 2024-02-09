// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	QtUIParser.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _QtUIParser_H_
#define _QtUIParser_H_

// ============================================================================

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QStringList>

#include "Buffer.h"

// ============================================================================

class QtUIParser {

public :

	QtUIParser(
	);

	virtual ~QtUIParser(
	);

	Buffer parse(
		const	Buffer&		content
	);

	void setDebug(
		const	bool
	);

protected :

private :

	bool		doDebug;
	QStringList	customList;

	void msgWrn( const Buffer& );

	QDomDocument parse(
			QDomDocument		doc
	);

	QDomElement parseContainer(
			QDomDocument		oDoc,
			QDomElement		iElement,
		const	Buffer&			tagName,
		const	Buffer&			properties
	);

	void parseBox(
			QDomDocument		oDoc,
			QDomElement		oElement,
			QDomElement		iElement
	);

	QDomElement parseSimpleWidget(
			QDomDocument		oDoc,
			QDomElement		iElement,
		const	Buffer&			tagName,
		const	Buffer&			properties,
		const	Buffer&			attributes
	);

	QDomElement parseComboBox(
			QDomDocument		oDoc,
			QDomElement		iElement
	);

	QDomElement parseButtonGroup(
			QDomDocument		oDoc,
			QDomElement		iElement
	);

	QDomElement parseWidgetStack(
			QDomDocument		oDoc,
			QDomElement		iElement
	);

	// others...

	QtUIParser(
		const	QtUIParser&
	);

	QtUIParser& operator = (
		const	QtUIParser&
	);

};

// ============================================================================

#endif // _QtUIParser_H_

// ============================================================================

