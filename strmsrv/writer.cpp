
#include "strmsrv_compatibility.h"
#include <sstream>

#include "common.hh"
#include "writer.hh"
#include "squeue.hh"
#include "diskio.hh"
#include "writerdiskiothr.hh"
#include "writerdiskioffthread.hh"
#include "writerdiskioinfo.hh"
#include "GenerateUniqueID.h"
#include "FFUtils.hh"
#include "FFUtils.hh"

using namespace std;

unsigned long CWriter::m_nGlobalBufferDrops = 0;

CWriter::CWriter(int nId,
                 char *pMediaName,
                 char *pMediaDir,
                 int nTrimLength,
                 int createFFTracks,
                 int createHLSMedia,
                 char *pStreamType,
                 int createPrimaryThumbnail,
                 int nStreamThumbnailFrequency,
                 char *pSDPFilePath) :
        CStreamer(nId)
{
    m_nPortType = DMSRECORDERPORT;

    m_nFileStartTime = 0;
    m_pMediaName = new char[MAXFILENAMELEN+1];
    m_pMediaDir = new char[MAXFILENAMELEN+1];
    m_pStreamType = new char[MAXFILENAMELEN+1];
    m_pSDPFilePath = new char[MAXFILENAMELEN+1];

    m_nTrimLength = nTrimLength;
    m_bEnableFFTracks = (createFFTracks != 0);
    m_bEnablePrimaryThumbnail = (createPrimaryThumbnail != 0);
    m_nStreamThumbnailsFrequency = nStreamThumbnailFrequency;

    m_bCreateHLSOutput = (createHLSMedia != 0);

    memset(m_pMediaName, 0, MAXFILENAMELEN);
    memset(m_pMediaDir, 0, MAXFILENAMELEN);
    memset(m_pStreamType, 0, MAXFILENAMELEN);
    memset(m_pSDPFilePath, 0, MAXFILENAMELEN);

    sprintf(m_pMediaName, "%s", pMediaName);
    sprintf(m_pMediaDir, "%s", pMediaDir);
    sprintf(m_pStreamType, "%s", pStreamType);
    sprintf(m_pSDPFilePath, "%s", pSDPFilePath);

    if (strcmp(m_pStreamType, "RTP") != 0)
        m_bCreateHLSOutput = false;

    m_bCreateFFTracks = (m_bEnableFFTracks || m_bEnablePrimaryThumbnail
            || m_bCreateHLSOutput || (m_nStreamThumbnailsFrequency != 0));

    m_nLastWriteTime = 0;
    m_nWriteWallClock = 0;

    m_nWriteOffset = 0;
    m_nBufferSize = 750000;

    m_nUniqueId = CGenerateUniqueID::GetID();

    m_nDiskIOQueueLimit = m_nBufferSize * 20;
    if (strstr(m_pMediaDir, "portal/site/NAS") != NULL)
        m_nDiskIOQueueLimit = m_nBufferSize * 20;

    m_sBuffer = new CSBuffer(m_nBufferSize + 1);
    m_sBuffer->AddReference();
    m_pWriteBuffer = m_sBuffer->GetBuffer(0);

    if (m_pWriteBuffer == NULL) { // memory pressure, give up on this object
        SYSLOG_ERROR("Memory Error. Given up on writer object %d", m_nId);
        SendErrorNotification(NOTIFICATION_ERROR_OUT_OF_MEMORY, "Out of Memory");
        SetError();
    }

    m_savedProgressInfo = NULL;
    m_previousProgressInfo = NULL;

    m_ScheduledWriteOperations = 0;

    m_pQueueSizeAverager = new CAverager<unsigned long>();
    m_pDiskIOQueueSizeAverager = new CAverager<unsigned long>();
    m_pChunkSizeAverager = new CAverager<unsigned long>(25);

    m_bytesWritten = 0;
    m_nFramesPerBuffer = 0;
    m_nFramesPerBufferCount = 0;
    m_nBufferDrops = 0;
    m_nConsecutiveBufferDrops = 0;
    m_bDroppingBuffers = false;

    m_nIndexTimestamp = 0;
    m_nIndexOffset = 0;
    m_nDiscontinuityOffset = 0;
    m_nDiscontinuityCount = 0;
    m_nLastIndexOffset = 0;

    // Create diskio queues
    ostringstream qid;
    qid << "WriterDiskIO_" << m_nUniqueId;
    m_qDiskIOSink = CSQueueManager::Instance()->GetQueueSink(qid.str());

    ostringstream altqid;
    altqid << "WriterDiskIO_Alt_" << m_nUniqueId;
    m_qDiskIOSource = CSQueueManager::Instance()->GetQueueSource(altqid.str());
    m_qDiskIOSource->SetOwner(m_nId);

    m_nPendingWrites = 0;
    m_nPendingBytes = 0;

    // Create and start the Writer DiskIO thread
    m_pWriterDiskIOThread = new CWriterDiskIOThread(this);
    m_pWriterDiskIOThread->Start();

    SetupFFThread();
}

