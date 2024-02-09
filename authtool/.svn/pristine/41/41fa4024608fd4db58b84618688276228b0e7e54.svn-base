#ifndef V2DFILECREATOR_H_92E85C47_6D9F_43f5_AADB_399F23F1EA5F__INCLUDED
#define V2DFILECREATOR_H_92E85C47_6D9F_43f5_AADB_399F23F1EA5F__INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFileCreator.h"
#include "CommonIncludes.hh"
#include "MediaFlags.hh"
#include "MediaServer.hh"
#include "vframeparser_c.h"
#include "yuv4mpeg2writer.h"
#include "srd.h"
#include "AudioDecoder.h"
#include "AudioFrameParser.h"
#include "WaveFile.h"

using namespace mediaindex;

#define DEFAULT_TARGET_FPS		                30
#define DEFAULT_AUDIO_FREQUENCY                 16315
#define STARTOFFSET_IFRAME_ADJUSTMENT		    300000

#define TRC_SUCCESS					            0L
#define TRC_VIDEOMODE_NOTSUPPORTED				9000
#define TRC_VIDEORESOLUTION_CHANGED				9001
#define TRC_VIDEOFRAMEDECODER_CREATION_FAILED	9002
#define TRC_FAILED_WRITING_FRAMES_IN_YUV_FILE	9003
#define TRC_FAILED_WRITING_YUV_HEADER			9004
#define TRC_WRONG_TARGET_FPS					9005
#define TRC_MEDIASERVER_CREATION_FAILED			9006
#define TRC_MEDIASERVER_INIT_FAILED				9007
#define TRC_MEDIASERVER_START_FAILED			9008
#define TRC_MEDIASERVER_GETFRAME_FAILED			9009
#define TRC_INIT_SRD_FAILED						9010
#define TRC_STREAMTYPE_NOTSUPPORTED				9011
#define TRC_WRITING_WAVE_HEADER_FAILED          9013
#define TRC_WRITING_WAVE_DATA_FAILED            9014
#define TRC_AUDIODECODER_DECODESAMPLE_FAILED    9015
#define TRC_AUDIOPARSER_FINDINGSOFEOF_FAILED    9016
#define TRC_AUDIOFRAMEDECODER_CREATION_FAILED	9017
#define TRC_AUDIOFRAMEPARSER_CREATION_FAILED	9018
#define TRC_WAVEFILE_CREATION_FAILED	        9019
#define TRC_YUV4MPEG2WRITER_CREATION_FAILED     9020


void WriteYUVToBmpFile(const unsigned char *a_pYUV, const size_t a_nWidth,
        const size_t a_nHeight, V2dLibColorTransform::YUVToRGBConverter::SubSample a_nSubsample,
        const std::string a_strFile);


class CV2dFileCreator : public IFileCreator
{
public:
    CV2dFileCreator(std::string& a_rSrtBlobDir,
                    std::string& a_rStrOutFile,
                    StTranscodeOpts& a_sArgList);

    virtual ~CV2dFileCreator(void);

	virtual int Initialize();
	virtual int CreateFile();

	virtual int SetTargetFps(const int a_nTargetFps);

private:
    enum ePktType { eVideoPacket = 0, eAudioPacket };

    bool ParseAVFileNames(const std::string& a_rStrOutFile);
    bool ParseAVMode(const std::string& a_rStrAVMode);

	bool HandleCFrame(CCFrame &a_rCFrame);
    bool HandleVideoCFrame(CCFrame &a_rCFrame, 
						   const CV2DStreamMediaFlag &a_rV2dSMediaFlag);
	bool HandleAudioCFrame(CCFrame &a_rCFrame, 
						   const CV2DStreamMediaFlag &a_rV2dSMediaFlag);
    bool HandleControlCFrame(CCFrame &a_rCFrame);
	
    uint64_t GetAdjustedStartOffset() const;
    double GetExpectedOutFrameCount(const uint64_t a_nTimeOffset) const;

	bool WriteYUVFrames(const uint64_t a_nCurrentTimeOffset);
    bool WriteYuvFileHeader();

    bool WriteWaveFileHeader();
    bool WriteWaveFileData(const unsigned char* a_pData, 
                       	   const unsigned int a_nLen);

    bool InitializeFrameDecoder();
    bool CheckForPktLoss(CCFrame &a_rCFrame, 
                         const bool a_bSrcRTP,
                         const ePktType a_ePktType);

	uint64_t m_nStartTimeOffset;
	uint64_t m_nDurationTime;

    std::string m_strBlobDir;
    std::string m_strVideoOutFile;
    std::string m_strAudioOutFile;

    uint64_t m_nTotalCFrames;
    uint64_t m_nVideoCFrames;
    uint64_t m_nVideoCFramesDiscarded;
    uint64_t m_nAudioCFrames;
    uint64_t m_nAudioCFramesDiscarded;
    uint64_t m_nControlCFrames;

	sliceRedData_t*	m_pSliceRdata;

    uint64_t m_nVFramesWritten;

    uint16_t m_nWidth;
    uint16_t m_nHeight;
    int m_nSliceSize;
	bool m_bGotVideoRes;

	uint64_t m_nCompleteVideoFrames;

	v2dDecConfiguration m_stDecConfig;
	CVideoFrameDecoder *m_pFrameDecoder;
	unsigned char* m_pDecodedBuf;

	double		m_fOutputFramePeriod;
	uint16_t	m_nTargetFps;

    uint16_t    m_nAudioChannels;
    uint16_t    m_nBitsPerSample;
    uint16_t    m_nAudioFrequency;
    bool        m_bWaveHdrWritten;

    bool        m_bTranscodeVideo;
    bool        m_bTranscodeAudio;

    CAudioDecoder*      m_pAudioDecoder;
    CAudioFrameParser*  m_pAudioFrameParser;
    CWaveFile*          m_pWaveFile;

	CYuv4Mpeg2Writer*	m_pYuv4Mpeg2Writer;

    uint64_t    m_nVideoPktLoss;
    uint64_t    m_nVideoOutOfOrder;
    uint64_t    m_nVideoSeq;

    uint64_t    m_nAudioPktLoss;
    uint64_t    m_nAudioOutOfOrder;
    uint64_t    m_nAudioSeq;

	uint16_t	m_nErrorCode;
    std::string		m_strErrorMsg;
};

inline double CV2dFileCreator::GetExpectedOutFrameCount(
                                    const uint64_t a_nCurrentTimeOffset) const
{
    return(static_cast<double>
			((a_nCurrentTimeOffset - m_nStartTimeOffset) / m_fOutputFramePeriod));
}

#endif // V2DFILECREATOR_H_92E85C47_6D9F_43f5_AADB_399F23F1EA5F__INCLUDED
