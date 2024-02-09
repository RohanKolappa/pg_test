#include "IPVSTranscode_compatibility.h"

#include "V2dFileCreator.h"
#include "MediaServer.hh"
#include "rtp.h"
#include "vframedecoder.hh"

#include "colortransform.h"
#include "bitmap.hh"

#include <iostream>
#include <string>
#include <memory>
using std::string;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::auto_ptr;

CV2dFileCreator::CV2dFileCreator(std::string& a_rStrBlobDir,
                                 std::string& a_rStrOutFile,
                                 StTranscodeOpts& a_rStArgList) 
        : m_nTotalCFrames(0)
        , m_nVideoCFrames(0)
        , m_nVideoCFramesDiscarded(0)
        , m_nAudioCFrames(0)
        , m_nAudioCFramesDiscarded(0)
        , m_nControlCFrames(0)
		, m_pSliceRdata(NULL)
        , m_nVFramesWritten(0)
        , m_nWidth(0)
        , m_nHeight(0)
        , m_nSliceSize(0)
		, m_bGotVideoRes(false)
		, m_nCompleteVideoFrames(0)
		, m_pFrameDecoder(NULL)
		, m_pDecodedBuf(NULL)
		, m_fOutputFramePeriod(0.0)
		, m_nTargetFps(DEFAULT_TARGET_FPS)
        , m_bWaveHdrWritten(false)
        , m_bTranscodeVideo(false)
        , m_bTranscodeAudio(false)
        , m_pAudioDecoder(NULL)
        , m_pAudioFrameParser(NULL)
        , m_pWaveFile(NULL)
		, m_pYuv4Mpeg2Writer(NULL)
        , m_nVideoPktLoss(0)
        , m_nVideoOutOfOrder(0)
        , m_nVideoSeq(0)
        , m_nAudioPktLoss(0)
        , m_nAudioOutOfOrder(0)
        , m_nAudioSeq(0)
        , m_nErrorCode(TRC_SUCCESS)
		, m_strErrorMsg("")
{
    m_strBlobDir = a_rStrBlobDir;

    ParseAVMode(a_rStArgList.strAVMode);
    ParseAVFileNames(a_rStrOutFile);

    m_nStartTimeOffset = a_rStArgList.nStartOffset;
    m_nDurationTime	= a_rStArgList.nDurationTime;
    if (m_nDurationTime > 0) {
        m_nDurationTime += m_nStartTimeOffset;
    }
}

CV2dFileCreator::~CV2dFileCreator(void)
{
    if (m_bTranscodeVideo) {
        destroySRD(m_pSliceRdata);
        delete m_pSliceRdata;
        delete [] m_pDecodedBuf;
        delete m_pYuv4Mpeg2Writer;
        delete m_pFrameDecoder;
    }

    if (m_bTranscodeAudio) {
        m_pWaveFile->Close();

        delete m_pAudioDecoder;
        delete m_pAudioFrameParser;
        delete m_pWaveFile;
    }
}

int CV2dFileCreator::Initialize()
{
    if (m_bTranscodeVideo) {
        m_pSliceRdata = new sliceRedData_t;

        if (initSRD(m_pSliceRdata) == 0) {
            setStereoPolicy(m_pSliceRdata, SP_MONO);
        }
        else {
            m_nErrorCode = TRC_INIT_SRD_FAILED;
            m_strErrorMsg = "initSRD failed.";

            return m_nErrorCode;
        }

        m_stDecConfig.m_eEncodeFormat = eYUV422P;
        m_pYuv4Mpeg2Writer = new CYuv4Mpeg2Writer(m_strVideoOutFile);
        if (!m_pYuv4Mpeg2Writer) {
            m_nErrorCode = TRC_YUV4MPEG2WRITER_CREATION_FAILED;
            m_strErrorMsg = "Could not create Yuv4Mpeg2 Writer object. ";

            cerr << m_strErrorMsg << endl;

            return m_nErrorCode;
        }
    }

    if (m_bTranscodeAudio) {
        m_pAudioDecoder = new CAudioDecoder();
        if (!m_pAudioDecoder) {
            m_nErrorCode = TRC_AUDIOFRAMEDECODER_CREATION_FAILED;
            m_strErrorMsg = "Could not create Audio Decoder object. ";

            cerr << m_strErrorMsg << endl;

            return m_nErrorCode;
        }

        m_pAudioFrameParser = new CAudioFrameParser();
        if (!m_pAudioFrameParser) {
            m_nErrorCode = TRC_AUDIOFRAMEPARSER_CREATION_FAILED;
            m_strErrorMsg = "Could not create Audio Frame Parser object. ";

            cerr << m_strErrorMsg << endl;

            return m_nErrorCode;
        }

        m_pWaveFile = new CWaveFile();
        if (!m_pWaveFile) {
            m_nErrorCode = TRC_WAVEFILE_CREATION_FAILED;
            m_strErrorMsg = "Could not create Wave File object. ";

            cerr << m_strErrorMsg << endl;

            return m_nErrorCode;
        }
    }

	return TRC_SUCCESS;
}

