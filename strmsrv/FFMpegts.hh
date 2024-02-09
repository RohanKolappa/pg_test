/*
 * CFFMpegts.h
 *
 *  Created on: Jun 12, 2013
 *      Author: durga
 */

#ifndef CFFMPEGTS_H_
#define CFFMPEGTS_H_

#include "FFCodec.hh"

class CMPEGTSPktParser;
class CTimeStampAdjuster;

class CFFMpegts: public CFFCodec {
public:
    CFFMpegts(int a_nId, bool a_bShowProgress,
              bool a_bLiveMode, bool a_bNativeOutput);
    virtual ~CFFMpegts();

    virtual void StopStream();

    virtual void ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset = 0);

    virtual void PrintStats();

private:
    void Initialize();
    void CreateOutput(CSFrame *a_pSFrame);
    void CreateStandardMedia(CSFrame *a_pSFrame);
    void CompleteStandardMedia();

    void UpdateAudioTimestamp(uint8_t *a_pPacket);
    void UpdateVideoTimestamp(uint8_t *a_pPacket);
    void UpdatePCRTimestamp(uint8_t *a_pPacket);

    CMPEGTSPktParser *m_pParser;

    CTimeStampAdjuster *m_pTSAdjusterForVideo;
    CTimeStampAdjuster *m_pTSAdjusterForAudio;
    CTimeStampAdjuster *m_pTSAdjusterForPCR;

    timestamp_t m_nCurrentTimestamp;
    timestamp_t m_nBeginTimestamp;

    bool  m_bUpdatedVDiscontinuity;
    bool  m_bUpdatedADiscontinuity;

    //stats
    uint32_t m_nTotalPackets;
    uint32_t m_nTotalPayloadStarts;
};

#endif /* CFFMPEGTS_H_ */
