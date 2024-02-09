
#include "strmsrv_compatibility.h"
#include "readerthr.hh"
#include "reader.hh"
#include "common.hh"

using namespace std;

/// This is the thread code


/// Construtor for the Reader thread
CReaderThr::CReaderThr(const char *pMediaDir) :
    CStreamerThread("Reader")
{
    m_pMediaDir = new char[MAXFILENAMELEN+1];
    sprintf (m_pMediaDir, "%s", pMediaDir);

}


/// Destructor for the Reader thread
CReaderThr::~CReaderThr()
{

    SYSLOG_TRACE("Destructing %s:%s\n", __FILE__, __FUNCTION__);

    delete[] m_pMediaDir;
}

int
CReaderThr::handleQueryStreamer(CCommand *a_pCmd, timestamp_t & a_timeStamp)
{
    char command[256];
    if (a_pCmd->GetVar("command", command, 256) == -1) {
        return -1;
    }

    if (strcasecmp(command, "GetTimecodeAtOffset") == 0) {
#ifdef HAVE_DISKIO
        char pMediaName[80];
        char pMediaDir[256];

        strcpy(pMediaName, "");
        a_pCmd->GetVar("srcname", pMediaName,80);
        if (a_pCmd->GetVar("srcdir", pMediaDir, 256) == -1) {
            strcpy(pMediaDir, m_pMediaDir);
        }

        timestamp_t nOffSet = 0;
        if (a_pCmd->GetTimestampVar("offset", &nOffSet) == -1) {
            return -1;
        }

        // The Reader takes control of the a_pCmd object and fills in
        // the result
        CReader::GetTimecodeAtOffset(a_pCmd,
                                     pMediaName,
                                     pMediaDir,
                                     nOffSet);

#endif
        return 0;

    }

    if (strcasecmp(command, "GetOffsetAtTimecode") == 0) {
#ifdef HAVE_DISKIO
        char pMediaName[80];
        char pMediaDir[256];

        strcpy(pMediaName, "");
        a_pCmd->GetVar("srcname", pMediaName,80);
        if (a_pCmd->GetVar("srcdir", pMediaDir, 256) == -1) {
            strcpy(pMediaDir, m_pMediaDir);
        }


        timestamp_t nTimecode = 0;
        if (a_pCmd->GetTimestampVar("tc", &nTimecode) == -1) {
            return -1;
        }

        // The Reader takes control of the a_pCmd object and fills in
        // the result
        CReader::GetOffsetAtTimecode(a_pCmd,
                                     pMediaName,
                                     pMediaDir,
                                     nTimecode);
#endif
        return 0;
    }

    // No commands to handle
    return -1;

}

CStreamer *
CReaderThr::handleAdd(CCommand *pCmd)
{
#ifdef HAVE_DISKIO
    char pMediaName[80];
    char pMediaDir[256];
    char speed[80];

    strcpy(pMediaName, "");
    pCmd->GetVar("srcname", pMediaName,80);
    if (pCmd->GetVar("srcdir", pMediaDir, 256) == -1) {
        strcpy(pMediaDir, m_pMediaDir);
    }
    timestamp_t nTimecode = pCmd->GetTimecode();
    timestamp_t nOffSet = 0;
    if (pCmd->GetTimestampVar("offset", &nOffSet) != -1) {
        /* Offset is provided. The reader module will use it only if non zero */
    }

    strcpy(speed, "1.0");
    pCmd->GetVar("speed", speed, 80);

    int nObjId = pCmd->GetId();

    CReader *pReader =
        new CReader (nObjId,
                     pMediaName,
                     pMediaDir,
                     nTimecode,
                     nOffSet,
                     speed);

    return pReader;
#endif

    return NULL;
}


/// Overridden handleSyncStart to make sure all readers are ready to serve
/// before issuing Start
int
CReaderThr::handleSyncStart(CCommand *pCmd)
{
#ifdef HAVE_DISKIO
    int nState = 0;
    timestamp_t nTimecode = 0;
    intlist pSyncIdList;
    intlist pSyncQList;
    int nIds = pCmd->GetIntListVar("idlist",pSyncIdList);
    nIds = pCmd->GetIntListVar("qlist",pSyncQList);

    SStartCounter * startCounter = new SStartCounter(nIds);
    for (int nCtr=0; nCtr < nIds; nCtr++) {
        int nObjId=pSyncIdList[nCtr];
        int nQueueId=pSyncQList[nCtr];
        for (list<CStreamer *>::iterator iter = m_LStreamers.begin();
             iter != m_LStreamers.end(); ++iter) {
            if (nObjId ==
                (*iter)->GetId()) {
                CReader *reader = static_cast <CReader *> (*iter);

                if (!((*iter)->IsStarted())) {
                    reader->setStartCounter(startCounter);
                    reader->Start(nQueueId);

                    nTimecode = reader->GetTimecode();
                    nState = reader->GetState();
                }
            }
        }
    }
    pCmd->SetResult(CMDSUCCESS);
    pCmd->SetTimecode(nTimecode);
    pCmd->SetStatus(nState);
#endif

    return 0;
}





