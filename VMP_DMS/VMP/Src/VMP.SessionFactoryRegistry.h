// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.SessionFactoryRegistry.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_SessionFactoryRegistry_H_
#define _VMP_SessionFactoryRegistry_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL(VMP_Libs_Base_DLL,SessionFactoryRegistry)

} // namespace VMP

// ============================================================================

#include "BFC.TL.Array.h"
#include "BFC.Debug.DObject.h"
#include "BFC.Base.Exception.h"
#include "BFC.MT.Mutex.h"

#include "VMP.Session.h"

// ============================================================================

namespace VMP {

// ============================================================================

class SessionFactory;

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL SessionFactoryRegistry : virtual public BFC::DObject {

protected :

	SessionFactoryRegistry(
	);

public :

	virtual ~SessionFactoryRegistry(
	);

	static SessionFactoryRegistryPtr instance(
	);

	SessionPtr getNewSession(
		const	BFC::UUId&		id
	);

	void addFactory(
			SessionFactory		*factory
	);

	void delFactory(
			SessionFactory		*factory
	);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.SessionFactoryRegistry");

	CLASS_EXCEPTION(NoSuchId, "No such UUID");

protected :

private :

	// internal state...

	BFC::CompArray< SessionFactory * >
				factories;
	BFC::MT::Mutex		listMutex;

	// others...

	SessionFactoryRegistry(
		const	SessionFactoryRegistry&
	);

	SessionFactoryRegistry& operator = (
		const	SessionFactoryRegistry&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_SessionFactoryRegistry_H_

// ============================================================================

