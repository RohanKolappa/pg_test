#ifndef __HH_WISENCRX_H__
#define __HH_WISENCRX_H__

#include <pthread.h>
#include <unistd.h>

#include "mutex.hh"
#include "receiver.hh"
#include "command.hh"
#include <VideoCapture.h>
#include <avcodec.h>
#include "MPEGTSMuxFilter.hh"
#include "common.hh"

#ifdef USE_IPP_AAC
#include <aac_enc.h>
#endif

// Video Size
#define CIF 0
#define D1  1

// Audio compression
#define MPA     0
#define AAC     1

// Encapsulation
#define RAW    0
#define MPEGTS 1

//AVDisable masks
#define NOAUDIO  0x01
#define NOVIDEO  0x02

#define MAX_WIS_PORTS 12

#define DEFAULT_HUE 0
#define DEFAULT_SATURATION 64
#define DEFAULT_BRIGHTNESS 49
#define DEFAULT_CONTRAST 48

typedef struct AVStat {
  timestamp_t tTime;
  unsigned long nAudioSamples;
  unsigned long nVideoFrames;
  unsigned long nAudioBytes;
  unsigned long nVideoBytes;
  unsigned long nRawAudioBytes;
  unsigned long nRawVideoBytes;
  unsigned long nDWF_Audio;
  unsigned long nDWF_Video;
  unsigned long nATimeSetCount;
  unsigned long nVTimeSetCount;
  unsigned long nTimeStampSetCount;
  unsigned long nGetSeqHeaderCount;
} AVStat;


class CWisEncRx: public CReceiver
{
private:
  int m_nPort;

  int m_nDev;
  int m_nVideoSource;
  int m_nVideoSize;
  int m_nVideoTVStandard;
  int m_nVideoCompression;
  int m_nVideoBitrate;
  int m_nQuant;
  int m_nAudioChannels;
  int m_nAudioCompression;
  int m_nAudioBitrate;
  int m_nEncapsulation;
  int m_nAVDisable;

  int m_nSaturation;
  int m_nHue;
  int m_nBrightness;
  int m_nContrast;

  unsigned long m_nAudioFrameType;
  unsigned long m_nVideoFrameType;

  int m_bAudioWorkState;
  int m_bVideoWorkState;
  
  int m_sleep;
  int m_got_audio;
  int m_got_video;
  int m_dwf_audio;
  int m_dwf_video;

  int m_nALen;
  int m_nVLen;
  timestamp_t m_tATime;
  timestamp_t m_tVTime;
  timestamp_t m_tTime;
  double m_fATime;
  double m_fVTime;

  AVStat m_AVStatCurr;
  AVStat m_AVStatLast;

  int m_nFrameLen;
  unsigned long m_nFrameType;
  timestamp_t m_tFrameTime;
  char *m_pFrameData;

  int m_nAPTSperSec;
  int m_nVPTSperSec;
  int m_nAPTSperFrame;
  int m_nVPTSperFrame;
  double m_fAtsperFrame;
  double m_fVtsperFrame;
  int m_nAlosserr;
  int m_nAbursterr;
  int m_nVlosserr;
  int m_nVbursterr;

  int m_nVideoFrameLen;
  int m_nVideoFrameBufOffset;
  int m_nAudioFrameLen;
  UINT8 *m_pVideoFrameBuf;
  UINT8 *m_pAudioFrameBuf;
  UINT8 *m_pAudioEncFrameBuf;

  char  *m_pVideoCaptureState;
  IVideoCapture *m_pVideoCapture;
  AUDIO_CONFIG m_AudioConfig;
  TCFGVIDEO m_VideoConfig;
  TVIDEOCFGFIX m_VideoConfigFix;
  TFrameInfo m_FrameInfo;
  AVStats m_WISAVStatsCurr;
  AVStats m_WISAVStatsLast;

  AVCodecContext *m_acodec;
#ifdef USE_IPP_AAC
  AACEnc *m_IPPaacencState;
#else
  void *m_dummy;
#endif

  MPEGTSMuxFilter *m_pMPEGTSMuxFilter;

  pthread_t m_tStartStopCaptureThread;
        
  /// Other Private functions
  void InitConfig();
  void SetConfig(CCommand *pCmd);
  void SetFixConfig();

  void InitAudioCodec();
  int InitVideoSeqHeader();
  void InitFrameBuffers();
  
  static CMutex m_nPortLocks[MAX_WIS_PORTS + 1];  //+1 for all ports lock
  static void *doStartStopCapture(void *pThis);

  int StartCapture(void);
  int Port2Dev(int port);

  void GetAudioFrame();
  void GetVideoFrame();
  int SendAudioFrame();
  int SendVideoFrame();

  int SendFrame();
public:
  CWisEncRx(int nId, 
            int nPort,
            CCommand *pCmd);

  ~CWisEncRx();
  void UpdateCustomStatus(CCommand *a_pCmd);
  int  Update(CCommand *a_pCmd);

  int ProcessStream();
};

#endif /*__HH_WISENCRX_H__*/
