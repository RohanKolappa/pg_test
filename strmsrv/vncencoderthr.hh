/*
 * vncencoderthr.hh
 *
 *  Created on: Nov 23, 2009
 *      Author: rkale
 */

#ifndef VNCENCODERTHR_HH_
#define VNCENCODERTHR_HH_

#include "detachedthread.hh"
#include "streamer.hh"
#include "squeue.hh"
#include "command.hh"
#include "rfbclient.h"
#include "codecdefs.h"
#include "mutex.hh"

// Forward Declarations
class CFBRx;
class CVideoFrameEncoder;
class CCompressionUpdate;
class CSVCCompressionUpdate;
class IV2DEncoderListener;

class CVNCEncoderThread : public CDetachedThread {

public:
    CVNCEncoderThread(CStreamer *a_pStreamer, IV2DEncoderListener *a_pV2DEncoderCallback);

    int RunThread();
    bool SetCompression(int nCompression);
    bool SetCodecVersion(int nCodecVersion);
    bool SetSliceRefresh(int nSliceRefresh);
    void UpdateCustomStatus(CCommand *a_pCmd);
    int GetNextFrame(unsigned char *a_pBuffer);
    void FullScreenRefresh();

    timestamp_t GetEncodeTimestamp();
    uint64_t GetEncodedFrameCount();
    uint64_t GetRectangleCount();
    uint64_t GetDroppedRectangleCount();
    int GetSliceRefresh();
    int GetCodecVersion();

private:
    IV2DEncoderListener * m_pV2DEncoderListener;
    CSQueueSource * m_qEncoderSource;

    CVideoFrameEncoder *m_pFrameEncoder;
    timestamp_t m_tEncodeTimestamp;
    int m_nFrameWidth;
    int m_nFrameHeight;
    int m_nMQuant;
    int m_nDCQuant;
    int m_nSliceRefresh;
    eEncodeFormat m_eEncodeFormat;
    ePixelFormat m_ePixelFormat;
    CMutex m_encoderMutex;
    uint64_t m_nDroppedRectangleCount;
    uint64_t m_nRectangleCount;

    struct LUT_struct * m_pCurrentLUT;

    ~CVNCEncoderThread();
    void InitializeVariables();
    void NewFrameEncoder();
    int EncoderProcessor();
    void SetupEncoder(CCompressionUpdate * compressionUpdate);
    void SetupEncoder(CSVCCompressionUpdate * compressionUpdate);
    void SetupFrameBuffer(int a_nWidth, int a_nHeight, enum ePixelFormat a_ePixelFormat);
    void UpdateFrameBuffer(RFBRect_t * a_pRectangle);
    void UpdateFrameBuffer(unsigned char * a_pRectangle);
};

#endif /* VNCENCODERTHR_HH_ */
