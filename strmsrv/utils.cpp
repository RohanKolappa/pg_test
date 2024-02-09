
#include "strmsrv_compatibility.h"
#include <stdarg.h>
#include <string>
#include <sys/stat.h>
#include <map>
using namespace std;

#include "common.hh"
#include "mutex.hh"
#include "diskio.hh"
#include "utils.hh"

#ifndef WINDOWS
typedef map<pthread_t, string> ThreadListType;
#else
typedef map<void *, string> ThreadListType;
#endif
static ThreadListType            m_tIdList;
static CMutex * logMutex;
static map<string, int> debugMap;
static bool log_inited = false;
static bool log_to_stdout = false;
static bool log_disabled = false;

/// Global method returns the name string assigned to current thread
static const char * GetThreadName()
{
    ThreadListType::iterator iter;
#ifndef WINDOWS
    pthread_t currentThreadId = pthread_self();
#else
    void * currentThreadId = (pthread_self()).p;
#endif
    iter = m_tIdList.find(currentThreadId);
    if (iter == m_tIdList.end())
        return "????";
    return (*iter).second.c_str();
}

void AddThreadName(const char * threadName)
{
    if (!log_inited) {
        InitSSSyslog(true);
    }
    CScopedLock lock(*logMutex);
#ifndef WINDOWS
    pthread_t currentThreadId = pthread_self();
#else
    void * currentThreadId = (pthread_self()).p;
#endif

    if (m_tIdList.find(currentThreadId) == m_tIdList.end()) {
        m_tIdList.insert(ThreadListType::value_type
                         (currentThreadId, threadName));
    }
}

void RemoveThreadName()
{
    if (!log_inited) {
        InitSSSyslog(true);
    }
    CScopedLock lock(*logMutex);
#ifndef WINDOWS
    pthread_t currentThreadId = pthread_self();
#else
    void * currentThreadId = (pthread_self()).p;
#endif

    if (m_tIdList.find(currentThreadId) != m_tIdList.end()) {
        m_tIdList.erase(currentThreadId);
    }
}


void SSSyslog(int a_priority,
              const char* a_format,
              ...)
{

    if (!log_inited)
        return;
    if (log_disabled)
        return;
    CScopedLock lock(*logMutex);
    va_list va;
    va_start(va, a_format);
    static char _debug_string_[256];

    int len = sprintf(_debug_string_, "%s: ", GetThreadName());
    vsnprintf(_debug_string_ + len, 255 - len, a_format, va);
    va_end(va);

    if (!log_to_stdout) {
        syslog(a_priority, _debug_string_);
    }
    else {
        static char _stdout_debug_string_[8192];
        len = sprintf(_stdout_debug_string_, "%s: ", GetThreadName());
        vsnprintf(_stdout_debug_string_ + len, 8191 - len, a_format, va);
        va_end(va);
        printf("%s", _stdout_debug_string_);
        if (*(strrchr(_stdout_debug_string_, '\0') - 1) != '\n')
            printf("\n");
    }
}

void SSSyslog_Debug(const char *a_file, const char *a_function,
                    const char* a_format,
                    ...)
{
    if (!log_inited)
        return;

    if (log_disabled)
        return;

    CScopedLock lock(*logMutex);
    va_list va;
    va_start(va, a_format);
    static char _debug_string_[256];

    if (debugMap[a_file] == 1 ||
            debugMap[string(a_file) + ":" + string(a_function)] == 1 ) {
        int len = sprintf(_debug_string_, "%s: ", GetThreadName());
        vsnprintf(_debug_string_ + len, 255 - len, a_format, va);
        va_end(va);

        if (!log_to_stdout) {
            syslog(LOG_LOCAL5 | LOG_INFO, _debug_string_);
        }
        else {
            static char _stdout_debug_string_[8192];
            len = sprintf(_stdout_debug_string_, "%s: ", GetThreadName());
            vsnprintf(_stdout_debug_string_ + len, 8191 - len, a_format, va);
            va_end(va);
            printf("%s", _stdout_debug_string_);
            if (*(strrchr(_stdout_debug_string_, '\0') - 1) != '\n')
                printf("\n");
        }
    }
}

