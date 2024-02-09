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
//	JBS.Tools.Qui2Vui.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.BufQString.h"
#include "BFC.Base.BufferParser.h"
#include "BFC.IO.File.h"

#include "JBS.Tools.Qui2Vui.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL_NS( Tools, Qui2Vui )

// ============================================================================

Tools::Qui2Vui::Qui2Vui() {



}

Tools::Qui2Vui::~Qui2Vui() {



}

// ============================================================================

void Tools::Qui2Vui::generate(
		Atoms::Ui2VuiAtomPtr	vuiAtom ) {

	FSPath	quiPath = vuiAtom->getUiAtom()->getAtomPath();
	FSPath	vuiPath = vuiAtom->getAtomPath();

	IO::File quiFile(
		quiPath.toBuffer(),
		IO::File::Read );
	IO::File vuiFile(
		vuiPath.toBuffer(),
		IO::File::Write | IO::File::Truncate | IO::File::Create );

	quiFile.open();
	vuiFile.open();

	Buffer	iContent = quiFile.dumpToBuffer();
	Buffer	oContent = parse( iContent );

	vuiFile.putBytes( oContent );

	quiFile.close();
	vuiFile.close();

}

// ============================================================================

Buffer Tools::Qui2Vui::parse(
	const	Buffer&		content ) {

	QDomDocument iDomDoc;
	QString qcontent = BufQString( content );

	QString		errMsg;
	int		errRow;
	int		errCol;

	if ( ! iDomDoc.setContent( qcontent, &errMsg, &errRow, &errCol ) ) {
		msgWrn( "Can't parse document!" );
		msgWrn( "msg: " + BufQString( errMsg ).toBuffer() );
//		msgWrn( "row: " + Buffer( errRow ) );
//		msgWrn( "col: " + Buffer( errCol ) );
		return Buffer();
	}

	QDomDocument oDomDoc = parse( iDomDoc );
	QString oString = oDomDoc.toString( 2 );

	return BufQString( oString ).toBuffer();

}

// ============================================================================

QDomDocument Tools::Qui2Vui::parse(
		QDomDocument	iDoc ) {

	QDomDocument oDoc;

	// <UI>...</UI>

//	msgDbg( "parse(): finding <UI>...</UI> element..." );

	QDomElement uiElt = iDoc.documentElement();

	if ( uiElt.isNull() ) {
		msgWrn( "No content in document!" );
		return oDoc;
	}

	if ( uiElt.tagName() != "UI" ) {
		msgWrn( "parse(): no <UI></UI> root element!" );
		return oDoc;
	}

	QDomElement oRootElt = oDoc.createElement( "UI" );

	oDoc.appendChild( oRootElt );

	// Check that the version is at least 3.2

//	msgDbg( "parse(): checking version..." );

	if ( ! uiElt.hasAttribute( "version" ) ) {
		msgWrn( "parse(): no \"version\" attribute found!" );
	}
	else if ( uiElt.attribute( "version" ) != "3.2" ) {
		msgWrn( "parse(): unknown version: "
			+ BufQString( uiElt.attribute( "version" ) ).toBuffer() );
	}
//	else {
//		msgDbg( "parse(): found version 3.2 UI file!" );
//	}

	if ( ! uiElt.nextSibling().isNull() ) {
		msgWrn( "parse(): found sibling elt to <UI>...</UI>!!!" );
	}

	// The 'uiElt' should have the following children:
	// (Y) <class> ... </class>
	// (Y) <widget class="..."> ... </widget>
	// (Y) <layoutdefaults ... />
	// (O) <tabstops> ... </tabstops>

	QDomElement	tabOrder;
	QDomElement	widget;

	for ( QDomElement e = uiElt.firstChild().toElement()
	    ; ! e.isNull()
	    ; e = e.nextSibling().toElement() ) {

//		msgDbg( "parse(): interpreting top element \""
//			+ BufQString( e.tagName() ).toBuffer()
//			+ "\"..." );

		if ( e.tagName() == "class" ) {
//			msgDbg( "parse(): found the class name!" );
//			oRootElt.appendChild( oDoc.createComment(
//				"VMP UI generated for (old) class "
//				+ e.text() ) );
		}

		else if ( e.tagName() == "widget" ) {
//			msgDbg( "parse(): found a widget!" );
			if ( ! widget.isNull() ) {
				msgWrn( "parse(): duplicate widget!!!" );
			}
			else {
				widget = e;
			}
		}

		else if ( e.tagName() == "tabstops" ) {
//			msgDbg( "parse(): found TAB order settings!" );
			if ( ! tabOrder.isNull() ) {
				msgWrn( "parse(): duplicate tab order!!!" );
			}
			else {
				tabOrder = e;
			}
		}

	}

	if ( widget.isNull() ) {
		msgWrn( "parse(): didn't found any widget definition!!!" );
		return oDoc;
	}

	if ( widget.attribute( "class" ) != "QWidget" ) {
		msgWrn( "parse(): top-level widget is not a QWidget!" );
		return oDoc;
	}

	// Now, parse this top-level widget.

	oRootElt.appendChild( parseContainer( oDoc, widget, "HiddenFrame", "" ) );

// FIXME: do something with tab order too!

	return oDoc;

}

