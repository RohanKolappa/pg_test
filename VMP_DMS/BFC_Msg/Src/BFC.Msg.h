// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Msg".
// 
// "BFC::Msg" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Msg" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Msg"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// FileName:
//	BFC.Msg.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_Msg_H_
#define _BFC_Msg_H_

// ============================================================================

#include "BFC.Msg.Consumer.h"
#include "BFC.Msg.Dispatcher.h"
#include "BFC.Msg.Event.h"
#include "BFC.Msg.Funcs.h"
#include "BFC.Msg.Observable.h"
#include "BFC.Msg.Observer.h"
#include "BFC.Msg.Producer.h"

// ============================================================================

/// \mainpage
///
/// \section intro Introduction
///
///	\image html BFC_Msg_Class_Model.png
///
/// \section example Example
///
/// \subsection MyObservable
///	
/// \code
///	#include "BFC.Msg.Observable.h"
///	
///	using namespace BFC;
///	
///	class MyObservable : public Msg::Observable {
///	
///	public :
///	
///		void genEvent(
///		) {
///			warnObservers( new Msg::Event );
///		}
///	
///	};
/// \endcode
///
/// \subsection MyObserver
///
/// \code
///	#include <iostream>
///	#include "BFC.Msg.Observer.h"
///	
///	using namespace std;
///	using namespace BFC;
///	
///	class MyObserver : public Msg::Observer {
///	
///	public :
///	
///		MyObserver(
///		) {
///			addCallback( this, &MyObserver::myCallback );
///		}
///	
///		virtual ~MyObserver(
///		) {
///			delCallbacks();
///		}
///	
///		void myCallback(
///				Msg::EventCPtr	/* event */
///		) {
///			cout << "Got EVENT!" << endl;
///		}
///	
///	};
/// \endcode
///
/// \subsection MyMain
///
/// \code
///	MyObservablePtr	observable = new MyObservable;
///	MyObserverPtr	observer = new MyObserver;
///
///	observable->genEvent();	// nothing happens...
///
///	observable->addObserver( observer );
///	observable->genEvent(); // observer got event!
///
///	observer.kill();
///
///	observable->genEvent();	// safe! and nothing happens anymore...
/// \endcode

// ============================================================================

#endif // _BFC_Msg_H_

// ============================================================================