CWriter::~CWriter()
{

    delete[] m_pMediaName;
    delete[] m_pMediaDir;
    delete [] m_pStreamType;
    delete m_savedProgressInfo;
    delete m_previousProgressInfo;

    if (m_sBuffer)
        m_sBuffer->RemoveReference();

    if (m_qDiskIOSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qDiskIOSink);
    if (m_qDiskIOSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qDiskIOSource);

    if (m_qDiskIOFFSink != NULL)
        CSQueueManager::Instance()->ReleaseQueueSink(m_qDiskIOFFSink);

    if (m_qQueueSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);

    delete m_pQueueSizeAverager;
    delete m_pDiskIOQueueSizeAverager;
    delete m_pChunkSizeAverager;
}

void
CWriter::ResetGlobalStats()
{
    m_nGlobalBufferDrops = 0;
}

void CWriter::SetupFFThread() {
    // Create and start the Writer DiskIO FF thread
    m_qDiskIOFFSink = NULL;
    m_pWriterDiskIOFFThread = NULL;
    if (m_bCreateFFTracks) {
        // Create FF diskio queues
        ostringstream ffqid;
        ffqid << "WriterDiskIOFF_" << m_nUniqueId;
        m_qDiskIOFFSink = CSQueueManager::Instance()->GetQueueSink(ffqid.str());

        m_pWriterDiskIOFFThread = new CWriterDiskIOFFThread(this, m_bEnableFFTracks,
                                                            m_pStreamType,
               	                                            m_bEnablePrimaryThumbnail,
                                                            m_nStreamThumbnailsFrequency,
                                                            m_pSDPFilePath,
                                                            m_bCreateHLSOutput);
        m_pWriterDiskIOFFThread->Start();
    }
}

void
CWriter::DeleteStream()
{

    // If we have not been "Stopped" yet do it now
    if (IsStarted()) {
        SYSLOG_DEBUG("Writer still running, Calling stop internally.");
        Stop();
    }

    // Send a quit request to the disk io thread
    SYSLOG_DEBUG("Requesting Quit");
    CWriterDiskIOInfo *diskIOInfo =
        new CWriterDiskIOInfo(CWriterDiskIOInfo::Quit, 0);

    CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    pSFrame->SetCustomObject(diskIOInfo);
    m_qDiskIOSink->InsertFrame(pSFrame);

    if (m_bCreateFFTracks) {
        CWriterDiskIOInfo *diskIOInfo =
            new CWriterDiskIOInfo(CWriterDiskIOInfo::Quit, 0);

        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(diskIOInfo);
        m_qDiskIOFFSink->InsertFrame(pSFrame);
    }

    // Remove our own (streamer) reference also. The last reference
    // removed (most likely in diskio thread) will delete this object
    RemoveReference();
}

