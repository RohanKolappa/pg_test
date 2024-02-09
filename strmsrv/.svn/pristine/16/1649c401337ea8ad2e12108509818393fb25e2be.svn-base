
#include "strmsrv_compatibility.h"
#include <sstream>

#include "streamerthr.hh"
#include "common.hh"

using namespace std;

CStreamerThread::CStreamerThread(string a_threadName) :
        m_delta(0),
        m_threadId(0),
        m_bitrate(0.0)
{
    m_LCommands.clear();

    if (m_pThreadName != NULL) {
        // Set the thread name string
        strcpy(m_pThreadName, a_threadName.c_str());

        // SendStatistics sends out integer ids
        // for backwards compatibility
        if (a_threadName == "Reader")
            m_threadId = READER_THREAD;
        else if (a_threadName == "Writer")
            m_threadId = WRITER_THREAD;
        else if (a_threadName == "Sender")
            m_threadId = SENDER_THREAD;
        else if (a_threadName == "Receiver")
            m_threadId = RECEIVER_THREAD;
    }

    m_qStreamerSource =
        CSQueueManager::Instance()->GetQueueSource(a_threadName.c_str());
    m_qControllerSink =
        CSQueueManager::Instance()->GetQueueSink("Controller");

    // Create a pool of processor threads for this thread. The
    // processor affinity for each thread is spread evenly among the
    // available number of processors
#ifndef WINDOWS
    m_numbCPUs = sysconf(_SC_NPROCESSORS_CONF);
    if (m_numbCPUs == 1)
        m_numbCPUs = 0;
#endif
    // For now, no scaling to multiple CPUs, till the facility is stable
    m_numbCPUs = 0;

    SYSLOG_DEBUG("Number of CPUs = %d", m_numbCPUs);
    m_poolSize = m_numbCPUs;
    for (int i = 0; i < m_poolSize; i++) {
        ostringstream namestream;
        namestream << a_threadName << "_" << i;

        CProcessorThread * thread = new CProcessorThread(namestream.str(),
                i % m_numbCPUs);
        m_processorThreadList.push_back(thread);
        //thread->Start();
    }
}


CStreamerThread::~CStreamerThread()
{

    SYSLOG_TRACE("Destructing %s in %s\n", m_pThreadName, __FUNCTION__);
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (m_poolSize == 0) {
            (*iter)->DeleteStream();
        }
        else {
            int processor_index = (*iter)->GetId() % m_poolSize;
            m_processorThreadList[processor_index]->RemoveStreamer((*iter));
        }
    }
    m_LStreamers.clear();

    for (list<CCommand *>::iterator cIter = m_LCommands.begin();
            cIter != m_LCommands.end(); ++cIter) {
        delete(*cIter);
    }
    m_LCommands.clear();

    CSQueueManager::Instance()->ReleaseQueueSink(m_qControllerSink);
    CSQueueManager::Instance()->ReleaseQueueSource(m_qStreamerSource);

    // Cleanup the processor threads
    while (!m_processorThreadList.empty()) {
        CProcessorThread * thread = m_processorThreadList.back();
        thread->Stop();
        m_processorThreadList.pop_back();
        delete thread;
    }

}


/// Main routine for the Streamer thread
int
CStreamerThread::Run()
{
    timestamp_t prev, now, delta;
    int count = 0;

    CTimeKeeper::UpdateTime();
    prev = CTimeKeeper::Now();

    if (m_poolSize > 0) {
        for (int i = 0; i < m_poolSize; i++) {
            m_processorThreadList[i]->Start();
        }
    }

    while (!m_bExit) {
        CTimeKeeper::UpdateTime(); // update latest time
        now = CTimeKeeper::Now();
        delta = now - prev;
        count = 0;

        count += ProcessCommands();
        count += ProcessStreamers();
        count += SendResponse();

        if (delta > 1000) {
            // count += SendErrors();
            count += SendStatistics();
            // count += SendNotifications();

            m_delta = delta;
            prev = now;
        }
        if (count == 0)
            LightSleep();
    }

    SYSLOG_INFO("Exiting the %s Thread\n", m_pThreadName);
    return 0;
}


