// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.FrameFactoryRegistry.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_FrameFactoryRegistry_H_
#define _VMP_FrameFactoryRegistry_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL(VMP_Libs_Base_DLL,FrameFactoryRegistry)

} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Debug.DObject.h"

#include "BFC.MT.Mutex.h"

#include "BFC.TL.Array.h"

#include "VMP.Frame.h"

// ============================================================================

namespace VMP {

// ============================================================================

class FrameFactory;

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL FrameFactoryRegistry : virtual public BFC::DObject {

protected :

	FrameFactoryRegistry(
	);

public :

	virtual ~FrameFactoryRegistry(
	);

	static FrameFactoryRegistryPtr instance(
	);

	FramePtr getNewFrame(
		const	BFC::UUId&		id
	);

	void addFactory(
			FrameFactory		*factory
	);

	void delFactory(
			FrameFactory		*factory
	);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.FrameFactoryRegistry");

	CLASS_EXCEPTION(NoSuchId, "No such UUID");

protected :

private :

	// internal state...

	BFC::CompArray<FrameFactory *>	factories;
	BFC::MT::Mutex			listMutex;

	// others...

	FrameFactoryRegistry(
		const	FrameFactoryRegistry&
	);

	FrameFactoryRegistry& operator = (
		const	FrameFactoryRegistry&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_FrameFactoryRegistry_H_

// ============================================================================

