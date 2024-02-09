// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.SessionFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_SessionFactory_H_
#define _VMP_SessionFactory_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {

BFC_PTR_DECL(VMP_Libs_Base_DLL,SessionFactory)
BFC_PTR_DECL(VMP_Libs_Base_DLL,BytesSessionFactory)
BFC_PTR_DECL(VMP_Libs_Base_DLL,VideoSessionFactory)
BFC_PTR_DECL(VMP_Libs_Base_DLL,AudioSessionFactory)

} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

#include "VMP.Session.h"
#include "VMP.SessionFactoryRegistry.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL SessionFactory : virtual public BFC::SObject {

protected :

	SessionFactory(
		const	BFC::DL::TypeCPtr &	type
	);

public :

	virtual ~SessionFactory(
	);

	virtual SessionPtr getNewSession(
	) = 0;

	const BFC::UUId& getUUId(
	) const;

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.SessionFactory");

	CLASS_EXCEPTION(SampleException, "Sample exception");

private :

	// internal state...

	BFC::UUId		uuid;

	SessionFactoryRegistryPtr	registry;

	// others...

	SessionFactory(
	);

	SessionFactory(
		const	SessionFactory&
	);

	SessionFactory& operator = (
		const	SessionFactory&
	);

};

// ============================================================================

template <class T>
class SessionFactoryTmpl : public SessionFactory {

public :

	SessionFactoryTmpl(
	) : SessionFactory( T::getClassType() ) {
	}

	virtual ~SessionFactoryTmpl(
	) {
	}

	virtual SessionPtr getNewSession(
	) {
		return new T;
	}

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_SessionFactory_H_

// ============================================================================

