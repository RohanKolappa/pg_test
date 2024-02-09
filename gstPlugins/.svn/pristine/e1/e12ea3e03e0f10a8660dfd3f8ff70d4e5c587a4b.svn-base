#include <stdio.h>
#include <arpa/inet.h>

#include "mediawriter_cwrapper.h"

#include "../mediaindex/MediaServer.hh"
#include "../mediaindex/MediaWriter.hh"
#include "../strmsrv/FFMediaProcessor.hh"
#include "../strmsrv/AdaptiveMediaGenerator.hh"

using namespace mediaindex;

struct mediawriterWrapper_t {
    mediaindex::CMediaWriter *pMediaWriter;
    CFFMediaProcessor *pTNGenerator;
    CAdaptiveMediaGenerator *pHLSGenerator;

    bool bCreateHLS;
    bool bCreatePrimaryTN;

    std::string strSDPFilePath;
};

struct mediawriterWrapper_t* createMediaWriter(char *pDestPath)
{
    mediawriterWrapper_t *pWriter = new mediawriterWrapper_t();
    if (pWriter == NULL)
        return NULL;

    pWriter->bCreateHLS = false;
    pWriter->bCreatePrimaryTN = false;
    pWriter->pHLSGenerator = NULL;
    pWriter->pTNGenerator = NULL;

    pWriter->pMediaWriter = new mediaindex::CMediaWriter(pDestPath);
    if (pWriter->pMediaWriter == NULL)
    {
        delete pWriter;
        return NULL;
    }
    if (pWriter->pMediaWriter->Init() != MSRC_OK)
    {
        //MediaWriter Init failed.
        delete pWriter->pMediaWriter;
        delete pWriter;
        return NULL;
    }

    return pWriter;
}

void destroyMediaWriter(struct mediawriterWrapper_t* a_pMediaWriter)
{
    if (a_pMediaWriter != NULL)
    {
        delete a_pMediaWriter->pMediaWriter;
        a_pMediaWriter->pMediaWriter = NULL;
    }

    delete a_pMediaWriter;
    a_pMediaWriter = NULL;
}

void startMediaWriter(struct mediawriterWrapper_t* a_pWriter, 
                      uint64_t nStartTime, int nHLS, int nTN)
{
    if (a_pWriter == NULL)
        return;

    if (a_pWriter->pMediaWriter != NULL)
    {
        a_pWriter->pMediaWriter->Start(nStartTime);
    }

    a_pWriter->bCreateHLS = (nHLS == 0) ? false : true;
    a_pWriter->bCreatePrimaryTN = (nTN == 0) ? false : true;
}

void stopMediaWriter(struct mediawriterWrapper_t* a_pWriter, 
                     uint64_t nStopTime)
{
    if ((a_pWriter != NULL) &&
        (a_pWriter->pMediaWriter != NULL))
    {
        a_pWriter->pMediaWriter->Stop(nStopTime);
    }
}

void putMediaFrame(struct mediawriterWrapper_t* a_pWriter, uint8_t* pData,
                   int nSize, uint64_t nTimeStamp, unsigned long nMediaFlags)
{
    if ((a_pWriter == NULL) || 
       (a_pWriter->pMediaWriter == NULL))
        return;

    CCFrame newFrame;
    newFrame.SetData(pData);
    newFrame.SetLength(nSize);
    newFrame.SetTimeStamp(nTimeStamp);
    newFrame.SetMediaFlag(nMediaFlags);

    a_pWriter->pMediaWriter->PutFrame(newFrame);
}

void startMediaWriterForHLS(struct mediawriterWrapper_t * pWriter, 
                            uint64_t nStartTime, char *pBlobDir,
                            char *pSDPFilePath)
{
    if (pWriter == NULL)
        return;

    std::string strBlobDir(pBlobDir);
    std::string strSDPFilePath(pSDPFilePath);

    pWriter->pHLSGenerator = new CAdaptiveMediaGenerator (strBlobDir, 
                                                          strSDPFilePath, 
                                                          0, false);
    if (pWriter->pHLSGenerator == NULL)
        return;

    pWriter->pHLSGenerator->StartMedia(nStartTime);
}