QDomElement Tools::Qui2Vui::parseContainer(
		QDomDocument	oDoc,
		QDomElement	iElt,
	const	Buffer&		vmpTagName,
	const	Buffer&		vmpProperties ) {

	// What we should find here:
	//	<property>
	//	<vbox> or <hbox> or <grid>

	BufferArray props = BufferParser::tokenize( vmpProperties, ", " );

	QDomElement oElt = oDoc.createElement( BufQString( vmpTagName ) );

//	msgDbg( "parseContainer(): --->" );
//	msgDbg( "parseContainer(): parsing subelements..." );

	for ( QDomElement n = iElt.firstChild().toElement()
	    ; ! n.isNull()
	    ; n = n.nextSibling().toElement() ) {

		QString tagName = n.tagName();

		if ( tagName == "property" ) {
			QString a = n.attribute( "name" );
			if ( a == "name" ) {
				QDomAttr attr = oDoc.createAttribute( "name" );
				attr.setValue( n.text() );
				oElt.setAttributeNode( attr );
			}
			else if ( props.contains( BufQString( a ).toBuffer() ) ) {
				QDomElement propElt = oDoc.createElement( "Property" );
				oElt.appendChild( propElt );
				QDomAttr propVal = oDoc.createAttribute( a );
				propVal.setValue( n.text() );
				propElt.setAttributeNode( propVal );
			}
			else if ( a != "geometry" && a != "caption" ) {
				msgWrn( "parseContainer(): ignoring property: "
					+ BufQString( a ).toBuffer() );
			}
			continue;
		}

		if ( tagName == "vbox" ) {
			QDomAttr attr = oDoc.createAttribute( "orient" );
			attr.setValue( "vertical" );
			oElt.setAttributeNode( attr );
		}

		else if ( tagName == "hbox" ) {
			QDomAttr attr = oDoc.createAttribute( "orient" );
			attr.setValue( "horizontal" );
			oElt.setAttributeNode( attr );
		}

		else if ( tagName == "grid" ) {
			msgWrn( "grid layout not supported!" );
			msgWrn( "Using a vertical layout instead!" );
			QDomAttr attr = oDoc.createAttribute( "orient" );
			attr.setValue( "vertical" );
			oElt.setAttributeNode( attr );
		}

		else {
			msgWrn( "Ignoring child \""
				+ BufQString( tagName ).toBuffer()
				+ "\" of element \""
				+ BufQString( iElt.tagName() ).toBuffer()
				+ "\"!!!" );
			continue;
		}

		parseBox( oDoc, oElt, n );

	}

//	msgDbg( "parseContainer(): <---" );

	return oElt;

}

