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
//	VMP.AAC.AudioSpecificConfig.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AAC_AudioSpecificConfig_H_
#define _VMP_AAC_AudioSpecificConfig_H_

// ============================================================================

#include "VMP.AAC.DLL.h"

// ============================================================================

#include "VMP.AAC.AudioSpecificConfigPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "VMP.ConfigInfo.h"

#include "VMP.AAC.BitStreamPtr.h"

// ============================================================================

namespace VMP {
namespace AAC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AAC

class VMP_AAC_DLL AudioSpecificConfig : public ConfigInfo {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new AudioSpecificConfig.

	AudioSpecificConfig(
	);

	/// \brief Decodes Buffer \a pData, which has to be encoded as
	///	specified in ISO/IEC 14496-3, section 1.6.2.1.

	virtual void decode(
		const	BFC::Buffer &	pData
	);

	/// \brief Encoded this object according to the format specified in
	///	ISO/IEC 14496-3, section 1.6.2.1.

	virtual BFC::Buffer encode(
	) const;

	virtual BFC::Buffer toBuffer(
	) const;

	BFC::Uint32 getObjectType(
	) const {
		return objtType;
	}

	void setObjectType(
		const	BFC::Uint32	pObjtType
	) {
		objtType = pObjtType;
	}

	BFC::Uint32 getSamplingIndex(
	) const {
		return smplIndx;
	}

	BFC::Uint32 getSamplingFrequency(
	) const {
		return smplFreq;
	}

	void setSamplingFrequency(
		const	BFC::Uint32	pSmplFreq
	);

	BFC::Uint32 getChannelConfiguration(
	) const {
		return chanCnfg;
	}

	void setChannelConfiguration(
		const	BFC::Uint32	pChanCnfg
	) {
		chanCnfg = pChanCnfg;
	}

	static BFC::Uint32 getSamplingFrequency(
		const	BFC::Uint32	pSmplIndx
	);

	static BFC::Uint32 getSamplingIndex(
		const	BFC::Uint32	pSmplFreq
	);

protected :

	BFC::Uint32 getAudioObjectType(
			BitStreamPtr	pIBitStrm
	);

private :

	BFC::Buffer	dataCopy;

	BFC::Uint32	objtType;
	BFC::Uint32	smplIndx;
	BFC::Uint32	smplFreq;
	BFC::Uint32	chanCnfg;

	static const BFC::Uint32 cSmplTbl[ 16 ];

	/// \brief Forbidden copy constructor.

	AudioSpecificConfig(
		const	AudioSpecificConfig &
	);

	/// \brief Forbidden copy operator.

	AudioSpecificConfig & operator = (
		const	AudioSpecificConfig &
	);

};

// ============================================================================

} // namespace AAC
} // namespace VMP

// ============================================================================

#endif // _VMP_AAC_AudioSpecificConfig_H_

// ============================================================================

