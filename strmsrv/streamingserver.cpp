/*
 * streamingplayer.cpp
 *
 *  Created on: Jul 5, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "streamingserver.hh"
#include "common.hh"
#include "rcvrthr.hh"
#include "ctrlthr.hh"
#include "writerthr.hh"
#include "senderthr.hh"
#include "readerthr.hh"
#include "squeue.hh"
#include "ipputils.h"

// TODO: remove these when we remove the stop gap solution
#include "URLFetcher.hh"
#include "rtpplayertx.hh"

using namespace std;

struct CStreamingServer::CStreamingServerImpl
{
    CCtrlThr *ctrl_thr;
    CRcvrThr *receiver_thr;
    CWriterThr *writer_thr;
    CReaderThr *reader_thr;
    CSenderThr *sender_thr;
    CSQueueManager * queueManager;
    CSQueueSink * qSchedulerSink;
    CSQueueSource * qSchedulerSource;
    string requestFile;
    string recoveryFile;
    int nRequestPort;
    int nNotifyPort;
    int nHTTPPort;
    bool bDoRequest;
    bool bDoRecovery;
    bool bReuseAddress;
    bool bStarted;

    bool WaitForResponse();

};

CStreamingServer::CStreamingServer(bool bLog)
{
    m_pimpl = new CStreamingServerImpl();
    m_pimpl->ctrl_thr = NULL;
    m_pimpl->receiver_thr = NULL;;
    m_pimpl->writer_thr = NULL;
    m_pimpl->reader_thr = NULL;
    m_pimpl->sender_thr = NULL;

    m_pimpl->nRequestPort = 0;
    m_pimpl->nNotifyPort = 0;
    m_pimpl->nHTTPPort = 0;
    m_pimpl->bDoRequest = false;
    m_pimpl->bDoRecovery = false;
    m_pimpl->bReuseAddress = false;
    m_pimpl->qSchedulerSink = NULL;
    m_pimpl->qSchedulerSource = NULL;
    m_pimpl->bStarted = false;

    CAppThread::DisableRealtime();


    InitIPPLib();

    InitSSSyslog(bLog); // log to std out default

    AddThreadName("Main");

    // Initialize the TimeKeeper
    CTimeKeeper::Begin();

    // Initialize the Command Library
    CCommand::Init();

    // Setup QueueManager
    m_pimpl->queueManager = CSQueueManager::Instance();

    m_pimpl->qSchedulerSink =
        CSQueueManager::Instance()->GetQueueSink("ToScheduler");
    m_pimpl->qSchedulerSource =
        CSQueueManager::Instance()->GetQueueSource("FromScheduler");

}

CStreamingServer::~CStreamingServer()
{
    if (m_pimpl->ctrl_thr != NULL)
        m_pimpl->ctrl_thr->Stop();

    if (m_pimpl->receiver_thr != NULL)
        m_pimpl->receiver_thr->Stop();
    if (m_pimpl->writer_thr != NULL)
        m_pimpl->writer_thr->Stop();
    if (m_pimpl->reader_thr != NULL)
        m_pimpl->reader_thr->Stop();
    if (m_pimpl->sender_thr != NULL)
        m_pimpl->sender_thr->Stop();

    delete(m_pimpl->receiver_thr);
    delete(m_pimpl->ctrl_thr);
    delete(m_pimpl->writer_thr);
    delete(m_pimpl->sender_thr);
    delete(m_pimpl->reader_thr);

    if (m_pimpl->qSchedulerSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_pimpl->qSchedulerSink);
    if (m_pimpl->qSchedulerSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_pimpl->qSchedulerSource);

    CSQueueManager::Instance()->Destroy(); // Releases memory


    delete m_pimpl;
}

bool CStreamingServer::Start()
{
    m_pimpl->ctrl_thr = new CCtrlThr(
            m_pimpl->nRequestPort,
            m_pimpl->nNotifyPort,
            m_pimpl->nHTTPPort,
            m_pimpl->requestFile.c_str(),
            m_pimpl->recoveryFile.c_str());

    m_pimpl->ctrl_thr->SetFileModes(m_pimpl->bDoRequest, m_pimpl->bDoRecovery);
    m_pimpl->ctrl_thr->SetReuseAddress(m_pimpl->bReuseAddress);

    if (m_pimpl->ctrl_thr->Init() == -1) {
        SYSLOG_INFO("Controller Thread may not have initialized fully.");
    }

    m_pimpl->receiver_thr = new CRcvrThr();
    m_pimpl->writer_thr = new CWriterThr(".");
    m_pimpl->reader_thr = new CReaderThr(".");
    m_pimpl->sender_thr = new CSenderThr();

    m_pimpl->ctrl_thr->Start();
    m_pimpl->receiver_thr->Start();
    m_pimpl->writer_thr->Start();
    m_pimpl->reader_thr->Start();
    m_pimpl->sender_thr->Start();

    SYSLOG_INFO("Started Streaming Player Object.\n");
    m_pimpl->bStarted = true;

    return true;
}

bool CStreamingServer::IsRunning(){
    if (m_pimpl->ctrl_thr == NULL)
        return false;
    return m_pimpl->ctrl_thr->isRunning();
}

void CStreamingServer::SetRequestPort(int port)
{
    m_pimpl->nRequestPort = port;
    if (m_pimpl->bStarted)
        m_pimpl->ctrl_thr->SetRequestPort(port);
}

void CStreamingServer::SetHTTPPort(int port)
{
    m_pimpl->nHTTPPort = port;
    if (m_pimpl->bStarted)
        m_pimpl->ctrl_thr->SetHTTPPort(port);
}

void CStreamingServer::SetNotifyPort(int port)
{
    m_pimpl->nNotifyPort = port;
    if (m_pimpl->bStarted)
        m_pimpl->ctrl_thr->SetNotifyPort(port);
}

void CStreamingServer::SetRequestFile(const string & a_fileName)
{
    if (a_fileName.length() == 0)
        return;
    m_pimpl->bDoRequest = true;
    m_pimpl->requestFile = a_fileName;
}

void CStreamingServer::SetRecoveryFile(const string & a_fileName)
{
    if (a_fileName.length() == 0)
        return;
    m_pimpl->bDoRecovery = true;
    m_pimpl->recoveryFile = a_fileName;
}

bool CStreamingServer::ExecuteCommand(const string & a_command)
{
    if (!m_pimpl->ctrl_thr->isRunning())
        return false;

    // Send command to scheduler queue
    string command (a_command);
    command.append(" schedid=123");
    CSFrame * sFrame = new CSFrame(command.c_str(), command.length());
    m_pimpl->qSchedulerSink->InsertFrame(sFrame);

    // Wait for response
    return m_pimpl->WaitForResponse();
}

bool CStreamingServer::CStreamingServerImpl::WaitForResponse()
{
    bool retval = false;
    int count = 0;
    while (count++ < 1000) {
        CSFrame *sFrame = qSchedulerSource->GetFrame();
        if (sFrame != NULL) {
            char * commandStr = sFrame->GetData();
            bool success;
            if (CCtrlThr::MatchSchedulerResponse(123, commandStr, success)) {
                retval = success;
                qSchedulerSource->ReleaseFrame(sFrame);
                break;
            }
            qSchedulerSource->ReleaseFrame(sFrame);
        }
        usleep(1000);
    }
    return retval;
}



bool CStreamingServer::RetrieveSDPFile(const std::string & a_sdpURL, const std::string & a_savePath,
        int & a_videoPort, int & a_audioPort, std::string & a_ipAddress)
{
    bool retval = true;
    a_videoPort = 0;
    a_audioPort = 0;
    a_ipAddress = "0.0.0.0";

    CURLFetcher *fetcher = new CURLFetcher(a_sdpURL.c_str());
    if (!fetcher->FetchToFile(a_savePath.c_str())) { // may block
        retval = false;
    }
    else {
        retval = CRtpPlayerTx::ParseSDPFile(a_savePath, a_videoPort, a_audioPort, a_ipAddress);
    }
    delete fetcher;

    return retval;
}
