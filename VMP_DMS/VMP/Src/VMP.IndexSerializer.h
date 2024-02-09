// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.IndexSerializer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_IndexSerializer_H_
#define _VMP_IndexSerializer_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.Index.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL IndexSerializer {

public :

	static void doSerialize(
			BFC::IO::ByteOutputStreamPtr,
			VMP::UnitsIndexPtr
	);

	static UnitsIndexPtr unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_IndexSerializer_H_

// ============================================================================

