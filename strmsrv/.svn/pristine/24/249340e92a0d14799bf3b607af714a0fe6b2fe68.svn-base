#ifndef __HH_SCHEDULER_H__
#define __HH_SCHEDULER_H__

#include <vector>

#include "receiver.hh"

enum SchedStatus {
    Pending = 1,
    Processing,
    Processed,
};

class CSchedEntry
{
public:


    CSchedEntry(const char * a_command, int a_id, timestamp_t a_tod) :
        m_status(Pending),
        m_command(a_command),
        m_response(""),
        m_id(a_id),
        m_tod(a_tod),
        m_nSchedCmdId(0) {};
    ~CSchedEntry() {};

    SchedStatus         m_status; // public member variable

    const std::string & GetCommand() const {return m_command;};
    int GetId() const {return m_id;};
    timestamp_t GetTimeOfDay() const {return m_tod;};
    void SetResponse(const char *a_response) {m_response = a_response;};
    const std::string & GetResponse() const {return m_response;};
    void SetSchedCmdId(int id) {m_nSchedCmdId = id;};
    int GetSchedCmdId() {return m_nSchedCmdId;};

private:
    std::string         m_command;
    std::string         m_response;
    int                 m_id;
    timestamp_t         m_tod;
    int                 m_nSchedCmdId;

};


typedef std::vector<CSchedEntry *>::iterator CSchedEntryIterator;

class CScheduler : public CReceiver
{
public:
    CScheduler(int nId);
    
    ~CScheduler();
    
    int ProcessStream();
    
    int  Update(CCommand *a_pCmd);

    void UpdateCustomStatus(CCommand *a_pCmd);
    
private:

//     CSchedEntry *          m_currentCommand;
    CSchedEntry *          m_lastCommand;
    CSchedEntry *          m_nextCommand;
    std::vector<CSchedEntry *>  m_LSchedules;
    CSQueueSink *          m_qSchedulerSink;
    CSQueueSource *        m_qSchedulerSource;
    unsigned long          m_commandsProcessed;
    unsigned long          m_commandsFinished;
    unsigned long          m_totalCommands;
    int                    m_nSchedCmdId;
    
    void DumpScheduleEntries();
    void FindNextCommand();
    int AddScheduleEntry(const char *a_command, int a_id, timestamp_t a_tod);
    int DeleteScheduleEntry(int a_id);
    int FlushScheduleEntries();
    int ListScheduleEntries(CCommand *a_pCmd);

};

#endif /* __HH_SCHEDULER_H__ */