int CV2dFileCreator::CreateFile()
{
    CCFrame cFrame;

    bool bResult = true;
    int nRetVal = 0;

    uint64_t nCurrentTimeOffset = 0;
    uint64_t nStartTimestamp = 0;

    m_fOutputFramePeriod = 1000.0 / m_nTargetFps;

    auto_ptr<CMediaServer> pMediaServer(CMediaServer::Create(m_strBlobDir.c_str()));
    if (pMediaServer.get() == NULL) {
        m_nErrorCode = TRC_MEDIASERVER_CREATION_FAILED;
        m_strErrorMsg = "Could not create MediaServer object. ";

        cerr << m_strErrorMsg << endl;

        bResult = false;
    }

    if (bResult) {
        if (pMediaServer->Init() != MSRC_OK) {
            m_nErrorCode = TRC_MEDIASERVER_INIT_FAILED;
            m_strErrorMsg = "MediaServer Init Failed: ";
            m_strErrorMsg.append(pMediaServer->GetErrorString());

            cerr << m_strErrorMsg << endl;

            bResult = false;
        }
    }

    if (bResult) {
        nStartTimestamp = pMediaServer->GetTimecodeAtOffset(
                GetAdjustedStartOffset());
        if (pMediaServer->Start(nStartTimestamp) != MSRC_OK) {
            m_nErrorCode = TRC_MEDIASERVER_START_FAILED;
            m_strErrorMsg = "MediaServer Start Failed: ";
            m_strErrorMsg.append(pMediaServer->GetErrorString());

            cerr << m_strErrorMsg << endl;

            bResult = false;
        }
    }

    while (m_nErrorCode == TRC_SUCCESS) {
        nRetVal = pMediaServer->GetFrame(cFrame);

        if (nRetVal == MSRC_END_OF_MEDIA) {
            break;
        }
        else if (nRetVal != MSRC_OK) {
            m_nErrorCode = TRC_MEDIASERVER_GETFRAME_FAILED;
            m_strErrorMsg = "MediaServer Get Frame Failed: ";
            m_strErrorMsg.append(pMediaServer->GetErrorString());

            cerr << m_strErrorMsg << endl;

            break;
        }
        m_nTotalCFrames++;

        if (cFrame.GetMessageType() != CCFrame::Normal) {
            switch (cFrame.GetMessageType()) {
            case CCFrame::MediaStarted:
                clog << "Received a start frame." << endl;
                m_nVideoSeq = 0;
                m_nAudioSeq = 0;
                break;

            case CCFrame::MediaStopped:
                clog << "Received a stop frame." << endl;
                m_nVideoSeq = 0;
                m_nAudioSeq = 0;
                break;

            default:
                clog << "Received a non-normal frame." << endl;
                break;
            }
            continue;
        }

        nCurrentTimeOffset = cFrame.GetTimeOffset();

        if (m_nDurationTime > 0) {
            if (nCurrentTimeOffset >= m_nDurationTime) {
                if (m_bTranscodeVideo)
                    WriteYUVFrames(m_nDurationTime);//If there is any pending frames, we need to fill up.

                break;
            }
        }

        bResult = HandleCFrame(cFrame);
        if (!bResult && m_nErrorCode) {
            break;
        }
    }

    if (pMediaServer.get())
        pMediaServer->Stop();

    return m_nErrorCode;
}

