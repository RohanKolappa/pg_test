/*
 * vsplashframe.cpp
 */

#include "strmsrv_compatibility.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#ifdef __XPI__
#include "gd.h"
#include "gdfontg.h"
#else
#include <gd.h>
#include <gdfontg.h>
#endif
#include "splash.hh"
#include "timekeeper.hh"
#include "common.hh"
#include "png.hh"
#include "v2o_lib.h"

#include "encodermutex.hh"
#include "vframeencoder.hh"
#include "codecdefs.h"
#include "netutils.hh"

#define SPLASH_WRITE_SIZE      1024

CVideoSplashFrame::CVideoSplashFrame(unsigned short a_hres,
        unsigned short a_vres,unsigned short a_mquant,
        unsigned short a_dcquant, unsigned short a_slicesize,
        int a_type, int a_codecversion,  bool a_stereo, int a_lrflag, bool a_animate)
{
    InitializeVariables();
   
    eEncodeFormat e_EncoderFormat = eYUV422P;
    m_nHRes = a_hres;
    m_nVRes = a_vres;
    m_nMQuant = a_mquant;
    m_nDCQuant = a_dcquant;
    m_nSliceSize = a_slicesize;
    m_nType = a_type;
    m_bStereo = a_stereo;
    m_nLRFlag = a_lrflag;
    m_bAnimate = a_animate;
    m_nRectWidth = 50;
    m_nRectHeight = 50;
    m_nRectXLocation = 50;
    m_nRectYLocation = 50;

    m_pFrameBuffer = new unsigned char[m_nHRes * m_nVRes * 4];
    m_pEncodedBuffer = new unsigned char[m_nHRes * m_nVRes];
    memset(m_pEncodedBuffer, 0x00, m_nHRes * m_nVRes);

    if(a_codecversion == 1)
        e_EncoderFormat = eYUV422P;
    else if(a_codecversion == 2)
        e_EncoderFormat = eSVC; 
  
    m_pFrameEncoder = new CVideoFrameEncoder(m_nHRes, m_nVRes, m_nMQuant,
           m_nDCQuant, e_EncoderFormat);
    m_pFrameEncoder->SetLeftRightFlag(m_nLRFlag);
}

CVideoSplashFrame::~CVideoSplashFrame()
{
    delete m_pFrameEncoder;
    delete [] m_pFrameBuffer;
    delete [] m_pEncodedBuffer;
    delete [] m_pRectangleBuffer;
    delete [] m_pRectangleBackground;
    delete [] m_pTimeStampBuffer;
    delete [] m_pTimeStampBackground;
    delete [] m_sTimeStamp;
    delete [] m_sHostName;
    delete [] m_sIPAddress;

    m_pFrameEncoder = NULL;

}

void CVideoSplashFrame::InitializeVariables() {
    m_pWritePtr = NULL;
    m_pEndPtr = NULL;
    m_pFrameEncoder = NULL;
    m_pFrameBuffer = NULL;
    m_pEncodedBuffer = NULL;
    m_pRectangleBuffer = NULL;
    m_pRectangleBackground = NULL;
    m_pTimeStampBuffer = NULL;
    m_pTimeStampBackground = NULL;
    m_nFramesCreated = 0;
    m_nLastFrameTimestamp = CTimeKeeper::Now();
    m_bServing = true;
    m_sHostName = NULL;
    m_sIPAddress = NULL;
    m_sTimeStamp = NULL;
    m_nEncodedLength = 0;
    m_nDeltaX = 0;
    m_nDeltaY = 0;
}