void InitSSSyslog(bool a_log_to_stdout)
{

    FILE *fp;
    char buf[128];


    logMutex = new CMutex();

    if (log_inited == true) {
        SYSLOG_INFO("Reinitializing logging config %s\n", buf);
        CScopedLock lock(*logMutex);
        debugMap.clear();
    }
    if ((fp = fopen("/home/onair/debug.conf", "r")) != NULL) {
        while (fgets(buf, 127, fp) != NULL) {
            if (buf[0] == '#')
                continue;
            char *cp = strchr(buf, '\n');
            if (cp != NULL) {
                *cp = '\0';
                if (strlen(buf) > 0) {
                    debugMap[buf] = 1;
                    SYSLOG_INFO("Tracing module %s\n", buf);
                }
            }
        }
        fclose(fp);
    }
    log_inited = true;

    log_to_stdout = a_log_to_stdout;
}

void SSSyslogTraceModule(const char *a_module)
{
    if (strlen(a_module) > 0) {
        debugMap[a_module] = 1;
        SYSLOG_INFO("Tracing module %s\n", a_module);
    }
}

void DisableSSSyslog()
{
    log_disabled = true;
}

void EnableSSSyslog()
{
    log_disabled = false;
}

void DeInitSSSyslog()
{

}

void DumpSSSyslog()
{

    SYSLOG_DEBUG("SyslogMutexContention = %ld", logMutex->GetContention());
}


char * BitrateToString(double bitrate, char *buffer) {

    if (bitrate > 1000000.0 )
        snprintf(buffer, 32, "%.3f M", bitrate/1048576.0);
    else if (bitrate > 1000.0)
        snprintf(buffer, 32, "%.3f K", bitrate/1024.0);
    else
        snprintf(buffer, 32, "%.0f", bitrate);
    return buffer;
}

char * GetTimeString(timestamp_t a_milliseconds, char *a_buffer)
{
    int seconds = (int) (a_milliseconds / 1000);
    int ms = (int) (a_milliseconds % 1000);
    int days = seconds / 86400;
    seconds = seconds % 86400;
    int hours = seconds / 3600;
    seconds = seconds % 3600;
    int mins = seconds / 60;
    seconds = seconds % 60;
    int len = 0;
    if (days > 0)
        len += sprintf(a_buffer + len, "%dd, ", days);
    if (hours > 0 || days > 0)
        len += sprintf(a_buffer + len, "%dh, ", hours);
    if (mins > 0 || hours > 0 || days > 0)
        len += sprintf(a_buffer + len, "%dm, ", mins);
    sprintf(a_buffer + len, "%2d.%03ds", seconds, ms);
    return a_buffer;
}

int ReadFileIntoBuffer(const char * filename, uint8_t * &buffer) {
    struct stat statbuf;
    int status;
    FILE *fp;

    buffer = NULL;

    // Get status on file
    status = stat(filename, &statbuf);
    if (status == -1)
        return -1;

    // Open file
    fp = fopen(filename, "rb");
    if (fp == NULL)
        return -1;

    // Allocate memory for file contents
    buffer = new uint8_t [statbuf.st_size + 1];
    if (buffer == NULL) {
        fclose(fp);
        return -1;
    }

    // Read the file into buffer
    fread(buffer, 1, statbuf.st_size, fp);
    buffer[statbuf.st_size] = 0;
    fclose(fp);

    return statbuf.st_size;
}

std::string TrimString(std::string a_string)
{
    const char * str = a_string.c_str();

    // Trim leading non-letters
    while (!isalnum(*str))
        str++;

    // Trim trailing non-letters
    const char *end = str + strlen(str) - 1;
    while (end > str && !isalnum(*end))
        end--;

    return string(str, end + 1);
}
