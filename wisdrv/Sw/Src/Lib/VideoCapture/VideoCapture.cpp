// VideoCapture.cpp: implementation of the CVideoCapture class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include "VCapture.h"
#include "DebugPrint.h"
#include "motion_detection_api.h"
#include "osd.h"
// for compatible with GO7007
INLINE  SINT32  INVERSE32(SINT32 x)
{
    UINT8 *code = (UINT8*)&x, r[4];
    r[3] = code[0]; r[2] = code[1]; r[1] = code[2]; r[0] = code[3];
    return *((SINT32*)r);
}
/////////////////////////////


//FILE *fp_dump;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoCapture::CVideoCapture(SINT32 instance, SINT32 *Result)
{
    m_instance = instance;
    *Result = 0;

    if(m_dev.OpenDevice(instance)!= RC_SUCCESS)
    {
        *Result = 0;
        m_initData = NULL;
        m_DeviceExist = 0;
        return;
    }

    m_DeviceExist = 1;
    m_DeviceStatus = DS_STOP;

    m_VideoSet = 0;
    m_AudioSet = 0;

    char FilePath[256];

    strcpy(FilePath, "/etc/wis/go7007sb.bin");

    FILE* fp = fopen(FilePath, "rb");
    if( fp == NULL )                                                   
    {
        *Result = 0;
        m_DeviceExist = 0;
        return;
    }

    m_initData = new UINT8[65536];
    fread(m_initData,1,65536,fp);
    fclose(fp);

    memset ((char *) &m_IOPacket, 0, sizeof(m_IOPacket));

    m_IOPacket.Type = IO_GETCHIPREVISION;
    m_IOPacket.BufLen = 0;
    m_dev.DeviceIO(&m_IOPacket);
    m_ChipRevision = m_IOPacket.Param1;

    m_IOPacket.Type = IO_SETINSTANCENUM;
    m_IOPacket.Param1 = instance;
    m_IOPacket.BufLen = 0;
    m_dev.DeviceIO(&m_IOPacket);

    *Result = 1;
}

CVideoCapture::~CVideoCapture()
{
    if(m_initData)
        delete[] m_initData;

    if( m_DeviceExist )
    {

        m_dev.CloseDevice();
    }
}


SINT32 CVideoCapture::DeviceExist()
{
    return m_DeviceExist;
}

void CVideoCapture::SetNotifyCallbackFunc(WIS_NOTIFY_CALLBACK *Callback, void *pContext)
{
    m_NotifyCallback = Callback;
    m_pContext = pContext;
}

void CVideoCapture::GetBoardInfo(REVISION_INFO *p_bi)
{
    m_IOPacket.Type = IO_GETCHIPREVISION;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);
    memcpy(p_bi, m_IOPacket.Buf, sizeof(REVISION_INFO));
}

void CVideoCapture::SetVideoConfig(TCFGVIDEO *pcfgvideo)
{
    m_VideoSet = 1;
    m_VideoCfg = *pcfgvideo;
}

void CVideoCapture::SetAudioConfig(AUDIO_CONFIG *pcfgaudio)
{
    m_AudioSet = 1;
    m_AudioCfg = *pcfgaudio;
}

void CVideoCapture::SetFixConfig(TVIDEOCFGFIX *pcfgVideoFix)
{
    m_FixCfg = *pcfgVideoFix;
    return;
}

