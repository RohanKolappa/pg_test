/*
 * V2DFrameServer.cpp
 *
 *  Created on: Sep 19, 2011
 *      Author: rkale
 */

#include "IPVSTranscode_compatibility.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}

#include <math.h>

#include "V2DFrameServer.hh"

#include "srd.h"
#include "v2d.h"
#include "rtp.h"
#include "vframedecoder.hh"
#include "ScapProcessor.hh"

using namespace mediaindex;

#define DEFAULT_AUDIO_FREQUENCY                 16315


// For debugging, use DPRINT() instead of printf() and un-comment one of
// these depending on where  you want to send your output to.
//#define USE_WINDEBUG
//#define USE_PRINTF

#ifdef USE_WINDEBUG
char _DBGPRINTBUF_[256];
#define DPRINTF(...) do { sprintf(_DBGPRINTBUF_, __VA_ARGS__); \
    OutputDebugString(_DBGPRINTBUF_); } while (0)
#else
#ifdef USE_PRINTF
#define DPRINTF printf
#else
#define DPRINTF(...) do {} while (0)
#endif
#endif

#define DPRINTFForce printf


CV2DFrameServer::CV2DFrameServer(std::string & a_strMediaPath, int a_nTargetFPS)
{
    DPRINTF("Creating Frame Server %s fps %d\n",
            a_rStrBlobDir.c_str(), a_nTargetFPS);
    InitializeVariables();

    m_strMediaPath = a_strMediaPath;
    m_nTargetFps = a_nTargetFPS;
    m_scapProcessor = new CScapProcessor();
}

CV2DFrameServer::~CV2DFrameServer()
{
    delete m_pMediaServer;
    delete [] m_pDecodedBuf;
    delete [] m_pScaledBuf;
    delete [] m_pOutFrame;
    delete m_scapProcessor;

    DestroySRD();
    DestroyDecoder();

    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);
    if (m_pDestPicture != NULL)
         av_free(m_pDestPicture);
}

void
CV2DFrameServer::InitializeVariables()
{
    m_nStartTimeOffset = 0;
    m_nTotalCFrames = 0;
    m_nVideoCFrames = 0;
    m_nAudioCFrames = 0;
    m_nControlCFrames = 0;
    m_pSliceRdata = NULL;
    m_nWidth = 0;
    m_nHeight = 0;
    m_nFrameWidth = 0;
    m_nFrameHeight = 0;
    m_nScaledWidth = 0;
    m_nScaledHeight = 0;
    m_nSliceSize = 0;
    m_bGotVideoRes = false;
    m_nCompleteVideoFrames = 0;
    m_pFrameDecoder = NULL;
    m_pDecodedBuf = NULL;
    m_nDecodeBufferLen = 0;
    m_pScaledBuf = NULL;
    m_nScaledBufferLen = 0;
    m_fOutputFramePeriod = 0.0;
    m_strErrorMsg = "";
    m_pMediaServer = NULL;
    m_nErrorCode = FS_SUCCESS;
    m_nVideoFrameCount  = 0;
    m_nCurrentVideoFrameNumber = -1;
    m_nCurrentTimeOffset = 0;
    m_nPreviousTimeOffset = 0;
    m_nTargetTimeOffset = 0;
    m_nCurrentVideoServedOffset = 0;
    m_pOutFrame = NULL;
    m_nOutFrameLen = 0;
    m_nOutFrameBufferLen = 0;
    m_nVideoFramesDecoded = 0;
    m_bSRDOnly = false;
    m_bSkipDecode = false;
    m_pSwsCtx = NULL;
    m_bNeedScaling = false;
    m_bServedScaled = false;
    m_bInitialized = false;
    m_pDestPicture = NULL;
    m_nBytesPerPixel = 4;
    m_scapProcessor = NULL;
    m_bDebugVideo = false;
}

int
CV2DFrameServer::Initialize(int a_nWidth, int a_nHeight)
{
    m_nFrameWidth = a_nWidth;
    m_nFrameHeight = a_nHeight;
    return Initialize();
}

