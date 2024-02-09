// VCapture.h: interface for the CVideoCapture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _VCAPTURE_H_
#define _VCAPTURE_H_

#include "Device.h"
#include "VideoCapture.h"
#include "Struct.h"
#include "../../Drv/include/motion_detection_api.h"

#define MAX_SENSOR_NUM  16

enum DEVICE_STATUS
{
    DS_STOP,
    DS_RUNNING,
};


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVideoCapture : IVideoCapture
{
public:
    CVideoCapture(SINT32 instance, SINT32 *Result);
    virtual ~CVideoCapture();

    STATIC SINT32 GetDeviceNumber();
    STATIC SINT32 CreateInstance(VOID **pp, SINT8 instance);
    VOID Release();
    
    SINT32 DeviceExist();
    void SetNotifyCallbackFunc(WIS_NOTIFY_CALLBACK *Callback, void *pContext);

    void GetBoardInfo(REVISION_INFO *p_bi);
    SINT32 sigDetect(void);
    void SetVideoConfig(TCFGVIDEO *pcfgvideo);
    void SetAudioConfig(AUDIO_CONFIG *pcfgaudio);
    void SetFixConfig(TVIDEOCFGFIX *pcfgVideoFix);
    SINT32 SetAudio(int AudioCode);
    SINT32 StartCapturing();
    SINT32 StopCapturing();
    
    SINT32 GetOneFrame(UINT8 *pBuf, SINT32 BufLen, TFrameInfo *pFI);
    SINT32 GetAudioSamples(UINT8 *pBuffer, SINT32 *BufferLength);

    SINT32 GetAVStats(AVStats *pavs);
    SINT32 GetSequenceHeader(UINT8 *pseqheadbuf, SINT32 *pseqheadlen);

    SINT32 SetMDRegions(UINT8 *arru8MotionCoordinates,
                UINT32 u32MaxXCoordinate,
                UINT32 u32MaxYCoordinate);
    SINT32 SetMDThresholdsAndSensitivities(UINT16 arrs16MotionThresholds[MAX_REGIONS_OF_INTEREST][NUM_MOTION_TYPES],
                UINT16* arru8MotionSensitivity);
    SINT32 WaitForMotion(SINT32& id);
    SINT32 ResetMotionDetection(void);
    SINT32 InitMotionDetection(void);
    SINT32 osd_show(unsigned char x,unsigned char y, void *String);
    SINT32 av_sync(int delay_threshould, int check_inteval);

/*  void ForceIntraFrame();

    SINT32 I2C_WriteRegister(UINT8 DevAddr, SINT32 AddrLen, UINT16 RegAddr, UINT8 Data, SINT32 I2C_mode);
    SINT32 I2C_ReadRegister(UINT8 DevAddr, SINT32 AddrLen, UINT16 RegAddr, UINT8 *pData, SINT32 I2C_mode);
    SINT32 I2C_BurstWriteRegister(UINT8 DevAddr, SINT32 AddrLen, UINT16 StartRegAddr, SINT32 RegNum, UINT8 *pData, SINT32 I2C_mode);
    SINT32 I2C_BurstReadRegister(UINT8 DevAddr, SINT32 AddrLen, UINT16 StartRegAddr, SINT32 RegNum, UINT8 *pData, SINT32 I2C_mode);

    SINT32 SPI_WriteRegister(SINT32 OpCodeLen, UINT8 OpCode, SINT32 AddrLen, UINT16 RegAddr, SINT32 DataLen, UINT16 Data, SINT32 SPI_mode);
    SINT32 SPI_ReadRegister(SINT32 OpCodeLen, UINT8 OpCode, SINT32 AddrLen, UINT16 RegAddr, SINT32 DataLen, UINT16 *pData, SINT32 SPI_mode);
    SINT32 SPI_BurstWriteRegister(SINT32 OpCodeLen, UINT8 OpCode, SINT32 AddrLen, UINT16 StartRegAddr, SINT32 DataLen, UINT16 *pData, SINT32 SPI_mode);
    SINT32 SPI_BurstReadRegister(SINT32 OpCodeLen, UINT8 OpCode, SINT32 AddrLen, UINT16 StartRegAddr, SINT32 DataLen, UINT16 *pData, SINT32 SPI_mode);

    void OSD_SetCharacterMap(UINT8 *CharactorMap, SINT32 StartIndex, SINT32 Number);
    void OSD_DisplayCharacters(SINT32 x, SINT32 y, UINT8 *Index, SINT32 Number);
    void OSD_ClearRegion(SINT32 x, SINT32 y, SINT32 Width, SINT32 Height);

    SINT32 GPIO_Write(SINT32 Index, SINT32 Value);
    SINT32 GPIO_Read(SINT32 Index, SINT32 *pValue);
    SINT32 GPIO_BatchWork(SINT32 IndexNum, SINT32 Index[], UINT8 *WorkItem, SINT32 Interval, UINT8 *Result);

    int  GetLastMotionFactor();

#if defined(_WINDOWS)
    UINT32  DoInNotifyThread();
#endif
*/
private:

    SINT32    m_ChipRevision;
    SINT32    m_instance;
    DEVICE_STATUS m_DeviceStatus;
    int       m_DeviceExist;
    CDevice   m_dev;
    IO_PACKET m_IOPacket;
    TCFGVIDEO m_VideoCfg;
    AUDIO_CONFIG m_AudioCfg;
    TVIDEOCFGFIX m_FixCfg;

    int       m_VideoSet;
    int       m_AudioSet;

    UINT8     *m_initData; 

    // for motion detection
    int       m_LastFrameLength;
    int       m_LastQ;

    WIS_NOTIFY_CALLBACK *m_NotifyCallback;
    void      *m_pContext;

#if defined(_WINDOWS)

    int       m_NotifyThreadState;
    HANDLE    m_hNotifyThread;
    HANDLE    m_hNotifyEvent;
    HANDLE    m_hNotifyThreadExitEvent;

#endif

public:
    // following functions are for chip debugging

    int ReadCBusReg(SINT32 RegNum, UINT16 Addr[], UINT16 Data[]);
    int WriteCBusReg(SINT32 RegNum, UINT16 AddrData[]);
    int ReadHPIReg(UINT16 Addr, UINT16 *Data);
    int WriteHPIReg(UINT16 Addr, UINT16 Data);

    int WriteInterrupt(UINT16 IntVal, UINT16 IntData);
    int ReadInterrupt(UINT16 *IntVal, UINT16 *IntData);
	SINT32 I2C_WriteRegister(UINT16 RegAddr, UINT8 Data);
    int SendPacket(UINT8 *Buf, SINT32 BufLen);
    int ReadStreamPipe(UINT8 *Buf, SINT32 BufLen);

    int DoTestWork(int Type, int Param);
    
        //Source selections
        SINT32 SetVideoSource(SINT32 source);
        SINT32 GetVideoSource(void);
    
        // Media on the fly changes
        SINT32 ForceIFrame(void);
        SINT32 ChangePFrameRate(UINT16 rate);
        SINT32 ChangeIFrameQuantizer(UINT16 iqscale);
        SINT32 ChangePFrameQuantizer(UINT16 pqscale);
        SINT32 InsertNewSequenceHeader(void);

        // Change FPS on the Fly
        SINT32 SetFPS(SINT32 fps);
	SINT32 ChangeBrightness(SINT32 brightness_value); /*Added by wiseway 2004-03-16*/
	SINT32 ChangeContrast(SINT32 contrast_value); /*Added by wiseway 2004-03-16*/
	SINT32 ChangeHue(SINT32 hue_value); /*Added by wiseway 2004-03-16*/
	SINT32 ChangeSaturation(SINT32 saturation_value); /*Added by wiseway 2004-03-16*/
        SINT32 SetTVStandard(UINT32 mask);

        //BRC OTF
        SINT32 BitrateControl(SINT32 change_direction,
                  SINT32 new_i_qscale,
                  SINT32 new_p_qscale,
                  SINT32 new_target_bitrate,
                  SINT32 new_peak_rate,
                  SINT32 new_vbv_buffer,
                  SINT32 new_converge_speed,
                  SINT32 new_lambda);

        //void SetResolution(TCFGVIDEO *pcfgvideo);
        SINT32 SetResolution(SINT32 width, SINT32 height);
    
        //Internal: register r/w
        SINT32 WriteCBus(UINT16 addr, UINT16 data);
        SINT32 ReadCBus(UINT16 addr, UINT16* data);
        SINT32 GetBoardInfo(void* info);
};

#endif 
