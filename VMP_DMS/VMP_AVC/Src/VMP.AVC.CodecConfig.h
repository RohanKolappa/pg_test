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
//	VMP.AVC.CodecConfig.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_CodecConfig_H_
#define _VMP_AVC_CodecConfig_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.CodecConfigPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.TL.Array.h"

#include "VMP.ConfigInfo.h"

#include "VMP.AVC.NALUPtr.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief Provides decoder configuration data to initialize the decoder.
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL CodecConfig : public ConfigInfo {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new CodecConfig.

	CodecConfig(
	);

	BFC::Uchar getProfileIndication(
	) const {
		return prfIndic;
	}

	BFC::Uchar getProfileCompatibility(
	) const {
		return prfCompt;
	}

	BFC::Uchar getLevelIndication(
	) const {
		return lvlIndic;
	}

	void setProfileIndication(
		const	BFC::Uchar	pCode
	);

	void setProfileCompatibility(
		const	BFC::Uchar	pCode
	);

	void setLevelIndication(
		const	BFC::Uchar	pCode
	);

	BFC::Bool hasSPS(
			NALUCPtr	pSPS
	) const;

	BFC::Uint32 getNbrSPSs(
	) const {
		return spsArray.getSize();
	}

	const BFC::Array< NALUCPtr > & getSPS(
	) const {
		return spsArray;
	}

	NALUCPtr getSPS(
		const	BFC::Uint32	pIndex
	) const {
		return spsArray[ pIndex ];
	}

	void addSPS(
			NALUCPtr	pSPS
	);

	BFC::Bool hasPPS(
			NALUCPtr	pPPS
	) const;

	BFC::Uint32 getNbrPPSs(
	) const {
		return ppsArray.getSize();
	}

	const BFC::Array< NALUCPtr > & getPPS(
	) const {
		return ppsArray;
	}

	NALUCPtr getPPS(
		const	BFC::Uint32	pIndex
	) const {
		return ppsArray[ pIndex ];
	}

	void addPPS(
			NALUCPtr	pPPS
	);

	BFC::Bool isInitialized(
	) const;

	/// \brief Decodes Buffer \a pData, which has to be encoded as
	///	specified in ISO/IEC 14496-15, section 5.2.4.1.1.

	virtual void decode(
		const	BFC::Buffer &	pData
	);

	/// \brief Encoded this object according to the format specified in
	///	ISO/IEC 14496-15, section 5.2.4.1.1.

	virtual BFC::Buffer encode(
	) const;

	virtual BFC::Buffer toBuffer(
	) const;

protected :

private :

	//	aligned(8) class AVCCodecConfig {
	//	    unsigned int(8) configurationVersion = 1;
	//	    unsigned int(8) AVCProfileIndication;
	//	    unsigned int(8) profile_compatibility;
	//	    unsigned int(8) AVCLevelIndication;
	//	    bit(6) reserved = ‘111111’b;
	//	    unsigned int(2) lengthSizeMinusOne;
	//	    bit(3) reserved = ‘111’b;
	//	    unsigned int(5) numOfSequenceParameterSets;
	//	    for (i=0; i< numOfSequenceParameterSets; i++) {
	//	        unsigned int(16) sequenceParameterSetLength ;
	//	        bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;
	//	    }
	//	    unsigned int(8) numOfPictureParameterSets;
	//	    for (i=0; i< numOfPictureParameterSets; i++) {
	//	        unsigned int(16) pictureParameterSetLength;
	//	        bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
	//	    }
	//	}

	BFC::Uchar		confVers;	///< The configuration version. Should be set to 1.
	BFC::Uchar		prfIndic;	///< The AVC profile indication (see profile code in 14496-10).
	BFC::Uchar		prfCompt;	///< The profile compatibility.
	BFC::Uchar		lvlIndic;	///< The AVC level indication (see level code in 14496-10).
	BFC::Uchar		lngthSze;	///< LengthSizeMinusOne.
	BFC::Array< NALUCPtr >	spsArray;	///< List of Sequence Parameter Set.
	BFC::Array< NALUCPtr >	ppsArray;	///< List of Picture Parameter Set.

	/// \brief Forbidden copy constructor.

	CodecConfig(
		const	CodecConfig &
	);

	/// \brief Forbidden copy operator.

	CodecConfig & operator = (
		const	CodecConfig &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_CodecConfig_H_

// ============================================================================

