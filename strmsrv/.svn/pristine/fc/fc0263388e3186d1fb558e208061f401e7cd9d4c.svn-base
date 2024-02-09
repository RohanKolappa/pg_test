/*
 * AdaptiveMediaGenerator.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include <sstream>

#include "common.hh"
#include "AdaptiveMediaGenerator.hh"
#include "FFUtils.hh"
#include "DataDropper.hh"

using namespace std;

CIdProvider CAdaptiveMediaGenerator::m_IdProvider(1);

class RTP2HLSIntitializer
{

public:
    RTP2HLSIntitializer()
     {
        const char *env_value = getenv("RTP2HLSLOGLEVEL");
        int loglevel = (env_value != NULL) ? atoi(env_value) : 1;
        VMP::DMS::RTP2HLS::setLogLevel(loglevel);
     }
};

static RTP2HLSIntitializer rtp2hlsintitializer;

CAdaptiveMediaGenerator::CAdaptiveMediaGenerator(
        const std::string & mediaDirectory,
        const std::string & sdpFilePath,
        uint32_t a_nPVRLen,
        bool a_bSyncWrites)
{

    m_bStoppedProcess = false;
    m_strMediaDirectory = mediaDirectory;
    m_strSDPURL = sdpFilePath;
    m_nPVRLen = a_nPVRLen;
    m_bError = false;
    m_bIsHLSAcceptingData = true;

    m_nMaxVideoFifoSize = 0;
    m_nMaxAudioFifoSize = 0;

    m_nTotalProcessedFrameCount = 0;
    m_nTotalStartFrameCount = 0;
    m_nTotalStopFrameCount = 0;
    m_nVideoFramesProcessed = 0;
    m_nAudioFramesProcessed = 0;
    m_nVideoFramesSentToHLS = 0;
    m_nAudioFramesSentToHLS = 0;

    m_nVideoDropCount = 0;
    m_nAudioDropCount = 0;

    m_pVideoDropper = NULL;
    m_pAudioDropper = NULL;

    m_bSyncWrites = a_bSyncWrites;

    SYSLOG_INFO("SDPFilePath %s", m_strSDPFilePath.c_str());

    ParseSDPURL();
    CreateHLSOutputDirectory();
    SpawnHLSGenerator();
}

CAdaptiveMediaGenerator::~CAdaptiveMediaGenerator()
{
    StopProcess();
    m_IdProvider.ReleaseId(m_nUniqueId);

    delete m_pVideoDropper;
    delete m_pAudioDropper;
}

void CAdaptiveMediaGenerator::CreateHLSOutputDirectory()
{
    m_strHLSOutputDirectory = m_strMediaDirectory + "/hls";
    CFFUtils::CreateDirectory(m_strHLSOutputDirectory);
}

void CAdaptiveMediaGenerator::SpawnHLSGenerator()
{

    // If NAS is configured, m_bSyncWrites is set to true
    // RTP2HLS should do the equivalent of setting the O_SYNC
    // on all files "open()" ed for writing

    rtp2hls = new VMP::DMS::RTP2HLS;

    // Forcing Sync Write to be false. Will optimize NAS case later
    m_bSyncWrites = false;

    // Fake close
    bool bFakeClose = false;
    //    if (m_nPVRLen == 0)
    //    bFakeClose = true;

    try {
        rtp2hls->init(
            m_strSDPFilePath.c_str(),
            true,               // Multiple SSRC's.
            m_strHLSOutputDirectory.c_str(),
            "segment-",
            2,
            "output.m3u8",
            "http://IPADDRESS/hlsmedia/",
            m_nPVRLen,
            m_bSyncWrites,
            bFakeClose );        // Fake close.

        m_nMaxVideoFifoSize = GetVideoMaxSize();
        m_nMaxAudioFifoSize = GetAudioMaxSize();

        rtp2hls->play();
    }
    catch ( exception & ) {
        m_bError = true;
        SYSLOG_ERROR( "Failed to start RTP2HLS agent!" );
    }

}

// Kill the process as gracefully and as quickly as possible
void CAdaptiveMediaGenerator::TerminateHLSGenerator()
{
    if (m_bStoppedProcess)
        return;

    if (m_bError == false) {
        try {
            rtp2hls->stop();
        }
        catch ( exception & ) {
            m_bError = true;
            SYSLOG_ERROR( "Failed to stop RTP2HLS agent!" );
        }
    }

    delete rtp2hls;

    rtp2hls = 0;

}

void CAdaptiveMediaGenerator::ProcessFrame(CSFrame * sFrame)
{
    if (m_bError)
        return;

    char * pData = sFrame->GetData();
    int nLen = sFrame->GetLength();
    unsigned long long tstmp = sFrame->GetTimeStamp();

    if (sFrame->GetMessageType() == CSFrame::Normal) {
        m_nTotalProcessedFrameCount++;

        unsigned long nStreamType = sFrame->GetStreamType();

        if (nStreamType == STREAMTYPE_RTP) {
            CRTPStreamMediaFlag rtpSMediaFlag(sFrame->GetMediaFlag());
            switch (rtpSMediaFlag.GetEncaps()) {
            case FRAMETYPE_RTP_ENCAPS_RTP:
                switch (rtpSMediaFlag.GetChannel()) {
                case FRAMETYPE_RTP_CHAN_VIDEO:
                    m_nVideoFramesProcessed++;

                    if (m_pVideoDropper && m_pVideoDropper->ShouldDrop())
                        break;

                    if (IsHLSAcceptingData(true, false)) {
                        rtp2hls->putVideoFrame( pData, nLen, tstmp );
                        m_nVideoFramesSentToHLS++;
                    }
                    else {
                        m_nVideoDropCount++;
                    }

                    break;

                case FRAMETYPE_RTP_CHAN_AUDIO:
                    m_nAudioFramesProcessed++;

                    if (m_pAudioDropper && m_pAudioDropper->ShouldDrop())
                        break;

                    if (IsHLSAcceptingData(false, false)) {
                        rtp2hls->putAudioFrame( pData, nLen, tstmp );
                        m_nAudioFramesSentToHLS++;
                    }
                    else {
                        m_nAudioDropCount++;
                    }

                    break;

                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }

}

void CAdaptiveMediaGenerator::ParseSDPURL()
{
    size_t foundPos = m_strSDPURL.find("://");

    if (foundPos != std::string::npos)
        m_strSDPFilePath = m_strSDPURL.substr(foundPos + 3, m_strSDPURL.length());
    else
        m_strSDPFilePath = m_strSDPURL;
}

void CAdaptiveMediaGenerator::Start(uint64_t nTimeStamp)
{
    m_nTotalStartFrameCount++;

    if (m_bError)
        return;

    //Resume HLS Media.
    rtp2hls->resume(nTimeStamp);
}

void CAdaptiveMediaGenerator::Stop(uint64_t nTimeStamp)
{
    m_nTotalStopFrameCount++;

    if (m_bError)
        return;

    //Pause HLS Media.
    rtp2hls->pause(nTimeStamp);
}

void CAdaptiveMediaGenerator::StartMedia(uint64_t a_nStartTime)
{
    if (rtp2hls == NULL || m_bError == true)
        return;

    rtp2hls->resume(a_nStartTime);

    m_nTotalStartFrameCount++;
}

void CAdaptiveMediaGenerator::StopMedia(uint64_t a_nStopTime)
{
    if (rtp2hls == NULL || m_bError == true)
        return;

    rtp2hls->pause(a_nStopTime);

    m_nTotalStopFrameCount++;
}

void CAdaptiveMediaGenerator::ProcessMediaFrame(mediaindex::CCFrame & a_rFrame)
{
    if (rtp2hls == NULL || m_bError == true)
        return;

    if (!m_bIsHLSAcceptingData)
        return;

    uint8_t * pData = a_rFrame.GetData();
    int nLen = a_rFrame.GetLength();
    unsigned long long tstmp = a_rFrame.GetTimeStamp();

    if (a_rFrame.GetMessageType() == mediaindex::CCFrame::Normal) {

        unsigned long nStreamType = a_rFrame.GetStreamType();

        if (nStreamType == STREAMTYPE_RTP) {
            CRTPStreamMediaFlag rtpSMediaFlag(a_rFrame.GetMediaFlag());
            switch (rtpSMediaFlag.GetEncaps()) {
            case FRAMETYPE_RTP_ENCAPS_RTP:
                switch (rtpSMediaFlag.GetChannel()) {
                case FRAMETYPE_RTP_CHAN_VIDEO:
                    m_nVideoFramesProcessed++;

                    if (m_pVideoDropper && m_pVideoDropper->ShouldDrop())
                        break;

                    m_bIsHLSAcceptingData = IsHLSAcceptingData(true, true);
                    if (m_bIsHLSAcceptingData) {
                        rtp2hls->putVideoFrame( (char*)pData, nLen, tstmp );
                        m_nVideoFramesSentToHLS++;
                    }

                    break;

                case FRAMETYPE_RTP_CHAN_AUDIO:
                    m_nAudioFramesProcessed++;

                    if (m_pAudioDropper && m_pAudioDropper->ShouldDrop())
                        break;

                    m_bIsHLSAcceptingData = IsHLSAcceptingData(false, true);
                    if (m_bIsHLSAcceptingData) {
                        rtp2hls->putAudioFrame( (char*)pData, nLen, tstmp );
                        m_nAudioFramesSentToHLS++;
                    }

                    break;

                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }
    m_nTotalProcessedFrameCount++;
}

void CAdaptiveMediaGenerator::StopProcess()
{
    TerminateHLSGenerator();

    m_bStoppedProcess = true;
}

uint64_t CAdaptiveMediaGenerator::GetBytesWritten()
{
    if (rtp2hls == NULL)
        return 0;

    uint64_t nBytesWritten = 0;
    uint64_t nBytesDeleted = 0;

    rtp2hls->getDiskStats(nBytesWritten, nBytesDeleted);

    return nBytesWritten;
}

uint64_t CAdaptiveMediaGenerator::GetBytesDeleted()
{
    if (rtp2hls == NULL)
        return 0;

    uint64_t nBytesWritten = 0;
    uint64_t nBytesDeleted = 0;

    rtp2hls->getDiskStats(nBytesWritten, nBytesDeleted);

    return nBytesDeleted;
}

uint64_t CAdaptiveMediaGenerator::GetTotalProcessedFrameCount()
{
    return m_nTotalProcessedFrameCount;
}

uint64_t CAdaptiveMediaGenerator::GetTotalStartFrameCount()
{
    return m_nTotalStartFrameCount;
}

uint64_t CAdaptiveMediaGenerator::GetTotalStopFrameCount()
{
    return m_nTotalStopFrameCount;
}

void CAdaptiveMediaGenerator::SetVideoDropPercentage(double a_dVideoDropPercentage,
                                                     int a_nVideoDropRepeats,
                                                     int a_nRandSeed)
{
    if (m_pVideoDropper != NULL)
    {
        delete m_pVideoDropper;
        m_pVideoDropper = NULL;
    }

    m_pVideoDropper = new CDataDropper(a_dVideoDropPercentage,
                                       a_nVideoDropRepeats,
                                       a_nRandSeed);
}

void CAdaptiveMediaGenerator::SetAudioDropPercentage(double a_dAudioDropPercentage,
                                                     int a_nAudioDropRepeats,
                                                     int a_nRandSeed)
{
    if (m_pAudioDropper != NULL)
    {
        delete m_pAudioDropper;
        m_pAudioDropper = NULL;
    }

    m_pAudioDropper = new CDataDropper(a_dAudioDropPercentage,
                                       a_nAudioDropRepeats,
                                       a_nRandSeed);
}

uint64_t CAdaptiveMediaGenerator::GetTotalVFramesSentToHLS()
{
    return m_nVideoFramesSentToHLS;
}

uint64_t CAdaptiveMediaGenerator::GetTotalAFramesSentToHLS()
{
    return m_nAudioFramesSentToHLS;
}

uint32_t CAdaptiveMediaGenerator::GetVideoFifoSize()
{
    if (rtp2hls == NULL)
        return 0;

    return rtp2hls->getVideoFifoSize();
}

uint32_t CAdaptiveMediaGenerator::GetAudioFifoSize()
{
    if (rtp2hls == NULL)
        return 0;

    return rtp2hls->getAudioFifoSize();
}

bool CAdaptiveMediaGenerator::IsHLSAcceptingData(bool bVideo,
                                                 bool IsFullSpeedMode)
{
    uint32_t nFifoSize = (bVideo ? GetVideoFifoSize() : GetAudioFifoSize());
    uint32_t nMaxFifoSize = (bVideo ? m_nMaxVideoFifoSize : m_nMaxAudioFifoSize);

    if (IsFullSpeedMode) {
        int nTotalTime = 0;
        do {
            if (nTotalTime >= 5) {
                printf("Aborting HLS Processing, because it reached the max fifo sizes\n");
                return false;
            }

            if (nFifoSize >= (nMaxFifoSize - 1)) {
                sleep(1);
                nTotalTime += 1;

                nFifoSize = (bVideo ? GetVideoFifoSize() : GetAudioFifoSize());
                continue;
            }
            else
                break;

        }while(1);
    } else {
        if (nFifoSize >= (nMaxFifoSize - 1))
            return false;
    }

    return true;
}

uint32_t CAdaptiveMediaGenerator::GetVideoMaxSize()
{
    //Need to implement getVideoMaxSize and getAudioMaxSize methods in RTP2HLS
    //For not we are using 10000
    return 10000;
}

uint32_t CAdaptiveMediaGenerator::GetAudioMaxSize()
{
    //Need to implement getVideoMaxSize and getAudioMaxSize methods in RTP2HLS
    //For not we are using 10000
    return 10000;
}

uint64_t CAdaptiveMediaGenerator::GetVideoDropCount()
{
    return m_nVideoDropCount;
}

uint64_t CAdaptiveMediaGenerator::GetAudioDropCount()
{
    return m_nAudioDropCount;
}