void CVideoSplashFrame::CreateBackgroundImage()
{
    int i, j;
    int k;
    unsigned char *cp;
    unsigned char l;

    // Input is in BGRA format
    cp = m_pFrameBuffer;


    for (j = 0; j < m_nVRes; j++) {
        k = 0;
        for (i = 0; i < m_nHRes; i++) {

            if (m_nType & SPLASH_TYPE_BLUE) {

#if 0
                // blue background
                *cp++ = 255;
                *cp++ = 0;
                *cp++ = 0;
                *cp++ = 0;
#else
                // Color gradient
                *cp++ = (j * 256) / m_nVRes; /* blue */
                //*cp++ = 0;

                //*cp++ = (i * 128) / m_nHRes; /* green */
                *cp++ = 0;

                *cp++ = ((i + j) * 64)/(m_nHRes + m_nVRes); /* red */
                //*cp++ = 0;

                *cp++ = 0;
#endif
            }
            else {
                // Grid pattern
                if (j < (m_nVRes * 1 / 4))
                    l = 3;
                else if (j < (m_nVRes * 2 / 4))
                    l = 2;
                else if (j < (m_nVRes * 3 / 4))
                    l = 1;
                else
                    l = 0;

                l = l % 4;

                l = 1 << l;

                if (i % l == 0)
                    k = ~k;

                if (k) {
                    *cp++ = 255;
                    *cp++ = 255;
                    *cp++ = 255;
                }
                else {
                    *cp++ = 0;
                    *cp++ = 0;
                    *cp++ = 0;
                }
                *cp++ = 0;
            }
        }
    }
}


void CVideoSplashFrame::DrawRectangle(int x, int y, int a_width, int a_height,
        unsigned char * a_pRectangleBuffer)
{


    int width = a_width;
    int height = a_height;
    width = (x + width <= m_nHRes) ? width : m_nHRes - x;
    height = (y + height <= m_nVRes) ? height : m_nVRes - y;

    unsigned char *dst = m_pFrameBuffer;
    dst += (y * m_nHRes * 4) + x * 4;

    unsigned char *src = a_pRectangleBuffer;
    for (int j = 0; j < height; j++) {
        memcpy(dst, src, width * 4);
        src += (a_width * 4);
        dst += (m_nHRes * 4);
    }

    // Tell Encoder about the area we just updated
    m_pFrameEncoder->UpdateRectangle(x, y, width, height);

}

// Save a portion of our frame buffer into the provided rectangle
void CVideoSplashFrame::SaveRectangle(int x, int y, int a_width, int a_height,
        unsigned char * a_pRectangleBuffer)
{
    int width = a_width;
    int height = a_height;
    width = (x + width <= m_nHRes) ? width : m_nHRes - x;
    height = (y + height <= m_nVRes) ? height : m_nVRes - y;

    unsigned char *dst = a_pRectangleBuffer;

    unsigned char *src = m_pFrameBuffer;
    src += (y * m_nHRes * 4) + x * 4;

    for (int j = 0; j < height; j++) {
        memcpy(dst, src, width * 4);
        src += (m_nHRes * 4);
        dst += (a_width * 4);
    }

}

void CVideoSplashFrame::MoveRectangle()
{
    // Wipe out previous Rectangle with background rectangle
    DrawRectangle(m_nRectXLocation, m_nRectYLocation, m_nRectWidth, m_nRectHeight,
            m_pRectangleBackground);

    // Compute new location
    m_nRectXLocation += m_nDeltaX;
    if (m_nRectXLocation < 0) {
        m_nRectXLocation = 0;
        m_nDeltaX = -m_nDeltaX;
    }
    if (m_nRectXLocation >= m_nHRes - m_nRectWidth) {
        m_nRectXLocation = m_nHRes - m_nRectWidth;
        m_nDeltaX = -m_nDeltaX;
    }

    m_nRectYLocation += m_nDeltaY;
    if (m_nRectYLocation < 0) {
        m_nRectYLocation = 0;
        m_nDeltaY = -m_nDeltaY;
    }
    if (m_nRectYLocation >= m_nVRes - m_nRectHeight) {
        m_nRectYLocation = m_nVRes - m_nRectHeight;
        m_nDeltaY = -m_nDeltaY;
    }

    // Save off background at new location into our background buffer
    SaveRectangle(m_nRectXLocation, m_nRectYLocation, m_nRectWidth, m_nRectHeight,
            m_pRectangleBackground);

    // Draw rectangle at new location
    DrawRectangle(m_nRectXLocation, m_nRectYLocation, m_nRectWidth, m_nRectHeight,
            m_pRectangleBuffer);

    // Randomize motion
    if (m_nFramesCreated % 100 == 0)
        ComputeDeltas();
}

