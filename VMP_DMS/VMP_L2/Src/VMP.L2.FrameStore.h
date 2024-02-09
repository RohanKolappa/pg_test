// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.FrameStore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_FrameStore_H_
#define _VMP_L2_FrameStore_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L2 {

BFC_PTR_DECL( VMP_L2_DLL, FrameStore )

} // namespace L2
} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.TL.Array.h"
#include "VMP.Frame.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Simple frame store.
///
/// \ingroup VMP_L2

class VMP_L2_DLL FrameStore : virtual public BFC::SObject {

public :

	FrameStore(
		const	BFC::Uint32	maxSize = 250
	);

	virtual ~FrameStore(
	);

	BFC::Bool hasFrame(
		const	BFC::Uint64	index
	);

	FramePtr getFrame(
		const	BFC::Uint64	index
	);

	void addFrame(
		const	BFC::Uint64	index,
			FramePtr	data
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.L2.FrameStore" );

	CLASS_EXCEPTION( NoSuchFrame, "No such frame" );

protected :

private :

	BFC::Array< FramePtr >		ring;
	BFC::Uint64			head;	// oldest element in ring
	BFC::Uint32			size;	// ring size

	// others...

	FrameStore(
		const	FrameStore&
	);

	FrameStore& operator = (
		const	FrameStore&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_FrameStore_H_

// ============================================================================

