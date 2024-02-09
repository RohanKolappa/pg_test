// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.VideoCompression.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.Dyn.TEnum.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.VideoCompression.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================
// VMP::VideoCompression
// ============================================================================

VideoCompression::ConvEntry VideoCompression::tbl[] = {

	ConvEntry( "Unknown",	"????",		"<unknown>" ),
	ConvEntry( "Uncompressed",	"????",		"<uncompressed>" ),

	ConvEntry( "AASC",	"AASC",		"Autodesk Animator" ),
	ConvEntry( "ABYR",	"ABYR",		"?" ),
	ConvEntry( "ADV1",	"ADV1",		"WaveCodec" ),
	ConvEntry( "AEMI",	"AEMI",		"Array VideoONE MPEG1-I" ),
	ConvEntry( "AFLI",	"AFLI",		"Autodesk Animator" ),
	ConvEntry( "AFLC",	"AFLC",		"Autodesk Animator" ),
	ConvEntry( "AJPG",	"AJPG",		"?" ),
	ConvEntry( "AMPG",	"AMPG",		"Array VideoONE MPEG" ),
	ConvEntry( "ANIM",	"ANIM",		"RDX" ),
	ConvEntry( "AP41",	"AP41",		"AngelPotion" ),
	ConvEntry( "ASLC",	"ASLC",		"?" ),
	ConvEntry( "ASV1",	"ASV1",		"Asus video" ),
	ConvEntry( "ASV2",	"ASV2",		"Asus video 2" ),
	ConvEntry( "ASVX",	"ASVX",		"?" ),
	ConvEntry( "AUR2",	"AUR2",		"Aura 2 codec" ),
	ConvEntry( "AURA",	"AURA",		"Aura 1 codec" ),
	ConvEntry( "AVC1",	"AVC1,avc1",		"AVC-1/H.264" ),
	ConvEntry( "AVDJ",	"AVDJ",		"?" ),
	ConvEntry( "AVDV",	"AVDV",		"?" ),
	ConvEntry( "AVRN",	"AVRN,ADVJ,ADVJ",	"Avid MJPEG" ),
	ConvEntry( "AVUI",	"AVUI",		"?" ),
	ConvEntry( "AZPR",	"AZPR",		"?" ),
	ConvEntry( "BBCD",	"BBCD",		"BBC Dirac" ),
	ConvEntry( "BHIV",	"BHIV",		"?" ),
	ConvEntry( "BINK",	"BINK",		"Bink video" ),
	ConvEntry( "BLC2",	"BLC2",		"Barco Lossless Codec 2" ),
	ConvEntry( "BT20",	"BT20",		"Prosumer video" ),
	ConvEntry( "BTCV",	"BTCV",		"Composite codec" ),
	ConvEntry( "BW10",	"BW10",		"Broadway MPEG" ),
	ConvEntry( "CDVC",	"CDVC",		"Canopus DV" ),
	ConvEntry( "CFCC",	"CFCC",		"DPS Perception" ),
	ConvEntry( "CGDI",	"CGDI",		"Camcorder video" ),
	ConvEntry( "CHAM",	"CHAM",		"Caviara Champagne" ),
	ConvEntry( "CJPG",	"CJPG",		"WebCam JPEG" ),
	ConvEntry( "CLJR",	"CLJR",		"?" ),
	ConvEntry( "CMYK",	"CMYK",		"?" ),
	ConvEntry( "COL0",	"COL0",		"?" ),
	ConvEntry( "COL1",	"COL1",		"?" ),
	ConvEntry( "CSCD",	"CSCD",		"CamStudio" ),
	ConvEntry( "CVID",	"CVID,cvid",		"Cinepak (Radius)" ),
	ConvEntry( "CWLT",	"CWLT",		"Color WLT DIB" ),
	ConvEntry( "D261",	"D261",		"H.261 (DEC)" ),
	ConvEntry( "D263",	"D263",		"H.263 (DEC)" ),
	ConvEntry( "DAVC",	"DAVC",		"MPEG-4/H.264 (Dicas)" ),
	ConvEntry( "DCL1",	"DCL1",		"Data Connection videoconf" ),
	ConvEntry( "DIRAC",	"DRAC",		"Dirac (BBC Research)" ),
	ConvEntry( "DIVX",	"DIVX",		"DivX 4.0 (OpenDivX - Project Mayo)" ),
	ConvEntry( "DM4V",	"DM4V",		"MPEG-4 (Dicas)" ),
	ConvEntry( "DMB1",	"DMB1",		"Rainbow Runner" ),
	ConvEntry( "DMB2",	"DMB2",		"?" ),
	ConvEntry( "DMK2",	"DMK2",		"?" ),
	ConvEntry( "DSVD",	"DSVD",		"DV codec" ),
	ConvEntry( "DUCK",	"DUCK",		"TrueMotion S (Duck)" ),
	ConvEntry( "DV25",	"DV25",		"DVCPRO (Matrox)" ),
	ConvEntry( "DV50",	"DV50",		"DVCPRO50 (Matrox)" ),
	ConvEntry( "DVAN",	"DVAN",		"?" ),
	ConvEntry( "DVCS",	"DVCS",		"DV video" ),
	ConvEntry( "DVE2",	"DVE2",		"DVE-2 videoconf" ),
	ConvEntry( "DVH1",	"DVH1",		"SMPTE 370M (100 Mbps)" ),
	ConvEntry( "DVHD",	"DVHD",		"DV (50 Mbps)" ),
	ConvEntry( "DVPP",	"DVPP",		"?" ),
	ConvEntry( "DVR",	"DVR ",		"?" ),
	ConvEntry( "DV",	"dvsd",		"DV (25 Mbs)" ),
	ConvEntry( "DVSL",	"DVSL",		"DV (12.5 Mbps)" ),
	ConvEntry( "DVX1",	"DVX1",		"DVX1000SP" ),
	ConvEntry( "DVX2",	"DVX2",		"DVX2000S" ),
	ConvEntry( "DVX3",	"DVX3",		"DVX3000S" ),
	ConvEntry( "MPEG4",	"DX50,MP4S,M4S2,DIV5,MP4V,UMP4",	"DivX 5.0 (divx.com)" ),
	ConvEntry( "DXGM",	"DXGM",		"? (Electronic Arts)" ),
	ConvEntry( "DXTN",	"DXTN",		"DirectX compressed texture" ),
	ConvEntry( "DXTC",	"DXTC",		"DirectX texture" ),
	ConvEntry( "ELK0",	"ELK0",		"?" ),
	ConvEntry( "EKQ0",	"EKQ0",		"Elsa Quick codec" ),
	ConvEntry( "EM2V",	"EM2V",		"Etymonix MPEG-2" ),
	ConvEntry( "ES07",	"ES07",		"Eyestream" ),
	ConvEntry( "ESCP",	"ESCP",		"Escape" ),
	ConvEntry( "ETV1",	"ETV1",		"eTreppid" ),
	ConvEntry( "ETV2",	"ETV2",		"eTreppid" ),
	ConvEntry( "ETVC",	"ETVC",		"eTreppid" ),
	ConvEntry( "FFV1",	"FFV1",		"FFMPEG codec" ),
	ConvEntry( "FFVH",	"FFVH",		"?" ),
	ConvEntry( "FLIC",	"FLIC",		"?" ),
	ConvEntry( "FLJP",	"FLJP",		"Field encoded motion JPEG" ),
	ConvEntry( "FLV1",	"FLV1",		"?" ),
	ConvEntry( "FMP4",	"FMP4",		"FFMPEG (MPEG-4)" ),
	ConvEntry( "FPS1",	"FPS1",		"Fraps codec" ),
	ConvEntry( "FRWA",	"FRWA",		"Forward motion JPEG" ),
	ConvEntry( "FRWD",	"FRWD",		"Forward motion JPEG" ),
	ConvEntry( "FVF1",	"FVF1",		"Fractal video frame" ),
	ConvEntry( "GEOX",	"GEOX",		"GEOMPEG4" ),
	ConvEntry( "GJPG",	"GJPG",		"GT891x" ),
	ConvEntry( "GLZW",	"GLZW",		"Motion LZW" ),
	ConvEntry( "GPEG",	"GPEG",		"Motion JPEG" ),
	ConvEntry( "GWLT",	"GWLT",		"Grayscale (Microsoft)" ),
	ConvEntry( "GXVE",	"GXVE",		"?" ),
	ConvEntry( "H261",	"H261,M261",	"ITU H.261" ),
	ConvEntry( "H262",	"H262",		"ITU H.262" ),
	ConvEntry( "H263",	"H263,M263",	"ITU H.263" ),
	ConvEntry( "H264",	"H264,h264,AVC1,avc1",	"ITU H.264" ),
	ConvEntry( "HDV2",	"HDV2",		"?" ),
	ConvEntry( "HDV3",	"HDV3",		"?" ),
	ConvEntry( "HUFFYUV",	"HFYU",		"Huffman Lossless" ),
	ConvEntry( "HMCR",	"HMCR",		"Rendition Motion Compensation" ),
	ConvEntry( "HMRR",	"HMRR",		"Rendition Motion Compensation" ),
	ConvEntry( "HVD2",	"HVD2",		"?" ),
	ConvEntry( "H263I",	"I263",		"H.263 (Intel)" ),
	ConvEntry( "IAN ",	"IAN ",		"Indeo 4" ),
	ConvEntry( "ICLB",	"ICLB",		"CellB Videoconf" ),
	ConvEntry( "IGOR",	"IGOR",		"Poxer DVD" ),
	ConvEntry( "IJPG",	"IJPG",		"Intergraph JPEG" ),
	ConvEntry( "ILVC",	"ILVC",		"Layered codec (Intel)" ),
	ConvEntry( "ILVR",	"ILVR",		"H.263+" ),
	ConvEntry( "IPDV",	"IPDV",		"Giga AVI DV" ),
	ConvEntry( "IPJ2",	"IPJ2",		"?" ),
	ConvEntry( "IR21",	"IR21",		"Indeo 2.1" ),
	ConvEntry( "ISME",	"ISME",		"?" ),
	ConvEntry( "INDEO31",	"IV31",		"Indeo 3.1 (Intel)" ),
	ConvEntry( "IV32",	"IV32",		"Indeo 3.2 (Intel)" ),
	ConvEntry( "IV41",	"IV41",		"Indeo 4.1 (Intel)" ),
	ConvEntry( "IV50",	"IV50",		"Indeo 5.0 (Intel)" ),
	ConvEntry( "JBYR",	"JBYR",		"? (Kensington)" ),
	ConvEntry( "JFGO",	"JFGO",		"JFGO's Codec" ),
	ConvEntry( "JFIF",	"JFIF",		"?" ),
	ConvEntry( "JPEG",	"JPEG",		"JPEG" ),
	ConvEntry( "JPGL",	"JPGL",		"Pegasus lossless JPEG" ),
	ConvEntry( "KMVC",	"KMVC",		"Karl Morton's codec" ),
	ConvEntry( "L261",	"L261",		"Lead H.261" ),
	ConvEntry( "L263",	"L263",		"Lead H.263" ),
	ConvEntry( "LBYR",	"LBYR",		"?" ),
	ConvEntry( "LCMW",	"LCMW",		"Motion CMW (Lead)" ),
	ConvEntry( "LCW2",	"LCW2",		"?" ),
	ConvEntry( "LEAD",	"LEAD",		"Lead video codec" ),
	ConvEntry( "LGRY",	"LGRY",		"Grayscale (Lead)" ),
	ConvEntry( "LJ2K",	"LJ2K",		"Lead JPEG 2000" ),
	ConvEntry( "LOCO",	"LOCO",		"?" ),
	ConvEntry( "LSVC",	"LSVC",		"Lightning Strike" ),
	ConvEntry( "LSVM",	"LSVM",		"Lightning Strike" ),
	ConvEntry( "LSVX",	"LSVX",		"Lightning Strike" ),
	ConvEntry( "LZO1",	"LZO1",		"Lempel Ziv" ),
	ConvEntry( "M4CC",	"M4CC",		"MPEG-4" ),
	ConvEntry( "MC12",	"MC12",		"Motion Compensation" ),
	ConvEntry( "MCAM",	"MCAM",		"Motion Compensation" ),
	ConvEntry( "MJ2C",	"MJ2C",		"Motion JPEG 2000" ),
	ConvEntry( "MJLS",	"MJLS",		"JPEG-LS" ),
	ConvEntry( "MJPA",	"MJPA",		"?" ),
	ConvEntry( "MJPB",	"MJPB",		"?" ),
	ConvEntry( "MJPEG",	"MJPG,LJPG",	"Motion JPEG" ),
	ConvEntry( "MMES",	"MMES",		"MPEG-2 ES (Matrox)" ),
	ConvEntry( "MP2A",	"MP2A",		"MPEG-2 Audio (Media Excel)" ),
	ConvEntry( "MP2T",	"MP2T",		"MPEG-2 TS (Media Excel)" ),
	ConvEntry( "MP2V",	"MP2V",		"MPEG-2 Video (Media Excel)" ),
	ConvEntry( "MSMPEG4V2",	"MP42,DIV4",	"DivX 3.0 (fast motion)" ),
	ConvEntry( "MSMPEG4V3",	"MP43,MPG3",	"MPEG-4 (Microsoft)" ),
	ConvEntry( "MP4A",	"MP4A",		"MPEG-4 Audio (Media Excel)" ),
	ConvEntry( "MP4T",	"MP4T",		"MPEG-4 TS (Media Excel)" ),
	ConvEntry( "MPEG1",	"MPEG,mpeg,MPG1,mpg1,VCR1",	"MPEG-1" ),
	ConvEntry( "MPEG2",	"MPG2",		"MPEG-2 Video" ),
	ConvEntry( "MSMPEG4V1",	"MPG4,DIV3",	"DivX 3.0 (low motion)" ),
	ConvEntry( "MPGI",	"MPGI",		"MPEG (Sigma Designs)" ),
	ConvEntry( "MPNG",	"MPNG",		"?" ),
	ConvEntry( "MR16",	"MR16",		"?" ),
	ConvEntry( "MRCA",	"MRCA",		"Mrcodec" ),
	ConvEntry( "MRLE",	"MRLE",		"Microsoft RLE" ),
	ConvEntry( "MSS1",	"MSS1",		"?" ),
	ConvEntry( "MSS2",	"MSS2",		"?" ),
	ConvEntry( "MSVC",	"MSVC,CRAM,WHAM",	"Microsoft Video 1" ),
	ConvEntry( "MSZH",	"MSZH",		"AVImszh" ),
	ConvEntry( "MTGA",	"MTGA",		"?" ),
	ConvEntry( "MTX1",	"MTX1",		"? (Matrox)" ),
	ConvEntry( "MTX2",	"MTX2",		"? (Matrox)" ),
	ConvEntry( "MTX3",	"MTX3",		"? (Matrox)" ),
	ConvEntry( "MTX4",	"MTX4",		"? (Matrox)" ),
	ConvEntry( "MTX5",	"MTX5",		"? (Matrox)" ),
	ConvEntry( "MTX6",	"MTX6",		"? (Matrox)" ),
	ConvEntry( "MTX7",	"MTX7",		"? (Matrox)" ),
	ConvEntry( "MTX8",	"MTX8",		"? (Matrox)" ),
	ConvEntry( "MTX9",	"MTX9",		"? (Matrox)" ),
	ConvEntry( "MVI1",	"MVI1",		"Motion Pixels MV11" ),
	ConvEntry( "MVI2",	"MVI2",		"Motion Pixels MV12" ),
	ConvEntry( "MWV1",	"MWV1",		"Aware Motion Wavelets" ),
	ConvEntry( "MX5P",	"MX5P",		"?" ),
	ConvEntry( "NAVI",	"NAVI",		"?" ),
	ConvEntry( "NHVU",	"NHVU",		"?" ),
	ConvEntry( "NTN1",	"NTN1",		"Video compression 1 (Zoran)" ),
	ConvEntry( "NTN2",	"NTN2",		"Video compression 2 (Zoran)" ),
	ConvEntry( "NUV1",	"NUV1",		"?" ),
	ConvEntry( "NVDS",	"NVDS",		"NVidia texture format" ),
	ConvEntry( "NVHS",	"NVHS",		"NVidia texture format" ),
	ConvEntry( "NVS0",	"NVS0",		"? (NVidia)" ),
	ConvEntry( "NVS1",	"NVS1",		"? (NVidia)" ),
	ConvEntry( "NVS2",	"NVS2",		"? (NVidia)" ),
	ConvEntry( "NVS3",	"NVS3",		"? (NVidia)" ),
	ConvEntry( "NVS4",	"NVS4",		"? (NVidia)" ),
	ConvEntry( "NVS5",	"NVS5",		"? (NVidia)" ),
	ConvEntry( "NVT0",	"NVT0",		"? (NVidia)" ),
	ConvEntry( "NVT1",	"NVT1",		"? (NVidia)" ),
	ConvEntry( "NVT2",	"NVT2",		"? (NVidia)" ),
	ConvEntry( "NVT3",	"NVT3",		"? (NVidia)" ),
	ConvEntry( "NVT4",	"NVT4",		"? (NVidia)" ),
	ConvEntry( "NVT5",	"NVT5",		"? (NVidia)" ),
	ConvEntry( "PDVC",	"PDVC",		"DVC codec" ),
	ConvEntry( "PGVV",	"PGVV",		"Radius Video Vision" ),
	ConvEntry( "PHMO",	"PHMO",		"Photomotion (IBM)" ),
	ConvEntry( "PIMJ",	"PIMJ",		"Pegasus Lossless JPEG" ),
	ConvEntry( "PIXL",	"PIXL",		"Video XL (Pinnacle)" ),
	ConvEntry( "PJPG",	"PJPG",		"PA MJPEG" ),
	ConvEntry( "PNG",	"PNG ",		"?" ),
	ConvEntry( "PVEZ",	"PVEZ",		"PowerEZ" ),
	ConvEntry( "PVMM",	"PVMM",		"PacketVideo MPEG-4" ),
	ConvEntry( "PVW2",	"PVW2",		"Pegasus Wavelet2000" ),
	ConvEntry( "Q1.0",	"Q1.0",		"?" ),
	ConvEntry( "Q1.1",	"Q1.1",		"?" ),
	ConvEntry( "QDRW",	"QDRW,qdrw",		"QuickDraw" ),
	ConvEntry( "QPEG",	"QPEG",		"QPEG (Q-Team)" ),
	ConvEntry( "QPEQ",	"QPEQ",		"QPEG 1.1 (Q-Team)" ),
	ConvEntry( "RGBT",	"RGBT",		"?" ),
	ConvEntry( "RJPG",	"RJPG",		"?" ),
	ConvEntry( "RLE",	"RLE ,rle ",		"Apple Animation (RLE)" ),
	ConvEntry( "RLE4",	"RLE4",		"4bpp RLE" ),
	ConvEntry( "RLE8",	"RLE8",		"8bpp RLE" ),
	ConvEntry( "RMP4",	"RMP4",		"MPEG-4 AS Profile" ),
	ConvEntry( "ROQV",	"ROQV",		"?" ),
	ConvEntry( "RPZA",	"RPZA,rpza",		"Apple Video (RPZA)" ),
	ConvEntry( "RT21",	"RT21",		"RealTime Video 2.1 (Intel)" ),
	ConvEntry( "RTLL",	"RTLL",		"ArithYUV Lossless Codec" ),
	ConvEntry( "RV10",	"RV10",		"?" ),
	ConvEntry( "RV13",	"RV13",		"?" ),
	ConvEntry( "RV20",	"RV20",		"RealVideo G2" ),
	ConvEntry( "RV30",	"RV30",		"RealVideo 8" ),
	ConvEntry( "RV40",	"RV40",		"RealVideo 10 (?)" ),
	ConvEntry( "RVX",	"RVX ",		"RDX (Intel)" ),
	ConvEntry( "S263",	"S263",		"?" ),
	ConvEntry( "S422",	"S422",		"VideoCap C210" ),
	ConvEntry( "SAN3",	"SAN3",		"DivX 3" ),
	ConvEntry( "SDCC",	"SDCC",		"Digital Camera codec" ),
	ConvEntry( "SEDG",	"SEDG",		"Samsung MPEG-4" ),
	ConvEntry( "SFMC",	"SFMC",		"Surface Fitting Method" ),
	ConvEntry( "SGI",	"SGI ,sgi ",		"SGI" ),
	ConvEntry( "SGI1",	"SGI1",		"SGI" ),
	ConvEntry( "SMC",	"SMC ,smc ",		"Apple Graphics (SMC)" ),
	ConvEntry( "SMP4",	"SMP4",		"?" ),
	ConvEntry( "SMSC",	"SMSC",		"? (Radius)" ),
	ConvEntry( "SMSD",	"SMSD",		"? (Radius)" ),
	ConvEntry( "SMSV",	"SMSV",		"Wavelet video" ),
	ConvEntry( "SNOW",	"SNOW",		"?" ),
	ConvEntry( "SP40",	"SP40",		"? (SunPlus)" ),
	ConvEntry( "SP44",	"SP44",		"? (SunPlus)" ),
	ConvEntry( "SP53",	"SP53",		"? (SunPlus)" ),
	ConvEntry( "SP54",	"SP54",		"? (SunPlus)" ),
	ConvEntry( "SP55",	"SP55",		"? (SunPlus)" ),
	ConvEntry( "SP56",	"SP56",		"? (SunPlus)" ),
	ConvEntry( "SP57",	"SP57",		"? (SunPlus)" ),
	ConvEntry( "SP58",	"SP58",		"? (SunPlus)" ),
	ConvEntry( "SPIG",	"SPIG",		"Spigot (Radius)" ),
	ConvEntry( "SQZ2",	"SQZ2",		"VXTreme video codec V2" ),
	ConvEntry( "STVA",	"STVA",		"? (ST Microelectronics)" ),
	ConvEntry( "STVB",	"STVB",		"? (ST Microelectronics)" ),
	ConvEntry( "STVC",	"STVC",		"? (ST Microelectronics)" ),
	ConvEntry( "STVX",	"STVX",		"? (ST Microelectronics)" ),
	ConvEntry( "STVY",	"STVY",		"? (ST Microelectronics)" ),
	ConvEntry( "SV10",	"SV10",		"?" ),
	ConvEntry( "SVQ1",	"SVQ1,svq1,svqi",		"Sorenson Video 1" ),
	ConvEntry( "SVQ3",	"SVQ3",		"Sorenson Video 3" ),
	ConvEntry( "THEORA",	"THEO,THRA",	"Theora" ),
	ConvEntry( "TLMS",	"TLMS",		"Motion I-Frame codec" ),
	ConvEntry( "TLST",	"TLST",		"Motion I-Frame codec" ),
	ConvEntry( "TM20",	"TM20",		"TrueMotion 2.0" ),
	ConvEntry( "TM2X",	"TM2X",		"TrueMotion 2X" ),
	ConvEntry( "TMIC",	"TMIC",		"Motion I-Frame codec" ),
	ConvEntry( "TMOT",	"TMOT",		"TrueMotion S" ),
	ConvEntry( "TR20",	"TR20",		"TrueMotion RT 2.0" ),
	ConvEntry( "TSCC",	"TSCC",		"TechSmith" ),
	ConvEntry( "TV10",	"TV10",		"Tecomac" ),
	ConvEntry( "TVJP",	"TVJP",		"? (Pinnacle)" ),
	ConvEntry( "TVMJ",	"TVMJ",		"? (Pinnacle)" ),
	ConvEntry( "TY2C",	"TY2C",		"? (Trident)" ),
	ConvEntry( "TY2N",	"TY2N",		"? (Trident)" ),
	ConvEntry( "U263",	"U263",		"?" ),
	ConvEntry( "UCOD",	"UCOD",		"ClearVideo" ),
	ConvEntry( "ULTI",	"ULTI",		"Ultimotion" ),
	ConvEntry( "V261",	"V261",		"Lucent VX2000S" ),
	ConvEntry( "VC-1",	"VC-1,vc-1",		"SMPTE RP 2025" ),
	ConvEntry( "VDCT",	"VDCT",		"VideoMaker Pro" ),
	ConvEntry( "VDOM",	"VDOM",		"VDOWave" ),
	ConvEntry( "VDOW",	"VDOW",		"VDOLive" ),
	ConvEntry( "VDTZ",	"VDTZ",		"VideoTizer YUV codec" ),
	ConvEntry( "VGPX",	"VGPX",		"VideoGramPix (Alaris)" ),
	ConvEntry( "VIDS",	"VIDS",		"? (Vitec Multimedia)" ),
	ConvEntry( "VIFP",	"VIFP",		"?" ),
	ConvEntry( "VIV1",	"VIV1",		"?" ),
	ConvEntry( "VIV2",	"VIV2",		"?" ),
	ConvEntry( "VIVO",	"VIVO",		"Vivo H.263" ),
	ConvEntry( "VIXL",	"VIXL",		"Video XL (MiroVideo)" ),
	ConvEntry( "VLV1",	"VLV1",		"VideoLogic" ),
	ConvEntry( "VMNC",	"VMNC",		"?" ),
	ConvEntry( "VP30",	"VP30,VP3 ",		"On2 VP30" ),
	ConvEntry( "VP31",	"VP31",		"On2 VP31" ),
	ConvEntry( "VP40",	"VP40,VP4 ",		"On2 VP40" ),
	ConvEntry( "VP50",	"VP50,VP5 ",		"On2 VP50" ),
	ConvEntry( "VP60",	"VP60,VP6 ",		"On2 VP60" ),
	ConvEntry( "VP61",	"VP61",		"On2 VP61" ),
	ConvEntry( "VP62",	"VP62",		"On2 VP62" ),
	ConvEntry( "VP70",	"VP70",		"On2 VP70" ),
	ConvEntry( "VQC1",	"VQC1",		"VideoQuest Codec 1" ),
	ConvEntry( "VQC2",	"VQC2",		"VideoQuest Codec 2" ),
	ConvEntry( "VSSH",	"VSSH",		"?" ),
	ConvEntry( "VSSV",	"VSSV",		"?" ),
	ConvEntry( "VSSW",	"VSSW",		"?" ),
	ConvEntry( "VTLP",	"VTLP",		"?" ),
	ConvEntry( "VUUU",	"VUUU",		"?" ),
	ConvEntry( "VX1K",	"VX1K",		"Lucent VX1000S" ),
	ConvEntry( "VX2K",	"VX2K",		"Lucent VX2000S" ),
	ConvEntry( "VXSP",	"VXSP",		"Lucent VX1000SP" ),
	ConvEntry( "WBVC",	"WBVC",		"? (Winbond)" ),
	ConvEntry( "WINX",	"WINX",		"? (Winnov Videum)" ),
	ConvEntry( "WJPG",	"WJPG",		"? (Winbond JPEG)" ),
	ConvEntry( "WMV1",	"WMV1",		"Windows Media Video 7" ),
	ConvEntry( "WMV2",	"WMV2",		"Windows Media Video 8" ),
	ConvEntry( "WMV3",	"WMV3",		"Windows Media Video 9" ),
	ConvEntry( "WMVA",	"WMVA",		"?" ),
	ConvEntry( "WMVP",	"WMVP",		"?" ),
	ConvEntry( "WNV1",	"WNV1",		"? (Winnov Videum)" ),
	ConvEntry( "WRLE",	"WRLE",		"MS RLE" ),
	ConvEntry( "WV1F",	"WV1F",		"?" ),
	ConvEntry( "WVP2",	"WVP2",		"?" ),
	ConvEntry( "X261",	"X261",		"?" ),
	ConvEntry( "X263",	"X263",		"Xirlink H.263" ),
	ConvEntry( "X264",	"X264",		"XiWave H.264" ),
	ConvEntry( "XLV0",	"XLV0",		"XL Video Decoder" ),
	ConvEntry( "XMPG",	"XMPG",		"I-frame only MPEG" ),
	ConvEntry( "XVID",	"XVID",		"Xvid MPEG-4" ),
	ConvEntry( "XVIX",	"XVIX",		"?" ),
	ConvEntry( "XWV3",	"XWV3",		"Xi-3" ),
	ConvEntry( "XXAN",	"XXAN",		"Wing Commander" ),
	ConvEntry( "YUYV",	"YUYV",		"Compressed YUV" ),
	ConvEntry( "YV92",	"YV92",		"Compressed YVU9" ),
	ConvEntry( "ZLIB",	"ZLIB",		"Lossless codec" ),
	ConvEntry( "ZMBV",	"ZMBV",		"?" ),
	ConvEntry( "ZPEG",	"ZPEG",		"Video Zipper" ),
	ConvEntry( "ZYGO",	"ZYGO",		"ZyGoVideo" )

};