int
CV2DFrameServer::Initialize()
{
    if (m_bInitialized) {
        m_strErrorMsg = "Already Initialized.";
        return FS_ALREADY_INITIALIZED;
    }

    if (!CreateMediaServer()) {
        return m_nErrorCode;
    }

    if (!ReadFramesTillVideoResolutionAcquired(60000, 10000)) {
        return m_nErrorCode;
    }

    ComputeNumberOfVideoFrames();

    SetupFrameDimensions();

    m_bInitialized = true;

    return FS_SUCCESS;
}

bool CV2DFrameServer::CreateMediaServer()
{
    m_pMediaServer = CMediaServer::Create(m_strMediaPath.c_str());

    if (m_pMediaServer == NULL) {
        m_strErrorMsg = "Could not create MediaServer object.";
        m_nErrorCode = FS_MEDIASERVER_CREATION_FAILED;
        return false;
    }

    if (m_pMediaServer->Init() != MSRC_OK) {
        m_strErrorMsg = "MediaServer Init Failed: ";
        m_strErrorMsg.append(m_pMediaServer->GetErrorString());
        m_nErrorCode = FS_MEDIASERVER_INIT_FAILED;
        return false;
    }

    if (!StartMediaServerAtOffset(0)) {
        return false;
    }

    return true;
}

bool CV2DFrameServer::StartMediaServerAtOffset(uint64_t aOffset)
{
    uint64_t nStartTimestamp = m_pMediaServer->GetTimecodeAtOffset(aOffset);
    if (m_pMediaServer->Start(nStartTimestamp) != MSRC_OK) {
        m_strErrorMsg = "MediaServer Start Failed: ";
        m_strErrorMsg.append(m_pMediaServer->GetErrorString());
        m_nErrorCode = FS_MEDIASERVER_START_FAILED;
        return false;
    }
    return true;
}

bool CV2DFrameServer::ReadFramesTillVideoResolutionAcquired(
        uint64_t aTimeOffsetLimit, uint64_t aFrameCountLimit)
{
    m_nErrorCode = FS_SUCCESS;
    m_bGotVideoRes = false;
    m_nCurrentTimeOffset = 0;
    m_nCurrentVideoServedOffset = 0;

    while (!m_bGotVideoRes) {
        if (!ReadFrame())
            break;
        // Better get a video resolution within a reasonable time
        if (m_nCurrentTimeOffset > aTimeOffsetLimit) {
            if (m_nErrorCode != FS_SUCCESS)
                break;
            if (aFrameCountLimit > 0 &&
                    m_nTotalCFrames > aFrameCountLimit)
                break;
        }
    }
    if (!m_bGotVideoRes) {
        if (m_nErrorCode == FS_SUCCESS) {
            m_strErrorMsg = "Media Stream Error";
            m_nErrorCode = FS_STREAM_ERROR;
        }
        return false;
    }

    return true;
}

void CV2DFrameServer::ComputeNumberOfVideoFrames()
{
    uint64_t nMediaDuration =
            m_pMediaServer->GetOffsetFromTimecode(m_pMediaServer->GetMediaEndTimestamp());
    m_nVideoFrameCount = llround((double)(nMediaDuration * m_nTargetFps)/1000.0);
}

void CV2DFrameServer::SetupFrameDimensions()
{
    // If Frame dimensions is not been forced on us, we latch on to the first
    // detected width and height
    if (m_nFrameWidth <= 0 || m_nFrameHeight <= 0) {
        m_nFrameWidth = m_nWidth;
        m_nFrameHeight = m_nHeight;
    }
}

