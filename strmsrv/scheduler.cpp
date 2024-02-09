
#include "strmsrv_compatibility.h"
#include <algorithm>

#include "common.hh"
#include "scheduler.hh"

using namespace std;

bool CompareSchedules(const CSchedEntry *a, const CSchedEntry *b) {

    if (a->GetTimeOfDay() < b->GetTimeOfDay()) {
        return true;
    }
    else if (a->GetTimeOfDay() == b->GetTimeOfDay()) {
        if (a->GetId() < b->GetId())
            return true;
    }
    return false;
}

CScheduler::CScheduler(int nId) :
    CReceiver(nId)
{
    m_nPortType = SCHEDULERPORT;

    // Create the queues for sending commands to controller thread
    m_qSchedulerSink =
        CSQueueManager::Instance()->GetQueueSink("ToScheduler");
    m_qSchedulerSource =
        CSQueueManager::Instance()->GetQueueSource("FromScheduler");

    m_commandsProcessed = 0;
    m_commandsFinished = 0;
    m_totalCommands = 0;
    //    m_currentCommand = NULL;
    m_lastCommand = NULL;
    m_nextCommand = NULL;
    m_nSchedCmdId = 1;
}

CScheduler::~CScheduler()
{

    CSQueueManager::Instance()->ReleaseQueueSink(m_qSchedulerSink);
    CSQueueManager::Instance()->ReleaseQueueSource(m_qSchedulerSource);

    FlushScheduleEntries();

}

int
CScheduler::AddScheduleEntry(const char * a_command, int a_id, timestamp_t a_tod)
{
    // fail on bad data
    if ((strlen(a_command) <= 0) || (a_id <= 0) || (a_tod == 0))
        return 0;


    // check for duplicates
    CSchedEntryIterator iter;
    for (iter=m_LSchedules.begin(); iter!=m_LSchedules.end(); ++iter) {
        if (a_id == (*iter)->GetId()) {
            return 0;
        }
    }

    // All ok, add the entry
    CSchedEntry *ce = new CSchedEntry(a_command, a_id, a_tod);
    if (ce != NULL) {
        m_LSchedules.push_back(ce);
        m_totalCommands++;
    }
    ASSERT(m_totalCommands == m_LSchedules.size());

    // sort our list
    std::sort(m_LSchedules.begin(), m_LSchedules.end(), CompareSchedules);
    //DumpScheduleEntries();

    FindNextCommand();

    return 1;

}

// Find the next command to execute
void
CScheduler::FindNextCommand() {

    m_nextCommand = NULL;
    CSchedEntryIterator iter;
    for (iter=m_LSchedules.begin(); iter!=m_LSchedules.end(); ++iter) {
        CSchedEntry * schedEntry = (*iter);
        if (schedEntry->m_status == Pending) {
            m_nextCommand = schedEntry;
            break;
        }
    }
}

int
CScheduler::DeleteScheduleEntry(int a_id)
{
    int retval = 0;
    // delete entry matching given id
    CSchedEntryIterator iter;
    for (iter=m_LSchedules.begin(); iter!=m_LSchedules.end(); ++iter) {
        CSchedEntry * schedEntry = (*iter);
        if (a_id == schedEntry->GetId()) {
            delete(schedEntry);
            m_LSchedules.erase(iter);
            retval = 1;
            break;
        }
    }
    // Fix our pointers/counters
    if (retval == 1) {
        m_lastCommand = NULL;
        m_totalCommands = 0;
        m_commandsProcessed = 0;
        m_commandsFinished = 0;
        for (iter=m_LSchedules.begin(); iter!=m_LSchedules.end(); ++iter) {
            m_totalCommands++;
            CSchedEntry * schedEntry = (*iter);
            if (schedEntry->m_status != Pending) {
                m_commandsProcessed++;
                m_lastCommand = schedEntry;
            }
            if (schedEntry->m_status == Processed) {
                m_commandsFinished++;
            }
        }
        FindNextCommand();
        ASSERT(m_totalCommands == m_LSchedules.size());
    }
    return retval;
}