bool CV2dFileCreator::HandleCFrame(CCFrame &a_rCFrame)
{
    bool bRetVal = false;

    if (a_rCFrame.GetStreamType(STREAMTYPE_V2D) == STREAMTYPE_V2D) {
        CV2DStreamMediaFlag v2dSMediaFlag(a_rCFrame.GetMediaFlag());
        const short nChannel = v2dSMediaFlag.GetChannel();

        switch (nChannel) {
        case FRAMETYPE_V2D_CHAN_VIDEO:
            if (m_bTranscodeVideo) {
                if (m_bGotVideoRes)
                    bRetVal = HandleVideoCFrame(a_rCFrame, v2dSMediaFlag);
                else
                    m_nVideoCFramesDiscarded++;
            }

            break;

        case FRAMETYPE_V2D_CHAN_AUDIO:
            if (m_bTranscodeAudio) {
                if (m_bWaveHdrWritten)
                    bRetVal = HandleAudioCFrame(a_rCFrame, v2dSMediaFlag);
            }
            break;

        case FRAMETYPE_V2D_CHAN_CTL:
            bRetVal = HandleControlCFrame(a_rCFrame);
            break;
        }
    }
    else {
        m_nErrorCode = TRC_STREAMTYPE_NOTSUPPORTED;
        m_strErrorMsg
                = "Found a non V2D frame. Transcoding is supported for V2D streams only.";

        cerr << m_strErrorMsg << endl;
    }

    return bRetVal;
}

bool CV2dFileCreator::HandleVideoCFrame(CCFrame &a_rCFrame,
        const CV2DStreamMediaFlag &a_rV2dSMediaFlag)
{
    const uint32_t nSampleTotalLength = a_rCFrame.GetLength();
    const uint64_t nCurrentTimeOffset = a_rCFrame.GetTimeOffset();
    const bool bIsVideoSrcRTP = (a_rV2dSMediaFlag.GetEncaps()
            == FRAMETYPE_V2D_ENCAPS_RTP);

    unsigned char *pOutFrame = NULL;
    int nOutFrameLen = 0;

    m_nVideoCFrames++;

    if (!CheckForPktLoss(a_rCFrame, bIsVideoSrcRTP, eVideoPacket)) {
        clog << "Video packet loss detected. " << endl;
    }

    const uint32_t iPacketLen = (bIsVideoSrcRTP) ? RTP_VIDEOBUFF_SIZE
        : AV_BUFFER_SIZE;
    const uint32_t iDataOffset = (bIsVideoSrcRTP) ? RTP_HEADER_SIZE
        : V2D_PACKET_HEADER_SIZE;

    int iPackets = 0;
    do {
        const int iOffset = (iPackets * iPacketLen) + iDataOffset;
        unsigned char* pData = a_rCFrame.GetData() + iOffset;

        findFramesinPacket(m_pSliceRdata, pData);

        iPackets++;
        if (iPacketLen * iPackets >= nSampleTotalLength)
            break;

    } while (1);

    if (get_hoFrame(m_pSliceRdata) != -1) {
        nOutFrameLen = getHoFrameSize(m_pSliceRdata);

        pOutFrame = new unsigned char[nOutFrameLen];

        copyHoFrameToBuffer(m_pSliceRdata, pOutFrame, nOutFrameLen);
    }

    if (nOutFrameLen) {
        m_nCompleteVideoFrames++;

        if (!WriteYUVFrames(nCurrentTimeOffset)) {
            delete[] pOutFrame;
            return false;
        }

        int rc = m_pFrameDecoder->Decode(pOutFrame, nOutFrameLen,
                m_pDecodedBuf, m_stDecConfig.m_nDecodeBufLen);
        if (rc != 0)
            cerr << "Failed in decoding the video frame." << endl;

        if (m_nVFramesWritten % m_nTargetFps == 0) {
            clog << "Decoded " << m_nCompleteVideoFrames << ", ";
            clog << "Written " << m_nVFramesWritten << " video frames.";
            clog << " (" << m_nVFramesWritten / m_nTargetFps << ") secs.";
            clog << "\r";
        }

    }

    delete[] pOutFrame;

    return true;
}

