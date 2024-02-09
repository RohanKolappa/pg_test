// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.L2.EngineRepository.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.DL.Event.h"
#include "BFC.DL.WrapperBuilder.h"

#include "VMP.L2.EngineFactory.h"
#include "VMP.L2.EngineRepository.h"

#include "VMP.L2.BytesDecoderEngine.h"
#include "VMP.L2.BytesEncoderEngine.h"
#include "VMP.L2.ConsumerEngine.h"
#include "VMP.L2.MultiProcessorEngine.h"
#include "VMP.L2.PictureDecoderEngine.h"
#include "VMP.L2.PictureEncoderEngine.h"
#include "VMP.L2.PictureProcessorEngine.h"
#include "VMP.L2.PicturesConsumerEngine.h"
#include "VMP.L2.PicturesProducerEngine.h"
#include "VMP.L2.ProcessorEngine.h"
#include "VMP.L2.ProducerEngine.h"
#include "VMP.L2.StreamDecoderEngine.h"
#include "VMP.L2.StreamEncoderEngine.h"

#include "VMP.L3.BytesDecoderBackend.h"
#include "VMP.L3.BytesEncoderBackend.h"
#include "VMP.L3.ConsumerBackend.h"
#include "VMP.L3.MultiProcessorBackend.h"
#include "VMP.L3.PictureDecoderBackend.h"
#include "VMP.L3.PictureEncoderBackend.h"
#include "VMP.L3.PictureProcessorBackend.h"
#include "VMP.L3.PicturesConsumerBackend.h"
#include "VMP.L3.PicturesProducerBackend.h"
#include "VMP.L3.ProcessorBackend.h"
#include "VMP.L3.ProducerBackend.h"
#include "VMP.L3.StreamDecoderBackend.h"
#include "VMP.L3.StreamEncoderBackend.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, EngineRepository )

// ============================================================================

L2::EngineRepository::EngineRepository() :

	Repository( UUId( "5364ab2a-c529-4b47-804b-f1fed9dcc4bf" ) ) {

	setObjectName( "VMP.L2.EngineRepository" );

//	msgDbg( "EngineRepository(): --->" );

	addCallback( this, &EngineRepository::observerCallback );

	l3rep = L3::BackendRepository::instance();

	l3rep->addObserver( this );

//	msgDbg( "EngineRepository(): <---" );

}

L2::EngineRepository::~EngineRepository() {

//	msgDbg( "~EngineRepository(): --->" );

	l3rep->delObserver( this );

//	msgDbg( "~EngineRepository(): <---" );

}

// ============================================================================

L2::EngineRepositoryPtr L2::EngineRepository::instance() {

	static EngineRepositoryPtr i = new EngineRepository;

	return i;

}

// ============================================================================

L2::EngineFactoryPtr L2::EngineRepository::getEngineFactory(
	const	UUId &		pUUId ) const {

	return getFactoryFor( pUUId );

}

// ============================================================================

void L2::EngineRepository::observerCallback(
		Msg::EventCPtr		pEvent ) {

	if ( pEvent.isA< DL::FactoryAddedEvent >() ) {
		DL::FactoryAddedEventCPtr e = pEvent;
		addWrapper( e->getFactory() );
		return;
	}

	if ( pEvent.isA< DL::FactoryDeletedEvent >() ) {
		DL::FactoryDeletedEventCPtr e = pEvent;
		delWrapper( e->getFactory() );
		return;
	}

}

// ============================================================================

void L2::EngineRepository::addWrapper(
		DL::FactoryPtr		pBackendFact ) {

	DL::BuilderPtr		backendBldr = pBackendFact->getBuilder();
	DL::TypeCPtr		backendType = pBackendFact->getType();
	DL::BuilderPtr		engineBldr;

	if ( backendType->inherits( L3::BytesDecoderBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::BytesDecoderEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::BytesEncoderBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::BytesEncoderEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::ConsumerBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::ConsumerEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::MultiProcessorBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::MultiProcessorEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::PictureDecoderBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::PictureDecoderEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::PictureEncoderBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::PictureEncoderEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::PictureProcessorBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::PictureProcessorEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::PicturesConsumerBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::PicturesConsumerEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::PicturesProducerBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::PicturesProducerEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::ProcessorBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::ProcessorEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::ProducerBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::ProducerEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::StreamDecoderBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::StreamDecoderEngine >( backendBldr );
	}
	else if ( backendType->inherits( L3::StreamEncoderBackend::getClassType() ) ) {
		engineBldr = new DL::WrapperBuilder< L2::StreamEncoderEngine >( backendBldr );
	}

	if ( engineBldr ) {
		DL::FactoryPtr engineFact = new EngineFactory( engineBldr );
		addFactory( engineFact, Buffer() );
		wrapped += WrappedPair( pBackendFact, engineFact );
	}

}

void L2::EngineRepository::delWrapper(
		DL::FactoryPtr		pBackendFact ) {

	for ( Uint32 i = 0 ; i < wrapped.size() ; i++ ) {
		if ( wrapped[ i ].left() == pBackendFact ) {
			delFactory( wrapped[ i ].right(), Buffer() );
			wrapped.pop( i );
		}
	}

}

// ============================================================================

