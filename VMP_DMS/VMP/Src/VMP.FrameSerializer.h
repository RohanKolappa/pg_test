// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.FrameSerializer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_FrameSerializer_H_
#define _VMP_FrameSerializer_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.Frame.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL FrameSerializer {

public :

	static void doSerialize(
			BFC::IO::ByteOutputStreamPtr,
			VMP::FramePtr
	);

	static FramePtr unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_FrameSerializer_H_

// ============================================================================

