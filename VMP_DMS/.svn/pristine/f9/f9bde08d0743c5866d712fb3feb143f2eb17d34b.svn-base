// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.FrameFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_FrameFactory_H_
#define _VMP_FrameFactory_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL(VMP_Libs_Base_DLL,FrameFactory)

} // namespace VMP

// ============================================================================

#include "BFC.Base.SObject.h"

#include "VMP.Frame.h"
#include "VMP.FrameFactoryRegistry.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL FrameFactory : virtual public BFC::SObject {

protected :

	FrameFactory(
		const	BFC::DL::TypeCPtr &	type
	);

public :

	virtual ~FrameFactory(
	);

	virtual FramePtr getNewFrame(
	) = 0;

	const BFC::UUId & getUUId(
	) const;

private :

	// internal state...

	BFC::UUId		uuid;

	FrameFactoryRegistryPtr	registry;

	// others...

	FrameFactory(
	);

	FrameFactory(
		const	FrameFactory&
	);

	FrameFactory& operator = (
		const	FrameFactory&
	);

};

// ============================================================================

template <class T>
class FrameFactoryTmpl : public FrameFactory {

public :

	FrameFactoryTmpl(
	) : FrameFactory( T::getClassType() ) {
	}

	virtual ~FrameFactoryTmpl(
	) {
	}

	virtual FramePtr getNewFrame(
	) {
		return new T;
	}

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_FrameFactory_H_

// ============================================================================

