/*
 * CStandardMediaHandler.h
 *
 *  Created on: May 14, 2013
 *      Author: durga
 */

#ifndef CRTPMediaHandler_H_
#define CRTPMediaHandler_H_

#include <string>
#include <list>
#include "StandardFormatHandler.hh"


class AVOutputFormat;
class AVFormatContext;
class AVStream;
class AVCodecContext;

struct StandardFrameData {
    uint8_t *pFrameData;
    int nFrameSize;
    bool IsVideoFrame;
    uint64_t nRTPTimeStamp;
};

class CRTPMediaHandler: public CStandardFormatHandler {
public:
    CRTPMediaHandler();
    virtual ~CRTPMediaHandler();

    virtual bool ProceedToProcess();

    virtual void SetNativeOutput();

    virtual bool IsThumbnailCreationEnabled();
    virtual bool IsNativeOutputEnabled();

    void SetVideoFound(bool a_bVideoFound);
    void SetAudioFound(bool a_bAudioFound);

    void SetSpropData(uint8_t *a_pData, int a_nSize);

    void SetVideoCodecID(int a_nCodecID);
    void SetAudioCodecID(int a_nCodecID);

    void SetAudioSampleFmt(int a_nSampleFmt);
    void SetAudioSampleRate(unsigned int a_nSampleRate);
    void SetAudioChannels(int a_nChannels);

    void SetAudioExtradata(uint8_t *a_pData, int a_nSize);

    void SetRTPTimestampFrequency(unsigned int a_nRTPTimestampFrequency);

    void SetVideoResolution(int a_nWidth, int a_nHeight);

    void WriteOutputTrailer();

    bool WriteFrame(uint8_t * a_pFrameData, int a_nSize, bool a_bVideoFrame,
                    uint64_t a_nCurrentTS);

private:
    void WriteFrameData(uint8_t * a_pFrameData, int a_nSize, bool a_bVideoFrame,
                        uint64_t a_nCurrentTS);
    bool WriteOutputHeader();

    void AddVideoStream();
    void AddAudioStream();

    void StoreFrameData(uint8_t * a_pFrameData, int a_nSize,
                        bool a_bVideoFrame, uint64_t a_nCurrentTS);
    void DeleteFrameData();

    void WriteAllSavedFramesData();

    uint8_t *m_pSpropData;
    int m_nSpropLen;

    uint8_t *m_pAudioExtradata;
    int m_nAudioExtradataLen;

    int m_nVideoCodecID;
    int m_nAudioCodecID;

    int m_nSampleFormat;
    unsigned int m_nSampleRate;
    int m_nAudioChannels;

    bool m_bNativeOutput;

    bool m_bVideoFound;
    bool m_bAudioFound;

    int m_nVideoStreamIndex;
    int m_nAudioStreamIndex;

    uint64_t m_nPrevVideoTimestamp;
    uint64_t m_nPrevAudioTimestamp;

    int m_nSrcWidth;
    int m_nSrcHeight;
    bool m_bGotResolution;
    bool m_bWrittenHeader;

    unsigned int m_nRTPTimestampFrequency;

    //avFrameData *m_pFrameData;
    std::list<StandardFrameData> m_pFrameData;

    //FFMPEG related.
    AVOutputFormat *m_pOutputFmt;
    AVFormatContext *m_pOutputContext;
    AVStream *m_pVideoStream;
    AVStream *m_pAudioStream;
};

#endif /* CRTPMediaHandler_H_ */
