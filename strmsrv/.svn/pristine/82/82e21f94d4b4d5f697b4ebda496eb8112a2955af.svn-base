/*
 * encoderthread_test.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include <gtest/gtest.h>

#include "common.hh"
#include "squeue.hh"
#include "streamer.hh"
#include "vncencoderthr.hh"
#include "v2dencoderlistener.hh"
#include "watcher.hh"
#include "fbrx.hh"
#include "v2o_lib.h"

class MockStreamerX : public CStreamer {
public:
    MockStreamerX(int a_nId) :  CStreamer(a_nId) { };
    virtual int ProcessStream() {
        return 0;
    }
};

class MockV2DEncoderListener : public IV2DEncoderListener {
public:
    MockV2DEncoderListener() :
        m_bDone(false),
        m_nVideoResChanged(0),
        m_nCompressionChanged(0),
        m_nSliceSize(0),
        m_nMQuant(0),
        m_nDCQuant(0),
        m_lut()
        {};

    void LutChanged(struct LUT_struct * a_pLUT, int a_nSliceSize) {
        m_nSliceSize = a_nSliceSize;
        memcpy(&m_lut, a_pLUT, sizeof(struct LUT_struct));
        m_nVideoResChanged++;
    }

    void CompressionChanged(int a_nMQuant, int a_nDCQuant) {
        m_nMQuant = a_nMQuant;
        m_nDCQuant = a_nDCQuant;
        m_nCompressionChanged++;
    }

    void Stopped() { m_bDone = true;};
    bool IsDone() { return m_bDone; };
    int VideoResolutionChangedCount() { return m_nVideoResChanged; };
    int CompressionChangedCount() { return m_nCompressionChanged; };

    bool m_bDone;
    int m_nVideoResChanged;
    int m_nCompressionChanged;
    int m_nSliceSize;
    int m_nMQuant;
    int m_nDCQuant;
    struct LUT_struct m_lut;
};


class CVNCEncoderThreadTest : public testing::Test {
protected:

    virtual void SetUp() {
        DisableSSSyslog();
//        InitSSSyslog(true);
//        EnableSSSyslog();
//        SSSyslogTraceModule("squeue.cpp");

        m_queueManager = CSQueueManager::Instance();

        // Setup the collaborators of the object
        m_streamer = new MockStreamerX(1234);
        m_listener = new MockV2DEncoderListener();
        m_qEncoderSink = m_queueManager->GetQueueSink("V2DEncoder_1234");


        // Create the object under test
        m_encoderThread = new CVNCEncoderThread(m_streamer, m_listener);

        // Create watchers
        m_listenerwatcher = new CWatcher<MockV2DEncoderListener> (m_listener);
        m_encoderwatcher = new CWatcher<CVNCEncoderThread> (m_encoderThread);

    }

    virtual void TearDown() {
        delete m_listener;
        delete m_listenerwatcher;
        delete m_encoderwatcher;

        m_queueManager->ReleaseQueueSink(m_qEncoderSink);
        CWatcher<CSQueueManager> watcher(m_queueManager);
        watcher.WaitFor(&CSQueueManager::NumberOfSinks, 0);
        //m_queueManager->Destroy();

    }

    void SendVideoReolution(int width, int height) {
        CNewVideoResolutionInfo *encoderInfo = new CNewVideoResolutionInfo(width, height);
        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(encoderInfo);
        m_qEncoderSink->InsertFrame(pSFrame);
    }

    void SendCompressionParameters(int compression, int codecVersion, int sliceRefresh) {
        CEncoderInfo *encoderInfo =
                new CCompressionUpdate(compression, codecVersion, sliceRefresh);
        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(encoderInfo);
        m_qEncoderSink->InsertFrame(pSFrame);
    }

    void SendRectangle(int x, int y, int width, int height, int count, int index) {
        RFBRect_t * pRectangle = RFBCreateRectangle(x, y, width, height, count, index);
        CRectangleUpdate *encoderInfo = new CRectangleUpdate(pRectangle);
        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(encoderInfo);
        m_qEncoderSink->InsertFrame(pSFrame);
    }

    CSQueueManager *m_queueManager;
    MockStreamerX *m_streamer;
    MockV2DEncoderListener *m_listener;
    CSQueueSink * m_qEncoderSink;
    CWatcher<MockV2DEncoderListener> *m_listenerwatcher;
    CWatcher<CVNCEncoderThread> *m_encoderwatcher;
    CVNCEncoderThread *m_encoderThread;
};


TEST_F(CVNCEncoderThreadTest, StartStopThread) {
    m_encoderThread->Start();

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}



TEST_F(CVNCEncoderThreadTest, SetupVideoResolution) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);

    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::VideoResolutionChangedCount, 1));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}

TEST_F(CVNCEncoderThreadTest, SendMultipleVideoResolutionChanges) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);
    SendVideoReolution(1280, 1024);
    SendVideoReolution(1024, 768);
    SendVideoReolution(1280, 1024);
    SendVideoReolution(1024, 768);
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::VideoResolutionChangedCount, 5));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}

TEST_F(CVNCEncoderThreadTest, DuplicateVideoResolutionChange) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);
    SendVideoReolution(1024, 768);
    SendVideoReolution(1280, 1024);
    // Duplicate is allowed so we should get 3 callbacks
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::VideoResolutionChangedCount, 3));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}

TEST_F(CVNCEncoderThreadTest, VideoResolutionParameters) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::VideoResolutionChangedCount, 1));
    EXPECT_EQ(1024, m_listener->m_lut.a_hres);
    EXPECT_EQ(768, m_listener->m_lut.a_vres);
    EXPECT_EQ(1024, m_listener->m_lut.a_orig_hres);
    EXPECT_EQ(768, m_listener->m_lut.a_orig_vres);
    EXPECT_EQ(32, m_listener->m_nSliceSize);

    SendVideoReolution(1280, 1024);
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::VideoResolutionChangedCount, 2));
    EXPECT_EQ(1280, m_listener->m_lut.a_hres);
    EXPECT_EQ(1024, m_listener->m_lut.a_vres);
    EXPECT_EQ(1280, m_listener->m_lut.a_orig_hres);
    EXPECT_EQ(1024, m_listener->m_lut.a_orig_vres);
    EXPECT_EQ(32, m_listener->m_nSliceSize);

    SendVideoReolution(1920, 1200);
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::VideoResolutionChangedCount, 3));
    EXPECT_EQ(1920, m_listener->m_lut.a_hres);
    EXPECT_EQ(1200, m_listener->m_lut.a_vres);
    EXPECT_EQ(1920, m_listener->m_lut.a_orig_hres);
    EXPECT_EQ(1200, m_listener->m_lut.a_orig_vres);
    EXPECT_EQ(36, m_listener->m_nSliceSize);

    SendVideoReolution(1400, 1052);
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::VideoResolutionChangedCount, 4));
    EXPECT_EQ(1408, m_listener->m_lut.a_hres);
    EXPECT_EQ(1056, m_listener->m_lut.a_vres);
    EXPECT_EQ(1400, m_listener->m_lut.a_orig_hres);
    EXPECT_EQ(1052, m_listener->m_lut.a_orig_vres);
    EXPECT_EQ(32, m_listener->m_nSliceSize);

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}

TEST_F(CVNCEncoderThreadTest, CompressionParameters) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);
    SendCompressionParameters(6, 2, 15);
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::CompressionChangedCount, 1));
    EXPECT_EQ(3, m_listener->m_nMQuant);
    EXPECT_EQ(2, m_listener->m_nDCQuant);

    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetSliceRefresh, 15));
    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetCodecVersion, 2));

    SendCompressionParameters(4, 1, 5);

    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::CompressionChangedCount, 2));
    EXPECT_EQ(2, m_listener->m_nMQuant);
    EXPECT_EQ(1, m_listener->m_nDCQuant);

    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetSliceRefresh, 5));
    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetCodecVersion, 1));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}


TEST_F(CVNCEncoderThreadTest, EncodeRectangleWithoutVideoInitialization) {
    m_encoderThread->Start();

    SendRectangle(10, 10, 10, 10, 1, 0);
    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetDroppedRectangleCount, 1));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}

TEST_F(CVNCEncoderThreadTest, EncodeSingleRectangle) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);
    SendRectangle(10, 10, 10, 10, 1, 0);

    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetRectangleCount, 1));
    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetEncodedFrameCount, 1));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}

TEST_F(CVNCEncoderThreadTest, EncodeMultipleRectanglesInOneFrame) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);
    SendRectangle(10, 10, 10, 10, 4, 0);
    SendRectangle(10, 10, 10, 10, 4, 1);
    SendRectangle(10, 10, 10, 10, 4, 2);
    SendRectangle(10, 10, 10, 10, 4, 3);

    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetRectangleCount, 4));
    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetEncodedFrameCount, 1));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}

TEST_F(CVNCEncoderThreadTest, EncodeMultipleRectanglesAndFrames) {
    m_encoderThread->Start();

    SendVideoReolution(1024, 768);

    // Send three frames worth of rectangles
    SendRectangle(10, 10, 10, 10, 4, 0);
    SendRectangle(10, 10, 10, 10, 4, 1);
    SendRectangle(10, 10, 10, 10, 4, 2);
    SendRectangle(10, 10, 10, 10, 4, 3);

    SendRectangle(10, 10, 10, 10, 1, 0);

    SendRectangle(10, 10, 10, 10, 4, 0);
    SendRectangle(10, 10, 10, 10, 4, 1);
    SendRectangle(10, 10, 10, 10, 4, 2);
    SendRectangle(10, 10, 10, 10, 4, 3);

    // Send partial frame
    SendRectangle(10, 10, 10, 10, 4, 0);
    SendRectangle(10, 10, 10, 10, 4, 1);

    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetRectangleCount, 11));
    EXPECT_TRUE(m_encoderwatcher->WaitFor(&CVNCEncoderThread::GetEncodedFrameCount, 3));

    m_encoderThread->Stop();
    EXPECT_TRUE(m_listenerwatcher->WaitFor(&MockV2DEncoderListener::IsDone));
}