int
CScheduler::FlushScheduleEntries()
{

    while (!m_LSchedules.empty()) {
        CSchedEntry * schedEntry = m_LSchedules.back();
        m_LSchedules.pop_back();
        delete schedEntry;
    }
//     m_currentCommand = NULL;
    m_lastCommand = NULL;
    m_nextCommand = NULL;
    m_commandsProcessed = 0;
    m_commandsFinished = 0;
    m_totalCommands = 0;
    ASSERT(m_totalCommands == m_LSchedules.size());
    return 1;
}

int
CScheduler::ListScheduleEntries(CCommand *a_pCmd)
{
    CSchedEntryIterator iter;

    a_pCmd->SetRootParam("CommandCount", m_totalCommands);

    for (iter=m_LSchedules.begin(); iter!=m_LSchedules.end(); ++iter) {
        CSchedEntry * schedEntry = (*iter);
        a_pCmd->SetCurrentObject(schedEntry->GetId());
        a_pCmd->SetObjectParam("Command", schedEntry->GetCommand().c_str());
        a_pCmd->SetObjectParam("Timestamp", schedEntry->GetTimeOfDay());
        a_pCmd->SetObjectParam("Response", schedEntry->GetResponse().c_str());
        switch(schedEntry->m_status) {
        case Pending:
            a_pCmd->SetObjectParam("Status", "Pending");
            break;
        case Processing:
            a_pCmd->SetObjectParam("Status", "Processing");
            break;
        case Processed:
            a_pCmd->SetObjectParam("Status", "Processed");
            break;
        }
    }

    return 1;
}

// Demux the update command for this object and invoke the appropriate
// internal command
int
CScheduler::Update(CCommand *a_pCmd)
{
    int retval=0;
    char command[256];
    int commandid;
    vector<int> commandidlist;
    timestamp_t tod;
    int numbentries;

    if ((a_pCmd->GetVar("addcommand", command, 256) != -1) &&
        (a_pCmd->GetIntVar("commandid", &commandid) != -1) &&
        (a_pCmd->GetTimestampVar("tod", &tod) != -1)) {
        retval = AddScheduleEntry(command, commandid, tod);
    }
    else if ((numbentries =
              a_pCmd->GetIntListVar("deletecommand", commandidlist)) != -1) {
        for (int i = 0; i < numbentries; i++)
            retval = DeleteScheduleEntry(commandidlist[i]);
    }
    else if (a_pCmd->GetIntVar("flushcommands", &commandid) != -1) {
        retval = FlushScheduleEntries();;
    }
    else if (a_pCmd->GetIntVar("listcommands", &commandid) != -1) {
        retval = ListScheduleEntries(a_pCmd);;
    }

    return retval;
}

