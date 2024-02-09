#include "strmsrv_compatibility.h"

#include <sys/stat.h>
#include "ctrlthr.hh"
#include "cpustats.hh"
#include "common.hh"
#include "v2d.h"
#include "v2o_lib.h"

using namespace std;

#define NOTIFICATION_TAG "SSNotificationStream"


/// This is the thread code
/// Constructor for the Controller thread
CCtrlThr::CCtrlThr (int nRequestPort, int nNotifyPort, int nHttpPort,
                    const char *requestDevice, const char *recoveryDevice)
{
    m_nFilePosition = 0;
    m_pRequestString = new char [MAXREQSIZE + 1];
    m_nSocket = 0;
    m_nRequestPort = nRequestPort;
    m_nNotifySocket = 0;
    m_nNotifyPort = nNotifyPort;
    m_nHTTPSocket = 0;
    m_nHTTPPort = nHttpPort;
    m_nCmdId = 0;
    m_bHup = false;
    m_bProcesedRequestFile = false;
    m_bReuseAddress = true;
    m_bRequestPortUpdated = false;
    m_bNotifyPortUpdated = false;
    m_bHTTPPortUpdated = false;

    InitializeSockets();

    CreateQueues();

    InitializeFiles(requestDevice, recoveryDevice);

    m_pCpuStats = new CCpuStats();

    if (m_pThreadName != NULL)
        strcpy(m_pThreadName, "Controller");

}

/// Destructor for the Receiver thread
CCtrlThr::~CCtrlThr()
{
    SYSLOG_TRACE("Destructing %s:%s\n", __FILE__, __FUNCTION__);
    delete[] m_pRecoveryFile;
    delete[] m_pRequestFile;
    delete[] m_pRequestString;

    ClearRequestList();
    ClearHTTPList();
    ClearSchedulerList();
    ClearNotificationList();

    CloseSocket(m_nSocket);
    CloseSocket(m_nNotifySocket);
    CloseSocket(m_nHTTPSocket);

    CSQueueManager::Instance()->ReleaseQueueSink(m_qReaderSink);
    CSQueueManager::Instance()->ReleaseQueueSink(m_qReceiverSink);
    CSQueueManager::Instance()->ReleaseQueueSink(m_qSenderSink);
    CSQueueManager::Instance()->ReleaseQueueSink(m_qWriterSink);
    CSQueueManager::Instance()->ReleaseQueueSource(m_qControllerSource);
    CSQueueManager::Instance()->ReleaseQueueSource(m_qSchedulerSource);
    CSQueueManager::Instance()->ReleaseQueueSink(m_qSchedulerSink);

    delete m_pCpuStats;
}

void CCtrlThr::ClearRequestList()
{
    RQ::iterator iter;
    CCommand *pCmd = NULL;
    if (!m_rList.empty()) {
        for (iter = m_rList.begin();
                iter != m_rList.end(); ++iter) {
            ASSERT((*iter).first > 0);
            CloseSocket((*iter).first);
            pCmd = (*iter).second;
            ASSERT(pCmd != NULL);
            delete (pCmd);
        }
        m_rList.clear();
    }

}

void CCtrlThr::ClearHTTPList()
{
    RQ::iterator iter;
    CCommand *pCmd = NULL;
    if (!m_rHTTPList.empty()) {
        for (iter = m_rHTTPList.begin();
                iter != m_rHTTPList.end(); ++iter) {
            ASSERT((*iter).first > 0);
            CloseSocket((*iter).first);
            pCmd = (*iter).second;
            ASSERT(pCmd != NULL);
            delete (pCmd);
        }
        m_rHTTPList.clear();
    }
}
void CCtrlThr::ClearSchedulerList()
{
    RQ::iterator iter;
    CCommand *pCmd = NULL;
    if (!m_rSchedulerList.empty()) {
        for (iter = m_rSchedulerList.begin();
                iter != m_rSchedulerList.end(); ++iter) {
            ASSERT((*iter).first > 0);
            pCmd = (*iter).second;
            ASSERT(pCmd != NULL);
            delete (pCmd);
        }
        m_rSchedulerList.clear();
    }
}

void CCtrlThr::ClearNotificationList()
{
    if (!m_rNotificationList.empty()) {
        map<int, CNotify*>::iterator it;
        for (it = m_rNotificationList.begin();
                it != m_rNotificationList.end(); ++it) {
            ASSERT((*it).first > 0);
            int sockfd = (*it).first;
            if (sockfd > 0) {
                send(sockfd, "</"NOTIFICATION_TAG">\r\n",
                        strlen("</"NOTIFICATION_TAG">\r\n"), MSG_DONTWAIT);
                CloseSocket(sockfd);
            }
            CNotify * pNotify = (*it).second;
            delete (pNotify);
        }
        m_rNotificationList.clear();
    }
}

void CCtrlThr::CreateQueues()
{
    m_qControllerSource =
        CSQueueManager::Instance()->GetQueueSource("Controller");
    m_qSchedulerSource =
        CSQueueManager::Instance()->GetQueueSource("ToScheduler");
    m_qSchedulerSink =
        CSQueueManager::Instance()->GetQueueSink("FromScheduler");
    m_qReceiverSink = CSQueueManager::Instance()->GetQueueSink("Receiver");
    m_qSenderSink = CSQueueManager::Instance()->GetQueueSink("Sender");
    m_qReaderSink = CSQueueManager::Instance()->GetQueueSink("Reader");
    m_qWriterSink = CSQueueManager::Instance()->GetQueueSink("Writer");

    m_qControllerSource->SetMaxBytes(20 * 1024 * 1024);

}

void CCtrlThr::InitializeFiles(const char *requestDevice, const char *recoveryDevice)
{
    m_pRequestFile = new char [MAXDEVICESIZE +1 ];
    if (requestDevice == NULL || strlen(requestDevice) == 0)
        strcpy (m_pRequestFile, REQUESTDEVICE);
    else
        strcpy (m_pRequestFile, requestDevice);

    m_pRecoveryFile = new char [MAXDEVICESIZE +1 ];
    if (recoveryDevice == NULL || strlen(recoveryDevice) == 0)
        strcpy (m_pRecoveryFile, RECOVERYDEVICE);
    else
        strcpy (m_pRecoveryFile, recoveryDevice);

    m_bDoRecovery = true;
    m_bDoRequest = true;
}