/// Routine to Empty Buffer to the disk
/// Assign the file names on the disk
int CWriter::FlushBuffer()
{
    CSFrame *pSFrame;
    bool bBufferDropped = false;

    // Compute diskio queue length stats
    m_pDiskIOQueueSizeAverager->Sample(m_nPendingWrites);

    // Are we getting backed up?? If so drop the buffer.

    // If we start dropping buffers we continue to drop consecutive
    // chunks till all pending writes clear up
    if (m_bDroppingBuffers == true && m_nPendingWrites == 0) {
        m_bDroppingBuffers = false;
        if (m_pWriterDiskIOFFThread != NULL)
            m_pWriterDiskIOFFThread->SetDropBuffers(false);
    }

    if (m_nPendingBytes > m_nDiskIOQueueLimit || m_bDroppingBuffers == true) {
        m_errors++;
        m_nBufferDrops++;
        m_nGlobalBufferDrops++;
        bBufferDropped = true;
        if (m_bDroppingBuffers == false) {
            SYSLOG_DEBUG("%d: Buffer #%ld drops at %d %ld of size %d \n",
                         m_nId, m_nBufferDrops, m_nPendingWrites,
                         m_nPendingBytes, m_nWriteOffset);
        }
        if (!m_bDroppingBuffers) {
            m_nConsecutiveBufferDrops = 0;
            m_bDroppingBuffers = true;
            if (m_pWriterDiskIOFFThread != NULL)
                m_pWriterDiskIOFFThread->SetDropBuffers(true);
        }
        m_nConsecutiveBufferDrops++;
    }
    else {
        // Schedule a write to the DiskIO thread
        CWriterDiskIOInfo *diskIOInfo =
            new CWriterDiskIOInfo(CWriterDiskIOInfo::Write, 0);

        pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(diskIOInfo);
        pSFrame->SetData(m_sBuffer, 0, m_nWriteOffset);
        m_pChunkSizeAverager->Sample((unsigned long)m_nWriteOffset);

        {
            CScopedLock lock(m_DiskIOLock);
            m_nPendingWrites++;
            m_nPendingBytes += m_nWriteOffset;
        }
        m_qDiskIOSink->InsertFrame(pSFrame);

        m_ScheduledWriteOperations++;
        m_bytesWritten += m_nWriteOffset;


        // Exchange old m_sBuffer for new
        m_sBuffer->RemoveReference();
        m_sBuffer = new CSBuffer(m_nBufferSize + 1);
        m_sBuffer->AddReference();
        m_pWriteBuffer = m_sBuffer->GetBuffer(0);
        if (m_pWriteBuffer == NULL) { // memory pressure, give up on this object
            SYSLOG_ERROR("Memory Error. Given up on writer object %d", m_nId);
            SendErrorNotification(NOTIFICATION_ERROR_OUT_OF_MEMORY, "Out of Memory");
            SetError();
        }
    }


    m_nLastWriteTime = m_nTimestamp;
    m_nWriteWallClock = CTimeKeeper::Now();

    // Adjust our index into the media. If buffer dropped, the
    // index offsets need to adjust
    if (!bBufferDropped)
        m_nIndexOffset += m_nWriteOffset;

    m_nWriteOffset = 0;
    m_nFramesPerBuffer = m_nFramesPerBufferCount;
    m_nFramesPerBufferCount = 0;

    // Also write out the index file
    if (m_nIndexTimestamp != 0) {
        char buf[64];
        sprintf(buf, "%"PRIu64" %lu\n", m_nIndexTimestamp, m_nLastIndexOffset);

        if (bBufferDropped == false) {

            CWriterDiskIOInfo *diskIOInfo =
                new CWriterDiskIOInfo(CWriterDiskIOInfo::WriteIndex, 0);

            pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
            pSFrame->SetCustomObject(diskIOInfo);
            int bufferLen = strlen(buf);
            pSFrame->SetData(buf, bufferLen);

            {
                CScopedLock lock(m_DiskIOLock);
                m_nPendingWrites++;
                m_nPendingBytes += bufferLen;
            }
            m_qDiskIOSink->InsertFrame(pSFrame);

            m_ScheduledWriteOperations++;
        }

        m_nIndexTimestamp = 0;
    }
    return 0;
}