// ============================================================================

VideoCompression::VideoCompression(
	const	Uint32		c ) {

	if ( c >= MaxFormat ) {
		throw InternalError( "VMP.VideoCompression: overflow (max: "
			+ Buffer( ( Uint32 )MaxFormat )
			+ ", req: "
			+ Buffer( ( Uint32 )c )
			+ ")!" );
	}

	f = ( Format )c;

}

VideoCompression::VideoCompression(
	const	FourCC &	c ) {

	Uint32 nbr = sizeof( tbl ) / sizeof( ConvEntry );

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		if ( tbl[ i ].contains( c ) ) {
			f = ( Format )i;
			return;
		}
	}

	f = Unknown;

}

VideoCompression::VideoCompression(
	const	Buffer &	b ) {

	Uint32 nbr = sizeof( tbl ) / sizeof( ConvEntry );

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		if ( tbl[ i ].getName() == b ) {
			f = ( Format )i;
			return;
		}
	}

	f = Unknown;

}

// ============================================================================

void VideoCompression::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	out->putLEUint32( ( Uint32 )f );

}

void VideoCompression::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	f = ( Format )in->getLEUint32();

}

// ============================================================================

Dyn::TEnumCPtr VideoCompression::getTEnum(
	const	Bool		pUnknown ) {

	static Dyn::TEnumCPtr t[ 2 ] = {
			makeTEnum( false ),
			makeTEnum( true ) };

	return t[ pUnknown ? 1 : 0 ];

}

// ============================================================================

Dyn::TEnumCPtr VideoCompression::makeTEnum(
	const	Bool		pUnknown ) {

	Dyn::TEnumPtr	res = new Dyn::TEnum;

	Uint32 nbr = sizeof( tbl ) / sizeof( ConvEntry );

	for ( Uint32 i = ( pUnknown ? 0 : 1 ) ; i < nbr ; i++ ) {
		res->addEntry( tbl[ i ].getName() );
	}

	return res;

}

// ============================================================================
// VMP::VideoCompression::ConvEntry
// ============================================================================

VideoCompression::ConvEntry::ConvEntry(
	const	char *		pName,
	const	char *		pList,
	const	char *		pInfo ) :

	name	( pName ),
	info	( pInfo ) {

	BufferArray tokens = BufferParser::tokenize( Buffer( pList ), ", " );

	fourCC.resize( tokens.size() );

	for ( Uint32 i = 0 ; i < fourCC.size() ; i++ ) {
		fourCC[ i ] = FourCC( *( const Uint32 * )tokens[ i ].getBufferAddr() );
	}

}

// ============================================================================

