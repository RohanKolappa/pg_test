/*
 * CStandardMediaHandler.cpp
 *
 *  Created on: May 14, 2013
 *      Author: durga
 */
#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "LiveMediaHeaders.hh"

#include "RTPMediaHandler.h"
#include "RTPParser.hh"

#define MAX_FPS_VALUE 1024
#define MAX_FRAMES_IN_LIST 10000

CRTPMediaHandler::CRTPMediaHandler()
: CStandardFormatHandler()
{
    m_pSpropData            = NULL;
    m_nSpropLen             = 0;

    m_pAudioExtradata       = NULL;
    m_nAudioExtradataLen    = 0;

    m_nVideoCodecID         = 0;
    m_nAudioCodecID         = 0;

    m_nSampleFormat         = AV_SAMPLE_FMT_NONE;
    m_nSampleRate           = 0;
    m_nAudioChannels        = 0;

    m_bNativeOutput         = false;
    m_strOutputFileName     = "";

    m_bVideoFound           = false;
    m_bAudioFound           = false;

    m_nVideoStreamIndex     = -1;
    m_nAudioStreamIndex     = -1;

    m_nPrevVideoTimestamp        = 0;
    m_nPrevAudioTimestamp        = 0;

    m_nSrcWidth             = -1;
    m_nSrcHeight            = -1;
    m_bGotResolution        = false;
    m_bWrittenHeader        = false;

    m_nRTPTimestampFrequency = -1;

    m_pOutputFmt            = NULL;
    m_pOutputContext        = NULL;

    m_pVideoStream          = NULL;
    m_pAudioStream          = NULL;
}

CRTPMediaHandler::~CRTPMediaHandler()
{
    DeleteFrameData();
}

bool CRTPMediaHandler::ProceedToProcess()
{
    return m_bNativeOutput;
}

void CRTPMediaHandler::SetNativeOutput()
{
    m_bNativeOutput = true;
}

bool CRTPMediaHandler::IsThumbnailCreationEnabled()
{
    return false;
}

bool CRTPMediaHandler::IsNativeOutputEnabled()
{
    return m_bNativeOutput;
}

void CRTPMediaHandler::SetVideoFound(bool a_bVideoFound)
{
    m_bVideoFound = a_bVideoFound;
}

void CRTPMediaHandler::SetAudioFound(bool a_bAudioFound)
{
    m_bAudioFound = a_bAudioFound;
}

