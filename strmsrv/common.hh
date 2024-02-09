#ifndef __H_COMMON__
#define __H_COMMON__



#define REQUESTDEVICE   "/home/onair/appcontrol"
#define RECOVERYDEVICE  "/home/onair/recovery"
#define CONTROLLER_THREAD       1
#define RECEIVER_THREAD         2
#define SENDER_THREAD           4
#define WRITER_THREAD           5
#define READER_THREAD           6
#define DISKIO_THREAD           7
#define MAXPARAMS               10
#define DEFAULT_DATADIR         "/data/DSS/data/portal/site/default/"
#define DEFAULT_REQPORT         10001
#define DEFAULT_NOTIFYPORT      10002
#define DEFAULT_HTTPPORT        8080
#define STATUSMSG               "STATUS"
#define ERRORMSG                "ERROR"
#define NOTIFYMSG               "NOTIFY"
#define LISTMSG                 "LIST"
#define RESETMSG                "RESET"
#define RESPMSG                 "RESPONSE"
#define MAXFILENAMELEN          256
#define MAXURLLEN               1024
#define MAXFRAMELEN             65535*40
#define MAXMSGLEN               8192
#define MAXHOSTLEN              32

#define CMDSUCCESS              200
#define CMDFAILURE              201

extern void AddThreadName(const char * threadName);
extern void RemoveThreadName();;
extern void SSSyslog(int a_priority,
                     const char* a_format,
                     ...) __attribute__ ((format (printf, 2, 3)));
extern void SSSyslog_Debug(const char *a_file, const char *a_function,
                           const char* a_format,
                           ...) __attribute__ ((format (printf, 3, 4)));
extern void InitSSSyslog(bool a_log_to_stdout);
extern void DeInitSSSyslog();
extern void DumpSSSyslog();
extern void DisableSSSyslog();
extern void EnableSSSyslog();
extern void SSSyslogTraceModule(const char *a_module);

#ifdef WINDOWS
#define LOG_LOCAL5     0
#define LOG_NOTICE     0
#define LOG_ERR        0
#define LOG_INFO       0
#define syslog(x, y) do {} while (0)
#endif

#define SYSLOG_NOTIFY(...)    SSSyslog(LOG_LOCAL5 | LOG_NOTICE,__VA_ARGS__)
#define SYSLOG_ERROR(...)    SSSyslog(LOG_LOCAL5 | LOG_ERR,__VA_ARGS__)
#define SYSLOG_INFO(...)    SSSyslog(LOG_LOCAL5 | LOG_INFO,__VA_ARGS__)
#define SYSLOG_DEBUG(...) SSSyslog_Debug(__FILE__, __FUNCTION__, __VA_ARGS__)
#ifdef SSDEBUG
#define SYSLOG_TRACE(...) SSSyslog_Debug(__FILE__, __FUNCTION__, __VA_ARGS__)
#else
#define SYSLOG_TRACE(...)   do {} while (0)
#endif

#ifdef SSDEBUG
#define ASSERT(expr) \
        if(! (expr)) \
        { \
                SYSLOG_ERROR("Assertion \"%s\" failed in file %s at line %d in function %s\n", \
                        #expr, __FILE__, __LINE__, __FUNCTION__); \
                abort(); \
        }
#else
#define ASSERT(expr) do {} while (0)
#endif

#define V2DNETWORKPORT          0x11
#define VNCNETWORKPORT          0x12
#define RTPNETWORKPORT          0x13
#define UDPNETWORKPORT          0x14
#define SCHEDULERPORT           0x15
#define MPEGTSNETWORKPORT       0x16
#define WINFBENCODERPORT        0x17
#define RTSPNETWORKPORT         0x18 //RK
#define DMSMEDIAPORT            0x41
#define WISENCODERPORT          0x42
#define V2DENCODERPORT          0x43
#define V2DDECODERPORT          0x21
#define MPEGTSDECODERPORT       0x22
#define RTPDECODERPORT          0x23
#define UDPDECODERPORT          0x24
#define DMSRECORDERPORT         0x81
#define DELETERPORT             0x82
#define V2DFFPORT               0x31
#define V2DHWPORT               0x44
#define V2DAUDIOENCODERPORT     0x45
#define V2DAUDIODECODERPORT     0x46
#define V2DPLAYERPORT           0x47
#define RTPPLAYERPORT           0x48
#define IMAGEWRITERPORT         0x49



#endif /*__H_COMMON__*/
