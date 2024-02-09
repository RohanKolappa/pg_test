#ifndef __HH_FPGARX_H__
#define __HH_FPGARX_H__

#include "receiver.hh"
#include "command.hh"
#include "shaper.hh"
#include "v2d.h"
#include "v2o_lib.h"
#include "portuse.hh"
#include "rfbcallback.hh"

#define MAX_FPGA_PORTS 12

// Forward declarations
class CRFBHandler;
class CSplash;
class CKbmIpc;
class CUSBHID;
class CUSB;


class CFPGARx: public CReceiver, public IRFBHandlerCallback {
public:
    CFPGARx(int nId, int nIOPort);

    ~CFPGARx();

    int ProcessStream();
    int ProcessAltStream();

    char *m_pSendBuffer;

    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);
    int Update(CCommand *a_pCmd);

    bool CheckKBMEvent(unsigned long a_sessionId);
    void UpdateKBMEvent(unsigned long a_sessionId);
    void UpdateCtlPacketStats(int nLen);
    CSQueueSink * RetrieveQueueSink() {
        return GetQueueSink();
    }
    timestamp_t RetrieveTimeStamp() {
        return GetTimeStamp();
    }
    void HandleRFBConnected() {};
    void HandleRFBDisconnected() {};
    int HandleRFBReady() {
        return 0;
    }
private:
    int m_hVideo;
    int m_hAudio;
    int m_hKeyboard;
    int m_hMouse;
    int m_hSerial;
    struct list_head *m_hUsbEventHead;
    CShaper *m_pShaper;
    HAL *m_pHal;
    SRVR *m_pSrvr;
    bool m_bXpiDevice;
    bool m_bIsMulticast;
    int m_nMulticastCount;
    int m_nMouseType;
    int m_nKeyboardType;
    struct LUT_struct *m_LUT;
    int m_nVideoChanged;
    int m_nMQuant;
    int m_nDCQuant;
    int m_nSliceSize;
    int m_nCompMode;
    int m_nMotionCompMode;
    timestamp_t m_tLastScapSent;
    timestamp_t m_tLastKbWritten;
    timestamp_t m_tLastMsWritten;
    uint64_t m_nVideoPackets;
    uint64_t m_nVideoBytes;
    uint64_t m_nAudioPackets;
    uint64_t m_nAudioBytes;
    uint64_t m_nCtlInPackets;
    uint64_t m_nCtlInBytes;
    uint64_t m_nCtlOutPackets;
    uint64_t m_nCtlOutBytes;
    uint64_t m_nMsPackets;
    uint64_t m_nMsLostPackets;
    uint64_t m_nMsBytes;
    uint64_t m_nKbPackets;
    uint64_t m_nKbLostPackets;
    uint64_t m_nKbBytes;
    uint64_t m_nSerInPackets;
    uint64_t m_nSerInLostPackets;
    uint64_t m_nSerInBytes;
    uint64_t m_nSerOutPackets;
    uint64_t m_nSerOutBytes;
    uint64_t m_nUsbKbInPackets;
    uint64_t m_nUsbKbInBytes;
    uint64_t m_nUsbMsInPackets;
    uint64_t m_nUsbMsInBytes;
    uint64_t m_nAudioSOFerrors;
    int m_nFSRefreshCount;
    unsigned long m_nRequestBandwidth;
    unsigned long m_nComputedBandwidth;
    int m_nAudioType;
    int m_nAVOption;
    int m_nMinCompression;
    int m_nMaxCompression;
    float m_fBurstRatio;
    int m_nBurstDuration;
    int m_nSliceDropCount;
    unsigned long m_nLastFrameCount;
    unsigned int m_nErrorCode;
    unsigned long m_nAudioSeqNumber;
    unsigned long m_nVideoSeqNumber;
    char *m_pVideoBuffer;
    char *m_pAudioBuffer;
    char *m_pScapBuffer;
    int m_nScapLength;
    unsigned char *m_pKbBuffer;
    unsigned char *m_pMsBuffer;
    unsigned char *m_pKbPktBuffer;
    int m_nOffset;
    int m_nMslen;
    char *m_pSerialBuffer;
    int m_nCodecVersion;
    int m_nSplashCodec;
    short  int MouseX;
    short  int MouseY;
    short  int size_x;
    short  int size_y;
    bool m_bKBMCflag;

    CRFBHandler * m_pRFB;
    CSplash  * m_pSplash;

	CUSBHID		*m_pUsbHid;
    CUSB    	*m_pUsb;
    CKbmIpc		*m_pKbmIpc;
 
    unsigned long m_nControllingSessionId;
    timestamp_t m_tLastKBMSendEvent;
    int m_nKBMIdletime;
    bool m_bAllowKBMSwitch;
    bool m_bDisplaySplash;
    bool m_bRealVideo;
    int m_nregVal;
    int m_nIOPort;
    unsigned long m_nNoData;
    static CPortUse m_portsUsed;

    int GetVideoFrame(void);
    int GetAudioFrame(void);
    int GetSerialData(void);
    int ProcessUsbEventData(struct event_s *evententryp);
    int CreateScap(void);
    int SendScap(void);
    int SetEncParams(char *pData);
    int ProcessNetHeader(char *pData);
    bool CheckForMulticast(NetHdrData_t & hdr);

    int PopulateKbBuffer(char *pData);
    int PopulateMsBuffer(char *pData);
    int ProcessMsBuffer();
    int PopulateSerialBuffer(char *pData);
    int CreateRFBMsData(unsigned char *pInMsBuf, 
                        unsigned char *pOutRFBBuf,
                        int msLen);
    int CreateRFBKbData(unsigned char *pInKbBuf, 
                        unsigned char *pOutRFBBuf,
                        int kbLen);
    
    int CreateRawKbData(unsigned char *pInKbBuf, 
                        unsigned char *pOutRFBBuf,
                        int kbLen);

    int ProcessRelMouse(unsigned char *pInMsBuf,
                        char *pOutMsBuf,
                        int msLen);


    void  CreateMouseEvent(int x,int y,int buttonmask);
    void ProcessVNCKeyboardEventData();
    void ProcessVNCMouseEventData();
    void GetAndSetWidthHeight();
    int ValidateParams();
    int SendReject();

    int BuildSerialData(unsigned short ident, char* serBuf, int serLen);

    int WriteSplashPacket();

    int  SetBandwidth();
    int  SetSliceDrop();
    void InitVariablesToZero();
    void SetupConnectionProfile();
    int  UpdateConnectionProfile(CCommand *a_pCmd);
    void InitSerialParameters();
    void InitKBMTypes();
    void InitScreenResolution();
    int  UpdateKBMTypes(CCommand *a_pCmd);
    void InitSplashFrame();
    void DeInitSplashFrame();
    int  UpdateSplash(CCommand *a_pCmd);
    int UpdateKeyboardEscapeSequences(CCommand *a_pCmd);
    void InitConsoleDevice();
    int  UpdateUSBDevices(CCommand *a_pCmd);
    void UpdateCodecVersion();
    bool CheckForBreakCode(unsigned char *buf, int *buflen);
    void KbmControlerRes(unsigned short hres, unsigned short vres);

};
#endif /* __HH_FPGARX_H__ */