void CRTPMediaHandler::SetSpropData(uint8_t *a_pData, int a_nSize)
{
    if (a_nSize <= 0 || a_pData == NULL)
        return;

    m_pSpropData = new uint8_t [a_nSize + FF_INPUT_BUFFER_PADDING_SIZE];

    memcpy(m_pSpropData, a_pData, a_nSize);
    memset(m_pSpropData + a_nSize, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    m_nSpropLen = a_nSize;
}

void CRTPMediaHandler::SetVideoCodecID(int a_nCodecID)
{
    m_nVideoCodecID = a_nCodecID;
}

void CRTPMediaHandler::SetAudioCodecID(int a_nCodecID)
{
    m_nAudioCodecID = a_nCodecID;
}

void CRTPMediaHandler::SetAudioSampleFmt(int a_nSampleFmt)
{
    m_nSampleFormat = a_nSampleFmt;
}

void CRTPMediaHandler::SetAudioSampleRate(unsigned int a_nSampleRate)
{
    m_nSampleRate = a_nSampleRate;
}

void CRTPMediaHandler::SetAudioChannels(int a_nChannels)
{
    m_nAudioChannels = a_nChannels;
}

void CRTPMediaHandler::SetAudioExtradata(uint8_t *a_pData, int a_nSize)
{
    if (a_nSize <= 0 || a_pData == NULL)
        return;

    m_pAudioExtradata = new uint8_t[a_nSize + FF_INPUT_BUFFER_PADDING_SIZE];

    memcpy(m_pAudioExtradata, a_pData, a_nSize);
    memset(m_pAudioExtradata + a_nSize, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    m_nAudioExtradataLen = a_nSize;
}

void CRTPMediaHandler::SetRTPTimestampFrequency(unsigned int a_nRTPTimestampFrequency)
{
    if (a_nRTPTimestampFrequency <= 0) {
        printf("RTP Time-stamp Frequency is not correct. \n");
        return;
    }

    m_nRTPTimestampFrequency = a_nRTPTimestampFrequency;
}

bool CRTPMediaHandler::WriteOutputHeader()
{
    if ((m_bVideoFound == false) &&
            m_bAudioFound == false)
        return false;

    if (m_bVideoFound == true)
    {
        if ((m_bGotResolution == false) ||
            (m_pSpropData == NULL))
            return false;
    }

    std::string strOutputFile = m_strOutputFileName + ".mp4";

    m_pOutputFmt = av_guess_format(NULL, strOutputFile.c_str(), NULL);
    if (m_pOutputFmt == NULL) {
        printf("Could not deduce output format file extension. \n");
        return false;
    }

    m_pOutputContext = avformat_alloc_context();
    if (m_pOutputContext == NULL) {
        printf("Could not allocated output context. \n");
        return false;
    }

    m_pOutputContext->oformat = m_pOutputFmt;
    strcpy(m_pOutputContext->filename, strOutputFile.c_str());

    AddVideoStream();
    AddAudioStream();

    if (av_set_parameters(m_pOutputContext, NULL) < 0) {
        printf("Invalid output format parameters. \n");
        return false;
    }

    if (!(m_pOutputContext->oformat->flags & AVFMT_NOFILE)) {
        avio_open(&m_pOutputContext->pb, m_pOutputContext->filename, URL_WRONLY);
    }

    av_write_header(m_pOutputContext);

    m_bWrittenHeader = true;

    return true;
}

void CRTPMediaHandler::AddVideoStream()
{
    if (m_bVideoFound == false)
        return;

    m_pVideoStream = av_new_stream(m_pOutputContext, 0);

    m_nVideoStreamIndex = m_pVideoStream->index;

    AVCodecContext *pOutputVCodecContext =
                          m_pVideoStream->codec;

    pOutputVCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    pOutputVCodecContext->codec_id = (CodecID)m_nVideoCodecID;//CODEC_ID_H264;

    pOutputVCodecContext->width = m_nSrcWidth;
    pOutputVCodecContext->height = m_nSrcHeight;
    pOutputVCodecContext->time_base.num = 1;
    pOutputVCodecContext->time_base.den = m_nRTPTimestampFrequency;

    pOutputVCodecContext->extradata = m_pSpropData;
    pOutputVCodecContext->extradata_size = m_nSpropLen;
}

void CRTPMediaHandler::AddAudioStream()
{
    if (m_bAudioFound == false)
        return;

    m_pAudioStream = av_new_stream(m_pOutputContext, 0);
    m_nAudioStreamIndex = m_pAudioStream->index;

    AVCodecContext *pOutputACodecContext =
                              m_pAudioStream->codec;
    avcodec_get_context_defaults2(pOutputACodecContext, AVMEDIA_TYPE_AUDIO);

    pOutputACodecContext->codec_type = AVMEDIA_TYPE_AUDIO;

    pOutputACodecContext->extradata = m_pAudioExtradata;
    pOutputACodecContext->extradata_size = m_nAudioExtradataLen;

    pOutputACodecContext->codec_id = (CodecID)m_nAudioCodecID;
    pOutputACodecContext->sample_fmt = (AVSampleFormat)m_nSampleFormat;
    pOutputACodecContext->sample_rate = m_nSampleRate;
    pOutputACodecContext->frame_size = 1024;
    pOutputACodecContext->channels = m_nAudioChannels;
}

void CRTPMediaHandler::WriteOutputTrailer()
{
    if((m_bVideoFound == false) &&
            m_bAudioFound == false)
        return;

    if (m_bWrittenHeader == false)
        return;

    av_write_trailer(m_pOutputContext);

    if (m_pOutputContext->oformat && !(m_pOutputContext->oformat->flags & AVFMT_NOFILE)
            && m_pOutputContext->pb)
        avio_close(m_pOutputContext->pb);

    if (m_pVideoStream != NULL) {
        avcodec_close(m_pVideoStream->codec);
    }

    if (m_pAudioStream != NULL) {
        avcodec_close(m_pAudioStream->codec);
    }

    avformat_free_context(m_pOutputContext);
}

bool CRTPMediaHandler::WriteFrame(uint8_t * a_pFrameData, int a_nSize,
                                  bool a_bVideoFrame, uint64_t a_nCurrentTS)
{
    if (m_bVideoFound == false &&
            m_bAudioFound == false)
        return false;

    if (m_bWrittenHeader == false) {
        if (WriteOutputHeader() == false) {
            StoreFrameData(a_pFrameData, a_nSize, a_bVideoFrame, a_nCurrentTS);
            return true;
        }
    }

    WriteAllSavedFramesData();

    WriteFrameData(a_pFrameData, a_nSize, a_bVideoFrame, a_nCurrentTS);

    return true;
}

void CRTPMediaHandler::WriteFrameData(uint8_t * a_pFrameData, int a_nSize,
                                      bool a_bVideoFrame, uint64_t a_nCurrentTS)
{
    uint64_t nDTSValue = 0;
    int nIndex = -1;

    if (a_bVideoFrame == true) {
        nIndex = m_nVideoStreamIndex;
        nDTSValue = m_nPrevVideoTimestamp;

        m_nPrevVideoTimestamp = a_nCurrentTS;
    }
    else {
        nIndex = m_nAudioStreamIndex;
        nDTSValue = m_nPrevAudioTimestamp;

        m_nPrevAudioTimestamp = a_nCurrentTS;
    }

    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index = nIndex;
    pkt.data = a_pFrameData;
    pkt.size = a_nSize;

    pkt.dts = nDTSValue;
    pkt.pts = a_nCurrentTS;

    int nRet = av_interleaved_write_frame(m_pOutputContext, &pkt);
    if (nRet != 0) {
        printf("Error while writing frame. \n");
    }

    av_free_packet(&pkt);
}

void CRTPMediaHandler::SetVideoResolution(int a_nWidth, int a_nHeight)
{
    m_nSrcWidth = a_nWidth;
    m_nSrcHeight = a_nHeight;

    if (m_nSrcWidth > 0 && m_nSrcHeight > 0)
        m_bGotResolution = true;
}

void CRTPMediaHandler::StoreFrameData(uint8_t * a_pFrameData, int a_nSize,
                                      bool a_bVideoFrame, uint64_t a_nCurrentTS)
{
    if (m_pFrameData.size() >= MAX_FRAMES_IN_LIST) {
        StandardFrameData pTempFrame;
        pTempFrame = m_pFrameData.front();

        delete [] pTempFrame.pFrameData;

        m_pFrameData.pop_front();
    }

    StandardFrameData pFrame;
    pFrame.pFrameData = new uint8_t[a_nSize];
    memcpy(pFrame.pFrameData, a_pFrameData, a_nSize);
    pFrame.nFrameSize = a_nSize;
    pFrame.nRTPTimeStamp = a_nCurrentTS;
    pFrame.IsVideoFrame = a_bVideoFrame;

    m_pFrameData.push_back(pFrame);
}

void CRTPMediaHandler::DeleteFrameData()
{
    std::list<StandardFrameData>::iterator i;

    for(i = m_pFrameData.begin(); i != m_pFrameData.end(); ++i) {
        delete [] (*i).pFrameData;
    }

    m_pFrameData.clear();
}

void CRTPMediaHandler::WriteAllSavedFramesData()
{
    std::list<StandardFrameData>::iterator i;

    for(i = m_pFrameData.begin(); i != m_pFrameData.end(); ++i) {
        WriteFrameData((*i).pFrameData,
                       (*i).nFrameSize,
                       (*i).IsVideoFrame,
                       (*i).nRTPTimeStamp);

        delete [] (*i).pFrameData;
    }

    m_pFrameData.clear();
}