bool CV2DFrameServer::InitializeMediaToOffset(uint64_t a_nMediaOffset) {

    bool retVal = false;
    if (m_pMediaServer == NULL) {
        return retVal;
    }

    m_nOutFrameLen = 0;
    DestroyDecodeBuffer();

    if (m_pScaledBuf != NULL) {
        memset(m_pScaledBuf, 0x00, m_nScaledBufferLen);
    }

    // Initially we build a full IFrame by running last few seconds of video data
    // through SRD. We start at 15 seconds of pre-scan and keep doubling it if we fail
    // to collect a full IFrame. We limit this to maximum of 8 minutes.
    uint32_t margin = 15000;

    while (true) {
        m_pMediaServer->Stop();

        uint64_t adjustedOffset = (a_nMediaOffset > margin) ?
            a_nMediaOffset - margin : 0;

        if (!StartMediaServerAtOffset(adjustedOffset)) {
            retVal = false;
            break;
        }

        InitializeSRD();

        DestroyDecoder();

        if (!ReadFramesTillVideoResolutionAcquired(a_nMediaOffset, 0)) {
            goto nextLoop;
        }

        DPRINTF("Got video resolution at %d\n", (int)m_nCurrentTimeOffset);

        // Now read frames till we reach desired offset
        // All frames only get SRDed
        m_bSRDOnly = true;
        m_nTargetTimeOffset = a_nMediaOffset;
        while (true) {
            if (!RetrieveNextVideoFrame())
                break;
            if (m_nCurrentTimeOffset >= a_nMediaOffset)
                break;
        }
        m_bSRDOnly = false;

    nextLoop:
        // Done if IFrame found or scan limit reached
        if (m_pSliceRdata->OutputIFrameCount >= 1) {
            retVal = true;
            break;
        }
        if (adjustedOffset <= 0) {
            retVal = true;
            break;
        }
        if (margin >= 8 * 60000)
            break;

        margin *= 2;
        DPRINTF("Trying again with margin = %d\n", margin);
    }
    return retVal;
}

void CV2DFrameServer::DestroySRD()
{
    if (m_pSliceRdata != NULL) {
        destroySRD(m_pSliceRdata);
        delete m_pSliceRdata;
        m_pSliceRdata = NULL;
    }
}

void CV2DFrameServer::InitializeSRD()
{
    DestroySRD();

    m_pSliceRdata = new sliceRedData_t;

    if (initSRD(m_pSliceRdata) == 0) {
        setStereoPolicy(m_pSliceRdata, SP_MONO);
        SRDSetCodecVersion(m_pSliceRdata, m_scapProcessor->GetCodecVersion());
    }
}

bool CV2DFrameServer::RetrieveNextVideoFrame()
{
    // Keep reading media frames till a video frame gets collected from SRD
    uint64_t nCurrentFrameCount = m_nCompleteVideoFrames;
    while (m_nCompleteVideoFrames == nCurrentFrameCount) {
        if (!ReadFrame())
            break;
    }
    if (m_nCompleteVideoFrames > nCurrentFrameCount) {
        return true;
    }
    return false;
}