void stopMediaWriterForHLS(struct mediawriterWrapper_t * pWriter, 
                           uint64_t nStopTime)
{
    if (pWriter == NULL || 
            pWriter->pHLSGenerator == NULL)
        return;

    pWriter->pHLSGenerator->StopMedia(nStopTime);
    pWriter->pHLSGenerator->StopProcess();

    delete pWriter->pHLSGenerator;
    pWriter->pHLSGenerator = NULL;
}

void putMediaFrameForHLS(struct mediawriterWrapper_t *pWriter, uint8_t *pData, 
                         int nSize, uint64_t nTimeStamp, 
                         unsigned long nMediaFlags)
{
    if (pWriter == NULL ||
            pWriter->pHLSGenerator == NULL)
        return;

    CCFrame newFrame;
    newFrame.SetData(pData);
    newFrame.SetLength(nSize);
    newFrame.SetTimeStamp(nTimeStamp);
    newFrame.SetMediaFlag(nMediaFlags);

    pWriter->pHLSGenerator->ProcessMediaFrame(newFrame);
}

void startMediaWriterForTN(struct mediawriterWrapper_t * pWriter, 
                           uint64_t nStartTime, char *pBlobDir,
                           char *pSDPFilePath)
{
    if (pWriter == NULL)
        return;

    pWriter->strSDPFilePath = pSDPFilePath;

    std::string strBlobDir(pBlobDir);
    std::string strSDPFilePath(pSDPFilePath);

    std::vector<int >trackSpeeds;
    trackSpeeds.push_back(8);
    trackSpeeds.push_back(64);
    trackSpeeds.push_back(256);

    pWriter->pTNGenerator = new CFFMediaProcessor( "", strBlobDir, 
                                                   trackSpeeds, "RTP", 
                                                   0, false, false, 
                                                   strSDPFilePath);
    pWriter->pTNGenerator->EnablePrimaryTN(true);
    pWriter->pTNGenerator->SetPrimaryTNResolution(640, 320);
    pWriter->pTNGenerator->EnableFFTracks(false);

    pWriter->pTNGenerator->InitMedia();

    pWriter->pTNGenerator->Start(nStartTime);
}

void stopMediaWriterForTN(struct mediawriterWrapper_t * pWriter, 
                          uint64_t nStopTime)
{
    if (pWriter == NULL ||
        pWriter->pTNGenerator == NULL)
        return;

    pWriter->pTNGenerator->Stop(nStopTime);

    delete pWriter->pTNGenerator;
    pWriter->pTNGenerator = NULL;
}

void putMediaFrameForTN(struct mediawriterWrapper_t * pWriter, 
                        uint8_t * pData, int nSize, uint64_t nTimeStamp, 
                        unsigned long nMediaFlags)
{
    if (pWriter == NULL ||
            pWriter->pTNGenerator == NULL)
        return;

    if (pWriter->pTNGenerator->IsPrimaryTNCreated())
        return;

    CSFrame newFrame((const char*)pData, nSize, nMediaFlags, nTimeStamp);

    pWriter->pTNGenerator->PutFrame(&newFrame);
}

uint32_t getRTPMediaFlags(int nVideo)
{
    if (nVideo == 1)
    {
        CRTPStreamMediaFlag rtpSMediaFlag(FRAMETYPE_RTP_ENCAPS_RTP,
                        FRAMETYPE_RTP_CHAN_VIDEO);
        return (rtpSMediaFlag.GetMediaFlag());
    }
   
    CRTPStreamMediaFlag rtpSMediaFlag(FRAMETYPE_RTP_ENCAPS_RTP,
                                      FRAMETYPE_RTP_CHAN_AUDIO);
    return (rtpSMediaFlag.GetMediaFlag());
}

uint64_t htonll (uint64_t hostFormatInt64)
{
    uData data;
    data.i64 = hostFormatInt64;
    int32_t temp = data.i32[0];
    data.i32[0] = htonl(data.i32[1]);
    data.i32[1] = htonl(temp);

    return data.i64;
}

uint64_t ntohll (uint64_t networkFormatInt64)
{
    uData data;
    data.i64 = networkFormatInt64;
    int32_t temp = data.i32[0];
    data.i32[0] = htonl(data.i32[1]);
    data.i32[1] = htonl(temp);

    return data.i64;
}