int
CStreamerThread::handleSyncStart(CCommand *pCmd)
{
    int nState = 0;
    timestamp_t nTimecode = 0;
    intlist pSyncIdList;
    intlist pSyncQList;
    int nIds = pCmd->GetIntListVar("idlist", pSyncIdList);
    nIds = pCmd->GetIntListVar("qlist", pSyncQList);
    for (int nCtr = 0; nCtr < nIds; nCtr++) {
        int nObjId = pSyncIdList[nCtr];
        int nQueueId = pSyncQList[nCtr];
        for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
                iter != m_LStreamers.end(); ++iter) {
            if (nObjId == (*iter)->GetId()) {
                if (!((*iter)->IsStarted())) {
                    (*iter)->Start(nQueueId);
                    nTimecode = (*iter)->GetTimecode();
                    nState = (*iter)->GetState();
                }
            }
        }
    }
    pCmd->SetResult(CMDSUCCESS);
    pCmd->SetTimecode(nTimecode);
    pCmd->SetStatus(nState);

    return 0;
}

int
CStreamerThread::handleSyncStop(CCommand *pCmd)
{
    int nState = 0;
    timestamp_t nTimecode = 0;
    intlist pSyncIdList;
    int nIds = pCmd->GetIntListVar("idlist", pSyncIdList);
    for (int nCtr = 0; nCtr < nIds; nCtr++) {
        int nObjId = pSyncIdList[nCtr];
        list<CStreamer *>::iterator iter;
        for (iter = m_LStreamers.begin();
                iter != m_LStreamers.end(); ++iter) {
            if (nObjId == (*iter)->GetId()) {
                if ((*iter)->IsStarted()) {
                    (*iter)->Stop();
                    nTimecode = (*iter)->GetTimecode();
                    nState = (*iter)->GetState();
                }
                else {
                    SYSLOG_ERROR("Object id=%d not started.\n",
                                 nObjId);
                }
                break;
            }
        }
        if (iter == m_LStreamers.end()) {
            SYSLOG_ERROR("Object id=%d not found.\n",
                         nObjId);
        }
    }
    pCmd->SetResult(CMDSUCCESS);
    pCmd->SetTimecode(nTimecode);
    pCmd->SetStatus(nState);

    return 0;
}

int
CStreamerThread::handleStop(CCommand *pCmd)
{
    timestamp_t nTimecode;
    int nObjId = 0;
    int nState = 0;

    nTimecode = pCmd->GetTimecode();
    nObjId = pCmd->GetId();

    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (nObjId != (*iter)->GetId())
            continue;

        if (!((*iter)->IsStarted()))
            break;
        (*iter)->Stop();
        nTimecode = (*iter)->GetTimecode();
        nState = (*iter)->GetState();
        pCmd->SetStatus(nState);
        pCmd->SetTimecode(nTimecode);
        pCmd->SetResult(CMDSUCCESS);
        return 0;
    }
    // return failure is we get here
    pCmd->SetResult(CMDFAILURE);
    return 0;
}

int
CStreamerThread::handleDelete(CCommand *pCmd)
{
    int nObjId = pCmd->GetId();

    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (nObjId != (*iter)->GetId())
            continue;
        timestamp_t nTimecode = (*iter)->GetTimecode();
        int nState = (*iter)->GetState();
        // Send delete request to the object, the object will delete itself
        //delete((*iter));
        if (m_poolSize == 0) {
            (*iter)->DeleteStream();
        }
        else {
            int processor_index = (*iter)->GetId() % m_poolSize;
            m_processorThreadList[processor_index]->RemoveStreamer((*iter));
        }
        m_LStreamers.erase(iter);
        pCmd->SetStatus(nState);
        pCmd->SetTimecode(nTimecode);
        pCmd->SetResult(CMDSUCCESS);
        return 0;
    }
    // if object not found, we get here
    pCmd->SetResult(CMDFAILURE);
    return 0;
}

int
CStreamerThread::handleQueryStreamer(CCommand *a_pCmd, timestamp_t & a_timeStamp)
{
    // Default implementation return error
    // Subclasses return success if they can handle the query
    return -1;
}

int
CStreamerThread::handleQuery(CCommand *a_pCmd)
{
    timestamp_t timeStamp = 0;

    a_pCmd->SetStatus(0);
    // Call handleQueryStreamer. If error is returned, we send error
    // response right away.  Else the handler object is supposed to
    // fill up the result when ready
    if (handleQueryStreamer(a_pCmd, timeStamp) < 0) {
        a_pCmd->SetTimecode(0);
        a_pCmd->SetResult(CMDFAILURE);
    }
    return 0;
}

//
int
CStreamerThread::handleUpdateStreamer(CCommand *a_pCmd, CStreamer *a_streamer)
{

    // Default implementation returns false.

    // Subclasses customize as needed.
    return 0;
}


