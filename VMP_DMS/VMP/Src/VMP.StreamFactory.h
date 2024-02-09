// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.StreamFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_StreamFactory_H_
#define _VMP_StreamFactory_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL(VMP_Libs_Base_DLL,StreamFactory)

} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "VMP.Stream.h"
#include "VMP.StreamFactoryRegistry.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL StreamFactory : virtual public BFC::SObject {

protected :

	StreamFactory(
		const	BFC::DL::TypeCPtr &	type
	);

public :

	virtual ~StreamFactory(
	);

	virtual StreamPtr getNewStream(
	) = 0;

	const BFC::UUId& getUUId(
	) const;

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.StreamFactory");

	CLASS_EXCEPTION(SampleException, "Sample exception");

private :

	// internal state...

	BFC::UUId		uuid;

	StreamFactoryRegistryPtr	registry;

	// others...

	StreamFactory(
	);

	StreamFactory(
		const	StreamFactory&
	);

	StreamFactory& operator = (
		const	StreamFactory&
	);

};

// ============================================================================

template <class T>
class StreamFactoryTmpl : public StreamFactory {

public :

	StreamFactoryTmpl(
	) : StreamFactory( T::getClassType() ) {
	}

	virtual ~StreamFactoryTmpl(
	) {
	}

	virtual StreamPtr getNewStream(
	) {
		return new T;
	}

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_StreamFactory_H_

// ============================================================================