int CWriter::CheckFile()
{

    ASSERT(m_nTimestamp != 0);

    timestamp_t nTimestamp = GetCurrentTimeStamp();
    bool bRequestNext = false;

    // We are done with this minute file if we have written upto the
    // the remaining length of the file
    if ((m_nFileStartTime > 0) && (nTimestamp > m_nFileStartTime) &&
            ((nTimestamp - m_nFileStartTime) >=
             m_pWriterDiskIOThread->GetRemainingLength()) ) {

        FlushBuffer();
        bRequestNext = true;
        m_nFileStartTime = 0;
    }
    else if (m_nFileStartTime == 0) {

        bRequestNext = true;
    }

    if (bRequestNext) {
        SYSLOG_DEBUG("Requesting GetNext");
        CWriterDiskIOInfo *diskIOInfo =
            new CWriterDiskIOInfo(CWriterDiskIOInfo::GetNext, nTimestamp);

        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(diskIOInfo);
        m_qDiskIOSink->InsertFrame(pSFrame);

        // Starting to write to a new file so reset these
        m_nIndexTimestamp = 0;
        m_nIndexOffset = 0;

        // Set FileStartTime to current time and use
        m_nFileStartTime = nTimestamp;

    }

    return 0;
}


void
CWriter::StartMedia(timestamp_t nFirstTimeStamp)
{
    if (nFirstTimeStamp == 0) // Not likely but just in case
        nFirstTimeStamp = CTimeKeeper::Now();

    m_nTimestamp = nFirstTimeStamp;
    m_nStartTimestamp = m_nTimestamp;
    m_nStartWallClock = CTimeKeeper::Now();
    m_nFrameWallClock = CTimeKeeper::Now();
    m_bSendNotify = true;
    m_nEventId = NOTIFICATION_EVENT_BEGIN;
    ASSERT(m_nTimestamp != 0);

    if (IsErrored())
        return;

    CWriterDiskIOInfo *diskIOInfo =
        new CWriterDiskIOInfo(CWriterDiskIOInfo::Start, m_nTimestamp);

    CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    pSFrame->SetCustomObject(diskIOInfo);

    SYSLOG_DEBUG("Requesting Start");
    m_qDiskIOSink->InsertFrame(pSFrame);

    if (m_bCreateFFTracks) {
        CWriterDiskIOInfo *diskIOInfo =
                new CWriterDiskIOInfo(CWriterDiskIOInfo::Start, m_nTimestamp);

        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(diskIOInfo);
        m_qDiskIOFFSink->InsertFrame(pSFrame);
    }

    CheckFile();
}