uint8_t * CV2DFrameServer::GetVideoFrame(int a_nFrameNumber)
{
    DPRINTF("Get Video Frame request %d\n", a_nFrameNumber);
    if (a_nFrameNumber < 0 ||
            a_nFrameNumber >= (int) m_nVideoFrameCount) {
        return NULL; // Invalid frame number
    }

    // If the frame number requested jumps
    // we re-initialize the media to the previous frame
    if (m_nCurrentVideoFrameNumber == -1 ||
            a_nFrameNumber != m_nCurrentVideoFrameNumber + 1) {
        uint64_t newTimeOffset = 0;
        if (a_nFrameNumber > 0)
            newTimeOffset = GetOffsetAtFrameNumber(a_nFrameNumber - 1);
        DPRINTF("Jump Frame: %d %d\n", (int)newTimeOffset, (int)m_nCurrentTimeOffset);
        m_nCurrentVideoFrameNumber = a_nFrameNumber - 1 ;
        InitializeMediaToOffset(newTimeOffset);
        DecodeFrame(m_nPreviousTimeOffset);

        // Collect the pending frame from SRD
        DPRINTF("Collecting pending SRD frame at %d\n", (int)m_nCurrentTimeOffset);
        CollectFrameFromSRD();

    }

    // The usual case is that the very next frame is requested.
    if (a_nFrameNumber == m_nCurrentVideoFrameNumber + 1) {

        uint64_t nTimeOffset = GetOffsetAtFrameNumber(a_nFrameNumber);
        m_nCurrentVideoFrameNumber = a_nFrameNumber;

        // If next frame to decode is in the future,
        // repeat the frame we already decoded.
        // May be initial black screen for first few frames.
        DPRINTF("Next Frame: %d %d\n", (int)nTimeOffset, (int)m_nCurrentTimeOffset);
        if (nTimeOffset < m_nCurrentTimeOffset) {
            return GetVideoFrameBuffer();
        }
        else {
            // Keep decoding and retrieving frames from SRD
            // till offset reaches what is needed to be served
            while (nTimeOffset >= m_nCurrentTimeOffset) {
                DPRINTF("Next Frame Loop: %d %d\n", (int)nTimeOffset, (int)m_nCurrentTimeOffset);
                DecodeFrame(m_nCurrentTimeOffset);
                if (!RetrieveNextVideoFrame()) {
                    break;
                }
            }
            return GetVideoFrameBuffer();
        }
    }
    return NULL;
}

uint8_t * CV2DFrameServer::GetVideoFrameBuffer()
{
    if (m_pScaledBuf)
        return m_pScaledBuf;
    return m_pDecodedBuf;
}

void CV2DFrameServer::DecodeFrame(uint64_t a_nOffset)
{
    if (m_bSkipDecode)
        return;

    if (m_nOutFrameLen <= 0)
        return;

    CreateFrameDecoderIfNeeded();

    DPRINTF("Decoding a frame %d of size %d\n",
            m_nCurrentVideoFrameNumber, m_nOutFrameLen);
    m_pFrameDecoder->Decode(m_pOutFrame, m_nOutFrameLen,
            m_pDecodedBuf, m_nDecodeBufferLen);

    if (m_bNeedScaling) {
        if (m_pSwsCtx == NULL)
            SetupScaler();
        m_bServedScaled = true;

        const uint8_t* srcSlice[3] = {m_pDecodedBuf, 0, 0};
        const int srcStride[3] = {m_nWidth * m_nBytesPerPixel, 0, 0};
        sws_scale(m_pSwsCtx, srcSlice, srcStride,
                0, m_nHeight,
                m_pDestPicture->data,
                m_pDestPicture->linesize);
    }
    else {
        ::memcpy(m_pScaledBuf, m_pDecodedBuf, m_nDecodeBufferLen);
        m_bServedScaled = false;
    }
    m_nVideoFramesDecoded++;
    m_nCurrentVideoServedOffset = a_nOffset;
    m_nOutFrameLen = 0;

}

uint64_t CV2DFrameServer::GetOffsetAtFrameNumber(int a_nFrameNumber)
{
    return llround((double)(a_nFrameNumber * 1000.0)/m_nTargetFps);
}

bool
CV2DFrameServer::ReadFrame()
{
    CCFrame cFrame;

    int nRetVal = m_pMediaServer->GetFrame(cFrame);

    if (nRetVal == MSRC_END_OF_MEDIA)
        return false;

    if (nRetVal != MSRC_OK) {
        m_nErrorCode = FF_MEDIASERVER_GETFRAME_FAILED;
        m_strErrorMsg = "MediaServer Get Frame Failed: ";
        m_strErrorMsg.append(m_pMediaServer->GetErrorString());
        return false;
    }
    m_nTotalCFrames++;

    if (cFrame.GetMessageType() != CCFrame::Normal) {
        switch (cFrame.GetMessageType()) {
        case CCFrame::MediaStarted:
            break;

        case CCFrame::MediaStopped:
            break;

        default:
            break;
        }
        return true;
    }

    m_nPreviousTimeOffset = m_nCurrentTimeOffset;
    m_nCurrentTimeOffset = cFrame.GetTimeOffset();

    HandleCFrame(cFrame);
    return true;
}

