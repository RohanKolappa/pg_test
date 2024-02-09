// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.VideoCompression.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_VideoCompression_H_
#define _VMP_VideoCompression_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.FourCC.h"

#include "BFC.Dyn.TEnumPtr.h"

#include "BFC.IO.Serializable.h"

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace VMP {

// ============================================================================

class VMP_Libs_Base_DLL VideoCompression : virtual public BFC::IO::Serializable {

public :

	enum Format {

		// Special modes...

		Unknown		= 0,
		Uncompressed,

		// Known compression formats...

		AASC,	// 2 - line 45
		ABYR,
		ADV1,
		AEMI,
		AFLI,
		AFLC,
		AJPG,
		AMPG,
		ANIM,
		AP41,
		ASLC,
		ASV1,
		ASV2,
		ASVX,
		AUR2,
		AURA,
		AVC1,
		AVDJ,
		AVDV,
		AVRN,
		AVUI,
		AZPR,
		BBCD,
		BHIV,
		BINK,
		BLC2,
		BT20,
		BTCV,
		BW10,
		CDVC,
		CFCC,
		CGDI,
		CHAM,
		CJPG,
		CLJR,
		CMYK,
		COL0,
		COL1,
		CSCD,
		CVID,
		CWLT,
		D261,
		D263,
		DAVC,
		DCL1,
		DIRAC,
		DIVX,
		DM4V,
		DMB1,
		DMB2,
		DMK2,
		DSVD,
		DUCK,
		DV25,
		DV50,
		DVAN,
		DVCS,
		DVE2,
		DVH1,
		DVHD,
		DVPP,
		DVR,
		DV,
		DVSL,
		DVX1,
		DVX2,
		DVX3,
		MPEG4,
		DXGM,
		DXTN,
		DXTC,
		ELK0,
		EKQ0,
		EM2V,
		ES07,
		ESCP,
		ETV1,
		ETV2,
		ETVC,
		FFV1,
		FFVH,
		FLIC,
		FLJP,
		FLV1,
		FMP4,
		FPS1,
		FRWA,
		FRWD,
		FVF1,
		GEOX,
		GJPG,
		GLZW,
		GPEG,
		GWLT,
		GXVE,
		H261,
		H262,
		H263,
		H264,
		HDV2,
		HDV3,
		HUFFYUV,
		HMCR,
		HMRR,
		HVD2,
		H263I,
		IAN,
		ICLB,
		IGOR,
		IJPG,
		ILVC,
		ILVR,
		IPDV,
		IPJ2,
		IR21,
		ISME,
		IV31,
		IV32,
		IV41,
		IV50,
		JBYR,
		JFGO,
		JFIF,
		JPEG,
		JPGL,
		KMVC,
		L261,
		L263,
		LBYR,
		LCMW,
		LCW2,
		LEAD,
		LGRY,
		LJ2K,
		LOCO,
		LSVC,
		LSVM,
		LSVX,
		LZO1,
		M4CC,
		MC12,
		MCAM,
		MJ2C,
		MJLS,
		MJPA,
		MJPB,
		MJPEG,
		MMES,
		MP2A,
		MP2T,
		MP2V,
		MSMPEG4V2,
		MSMPEG4V3,
		MP4A,
		MP4T,
		MPEG1,
		MPEG2,
		MSMPEG4V1,
		MPGI,
		MPNG,
		MR16,
		MRCA,
		MRLE,
		MSS1,
		MSS2,
		MSVC,
		MSZH,
		MTGA,
		MTX1,
		MTX2,
		MTX3,
		MTX4,
		MTX5,
		MTX6,
		MTX7,
		MTX8,
		MTX9,
		MVI1,
		MVI2,
		MWV1,
		MX5P,
		NAVI,
		NHVU,
		NTN1,
		NTN2,
		NUV1,
		NVDS,
		NVHS,
		NVS0,
		NVS1,
		NVS2,
		NVS3,
		NVS4,
		NVS5,
		NVT0,
		NVT1,
		NVT2,
		NVT3,
		NVT4,
		NVT5,
		PDVC,
		PGVV,
		PHMO,
		PIMJ,
		PIXL,
		PJPG,
		PNG,
		PVEZ,
		PVMM,
		PVW2,
		Q1_0,
		Q1_1,
		QDRW,
		QPEG,
		QPEQ,
		RGBT,
		RJPG,
		RLE,
		RLE4,
		RLE8,
		RMP4,
		ROQV,
		RPZA,
		RT21,
		RTLL,
		RV10,
		RV13,
		RV20,
		RV30,
		RV40,
		RVX,
		S263,
		S422,
		SAN3,
		SDCC,
		SEDG,
		SFMC,
		SGI,
		SGI1,
		SMC,
		SMP4,
		SMSC,
		SMSD,
		SMSV,
		SNOW,
		SP40,
		SP44,
		SP53,
		SP54,
		SP55,
		SP56,
		SP57,
		SP58,
		SPIG,
		SQZ2,
		STVA,
		STVB,
		STVC,
		STVX,
		STVY,
		SV10,
		SVQ1,
		SVQ3,
		THEORA,
		TLMS,
		TLST,
		TM20,
		TM2X,
		TMIC,
		TMOT,
		TR20,
		TSCC,
		TV10,
		TVJP,
		TVMJ,
		TY2C,
		TY2N,
		U263,
		UCOD,
		ULTI,
		V261,
		VC1,
		VDCT,
		VDOM,
		VDOW,
		VDTZ,
		VGPX,
		VIDS,
		VIFP,
		VIV1,
		VIV2,
		VIVO,
		VIXL,
		VLV1,
		VMNC,
		VP30,
		VP31,
		VP40,
		VP50,
		VP60,
		VP61,
		VP62,
		VP70,
		VQC1,
		VQC2,
		VSSH,
		VSSV,
		VSSW,
		VTLP,
		VUUU,
		VX1K,
		VX2K,
		VXSP,
		WBVC,
		WINX,
		WJPG,
		WMV1,
		WMV2,
		WMV3,
		WMVA,
		WMVP,
		WNV1,
		WRLE,
		WV1F,
		WVP2,
		X261,
		X263,
		X264,
		XLV0,
		XMPG,
		XVID,
		XVIX,
		XWV3,
		XXAN,
		YUYV,
		YV92,
		ZLIB,
		ZMBV,
		ZPEG,
		ZYGO,	// 334

		MinFormat	= Uncompressed,
		MaxFormat	= ZYGO + 1

	};

	VideoCompression(
		const	Format		pf = Unknown
	) :
		f	( pf )
	{
	}

	VideoCompression(
		const	BFC::Uint32	pCode
	);

	explicit VideoCompression(
		const	BFC::FourCC &	pFourCC
	);

	VideoCompression(
		const	BFC::Buffer &	pName
	);

	VideoCompression(
		const	VideoCompression &
					pOther
	) :
		SObject		(),
		Serializable	(),
		f		( pOther.f )
	{
	}

	VideoCompression & operator = (
		const	VideoCompression &
					pOther
	) {
		if ( this != &pOther ) {
			f = pOther.f;
		}
		return *this;
	}

	BFC::Bool operator == (
		const	VideoCompression &
					pOther
	) const {
		return ( f == pOther.f );
	}

	BFC::Bool operator != (
		const	VideoCompression &
					pOther
	) const {
		return ( f != pOther.f );
	}

	BFC::FourCC getFourCC(
	) const {
		return tbl[ f ].getFourCC();
	}

	const BFC::Buffer & getName(
	) const {
		return tbl[ f ].getName();
	}

	const BFC::Buffer & getInfo(
	) const {
		return tbl[ f ].getInfo();
	}

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

	static BFC::Dyn::TEnumCPtr getTEnum(
		const	BFC::Bool	pUnknown
	);

protected :

	static BFC::Dyn::TEnumCPtr makeTEnum(
		const	BFC::Bool	pUnknown
	);

private :

	Format f;

	class ConvEntry{
	public :
		ConvEntry(
			const	char *		name,
			const	char *		fourCCList,
			const	char *		info
		);
		BFC::FourCC getFourCC(
		) const {
			return ( fourCC.isEmpty() ? BFC::FourCC() : fourCC[ 0 ] );
		}
		const BFC::Buffer & getName(
		) const {
			return name;
		}
		const BFC::Buffer & getInfo(
		) const {
			return info;
		}
		BFC::Bool contains(
			const	BFC::FourCC &	c
		) const {
			return ( fourCC.contains( c ) );
		}
	private :
		BFC::Buffer			name;
		BFC::Buffer			info;
		BFC::CompArray< BFC::FourCC >	fourCC;
	};

	static ConvEntry tbl[];

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_VideoCompression_H_

// ============================================================================