int
CWriter::ProcessStream()
{

    ProcessAltStream();

    // Quit if no queue defined yet (writer not "started")
    if (m_qQueueSource == NULL)
        return 0;


    // Check and process a frame in our queue
    CSFrame * sFrame = m_qQueueSource->GetFrame();

    if (sFrame) {

        m_pQueueSizeAverager->Sample(m_qQueueSource->Size());

        if (m_nTimestamp == 0) {
            StartMedia(sFrame->GetTimeStamp() - m_nDiscontinuityOffset);
        }
        else if (sFrame->HasDiscontinuity()) {
            // offset = offset + new_ts - (ts + offset) = new_ts - ts
            m_nDiscontinuityOffset = sFrame->GetTimeStamp() - m_nTimestamp;
            m_nDiscontinuityCount++;
        }
        m_nTimestamp = sFrame->GetTimeStamp() - m_nDiscontinuityOffset;
        m_nFrameWallClock = CTimeKeeper::Now();


        // Check if we have to move on to the next file
        if (IsErrored()) {
            // Stop writing once we detect error
        }
        else if (sFrame->GetMessageType() == CSFrame::Normal) {
            CheckFile();
            // Write the frame header
            hdr_t hdr;
            hdr.h_nFlags = sFrame->GetMediaFlag();
            hdr.h_nTimestamp = m_nTimestamp;
            hdr.h_nLen   = sFrame->GetLength();
            if ((int)hdr.h_nLen + (int)sizeof(hdr) + m_nWriteOffset >
                    m_nBufferSize) {
                FlushBuffer();
            }
            if (m_nTimestamp - m_nLastWriteTime > 1000) {
                FlushBuffer();
            }


            // Set the timestamp and file offset of the first frame going
            // out in the next chunk
            if (m_nIndexTimestamp == 0) {
                m_nIndexTimestamp = m_nTimestamp;
                m_nLastIndexOffset = m_nIndexOffset;
            }
            if (m_pWriteBuffer != NULL) {
                memcpy(m_pWriteBuffer + m_nWriteOffset,
                        (void *)&hdr, sizeof (hdr_t));
                m_nWriteOffset += sizeof(hdr_t);

                memcpy(m_pWriteBuffer + m_nWriteOffset,
                        (void *)sFrame->GetData(),
                        sFrame->GetLength());
                m_nWriteOffset += hdr.h_nLen;
                m_nFramesPerBufferCount++;
            }
        }
        else {
            CheckFile();
        }

        if (sFrame->GetMessageType() == CSFrame::Normal) {
            m_frameCount++;
            m_bytesTransfered += sFrame->GetLength();
        }

        if (m_bCreateFFTracks && sFrame->GetMessageType() == CSFrame::Normal) {
            CSFrame *newSFrame =
                new CSFrame(sFrame->GetData(),
                            sFrame->GetLength(),
                            sFrame->GetMediaFlag(),
                            sFrame->GetTimeStamp());

            m_qDiskIOFFSink->InsertFrame(newSFrame);
        }

        m_qQueueSource->ReleaseFrame(sFrame);

        return 1;
    }
    else {
        // Here we handle all the activity needed to be done even if
        // no packets are coming in to the Writer module

        // If we have not seen the first packet yet, find the start
        // time from the queue source itself
        if (m_nTimestamp == 0) {
            CSQueueSink * qSink = m_qQueueSource->GetSink();
            if (qSink->GetLastFrameTimestamp() != 0) {
                timestamp_t currentTimeStamp =
                    qSink->GetLastFrameTimestamp() +
                    CTimeKeeper::Now() - qSink->GetLastFrameWallClock();

                StartMedia(currentTimeStamp);
            }
        }

        else if ((CTimeKeeper::Now() - m_nWriteWallClock) > 1000) {
            // If no packets have come in since the last FlushBuffer, we
            // still need to Flush out the last second of data
            if (!IsErrored()) {
                CheckFile();
                FlushBuffer();
            }
        }
    }

    return 0;
}

int CWriter::ProcessAltStream()
{
    CSFrame * sFrame = m_qDiskIOSource->GetFrame();

    if (sFrame) {

        CWriterDiskIOInfo *diskIOInfo =
            static_cast<CWriterDiskIOInfo *> (sFrame->GetCustomObject());

        switch (diskIOInfo->GetMessageType()) {
        case CWriterDiskIOInfo::Progress:
        {
            CWriterDiskIOProgress * progressInfo = dynamic_cast<CWriterDiskIOProgress *>(diskIOInfo);
            delete m_savedProgressInfo;
            m_savedProgressInfo = new CWriterDiskIOProgress(progressInfo);
            if (m_bCreateFFTracks) {
                m_savedProgressInfo->AdjustByteCounts(
                        m_pWriterDiskIOFFThread->GetBytesWritten(),
                        m_pWriterDiskIOFFThread->GetBytesDeleted());
            }
            if (m_bSendNotify != true) {
                m_bSendNotify = true;
                m_nEventId = NOTIFICATION_EVENT_PROGRESS;
            }
            break;
        }
        default:
            // Not expecting any other type
            break;
        }

        delete (diskIOInfo);
        m_qDiskIOSource->ReleaseFrame(sFrame);
    }
    return 0;
}