bool
CV2DFrameServer::HandleCFrame(CCFrame &a_rCFrame)
{
    bool bRetVal = false;

    if (a_rCFrame.GetStreamType(STREAMTYPE_V2D) == STREAMTYPE_V2D) {
        CV2DStreamMediaFlag v2dSMediaFlag(a_rCFrame.GetMediaFlag());
        const short nChannel = v2dSMediaFlag.GetChannel();

        switch (nChannel) {
        case FRAMETYPE_V2D_CHAN_VIDEO:
            bRetVal = HandleVideoCFrame(a_rCFrame, v2dSMediaFlag);
            break;

        case FRAMETYPE_V2D_CHAN_AUDIO:
//            bRetVal = HandleAudioCFrame(a_rCFrame, v2dSMediaFlag);
            break;

        case FRAMETYPE_V2D_CHAN_CTL:
            bRetVal = HandleControlCFrame(a_rCFrame);
            break;
        }
    }
    else {
        m_nErrorCode = FS_STREAMTYPE_NOTSUPPORTED;
        m_strErrorMsg  = "Found a non V2D frame.";
        bRetVal = false;
    }

    return bRetVal;
}

bool
CV2DFrameServer::HandleControlCFrame(CCFrame &a_rCFrame)
{
    char *pCtlData = reinterpret_cast<char *> (a_rCFrame.GetData());
    if (!m_scapProcessor->Process(pCtlData))
        return true;

    if (m_scapProcessor->ResolutionChangeDetected()) {
        DPRINTF("Resolution changed %d: %dx%d\n",
                m_nCurrentVideoFrameNumber, newWidth, newHeight);

        if (m_bSRDOnly) {
            // Should collect SRD frame and decode it since we have to serve this
            // if new frame cannot be picked up after resolution change
            CollectFrameFromSRD();
            DecodeFrame(m_nPreviousTimeOffset);
        }
        InitializeSRD();

        m_nWidth = m_scapProcessor->GetWidth();
        m_nHeight = m_scapProcessor->GetHeight();
        m_nSliceSize = m_scapProcessor->GetSliceSize();
    }

    if (m_pSliceRdata != NULL) {
        m_pSliceRdata->MaxSliceNo = m_scapProcessor->GetMaxSliceNumber();
        m_pSliceRdata->sliceSize = m_scapProcessor->GetSliceSize();
        setStereoVideo(m_pSliceRdata, m_scapProcessor->GetStereoMode());
        setCompMode(m_pSliceRdata, m_scapProcessor->GetVideoCompMode());
        setMotionCompMode(m_pSliceRdata, m_scapProcessor->GetVideoMotionCompMode());
    }

    if (m_nWidth > 0 && m_nHeight > 0 && m_nSliceSize > 0) {
        m_bGotVideoRes = true;

        if (m_scapProcessor->ResolutionChangeDetected()) {
            DestroyDecoder();
        }

        if (m_pDecodedBuf == NULL)
            CreateDecodeBuffer();
    }

    CheckScaling();

    if (m_pScaledBuf == NULL && m_nFrameWidth > 0 && m_nFrameHeight > 0) {
        m_nScaledBufferLen = avpicture_get_size(PIX_FMT_RGB32, m_nFrameWidth, m_nFrameHeight);
        m_pScaledBuf = new uint8_t [m_nScaledBufferLen];
        memset(m_pScaledBuf, 0x00, m_nScaledBufferLen);
    }

    if (m_bNeedScaling &&
            m_scapProcessor->ResolutionChangeDetected()) {
        DestroyScaler();
    }

    return true;
}