bool CV2dFileCreator::HandleControlCFrame(CCFrame &a_rCFrame)
{
    ScapData_t stCapData;
    char *pCtlData = reinterpret_cast<char *> (a_rCFrame.GetData());
    bool bResChanged = false;

    m_nControlCFrames++;

    if (v2dLibParseScap(&stCapData, pCtlData) == 0) {
        if (m_bTranscodeVideo) {
            if (stCapData.m_hResSet) {
                if ((m_nWidth != 0) && (m_nWidth != stCapData.m_hRes))
                    bResChanged = true;

                m_nWidth = stCapData.m_hRes;
                m_stDecConfig.m_nWidth = m_nWidth;
            }
            if (stCapData.m_vResSet) {
                if ((m_nHeight != 0) && (m_nHeight != stCapData.m_vRes))
                    bResChanged = true;

                m_nHeight = stCapData.m_vRes;
                m_stDecConfig.m_nHeight = m_nHeight;
            }
            if (stCapData.m_sliceSizeSet) {
                if ((m_nSliceSize != 0) && (m_nSliceSize
                        != stCapData.m_sliceSize))
                    bResChanged = true;

                m_nSliceSize = stCapData.m_sliceSize;
                m_stDecConfig.m_nSliceSize = m_nSliceSize;
            }
            if (stCapData.m_videoCompmode) {
                m_nErrorCode = TRC_VIDEOMODE_NOTSUPPORTED;
                m_strErrorMsg = "The YUV 444 mode is not yet supported.";

                cerr << m_strErrorMsg << endl;
                return false;
            }
        }

        if (m_bTranscodeAudio) {
            if (!m_bWaveHdrWritten) {
                if (stCapData.m_audioType & AUDIOTYPE_MONO)
                    m_nAudioChannels = 1;
                else
                    m_nAudioChannels = 2;

                if (stCapData.m_audioType & AUDIOTYPE_8BIT)
                    m_nBitsPerSample = 8;
                else
                    m_nBitsPerSample = 16;

                m_nAudioFrequency = DEFAULT_AUDIO_FREQUENCY;

                if (!WriteWaveFileHeader())
                    return false;

                m_bWaveHdrWritten = true;
            }
        }
    }

    if (bResChanged) {
        const uint64_t nCurrentTimeOffset = a_rCFrame.GetTimeOffset();
        if (nCurrentTimeOffset > m_nStartTimeOffset) {
            m_nErrorCode = TRC_VIDEORESOLUTION_CHANGED;
            m_strErrorMsg
                    = "Change in video resolution detected, stopping transcoding.";

            cerr << m_strErrorMsg << endl;
            return false;
        }
        else {
            delete m_pFrameDecoder;
            m_pFrameDecoder = NULL;

            delete[] m_pDecodedBuf;
            m_pDecodedBuf = NULL;
        }
    }

    if (m_bTranscodeVideo) {
        if (m_nWidth && m_nHeight && m_nSliceSize)
            m_bGotVideoRes = true;

        if (m_pFrameDecoder == NULL) {
            if (InitializeFrameDecoder())
                return WriteYuvFileHeader();
            else
                return false;
        }
    }

    return true;
}

bool CV2dFileCreator::HandleAudioCFrame(CCFrame &a_rCFrame,
        const CV2DStreamMediaFlag &a_rV2dSMediaFlag)
{
    if (a_rCFrame.GetTimeOffset() < m_nStartTimeOffset)
        return false;

    bool bRetVal = true;
    const uint32_t nSampleTotalLength = a_rCFrame.GetLength();
    const bool bIsAudioSrcRTP = (a_rV2dSMediaFlag.GetEncaps()
            == FRAMETYPE_V2D_ENCAPS_RTP);
    unsigned char* pData = a_rCFrame.GetData();

    uint32_t nSof = 0;
    uint32_t nEof = 0;

    m_nAudioCFrames++;

    if (!CheckForPktLoss(a_rCFrame, bIsAudioSrcRTP, eAudioPacket)) {
        clog << "Audio packet loss detected. " << endl;
    }

    if (!(bRetVal = m_pAudioFrameParser->GetAudioSofEofIndex(pData,
            nSampleTotalLength, bIsAudioSrcRTP, nSof, nEof))) {
        m_nErrorCode = TRC_AUDIOPARSER_FINDINGSOFEOF_FAILED;
        m_strErrorMsg = "Failed in finding SOF/EOF in audio frame.";

        cerr << m_strErrorMsg << endl;

        return false;
    }
    else if (nSof <= 0) {
        clog << "Did not find SOF/EOF." << endl;
        return false;
    }

    const uint32_t nAvailableData = nEof - nSof;

    pData += nSof;

    if (nAvailableData > 0) {
        unsigned char* pDecodedSample = new unsigned char[nAvailableData];

        if ((bRetVal = m_pAudioDecoder->DecodeSample(pData, pDecodedSample,
                nAvailableData)))
            bRetVal = WriteWaveFileData(pDecodedSample, nAvailableData);
        else {
            m_nErrorCode = TRC_AUDIODECODER_DECODESAMPLE_FAILED;
            m_strErrorMsg = "Failed in decoding audio sample.";

            cerr << m_strErrorMsg << endl;
        }

        delete[] pDecodedSample;
    }

    return bRetVal;
}

