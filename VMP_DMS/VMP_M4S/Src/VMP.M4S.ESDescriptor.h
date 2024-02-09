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
//	VMP.M4S.ESDescriptor.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M4S_ESDescriptor_H_
#define _VMP_M4S_ESDescriptor_H_

// ============================================================================

#include "VMP.M4S.DLL.h"

// ============================================================================

#include "VMP.M4S.ESDescriptorPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "VMP.M4S.DecoderConfigDescriptorPtr.h"
#include "VMP.M4S.SLConfigDescriptorPtr.h"

// ============================================================================

namespace VMP {
namespace M4S {

// ============================================================================

/// \brief ISO/IEC 14496-1, Section 8.6.5.
///
/// The ES_Descriptor conveys all information related to a particular
/// elementary stream and has three major parts.
///
/// The first part consists of the ES_ID which is a unique reference to the
/// elementary stream within its name scope (see 8.7.2.4), a mechanism to
/// describe dependencies of elementary streams within the scope of the parent
/// object descriptor and an optional URL string. Dependencies and usage of
/// URLs are specified in 8.7.
///
/// The second part consists of the component descriptors which convey the
/// parameters and requirements of the elementary stream.
///
/// The third part is a set of optional extension descriptors that support the
/// inclusion of future extensions as well as the transport of private data in
/// a backward compatible way.
///
/// \ingroup VMP_M4S

class VMP_M4S_DLL ESDescriptor : virtual public BFC::DObject {

public :

	/// \brief Creates a new ESDescriptor.

	ESDescriptor(
	);

	DecoderConfigDescriptorPtr getDecoderConfigDescriptor(
	) const {
		return cnfgDesc;
	}

	void decode(
		const	BFC::Buffer &	pData
	);

	BFC::Buffer encode(
	) const;

	BFC::Buffer toBuffer(
	) const;

protected :

private :

	BFC::Buffer			data;

	BFC::Uint32			ES_id;
	DecoderConfigDescriptorPtr	cnfgDesc;
	SLConfigDescriptorPtr		slCfgDsc;

	/// \brief Forbidden copy constructor.

	ESDescriptor(
		const	ESDescriptor &
	);

	/// \brief Forbidden copy operator.

	ESDescriptor & operator = (
		const	ESDescriptor &
	);

};

// ============================================================================

} // namespace M4S
} // namespace VMP

// ============================================================================

#endif // _VMP_M4S_ESDescriptor_H_

// ============================================================================

