
#include "strmsrv_compatibility.h"
#include <signal.h>

//RK: relevant libs that require further inspection for deeper understanding
#include "rcvrthr.hh"
#include "ctrlthr.hh"
#include "writerthr.hh"
#include "senderthr.hh"
#include "readerthr.hh"
#include "diskiothr.hh"
#include "squeue.hh"
#include "common.hh"
#include "v2d.h"
#include "v2o_lib.h"
#include "ipputils.h"

//RK: prints out information about how to use the program and its available command-line options.
static void usage(char *command) { 
    printf("Usage: %s [OPTIONS]\n", command);
    printf("\nOptions:\n");
    printf("\t-r port\t\tTCP port to listen on for command requests \n\t\t\t(default %d)\n", DEFAULT_REQPORT);
    printf("\t-n port\t\tTCP port to send  notifications \n\t\t\t(default %d)\n", DEFAULT_NOTIFYPORT);
    printf("\t-d directory\tData directory for media files \n\t\t\t(default %s)\n", DEFAULT_DATADIR);
    printf("\t-R file\t\tComand request file (default %s)\n", REQUESTDEVICE);
    printf("\t-C file\t\tComand recovery file (default %s)\n", RECOVERYDEVICE);
    printf("\t-F\t\tRun application in foreground\n");
    exit(1);
}

//RK: establish control thread but don't assign values because it will be assigned later on 
static CCtrlThr *ctrl_thr;

void v2dLibraryLogger(int logLevel, char *logMessage) {
    if (logLevel < V2DLIB_INFO) {
        SYSLOG_INFO("V2DLIB: %s", logMessage);
    }
}

//RK: send commands to server using keyboard commands, handles UNIX signals
#ifndef WINDOWS
void sighandler(int sig)
{
    switch (sig) {
    case SIGPIPE:
        SYSLOG_INFO("Process %d ignoring SIGPIPE\n", getpid());
        break;
    case SIGINT:
    case SIGQUIT:
    case SIGTERM:
        ctrl_thr->Quit();
        break;
    case SIGHUP:
        ctrl_thr->HandleHup();
        break;
    default:
        break;
    }
    return;
}
#endif