bool CV2dFileCreator::InitializeFrameDecoder()
{
    if (m_nWidth == 0 || m_nHeight == 0)
        return false;

    m_pFrameDecoder = CVideoFrameDecoder::CreateObj(&m_stDecConfig);
    if (m_pFrameDecoder == NULL) {
        m_nErrorCode = TRC_VIDEOFRAMEDECODER_CREATION_FAILED;
        m_strErrorMsg = "Failed in creating VideoFrameDecoder object";

        cerr << m_strErrorMsg << endl;
        return false;
    }

    size_t nSize = m_nWidth * m_nHeight * 2;
    m_pDecodedBuf = new unsigned char[nSize];
    ::memset(m_pDecodedBuf, 0x00, nSize);

    return true;
}

bool CV2dFileCreator::WriteYUVFrames(const uint64_t a_nCurrentTimeOffset)
{
    if (a_nCurrentTimeOffset > m_nStartTimeOffset) {
        const double fExpectedOutFrameCount = GetExpectedOutFrameCount(
                a_nCurrentTimeOffset);
        bool bRet = true;
        while (fExpectedOutFrameCount > m_nVFramesWritten) {
            bRet = m_pYuv4Mpeg2Writer->WriteFrame(m_pDecodedBuf,
                    m_stDecConfig.m_nDecodeBufLen);
            if (!bRet) {
                m_nErrorCode = TRC_FAILED_WRITING_FRAMES_IN_YUV_FILE;
                m_strErrorMsg = "Failed to write to the output file.";

                cerr << m_strErrorMsg << endl;
                return false;
            }

            m_nVFramesWritten++;
        }
    }

    return true;
}

bool CV2dFileCreator::WriteYuvFileHeader()
{
    CYUV4MPEG2HEADER yuvHdr;
    yuvHdr.m_nWidth = m_nWidth;
    yuvHdr.m_nHeight = m_nHeight;
    yuvHdr.m_nFPSNum = m_nTargetFps;
    yuvHdr.m_nFPSDen = 1;
    yuvHdr.m_cInterlacing = 'p';
    yuvHdr.m_nPARNum = 1;
    yuvHdr.m_nPARDen = 1;
    yuvHdr.m_strChromaFormat = "422";

    bool bRet = m_pYuv4Mpeg2Writer->WriteHeader(yuvHdr);
    if (!bRet) {
        m_nErrorCode = TRC_FAILED_WRITING_YUV_HEADER;
        m_strErrorMsg = "Failed to write the output file header.";

        cerr << m_strErrorMsg << endl;
        return false;
    }

    return true;
}

int CV2dFileCreator::SetTargetFps(const int a_nTargetFps)
{
    if (a_nTargetFps <= 0 || a_nTargetFps > 60) {
        m_nErrorCode = TRC_WRONG_TARGET_FPS;
        m_strErrorMsg = "Target fps should be > 0 and < 60";

        cerr << m_strErrorMsg << endl;
        return m_nErrorCode;
    }

    m_nTargetFps = a_nTargetFps;

    return TRC_SUCCESS;
}

uint64_t CV2dFileCreator::GetAdjustedStartOffset() const
{
    if(m_nStartTimeOffset > STARTOFFSET_IFRAME_ADJUSTMENT)
		return (m_nStartTimeOffset - STARTOFFSET_IFRAME_ADJUSTMENT);
    else
        return 0;
}

bool CV2dFileCreator::WriteWaveFileHeader()
{
    WAVEFORMATEX wfx;

    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = m_nAudioChannels;
    wfx.nSamplesPerSec = m_nAudioFrequency;
    wfx.nAvgBytesPerSec = ((m_nAudioChannels * m_nBitsPerSample) / 8)
            * m_nAudioFrequency;
    wfx.nBlockAlign = (m_nAudioChannels * m_nBitsPerSample) / 8;
    wfx.wBitsPerSample = m_nBitsPerSample;
    wfx.cbSize = 0;

    wchar_t wcharAudioFile[128];

    mbstowcs(wcharAudioFile, m_strAudioOutFile.c_str(), 128);
    HRESULT hr = m_pWaveFile->Open(wcharAudioFile, &wfx, 0);
    if (FAILED(hr)) {
        m_nErrorCode = TRC_WRITING_WAVE_HEADER_FAILED;
        m_strErrorMsg = "Failed in writing the Wave file header.";

        cerr << m_strErrorMsg << endl;
        return false;
    }
    return true;
}

