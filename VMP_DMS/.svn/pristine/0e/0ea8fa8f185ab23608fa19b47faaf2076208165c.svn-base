// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.SessionSerializer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_SessionSerializer_H_
#define _VMP_SessionSerializer_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.Session.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL SessionSerializer {

public :

	static void doSerialize(
			BFC::IO::ByteOutputStreamPtr,
			VMP::SessionPtr
	);

	static SessionPtr unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_SessionSerializer_H_

// ============================================================================