int
CStreamerThread::handleUpdate(CCommand *pCmd)
{
    bool bMatched = false;
    int nObjId = pCmd->GetId();

    // find the streamer object and call upon it to process the update message
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        // if object id is set, send update command to that specific
        // object, else send it to all objects, one of which will
        // handle it
        if ((nObjId != (*iter)->GetId()) && nObjId)
            continue;

        if (handleUpdateStreamer(pCmd, (*iter))) {
            timestamp_t nTimecode = (*iter)->GetTimecode();
            int nState = (*iter)->GetState();
            pCmd->SetStatus(nState);
            pCmd->SetTimecode(nTimecode);
            pCmd->SetResult(CMDSUCCESS);
            bMatched = true;
        }
        else {
            if (nObjId) {
                pCmd->SetStatus(0);
                pCmd->SetTimecode(0);
                pCmd->SetResult(CMDFAILURE);
            }
        }
        if (nObjId) {
            return 0;
        }
    }
    // return failure is we get here
    if (!bMatched)
        pCmd->SetResult(CMDFAILURE);
    return 0;
}

void
CStreamerThread::resetGlobalStats()
{
    // Default implementation does nothing.
    // Subclasses over-ride as needed
}

int
CStreamerThread::handleReset(CCommand *pCmd)
{
    int deletedCount = 0;
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (m_poolSize == 0) {
            (*iter)->DeleteStream();
        }
        else {
            int processor_index = (*iter)->GetId() % m_poolSize;
            m_processorThreadList[processor_index]->RemoveStreamer((*iter));
        }
        deletedCount++;
    }
    m_LStreamers.clear();

    resetGlobalStats();

    char buff[256];
    sprintf (buff, "%s cid=%d Module=%d Deleted=%d;",
             RESETMSG, pCmd->GetCmdId(),
             m_threadId, deletedCount);
    pCmd->GetResultString().append(buff);
    pCmd->SetResult(CMDSUCCESS);

    SYSLOG_DEBUG("Sbuffer Count = %d", CSBuffer::BufferCount());
    SYSLOG_DEBUG("Sbuffer Size = %d", CSBuffer::BufferSize());


    return 0;
}

int
CStreamerThread::ProcessCommands()
{
    CSFrame *sFrame = m_qStreamerSource->GetFrame();
    if (sFrame == NULL) {
        return 0;
    }

    char * pCmdString = sFrame->GetData();
    CCommand *pCmd = new CCommand();

    if (pCmd == NULL || pCmd->Init(pCmdString) < 0)
        return 0;

    int nObjId = pCmd->GetId();
    int nState = -1;
    timestamp_t nTimecode = 0;

    list<CStreamer *>::iterator iter;
    for (iter = m_LStreamers.begin(); iter != m_LStreamers.end(); ++iter) {
        if (nObjId == (*iter)->GetId()) {
            nTimecode = (*iter)->GetTimecode();
            nState = (*iter)->GetState();
            break;
        }
    }

    /* Log only important commands */
    switch (pCmd->GetOperation()) {
    case OP_ADD:
    case OP_DEL:
    case OP_START:
    case OP_STOP:
    case OP_SYNCSTART:
    case OP_SYNCSTOP:
    case OP_STATUS:
    case OP_RESET:
    case OP_LIST:
    case OP_QUERY:
        SYSLOG_INFO("%s\n", pCmdString);
        break;
    default:
        break;
    }

    switch (pCmd->GetOperation()) {
    case OP_ADD:
        if (iter == m_LStreamers.end() && nObjId > 0) {
            CStreamer * pStreamer = handleAdd(pCmd);
            if (pStreamer != NULL) {
                m_LStreamers.push_back(pStreamer);
                pStreamer->AddStream();
                pStreamer->SetCustomStatusInfo();
                pCmd->SetResult(CMDSUCCESS);
                pCmd->SetStatus(0);
                pCmd->SetTimecode(0);
                if (m_poolSize > 0) {
                    int processor_index = pStreamer->GetId() % m_poolSize;
                    m_processorThreadList[processor_index]->AddStreamer(pStreamer);
                }
            }
            else {
                pCmd->SetResult(CMDFAILURE);
                pCmd->SetTimecode(0);
                pCmd->SetStatus(0);
            }
        }
        else {
            pCmd->SetResult(CMDFAILURE);
        }
        break;
    case OP_DEL:
        if (iter != m_LStreamers.end()) {
            handleDelete(pCmd);
        }
        else {
            pCmd->SetResult(CMDFAILURE);
        }
        break;
    case OP_UPDATE:
        if ((iter != m_LStreamers.end()) || (!nObjId)) {
            handleUpdate(pCmd);
        }
        else {
            pCmd->SetResult(CMDFAILURE);
        }
        break;
    case OP_START:
        if (iter != m_LStreamers.end()) {
            if ((*iter)->IsStarted()) {
                pCmd->SetResult(CMDFAILURE);
            }
            else {
                (*iter)->Start(
                        pCmd->GetQueueId());
                nState = (*iter)->GetState();
                pCmd->SetResult(CMDSUCCESS);
                pCmd->SetTimecode(nTimecode);
                pCmd->SetStatus(nState);
            }
        }
        else {
            pCmd->SetResult(CMDFAILURE);
        }
        break;
    case OP_STOP:
        handleStop(pCmd);
        break;
    case OP_STATUS:
        if (iter != m_LStreamers.end()) {
            pCmd->SetResult(CMDSUCCESS);
            pCmd->SetStatus(nState);
            pCmd->SetTimecode(nTimecode);
        }
        else {
            pCmd->SetResult(CMDFAILURE);
        }
        break;
    case OP_SYNCSTART:
        handleSyncStart(pCmd);
        break;
    case OP_SYNCSTOP:
        handleSyncStop(pCmd);
        break;
    case OP_STATS:
        if (iter != m_LStreamers.end()) {
            int nStatsFlag = 0;
            pCmd->GetIntVar("statsvalue",
                    &nStatsFlag);
            (*iter)->SetStatsFlag(nStatsFlag);
            pCmd->SetResult(CMDSUCCESS);
        }
        else {
            pCmd->SetResult(CMDFAILURE);
        }
        break;
    case OP_LIST:
        SendStatus(pCmd);
        pCmd->SetResult(CMDSUCCESS);
        break;
    case OP_RESET:
        handleReset(pCmd);
        break;
    case OP_QUERY:
        handleQuery(pCmd);
        break;
    case OP_XLIST:
        SendObjectList(pCmd);
        pCmd->SetResult(CMDSUCCESS);
        break;
    case OP_XQLIST:
        SendObjectListByQueue(pCmd);
        pCmd->SetResult(CMDSUCCESS);
        break;
    case OP_XHELP:
        SendStatusHelp(pCmd);
        break;
    case OP_XSTATUS:
        if (iter != m_LStreamers.end()) {
            pCmd->SetResult(CMDSUCCESS);
            pCmd->SetStatus(nState);
            pCmd->SetTimecode(nTimecode);
            SendObjectStatus(pCmd);
        }
        else {
            pCmd->SetResult(CMDFAILURE);
        }
        break;
    default:
        ASSERT(0);
        break;
    }
    m_LCommands.push_back(pCmd);

    m_qStreamerSource->ReleaseFrame(sFrame);

    return 0;
}