bool CV2dFileCreator::WriteWaveFileData(const unsigned char* a_pData,
        const unsigned int a_nLen)
{
    if ((a_nLen > 0) && (m_bWaveHdrWritten)) {
        UINT nBytesWritten = 0;

        HRESULT hr =
                m_pWaveFile->Write(a_nLen, (BYTE*) a_pData, &nBytesWritten);
        if ((a_nLen != nBytesWritten) || FAILED(hr)) {
            m_nErrorCode = TRC_WRITING_WAVE_DATA_FAILED;
            m_strErrorMsg = "Failed in writing Audio data in wave file.";

            cerr << m_strErrorMsg << endl;
            return false;
        }
    }

    return true;
}

bool CV2dFileCreator::ParseAVFileNames(const std::string& a_rStrOutFile)
{
    std::string strFileName = "";
    int nIndex = a_rStrOutFile.find(".");

    if (nIndex == (int)std::string::npos) {
        strFileName = a_rStrOutFile;
    }
    else {
        strFileName = a_rStrOutFile.substr(0, nIndex);
    }

    if (m_bTranscodeVideo) {
        if (strFileName == "-")
            m_strVideoOutFile = strFileName;
        else
            m_strVideoOutFile = strFileName + ".y4m";
    }

    if (m_bTranscodeAudio)
        m_strAudioOutFile = strFileName + ".wav";

    return true;
}

bool CV2dFileCreator::ParseAVMode(const std::string& a_rStrAVMode)
{
    if (a_rStrAVMode == "both") {
        m_bTranscodeAudio = true;
        m_bTranscodeVideo = true;
    }
    else if (a_rStrAVMode == "audio") {
        m_bTranscodeAudio = true;
        m_bTranscodeVideo = false;
    }
    else if (a_rStrAVMode == "video") {
        m_bTranscodeVideo = true;
        m_bTranscodeAudio = false;
    }

    return true;
}

bool CV2dFileCreator::CheckForPktLoss(CCFrame& a_rCFrame,
        const bool a_bIsSrcRTP, const ePktType a_ePktType)
{
    bool bRetVal = true;
    uint64_t nCurSeqNum = -1;

    uint64_t& nSeqNum = (a_ePktType == eVideoPacket) ? m_nVideoSeq
        : m_nAudioSeq;
    uint64_t& nPktLoss = (a_ePktType == eVideoPacket) ? m_nVideoPktLoss
        : m_nAudioPktLoss;
    uint64_t& nOutOfOrder = (a_ePktType == eVideoPacket) ? m_nVideoOutOfOrder
        : m_nAudioOutOfOrder;

    if (a_bIsSrcRTP)
        nCurSeqNum = RTPGETSEQUENCENUM(a_rCFrame.GetData());
    else
        nCurSeqNum = V2DGETSEQUENCENUM(a_rCFrame.GetData());

    if (nSeqNum && nSeqNum != nCurSeqNum) {
        if (nSeqNum < nCurSeqNum) {
            nPktLoss += (nCurSeqNum - nSeqNum);
            nSeqNum = nCurSeqNum + 1;

            bRetVal = false;
        }
        else if (nSeqNum - nCurSeqNum == 65536) {
            nSeqNum = nCurSeqNum + 1;
        }
        else {
            nOutOfOrder++;
            bRetVal = false;
        }
    }
    else
        nSeqNum = nCurSeqNum + 1;

    return bRetVal;
}

void WriteYUVToBmpFile(const unsigned char *a_pYUV, const size_t a_nWidth,
        const size_t a_nHeight,
        V2dLibColorTransform::YUVToRGBConverter::SubSample a_nSubsample,
        const std::string a_strFile)
{
    V2dLibColorTransform::YUVToRGBConverter oRGB;

    const unsigned char* pY = a_pYUV;
    const unsigned char* pU = pY + (a_nWidth * a_nHeight);
    const unsigned char* pV = pU + (a_nWidth * a_nHeight) / 2;

    unsigned char* pRGB = new unsigned char[a_nWidth * a_nHeight * 3];
    unsigned char* pRGBA = new unsigned char[a_nWidth * a_nHeight * 4];

    oRGB.yuvToRgb(pY, pU, pV, pRGB, a_nWidth, a_nHeight, oRGB.SubSample_420);

    V2dLibColorTransform::ConvertRGBToRGBA(pRGB, pRGBA, a_nWidth, a_nHeight);

    CBitmap oSaveBmp;
    oSaveBmp.SetBitsRGBA(pRGBA, a_nWidth, a_nHeight);
    oSaveBmp.Save(a_strFile.c_str(), 32);

    delete[] pRGB;
    delete[] pRGBA;
}