int
CWriter::StartStream(int nQueueId)
{
    ASSERT(m_qQueueSource == NULL);
    ostringstream qid;
    qid << "Queue_" << nQueueId;
    m_qQueueSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qQueueSource->SetOwner(m_nId);

    // Increase the maximum size of the source queue
    m_qQueueSource->SetMaxSize(50000);
    m_qQueueSource->SetMaxBytes(20 * 1024 * 1024);
    return 0;
};

/// Remove the Queue Assignment
int
CWriter::StopStream()
{
    // First write out whatever is collected so far
    FlushBuffer();

    timestamp_t nStopTimestamp = GetCurrentTimeStamp();
    if (nStopTimestamp == 0) {
        nStopTimestamp = CTimeKeeper::Now();
        if (m_nTimestamp == 0)
            StartMedia(CTimeKeeper::Now());
    }
    CWriterDiskIOInfo *diskIOInfo =
        new CWriterDiskIOInfo(CWriterDiskIOInfo::Stop, nStopTimestamp);

    CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
    pSFrame->SetCustomObject(diskIOInfo);
    SYSLOG_DEBUG("Requesting Stop");
    m_qDiskIOSink->InsertFrame(pSFrame);

    if (m_bCreateFFTracks) {
        CWriterDiskIOInfo *diskIOInfo =
            new CWriterDiskIOInfo(CWriterDiskIOInfo::Stop, nStopTimestamp);

        CSFrame *pSFrame = new CSFrame(CTimeKeeper::Now(), CSFrame::Custom);
        pSFrame->SetCustomObject(diskIOInfo);
        SYSLOG_DEBUG("Requesting Stop");
        m_qDiskIOFFSink->InsertFrame(pSFrame);
    }

    if (m_qQueueSource != NULL) {
        CSQueueManager::Instance()->ReleaseQueueSource(m_qQueueSource);
        m_qQueueSource = NULL;
    }

    return 0;
}

void
CWriter::DecrementPendingWrite(int a_nLength)
{
    CScopedLock lock(m_DiskIOLock);
    m_nPendingWrites--;
    m_nPendingBytes -= a_nLength;
}


void
CWriter::PrintStatistics(string &a_stats)
{
    m_pQueueSizeAverager->ResetStats();
    m_pDiskIOQueueSizeAverager->ResetStats();
    m_pWriterDiskIOThread->ResetLatencyStats();

}

void
CWriter::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectGuageParam("TrimLength", m_nTrimLength);
    a_pCmd->SetObjectParam("HLSOutput", m_bCreateHLSOutput);
    a_pCmd->SetObjectParam("FastForwardTracks", m_bEnableFFTracks);
    a_pCmd->SetObjectParam("PrimaryThumbnail", m_bEnablePrimaryThumbnail);
    a_pCmd->SetObjectParam("StreamThumbnailFreq", m_nStreamThumbnailsFrequency);

    a_pCmd->SetObjectCounterParam("SchedWriteOperations", m_ScheduledWriteOperations);
    a_pCmd->SetObjectCounterParam("WriteOperations",
                                  m_pWriterDiskIOThread->GetWriteOperations());
    if (a_pCmd->StatusLevel(3)) {
        a_pCmd->SetObjectGuageParam("MinQueueSize", m_pQueueSizeAverager->GetMinValue());
        a_pCmd->SetObjectGuageParam("MaxQueueSize", m_pQueueSizeAverager->GetMaxValue());
    }
    a_pCmd->SetObjectGuageParam("DiskIOQueueLimit", m_nDiskIOQueueLimit);
    a_pCmd->SetObjectGuageParam("DiskIOQueueSize", m_nPendingWrites);
    a_pCmd->SetObjectGuageParam("DiskIOQueueBytes", m_nPendingBytes);
    if (a_pCmd->StatusLevel(4)) {
        a_pCmd->SetObjectParam("AvgChunkSize",
                (double)m_pChunkSizeAverager->GetExponentialAverage());
        a_pCmd->SetObjectParam("ConsecutiveBufferDrops", m_nConsecutiveBufferDrops);
    }
    if (a_pCmd->StatusLevel(4)) {
        a_pCmd->SetObjectGuageParam("MinDiskIOQueueSize",
                m_pDiskIOQueueSizeAverager->GetMinValue());
    }

    a_pCmd->SetObjectGuageParam("MaxDiskIOQueueSize",
            m_pDiskIOQueueSizeAverager->GetMaxValue());
    if (a_pCmd->StatusLevel(2)) {
        a_pCmd->SetObjectGuageParam("MaxSoFarDiskIOQueueSize",
                m_pDiskIOQueueSizeAverager->GetMaxSoFar());
    }
    a_pCmd->SetObjectParam("AvgDiskIOQueueSize",
            (double)m_pDiskIOQueueSizeAverager->GetExponentialAverage());