/// Init method
int CCtrlThr::Init()
{
    int rc = 0;
    /* Copy previous recovery file onto the current request file
       for auto replay of commands issued during previous
       session */
    if (m_bDoRecovery && m_bDoRequest) {
        rename(m_pRecoveryFile, m_pRequestFile);
    }

    if (m_nRequestPort > 0) {
        m_nSocket = CreateTCPListener(m_nRequestPort, m_bReuseAddress);
        if (m_nSocket == -1)
            rc = -1;
        else
            SYSLOG_INFO("Listening on port %d for SS commands", m_nRequestPort);
    }

    if (m_nNotifyPort > 0) {
        m_nNotifySocket = CreateTCPListener(m_nNotifyPort, m_bReuseAddress);
        if (m_nNotifySocket == -1)
            rc = -1;
        else
            SYSLOG_INFO("Listening on port %d for Notifications", m_nNotifyPort);
    }

    if (m_nHTTPPort > 0) {
        m_nHTTPSocket = CreateTCPListener(m_nHTTPPort, m_bReuseAddress);
        if (m_nHTTPSocket == -1)
            rc = -1;
        else
            SYSLOG_INFO("Listening on port %d for V2D HTTP connections", m_nHTTPPort);
    }


    return rc;
}

// Creates a TCP listener on given port. Returns -1 on failure or
// the created socket FD on success
int CCtrlThr::CreateTCPListener(int port, bool bReuseAddress)
{
    int sockFD;

    if (port <= 0)
        return -1;

    if ((sockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        SYSLOG_ERROR("Error creating socket");
        return -1;
    }

    if (bReuseAddress) {
        int sockoptval = 1;
        if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR,
                SOCKOPTCAST &sockoptval, sizeof (sockoptval)) < 0) {
            SYSLOG_ERROR("Error setting socket options.");
            CloseSocket(sockFD);
            return -1;
        }
    }

    // Bind to port
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    memset(sa.sin_zero, 0, sizeof (sa.sin_zero));
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockFD, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        SYSLOG_ERROR("Cannot bind to port %d", port);
        CloseSocket(sockFD);
        return -1;
    }

    // Start Listening
    if (listen (sockFD, 5) == -1) {
        SYSLOG_ERROR("Listen system call failed.");
        CloseSocket(sockFD);
        return -1;
    }

    return sockFD;
}

bool CCtrlThr::CreateListenerPort(int & a_nSocket, int a_nPort) {
    CloseSocket(a_nSocket);
    a_nSocket = -1;
    if (a_nPort <= 0)
        return true;
    a_nSocket = CreateTCPListener(a_nPort, m_bReuseAddress);
    if (a_nSocket == -1)
        return false;
    SYSLOG_INFO("Listening on port %d", a_nPort);
    return true;
}

void CCtrlThr::SetReuseAddress(bool bValue)
{
    m_bReuseAddress = bValue;
}

void CCtrlThr::SetFileModes(bool bRequest, bool bRecovery)
{
    m_bDoRequest = bRequest;
    m_bDoRecovery = bRecovery;
}

void CCtrlThr::SetRequestPort(int port)
{
    m_nRequestPort = port;
    m_bRequestPortUpdated = true;
}

void CCtrlThr::SetHTTPPort(int port)
{
    m_nHTTPPort = port;
    m_bHTTPPortUpdated = true;
}

void CCtrlThr::SetNotifyPort(int port)
{
    m_nNotifyPort = port;
    m_bNotifyPortUpdated = true;
}

/// Main routine for the controller thread
/// Calls ProcessFileRequests
int CCtrlThr::Run()
{
    timestamp_t prev, now, delta;

    SYSLOG_INFO("Starting %s Thread\n", m_pThreadName);
    prev = CTimeKeeper::Now();
    while (!m_bExit) {
        now = CTimeKeeper::Now();
        delta = now - prev;
        int retval = 0;
        retval += ProcessFileRequests();
        retval += ProcessHTTPRequests();
        retval += ProcessSocketRequests();
        retval += ProcessModuleRequests();
        retval += ProcessSchedulerRequests();
        retval += ProcessSchedulerResponse();
        retval += ProcessNotificationMessages();

        if (m_bHup == true) {
            InitSSSyslog(false);
            // Reinitialise V2O logging library
#ifdef HAVE_V2O
            vlog_reinit();
#endif
            m_bHup = false;
        }

        if (delta > 250) {
            // Request QueueManger to dump stats
#ifdef SSDEBUG
            SYSLOG_DEBUG("Total Contention = %lu", CMutex::m_totalContention);
            DumpSSSyslog();
            CSQueueManager::Instance()->PrintStatistics();
#endif
            //            ComputeCpuUsage();
            prev = now;
        }

        if (retval <= 0) {
            LightSleep();
        }

    }
    SYSLOG_INFO("Exiting the %s Thread\n", m_pThreadName);
    return 0;
}

