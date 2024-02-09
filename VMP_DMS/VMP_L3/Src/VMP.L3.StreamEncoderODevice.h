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
//	VMP.L3.StreamEncoderODevice.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_StreamEncoderODevice_H_
#define _VMP_L3_StreamEncoderODevice_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, StreamEncoderODevice )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.IO.FinalDevice.h"

#include "BFC.TL.FIFO.h"

#include "VMP.BytesFrame.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L3

class VMP_L3_DLL StreamEncoderODevice : public BFC::IO::FinalDevice {

public :

	/// \brief Creates a new StreamEncoderODevice.

	StreamEncoderODevice(
	);

	virtual void open(
	);

	virtual void close(
	);

	/// \brief Returns true if there is an output frame ready.

	virtual BFC::Bool hasFrame(
	) const;

	/// \brief Returns the next available output Frame.
	///
	/// It is an error to call this method if there is no output frame ready.
	/// To check this condition, use the hasFrame() method.

	virtual FramePtr getFrame(
	);

	virtual void putBytes(
		const	BFC::Buffer &	pOutputBuffer
	);

	virtual BFC::Uint64 seek(
		const	BFC::Uint64	pOffset
	);

	virtual BFC::Uint64 tell(
	) const;

	virtual BFC::Uint64 length(
	) const;

	void putFrame(
			BytesFramePtr	pOutputFrame
	);

protected :

private :

	BFC::Uint64			curr;
	BFC::FIFO< FramePtr >		fifo;

	/// \brief Forbidden copy constructor.

	StreamEncoderODevice(
		const	StreamEncoderODevice &
	);

	/// \brief Forbidden copy operator.

	StreamEncoderODevice & operator = (
		const	StreamEncoderODevice &
	);

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_StreamEncoderODevice_H_

// ============================================================================