#ifdef SSDEBUG
    a_pCmd->SetObjectParam("AVGDiskIOQueueSize",
                           (double)m_pWriterDiskIOThread->GetAverageQueueSize());
#endif
    a_pCmd->SetObjectParam("AvgWriteLatency",
            (double)m_pWriterDiskIOThread->GetMovingAvgWriteLatency());
    if (a_pCmd->StatusLevel(2)) {
        a_pCmd->SetObjectParam("ExpAvgWriteLatency",
                (double)m_pWriterDiskIOThread->GetExpAvgWriteLatency());
    }
    if (a_pCmd->StatusLevel(3)) {
        a_pCmd->SetObjectGuageParam("MinWriteLatency",
                m_pWriterDiskIOThread->GetMinWriteLatency());
        a_pCmd->SetObjectGuageParam("MaxWriteLatency",
                m_pWriterDiskIOThread->GetMaxWriteLatency());
        a_pCmd->SetObjectGuageParam("MedianWriteLatency",
                m_pWriterDiskIOThread->GetWriteLatencyPercentile(50));
        a_pCmd->SetObjectGuageParam("WriteLatency",
                m_pWriterDiskIOThread->GetWriteLatency());
    }
    if (a_pCmd->StatusLevel(4)) {
        for (int p = 10; p < 100; p += 10) {
            char label[32];
            sprintf(label, "WriteLatencyP%d", p);
            a_pCmd->SetObjectGuageParam(label,
                    m_pWriterDiskIOThread->GetWriteLatencyPercentile(p));
        }
    }
    a_pCmd->SetObjectErrorCounterParam("BufferDrops", m_nBufferDrops);
    a_pCmd->SetObjectErrorCounterParam("GlobalBufferDrops", m_nGlobalBufferDrops);

    a_pCmd->SetObjectByteCounterParam("BytesWritten", m_bytesWritten);
    a_pCmd->SetObjectByteCounterParam("BytesDeleted", m_bytesWritten);
    if (m_pWriterDiskIOFFThread != NULL) {
        a_pCmd->SetObjectByteCounterParam("HLSBytesWritten",
                m_pWriterDiskIOFFThread->GetHLSBytesWritten());
        a_pCmd->SetObjectByteCounterParam("HLSBytesDeleted",
                m_pWriterDiskIOFFThread->GetHLSBytesDeleted());
    }
    a_pCmd->SetObjectParam("MediaDirectory", m_pMediaDir);
    a_pCmd->SetObjectParam("MediaName", m_pMediaName);
    a_pCmd->SetObjectParam("FileName",
                           m_pWriterDiskIOThread->GetFileBaseName());
    a_pCmd->SetObjectParam("FileIndex",
                           m_pWriterDiskIOThread->GetFileIndex());

    if (a_pCmd->StatusLevel(0)) {
        if (m_pWriterDiskIOFFThread != NULL) {
            a_pCmd->SetObjectCounterParam("FFFramesProcessed",
                    m_pWriterDiskIOFFThread->GetFramesProcessed());
            a_pCmd->SetObjectErrorCounterParam("FFBufferDrops",
                        m_pWriterDiskIOFFThread->GetBuffersDropped());
            a_pCmd->SetObjectErrorCounterParam("FFFrameDrops",
                        m_pWriterDiskIOFFThread->GetFramesDropped());
        }
    }

    if (a_pCmd->StatusLevel(3)) {
        a_pCmd->SetObjectParam("FileStartTimestamp",
                               (timestamp_t) m_nFileStartTime);
        a_pCmd->SetObjectParam("DiscoOffset", (int)m_nDiscontinuityOffset);
        a_pCmd->SetObjectGuageParam("DiscoCount", (int)m_nDiscontinuityCount);
    }
    a_pCmd->SetObjectByterateGuageParam("FramesPerBuffer", m_nFramesPerBuffer);

    if (a_pCmd->StatusLevel(1)) {
        if (m_pWriterDiskIOFFThread != NULL) {
    	    a_pCmd->SetObjectByteCounterParam("HLSVideoDropCount",
    	                m_pWriterDiskIOFFThread->GetHLSVideoDropCount());
            a_pCmd->SetObjectByteCounterParam("HLSAudioDropCount",
    	                m_pWriterDiskIOFFThread->GetHLSAudioDropCount());
        }
    }