bool CV2DFrameServer::CreateFrameDecoderIfNeeded()
{
    if (m_nWidth == 0 || m_nHeight == 0)
        return false;

    if (!m_bInitialized)
        return false;

    if (m_pFrameDecoder != NULL)
        return false;

    DestroyDecoder();

    v2dDecConfiguration decConfig;
    decConfig.m_nWidth        = m_nWidth;
    decConfig.m_nHeight       = m_nHeight;
    decConfig.m_nSliceSize    = m_nSliceSize;
    decConfig.m_eDecodeFormat = eBGRX32;  // TODO: handle YUV case
    decConfig.m_eEncodeFormat = m_scapProcessor->GetEncodeFormat();

    m_pFrameDecoder = CVideoFrameDecoder::CreateObj(&decConfig);
    if (m_pFrameDecoder == NULL) {
        m_nErrorCode = FS_VIDEOFRAMEDECODER_CREATION_FAILED;
        m_strErrorMsg = "Failed in creating VideoFrameDecoder object";
        return false;
    }

    if (m_bDebugVideo) {
        m_pFrameDecoder->SetSliceDebug(true);
    }
    DPRINTF("Created new decoder %d\n", m_nCurrentVideoFrameNumber);

    CreateDecodeBuffer();

    return true;
}

void CV2DFrameServer::DestroyDecoder()
{
    if (m_pFrameDecoder != NULL) {
        delete m_pFrameDecoder;
        m_pFrameDecoder = NULL;
    }
}

bool CV2DFrameServer::HandleVideoCFrame(CCFrame &a_rCFrame,
        const CV2DStreamMediaFlag &a_rV2dSMediaFlag)
{
    if (m_pSliceRdata == NULL)
        return false;

    const uint32_t nSampleTotalLength = a_rCFrame.GetLength();
    const bool bIsVideoSrcRTP = (a_rV2dSMediaFlag.GetEncaps()
            == FRAMETYPE_V2D_ENCAPS_RTP);

    if (m_bSRDOnly && m_nCurrentTimeOffset >= m_nTargetTimeOffset) {
        CollectFrameFromSRD();
    }
    m_nVideoCFrames++;

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

    if (m_bSRDOnly) {
        m_nCompleteVideoFrames++;
        return true;
    }
    CollectFrameFromSRD();

    return true;
}

void
CV2DFrameServer::CollectFrameFromSRD()
{
    if (m_pSliceRdata == NULL)
        return;

    if (get_hoFrame(m_pSliceRdata) != -1) {
        m_nOutFrameLen = getHoFrameSize(m_pSliceRdata);

        if (m_pOutFrame == NULL || m_nOutFrameBufferLen < m_nOutFrameLen) {
            m_nOutFrameBufferLen = m_nOutFrameLen;
            m_pOutFrame = new uint8_t [m_nOutFrameBufferLen];
        }
        copyHoFrameToBuffer(m_pSliceRdata, m_pOutFrame, m_nOutFrameLen);

        m_nCompleteVideoFrames++;

    }
}

void CV2DFrameServer::CheckScaling()
{
    m_bNeedScaling = false;
    if (!m_bGotVideoRes)
        return;

    if (m_nFrameWidth == 0 || m_nFrameHeight == 0)
        return;

    if (m_nWidth != m_nFrameWidth || m_nHeight != m_nFrameHeight) {
        m_bNeedScaling = true;

        // Compute scaled dimensions so that aspect ratio is maintained
        double aspectRatio = ((double)m_nWidth)/m_nHeight;
        int targetHeight = (int)rint((double)m_nFrameWidth / aspectRatio);
        if (targetHeight <= m_nFrameHeight) {
            m_nScaledHeight = targetHeight;
            m_nScaledWidth = m_nFrameWidth;
            DPRINTF("Fitting to width %d, height set to %d\n", m_nScaledWidth,  m_nScaledHeight);
        }
        else {
            m_nScaledWidth = (int)rint((double)m_nFrameHeight * aspectRatio);
            m_nScaledHeight = m_nFrameHeight;
            DPRINTF("Fitting to height %d, width set to %d\n", m_nScaledHeight, m_nScaledWidth);
        }
    }

}