int
CScheduler::ProcessStream() {


    // Send the next command for which the time of day has been reached

    if (IsStarted()) {     // Wait till the object is started

        ASSERT(m_nextCommand != NULL || m_commandsProcessed == m_totalCommands);
        if (m_nextCommand != NULL) {
            ASSERT(m_nextCommand->m_status == Pending);
            timestamp_t now = CTimeKeeper::Now();

            // Have we reached the time to schedule this command??
            if (m_nextCommand->GetTimeOfDay() <= now) {

                if (m_nTimestamp == 0) {
                    m_nStartTimestamp = now;
                    m_nStartWallClock = now;
                    m_bSendNotify = true;
                    m_nEventId = NOTIFICATION_EVENT_BEGIN;
                }

                m_nTimestamp = now;
                m_nFrameWallClock = now;

                string cmdStr(m_nextCommand->GetCommand());
                char buff[32];
                snprintf(buff, 31, " schedid=%d schedcmdid=%d", m_nId, m_nSchedCmdId);
                m_nextCommand->SetSchedCmdId(m_nSchedCmdId);
                m_nSchedCmdId++;
                if (m_nSchedCmdId > 100000000)
                    m_nSchedCmdId = 0;
                cmdStr.append(buff);
                CSFrame * sFrame = new CSFrame(cmdStr.c_str(),
                                               cmdStr.length());
                if (sFrame != NULL) {
                    m_qSchedulerSink->InsertFrame(sFrame);
                    m_nextCommand->m_status = Processing;
//                     m_currentCommand = m_nextCommand;
                    m_lastCommand = m_nextCommand;
                    m_commandsProcessed++;
                }
                else {
                    m_nextCommand->m_status = Processed;
                    m_commandsFinished++;
                }
                // Find next command to process
                FindNextCommand();
            }
        }
    }

    // Process any results that may have come from the control thread.
    CSFrame *sFrame = m_qSchedulerSource->GetFrame();
    if (sFrame) {
        // Was this response meant for us??
        char * commandStr = sFrame->GetData();
        int schedid;
        int schedcmdid = 0;
        char *cp;
        CSchedEntryIterator iter;
        sscanf(commandStr, "schedid=%9d ", &schedid);
        if (schedid == m_nId) {
            // response was for us
            SYSLOG_DEBUG("Got a response %s", commandStr);

            /* Get Sched Command ID from the response */
            cp = strstr(commandStr, " i:") + 3;
            if (cp != NULL)
                sscanf(cp, "%9d", &schedcmdid);
            /* Match response to request. Most often, the response
               will be for the last command issued, so we try to match
               that first instead of a brute force (more expensive)
               search */
            if (m_lastCommand != NULL &&
                m_lastCommand->m_status == Processing &&
                m_lastCommand->GetSchedCmdId() == schedcmdid) {
                m_lastCommand->SetResponse(strstr(commandStr, " ") + 1);
                m_lastCommand->m_status = Processed;
                m_commandsFinished++;
            }
            else {
                for (iter=m_LSchedules.begin(); iter!=m_LSchedules.end(); ++iter) {
                    CSchedEntry * schedEntry = (*iter);
                    if (schedEntry->m_status == Processing &&
                        schedEntry->GetSchedCmdId() == schedcmdid) {
                        schedEntry->SetResponse(strstr(commandStr, " ") + 1);
                        schedEntry->m_status = Processed;
                        m_commandsFinished++;
                    }
                }
            }
        }
        m_qSchedulerSource->ReleaseFrame(sFrame);
    }
    return 0;
}



void
CScheduler::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectGuageParam("NumberOfEntries", m_totalCommands);
    a_pCmd->SetObjectGuageParam("Processed", m_commandsProcessed);
    a_pCmd->SetObjectGuageParam("Finished", m_commandsFinished);
    if (m_lastCommand == NULL) {
        a_pCmd->SetObjectParam("LastCommand", "");
        a_pCmd->SetObjectParam("LastCmdId", (int)-1);
        a_pCmd->SetObjectParam("LastResponse", "");
    }
    else {
        a_pCmd->SetObjectParam("LastCommand", m_lastCommand->GetCommand().c_str());
        a_pCmd->SetObjectParam("LastCmdId", m_lastCommand->GetId());
        a_pCmd->SetObjectParam("LastResponse", m_lastCommand->GetResponse().c_str());
    }

    if (m_nextCommand == NULL) {
        a_pCmd->SetObjectParam("NextCommand", "");
        a_pCmd->SetObjectParam("NextCmdId", (int)-1);
        a_pCmd->SetObjectParam("TimeLeft", "");
    }
    else {
        a_pCmd->SetObjectParam("NextCommand", m_nextCommand->GetCommand().c_str());
        a_pCmd->SetObjectParam("NextCmdId", m_nextCommand->GetId());
        timestamp_t timeleft = 0;
        if (m_nextCommand->GetTimeOfDay() > CTimeKeeper::Now())
            timeleft = m_nextCommand->GetTimeOfDay() - CTimeKeeper::Now();
        char buffer[32];
        a_pCmd->SetObjectParam("TimeLeft",
                               CTimeKeeper::GetDurationString(timeleft, buffer));
     }

}

void
CScheduler::DumpScheduleEntries()
{
    CSchedEntryIterator iter;
    int i = 0;
    for (iter=m_LSchedules.begin(); iter!=m_LSchedules.end(); ++iter) {
        CSchedEntry *schedEntry = (*iter);
        SYSLOG_DEBUG("%d: %s, %d, %"PRIu64"", i++,
                     schedEntry->GetCommand().c_str(),
                     schedEntry->GetId(),
                     schedEntry->GetTimeOfDay());
    }
}
