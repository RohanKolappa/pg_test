#include "strmsrv_compatibility.h"
#include <sstream>

#include "MediaIndexUtils.hh"
#include "readerquerythr.hh"
#include "common.hh"


CReaderQueryThread::CReaderQueryThread(CCommand *a_pCmd,
                                       char *a_pMediaName,
                                       char *a_pMediaDir,
                                       int nCommandType,
                                       timestamp_t a_nTimecode)
{
    m_priority = 5; // lower than the other threads which are at 10
    m_bDetached = true; // This a non-joinable thread;
    
    if (m_pThreadName != NULL) {
        sprintf(m_pThreadName, "ReaderQuery");
    }

    m_nTimeCode = a_nTimecode;
    m_pCmd = a_pCmd;
    m_nCommandType = nCommandType;

    m_pMediaName = new char[MAXFILENAMELEN+1];
    m_pMediaDir = new char[MAXFILENAMELEN+1];

    memset(m_pMediaName, 0, MAXFILENAMELEN);
    memset(m_pMediaDir, 0, MAXFILENAMELEN);

    sprintf(m_pMediaName, "%s", a_pMediaName);
    sprintf(m_pMediaDir, "%s", a_pMediaDir);

    m_pSourceIndex = NULL;

}

CReaderQueryThread::~CReaderQueryThread()
{

    SYSLOG_INFO("Deleting CReaderQueryThread");

    if (m_pSourceIndex)
        delete(m_pSourceIndex);

    delete[] m_pMediaName;
    delete[] m_pMediaDir;


}

int
CReaderQueryThread::Run()
{
    int rc;

    if (m_pCmd == NULL)
        goto EXIT_LABEL;

    // First create the source index
    m_pSourceIndex = mediaindex::createSourceIndex(m_pMediaName, m_pMediaDir);

    // No need to "start" the source index. The functions we will
    // be using below dont need it.

    switch(m_nCommandType) {
    case COMMAND_TC_AT_OFFSET: 
        {
            timestamp_t offset = m_nTimeCode;
            timestamp_t timeCode = 0;
            rc = m_pSourceIndex->getTimecodeAtOffset(offset, &timeCode);
            
            if (rc == INDEX_ERROR) {
                SYSLOG_ERROR("Index Error for getTimecodeAtOffset: %s",
                             m_pSourceIndex->getLastError());
                m_pCmd->SetTimecode(0);
                m_pCmd->SetResult(CMDFAILURE);
            }
            else {
                SYSLOG_DEBUG("Timecode at Offset %"PRIu64" = %"PRIu64"",
                             offset, timeCode);
                m_pCmd->SetTimecode(timeCode);
                m_pCmd->SetResult(CMDSUCCESS);
            }
            
            break;
        }
    case COMMAND_OFFSET_AT_TC: 
        {
            timestamp_t timeCode = m_nTimeCode;
            timestamp_t offset = 0;
            rc = m_pSourceIndex->getOffsetFromTimecode(timeCode, &offset);
            
            if (rc == INDEX_ERROR) {
                SYSLOG_ERROR("Index Error for getOffsetFromTimecode: %s",
                             m_pSourceIndex->getLastError());
                m_pCmd->SetTimecode(0);
                m_pCmd->SetResult(CMDFAILURE);
            }
            else {
                SYSLOG_DEBUG("Offset at Timecode %"PRIu64" = %"PRIu64"",
                             timeCode, offset);
                m_pCmd->SetTimecode(offset);
                m_pCmd->SetResult(CMDSUCCESS);
            }
            break;
        }
    default: 
        {
            m_pCmd->SetTimecode(0);
            m_pCmd->SetResult(CMDFAILURE);
            break;
        }
    }
    
 EXIT_LABEL:
    // We are responsible to delete ourselves
    delete(this);

    return 0;
}