/// Processes one command from the administrator
int CCtrlThr::ProcessOneCommand(const char *pCmdString, CCommand *pCmd)
{
    //CSFrame *sFrame = NULL;

    ASSERT(pCmd != NULL);

    if (pCmd->Initialized()) {
        /* We are already processing a command from this client (may
         * even have been processed) */
        if (pCmd->Processed())
            SYSLOG_DEBUG("Ignoring command (already served) %s\n", pCmdString);
        else
            SYSLOG_DEBUG("Ignoring command (processing) %s\n", pCmdString);
        return -1;
    }

    /* Initialize the command object */
    if (pCmd->Init(pCmdString) < 0) {
        pCmd->SetResult(CMDFAILURE);
        return -1;
    }

    LogCommandToRecoveryFile(pCmdString, pCmd);

    /* Log to syslog only important commands */
    switch (pCmd->GetOperation()) {
    case OP_UNKNOWN:
    case OP_TRACE:
    case OP_RESET:
    case OP_XLIST:
    case OP_XQLIST:
    case OP_XGLOBAL:
    case OP_XHELP:
        break;
    default:
        SYSLOG_INFO("%s\n", pCmdString);
        break;
    }

    int nCmdLen = strlen(pCmdString);
    switch (pCmd->GetInputModule()) {

    case MOD_READER:
        m_qReaderSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();
        break;

    case MOD_RECEIVER:
        m_qReceiverSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();
        break;

    }

    switch (pCmd->GetOutputModule()) {

    case MOD_WRITER:
        m_qWriterSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();
        break;

    case MOD_SENDER:
        m_qSenderSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();
        break;

    }

    switch (pCmd->GetOperation()) {

    case OP_QUIT:
        pCmd->SetResult(CMDSUCCESS);
        m_bExit = 1;
        break;

    case OP_XGLOBAL:
        SendGlobalStatus(pCmd);
        pCmd->SetResult(CMDSUCCESS);
        pCmd->GetResponse();
        break;

    case OP_LIST:
    case OP_RESET:
        m_qSenderSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();

        m_qReceiverSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();

        m_qReaderSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();

        m_qWriterSink->InsertFrame(pCmdString, nCmdLen);
        pCmd->IncreaseRef();
        break;

    case OP_TRACE:
        pCmd->SetResult(CMDSUCCESS);
        break;
    }

    // If ref count is not increased by now, then the result
    // better be set
    if ((!pCmd->GetRef()) && !pCmd->GetResult())
        pCmd->SetResult(CMDFAILURE);

    return 0;
}

void CCtrlThr::InitializeSockets()
{
#ifdef WINDOWS
    // Windows sockets need a startup method
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
}

void CCtrlThr::CloseSocket(int a_sockfd)
{
    if (a_sockfd <= 0)
        return;
#ifdef WINDOWS
    closesocket(a_sockfd);
#else
    close(a_sockfd);
#endif
}

void CCtrlThr::LogCommandToRecoveryFile(const char *pCmdString, CCommand *pCmd)
{
    if (!m_bDoRecovery)
        return;

    switch (pCmd->GetOperation()) {
    case OP_UNKNOWN:
    case OP_TRACE:
    case OP_LIST:
    case OP_STATUS:
    case OP_QUIT:
    case OP_XLIST:
    case OP_XQLIST:
    case OP_XSTATUS:
    case OP_XGLOBAL:
    case OP_XHELP:
        // These commands do not log
        return;
    default:
        // All others do
        break;
    }

    // Commands with socket descriptors should be discarded
    int  nSockfd;
    if (pCmd->GetIntVar("sockfd", &nSockfd) != -1)
        return;

    // Log the command
    FILE *fpLog = fopen(m_pRecoveryFile, "a");
    if (fpLog == NULL)
        return;
    fprintf (fpLog, "%s", pCmdString);
    fclose(fpLog);

    // When a delete command comes in, trim recovery file
    // to remove all older related commands
    if (pCmd->GetOperation() == OP_DEL) {
        TrimRecoveryFile(pCmd->GetId(), false);
    }
    else if (pCmd->GetOperation() == OP_RESET) {
        TrimRecoveryFile(pCmd->GetId(), true);
    }

}

/// Processes messages from the modules/threads
int CCtrlThr::ProcessModuleRequests()
{
    int retval = 0;
    CSFrame *sFrame;
    char        *pCmdString, *pTmpString, *pTmpString2;
    int         nId, nResult, nStatus, nCmdId;
    timestamp_t nTimecode;
    CCommand *pCmd = NULL;
    RQ::iterator iter;

    // Check if amy messages have come in
    sFrame = m_qControllerSource->GetFrame();
    if (sFrame == NULL)
        goto EXIT_LABEL;

    pCmdString = sFrame->GetData();
    if (strncmp (pCmdString, NOTIFYMSG, (sizeof(NOTIFYMSG) - 1)) == 0) {
        // Cannot send to syslog since it is xml
    }
    // TODO: not needed
    // else if (strncmp (pCmdString, ERRORMSG, (sizeof(ERRORMSG) - 1)) == 0) {
    // Cannot send to syslog since it is xml
    // }
    else if (strncmp (pCmdString, STATUSMSG, (sizeof(STATUSMSG) - 1)) == 0) {
        SYSLOG_DEBUG ("%s\n", pCmdString);
    }
    else {
        SYSLOG_DEBUG ("%s\n", pCmdString);
    }


    retval++;
    if (sscanf(pCmdString, "RESPONSE i:%9d r:%9d s:%9d t:%"PRIu64" cid:%d",
                &nId, &nResult, &nStatus, &nTimecode, &nCmdId) >= 2) {
        pTmpString = strstr (pCmdString, "result");
        ASSERT(strlen(pTmpString) > strlen("result:\""));
        pTmpString += strlen("result:\"");
        pTmpString2 = strrchr(pTmpString, '\"');
        if (pTmpString2) {
            *pTmpString2 = '\0';
        }


        // search in m_rList
        for (iter = m_rList.begin();
                iter != m_rList.end(); ++iter) {
            pCmd = (*iter).second;
            if (!pCmd->Initialized()) continue;
            if (pCmd->GetCmdId() == nCmdId) {
                pCmd->DecreaseRef();
                pCmd->SetResult(nResult);
                pCmd->SetStatus(nStatus);
                pCmd->SetTimecode(nTimecode);

                (pCmd->GetResultString()).append(pTmpString);
                SYSLOG_TRACE("Command %d took %d ms to process\n",
                             pCmd->GetCmdId(), pCmd->GetElapsed());
                goto EXIT_LABEL;
            }
        }

        // search in m_rSchedulerList
        for (iter = m_rSchedulerList.begin();
                iter != m_rSchedulerList.end(); ++iter) {
            pCmd = (*iter).second;
            if (!pCmd->Initialized()) {
                continue;
            }
            if (pCmd->GetCmdId() == nCmdId) {
                pCmd->DecreaseRef();
                pCmd->SetResult(nResult);
                pCmd->SetStatus(nStatus);
                pCmd->SetTimecode(nTimecode);

                (pCmd->GetResultString()).append(pTmpString);
                SYSLOG_TRACE("Scheduler Command %d took %d ms to process\n",
                             pCmd->GetCmdId(), pCmd->GetElapsed());
                goto EXIT_LABEL;
            }
        }

        // search in m_rHTTPList
        for (iter = m_rHTTPList.begin();
                iter != m_rHTTPList.end(); ++iter) {
            pCmd = (*iter).second;
            if (!pCmd->Initialized()) continue;
            if (pCmd->GetCmdId() == nCmdId) {
                pCmd->DecreaseRef();
                pCmd->SetResult(nResult);
                pCmd->SetStatus(nStatus);
                pCmd->SetTimecode(nTimecode);

                (pCmd->GetResultString()).append(pTmpString);
                SYSLOG_TRACE("Command %d took %d ms to process\n",
                             pCmd->GetCmdId(), pCmd->GetElapsed());
                goto EXIT_LABEL;
            }
        }


        SYSLOG_DEBUG("Could not find command %u\n", nCmdId);
    }
    // For notify messages, create socket to send the notifications
    else if (strncmp(pCmdString, NOTIFYMSG, (sizeof(NOTIFYMSG) - 1)) == 0) {

        /* Add the notification to the current active notification clients */
        if (!m_rNotificationList.empty()) {
            map<int, CNotify*>::iterator rIter;
            for (rIter = m_rNotificationList.begin();
                    rIter != m_rNotificationList.end(); ++rIter) {
                ASSERT((*rIter).first > 0);
                CNotify *pNotify = (*rIter).second;
                // Any older notification not yet sent will be dropped
                if (pNotify != NULL) {
                    SYSLOG_INFO("Dropping Older Notification");
                    delete(pNotify);
                }
                (*rIter).second = new CNotify(pCmdString + sizeof(NOTIFYMSG));

            }
        }
    }
    else {
        SYSLOG_TRACE("Could not parse %s\n", pCmdString);
    }


EXIT_LABEL:
    if (sFrame != NULL)
        m_qControllerSource->ReleaseFrame(sFrame);

    return retval;
}

