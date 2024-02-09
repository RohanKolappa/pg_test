#ifndef __H_CTRLTHR__
#define __H_CTRLTHR__

#include "appthread.hh"
#include "command.hh"
#include "command.hh"
#include "squeue.hh"
#include "notify.hh"




#define MAXDEVICESIZE   120
#define MAXREQSIZE      8192
#define HTTP_PORT       8080

#define CPUWIN          32

class CCpuStats;

typedef std::map<int ,CCommand*> RQ;
typedef RQ::value_type RQEntry;


/// The Controller Thread Class
class CCtrlThr : public CAppThread
{
private:
    CSQueueSource *         m_qControllerSource;
    CSQueueSource *         m_qSchedulerSource;
    CSQueueSink *           m_qSchedulerSink;
    CSQueueSink *           m_qSenderSink;
    CSQueueSink *           m_qReceiverSink;
    CSQueueSink *           m_qWriterSink;
    CSQueueSink *           m_qReaderSink;
    RQ                      m_rList;
    RQ                      m_rHTTPList;
    RQ                      m_rSchedulerList;
    std::map<int, CNotify*> m_rNotificationList;
    int                     m_nFilePosition;
    int                     m_nSocket;
    int                     m_nHTTPSocket;
    int                     m_nNotifySocket;
    int                     m_nHTTPPort;
    int                     m_nRequestPort;
    int                     m_nNotifyPort;
    int                     m_nCmdId;
    char *                  m_pRequestFile;
    char *                  m_pRecoveryFile;
    char *                  m_pRequestString;
    bool                    m_bHup;
    bool                    m_bProcesedRequestFile;
    bool                    m_bDoRecovery;
    bool                    m_bDoRequest;
    bool                    m_bReuseAddress;
    bool                    m_bRequestPortUpdated;
    bool                    m_bNotifyPortUpdated;
    bool                    m_bHTTPPortUpdated;

    CCpuStats *             m_pCpuStats;

    int ProcessFileRequests();
    int ProcessSchedulerRequests();
    int ProcessSchedulerResponse();
    int ProcessNotificationMessages();
    int ProcessSocketRequests();
    int ProcessHTTPRequests();
    int ProcessModuleRequests();
    int ProcessOneCommand(const char *, CCommand *pCmd);
    void LogCommandToRecoveryFile(const char *pCmdString, CCommand *pCmd);
    int SendGlobalStatus(CCommand *pCmd);
    void TrimRecoveryFile(int a_id, bool a_bFullReset);
    void InitializeSockets();
    static int CreateTCPListener(int port, bool bReuseAddress);
    static void CloseSocket(int a_sockfd);
    bool CreateListenerPort(int & a_nSocket, int a_nPort);
    void CreateQueues();
    void InitializeFiles(const char *requestDevice, const char *recoveryDevice);
    void ClearNotificationList();
    void ClearHTTPList();
    void ClearRequestList();
    void ClearSchedulerList();

public:
    CCtrlThr(int nRequestPort, int nNotifyPort, int nHttpPort,
             const char *requestDevice, const char *recoveryDevice);
    ~CCtrlThr();
    int Init();
    int Run();
    void Quit();
    void HandleHup();
    int isRunning () { return !(m_bExit); };
    void SetReuseAddress(bool bValue);
    void SetFileModes(bool bRequest, bool bRecovery);
    void SetRequestPort(int port);
    void SetNotifyPort(int port);
    void SetHTTPPort(int port);
    static bool MatchSchedulerResponse(int a_nSchedId,
            const char * a_responseString, bool & a_bSuccess);

};
#endif /* __H_CTRLTHR__ */
