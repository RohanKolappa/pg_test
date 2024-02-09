// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.IndexFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_IndexFactory_H_
#define _VMP_IndexFactory_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL( VMP_Libs_Base_DLL, IndexFactory )

} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "VMP.Index.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL IndexFactory : virtual public BFC::SObject {

protected :

	IndexFactory(
		const	BFC::DL::TypeCPtr &	type
	);

public :

	virtual ~IndexFactory(
	);

	virtual UnitsIndexPtr getNewIndex(
	) = 0;

	const BFC::UUId& getUUId(
	) const;

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.IndexFactory");

	CLASS_EXCEPTION(SampleException, "Sample exception");

private :

	// internal state...

	BFC::DL::TypeCPtr		type;

	// others...

	IndexFactory(
	);

	IndexFactory(
		const	IndexFactory&
	);

	IndexFactory& operator = (
		const	IndexFactory&
	);

};

// ============================================================================

template < class T >
class IndexFactoryTmpl : public IndexFactory {

public :

	IndexFactoryTmpl(
	) : IndexFactory( T::getClassType() ) {
	}

	virtual ~IndexFactoryTmpl(
	) {
	}

	virtual UnitsIndexPtr getNewIndex(
	) {
		return new T;
	}

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_IndexFactory_H_

// ============================================================================

