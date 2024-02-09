#ifndef __H_DELETERDISKIOTHR__
#define __H_DELETERDISKIOTHR__


#include "appthread.hh"
#include "deleter.hh"

// Forward Declarations
class CDeleter;

class CDeleterDiskIOThread : public CAppThread {

public:
    CDeleterDiskIOThread(CDeleter *a_deleter);
    ~CDeleterDiskIOThread();
    int Run();

    unsigned long GetDeleteOperations() {
        return m_nDeleteOperations;
    }
    unsigned long GetForceDeleteOperations() {
        return m_nForceDeleteOperations;
    }
    unsigned long GetFileDeleteOperations() {
        return m_nFileDeleteOperations;
    }
    unsigned long GetDirectoryDeleteOperations() {
        return m_nDirectoryDeleteOperations;
    }
    unsigned long GetDeletedBytes() {
        return m_nDeletedBytes;
    }
    unsigned long GetDelayedDeletes() {
        return m_nDelayedDeletes;
    }
private:
    CDeleter *              m_pDeleter;

    char *                  m_pDirectoryName;
    char *                  m_pFilePath;

    bool                    m_bError;
    bool                    m_bNotificationPending;

    unsigned long           m_nDeleteOperations;
    unsigned long           m_nForceDeleteOperations;
    unsigned long           m_nFileDeleteOperations;
    unsigned long           m_nDirectoryDeleteOperations;
    unsigned long long      m_nDeletedBytes;
    timestamp_t             m_nDelayedDeletes;

    int ScanDirectory(char *a_filepath, int a_depth);
    int ForceDeletePath(char *a_filepath);
    int DeletePath(char *a_filepath);
    void SetError(std::string a_error_string);
    int DiskIOProcessor();

};

#endif // __H_DELETERDISKIOTHR__
