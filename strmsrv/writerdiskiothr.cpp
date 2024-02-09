#include "strmsrv_compatibility.h"
#include <sstream>

#include "writerdiskiothr.hh"
#include "writerdiskioinfo.hh"
#include "writer.hh"
#include "squeue.hh"
#include "diskio.hh"
#include "MediaIndexUtils.hh"
#include "common.hh"

using namespace std;
//using namespace mediaindex;

CWriterDiskIOThread::CWriterDiskIOThread(CWriter *a_writer)
{
    m_priority = 5; // lower than the other threads which are at 10
    m_bDetached = true; // This a non-joinable thread;

    // Set our pointer to the Writer object and set an extra reference
    // to it
    m_pWriter = a_writer;
    m_pWriter->AddReference();

    if (m_pThreadName != NULL) {
        sprintf(m_pThreadName, "WriterDiskIOFF_%d_%lu",
                m_pWriter->GetId(),
                (unsigned long)m_pWriter->GetUniqueId());
    }
    m_bError = false;

    m_pDestinationIndex = NULL;
    m_nFileFD = 0;
    m_nIndexFileFD = 0;

    m_pFileName = new char[MAXFILENAMELEN+1];
    m_pIndexFileName = new char[MAXFILENAMELEN+1];

    memset(m_pFileName, 0, MAXFILENAMELEN);
    memset(m_pIndexFileName, 0, MAXFILENAMELEN);
    m_pFileBaseName = m_pFileName;

    m_nFileRemainingLength = 60000;

    // For a NAS drive do a 'sync' on every write
    m_nSync = 0;
#ifndef WINDOWS
    if (strstr(m_pWriter->GetMediaDir(), "portal/site/NAS") != NULL)
        m_nSync = O_SYNC;
#endif

    m_nWriteOperations = 0;
    m_nBytesWritten = 0;

    m_nFileIndex = -1;

    m_nWriteLatency = 0;
    m_pLatencyAverager = new CAverager<unsigned long>();

    ostringstream qid;
    qid << "WriterDiskIO_" << m_pWriter->GetUniqueId();
    m_qDiskIOSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qDiskIOSource->SetOwner(m_pWriter->GetUniqueId());
    m_qDiskIOSource->SetMaxBytes(20 * 1024 * 1024);

    ostringstream altqid;
    altqid << "WriterDiskIO_Alt_" << m_pWriter->GetUniqueId();
    m_qDiskIOSink = CSQueueManager::Instance()->GetQueueSink(altqid.str());
}

CWriterDiskIOThread::~CWriterDiskIOThread()
{
    if (m_nFileFD > 0)
        close(m_nFileFD);
    if (m_nIndexFileFD > 0)
        close(m_nIndexFileFD);

    if (m_pDestinationIndex)
        delete(m_pDestinationIndex);

    if (m_qDiskIOSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qDiskIOSource);
    if (m_qDiskIOSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qDiskIOSink);

    delete[] m_pFileName;
    delete[] m_pIndexFileName;
    delete m_pLatencyAverager;

}

void CWriterDiskIOThread::CloseFiles() {

    if (m_nFileFD > 0) {
        SYSLOG_DEBUG("Closing file %s: %"PRIu64" fd=%d\n",
                     m_pFileName, m_nWriteOperations, m_nFileFD);
        close(m_nFileFD);
        SYSLOG_DEBUG("Closed file %s\n",
                     m_pFileName);
        m_nFileFD = 0;
    }
    if (m_nIndexFileFD > 0) {
        SYSLOG_DEBUG("Closing Index file %s: %"PRIu64" fd=%d\n",
                     m_pIndexFileName, m_nWriteOperations, m_nIndexFileFD);
        close(m_nIndexFileFD);
        SYSLOG_DEBUG("Closed file %s\n",
                     m_pIndexFileName);
        m_nIndexFileFD = 0;
    }

}

