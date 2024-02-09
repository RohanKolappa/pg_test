// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.AudioCompression.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AudioCompression_H_
#define _VMP_AudioCompression_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.FourCC.h"

#include "BFC.IO.Serializable.h"

// ============================================================================

namespace VMP {

// ============================================================================

class VMP_Libs_Base_DLL AudioCompression : virtual public BFC::IO::Serializable {

public :

	enum Format {

		// Special modes...

		Unknown		= 0,
		Uncompressed,

		// Known compression formats...

		MP3,		// MPEG-1 Layer 3

		WMA1,		// WMA1
		WMA2,		// WMA2

		VORBIS,		// Xiph VORBIS
		SPEEX,		// Xiph SPEEX
		FLAC,		// Free Lossless Audio Codec

		AAC,		// MPEG-2 Part 7 || MPEG-4 Part 3.

		MaxFormat,
		MinFormat	= Uncompressed

	};

	AudioCompression(
		const	Format		pFormat = Unknown
	) :
		f( pFormat ) {
	}

	AudioCompression(
		const	AudioCompression &
					pOther
	) :
		SObject(),
		Serializable(),
		f( pOther.f ) {
	}

	AudioCompression(
		const	BFC::FourCC &	pFourCC
	);

	AudioCompression & operator = (
		const	AudioCompression &
					pOther
	) {
		if ( this != &pOther ) {
			f = pOther.f;
		}
		return *this;
	}

	BFC::Bool operator == (const AudioCompression& pOther) const {return (f == pOther.f);}
	BFC::Bool operator != (const AudioCompression& pOther) const {return (f != pOther.f);}

	BFC::FourCC fourCC(
	) const;

	const char * name(
	) const {
		return nms[f];
	}

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

protected :

private :

	Format f;

	static struct ConvEntry{
		Format		format;
		const char	*fourCC;
	} tbl[];

	static const char *nms[];

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_AudioCompression_H_

// ============================================================================

