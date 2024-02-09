
#include "strmsrv_compatibility.h"
#include <sstream>

#include "deleterdiskiothr.hh"
#include "deleter.hh"
#include "squeue.hh"
#include "diskiothr.hh"
#include "diskio.hh"
#include "MediaIndexUtils.hh"
#include "common.hh"

using namespace std;

CDeleterDiskIOThread::CDeleterDiskIOThread(CDeleter *a_deleter)
{
    m_priority = 5; // lower than the other threads which are at 10
    m_bDetached = true; // This a non-joinable thread;

    // Set our pointer to the Deleter object and set an extra reference
    // to it
    m_pDeleter = a_deleter;
    m_pDeleter->AddReference();

    if (m_pThreadName != NULL) {
        sprintf(m_pThreadName, "DeleterDiskIO_%d", m_pDeleter->GetId());
    }

    m_bError = false;

    m_pDirectoryName = new char[MAXFILENAMELEN + 1];

    strncpy(m_pDirectoryName, m_pDeleter->GetDirectoryName(), MAXFILENAMELEN + 1);


    // Prefill m_pFilePath buffer for the next ScanDirectory()
    m_pFilePath = new char[MAXFILENAMELEN + 1];
    strcpy(m_pFilePath, m_pDirectoryName);

    m_bNotificationPending = false;
    m_nDeleteOperations = 0;
    m_nForceDeleteOperations = 0;
    m_nFileDeleteOperations = 0;
    m_nDirectoryDeleteOperations = 0;
    m_nDeletedBytes = 0;
    m_nDelayedDeletes = 0;


}

CDeleterDiskIOThread::~CDeleterDiskIOThread()
{
    delete [] m_pDirectoryName;
    delete [] m_pFilePath;
}

// Recursively scan input directory and return the first delete
// candidate which is either a regular file or an empty directory
int
CDeleterDiskIOThread::ScanDirectory(char *a_filepath, int a_depth)
{

    int retval;

    // Open directory
    DIR *dir = opendir(a_filepath);

    if (dir == NULL) {
        // We only tolerate ENOENT error (dir does not exist) since we
        // allow "trash" not to exist. Any other error (e.g. EIO if NAS
        // goes off line) is a good reason to abort
        if (errno != ENOENT) {
            SYSLOG_ERROR("ScanDirectory: Failed to scan dir %s: %s",
                    a_filepath, strerror(errno));
            return -1;
        }
        return 0;
    }

    struct dirent *path;
    while (1) {

        // Find the next file in the directory
        path = readdir(dir);

        // If no more files, input directory is empty. return 1
        if (path == NULL) {
            retval = a_depth;
            break;
        }
        // Skip paths "." and ".."
        if ( (strcmp(path->d_name, ".") == 0) ||
                (strcmp(path->d_name, "..") == 0) )
            continue;


        // Found a path to delete. If path is a file, we can return
        // it. If path is a directory, recurse to find first file in
        // it. If directory is empty, this recursive function will
        // automatically return 1 leaving a_filepath unmodified
        if (path->d_type == DT_REG || path->d_type == DT_LNK) {
            strcat(a_filepath, "/");
            strcat(a_filepath, path->d_name);
            retval = a_depth + 1;
            break;
        }
        else if (path->d_type == DT_DIR) {
            strcat(a_filepath, "/");
            strcat(a_filepath, path->d_name);
            retval = ScanDirectory(a_filepath, a_depth + 1);
            break;
        }
        else {
            // Cannot determine file type. Happens often on Windows based NFS servers
            // Resort to using stat function to determine file type
            strcat(a_filepath, "/");
            strcat(a_filepath, path->d_name);
            SYSLOG_DEBUG("ScanDirectory: Strange file type %d for %s. Trying stat",
                    path->d_type, a_filepath);

            struct stat buf;
            if (lstat(a_filepath, &buf) == -1) {
                SYSLOG_ERROR("ScanDirectory: Cannot determine file type (stat failed) for %s",
                       a_filepath);
                retval = -1;
                break;
            }
            else {
                if (S_ISREG(buf.st_mode)) {
                    retval = a_depth + 1;
                    break;
                }
                else if (S_ISDIR(buf.st_mode)) {
                    retval = ScanDirectory(a_filepath, a_depth + 1);
                    break;
                }
                else {
                    // Some other file type... assume this can be deleted using
                    // regular file delete
                    retval = a_depth + 1;
                    break;
                }
            }
        }
    }

    closedir(dir);
    return retval;
}

int
CDeleterDiskIOThread::ForceDeletePath(char *a_filepath)
{
    char buffer[512];

    SYSLOG_INFO("Deleter: Force Deleting file %s", a_filepath);
    m_nForceDeleteOperations++;

    snprintf(buffer, 512, "/bin/rm -rf %s", a_filepath);
    system(buffer);

    return 0;
}

