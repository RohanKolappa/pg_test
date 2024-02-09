#ifndef __H_READERQUERYTHR__
#define __H_READERQUERYTHR__


#include "appthread.hh"
#include "reader.hh"


#define COMMAND_TC_AT_OFFSET   1
#define COMMAND_OFFSET_AT_TC   2

class CReaderQueryThread : public CAppThread {

public:
    CReaderQueryThread(CCommand *a_pCmd,
                       char *pMedianame,
                       char *pMediaDir,
                       int nCommandType,
                       timestamp_t nTimecode);
    int Run();

private:
    char * m_pMediaName;
    char * m_pMediaDir;
    mediaindex::SourceIndexInterface * m_pSourceIndex;
    timestamp_t m_nTimeCode;
    CCommand * m_pCmd;
    int m_nCommandType;

    ~CReaderQueryThread();
};

#endif // __H_READERQUERYTHR__