int CWriterDiskIOThread::GetNextFile(timestamp_t a_nTimestamp)
{
    unsigned long long  offset, length;
    char *pFileName = NULL;

    // Close current file handles
    CloseFiles();

    if (m_pDestinationIndex == NULL)
        return 0;

    m_pDestinationIndex->getNext(a_nTimestamp, &pFileName, &offset, &length);

    if (pFileName == NULL)
        return 0;

    SYSLOG_DEBUG("Opening file %s written %"PRIu64" offset=%"PRIu64" length=%"PRIu64"\n",
                 pFileName, m_nBytesWritten, offset, length);
    m_nFileFD = open(pFileName,
                     O_CREAT | O_APPEND | O_WRONLY | m_nSync, 00666);
    if (m_nFileFD > 0) {


        // Use RemainingLength to record how many more 'ticks' we
        // will be writing to this minute file
        m_nFileRemainingLength = length;

        sprintf(m_pFileName, pFileName);
        sprintf(m_pIndexFileName, "%s.idx", pFileName);

        // Open index file too
        m_nIndexFileFD = open(m_pIndexFileName,
                              O_CREAT | O_APPEND | O_WRONLY | m_nSync, 00666);
        if (m_nIndexFileFD <= 0) {
            m_nIndexFileFD = 0;
        }

        // set m_pFileBaseName to string after "blob"
        m_pFileBaseName = strrchr(m_pFileName, '/');
        m_nFileIndex = atoi(m_pFileBaseName + 1);
        while (m_pFileBaseName > m_pFileName) {
            m_pFileBaseName--;
            if (*m_pFileBaseName == '/')
                break;
        }
    }
    else {
        m_bError = true;
        m_nFileFD = 0;
        SYSLOG_ERROR("Could not open file %s\n", pFileName);
        m_pWriter->SetError();

        ostringstream os;
        os << "Could not open file " << pFileName;
        m_pWriter->SendErrorNotification(NOTIFICATION_ERROR_FILE_OPEN, os);
        CloseFiles();

    }
    free(pFileName);

    return 0;
}

int
CWriterDiskIOThread::DiskIOInit()
{

    // Create Destination Index
    SYSLOG_DEBUG("Creating destination index for %s, %s",
                 m_pWriter->GetMediaDir(), m_pWriter->GetMediaName());
    m_pDestinationIndex = mediaindex::createDestIndex(m_pWriter->GetMediaName(),
                                          m_pWriter->GetMediaDir(),
                                          m_pWriter->GetTrimLength());

    if (m_pDestinationIndex == NULL) {
        m_bError = true;
        SYSLOG_ERROR("Could not create destination index for %s, %s",
                     m_pWriter->GetMediaDir(), m_pWriter->GetMediaName());

        CloseFiles();
    }
    else {
        SYSLOG_DEBUG("Done creating destination index for %s, %s",
                     m_pWriter->GetMediaDir(), m_pWriter->GetMediaName());
    }

    return 0;
}

int
CWriterDiskIOThread::Run()
{

    // Init
    DiskIOInit();

    // Loop till done
    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time

        if (DiskIOProcessor() == -1) {
            SYSLOG_DEBUG("Thread %s giving up\n", m_pThreadName);
            break;
        }
        LightSleep();

    }

    // Remove our reference to the writer object so that it can die
    m_pWriter->RemoveReference();

    // We are responsible to delete ourselves
    delete(this);

    return 0;
}