// Retrieves "hostname" of machine. If not in stand-alone mode and
// XMPP server is configured, retrieves the Agent ID instead
bool CVideoSplashFrame::GetHostName(char * a_sBuffer, int a_nLen) {
    char hostname[255];

    strcpy(a_sBuffer, "");

    if (gethostname(hostname, 255) != 0)
        return false;

    strncpy(a_sBuffer, hostname, a_nLen);
    a_sBuffer[a_nLen - 1] = 0;

    // See if need to plugin XMPP Agent Id
    if (!is_standalone_enabled()) {
        struct xmpp_agent_config xmpp_agent_cfg;
        get_xmpp_agent_config(&xmpp_agent_cfg);
        if (xmpp_agent_cfg.enable) {
            strncpy(a_sBuffer, xmpp_agent_cfg.devicename, a_nLen);
            a_sBuffer[a_nLen - 1] = 0;
        }
    }
    return true;
}

bool CVideoSplashFrame::GetSystemInfo() {

    bool retval = false;
    if (m_sTimeStamp == NULL) {
        m_sTimeStamp = new char [64];
        strcpy(m_sTimeStamp, "");
        m_sHostName = new char [33];
        strcpy(m_sHostName, "");
        m_sIPAddress = new char [33];
        strcpy(m_sIPAddress, "");
    }

    // Create String for current time
    if (m_bAnimate) {
        time_t tt;
        tt = CTimeKeeper::Now()/1000;
        struct tm tm;
        gmtime_r(&tt, &tm);

        char timestring[32];
        sprintf(timestring, "%02d:%02d:%02d.%03d",
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                (int)(CTimeKeeper::Now() % 1000));

        if (strcmp(timestring, m_sTimeStamp) != 0) {
            strcpy(m_sTimeStamp, timestring);
            retval = true;
        }
    }

    // Get Hostname/IP address strings
    if (m_nFramesCreated % 24 == 1) {  // Not too often
        char sHostName[33];
        if (GetHostName(sHostName, 32)) {
            if (strncmp(sHostName, m_sHostName, 32) != 0) {
                strncpy(m_sHostName, sHostName, 32);
                m_sHostName[32] = 0;
                retval = true;
            }
        }
        char sIPAddress[33];
        if (((CNETutils *)NULL)->GetLocalIPAddress(sIPAddress, 32)) {
            if (strncmp(sIPAddress, m_sIPAddress, 32) != 0) {
                strncpy(m_sIPAddress, sIPAddress, 32);
                m_sIPAddress[32] = 0;
                retval = true;
            }
        }
    }
    return retval;
}