SINT32 CVideoCapture::StartCapturing()
{
    if(m_DeviceStatus != DS_STOP)
        return ERR_DEVICE;

    if(m_VideoSet == 0 || m_AudioSet == 0)
    {
        printf("config not set");
        return 1;
    }

    m_LastFrameLength = 0;
    m_LastQ = 0;
    
// pass down video config
    m_IOPacket.Type = IO_SETVIDEOCONFIG;
    m_IOPacket.BufLen = sizeof(TCFGVIDEO);
    memcpy(m_IOPacket.Buf, &m_VideoCfg, sizeof(TCFGVIDEO) );

    m_dev.DeviceIO(&m_IOPacket);

// pass down audio config
    m_IOPacket.Type = IO_SETAUDIOCONFIG;
    m_IOPacket.BufLen = sizeof(AUDIO_CONFIG);
    memcpy(m_IOPacket.Buf, &m_AudioCfg, sizeof(AUDIO_CONFIG) );

    m_dev.DeviceIO(&m_IOPacket);

// pass down fix config
    m_IOPacket.Type = IO_SETFIXCONFIG;
    m_IOPacket.BufLen = sizeof(FIXCONFIG);
    if ((m_FixCfg.deintercfg.DeInter_Always_Motion==0) &&
        (m_FixCfg.deintercfg.DeInter_Always_Blending==0))
        m_FixCfg.deintercfg.DeInter_Always_Weave=1;
    memcpy(m_IOPacket.Buf, &m_FixCfg, sizeof(FIXCONFIG) );

    m_dev.DeviceIO(&m_IOPacket);

//  fp_dump = fopen("c:\\dump.dat", "wb");

    m_IOPacket.Type = IO_STARTDEVICE;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
        m_DeviceStatus = DS_RUNNING;
        return SUCCESS;
    }
    else
        return ERR_DEVICE;
}

SINT32 CVideoCapture::StopCapturing()
{
        if(m_DeviceStatus != DS_RUNNING){
        printf("CVideoCapture::StopCapturing: device is not running!\n");
        return ERR_DEVICE;
    }

    m_DeviceStatus = DS_STOP;

//  fclose(fp_dump);

    m_IOPacket.Type = IO_STOPDEVICE;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
        return SUCCESS;
    }
    else
        return ERR_DEVICE;
}

// BufLen : [In]  Buffer length
SINT32 CVideoCapture::GetOneFrame(UINT8 *pBuf, SINT32 BufLen, TFrameInfo *pFI)
{
    int r;

    if(m_DeviceStatus != DS_RUNNING)
        return ERR_DEVICE;

    int BufLen1 = (BufLen/10-1)*10+1;
    r = m_dev.GetOneVideoFrame(pBuf, &BufLen1); 

//      fwrite(pBuf, *BufLen, 1, fp_dump); 

    if(r != RC_SUCCESS)
        return ERR_DEVICE;

    if( BufLen1 == 0 )
        return ERR_NOMOREFRAME;
    
    if(pFI->FrameType==1)   // P frame
    {
        m_LastFrameLength = pFI->VideoLength;
        m_LastQ = m_VideoCfg.ctlcfg.Q;
    }

    memcpy(pFI, pBuf+BufLen1-sizeof(TFrameInfo), sizeof(TFrameInfo));
#if 0
    if(pFI->frame_seq_pos==0)
	printf("videocapture:seq_pos=%d, video length=%d\n",
	       pFI->frame_seq_pos,pFI->VideoLength);
#endif
    return SUCCESS;
}

SINT32 CVideoCapture::GetAudioSamples(UINT8 *pBuffer, SINT32 *BufferLength)
{
    return m_dev.GetAudioSamples(pBuffer, BufferLength);
}

/*
void CVideoCapture::ForceIntraFrame()
{
}

void CVideoCapture::OSD_SetCharacterMap(UINT8 *CharactorMap, SINT32 StartIndex, SINT32 Number)
{
}

void CVideoCapture::OSD_DisplayCharacters(SINT32 x, SINT32 y, UINT8 *Index, SINT32 Number)
{
}

void CVideoCapture::OSD_ClearRegion(SINT32 x, SINT32 y, SINT32 Width, SINT32 Height)
{
}

SINT32 CVideoCapture::GPIO_Write(SINT32 Index, SINT32 Value)
{
    return SUCCESS;
}

SINT32 CVideoCapture::GPIO_Read(SINT32 Index, SINT32 *pValue)
{
    return SUCCESS;
}

SINT32 CVideoCapture::GPIO_BatchWork(SINT32 IndexNum, SINT32 Index[], UINT8 *WorkItem, SINT32 Interval, UINT8 *Result)
{
    return SUCCESS;
}

*/

