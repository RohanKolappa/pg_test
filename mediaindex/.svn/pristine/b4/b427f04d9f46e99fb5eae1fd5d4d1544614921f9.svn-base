

#include "ClipDestIndex.hh"
#include "MediaPortalUtils.hh"

// ClipDestIndex Class

namespace mediaindex {

ClipDestIndex::ClipDestIndex()
{
    m_pIndexData = NULL;
    m_nBytesDeleted = 0;
}

ClipDestIndex::~ClipDestIndex()
{

    if (m_pIndexData == NULL)
        return;

    if (m_pIndexData->InProgress() == 1) {
        m_pIndexData->updateStop(m_pIndexData->getEndTime());
        writeIndexFile();
    }

    delete m_pIndexData;
}

int ClipDestIndex::init(char *mediaID, char *homedir, uint64_t trimLength)
{
    m_pIndexData = new IndexData(mediaID, homedir);
    m_nTrimLength = trimLength;
    m_nBytesDeleted = 0;

    // Check for existing index file
    char *filename = getIndexFile(m_pIndexData->GetMediaID(),
                                  m_pIndexData->GetHomeDir());
    struct stat statbuf;
    int status = LSTAT(filename, &statbuf);
    if (status == -1) { // This is a new index
        free(filename);
    }
    else { // Read the existing index
        char *buf = readFile(filename);
        free(filename);
        status = m_pIndexData->init(buf);
        free(buf);
        if (status == 0) {
            m_pIndexData->setLastError("Failed to read index file");
            return INDEX_ERROR;
        }
        // Appending to existing media: since we found an existing
        // index file, we issue a "stop" here so that the next "start"
        // will automatically create a gap between the current end
        // time and the next start time
        m_pIndexData->updateStop(m_pIndexData->getEndTime());
    }

    return INDEX_OK;
}

int ClipDestIndex::writeIndexFile()
{
    char *filename = getIndexFile(m_pIndexData->GetMediaID(),
                                  m_pIndexData->GetHomeDir());
#ifdef WINDOWS
    FILE *fp = fopen(filename, "w+b");
#else
    FILE *fp = fopen(filename, "a");
#endif
    if (fp == NULL) {
        char *errorStr = (char *)malloc(strlen(m_pIndexData->GetMediaID()) + 50);
        sprintf(errorStr, "Unable to open index file for %s",
                m_pIndexData->GetMediaID());
        m_pIndexData->setLastError(errorStr);
        free(errorStr);
        free(filename);
        return INDEX_ERROR;
    }
    free(filename);
    char *dataStr = m_pIndexData->toString();
    // Lock the file before writing to it. Unfortuately, we need to open
    // the file in append mode (done above), truncate the file (fseek
    // does not work), and write the new data before we unlock the file.
#ifdef WINDOWS
    fprintf(fp, "%s", dataStr);
#else
    int fd = fileno(fp);
    if (flock(fd, LOCK_EX) == 0) {
        ftruncate(fd, 0);
        fprintf(fp, "%s", dataStr);
        flock(fd, LOCK_UN);
    }
#endif
    fclose(fp);
    free(dataStr);
    return INDEX_OK;
}

int ClipDestIndex::start(uint64_t startTime, uint64_t startTimeCode)
{
    if (m_pIndexData->updateStart(startTime, startTimeCode) == 0)
        return INDEX_ERROR;

    return writeIndexFile();
}

int ClipDestIndex::trimBlob(uint64_t begin, uint64_t end)
{

    uint64_t toffset = begin - m_pIndexData->getBeginTime();
    int mbegin = (int) (toffset / 60000);
    toffset = end - m_pIndexData->getBeginTime();
    int mend = (int) (toffset / 60000);
    struct stat statbuf;
    for (int i = mbegin; i  < mend; i++) {
        char *pfilename = getBlobFile(m_pIndexData->GetMediaID(),
                                      m_pIndexData->GetHomeDir(), i);
        char *pindexfile = (char *)malloc(strlen(pfilename)  + 16);
        sprintf(pindexfile, "%s.idx", pfilename);
        if (LSTAT(pfilename, &statbuf) != -1)
            m_nBytesDeleted += statbuf.st_size;
        if (LSTAT(pindexfile, &statbuf) != -1)
            m_nBytesDeleted += statbuf.st_size;
        unlink (pfilename);
        unlink (pindexfile);
        free(pfilename);
        free(pindexfile);
    }
    return 1;

}

int ClipDestIndex::trim(uint64_t timeCode)
{
    if (!m_pIndexData->HasGaps()) {
        if (trimBlob(m_pIndexData->getBeginTime(), timeCode) == 0)
            m_pIndexData->setLastError("Failed to trimBlob");
        Interval *gap = new Interval(m_pIndexData->getBeginTime(), timeCode);
        m_pIndexData->AddGap(gap);
    }
    else {
        Interval *gap = m_pIndexData->GetFirstGap();
        if (trimBlob(gap->GetEndTime(), timeCode) == 0)
            m_pIndexData->setLastError("Failed to trimBlob");
        gap->SetEndTime(timeCode);
    }
    return INDEX_OK;
}

int ClipDestIndex::getNext(uint64_t timeCode, char **pfilename,
                           uint64_t *poffset, uint64_t *plength)
{

    m_pIndexData->setEndTime(timeCode);
    if (m_nTrimLength > 0) {
        uint64_t trimTimeCode = timeCode - m_nTrimLength;
        if (trimTimeCode > m_pIndexData->getBeginTime())
            if (trim(trimTimeCode) != INDEX_OK)
                m_pIndexData->setLastError("Unable to trim blobs\n");
    }

    writeIndexFile();

    uint64_t toffset = timeCode - m_pIndexData->getBeginTime();
    *pfilename = getBlobFile(m_pIndexData->GetMediaID(),
                             m_pIndexData->GetHomeDir(),
                             (int)(toffset / 60000));
    *poffset = toffset % 60000;
    *plength = 60000 - *poffset;

    return INDEX_OK;
}

int ClipDestIndex::stop(uint64_t stopTimeCode)
{
    m_pIndexData->updateStop(stopTimeCode);
    return writeIndexFile();
}

char* ClipDestIndex::getLastError()
{
    return m_pIndexData->getLastError();
}

int ClipDestIndex::getTimecodeAtOffset(uint64_t offset, uint64_t *timecode)
{
    if (m_pIndexData == NULL)
        return INDEX_ERROR;

    /* Compute timecode */
    *timecode = m_pIndexData->getTimecodeAtOffset(offset);

    // If result is beyond end of media, then we give upto a minute of
    // lee way if clip writing is still in progress
    if (*timecode >= m_pIndexData->getEndTime()) {
        if (m_pIndexData->InProgress() == 1) {
            if ((*timecode - m_pIndexData->getEndTime()) < 60000)  {
                // timecode is still ok
                return INDEX_OK;
            }
            else {
                *timecode = m_pIndexData->getEndTime() + 60000;
            }
        }
        else {
            *timecode = m_pIndexData->getEndTime();
            return INDEX_RETRY; // indicates end of media
        }
    }

    return INDEX_OK;
}

int ClipDestIndex::getOffsetFromTimecode(uint64_t timecode, uint64_t *offset)
{
    if (m_pIndexData == NULL)
        return INDEX_ERROR;

    /* Compute offset */
    *offset = m_pIndexData->getOffsetFromTimecode(timecode);

    // If we are beyond end of media, and we are not in progress,
    // the offset should just be the offset of end of media
    if (timecode >= m_pIndexData->getEndTime()) {
        if (m_pIndexData->InProgress() == 0) {
            *offset = m_pIndexData->getOffsetFromTimecode(m_pIndexData->getEndTime());
        }
    }
    return INDEX_OK;
}

int ClipDestIndex::getBeginTimecode(uint64_t *timecode)
{
    if (m_pIndexData == NULL)
        return INDEX_ERROR;

    *timecode = m_pIndexData->getBeginTime();

    return INDEX_OK;
}

int ClipDestIndex::getEndTimecode(uint64_t *timecode)
{
    if (m_pIndexData == NULL)
        return INDEX_ERROR;

    *timecode = m_pIndexData->getEndTime();

    return INDEX_OK;
}

int ClipDestIndex::getNextGap(uint64_t timecode, uint64_t *begintimecode, uint64_t *endtimecode) {
    Interval *gap = m_pIndexData->getGapAfter(timecode);
    if (gap == NULL)
        return 0;
    *begintimecode = gap->GetBeginTime();
    *endtimecode = gap->GetEndTime();
    return 1;
}

int ClipDestIndex::getProgress(bool & inProgress)
{
    if (m_pIndexData == NULL)
        return INDEX_ERROR;
    inProgress = false;
    if (m_pIndexData->InProgress() != 0)
        inProgress = true;
    return INDEX_OK;
}

uint64_t ClipDestIndex::getBytesDeleted()
{

    return m_nBytesDeleted;
}
};