void CVideoSplashFrame::DisplaySystemInfo() {

    // Create Image for time string using GD library
    int width = 350;
    int height = 72;
    int xPosition;
    int yPosition;

    // Draw system info on right side
    xPosition = m_nHRes - (16 + width);
    yPosition = 72;

    gdImagePtr im;
    im = gdImageCreate(width, height);
    int backgroundcolor = gdImageColorAllocate(im, 0, 0, 255); // Blue background

    int foregroundcolor;
    if (m_nType & SPLASH_TYPE_TX) {
        foregroundcolor = gdImageColorAllocate(im, 0, 255, 0);
    }
    else {
        foregroundcolor = gdImageColorAllocate(im, 255, 0, 0);
    }
    int brect[8];
    char *fontpath = (char *) "/usr/share/fonts/ipvs.ttf";

    char * err;

    if (m_bStereo) {
        char sLRString[16];
        if (m_nLRFlag == 0)
            strcpy(sLRString, "Left");
        else
            strcpy(sLRString, "Right");
        err = gdImageStringFT(im, &brect[0], foregroundcolor, fontpath,
                32.0, 0.0, 8, height/2 - 8 , sLRString);
    }
    else {
        err = gdImageStringFT(im, &brect[0], foregroundcolor, fontpath,
                32.0, 0.0, 8, height/2 - 8 , m_sHostName);
    }
    if (err != NULL)
        return;

    if (m_bAnimate) {
        err = gdImageStringFT(im, &brect[0], foregroundcolor, fontpath,
                24.0, 0.0, 8, height - 1, m_sTimeStamp);
    }
    else {
        err = gdImageStringFT(im, &brect[0], foregroundcolor, fontpath,
                24.0, 0.0, 8, height - 1, m_sIPAddress);
    }
    if (err != NULL)
        return;


    // Extract time string image into our rectangle BGRA buffer
    if (m_pTimeStampBuffer == NULL) {
        m_pTimeStampBuffer = new unsigned char [width * height * 4];
    }
    if (m_pTimeStampBackground == NULL) {
        // Save off initial background
        m_pTimeStampBackground = new unsigned char [width * height * 4];
        SaveRectangle(xPosition, yPosition, width, height, m_pTimeStampBackground);
    }
    // Copy background pattern and then write the text over it
    memcpy(m_pTimeStampBuffer, m_pTimeStampBackground, width * height * 4);
    unsigned char *cp = m_pTimeStampBuffer;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            int c = gdImageGetPixel(im, i, j);
            if (c != backgroundcolor) {
                int r = gdImageRed(im, c);
                int g = gdImageGreen(im, c);
                int b = gdImageBlue(im, c);
                *cp++ = b;
                *cp++ = g;
                *cp++ = r;
                *cp++ = 0;
            }
            else {
                cp += 4;
            }
        }

    }

    // Paint the rectangle
    DrawRectangle(xPosition, yPosition, width, height, m_pTimeStampBuffer);

    // Cleanup
    gdImageDestroy(im);

}
void CVideoSplashFrame::CreateRectangle()
{
    char *logofile;


    if (m_nType & SPLASH_TYPE_TX) {
        logofile = (char *) "/V2O/config/txlogo.png";
    }
    else {
        logofile = (char *) "/V2O/config/rxlogo.png";
    }
    CPng png(logofile);
    if (png.Ok()) {
        m_nRectWidth = png.GetWidth();
        m_nRectHeight = png.GetHeight();
    }

    // Create foreground
    m_pRectangleBuffer = new unsigned char [m_nRectWidth * m_nRectHeight * 4];
    unsigned char * cp = m_pRectangleBuffer;
    for (int j = 0; j < m_nRectWidth; j++) {
        for (int i = 0; i < m_nRectHeight; i++) {
            *cp++ = 0;
            *cp++ = 255;
            *cp++ = 0;
            *cp++ = 0;
        }
    }
    if (png.Ok()) {
        png.GetBGRA(m_pRectangleBuffer);
    }


    // Set rectangle's initial location and velocity
    if (m_bAnimate) {
        m_nRectXLocation = m_nHRes / 2;
        m_nRectYLocation = m_nVRes / 2;
        ComputeDeltas();
    }
    else {
        // m_nRectXLocation = m_nHRes - (50 + m_nRectWidth);
        m_nRectXLocation = 50;
        m_nRectYLocation = 50;
        m_nDeltaX = 0;
        m_nDeltaY = 0;
    }

    // Create area to store background rectangle
    m_pRectangleBackground = new unsigned char [m_nRectWidth * m_nRectHeight * 4];

    // Save off background at initial location into our background buffer
    SaveRectangle(m_nRectXLocation, m_nRectYLocation, m_nRectWidth, m_nRectHeight,
            m_pRectangleBackground);

}

void CVideoSplashFrame::ComputeDeltas()
{
    m_nDeltaX = (rand() % 5) - 2;
    m_nDeltaY = (rand() % 5) - 2;
    if (m_nDeltaX == 0 && m_nDeltaY == 0) {
        m_nDeltaX = (rand() % 5) - 2;
        m_nDeltaY = (rand() % 5) - 2;
    }
}