SINT32 CVideoCapture::SetMDRegions(UINT8* arru8MotionCoordinates,
                   UINT32 u32MaxXCoordinate,
                   UINT32 u32MaxYCoordinate)
{   
    m_IOPacket.Type = IO_SET_MD_REGIONS;
    m_IOPacket.BufLen =  2*sizeof(UINT32)+ (u32MaxXCoordinate * u32MaxYCoordinate);
    memcpy(m_IOPacket.Buf, &u32MaxXCoordinate, sizeof(UINT32));
    memcpy(m_IOPacket.Buf+sizeof(UINT32), &u32MaxYCoordinate,
           sizeof(UINT32));
    memcpy(m_IOPacket.Buf+2*sizeof(UINT32),
           arru8MotionCoordinates,
           (u32MaxXCoordinate * u32MaxYCoordinate));

    m_dev.DeviceIO(&m_IOPacket);
    if(m_IOPacket.Param1 == 0)
    {
        return SUCCESS;
    }
    else
        return ERR_DEVICE;
}

SINT32 CVideoCapture::SetMDThresholdsAndSensitivities(UINT16 arrs16MotionThresholds[MAX_REGIONS_OF_INTEREST][NUM_MOTION_TYPES],
                                                      UINT16 *arru8MotionSensitivity)
{   
    m_IOPacket.Type = IO_SET_MD_THRESHOLDS_AND_SENSITIVITIES;
    m_IOPacket.BufLen = MAX_REGIONS_OF_INTEREST*sizeof(UINT16)*(1+NUM_MOTION_TYPES);
    memcpy(m_IOPacket.Buf, arru8MotionSensitivity, MAX_REGIONS_OF_INTEREST*sizeof(UINT16));
    memcpy((m_IOPacket.Buf+sizeof(UINT16)*MAX_REGIONS_OF_INTEREST), arrs16MotionThresholds,
           sizeof(UINT16)*MAX_REGIONS_OF_INTEREST*NUM_MOTION_TYPES);
    m_dev.DeviceIO(&m_IOPacket);
    if(m_IOPacket.Param1 == 0)
        return SUCCESS;
    else
        return ERR_DEVICE;
}

SINT32 CVideoCapture::WaitForMotion(SINT32& region)
{   
    m_IOPacket.Type = IO_WAIT_MD_EVENT;
    m_dev.DeviceIO(&m_IOPacket);
    region = m_IOPacket.Param2;
    return m_IOPacket.Param1;
}

SINT32 CVideoCapture::InitMotionDetection(void)
{   
    m_IOPacket.Type = IO_INIT_MD;
    m_dev.DeviceIO(&m_IOPacket);
    return 0;
}

SINT32 CVideoCapture::ResetMotionDetection(void)
{   
    m_IOPacket.Type = IO_MD_STOP;
    m_dev.DeviceIO(&m_IOPacket);
    return 0;
}

/*
int  CVideoCapture::GetLastMotionFactor()
{
    return m_LastFrameLength*m_LastQ;
}
*/


//////////////////////////////////////////////////////////////////
// static functions of IVideoCapture

SINT32 IVideoCapture::GetDeviceNumber()
{
  return 1;
}

SINT32 IVideoCapture::CreateInstance(VOID **pp, SINT8 instance)
{
    SINT32 Result;

    if(pp==NULL)
        return ERR_MEMORY;

    *pp=(VOID *)new CVideoCapture(instance, &Result);

    if(*pp==NULL)
        return ERR_MEMORY;

    if(Result == 0)
        return ERR_DEVICE;

    return SUCCESS;
}

void IVideoCapture::Release()
{
    delete (CVideoCapture *)this;
}


// following functions are for chip debugging

int CVideoCapture::ReadCBusReg(SINT32 RegNum, UINT16 Addr[], UINT16 Data[])
{
    m_IOPacket.Type = IO_READCBUSREGFW;
    m_IOPacket.BufLen = 2*RegNum;
    memcpy(m_IOPacket.Buf, Addr, 2*RegNum);

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
        memcpy(Data, m_IOPacket.Buf, 2*RegNum);
    }

    return m_IOPacket.Param1;
}

int CVideoCapture::WriteCBusReg(SINT32 RegNum, UINT16 AddrData[])
{
    m_IOPacket.Type = IO_WRITECBUSREGFW;
    m_IOPacket.BufLen = 4*RegNum;
    memcpy(m_IOPacket.Buf, AddrData, 4*RegNum);

    m_dev.DeviceIO(&m_IOPacket);

    return m_IOPacket.Param1;
}