void CV2DFrameServer::SetupScaler()
{
    enum PixelFormat dst_pix_fmt;
    dst_pix_fmt = PIX_FMT_RGB32;  // BGRX on little-endian

    enum PixelFormat src_pix_fmt = PIX_FMT_RGB32;

    DPRINTF("Setting up scaler %dx%d -> %dx%d within %dx%d\n",
            m_nWidth, m_nHeight, m_nScaledWidth, m_nScaledHeight,m_nFrameWidth, m_nFrameHeight);
    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);
    m_pSwsCtx = sws_getContext(
            m_nWidth, m_nHeight, src_pix_fmt,
            m_nScaledWidth, m_nScaledHeight, dst_pix_fmt,
            SWS_BICUBIC, NULL, NULL, NULL);

    memset(m_pScaledBuf, 0x00, m_nScaledBufferLen);

    if (m_pDestPicture == NULL)
        m_pDestPicture = avcodec_alloc_frame();

    // In case scaled dimension do not fit the frame dimension, we need to compute
    // offset into the painting area so that black borders are drawn appropriately
    int offset = 0;
    if (!((m_nFrameWidth == m_nScaledWidth) && (m_nFrameHeight == m_nScaledHeight))) {
        DPRINTF("Aspect ratio mismatch: need black borders\n");
        if (m_nFrameWidth > m_nScaledWidth) {
            // Horizontal offset needed
            offset = (m_nFrameWidth - m_nScaledWidth)/2;
            offset = offset * m_nBytesPerPixel;
        }
        else {
            // Vertical offset needed
            offset = (m_nFrameHeight - m_nScaledHeight)/2;
            offset = offset * m_nFrameWidth * m_nBytesPerPixel;
        }
    }
    avpicture_fill((AVPicture *)m_pDestPicture, (uint8_t *)(m_pScaledBuf + offset),
            dst_pix_fmt, m_nFrameWidth, m_nFrameHeight);

}

void CV2DFrameServer::DestroyScaler()
{
    if (m_pSwsCtx != NULL)
        sws_freeContext(m_pSwsCtx);
    m_pSwsCtx = NULL;
}

void
CV2DFrameServer::CreateDecodeBuffer()
{
    // Create buffer to decode video into
    m_nDecodeBufferLen = m_nWidth * m_nHeight * m_nBytesPerPixel;
    delete [] m_pDecodedBuf;
    m_pDecodedBuf = new unsigned char[m_nDecodeBufferLen];

    // Initially fill buffer with black video
    ::memset(m_pDecodedBuf, 0x00, m_nDecodeBufferLen);
}

void
CV2DFrameServer::DestroyDecodeBuffer()
{
    delete [] m_pDecodedBuf;
    m_pDecodedBuf = NULL;
}

void
CV2DFrameServer::EnableDebug()
{
    m_bDebugVideo = true;
}

int
CV2DFrameServer::GetWidth()
{
    return m_nWidth;
}

int
CV2DFrameServer::GetHeight()
{
    return m_nHeight;
}

int
CV2DFrameServer::GetFrameWidth()
{
    return m_nFrameWidth;
}

int
CV2DFrameServer::GetFrameHeight()
{
    return m_nFrameHeight;
}

uint64_t
CV2DFrameServer::GetVideoFrameCount()
{
    return m_nVideoFrameCount;
}

int
CV2DFrameServer::GetVideoFrameSize()
{
    return m_nFrameWidth * m_nFrameHeight * m_nBytesPerPixel;
}

uint64_t
CV2DFrameServer::GetVideoFramesDecoded()
{
    return m_nVideoFramesDecoded;
}

uint64_t
CV2DFrameServer::GetCurrentVideoServedOffset()
{
    return m_nCurrentVideoServedOffset;
}

void
CV2DFrameServer::SkipDecode(bool m_bValue)
{
    // Used for speeding up testing.
    m_bSkipDecode = m_bValue;
}

const std::string &
CV2DFrameServer::GetErrorString()
{
    return m_strErrorMsg;
}