void Tools::Qui2Vui::parseBox(
		QDomDocument	oDoc,
		QDomElement	oElt,
		QDomElement	iElt ) {

	// What we should find here:
	//	<property>*
	//	<widget>*
	//	<spacer>*

//	msgDbg( "parseBox(): --->" );
//	msgDbg( "parseBox(): parsing subelements..." );

	for ( QDomElement n = iElt.firstChild().toElement()
	    ; ! n.isNull()
	    ; n = n.nextSibling().toElement() ) {

		QString tagName = n.tagName();

		if ( tagName == "property" ) {
			QString a = n.attribute( "name" );
			if ( a != "name" && a != "spacing" && a != "margin" ) {
				msgWrn( "parseBox(): ignoring property: "
					+ BufQString( a ).toBuffer() );
			}
			continue;
		}

		if ( tagName == "spacer" ) {
			QDomElement s = oDoc.createElement( "Spacer" );
			oElt.appendChild( s );
			continue;
		}

		if ( tagName != "widget" ) {
			msgWrn( "parseBox(): ignoring child \""
				+ BufQString( tagName ).toBuffer()
				+ "\" of boxed element \""
				+ BufQString( oElt.tagName() ).toBuffer()
				+ "\"!!!" );
			continue;
		}

		QString className = n.attribute( "class" );

		QDomElement subElt;

		if ( className == "QLayoutWidget" )
			subElt = parseContainer(
				oDoc, n,
				"HiddenFrame",
				"" );

		else if ( className == "QCheckBox" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"CheckBox",
				"text,checked",
				"name" );

		else if ( className == "QDial" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"Dial",
				"value,minValue,maxValue,"
				"lineStep,pageStep,"
				"tickInterval,tickmarks"
				"notchSize,notchTarget,notchesVisible",
				"name" );

		else if ( className == "QLCDNumber" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"LCDNumber",
				"value,smallDecimalPoint,numDigits,mode",
				"name" );

		else if ( className == "QLabel" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"TextLabel",
				"text,alignment",
				"name" );

		else if ( className == "QLineEdit" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"LineEdit",
				"text,alignment",
				"name" );

		else if ( className == "QListView" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"ListView",
				"",
				"name" );

		else if ( className == "QProgressBar" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"ProgressBar",
				"centerIndicator,percentageVisible,"
				"progress,totalSteps",
				"name" );

		else if ( className == "QPushButton" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"PushButton",
				"text",
				"name" );

		else if ( className == "QSlider" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"Slider",
				"orientation,"
				"value,minValue,maxValue,"
				"lineStep,pageStep,"
				"tickInterval,tickmarks",
				"name,orientation" );

		else if ( className == "QSpinBox" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"SpinBox",
				"value,minValue,maxValue,lineStep",
				"name" );

		else if ( className == "QTextEdit" )
			subElt = parseSimpleWidget(
				oDoc, n,
				"TextEdit",
				"text",
				"name" );

		// === ITEMS CONTAINERS ===

		else if ( className == "QComboBox" )
			subElt = parseComboBox(
				oDoc, n );

		else if ( className == "QButtonGroup" )
			subElt = parseButtonGroup(
				oDoc, n );

		// === WIDGETS CONTAINERS ===

		else if ( className == "QGroupBox" )
			subElt = parseContainer(
				oDoc, n,
				"TitledFrame",
				"title" );

		// === NOT SUPPORTED ===

//		else if ( className == "QFrame" ) subElt = parseFrame( oDoc, n );
//		else if ( className == "QTextBrowser" ) subElt = parseTextBrowser( oDoc, n );
//		else if ( className == "QTabWidget" ) subElt = parseTabWidget( oDoc, n );

//		else if ( className == "QListBox" ) subElt = parseListBox( oDoc, n );
//		else if ( className == "QRadioButton" ) subElt = parseRadioButton( oDoc, n );

		if ( subElt.isNull() ) {
			msgWrn( "parseBox(): UNSUPPORTED WIDGET: child \""
				+ BufQString( className ).toBuffer()
				+ "\" of parent \""
				+ BufQString( oElt.attribute( "name" ) ).toBuffer()
				+ "\"!" );
		}
		else {
			oElt.appendChild( subElt );
		}

	}

//	msgDbg( "parseBox(): <---" );

}

// ============================================================================