int CVideoCapture::ReadHPIReg(UINT16 Addr, UINT16 *Data)
{
    m_IOPacket.Param1 = Addr;
    m_IOPacket.Type = IO_READHPIREGISTER;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
        *Data = m_IOPacket.Param2; 
    }

    return m_IOPacket.Param1;
}

int CVideoCapture::WriteHPIReg(UINT16 Addr, UINT16 Data)
{
    m_IOPacket.Param1 = Addr;
    m_IOPacket.Param2 = Data;
    m_IOPacket.Type = IO_WRITEHPIREGISTER;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);

    return m_IOPacket.Param1;
}

int CVideoCapture::WriteInterrupt(UINT16 IntVal, UINT16 IntData)
{
    m_IOPacket.Type = IO_WRITEINTERRUPT;
    m_IOPacket.Param1 = IntVal;
    m_IOPacket.Param2 = IntData;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);

    return m_IOPacket.Param1;
}

int CVideoCapture::ReadInterrupt(UINT16 *IntVal, UINT16 *IntData)
{
    m_IOPacket.Type = IO_READINTERRUPT;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
        *IntVal = (m_IOPacket.Param2 >> 16) & 0x0000ffff;
        *IntData = m_IOPacket.Param2 & 0x0000ffff;
    }

    return m_IOPacket.Param1;
}

int CVideoCapture::I2C_WriteRegister(UINT16 Addr, UINT8 Data)
{
	m_IOPacket.Type = IO_I2CWRITEREGISTER;
	m_IOPacket.Param1 = Addr;
	m_IOPacket.Param2 = Data;
	m_IOPacket.BufLen = 0;

	m_dev.DeviceIO(&m_IOPacket);
	return m_IOPacket.Param1;
}
int CVideoCapture::SendPacket(UINT8 *Buf, SINT32 BufLen)
{
    m_IOPacket.Type = IO_SENDPACKET;
    m_IOPacket.BufLen = BufLen;
    memcpy(m_IOPacket.Buf, Buf, BufLen);

    m_dev.DeviceIO(&m_IOPacket);

    return m_IOPacket.Param1;
}

int CVideoCapture::ReadStreamPipe(UINT8 *Buf, SINT32 BufLen)
{
    m_IOPacket.Type = IO_READSTREAMPIPE;
    m_IOPacket.BufLen = BufLen;

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
        memcpy(Buf, m_IOPacket.Buf, BufLen);
    }

    return m_IOPacket.Param1;
}

// Type == 0 : Set Power State , Param can be 0, 1, 2 or 3

int CVideoCapture::DoTestWork(int Type, int Param)
{
    m_IOPacket.Type = IO_ADVANCED;
    m_IOPacket.Param1 = Type;
    m_IOPacket.Param2 = Param;
    m_IOPacket.BufLen = 0;

    m_dev.DeviceIO(&m_IOPacket);

    return m_IOPacket.Param1;
}


SINT32 CVideoCapture::SetVideoSource(SINT32 source)
{
    m_IOPacket.Type = IO_SETVIDEO_SOURCE;
    m_IOPacket.BufLen = sizeof(SINT32);
    memcpy(m_IOPacket.Buf, &source, sizeof(SINT32));
    m_dev.DeviceIO(&m_IOPacket);

    return m_IOPacket.Param1;
}

SINT32 CVideoCapture::GetVideoSource(void)
{
        SINT32 source;
    m_IOPacket.Type = IO_GETVIDEO_SOURCE;
    m_dev.DeviceIO(&m_IOPacket);
    memcpy(&source, m_IOPacket.Buf, sizeof(SINT32));
    return source;
}


SINT32 CVideoCapture::ForceIFrame(void)
{
    m_IOPacket.Type = IO_FORCE_IFRAME;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param2;
}

SINT32 CVideoCapture::ChangePFrameRate(UINT16 rate)
{
    m_IOPacket.Type = IO_CHANGE_PFRAME_RATE;
    m_IOPacket.Param1 = rate;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param2;
}

SINT32 CVideoCapture::ChangeIFrameQuantizer(UINT16 iqscale)
{
    m_IOPacket.Type = IO_CHANGE_IFRAME_QUANTIZER;
    m_IOPacket.Param1 = iqscale;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param2;
}

