// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.TProgramCSVWriterEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TProgramCSVWriterEngine_H_
#define _VMP_RTP_TProgramCSVWriterEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TProgramCSVWriterEnginePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.IO.FilePtr.h"

#include "BFC.TL.Array.h"

#include "BFC.Time.Clock.h"

#include "VMP.L2.FinalPushEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TProgramCSVWriterEngine : public L2::FinalPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TProgramCSVWriterEngine.

	TProgramCSVWriterEngine(
	);

	/// \brief Creates a new TProgramCSVWriterEngine.

	TProgramCSVWriterEngine(
		const	BFC::Buffer &	pFileName
	);

	void init(
		const	BFC::Buffer &	pFileName
	);

	virtual void putSessionCallback(
			SessionPtr	pOutpSess
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pOutpFrme
	);

protected :

private :

	BFC::Buffer		fileName;
	BFC::IO::FilePtr	outpFile;
	BFC::Uint64		currLine;
	struct Entry {
		Entry() : initDone( false ) {}
		BFC::Bool		initDone;
		BFC::Time::Clock	prevRead;
		BFC::Uint16		prevSqNo;
		BFC::Uint64		prevStmp;
		BFC::Uint32		prevSSRC;
	};
	BFC::Array< Entry >	entrList;

	/// \brief Forbidden copy constructor.

	TProgramCSVWriterEngine(
		const	TProgramCSVWriterEngine &
	);

	/// \brief Forbidden copy operator.

	TProgramCSVWriterEngine & operator = (
		const	TProgramCSVWriterEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TProgramCSVWriterEngine_H_

// ============================================================================

