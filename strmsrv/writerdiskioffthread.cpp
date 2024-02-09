/*
 * writerdiskioffthread.cpp
 *
 *  Created on: Aug 17, 2012
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"
#include <sstream>

#include "writerdiskioffthread.hh"
#include "writerdiskioinfo.hh"
#include "writer.hh"
#include "FFMediaProcessor.hh"
#include "AdaptiveMediaGenerator.hh"
#include "squeue.hh"
#include "common.hh"

using namespace std;

CWriterDiskIOFFThread::CWriterDiskIOFFThread(CWriter *a_writer, bool a_bCreateFFTracks,
                                             std::string a_strStreamType,
		                                     bool a_bCreatePrimaryThumbnail,
		                                     int a_nStreamThumbnailsFrequency,
		                                     std::string a_strSDPFilePath,
		                                     bool a_bCreateHLSOutput)
: m_bCreateFFTracks(a_bCreateFFTracks)
, m_strStreamType(a_strStreamType)
, m_bCreatePrimaryThumbnail(a_bCreatePrimaryThumbnail)
, m_nStreamThumbnailsFrequency(a_nStreamThumbnailsFrequency)
, m_strSDPFilePath(a_strSDPFilePath)
, m_bCreateHLSOutput(a_bCreateHLSOutput)
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
    m_nFramesProcessed = 0;
    m_pFFMediaProcessor = NULL;
    m_pAdaptiveMediaGenerator = NULL;

    ostringstream qid;
    qid << "WriterDiskIOFF_" << m_pWriter->GetUniqueId();
    m_qDiskIOSource = CSQueueManager::Instance()->GetQueueSource(qid.str());
    m_qDiskIOSource->SetOwner(m_pWriter->GetUniqueId());
    m_qDiskIOSource->SetMaxSize(50000);
    m_qDiskIOSource->SetMaxBytes(20 * 1024 * 1024);
}

CWriterDiskIOFFThread::~CWriterDiskIOFFThread()
{
    SYSLOG_DEBUG("FF thread object dying");

    delete m_pFFMediaProcessor;

    delete m_pAdaptiveMediaGenerator;

    if (m_qDiskIOSource != NULL)
        CSQueueManager::Instance()->ReleaseQueueSource(m_qDiskIOSource);
}

int CWriterDiskIOFFThread::Run()
{
    DiskIOInit();
    SetupHLSOutput();

    while (!m_bExit) {

        CTimeKeeper::UpdateTime(); // update latest time
        int rc = DiskIOProcessor();
        if (rc < 0) {
            // Quit signal received
            break;
        }
        if (rc == 0)
            LightSleep();
    }

    // Remove our reference to the writer object so that it can die
    m_pWriter->RemoveReference();

    // We are responsible to delete ourselves
    delete(this);

    return 0;
}

// This thread can afford to block during file IO operations
int CWriterDiskIOFFThread::DiskIOProcessor()
{

    int rc = 0;

    // Check and process a frame in our queue
    CSFrame * sFrame = m_qDiskIOSource->GetFrame();

    if (sFrame == NULL) {
        return 0;
    }
    m_nFramesProcessed++;

    rc = 1; // Ensures we are called back without a sleep

    if (sFrame->GetMessageType() == CSFrame::Normal) {
        if (!m_bError) {
            if (!m_pFFMediaProcessor->PutFrame(sFrame))
                SendErrorNotification(m_pFFMediaProcessor->GetErrorString());

            if (m_bCreateHLSOutput) {
                m_pAdaptiveMediaGenerator->ProcessFrame(sFrame);
            }
        }
        m_qDiskIOSource->ReleaseFrame(sFrame);
        return rc;
    }

    if (sFrame->GetMessageType() != CSFrame::Custom) {
        // Not expecting any other frame type
        m_qDiskIOSource->ReleaseFrame(sFrame);
        return rc;
    }

    // Handle Custom frames
    CWriterDiskIOInfo *diskIOInfo =
        static_cast<CWriterDiskIOInfo *>(sFrame->GetCustomObject());

    timestamp_t nTimestamp = diskIOInfo->GetTimeStamp();

    switch (diskIOInfo->GetMessageType()) {

    case CWriterDiskIOInfo::Write:
    case CWriterDiskIOInfo::WriteIndex:
    case CWriterDiskIOInfo::GetNext:
    case CWriterDiskIOInfo::Progress:
        // Not expecting any of these
        break;

    case CWriterDiskIOInfo::Start:
        if (!m_pFFMediaProcessor->Start(nTimestamp))
            SendErrorNotification(m_pFFMediaProcessor->GetErrorString());

        if (m_pAdaptiveMediaGenerator != NULL)
            m_pAdaptiveMediaGenerator->Start(nTimestamp);

        break;

    case CWriterDiskIOInfo::Stop:
        if (!m_pFFMediaProcessor->Stop(nTimestamp))
            SendErrorNotification(m_pFFMediaProcessor->GetErrorString());

        if (m_pAdaptiveMediaGenerator != NULL)
            m_pAdaptiveMediaGenerator->Stop(nTimestamp);

        break;

    case CWriterDiskIOInfo::Quit:
        rc = -1;
        break;
    }

    delete (diskIOInfo);

    m_qDiskIOSource->ReleaseFrame(sFrame);

    return rc;
}

int CWriterDiskIOFFThread::DiskIOInit()
{
    ostringstream mediaPath;
    mediaPath << m_pWriter->GetMediaDir() << "/blob/" << m_pWriter->GetMediaName();

    // TODO: get speeds from the writer
    std::vector<int> trackSpeeds;
    trackSpeeds.push_back(2);
    trackSpeeds.push_back(10);
    trackSpeeds.push_back(60);
    trackSpeeds.push_back(256);

    m_pFFMediaProcessor = new CFFMediaProcessor("", mediaPath.str(), trackSpeeds,
                                                m_strStreamType, m_pWriter->GetUniqueId(),
                                                true, false, m_strSDPFilePath);

    m_pFFMediaProcessor->SetTrimLength(m_pWriter->GetTrimLength());
    m_pFFMediaProcessor->SetAppendMode(true);

#ifndef WINDOWS
    // For a NAS drive do a 'sync' on every write TODO
    if (strstr(m_pWriter->GetMediaDir(), "portal/site/NAS") != NULL)
        m_pFFMediaProcessor->SetSyncWrites(true);
#endif
    m_pFFMediaProcessor->EnableFFTracks(m_bCreateFFTracks);

    m_pFFMediaProcessor->EnablePrimaryTN(m_bCreatePrimaryThumbnail);


    if (m_nStreamThumbnailsFrequency > 0) {
    	m_pFFMediaProcessor->EnableStreamTN(true);
    	//Input StreamThumbnail frequency is in Seconds. So, converting to milli seconds.
    	m_pFFMediaProcessor->SetStreamTNTimeInterval(m_nStreamThumbnailsFrequency * 1000);
    }

    if (!m_pFFMediaProcessor->InitMedia()) {
        SYSLOG_INFO("Failed FF Init: %s", m_pFFMediaProcessor->GetErrorString().c_str());
        // TODO: Generate error
    }

    return 0;
}

void CWriterDiskIOFFThread::SetDropBuffers(bool dropBuffers) {
    if (m_pFFMediaProcessor != NULL)
        m_pFFMediaProcessor->SetDropBuffers(dropBuffers);
}

uint64_t CWriterDiskIOFFThread::GetFramesProcessed()
{
    return m_nFramesProcessed;
}

uint64_t CWriterDiskIOFFThread::GetBytesWritten()
{
    if (m_pFFMediaProcessor == NULL)
        return 0;

    uint64_t nBytesWritten = m_pFFMediaProcessor->GetBytesWritten();

    if (m_pAdaptiveMediaGenerator != NULL)
    	nBytesWritten += m_pAdaptiveMediaGenerator->GetBytesWritten();

    return nBytesWritten;
}

uint64_t CWriterDiskIOFFThread::GetBytesDeleted()
{
    if (m_pFFMediaProcessor == NULL)
        return 0;

    uint64_t nBytesDeleted = m_pFFMediaProcessor->GetBytesDeleted();

    if (m_pAdaptiveMediaGenerator != NULL)
    	nBytesDeleted += m_pAdaptiveMediaGenerator->GetBytesDeleted();

    return nBytesDeleted;
}

uint64_t CWriterDiskIOFFThread::GetHLSBytesWritten()
{
    if (m_pFFMediaProcessor == NULL)
        return 0;

    if (m_pAdaptiveMediaGenerator == NULL)
        return 0;

    return m_pAdaptiveMediaGenerator->GetBytesWritten();
}

uint64_t CWriterDiskIOFFThread::GetHLSBytesDeleted()
{
    if (m_pFFMediaProcessor == NULL)
        return 0;

    if (m_pAdaptiveMediaGenerator == NULL)
        return 0;

    return m_pAdaptiveMediaGenerator->GetBytesDeleted();
}

uint64_t CWriterDiskIOFFThread::GetHLSVideoDropCount()
{
    if (m_pAdaptiveMediaGenerator == NULL)
        return 0;

    return m_pAdaptiveMediaGenerator->GetVideoDropCount();
}

uint64_t CWriterDiskIOFFThread::GetHLSAudioDropCount()
{
    if (m_pAdaptiveMediaGenerator == NULL)
        return 0;

    return m_pAdaptiveMediaGenerator->GetAudioDropCount();
}

uint64_t CWriterDiskIOFFThread::GetBuffersDropped()
{
    if (m_pFFMediaProcessor == NULL)
        return 0;
    return m_pFFMediaProcessor->GetBuffersDropped();
}

uint64_t CWriterDiskIOFFThread::GetFramesDropped()
{
    return m_qDiskIOSource->DropCount();
}

void CWriterDiskIOFFThread::SetupHLSOutput() {

    if (!m_bCreateHLSOutput)
        return;

    ostringstream mediaBlobDir;
    mediaBlobDir << m_pWriter->GetMediaDir() << "/blob/" << m_pWriter->GetMediaName();

    bool bSyncWrites = false;
#ifndef WINDOWS
    if (strstr(m_pWriter->GetMediaDir(), "portal/site/NAS") != NULL)
        bSyncWrites = true;
#endif

    m_pAdaptiveMediaGenerator = new CAdaptiveMediaGenerator(mediaBlobDir.str(),
                                    m_strSDPFilePath,
                                    (m_pWriter->GetTrimLength() / 1000),
                                    bSyncWrites);
}


void CWriterDiskIOFFThread::SendErrorNotification(const string errorString) {
    m_bError = true;
    m_pWriter->SetError();
    m_pWriter->SendErrorNotificationString(NOTIFICATION_ERROR_FILE_WRITE, errorString);
}