/// Gets data from the various data sources
int
CStreamerThread::ProcessStreamers()
{
    // If processor threads are helping us, we have nothing to do
    if (m_poolSize > 0)
        return 0;

    int retval = 0;
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if ((*iter)->ProcessStream() > 0) {
            retval++;
        }
        /* If a notification is created by the object, send it out to
           the controller */
        if ((*iter)->GetNotifyFlag()) {
            string str;
            (*iter)->SendNotification(str);
            if (str.length() > 0) {
                CSFrame *sFrame = new CSFrame(str.c_str(), str.length());
                m_qControllerSink->InsertFrame(sFrame);
            }
        }
        /* If an error notification is created by the object, send it out to
           the controller */
        if ((*iter)->GetErrorFlag()) {
            string str;
            (*iter)->SendError(str);
            if (str.length() > 0) {
                CSFrame *sFrame = new CSFrame(str.c_str(), str.length());
                m_qControllerSink->InsertFrame(sFrame);
            }
        }
    }
    return retval;
}

/// Sends Errors to the controller thread
int CStreamerThread::SendErrors()
{
    string str;

    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (!((*iter)->GetErrorFlag())) continue;
        (*iter)->SendError(str);
    }
    if (str.length() > 0) {
        CSFrame *sFrame = new CSFrame(str.c_str(), str.length());
        m_qControllerSink->InsertFrame(sFrame);
    }
    return 0;
}

/// Sends Notifications to the controller thread
int CStreamerThread::SendNotifications()
{
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (!(*iter)->GetNotifyFlag()) continue;
        string str;
        (*iter)->SendNotification(str);
        if (str.length() > 0) {
            CSFrame *sFrame = new CSFrame(str.c_str(), str.length());
            m_qControllerSink->InsertFrame(sFrame);
        }
    }
    return 0;
}