/// Processes commands from the administrator
int CCtrlThr::ProcessSocketRequests()
{
    struct sockaddr_in sPeer;
    socklen_t nPeerlen = sizeof(sPeer);
    fd_set read_fdset, write_fdset;
    int nDescriptors, nMaxfd = -1;
    int nRequestSock = 0;
    struct timeval timeout;
    int retval = 0;
    CCommand *pCmd = NULL;
    char *cp;
    RQ::iterator rIter;

    if (m_bRequestPortUpdated) {
        ClearRequestList();
        CreateListenerPort(m_nSocket, m_nRequestPort);
        m_bRequestPortUpdated = false;
    }
    if (m_nSocket <= 0)
        goto EXIT_LABEL;

    /* Get ready for select */
    FD_ZERO(&read_fdset);
    FD_ZERO(&write_fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    /* Add main socket to read fdset */
    FD_SET ((unsigned int) m_nSocket, &read_fdset);
    if (nMaxfd <= m_nSocket)
        nMaxfd = m_nSocket + 1;

    /* Add client sockets to read and write fdsets. All clients
       get added to read fdset. If any client has a result pending to
       be sent out then it is added to the write fdset */
    if (!m_rList.empty()) {
        for (rIter = m_rList.begin();
                rIter != m_rList.end(); ++rIter) {
            nRequestSock = (*rIter).first;

            pCmd = (*rIter).second;
            ASSERT(nRequestSock > 0);
            if (pCmd != NULL) {
                if (pCmd->GetResult()) {
                    FD_SET ((unsigned int) nRequestSock, &write_fdset);
                }
            }
            FD_SET ((unsigned int) nRequestSock, &read_fdset);
            if (nMaxfd <= nRequestSock)
                nMaxfd = nRequestSock + 1;
        }
    }

    /* Call select */
    nDescriptors = select(nMaxfd + 1, &read_fdset, &write_fdset,
                          NULL, &timeout);
    if (nDescriptors <= 0) {
        //        goto EXIT_LABEL; // nothing to do
    }

    /* If activity on main socket, add the new client into our list of
     * active clients */
    if (FD_ISSET(m_nSocket, &read_fdset)) {
        nRequestSock = accept (m_nSocket,
                               (struct sockaddr *) & sPeer, &nPeerlen);
        if (nRequestSock > 0) {
            SYSLOG_DEBUG("Received Connection from %s on socket %d\n",
                         inet_ntoa (sPeer.sin_addr),
                         nRequestSock);
            /* create command object for this client */
            pCmd = new CCommand();
            if (pCmd != NULL) {
                m_rList.insert(RQEntry(nRequestSock, pCmd));
            }
            else { // no memory to do anything so close connection
                CloseSocket(nRequestSock);
            }

            //            m_rList.insert(RQEntry(nRequestSock, NULL));
        }

        retval++;
    }

    /* Deal with active client sockets */
    if (!m_rList.empty()) {
        for (rIter = m_rList.begin();
                rIter != m_rList.end(); ) {
            nRequestSock = (*rIter).first;
            pCmd = (*rIter).second;
            ASSERT(pCmd != NULL);
            ASSERT(nRequestSock > 0);

            /* Send out result to any client that has a result pending.
               Close the socket after sending the reply */
            if (pCmd->Initialized() && !pCmd->Processed()) {
                if (pCmd->GetResult() && (!pCmd->GetRef()) &&
                        (FD_ISSET(nRequestSock, &write_fdset))) {

                    char *pResponseString =
                        new char [(pCmd->GetResultString()).length() + 256];
                    int responselen =
                        sprintf (pResponseString,
                                 "%d OK s:%d t:%"PRIu64" i:%d result:\"%s\"\n",
                                 pCmd->GetResult(),
                                 pCmd->GetStatus(),
                                 pCmd->GetTimecode(),
                                 pCmd->GetCmdId(),
                                 pCmd->GetResultString().c_str());

                    send (nRequestSock,
                          pResponseString,
                          responselen, 0);
                    delete[] pResponseString;
                    SYSLOG_DEBUG("Sent response.\n");
                    pCmd->SetProcessed();

                    continue;
                }
            }
            else if (pCmd->Initialized() && pCmd->Processed()) {
                // Client has been responded to. We give a chance for the
                // client to disconnect first
                if (pCmd->GetElapsed() > 250) { // 250 ms timeout
                    SYSLOG_DEBUG("Client %d processed and timed out.\n", nRequestSock);
                    CloseSocket(nRequestSock);
                    (*rIter).second = NULL;
                    m_rList.erase(rIter++);
                    delete(pCmd);
                    continue;
                }
            }
            else {
                // Still waiting for this client to send a message
                if (pCmd->GetElapsed() > 10000) { // 10 second timeout
                    SYSLOG_DEBUG("Client Timed Out.\n");
                    CloseSocket(nRequestSock);
                    (*rIter).second = NULL;
                    m_rList.erase(rIter++);
                    delete(pCmd);
                    continue;
                }
            }

            /* Get message from a connected client. If
               recv() succeeds, grab the message and process
               it. Else assume client disconneted, so
               cleanup the socket */
            if (FD_ISSET(nRequestSock, &read_fdset)) {
                memset(m_pRequestString, 0, MAXREQSIZE + 1);
                if (recv(nRequestSock, m_pRequestString,
                                MAXREQSIZE - 32, 0) > 0) {
                    SYSLOG_DEBUG("Received Control Request: %s\n",
                                 m_pRequestString);
                    // terminate received command at first new line
                    if ((cp = strchr(m_pRequestString, '\n')) != NULL) {
                        *cp = '\0';
                    }
                    else if ((cp = strchr(m_pRequestString, '\r')) != NULL) {
                        *cp = '\0';
                    }
                    // append comamnd id to the received string
                    m_nCmdId++;
                    if (m_nCmdId > 100000000)
                        m_nCmdId = 0;
                    char buff[32];
                    sprintf (buff, " cmdid=%d \n", m_nCmdId);
                    strcat(m_pRequestString, buff);

                    // process the command
                    ProcessOneCommand(m_pRequestString, pCmd);
                    retval++;
                    break;
                }
                else {
                    // Remote closed connection so we close
                    SYSLOG_DEBUG("Closing sock=%d cmdid=%d\n",
                                 nRequestSock, pCmd->GetCmdId());
                    CloseSocket(nRequestSock);
                    delete(pCmd);
                    (*rIter).second = NULL;
                    m_rList.erase(rIter++);
                    break;
                }
            }

            ++rIter;
        }
        //        SYSLOG_TRACE("Dealt with some clients at time %"PRIu64"\n", CTimeKeeper::Now());
    }

EXIT_LABEL:
    return retval;
}


/// Processes commands from the request file
int CCtrlThr::ProcessFileRequests()
{
    FILE *fpReq = NULL;
    if (!m_bDoRequest)x
        return 0;
    
    // Create temporary file so others know we're ready
    if (!m_bProcesedRequestFile) {
        FILE *fp = fopen("/tmp/ssready", "w");
        if (fp == NULL)
            SYSLOG_INFO("Failed to open /tmp/ssready\n");
        else 
            fclose(fp);
    }

#ifndef WINDOWS
    struct stat fileStat;
    if (stat("/tmp/ssstop", &fileStat) == 0) {
        unlink("/tmp/ssstop");
        m_bExit = 1;
        goto EXIT_LABEL;
    }

    if (stat ((char *)m_pRequestFile, &fileStat)) {
        //LOG_ERROR ("File not present\n");
        goto EXIT_LABEL;
    }

    if ((m_nFilePosition != 0)  &&
            (fileStat.st_mtime < fileStat.st_atime)) {
        goto EXIT_LABEL;
    }
#endif

#ifdef WINDOWS
    // On windows, we read the input file only once
    if (m_bProcesedRequestFile) {
        return 0;
    }
#endif
    m_bProcesedRequestFile = true;

    fpReq = fopen (m_pRequestFile, "r");
    if (fpReq == NULL) {
        SYSLOG_ERROR ("Could not open file %s\n", m_pRequestFile);
        goto EXIT_LABEL;
    }
    if (fseek(fpReq, m_nFilePosition, SEEK_SET)) {
        SYSLOG_ERROR ("Could not seek file %d\n", m_nFilePosition);
        goto EXIT_LABEL;
    }
    while (fgets(m_pRequestString, MAXREQSIZE, fpReq)) {
        CCommand *pCmd = new CCommand();
        if (pCmd != NULL) {
            ASSERT(!pCmd->Initialized());
            ProcessOneCommand(m_pRequestString, pCmd);
            delete(pCmd);
            pCmd = NULL;
        }
    }
    m_nFilePosition = ftell(fpReq);

EXIT_LABEL:
    if (fpReq)
        fclose(fpReq);
    return 0;
}
/// Processes HTTP requests from the users/clients
int CCtrlThr::ProcessHTTPRequests()
{
    struct sockaddr_in sPeer;
    socklen_t nPeerlen = sizeof(sPeer);
    fd_set read_fdset, write_fdset;
    int nDescriptors, nMaxfd = -1;
    int nRequestSock = 0;
    struct timeval timeout;
    int retval = 0;
    CCommand *pCmd = NULL;
    char *cp;
    RQ::iterator rIter;

    if (m_bHTTPPortUpdated) {
        ClearHTTPList();
        CreateListenerPort(m_nHTTPSocket, m_nHTTPPort);
        m_bHTTPPortUpdated = false;
    }

    if (m_nHTTPSocket <= 0)
        goto EXIT_LABEL;

    /* Get ready for select */
    FD_ZERO(&read_fdset);
    FD_ZERO(&write_fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;


    /* Add main socket to read fdset */
    FD_SET((unsigned int) m_nHTTPSocket, &read_fdset);
    if (nMaxfd <= m_nHTTPSocket)
        nMaxfd = m_nHTTPSocket + 1;


    /* Add client sockets to read and write fdsets. All clients
       get added to read fdset. If any client has a result pending to
       be sent out then it is added to the write fdset */
    if (!m_rHTTPList.empty()) {
        for (rIter = m_rHTTPList.begin();
                rIter != m_rHTTPList.end(); ++rIter) {
            nRequestSock = (*rIter).first;

            pCmd = (*rIter).second;
            ASSERT(nRequestSock > 0);
            if (pCmd != NULL) {
                if (pCmd->GetResult() == CMDFAILURE) {
                    FD_SET((unsigned int) nRequestSock, &write_fdset);
                    FD_SET((unsigned int) nRequestSock, &read_fdset);
                }
                if (!pCmd->Initialized()) {
                    FD_SET((unsigned int) nRequestSock, &read_fdset);
                }
            }
            else {
                FD_SET((unsigned int) nRequestSock, &read_fdset);
            }
            if (nMaxfd <= nRequestSock)
                nMaxfd = nRequestSock + 1;
        }
    }

    /* Call select */
    nDescriptors = select(nMaxfd + 1, &read_fdset, &write_fdset,
                          NULL, &timeout);
    if (nDescriptors <= 0) {
        //        goto EXIT_LABEL; // nothing to do
    }

    /* If activity on main socket, add the new client into our list of
     * active clients */
    if (FD_ISSET(m_nHTTPSocket, &read_fdset)) {
        nRequestSock = accept(m_nHTTPSocket,
                              (struct sockaddr *) &sPeer, &nPeerlen);
        if (nRequestSock > 0) {
            SYSLOG_DEBUG("Received Connection from %s on socket %d\n",
                         inet_ntoa (sPeer.sin_addr),
                         nRequestSock);
            /* create command object for this client */
            pCmd = new CCommand();
            if (pCmd != NULL) {
                m_rHTTPList.insert(RQEntry(nRequestSock, pCmd));
            }
            else { // no memory to do anything so close connection
                CloseSocket(nRequestSock);
            }

            //            m_rList.insert(RQEntry(nRequestSock, NULL));
        }

        retval++;
    }

    /* Deal with active client sockets */
    if (!m_rHTTPList.empty()) {
        for (rIter = m_rHTTPList.begin();
                rIter != m_rHTTPList.end();) {
            nRequestSock = (*rIter).first;
            pCmd = (*rIter).second;
            ASSERT(nRequestSock > 0);
            ASSERT(pCmd != NULL);

            /* Send out result to any client that has a result pending.
               Close the socket after sending the reply */
            if (pCmd->Initialized() && !pCmd->Processed()) {
                if ((pCmd->GetResult() == CMDSUCCESS) && (!pCmd->GetRef())) {
                    pCmd->SetProcessed();
                    (*rIter).second = NULL;
                    m_rHTTPList.erase(rIter++);
                    delete(pCmd);
                    continue;
                }
                else if (pCmd->GetResult() && (!pCmd->GetRef()) &&
                         (FD_ISSET(nRequestSock, &write_fdset))) {
                    if (pCmd->GetResult() == CMDFAILURE) {
                        char *pResponseString =
                            new char [(pCmd->GetResultString()).length() + 256];
                        int responselen = 0;
                        const char *  ResponseFmt =
                            "HTTP/1.0 200 OK\n"
                            "Content-Type: text/html\n"
                            "Content-Length: 0\n"
                            "\r\n"
                            "<HTML>\n"
                            "No Matching objects found \n"
                            "<br>%s<br>\n"
                            "<br>%d<br>\n"
                            "</HTML>\n";

                        sprintf(pResponseString, ResponseFmt,
                                 "Error", pCmd->GetResult());
                        responselen = strlen(pResponseString);
                        send (nRequestSock, pResponseString,
                              responselen, 0);
                        delete[] pResponseString;
                        SYSLOG_DEBUG("Sent response.\n");
                        pCmd->SetProcessed();
                        continue;
                    }
                }
            }
            else if (pCmd->Initialized() && pCmd->Processed()) {
                // Client has been responded to. We give a chance for the
                // client to disconnect first
                if (pCmd->GetElapsed() > 250) { // 250 ms timeout
                    SYSLOG_DEBUG("Client %d processed and timed out.\n", nRequestSock);
                    CloseSocket(nRequestSock);
                    (*rIter).second = NULL;
                    m_rHTTPList.erase(rIter++);
                    delete(pCmd);
                    continue;
                }
            }
            else if (!pCmd->Initialized()) {
                // Still waiting for this client to send a message
                if (pCmd->GetElapsed() > 10000) { // 10 second timeout
                    SYSLOG_DEBUG("Client Timed Out.\n");
                    CloseSocket(nRequestSock);
                    (*rIter).second = NULL;
                    m_rHTTPList.erase(rIter++);
                    delete(pCmd);
                    continue;
                }

                /* Get message from a connected client. If
                   recv() succeeds, grab the message and process
                   it. Else assume client disconneted, so
                   cleanup the socket */
                if (FD_ISSET(nRequestSock, &read_fdset)) {
                    memset(m_pRequestString, 0, MAXREQSIZE + 1);
                    if (recv(nRequestSock, m_pRequestString,
                                    MAXREQSIZE - 32, 0) > 0) {
                        SYSLOG_DEBUG("Received Control Request: %s\n",
                                     m_pRequestString);
                        // terminate received command at first new line
                        if ((cp = strchr(m_pRequestString, '\n')) != NULL) {
                            *cp = '\0';
                        }
                        else if ((cp = strchr(m_pRequestString, '\r')) != NULL) {
                            *cp = '\0';
                        }
                        // append comamnd id to the received string
                        m_nCmdId++;
                        if (m_nCmdId > 100000000)
                            m_nCmdId = 0;
                        char buff[32];
                        sprintf(buff, " sockfd=%d ", nRequestSock);
                        strcat(m_pRequestString, buff);
                        sprintf(buff, " cmdid=%d \n", m_nCmdId);
                        strcat(m_pRequestString, buff);


                        // process the command
                        ProcessOneCommand(m_pRequestString, pCmd);
                        retval++;
                        break;
                    }
                    else {
                        // Remote closed connection so we close
                        SYSLOG_DEBUG("Closing sock=%d cmdid=%d\n",
                                     nRequestSock, pCmd->GetCmdId());
                        CloseSocket(nRequestSock);
                        delete(pCmd);
                        (*rIter).second = NULL;
                        m_rHTTPList.erase(rIter++);
                        break;
                    }
                }
            }

            ++rIter;
        }
        SYSLOG_TRACE("Dealt with some clients at time %"PRIu64"\n", CTimeKeeper::Now());
    }

EXIT_LABEL:
    return retval;
}

/// Send response to scheduler requests
int CCtrlThr::ProcessSchedulerResponse()
{

    if (!m_rSchedulerList.empty()) {

        RQ::iterator rIter;
        for (rIter = m_rSchedulerList.begin();
                rIter != m_rSchedulerList.end(); ++rIter) {

            CCommand *pCmd = (*rIter).second;
            ASSERT(pCmd != NULL);

            if ( pCmd->Initialized() &&
                    pCmd->GetResult() &&
                    !pCmd->GetRef() ) {

                int schedid;
                pCmd->GetIntVar("schedid", &schedid);
                char *pResponseString =
                    new char [(pCmd->GetResultString()).length() + 256];
                int responselen =
                    sprintf (pResponseString,
                             "schedid=%d %d OK s:%d t:%"PRIu64" i:%d result:\\\"%s\\\"",
                             schedid,
                             pCmd->GetResult(),
                             pCmd->GetStatus(),
                             pCmd->GetTimecode(),
                             pCmd->GetSchedCmdId(),
                             pCmd->GetResultString().c_str());

                m_qSchedulerSink->InsertFrame(pResponseString, responselen);
                (*rIter).second = NULL;
                m_rSchedulerList.erase(rIter);
                delete(pCmd);
                delete[] pResponseString;
                break;
            }
        }
    }
    return 0;
}

int CCtrlThr::ProcessNotificationMessages()
{
    struct sockaddr_in sPeer;
    socklen_t nPeerlen = sizeof(sPeer);
    fd_set read_fdset, write_fdset;
    int nMaxfd = -1;
    int nRequestSock = 0;
    struct timeval timeout;
    int retval = 0;
    CNotify *pNotify = NULL;
    map<int, CNotify*>::iterator rIter;

    if (m_bNotifyPortUpdated) {
        ClearNotificationList();
        CreateListenerPort(m_nNotifySocket, m_nNotifyPort);
        m_bNotifyPortUpdated = false;
    }

    if (m_nNotifySocket <= 0)
        goto EXIT_LABEL;

    /* Get ready for select */
    FD_ZERO(&read_fdset);
    FD_ZERO(&write_fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    /* Add main socket to read fdset */
    FD_SET((unsigned int) m_nNotifySocket, &read_fdset);
    if (nMaxfd <= m_nNotifySocket)
        nMaxfd = m_nNotifySocket + 1;

    /* Add notification client sockets to read and write fdsets. All
       clients get added to read fdset. If any client has a notification
       pending to be sent out then it is added to the write fdset */
    if (!m_rNotificationList.empty()) {
        for (rIter = m_rNotificationList.begin();
                rIter != m_rNotificationList.end(); ++rIter) {
            nRequestSock = (*rIter).first;

            pNotify = (*rIter).second;
            ASSERT(nRequestSock > 0);
            if (pNotify != NULL) {
                FD_SET((unsigned int) nRequestSock, &write_fdset);
            }
            FD_SET((unsigned int) nRequestSock, &read_fdset);
            if (nMaxfd <= nRequestSock)
                nMaxfd = nRequestSock + 1;
        }
    }

    /* Call select */
    select(nMaxfd + 1, &read_fdset, &write_fdset,
                          NULL, &timeout);

    /* If activity on main socket, add the new client into our list of
     * active clients */
    if (FD_ISSET(m_nNotifySocket, &read_fdset)) {
        nRequestSock = accept (m_nNotifySocket,
                               (struct sockaddr *) & sPeer, &nPeerlen);
        if (nRequestSock > 0) {
            SYSLOG_INFO("Received Notification Connection from %s on socket %d\n",
                        inet_ntoa (sPeer.sin_addr),
                        nRequestSock);
            // add socket to the active clients list
            m_rNotificationList[nRequestSock] =
                new CNotify("<"NOTIFICATION_TAG">");
        }

        retval++;
    }

    /* Deal with active client sockets */
    if (!m_rNotificationList.empty()) {
        for (rIter = m_rNotificationList.begin();
                rIter != m_rNotificationList.end(); ++rIter) {
            nRequestSock = (*rIter).first;
            pNotify = (*rIter).second;
            ASSERT(nRequestSock > 0);

            /* Send out notification to any client that has a
               notification pending. */
            if (pNotify != NULL) {
                if (FD_ISSET(nRequestSock, &write_fdset)) {

                    send(nRequestSock,
                         pNotify->GetMessage(),
                         pNotify->GetMessageLength(), 0);
                    (*rIter).second = NULL;
                    delete(pNotify);
                    retval++;
                    continue;

                }
            }

            /* If activity of read socket, the client probably
               disconnected. If the client sends any data, it is a
               protocol error and we disconnect. Else the client
               closed connection. Either way we cleanup.  */
            if (FD_ISSET(nRequestSock, &read_fdset)) {
                char buf[32];
                int len = recv(nRequestSock, buf, 32, 0);
                if (len > 0) {
                    buf[31] = 0;
                    SYSLOG_INFO("Notification Client sent data: Protocol Error. Closing connection.");
                }
                else {
                    SYSLOG_INFO("Notication Client closed connection...");
                }
                /* send close tag before closing socket */
                send(nRequestSock, "</"NOTIFICATION_TAG">\r\n",
                     strlen("</"NOTIFICATION_TAG">\r\n"), MSG_DONTWAIT);
                CloseSocket(nRequestSock);
                delete(pNotify);
                (*rIter).second = NULL;
                m_rNotificationList.erase(rIter); // remove client from list
                retval++;
                break;
            }
        }
    }

EXIT_LABEL:
    return retval;
}

// Returns true if response matches given scheduler id
// If return code is successful ("200 OK") then success code is set to true.
bool CCtrlThr::MatchSchedulerResponse(int a_nSchedId,
        const char * a_responseString, bool & a_bSuccess)
{
    a_bSuccess = false;
    int schedid = 0;
    char responsecode[12];
    sscanf(a_responseString, "schedid=%9d %12s ", &schedid, responsecode);
    if (schedid == a_nSchedId) {
        if (strncmp(responsecode, "200", 3) == 0)
            a_bSuccess = true;
        return true;
    }
    return false;
}

int CCtrlThr::ProcessSchedulerRequests()
{
    // Check if any requests have come in
    CSFrame *sFrame = m_qSchedulerSource->GetFrame();
    if (sFrame == NULL)
        return 0;

    // Get the request string and append our unique command id to it
    strcpy(m_pRequestString, sFrame->GetData());
    char buff[32];
    m_nCmdId++;
    if (m_nCmdId > 100000000)
        m_nCmdId = 0;
    sprintf (buff, " cmdid=%d \n", m_nCmdId);
    strcat(m_pRequestString, buff);

    // Process the command
    SYSLOG_DEBUG("Got Request on SCHEDULARFD %s\n", m_pRequestString);
    CCommand *pCmd = new CCommand();
    if (pCmd != NULL) {
        ASSERT(!pCmd->Initialized());
        char *cp = strstr(m_pRequestString, "schedid=");
        if (cp != NULL) {
            int schedid = 0;
            sscanf(cp, "schedid=%10d", &schedid);
            ASSERT(schedid > 0);
            m_rSchedulerList.insert(RQEntry(m_nCmdId, pCmd));
            ProcessOneCommand(m_pRequestString, pCmd);
        }
        else {
            SYSLOG_ERROR("Got Request on SCHEDULARFD without schedid set %s\n", m_pRequestString);
            delete (pCmd);
        }
    }

    // Release the incoming frame
    m_qSchedulerSource->ReleaseFrame(sFrame);

    return 0;
}

/// Request control thread to stop so that the application exits cleanly
void CCtrlThr::Quit()
{
    if (m_bExit != 1) {
        SYSLOG_INFO("Stopping Control thread: Quitting Application...\n");
        m_bExit = 1;
    }
}

/// Request control thread to register a hup signal
void CCtrlThr::HandleHup()
{
    SYSLOG_INFO("Received a HUP signal");
    m_bHup = true;
}


/// Sends Status of all objects to the controller thread
int CCtrlThr::SendGlobalStatus(CCommand   *pCmd)
{

    pCmd->SetRootParam("ObjectCount", 0);
    pCmd->SetRootParam("Bitrate", 0);
    pCmd->SetRootParam("DateStamp", (timestamp_t) CTimeKeeper::Now());
    pCmd->SetRootParam("Uptime", (unsigned long) CTimeKeeper::Uptime());

    pCmd->SetCurrentObject(1);
    pCmd->SetObjectParam("Type", "CPUStats");
    if (m_pCpuStats != NULL)
        m_pCpuStats->UpdateCustomStatus(pCmd);

    pCmd->SetCurrentObject(2);
    pCmd->SetObjectParam("Type", "MemStats");
    pCmd->SetObjectParam("SBufferCount", CSBuffer::BufferCount());
    pCmd->SetObjectByterateGuageParam("SBufferSize", CSBuffer::BufferSize());
    pCmd->SetObjectErrorCounterParam("SBuffersUnavailable", CSBuffer::BuffersUnavailableCount());


    return 0;
}

// Trim recovery file to remove all commands that reference the
// given id
void CCtrlThr::TrimRecoveryFile(int a_id, bool a_bFullReset)
{

    char linebuffer[MAXREQSIZE];

    FILE *fpLog = NULL;
    FILE *fpLogTmp = NULL;
    fpLog = fopen (m_pRecoveryFile, "r");
    if (fpLog == NULL)
        return;

    while (fgets(linebuffer, MAXREQSIZE, fpLog)) {
        if (fpLogTmp == NULL) {
            fpLogTmp = fopen ("/home/onair/recovery.tmp", "w");
            if (fpLogTmp == NULL) {
                fclose(fpLog);
                return;
            }
        }

        bool deleteline = false;
        if (a_bFullReset) {
            deleteline = true;
        }
        else {
            CCommand *pCmd = new CCommand();
            if (pCmd == NULL)
                break;

            if (pCmd->Init(linebuffer) < 0) {
                delete (pCmd);
                break;
            }

            else {
                if ((pCmd->GetOperation() == OP_SYNCSTART) ||
                        (pCmd->GetOperation() == OP_SYNCSTOP)) {
                    intlist pSyncIdList;
                    int nIds = pCmd->GetIntListVar("idlist", pSyncIdList);
                    for (int nCtr = 0; nCtr < nIds; ++nCtr) {
                        int nObjId = pSyncIdList[nCtr];
                        if (nObjId == a_id)
                            deleteline = true;
                    }
                }
                if (pCmd->GetId() == a_id) {
                    deleteline = true;
                }
            }
            delete (pCmd);
        }
        if (deleteline) {
            SYSLOG_DEBUG("Deleting Line: %s", linebuffer);
        }
        else {
            fputs(linebuffer, fpLogTmp);
        }
    }

    fclose(fpLog);
    if (fpLogTmp != NULL) {
        fclose(fpLogTmp);
        rename ("/home/onair/recovery.tmp", m_pRecoveryFile);
    }

}
