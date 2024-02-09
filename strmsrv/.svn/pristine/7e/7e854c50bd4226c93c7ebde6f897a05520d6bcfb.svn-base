/*
 * fbrx.hh
 *
 *  Created on: Nov 29, 2010
 *      Author: rkale
 */

#ifndef FBRX_HH_
#define FBRX_HH_

#include <string>
#include "receiver.hh"
#include "rfbclient.h"
#include "geometry.hh"
#include "v2d.h"
#include "mutex.hh"
#include "codecdefs.h"
#include "v2dencoderlistener.hh"

#define WAIT_1_FPS   1000
#define WAIT_16_FPS   62
#define WAIT_24_FPS   41
#define WAIT_60_FPS   17

class CShaper;
class CVNCEncoderThread;
class CMeter;
class CRateController;

class CFBRx : public CReceiver, public IV2DEncoderListener
{

public:
    CFBRx(int nId, int compression, int codecversion);
    virtual ~CFBRx();

    virtual int ProcessStream();
    virtual void DeleteStream();
    virtual void UpdateCustomStatus(CCommand *a_pCmd);
    int Update(CCommand *a_pCmd);
    virtual void LutChanged(struct LUT_struct * a_pLUT, int a_nSliceSize);
    virtual void CompressionChanged(int a_nMQuant, int a_nDCQuant);
    virtual void Stopped() {};

protected:
    int m_nCompression;
    int m_nCodecVersion;
    int m_nSliceRefresh;
    int m_nBandwidth;
    eSVCEncodingType m_eEncodingType;
    eSVCEncodingSubType m_eEncodingSubType;
    int m_nLayerDimension;

    bool m_bResolutionChanged;
    V2DLib::Dimension m_FrameDim;
    unsigned long m_nEncoderThrottled;
    unsigned long m_nFramesServed;
    unsigned long m_nVideoOutBytes;

    unsigned long m_nCtlOutPackets;
    unsigned long m_nCtlOutBytes;
    unsigned long m_nCtlInPackets;
    unsigned long m_nCtlInBytes;
    unsigned long m_nMsPackets;
    unsigned long m_nMsBytes;
    unsigned long m_nKbPackets;
    unsigned long m_nKbBytes;
    unsigned long m_nFSResfreshCount;

    uint8_t * m_pEncodedBuffer;
    uint8_t * m_pWritePtr;
    uint8_t * m_pEndPtr;
    unsigned long m_nPadding;
    unsigned long m_nSequenceNumber;
    timestamp_t m_tLastScapSent;
    bool m_bKBMEvent;
    char * m_pScapBuffer;
    CShaper *m_pShaper;
    CMeter *m_pMeter;
    CRateController *m_pOutputRateController;

    CSQueueSink * m_qEncoderSink;
    struct LUT_struct * m_pLUT;

    int m_nMouseType;
    int m_nKeyboardType;
    int m_nVideoChanged;
    int m_nSliceSize;
    int m_nMQuant;
    int m_nDCQuant;
    int m_nScapLength;

    CVNCEncoderThread * m_pEncoderThread;

    virtual int CustomUpdate(CCommand *a_pCmd);
    virtual int ProcessFrameBuffers() = 0;
    virtual void ProcessAVRequest(AVReqData_t *) = 0;
    virtual void ProcessRFBQuery(char *pPayload) = 0;
    virtual void ProcessKeyboard(char *pPayload) = 0;
    virtual void ProcessMouse(char *pPayload) = 0;
    virtual void RequestReady() = 0;
    virtual void HandleDownstreamConnected() {};
    virtual void HandleDownstreamDisconnected() {};
    virtual void CreateEncoderThread();
    virtual void SetBandwidth();

    void HandleConnected();
    void HandleDisconnected();
    int ProcessEncodedPackets();

private:
    CMutex m_LUTMutex;
    CMutex m_BufferMutex;