#ifdef SSDEBUG
    if (m_qQueueSource != NULL) {
        a_pCmd->SetObjectCounterParam("SourceContention",
                                      m_qQueueSource->GetContention());
        a_pCmd->SetObjectParam("AverageQueueLen",
                               (double)m_qQueueSource->AvgSize());
    }
#endif
}

void
CWriter::PrintError(CNotify *a_notify)
{

}

void
CWriter::PrintNotification(CNotify *a_notify)
{
    switch (m_nEventId) {
    case NOTIFICATION_EVENT_PROGRESS:
        a_notify->SetNotificationData("MediaDirectory", m_pMediaDir);
        a_notify->SetNotificationData("MediaName", m_pMediaName);
        if (m_savedProgressInfo != NULL) {
            a_notify->SetNotificationData("BytesWritten", m_savedProgressInfo->GetBytesWritten());
            a_notify->SetNotificationData("BytesDeleted", m_savedProgressInfo->GetBytesDeleted());
            if (m_previousProgressInfo != NULL) {
                a_notify->SetNotificationData("BytesWrittenDelta",
                        m_savedProgressInfo->GetBytesWritten() - m_previousProgressInfo->GetBytesWritten());
                a_notify->SetNotificationData("BytesDeletedDelta",
                        m_savedProgressInfo->GetBytesDeleted() - m_previousProgressInfo->GetBytesDeleted());
            }
            else {
                a_notify->SetNotificationData("BytesWrittenDelta",
                        m_savedProgressInfo->GetBytesWritten());
                a_notify->SetNotificationData("BytesDeletedDelta",
                        m_savedProgressInfo->GetBytesDeleted());
            }
            a_notify->SetNotificationData("PlayLength", m_savedProgressInfo->GetPlayLength());
            if (m_savedProgressInfo->GetInProgress()) {
                a_notify->SetNotificationData("MediaStatus", "Started");
            }
            else {
                a_notify->SetNotificationData("MediaStatus", "Stopped");
            }
            delete m_previousProgressInfo;
            m_previousProgressInfo = new CWriterDiskIOProgress(m_savedProgressInfo);

        }
        break;
    case NOTIFICATION_EVENT_END:
    case NOTIFICATION_EVENT_BEGIN:
        a_notify->SetNotificationData("MediaDirectory", m_pMediaDir);
        a_notify->SetNotificationData("MediaName", m_pMediaName);
        break;
    default:
        break;
    }
}