// This thread can afford to block during file IO operations
int
CWriterDiskIOThread::DiskIOProcessor()
{

    int rc = 0;

    // Check and process a frame in our queue
    CSFrame * sFrame = m_qDiskIOSource->GetFrame();

    if (sFrame == NULL) {
        return rc;
    }

    if (sFrame->GetMessageType() != CSFrame::Custom) {
        // Not expecting any other frame type
        ASSERT(0);
        m_qDiskIOSource->ReleaseFrame(sFrame);
        return rc;
    }

    CWriterDiskIOInfo *diskIOInfo =
        static_cast<CWriterDiskIOInfo *>(sFrame->GetCustomObject());

    timestamp_t nTimestamp = diskIOInfo->GetTimeStamp();
    switch (diskIOInfo->GetMessageType()) {

    case CWriterDiskIOInfo::Write:
    case CWriterDiskIOInfo::WriteIndex:
    {
        int fd = m_nFileFD;
        if (diskIOInfo->GetMessageType() == CWriterDiskIOInfo::Write) {
            fd = m_nFileFD;
            SYSLOG_DEBUG("Writing to file %s: %"PRIu64" fd=%d\n",
                         m_pFileName, m_nWriteOperations, fd);
        }
        else {
            fd = m_nIndexFileFD;
            SYSLOG_DEBUG("Writing to file %s: %"PRIu64" fd=%d\n",
                         m_pIndexFileName, m_nWriteOperations, fd);
        }

        CDiskIOThr::SetFileOpTimestamp(CTimeKeeper::Now());

        if (fd > 0 && !m_bError) {
            timestamp_t before, after;
            before = CTimeKeeper::GetTimeInMicroSec();
            int rc  = write(fd, sFrame->GetData(), sFrame->GetLength());
            after = CTimeKeeper::GetTimeInMicroSec();
            m_nWriteLatency = after - before;
            m_pLatencyAverager->Sample(m_nWriteLatency);

            m_pWriter->DecrementPendingWrite(sFrame->GetLength());
            m_nBytesWritten += sFrame->GetLength();
            if (rc < 0) {

                m_bError = true;
                SYSLOG_ERROR("Could not write to file %s errno = %s: %"PRIu64"\n",
                             m_pFileName, strerror(errno), m_nWriteOperations);
                m_pWriter->SetError();

                ostringstream os;
                os << "Could not write to file " << m_pFileName;
                m_pWriter->SendErrorNotification(NOTIFICATION_ERROR_FILE_WRITE, os);
                CloseFiles();
            }
        }
        m_nWriteOperations++;
        break;
    }
    case CWriterDiskIOInfo::Start:
        SYSLOG_DEBUG("Starting DestIndex %"PRIu64"\n",
                     nTimestamp);
        if (m_pDestinationIndex != NULL && !m_bError) {
            m_pDestinationIndex->start(nTimestamp, nTimestamp);
        }
        break;

    case CWriterDiskIOInfo::Stop:
        SYSLOG_DEBUG("Stopping DestIndex %"PRIu64"\n",
                     nTimestamp);
        if (m_pDestinationIndex != NULL && !m_bError) {
            m_pDestinationIndex->stop(nTimestamp);
        }

        SendProgressMessage();

        break;

    case CWriterDiskIOInfo::GetNext:
        SYSLOG_DEBUG("Getting Next DestIndex %"PRIu64"\n",
                     nTimestamp);
        if (!m_bError)
            GetNextFile(nTimestamp);

        SendProgressMessage();

        break;

    case CWriterDiskIOInfo::Quit:
        SYSLOG_DEBUG("Handling Quit request");
        rc = -1;
        break;

    case CWriterDiskIOInfo::Progress:
        break;

    }  // end of switch

    delete (diskIOInfo);


    m_qDiskIOSource->ReleaseFrame(sFrame);

    return rc;
}

void
CWriterDiskIOThread::SendProgressMessage()
{
    if (m_pDestinationIndex == NULL)
        return;

    timestamp_t endTimeCode;
    if (m_pDestinationIndex->getEndTimecode(&endTimeCode) != INDEX_OK)
        return;

    uint64_t playLength;
    if (m_pDestinationIndex->getOffsetFromTimecode(endTimeCode, &playLength) != INDEX_OK)
        return;

    bool inProgress = false;
    if (m_pDestinationIndex->getProgress(inProgress) != INDEX_OK)
        return;

    CWriterDiskIOProgress *progressInfo =
            new CWriterDiskIOProgress(playLength, m_nBytesWritten,
                    m_pDestinationIndex->getBytesDeleted(), inProgress, CTimeKeeper::Now());

    CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    pSFrame->SetCustomObject(progressInfo);
    m_qDiskIOSink->InsertFrame(pSFrame);
}

double
CWriterDiskIOThread::GetAverageQueueSize() {
    return m_qDiskIOSource->AvgSize();
}
