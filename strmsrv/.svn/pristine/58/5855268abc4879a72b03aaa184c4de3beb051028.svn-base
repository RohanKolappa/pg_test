/*
 * rendererframeinfo.h
 *
 *  Created on: Jul 13, 2010
 *      Author: rkale
 */

#ifndef RENDERERMESSAGE_H_
#define RENDERERMESSAGE_H_

enum eScalingMode {
    OneToOne = 1,
    KeepAspect,
    Fill,
};

#define SCALING_MODE_NATIVE         0
#define SCALING_MODE_KEEP_ASPECT    1
#define SCALING_MODE_FILL           2

class CRendererMessage {

public:
    enum MessageType {
        FrameInfo = 1,
        DebugVideo,
        RFBStatus,
        RFBMouseEvent,
        RFBKeyboardEvent,
        ScalingRequest,
    };

    CRendererMessage(MessageType a_nMessageType) {
        m_nMessageType = a_nMessageType;
    }
    ~CRendererMessage() {};

    MessageType GetMessageType() {
        return m_nMessageType;
    }

private:
    MessageType m_nMessageType;
};

class CRendererFrameInfo : public CRendererMessage {

public:

    CRendererFrameInfo(int width, int height,
            int scaledWidth, int scaledHeight) : CRendererMessage(FrameInfo)
    , m_nWidth(width)
    , m_nHeight(height)
    , m_nScaledWidth(scaledWidth)
    , m_nScaledHeight(scaledHeight)
    {
    }
    ~CRendererFrameInfo() {};
    int Width() { return m_nWidth; };
    int Height() { return m_nHeight; };
    int ScaledWidth() { return m_nScaledWidth; };
    int ScaledHeight() { return m_nScaledHeight; };

private:
    int m_nWidth;
    int m_nHeight;
    int m_nScaledWidth;
    int m_nScaledHeight;
};

class CRendererDebugVideo : public CRendererMessage {

public:

    CRendererDebugVideo(int aDebugLevel) : CRendererMessage(DebugVideo)
    , m_nDebugLevel(aDebugLevel)
    {
    }
    ~CRendererDebugVideo() {};
    int DebugLevel() { return m_nDebugLevel; };

private:
    int m_nDebugLevel;
};

class CRendererRFBStatus : public CRendererMessage {

public:

    CRendererRFBStatus(bool a_bRFBReady) : CRendererMessage(RFBStatus)
    , m_bRFBReady(a_bRFBReady)
    {
    }
    ~CRendererRFBStatus() {};
    int IsRFBReady() { return m_bRFBReady; };

private:
    bool m_bRFBReady;
};

class CRendererRFBMouseEvent : public CRendererMessage {

public:

    CRendererRFBMouseEvent(uint16_t a_x, uint16_t a_y, uint8_t a_buttonmask)
    : CRendererMessage(RFBMouseEvent)
    , m_x(a_x), m_y(a_y), m_buttonmask(a_buttonmask)
    {
    }
    ~CRendererRFBMouseEvent() {};
    uint16_t x() { return m_x; };
    uint16_t y() { return m_y; };
    uint8_t buttonmask() { return m_buttonmask; };

private:
    uint16_t m_x;
    uint16_t m_y;
    uint8_t m_buttonmask;
};

class CRendererRFBKeyboardEvent : public CRendererMessage {

public:

    CRendererRFBKeyboardEvent(uint32_t a_key, uint8_t a_down)
    : CRendererMessage(RFBKeyboardEvent)
    , m_key(a_key), m_down(a_down)
    {
    }
    ~CRendererRFBKeyboardEvent() {};
    uint32_t key() { return m_key; };
    uint8_t down() { return m_down; };

private:
    uint32_t m_key;
    uint8_t m_down;
};

class CRendererScalingRequest : public CRendererMessage {

public:

    CRendererScalingRequest(int a_scalingMode, int a_scaledWidth, int a_scaledHeight)
    : CRendererMessage(ScalingRequest)
    , m_scalingMode(a_scalingMode), m_scaledWidth(a_scaledWidth), m_scaledHeight(a_scaledHeight)
    {
    }
    ~CRendererScalingRequest() {};
    int scaledWidth() { return m_scaledWidth; };
    int scaledHeight() { return m_scaledHeight; };
    int scalingMode() { return m_scalingMode; };
private:
    int m_scalingMode;
    int m_scaledWidth;
    int m_scaledHeight;
};

#endif /* RENDERERMESSAGE_H_ */