//RK: entry point of application
int main(int argc, char *argv[])
{
    //RK: declare and initialize vars
    CRcvrThr *receiver_thr;
    CWriterThr *writer_thr;
    CReaderThr *reader_thr;
    CSenderThr *sender_thr;
    int nRequestPort = DEFAULT_REQPORT;
    int nNotifyPort = DEFAULT_NOTIFYPORT;
    int nHTTPPort = DEFAULT_HTTPPORT;
    char pMediaDir[MAXFILENAMELEN + 1];
    char requestDevice[MAXDEVICESIZE + 1];
    char recoveryDevice[MAXDEVICESIZE + 1];
    int ch = 0;
    int foreGround = 0;
    int retval = 0;
    bool logToStdOut = false;
    bool bRecovery = true;
    bool bRequest = true;
    bool bReuseAddress = true;
    bool bRequestPortProvided = false;
    bool bNotifyPortProvided = false;
    bool bHTTPPortProvided = false;

#ifndef WINDOWS
    signal (SIGPIPE, sighandler);
    signal (SIGTTOU, SIG_IGN);
    signal (SIGCHLD, SIG_IGN);
    signal (SIGINT, sighandler);
    signal (SIGTERM, sighandler);
    signal (SIGQUIT, sighandler);
    signal (SIGHUP, sighandler);
#endif

    sprintf (pMediaDir, "%s", DEFAULT_DATADIR);
    strcpy(requestDevice, "");
    strcpy(recoveryDevice, "");
//RK: The code uses a while loop with getopt to parse command-line arguments and set the corresponding variables. Command-line options include:
//-r port: TCP port to listen on for command requests.
//-n port: TCP port to send notifications.
//-d directory: Data directory for media files.
//-R file: Command request file.
//-C file: Command recovery file.
//-F: Run the application in the foreground.
//default vals defined in common.hh
    while ((ch = getopt(argc, argv, "r:n:d:hR:C:FH:h")) != -1) {
        switch(ch)
        {
        case 'r':
            nRequestPort = atoi(optarg);
            bRequestPortProvided = true;
            break;
        case 'n':
            nNotifyPort = atoi(optarg);
            bNotifyPortProvided = true;
            break;
        case 'H':
            nHTTPPort = atoi(optarg);
            bHTTPPortProvided = true;
            break;
        case 'd':
            sprintf (pMediaDir, "%s", optarg);
            break;
        case 'R':
            strncpy(requestDevice, optarg, MAXDEVICESIZE);
            break;
        case 'C':
            strncpy(recoveryDevice, optarg, MAXDEVICESIZE);
            break;
        case 'F':
            foreGround = 1;
            break;
        case 'h':
        default:
            usage(argv[0]); //RK: in case none of the requested char options are provided
            break;
        }
    }

    // Special things to do when we are not run as root:
    // - Not log to syslog
    // - Do not bind to 8080
    // - Do not daemonize
    // - No real time threads
    // - no request/recovery device unless explicitly requested
    int uid = 1;
#ifndef WINDOWS
    uid = getuid();
#endif
    if (uid != 0) { //RK: if running as non-root user
        foreGround = 1;
        CAppThread::DisableRealtime();
        logToStdOut = true;
        if (strlen(recoveryDevice) == 0)
            bRecovery = false;
        if (strlen(requestDevice) == 0)
            bRequest = false;
        if (!bRequestPortProvided)
            nRequestPort = 0;
        if (!bNotifyPortProvided)
            nNotifyPort = 0;
        if (!bHTTPPortProvided)
            nHTTPPort = 0;
        bReuseAddress = false;
    }

    // Daemonize
#ifndef WINDOWS
    if (foreGround == 0) {
        daemon(1, 0);
    }
#endif

    // Initialise IPP library
    InitIPPLib();

    // v2dLibSetLogger(v2dLibraryLogger);

    // Initialise our logging library
    // Setup syslog
    InitSSSyslog(logToStdOut);
    SYSLOG_INFO("Rohan");

#ifdef HAVE_V2O
    vlog_init("StreamingServer");
#endif
    AddThreadName("Main");

    // Initialize the timekeeper
    CTimeKeeper::Begin();

    // Initialize the Command Library
    CCommand::Init();

    // Setup QueueManager
    CSQueueManager * queueManager = CSQueueManager::Instance();

    ctrl_thr = new CCtrlThr(nRequestPort, nNotifyPort, nHTTPPort,
                            requestDevice, recoveryDevice);

    ctrl_thr->SetFileModes(bRequest, bRecovery);
    ctrl_thr->SetReuseAddress(bReuseAddress);

    if (ctrl_thr->Init() == -1) {
        SYSLOG_ERROR("Failed to Initialize Control thread.\n");
        retval = 1;
        goto EXIT_LABEL;
    }

//RK: all threads are being instantiated here
    receiver_thr = new CRcvrThr();
    writer_thr = new CWriterThr(pMediaDir);
    reader_thr = new CReaderThr(pMediaDir);
    sender_thr = new CSenderThr();
    //rtsp_srvr = new CRTSPServer(); //RK



//RK: all threads are being started here
    ctrl_thr->Start();
    receiver_thr->Start();
    writer_thr->Start();
    reader_thr->Start();
    sender_thr->Start();

    // Wait till Controller thread is done.
    // (kill signal,  action=quit, /tmp/ssstop etc)
    while (ctrl_thr->isRunning())
        usleep(1000);

    ctrl_thr->Stop();

    receiver_thr->Stop();
    writer_thr->Stop();
    reader_thr->Stop();
    sender_thr->Stop();

    delete(receiver_thr);
    delete(ctrl_thr);
    delete(writer_thr);
    delete(sender_thr);
    delete(reader_thr);

    queueManager->Destroy(); // Releases memory
    v2dLibCleanup();

 EXIT_LABEL:
    SYSLOG_INFO("StreamingServer Exiting...");

    SYSLOG_TRACE("Sbuffer Count = %d", CSBuffer::BufferCount());

    CCommand::Finish();

    //        DeInitSSSyslog();

    return retval;
}
