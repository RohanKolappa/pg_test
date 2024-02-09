#ifndef __HH_FPGATX_H__
#define __HH_FPGATX_H__

#include "sender.hh"
#include "portuse.hh"
#include "v2d.h"
#include "v2o_lib.h"

class CSplash;
class CUSBHID;
class CUSB;

class CFPGATx : public CSender {
public:
    CFPGATx(int nId, int nIOPort, bool bEnableKBMS);
    ~CFPGATx();

    int ProcessStream();
    int ProcessAltStream();

    char *m_pSendBuffer;

    void PrintStatistics(std::string& a_stats);
    void UpdateCustomStatus(CCommand *a_pCmd);
    int Update(CCommand *a_pCmd);

private:
    unsigned long m_nControlPackets;
    unsigned long m_nVideoPackets;
    unsigned long m_nAudioPackets;
    unsigned long m_nKbInPackets;
    unsigned long m_nKbInBytes;
    unsigned long m_nMsInPackets;
    unsigned long m_nMsInBytes;
    unsigned long m_nScapCount;
    unsigned long m_nVideoChanged;
    unsigned long m_nSerInPackets;
    unsigned long m_nSerInBytes;
    unsigned long m_nSerOutPackets;
    unsigned long m_nSerOutBytes;
    unsigned long m_nUsbKbInPackets;
    unsigned long m_nUsbKbInBytes;
    unsigned long m_nUsbMsInPackets;
    unsigned long m_nUsbMsInBytes;
    unsigned long m_nVideoPacketsWritten;
    unsigned long m_nVideoBytesWritten;
    unsigned long m_nAudioPacketsWritten;
    unsigned long m_nAudioBytesWritten;
    unsigned long m_nRFBStatusReplies;
    unsigned long m_nRFBStatusQueries;
    int m_nRetryDropCount;
    unsigned long  m_tLastMouseReceivedTimestamp;

    int m_hVideo;
    int m_hAudio;
    int m_hKeyboard;
    int m_hMouse;
    int m_hSerial;
    struct list_head *m_hUsbEventHead;
    int m_nMaxfd;
    int m_nAvoption;
    int m_nLocalMousePointerIdleTime;
    bool m_bLocalMousePointerEnable;

    HAL *m_pHal;
    CLNT *m_pClnt;
    CSplash  *m_pSplash;
    int  m_nSplashType;

    ScapData_t *m_scapData;
    ScapData_t *m_splashScap;
	CUSBHID		*m_pUsbHid;
    CUSB    	*m_pUsb;

    int  m_nIOPort;
    static CPortUse m_portsUsed;
    bool m_bXpiDevice;
    bool m_bSplashVideo;
    bool m_bSplashRequested;
    bool m_bEnableKBMS;
    bool m_bRFBStatus;
    bool m_bIsWaiting;

    timestamp_t m_tSplashReqTimestamp;

    bool CompareAndSetScap(ScapData_t *a_pScap);
    bool CompareAndSetKbMsScap(ScapData_t *a_pScap);
    void ProcessFrameTypeControl(CSFrame *sFrame);
    void ProcessFrameTypeVideo(CSFrame *sFrame);
    void ProcessFrameTypeAudio(CSFrame *sFrame);
    int BuildKbData(char *pKbInBuf, int kbLen);
    int ProcessKeyboardFrame();
    int ProcessMouseFrame();
    int ProcessSerialFrame();
    int ProcessUsbEventFrame(struct event_s *eventp);
    int WriteSplashFrame();
    int ScapIoctl(ScapData_t *a_pScap);
    int InitKeyboardType(int kbtype);
    void SwitchSplashMode(bool a_mode);
    void CheckSplashMode();

    void SetSerialParameters(int fd,
                             int baudrate,
                             int databits,
                             int stopbits,
                             unsigned char parity,
                             unsigned char flowcontrol);

    int PopulateSerialBuffer(char *pData);

    int ClearRtpFilter();
    int ProcessRtpFilter(char *pData);

    int ProcessRFBStatus(char *pData);
    int SendRFBStatusQuery();
    int CreateRFBKbData(unsigned char *pInKbBuf,
                        unsigned char *pRfbDataBuf,
                        unsigned char *pOutRFBBuf,
                        int kbLen);
    int ProcessRFBKbData(unsigned char *pInKbBuf, unsigned char *pRfbDataBuf, int kbLen);
    int CreateRFBMsData(unsigned char *pInMsBuf,
                        unsigned char *pOutRFBBuf,
                        int msLen);
    int ProcessRFBMsData(unsigned char *pInMsBuf, int msLen);
    int ProcessLocalMouse(unsigned char *pInMsBuf,
                          int msLen);
    int ProcessLocalKeyboard(unsigned char *pInKbBuf,
                             int kbLen);

    int GetMaxfd(int hkeyboard,
            int hmouse,
            int hserial,
            struct list_head *heventhead);
    void InitSplashFrame();
    int UpdateSplash(CCommand *a_pCmd);
    void InitVariablesToZero();
    void InitSerialConfig();
    void InitConsoleDevice();
    void CloseConsoleDevice();
    int UpdateEnableKBMS(CCommand *a_pCmd);
    int UpdateKeyboardMode(CCommand *a_pCmd);
    int UpdateKeyboardEscapeSequences(CCommand *a_pCmd);
    int UpdateKBMTypes(CCommand *a_pCmd);
    int UpdateUSBDevices(CCommand *a_pCmd);
    int UpdateOverrideParameters(CCommand *a_pCmd);
    int UpdateRFBDisplayParameters(CCommand *a_pCmd);
    int UpdateKeyboardLayout(CCommand *a_pCmd);
    void NotifyCodecVersionChange();
    void ReloadScapIoctl();
    void CheckMouseActive();
    int UpdateLocalMousePointerParameters(CCommand *a_pCmd);
    void InitMousePointer();
};
#endif /*__HH_FPGATX_H__*/


