// ============================================================================
// 
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
// Filename:
//	BFC.Msg.Dispatcher.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Msg_Dispatcher_H_
#define _BFC_Msg_Dispatcher_H_

// ============================================================================

#include "BFC.Msg.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Msg {

BFC_PTR_DECL( BFC_Msg_DLL, Dispatcher )

} // namespace Msg
} // namespace BFC

// ============================================================================

#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"

#include "BFC.STL.SHash.h"

#include "BFC.TL.Array.h"

#include "BFC.Msg.Consumer.h"
#include "BFC.Msg.Event.h"
#include "BFC.Msg.Producer.h"

// ============================================================================

namespace BFC {
namespace Msg {

// ============================================================================

/// \brief Dispatches events between Producer objects and Consumer objects.
///
/// \ingroup BFC_Msg

class BFC_Msg_DLL Dispatcher : virtual public SObject {

protected :

	/// \brief Creates a new Dispatcher.

	Dispatcher(
	);

public :

	/// \brief Returns the unique instance of this class.

	static DispatcherPtr instance(
	);

	/// \brief Establishes a connection between a producer and a consumer.

	void connect(
			ProducerPtr		pProducer,
		const	Uint32			prodServId,
			ConsumerPtr		pConsumer,
		const	Uint32			pConsServId
	);

protected :

	friend class Consumer;
	friend class Producer;

	Uint32 addProducer(
			ProducerPtr		pProducer
	);

	void delProducer(
		const	Uint32			pProducerId
	);

	Uint32 addConsumer(
			ConsumerPtr		pConsumer
	);

	void delConsumer(
		const	Uint32			pConsumerId
	);

	// To break an existing connection...

	void disconnectProducer(
		const	Uint32			pProducerId
	);

	void disconnectConsumer(
		const	Uint32			pConsumerId
	);

	/// \brief Dispatches a new Event to associated consumers.

	void dispatch(
		const	Uint32			pProdUniqId,
		const	Uint32			pProdServId,
		const	EventPtr &		pEvent
	);

private :

//	void processPendingEvents(
//	);

	/// \brief Represents a connection between a producer[prodServ] and
	///	a consumer[consServ].

	struct AssocTableEntry {
		AssocTableEntry(
		) {
		}
		AssocTableEntry(
			const	Uint32		pProdUniqId,
			const	Uint32		pProdServId,
			const	Uint32		pConsUniqId,
			const	Uint32		pConsServId
		) :
			prodUniqId( pProdUniqId ),
			prodServId( pProdServId ),
			consUniqId( pConsUniqId ),
			consServId( pConsServId ) {
		}
		Uint32		prodUniqId;
		Uint32		prodServId;
		Uint32		consUniqId;
		Uint32		consServId;
	};

	/// \brief Represents an Event that should still be sent to some
	///	consumer[consServ].

	struct PendingTableEntry {
		PendingTableEntry(
		) {
		}
		PendingTableEntry(
			const	Uint32		pConsUniqId,
			const	Uint32		pConsServId,
			const	EventPtr &	pProducerEv
		) :
			consUniqId( pConsUniqId ),
			consServId( pConsServId ),
			producerEv( pProducerEv ) {
		}
		Uint32		consUniqId;
		Uint32		consServId;
		EventPtr	producerEv;
	};

	typedef Array< AssocTableEntry >	AssocTable;
	typedef Array< PendingTableEntry >	PendingTable;

	FastCS			prodConsCS;	///< Hash tables protector.
	Uint32			prodConsId;	///< Unique id counter.
	STL::SHash		prodSHash;	///< Hash table of producers.
	STL::SHash		consSHash;	///< Hash table of consumers.

	FastCS			assocMutex;	///< Array protector.
	AssocTable		assocTable;	///< Array of associations.

	FastCS			pendingMutex;	///< Array protector.
	PendingTable		pendingTable;	///< Array of pending events.

	/// \brief Forbidden copy constructor.

	Dispatcher(
		const	Dispatcher &
	);

	/// \brief Forbidden copy operator.

	Dispatcher & operator = (
		const	Dispatcher &
	);

};

// ============================================================================

} // namespace Msg
} // namespace BFC

// ============================================================================

#endif // _BFC_Msg_Dispatcher_H_

// ============================================================================