SINT32 CVideoCapture::SetAudio(int AudioCode)
{

        m_IOPacket.Type = IO_SETAUDIO;
        m_IOPacket.Param1 = AudioCode;
        m_dev.DeviceIO(&m_IOPacket);
        return m_IOPacket.Param1;
}

SINT32 CVideoCapture::ChangePFrameQuantizer(UINT16 pqscale)
{
    m_IOPacket.Type = IO_CHANGE_PFRAME_QUANTIZER;
    m_IOPacket.Param1 = pqscale;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param2;
}

SINT32 CVideoCapture::InsertNewSequenceHeader(void)
{
    m_IOPacket.Type = IO_INSERT_SEQUENCE_HEADER_AT_NEXT_GOP;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param2;
}

SINT32 CVideoCapture::SetFPS(SINT32 fps)
{
    m_IOPacket.Type = IO_SET_FPS;
    m_IOPacket.Param1 = fps;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param2;
}

SINT32 CVideoCapture::ChangeBrightness(SINT32 brightness_value)
{
    m_IOPacket.Type = IO_SET_BRIGHTNESS;
    m_IOPacket.Param1 = brightness_value;
    m_IOPacket.BufLen = sizeof(SINT32);
    memcpy(m_IOPacket.Buf, &brightness_value, sizeof(SINT32));
    m_dev.DeviceIO(&m_IOPacket);                                                                                                
    return m_IOPacket.Param1;    	

}

SINT32 CVideoCapture::ChangeContrast(SINT32 contrast_value)
{
    m_IOPacket.Type = IO_SET_CONTRAST;
    m_IOPacket.Param1 = contrast_value;
    m_IOPacket.BufLen = sizeof(SINT32);
    memcpy(m_IOPacket.Buf, &contrast_value, sizeof(SINT32));
    m_dev.DeviceIO(&m_IOPacket);                                        
    return m_IOPacket.Param1;
                                                                                
}
SINT32 CVideoCapture::ChangeHue(SINT32 hue_value)
{
    m_IOPacket.Type = IO_SET_HUE;
    m_IOPacket.Param1 = hue_value;
    m_IOPacket.BufLen = sizeof(SINT32);
    memcpy(m_IOPacket.Buf, &hue_value, sizeof(SINT32));
    m_dev.DeviceIO(&m_IOPacket);                                               
    return m_IOPacket.Param1;                                                                          
}

SINT32 CVideoCapture::ChangeSaturation(SINT32 saturation_value)
{
    m_IOPacket.Type = IO_SET_SATURATION;
    m_IOPacket.Param1 = saturation_value;
    m_IOPacket.BufLen = sizeof(SINT32);
    memcpy(m_IOPacket.Buf, &saturation_value, sizeof(SINT32));
    m_dev.DeviceIO(&m_IOPacket);                                               
    return m_IOPacket.Param1;
                                                                                
}

SINT32 CVideoCapture::ReadCBus(UINT16 addr, UINT16* data)
{
    m_IOPacket.Type = IO_READCBUSREGFW;
    m_IOPacket.Param1 = (int)addr;
    m_dev.DeviceIO(&m_IOPacket);
    *data = m_IOPacket.Param2;
    return m_IOPacket.Param1;
}

SINT32 CVideoCapture::WriteCBus(UINT16 addr, UINT16 data)
{
    m_IOPacket.Type = IO_WRITECBUSREGFW;
    m_IOPacket.Param1 = (int)addr;
    m_IOPacket.Param2 = (int)data;    
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param1;
}

SINT32 CVideoCapture::SetTVStandard(UINT32 mask)
{
    m_IOPacket.Type = IO_SET_TV_STANDARD;
    m_IOPacket.Param1 = mask;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param2;
}

