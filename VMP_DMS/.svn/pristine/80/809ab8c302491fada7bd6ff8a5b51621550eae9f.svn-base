// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.StreamSerializer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_StreamSerializer_H_
#define _VMP_StreamSerializer_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.Stream.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL StreamSerializer {

public :

	static void doSerialize(
			BFC::IO::ByteOutputStreamPtr,
			VMP::StreamPtr
	);

	static StreamPtr unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

private :

	/// \brief Forbidden default constructor.

	StreamSerializer(
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_StreamSerializer_H_

// ============================================================================