/// Sends Statistics to the controller thread
int CStreamerThread::SendStatistics()
{
    unsigned long aggregateFrameDiff = 0;
    double aggregateBitRate = 0.0;
    string  str;

    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (!((*iter)->GetStatsFlag())) continue;
        (*iter)->ComputeStatistics(m_delta);
        (*iter)->SendStatistics(str);
        aggregateBitRate += (*iter)->GetBitRate();
        aggregateFrameDiff += (*iter)->GetFrameDiff();
    }

//     if (str.length() > 0) {
//         CSFrame *sFrame = new CSFrame(str.c_str(), str.length());
//         m_qControllerSink->InsertFrame(sFrame);
//     }

    if (!m_LStreamers.empty()) {
        SYSLOG_DEBUG("AGGREGATE Frames=%lu Bitrate:%.2fM\n",
                     aggregateFrameDiff,
                     aggregateBitRate);
    }
    m_bitrate = aggregateBitRate;

    return 0;
}

/// Sends Status to the controller thread
int CStreamerThread::SendStatus(CCommand   *pCmd)
{
    char buff[256];
    sprintf (buff, "%s cid=%d Module=%d Objects=%d Commands=%d;",
             LISTMSG, pCmd->GetCmdId(),
             m_threadId, m_LStreamers.size(), m_LCommands.size());
    pCmd->GetResultString().append(buff);
    return 0;
}


/// Sends Status of all objects to the controller thread
int CStreamerThread::SendObjectList(CCommand   *pCmd)
{
    pCmd->SetRootParam("ObjectCount", (int)m_LStreamers.size());
    pCmd->SetRootParam("Bitrate", (double) m_bitrate);
    pCmd->SetRootParam("DateStamp", (timestamp_t) CTimeKeeper::Now());
    pCmd->SetRootParam("Uptime", (unsigned long) CTimeKeeper::Uptime());
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        (*iter)->UpdateStatus(pCmd);
    }

    return 0;
}

/// Sends Status of all objects started with a given queueid to the
/// controller thread
int CStreamerThread::SendObjectListByQueue(CCommand *pCmd)
{
    pCmd->SetRootParam("DateStamp", (timestamp_t) CTimeKeeper::Now());
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {

        if ((*iter)->IsStarted()) { // only if the object is running
            if ((pCmd->GetQueueId() > 0) &&
                    (pCmd->GetQueueId() == (*iter)->GetQueueId())) {
                (*iter)->UpdateStatus(pCmd);
            }
        }
    }

    return 0;
}

/// Sends Help on Status name value pairs
int CStreamerThread::SendStatusHelp(CCommand   *pCmd)
{
    char helpOn[80];
    int portType = pCmd->GetPortType();
    strcpy(helpOn, "");
    pCmd->GetVar("name", helpOn, 80);
    const char * helpstr = CStreamer::GetInfoString(portType, helpOn);
    if (helpstr != NULL) {
        pCmd->GetResultString().append(helpstr);
        pCmd->SetResult(CMDSUCCESS);
    }
    else {
        pCmd->SetResult(CMDSUCCESS);
    }

    return 0;
}

/// Sends Status of a specific object to the controller thread
int CStreamerThread::SendObjectStatus(CCommand   *pCmd)
{
    int nObjId = pCmd->GetId();

    pCmd->SetRootParam("DateStamp", (timestamp_t) CTimeKeeper::Now());
    for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
            iter != m_LStreamers.end(); ++iter) {
        if (nObjId != (*iter)->GetId())
            continue;
        (*iter)->UpdateStatus(pCmd);
        break;
    }

    return 0;
}


/// Updates Response to the controller thread
int CStreamerThread::SendResponse()
{
    CCommand   *pCmd;
    CSFrame *sFrame;

    for (list<CCommand *>::iterator cIter = m_LCommands.begin();
            cIter != m_LCommands.end(); ++cIter) {
        pCmd = (*cIter);
        if (pCmd == NULL) continue;
        if (pCmd->GetResult() == 0)
            continue;

        /* Log only important commands */
        switch (pCmd->GetOperation()) {
        case OP_ADD:
        case OP_DEL:
        case OP_START:
        case OP_STOP:
        case OP_SYNCSTART:
        case OP_SYNCSTOP:
        case OP_STATUS:
        case OP_RESET:
        case OP_LIST:
        case OP_QUERY:
            SYSLOG_INFO("%s\n", pCmd->GetResponse().c_str());
            break;
        default:
            break;
        }

        sFrame = new CSFrame(pCmd->GetResponse().c_str(),
                             pCmd->GetResponse().length());
        m_qControllerSink->InsertFrame(sFrame);

        delete(pCmd);
        cIter = m_LCommands.erase(cIter);
    }
    return 0;
}
