// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.StreamFactoryRegistry.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_StreamFactoryRegistry_H_
#define _VMP_StreamFactoryRegistry_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL(VMP_Libs_Base_DLL,StreamFactoryRegistry)

} // namespace VMP

// ============================================================================

#include "BFC.TL.Array.h"
#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.MT.Mutex.h"

#include "VMP.Stream.h"

// ============================================================================

namespace VMP {

// ============================================================================

class StreamFactory;

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL StreamFactoryRegistry : virtual public BFC::DObject {

protected :

	StreamFactoryRegistry(
	);

public :

	virtual ~StreamFactoryRegistry(
	);

	static StreamFactoryRegistryPtr instance(
	);

	StreamPtr getNewStream(
		const	BFC::UUId&		id
	);

	void addFactory(
			StreamFactory		*factory
	);

	void delFactory(
			StreamFactory		*factory
	);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.StreamFactoryRegistry");

	CLASS_EXCEPTION(NoSuchId, "No such UUID");

protected :

private :

	// internal state...

	BFC::CompArray< StreamFactory * >
				factories;
	BFC::MT::Mutex		listMutex;

	// others...

	StreamFactoryRegistry(
		const	StreamFactoryRegistry&
	);

	StreamFactoryRegistry& operator = (
		const	StreamFactoryRegistry&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_StreamFactoryRegistry_H_

// ============================================================================