QDomElement Tools::Qui2Vui::parseSimpleWidget(
		QDomDocument		oDoc,
		QDomElement		iElt,
	const	Buffer&		tagName,
	const	Buffer&		properties,
	const	Buffer&		attributes ) {

	QDomElement oElt = oDoc.createElement( BufQString( tagName ) );

	BufferArray props = BufferParser::tokenize( properties, ", " );
	BufferArray attrs = BufferParser::tokenize( attributes, ", " );

	props += "enabled";

	for ( QDomElement n = iElt.firstChild().toElement()
	    ; ! n.isNull()
	    ; n = n.nextSibling().toElement() ) {

		if ( n.tagName() != "property" ) {
			msgWrn( "parseSimpleWidget("
				+ tagName
				+ "): ignoring child \""
				+ BufQString( n.tagName() ).toBuffer()
				+ "\" of element \""
				+ BufQString( iElt.tagName() ).toBuffer()
				+ "\"!!!" );
			continue;
		}

		QString a = n.attribute( "name" );

		if ( attrs.contains( BufQString( a ).toBuffer() ) ) {
			QDomAttr attr = oDoc.createAttribute( a );
			attr.setValue( n.text() );
			oElt.setAttributeNode( attr );
		}
		else if ( props.contains( BufQString( a ).toBuffer() ) ) {
			QDomElement propElt = oDoc.createElement( "Property" );
			oElt.appendChild( propElt );
			QDomAttr propVal = oDoc.createAttribute( a );
			propVal.setValue( n.text() );
			propElt.setAttributeNode( propVal );
		}
		else if ( a != "sizePolicy" ) {
			msgWrn( "parseSimpleWidget("
				+ tagName
				+ "): ignoring property: "
				+ BufQString( a ).toBuffer() );
		}

	}

	return oElt;

}

// ============================================================================

QDomElement Tools::Qui2Vui::parseComboBox(
		QDomDocument		oDoc,
		QDomElement		iElt ) {

	QDomElement oElt = oDoc.createElement( "ComboBox" );

	for ( QDomElement n = iElt.firstChild().toElement()
	    ; ! n.isNull()
	    ; n = n.nextSibling().toElement() ) {

		if ( n.tagName() == "property" ) {
			QString a = n.attribute( "name" );
			if ( a == "name" ) {
				QDomAttr attr = oDoc.createAttribute( "name" );
				attr.setValue( n.text() );
				oElt.setAttributeNode( attr );
			}
			else if ( a == "enabled" ) {
				QDomElement propElt = oDoc.createElement( "Property" );
				oElt.appendChild( propElt );
				QDomAttr propVal = oDoc.createAttribute( "enabled" );
				propVal.setValue( n.text() );
				propElt.setAttributeNode( propVal );
			}
			else if ( a != "sizePolicy" ) {
				msgWrn( "parseComboBox(): ignoring property: "
					+ BufQString( a ).toBuffer() );
			}
		}

		else if ( n.tagName() == "item" ) {
			for ( QDomElement m = n.firstChild().toElement()
			    ; ! m.isNull()
			    ; m = m.nextSibling().toElement() ) {
				if ( m.tagName() != "property" ) {
					msgWrn( "parseComboBox(): ignoring item child!" );
					continue;
				}
				if ( m.attribute( "name" ) != "text" ) {
					msgWrn( "parseComboBox(): ignoring non-text item!" );
					continue;
				}
				QDomElement s = oDoc.createElement( "Item" );
				oElt.appendChild( s );
				QDomAttr a = oDoc.createAttribute( "text" );
				a.setValue( m.text() );
				s.setAttributeNode( a );
				
			}
		}

		else {
			msgWrn( "parseComboBox(): ignoring child \""
				+ BufQString( n.tagName() ).toBuffer()
				+ "\" of element \""
				+ BufQString( iElt.tagName() ).toBuffer()
				+ "\"!!!" );
		}

	}

	return oElt;

}