SINT32 CVideoCapture::BitrateControl(SINT32 change_direction,
                     SINT32 new_i_qscale,
                     SINT32 new_p_qscale,
                     SINT32 new_target_bitrate,
                     SINT32 new_peak_rate,
                     SINT32 new_vbv_buffer,
                     SINT32 new_converge_speed,
                     SINT32 new_lambda)
{
    m_IOPacket.Type = IO_SET_BITRATE;

    m_IOPacket.BufLen =  8*sizeof(UINT32);
    
    memcpy(m_IOPacket.Buf + 0*sizeof(UINT32),
       &change_direction,sizeof(UINT32));
    
    memcpy(m_IOPacket.Buf + 1*sizeof(UINT32),
       &new_i_qscale, sizeof(UINT32));
    
    memcpy(m_IOPacket.Buf + 2*sizeof(UINT32),
       &new_p_qscale, sizeof(UINT32));
    
    memcpy(m_IOPacket.Buf + 3*sizeof(UINT32),
       &new_target_bitrate,sizeof(UINT32));
    
    memcpy(m_IOPacket.Buf + 4*sizeof(UINT32),
       &new_peak_rate, sizeof(UINT32));
    
    memcpy(m_IOPacket.Buf + 5*sizeof(UINT32),
       &new_vbv_buffer, sizeof(UINT32));
    
    memcpy(m_IOPacket.Buf + 6*sizeof(UINT32),
       &new_converge_speed, sizeof(UINT32));
    
    memcpy(m_IOPacket.Buf + 7*sizeof(UINT32),
       &new_lambda, sizeof(UINT32));
    
    m_dev.DeviceIO(&m_IOPacket);

    return m_IOPacket.Param1;
}

//void CVideoCapture::SetResolution(TCFGVIDEO *pcfgvideo)
SINT32 CVideoCapture::SetResolution(SINT32 width, SINT32 height)
{
    m_IOPacket.Type = IO_SET_RESOLUTION;
    m_IOPacket.Param1 = width;
    m_IOPacket.Param2 = height;
    //m_IOPacket.BufLen = sizeof(TCFGVIDEO);
    //memcpy(m_IOPacket.Buf, pcfgvideo, sizeof(TCFGVIDEO) );
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param1;
}

SINT32 CVideoCapture::GetBoardInfo(void * info)
{
    m_IOPacket.Type = IO_GET_BOARD_TYPE;

    m_dev.DeviceIO(&m_IOPacket);
    memcpy(info, (void*)m_IOPacket.Buf, m_IOPacket.BufLen);
    return 0;
}
SINT32 CVideoCapture::sigDetect(void)
{   
    m_IOPacket.Type = IO_SIGDETECT;
    m_dev.DeviceIO(&m_IOPacket);
    return m_IOPacket.Param1;
}

SINT32 CVideoCapture::osd_show(unsigned char x,unsigned char y, void *String)
{   
    unsigned char *ptr;
	m_IOPacket.Type = IO_OSD_DISPLAY;
	m_IOPacket.BufLen = sizeof(osd_string_t)*y;//200;
	m_IOPacket.Param1 = (int)x;
    m_IOPacket.Param2 = (int)y;
    memcpy(m_IOPacket.Buf, String, m_IOPacket.BufLen);

    m_dev.DeviceIO(&m_IOPacket);
}

SINT32 CVideoCapture::av_sync(int delay_threshould, int check_inteval)
{   
    m_IOPacket.Type = IO_AV_SYNC_SETTING;
    m_IOPacket.Param1 = (int)delay_threshould;
    m_IOPacket.Param2 = (int)check_inteval;
    m_dev.DeviceIO(&m_IOPacket);
}

SINT32 CVideoCapture::GetAVStats(AVStats *pavs)
{
    m_IOPacket.Type = IO_GET_AVSTATS;
    m_IOPacket.BufLen = sizeof(AVStats);

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
      memcpy(pavs, (AVStats *)m_IOPacket.Buf, m_IOPacket.BufLen);
    }

    return m_IOPacket.Param1;
}

SINT32 CVideoCapture::GetSequenceHeader(UINT8 *pseqheadbuf, SINT32 *pseqheadlen)
{
    m_IOPacket.Type = IO_GET_SEQHEADER;
    m_IOPacket.BufLen = *pseqheadlen;

    m_dev.DeviceIO(&m_IOPacket);

    if(m_IOPacket.Param1 == 0)
    {
      memcpy(pseqheadbuf, m_IOPacket.Buf, m_IOPacket.BufLen);
      *pseqheadlen = m_IOPacket.BufLen;
    }

    return m_IOPacket.Param1;
}

