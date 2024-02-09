/*
 * AdaptiveMediaGenerator.hh
 *
 *  Created on: Dec 10, 2013
 *      Author: rkale
 */

#ifndef ADAPTIVEMEDIAGENERATOR_HH_
#define ADAPTIVEMEDIAGENERATOR_HH_

#include <string>
#include "IdProvider.hh"
#include "sframe.hh"

#include "MediaProcessorInterface.hh"
#include "VMP.DMS.RTP2HLS.h"

class CDataDropper;

class CAdaptiveMediaGenerator : public mediaindex::CMediaProcessorInterface {

public:
    CAdaptiveMediaGenerator(const std::string & mediaDirectory,
            const std::string & sdpFilePath,
            uint32_t a_nPVRLen,
            bool a_bSyncWrites);
    ~CAdaptiveMediaGenerator();

    void ProcessFrame(CSFrame * sFrame);

    void Start(uint64_t nTimeStamp);
    void Stop(uint64_t nTimeStamp);

    void StartMedia(uint64_t a_nStartTime);
    void StopMedia(uint64_t a_nStopTime);
    void ProcessMediaFrame(mediaindex::CCFrame & a_rFrame);

    void StopProcess();

    uint64_t GetBytesWritten();
    uint64_t GetBytesDeleted();
    uint64_t GetTotalProcessedFrameCount();
    uint64_t GetTotalStartFrameCount();
    uint64_t GetTotalStopFrameCount();
    uint64_t GetTotalVFramesSentToHLS();
    uint64_t GetTotalAFramesSentToHLS();

    void SetVideoDropPercentage(double a_dVideoDropPercentage,
                                int a_nVideoDropRepeats = 1,
                                int a_nRandomSeed = 0);
    void SetAudioDropPercentage(double a_dAudioDropPercentage,
                                int a_nAudioDropRepeats = 1,
                                int a_nRandomSeed = 0);

    uint32_t GetVideoFifoSize();
    uint32_t GetAudioFifoSize();

    uint64_t GetVideoDropCount();
    uint64_t GetAudioDropCount();

private:

    void CreateHLSOutputDirectory();
    void SpawnHLSGenerator();
    void TerminateHLSGenerator();
    void ParseSDPURL();

    bool IsHLSAcceptingData(bool bVideo, bool IsFullSpeedMode);

    uint32_t GetVideoMaxSize();
    uint32_t GetAudioMaxSize();

    bool m_bStoppedProcess;
    std::string m_strMediaDirectory;
    std::string m_strHLSOutputDirectory;
    std::string m_strSDPFilePath;
    std::string m_strSDPURL;
    int m_nUniqueId;
    bool m_bError;
    bool m_bSyncWrites;
    static CIdProvider m_IdProvider;
    uint32_t m_nPVRLen;

    VMP::DMS::RTP2HLS *	rtp2hls;

    uint32_t m_nMaxVideoFifoSize;
    uint32_t m_nMaxAudioFifoSize;
    bool m_bIsHLSAcceptingData;

    uint64_t m_nTotalProcessedFrameCount;
    uint64_t m_nTotalStartFrameCount;
    uint64_t m_nTotalStopFrameCount;

    uint64_t m_nVideoFramesProcessed;
    uint64_t m_nAudioFramesProcessed;
    uint64_t m_nVideoFramesSentToHLS;
    uint64_t m_nAudioFramesSentToHLS;

    uint64_t m_nVideoDropCount;
    uint64_t m_nAudioDropCount;

    //To force buffer drops
    CDataDropper *m_pVideoDropper;
    CDataDropper *m_pAudioDropper;
};


#endif /* ADAPTIVEMEDIAGENERATOR_HH_ */
