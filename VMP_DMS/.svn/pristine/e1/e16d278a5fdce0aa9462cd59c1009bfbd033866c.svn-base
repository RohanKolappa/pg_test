// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.IndexFactoryRegistry.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_IndexFactoryRegistry_H_
#define _VMP_IndexFactoryRegistry_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL( VMP_Libs_Base_DLL, IndexFactoryRegistry )

} // namespace VMP

// ============================================================================

#include "BFC.TL.Array.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "VMP.Index.h"
#include "VMP.IndexFactory.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL IndexFactoryRegistry : virtual public BFC::SObject {

protected :

	IndexFactoryRegistry(
	);

public :

	virtual ~IndexFactoryRegistry(
	);

	static IndexFactoryRegistryPtr instance(
	);

	UnitsIndexPtr getNewIndex(
		const	BFC::UUId&		id
	) const;

	void addFactory(
			IndexFactoryPtr		factory
	);

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.IndexFactoryRegistry");

	CLASS_EXCEPTION(NoSuchId, "No such UUID");

protected :

private :

	// internal state...

	BFC::Array< IndexFactoryPtr >	factories;

	// others...

	IndexFactoryRegistry(
		const	IndexFactoryRegistry&
	);

	IndexFactoryRegistry& operator = (
		const	IndexFactoryRegistry&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_IndexFactoryRegistry_H_

// ============================================================================

