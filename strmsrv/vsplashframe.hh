/*
 * videosplashframe.hh
 */

#ifndef VIDEOSPLASHFRAME_HH_
#define VIDEOSPLASHFRAME_HH_

#include <sys/time.h>
#include "v2d.h"
#include "v2o_lib.h"
#include "timekeeper.hh"

class CVideoFrameEncoder;

class CVideoSplashFrame {
public:

    CVideoSplashFrame(unsigned short a_hres, unsigned short a_vres, 
            unsigned short a_mquant, unsigned short a_dcquant,
            unsigned short a_slicesize, int a_type, int a_codecversion, bool a_stereo, 
            int a_lrflag, bool a_animate);

    ~CVideoSplashFrame();

    void MoveRectangle();
    int GetMotionPacket(char * a_buffer, bool * eof);
    void SetFSRefresh();
    int GetNextFrame();
    void EncodeFirstFrame();
    void EncodeFrame();
    bool Animate();
    void DisplaySystemInfo();

    int MotionRun();

private:
    unsigned short m_nHRes;
    unsigned short m_nVRes;
    unsigned short m_nMQuant;
    unsigned short m_nDCQuant;
    unsigned short m_nSliceSize;
    int m_nType;
    bool m_bStereo;
    int m_nLRFlag;
    bool m_bAnimate;

    CVideoFrameEncoder *m_pFrameEncoder;
    unsigned char * m_pFrameBuffer;
    unsigned char * m_pEncodedBuffer;
    unsigned char * m_pWritePtr;
    unsigned char * m_pEndPtr;
    int m_nEncodedLength;
    unsigned long m_nFramesCreated;
    unsigned char * m_pRectangleBuffer;
    unsigned char * m_pRectangleBackground;
    unsigned char * m_pTimeStampBuffer;
    unsigned char * m_pTimeStampBackground;
    char * m_sHostName;
    char * m_sIPAddress;
    char * m_sTimeStamp;
    int m_nRectXLocation;
    int m_nRectYLocation;
    int m_nRectWidth;
    int m_nRectHeight;
    int m_nDeltaX;
    int m_nDeltaY;
    bool m_bServing;
    timestamp_t m_nLastFrameTimestamp;

    void InitializeVariables();
    void CreateBackgroundImage();
    void CreateRectangle();
    void DrawRectangle(int x, int y, int a_width, int a_height,
            unsigned char * a_pRectangleBuffer);
    void SaveRectangle(int x, int y, int a_width, int a_height,
            unsigned char * a_pRectangleBuffer);
    void ComputeDeltas();
    bool GetHostName(char * a_sBuffer, int a_nLen);
    bool GetSystemInfo();
};

#endif /* VIDEOSPLASHFRAME_HH_ */