    int ProcessAltStream();
    void HandleAVRequest(char *pData);
    void PublishScap();
    int CreateScap(int a_nVideoChanged);
    void SendScap();
    void InitializeVariables();
    void CreateEncodedBuffer();

};


class CEncoderInfo {

public:
    enum MessageType {
        NewVideoResolution = 1,
        RectangleUpdate,
        FrameBufferUpdate,
        CompressionUpdate,
        SVCCompressionUpdate,
    };

    CEncoderInfo(MessageType a_nMessageType) {
        m_nMessageType = a_nMessageType;
    }
    ~CEncoderInfo() {};

    MessageType GetMessageType() { return m_nMessageType; }

private:
    MessageType m_nMessageType;
};

class CNewVideoResolutionInfo : public CEncoderInfo {

public:

    CNewVideoResolutionInfo(int a_width, int a_height,
            enum ePixelFormat a_ePixelFormat = eBGRX32) :
        CEncoderInfo(NewVideoResolution),
        m_nFrameWidth(a_width),
        m_nFrameHeight(a_height),
        m_ePixelFormat(a_ePixelFormat) {}
    ~CNewVideoResolutionInfo() {};

    int GetWidth() const { return m_nFrameWidth; }
    int GetHeight() const { return m_nFrameHeight; }
    enum ePixelFormat GetPixelFormat() const { return m_ePixelFormat; }

private:
    int m_nFrameWidth;
    int m_nFrameHeight;
    enum ePixelFormat m_ePixelFormat;
};

class CRectangleUpdate : public CEncoderInfo {

public:

    CRectangleUpdate(RFBRect_t * a_pRectangle) :
        CEncoderInfo(RectangleUpdate),
        m_pRectangle(a_pRectangle) {};
    ~CRectangleUpdate() {};

    RFBRect_t * GetRectangle() const { return m_pRectangle; }

private:
    RFBRect_t * m_pRectangle;
};

class CFrameBufferUpdate : public CEncoderInfo {

public:

    CFrameBufferUpdate(uint8_t * a_pBuffer) :
        CEncoderInfo(FrameBufferUpdate),
        m_pBuffer(a_pBuffer) {};
    ~CFrameBufferUpdate() {};

    unsigned char * GetBuffer() const { return m_pBuffer; }

private:
    uint8_t * m_pBuffer;
};

class CCompressionUpdate : public CEncoderInfo {

public:

    CCompressionUpdate(int a_nCompression,
            int a_nCodecVersion,
            int a_nSliceRefresh) :
        CEncoderInfo(CompressionUpdate),
        m_nCompression(a_nCompression),
        m_nCodecVersion(a_nCodecVersion),
        m_nSliceRefresh(a_nSliceRefresh) {};
    ~CCompressionUpdate() {};

    int GetCompression () const { return m_nCompression; };
    int GetCodecVersion () const { return m_nCodecVersion; };
    int GetSliceRefresh () const { return m_nSliceRefresh; };

private:
    int m_nCompression;
    int m_nCodecVersion;
    int m_nSliceRefresh;
};

class CSVCCompressionUpdate : public CEncoderInfo {

public:

    CSVCCompressionUpdate(eSVCEncodingType a_eEncodingType,
            eSVCEncodingSubType a_eEncodingSubType,
            int a_nLayerDimension) :
        CEncoderInfo(SVCCompressionUpdate),
        m_eEncodingType(a_eEncodingType),
        m_eEncodingSubType(a_eEncodingSubType),
        m_nLayerDimension(a_nLayerDimension) {};
    ~CSVCCompressionUpdate() {};

    eSVCEncodingType GetEncodingType () const { return m_eEncodingType; };
    eSVCEncodingSubType GetEncodingSubType () const { return m_eEncodingSubType; };
    int GetLayerDimension () const { return m_nLayerDimension; };

private:
    eSVCEncodingType m_eEncodingType;
    eSVCEncodingSubType m_eEncodingSubType;
    int m_nLayerDimension;
};

#endif /* FBRX_HH_ */
