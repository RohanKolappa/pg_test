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
//	BFC.DL.Event.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Event_H_
#define _BFC_DL_Event_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace DL {

BFC_PTR_DECL( BFC_DL_DLL, RepositoryAddedEvent )
BFC_PTR_DECL( BFC_DL_DLL, RepositoryDeletedEvent )

BFC_PTR_DECL( BFC_DL_DLL, FactoryAddedEvent )
BFC_PTR_DECL( BFC_DL_DLL, FactoryDeletedEvent )

} // namespace DL
} // namespace BFC

// ============================================================================

#include "BFC.DL.DescrPtr.h"
#include "BFC.DL.FactoryPtr.h"
#include "BFC.DL.RepositoryPtr.h"

#include "BFC.Msg.Event.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL RepositoryAddedEvent : public Msg::Event {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new RepositoryAddedEvent.

	RepositoryAddedEvent(
	) :
		Msg::Event	( classUUId() ) {
	}

	/// \brief Creates a new RepositoryAddedEvent.

	RepositoryAddedEvent(
			RepositoryPtr		pRepository
	) :
		Msg::Event	( classUUId() ),
		repository	( pRepository ) {
	}

	virtual Buffer toBuffer(
	) const;

	RepositoryPtr getRepository(
	) const {
		return repository;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

protected :

private :

	RepositoryPtr	repository;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL RepositoryDeletedEvent : public Msg::Event {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new RepositoryDeletedEvent.

	RepositoryDeletedEvent(
	) :
		Msg::Event	( classUUId() ) {
	}

	/// \brief Creates a new RepositoryDeletedEvent.

	RepositoryDeletedEvent(
		const	UUId &		pUUId
	) :
		Msg::Event	( classUUId() ),
		uuid		( pUUId ) {
	}

	virtual Buffer toBuffer(
	) const;

	const UUId & getUUId(
	) const {
		return uuid;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

protected :

private :

	UUId	uuid;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL FactoryAddedEvent : public Msg::Event {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new FactoryAddedEvent.

	FactoryAddedEvent(
	);

	/// \brief Creates a new FactoryAddedEvent.

	FactoryAddedEvent(
			FactoryPtr	pFactory
	);

	virtual Buffer toBuffer(
	) const;

	FactoryPtr getFactory(
	) const {
		return fact;
	}

	DescrCPtr getDescr(
	) const {
		return desc;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

protected :

private :

	FactoryPtr	fact;
	DescrCPtr	desc;

};

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL FactoryDeletedEvent : public Msg::Event {

public :

	static const UUId & classUUId(
	);

	/// \brief Creates a new FactoryDeletedEvent.

	FactoryDeletedEvent(
	);

	/// \brief Creates a new FactoryDeletedEvent.

	FactoryDeletedEvent(
			FactoryPtr	pFactory
	);

	virtual Buffer toBuffer(
	) const;

	FactoryPtr getFactory(
	) const {
		return fact;
	}

	DescrCPtr getDescr(
	) const {
		return desc;
	}

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

protected :

private :

	FactoryPtr	fact;
	DescrCPtr	desc;

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Event_H_

// ============================================================================