QDomElement Tools::Qui2Vui::parseButtonGroup(
		QDomDocument		oDoc,
		QDomElement		iElt ) {

	QDomElement	oElt; // could be RadioButtons or CheckBoxes
	QDomAttr	oNameAttr;
	QDomElement	oTitleElt;
	QDomElement	oCheckedElt;
	QDomAttr	oOrientAttr;
	QDomElement	iBox;
	QDomElement	n;

	for ( n = iElt.firstChild().toElement()
	    ; ! n.isNull()
	    ; n = n.nextSibling().toElement() ) {

		QString tagName = n.tagName();

		if ( tagName == "property" ) {
			QString a = n.attribute( "name" );
			if ( a == "name" ) {
				oNameAttr = oDoc.createAttribute( "name" );
				oNameAttr.setValue( n.text() );
			}
			else if ( a == "title" ) {
				oTitleElt = oDoc.createElement( "Property" );
				QDomAttr propVal = oDoc.createAttribute( a );
				propVal.setValue( n.text() );
				oTitleElt.setAttributeNode( propVal );
			}
			else {
				msgWrn( "parseContainer(): ignoring property: "
					+ BufQString( a ).toBuffer() );
			}
			continue;
		}

		if ( tagName == "vbox" ) {
			oOrientAttr = oDoc.createAttribute( "orient" );
			oOrientAttr.setValue( "vertical" );
		}

		else if ( tagName == "hbox" ) {
			oOrientAttr = oDoc.createAttribute( "orient" );
			oOrientAttr.setValue( "horizontal" );
		}

		else if ( tagName == "grid" ) {
			msgWrn( "parseButtonGroup(): grid layout not supported!" );
			oOrientAttr = oDoc.createAttribute( "orient" );
			oOrientAttr.setValue( "vertical" );
		}

		else {
			msgWrn( "Ignoring child \""
				+ BufQString( tagName ).toBuffer()
				+ "\" of element \""
				+ BufQString( iElt.tagName() ).toBuffer()
				+ "\"!!!" );
			continue;
		}

		if ( ! iBox.isNull() ) {
			msgWrn( "parseButtonGroup(): multiple boxes!!!" );
			continue;
		}

		iBox = n;

	}

	// Now, parse the vbox or hbox...

	if ( iBox.isNull() ) {
		msgWrn( "parseButtonGroup(): no box!!!" );
		return oElt;
	}

	bool isRadio = false;
	bool isCheck = false;

	int counter = -1;

	for ( n = iBox.firstChild().toElement()
	    ; ! n.isNull()
	    ; n = n.nextSibling().toElement() ) {

		QString tagName = n.tagName();

		if ( tagName == "property" ) {
			QString a = n.attribute( "name" );
			if ( a != "name" ) {
				msgWrn( "parseButtonGroup(): ignoring property: "
					+ BufQString( a ).toBuffer() );
			}
			continue;
		}

		if ( tagName != "widget" ) {
			msgWrn( "parseButtonGroup(): ignoring unrecognized \""
				+ BufQString( tagName ).toBuffer()
				+ "\" child of button group!!!" );
			continue;
		}

		counter++;

		QString className = n.attribute( "class" );

		if ( className == "QRadioButton" ) {
			if ( oElt.isNull() ) {
				oElt = oDoc.createElement( "RadioButtons" );
				isRadio = true;
			}
			else if ( ! isRadio ) {
				msgWrn( "parseButtonGroup(): found radio button in not radio button group!" );
				continue;
			}
		}
		else if ( className == "QCheckBox" ) {
			if ( oElt.isNull() ) {
				oElt = oDoc.createElement( "CheckBoxes" );
				isCheck = true;
			}
			else if ( ! isCheck ) {
				msgWrn( "parseButtonGroup(): found check box in not check box group!" );
				continue;
			}
		}
		else {
			msgWrn( "parseButtonGroup(): found incompatible element in button group!" );
			continue;
		}

		for ( QDomElement m = n.firstChild().toElement()
		    ; ! m.isNull()
		    ; m = m.nextSibling().toElement() ) {
			if ( m.tagName() != "property" ) {
				msgWrn( "parseButtonGroup(): unknown child to button!" );
				continue;
			}
			if ( m.attribute( "name" ) == "text" ) {
				QDomElement s = oDoc.createElement( "Item" );
				oElt.appendChild( s );
				QDomAttr a = oDoc.createAttribute( "text" );
				a.setValue( m.text() );
				s.setAttributeNode( a );
				continue;
			}
			if ( m.attribute( "name" ) == "checked" ) {
				if ( m.text() == "true" ) {
					oCheckedElt = oDoc.createElement( "Property" );
					QDomAttr propVal = oDoc.createAttribute( "checked" );
					propVal.setValue( QString::number( counter ) );
					oCheckedElt.setAttributeNode( propVal );
				}
				continue;
			}
		}

	}

	// Now assemble final DOM element...

	oElt.setAttributeNode( oNameAttr );
	oElt.setAttributeNode( oOrientAttr );
	oElt.appendChild( oTitleElt );
	oElt.appendChild( oCheckedElt );

	return oElt;

}

// ============================================================================