int
CDeleterDiskIOThread::DeletePath(char *a_filepath)
{

    struct stat buf;

    if (lstat(a_filepath, &buf) == -1) {
        ForceDeletePath(a_filepath);
        return -1;
    }

    // Delete file
    if (S_ISREG(buf.st_mode)) {
        // Delete a file


        SYSLOG_DEBUG("Deleter: Deleting file %s", a_filepath);
        if (unlink(a_filepath) == -1) {
            // Sometimes temporary files are created by the system on NFS servers
            // that we detect during scanning but are no longer there or are busy
            // by the time we get here. So we ignore such errors.
            if (errno != ENOENT && errno != EBUSY) {
                SYSLOG_ERROR("Deleter: Could not delete file %s %s",
                        a_filepath, strerror(errno));
                ostringstream os;
                os << "Could not delete file " << a_filepath << " : " << strerror(errno);
                SetError(os.str());
            }
            else {
                SYSLOG_INFO("Deleter: Ignoring error on file %s due to %s",
                        a_filepath, strerror(errno));
            }
            ForceDeletePath(a_filepath);
        }
        else {
            m_nDeleteOperations++;
            m_nFileDeleteOperations++;
            m_nDeletedBytes += buf.st_size;
        }
    }
    // Delete directory
    else if (S_ISDIR(buf.st_mode)) {
        SYSLOG_DEBUG("Deleter: Deleting directory %s", a_filepath);
        if (rmdir(a_filepath) == -1) {
            // Typically, the directory will always be empty since
            // it is assumed that all things thrown into trash is done
            // using a "mv" command. We try to handle cases where
            // user (developers) have moved directories into trash
            // using for example a "cp -r" command
            if (errno != ENOTEMPTY && errno != ENOENT && errno != EBUSY) {
                SYSLOG_ERROR("Deleter: Could not delete directory %s %s",
                        a_filepath, strerror(errno));
                ostringstream os;
                os << "Could not delete directory " << a_filepath << " : "
                        << strerror(errno);
                SetError(os.str());
            } else {
                SYSLOG_INFO("Deleter: Ignoring error on directory %s due to %s",
                        a_filepath, strerror(errno));
            }
            ForceDeletePath(a_filepath);
        }
        else {
            m_nDeleteOperations++;
            m_nDirectoryDeleteOperations++;
        }
    }
    else {
        // Any other file type we try to delete anyway
        SYSLOG_DEBUG("Deleter: Deleting unexpected type of file %s", a_filepath);
        if (unlink(a_filepath) == -1) {
            if (errno != ENOENT && errno != EBUSY) {
                SYSLOG_ERROR("Deleter: Could not delete unexpected type of file %s %s",
                        a_filepath, strerror(errno));
                ostringstream os;
                os << "Could not delete file " << a_filepath << " : " << strerror(errno);
                SetError(os.str());
            }
            ForceDeletePath(a_filepath);
        }
        else {
            m_nDeleteOperations++;
            m_nFileDeleteOperations++;
            m_nDeletedBytes += buf.st_size;
        }
    }
    return 0;
}

int
CDeleterDiskIOThread::Run()
{

    // Loop till done
    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        if (DiskIOProcessor() == -1) {
            SYSLOG_DEBUG("Thread %s giving up\n", m_pThreadName);
            break;
        }
        LightSleep();

    }

    // Remove our reference to the deleter object so that it can die
    m_pDeleter->RemoveReference();

    // We are responsible to delete ourselves
    delete(this);

    return 0;
}

// This thread can afford to block during file IO operations
int
CDeleterDiskIOThread::DiskIOProcessor()
{

    int rc = 0;

    if (m_bError == true)
        return rc;

    int depth = ScanDirectory(m_pFilePath, 0);


    // Depth > 0 means a delete candidate found
    if (depth > 0) {

        // Simple heuristic for deleting files under heavy IO
        // load: If the last file operation was less than 100 ms
        // ago, delay file deletion by that much amount
        timestamp_t delay =
            CTimeKeeper::Now() - CDiskIOThr::GetFileOpTimestamp();
        if (delay < 100) {
            m_nDelayedDeletes += (100 - delay);
            usleep((100 - delay) * 100);
        }

        // The only incoming messages are delete file/directory
        // requests
        CDiskIOThr::SetFileOpTimestamp(CTimeKeeper::Now());

        // Delete the file/directory found
        DeletePath(m_pFilePath);

        m_bNotificationPending = true;

        // Reset filepath to root directory for next ScanDirectory()
        strcpy(m_pFilePath, m_pDirectoryName);
    }
    else if (depth == -1 ) {
        ostringstream os;
        os << "Error accessing files to delete in " << m_pDirectoryName;
        SetError(os.str());
    }
    else {
        // Idle: Nothing left to delete. If we recently deleted a
        // bunch of stuff, issue a notification
        if (m_bNotificationPending == true) {
            m_bNotificationPending = false;
            m_pDeleter->SetNotification(NOTIFICATION_EVENT_PROGRESS);
        }
    }

    return rc;
}

void
CDeleterDiskIOThread::SetError(string a_pErrorString)
{
    m_pDeleter->SetError();
    m_bError = true;
    m_pDeleter->SendErrorNotificationString(NOTIFICATION_ERROR_FILE_WRITE,
                                      a_pErrorString);
}