void CVideoSplashFrame::EncodeFirstFrame()
{
    timestamp_t last = CTimeKeeper::GetTime();

    // Create background image
    CreateBackgroundImage();
    timestamp_t timeSpent = CTimeKeeper::GetTime() - last;
    SYSLOG_DEBUG("Created Background Image in %llu ms", timeSpent);
    m_nFramesCreated++;

    // Setup Encoder
    last = CTimeKeeper::GetTime();
    m_pFrameEncoder->SetBitmap(m_pFrameBuffer);
    m_pFrameEncoder->SetMutex(new CEncoderMutex());
    m_pFrameEncoder->SetSliceRefresh(5);

    // Draw logo at initial position
    CreateRectangle();
    MoveRectangle();

    // Draw system information
    if (GetSystemInfo())
        DisplaySystemInfo();

    // Encode first image
    m_pFrameEncoder->UpdateAll();
    m_pFrameEncoder->EncodeFrame();
    m_nEncodedLength = m_pFrameEncoder->GetNextFrame(m_pEncodedBuffer);

    timeSpent = CTimeKeeper::GetTime() - last;
    SYSLOG_DEBUG("Created %s motion SPLASH of size %d in %llu ms",
           (m_nLRFlag == 0) ? "Left" : "Right", m_nEncodedLength, timeSpent);
    m_nEncodedLength += 1024 - (m_nEncodedLength % 1024);

    m_pWritePtr = m_pEncodedBuffer;
    m_pEndPtr = m_pEncodedBuffer + m_nEncodedLength;
}

// Determine if we need to animate.
// No point creating new frames unless someone is actively
// retrieving them -- saves CPU cycles.
bool CVideoSplashFrame::Animate() {
    if (m_bServing) {
        if ((CTimeKeeper::Now() - m_nLastFrameTimestamp) > 1000) {
            SYSLOG_DEBUG("Stopped Animating %llu\n", m_nLastFrameTimestamp);
            m_bServing = false;
        }
    }

    return m_bServing;
}

void CVideoSplashFrame::SetFSRefresh()
{
    if (m_pFrameEncoder != NULL)
        m_pFrameEncoder->SetFSRefresh();
}

void CVideoSplashFrame::EncodeFrame()
{
    if (GetSystemInfo())
        DisplaySystemInfo();

    if (m_pFrameEncoder != NULL)
        m_pFrameEncoder->EncodeFrame();

    m_nFramesCreated++;

    SYSLOG_DEBUG("Created %s Motion Frame %lu",
            (m_nLRFlag == 0) ? "Left" : "Right", m_nFramesCreated);
}

int CVideoSplashFrame::GetMotionPacket(char * a_buffer, bool * a_eof)
{
    int pktlen = SPLASH_WRITE_SIZE;

    *a_eof = false;

    // If we do not have enough bytes, set eof & return what we have
    if ((m_pEndPtr - m_pWritePtr) <= SPLASH_WRITE_SIZE) {
        memset(a_buffer, 0, SPLASH_WRITE_SIZE);
        pktlen = (m_pEndPtr - m_pWritePtr);
        *a_eof = true;
    }

    if (pktlen == 0)
        return 0;

    memcpy(a_buffer, m_pWritePtr, pktlen);

    m_pWritePtr += pktlen;

    return SPLASH_WRITE_SIZE;
}

int CVideoSplashFrame::GetNextFrame()
{
    int retval = 0;

    // Maintain 24 FPS. Get a new frame only if sufficient time has
    // elapsed since last frame.
    if ((CTimeKeeper::Now() - m_nLastFrameTimestamp) > 41) {
        int nRemainingBytes = m_pEndPtr - m_pWritePtr;
        memmove(m_pEncodedBuffer, m_pWritePtr, nRemainingBytes);
        m_nEncodedLength =
            m_pFrameEncoder->GetNextFrame(m_pEncodedBuffer + nRemainingBytes);
        SYSLOG_DEBUG("SPLASH: Serving new %s frame of size %d (remaining %d)",
            (m_nLRFlag == 0) ? "Left" : "Right", m_nEncodedLength,
            nRemainingBytes);

        m_nLastFrameTimestamp = CTimeKeeper::Now();

        m_pWritePtr = m_pEncodedBuffer;
        m_pEndPtr = m_pEncodedBuffer + m_nEncodedLength + nRemainingBytes;
        retval = m_nEncodedLength + nRemainingBytes;
    }
    m_bServing = true;

    return retval;
}
