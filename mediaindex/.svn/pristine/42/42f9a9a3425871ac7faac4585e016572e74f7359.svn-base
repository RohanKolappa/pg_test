
#include "ClipSourceIndex.hh"
#include "MediaPortalUtils.hh"


namespace mediaindex {

ClipSourceIndex::ClipSourceIndex()
{
    m_pIndexData = NULL;
    m_nStartTimeCode = 0;
    m_nIndexFileTimestamp = 0;
    m_bDiscontinuity = 0;
}

ClipSourceIndex::~ClipSourceIndex()
{
    if (m_pIndexData != NULL)
        delete m_pIndexData;
}

int ClipSourceIndex::init(char *mediaID, char *homedir)
{
    m_pIndexData = new IndexData(mediaID, homedir);
    return INDEX_OK;
}


int ClipSourceIndex::refreshIndex()
{
    char *filename = getIndexFile(m_pIndexData->GetMediaID(),
                                  m_pIndexData->GetHomeDir());

    struct stat statbuf;
    int status = LSTAT(filename, &statbuf);

    if (status == -1) {
        m_pIndexData->setLastError("Index file not found.");
        free(filename);
        return INDEX_ERROR;
    }

    int indexTimestamp = (int)statbuf.st_mtime;
    if (m_nIndexFileTimestamp == indexTimestamp) {
        free(filename);
        return INDEX_OK;
    }

    m_nIndexFileTimestamp = indexTimestamp;
    char *fileContents = readFile(filename);
    free(filename);
    if (m_pIndexData->init(fileContents) != INDEX_OK) {
        free(fileContents);
        return INDEX_ERROR;
    }
    free(fileContents);
    return INDEX_OK;
}

int ClipSourceIndex::start(uint64_t startTime, uint64_t startTimeCode)
{
    m_nStartTimeCode = startTimeCode;
    m_bDiscontinuity = 1;

    if (refreshIndex() == INDEX_ERROR)
        return INDEX_ERROR;

    // If invalid startTimeCode go to start of clip
    if (m_nStartTimeCode < m_pIndexData->getBeginTime()) {
        m_nStartTimeCode = m_pIndexData->getBeginTime();
    }

    return INDEX_OK;
}

int ClipSourceIndex::getFileOffset(uint64_t toffset,
                                   char **pfilename,
                                   uint64_t *poffset)
{
    *pfilename = getBlobFile(m_pIndexData->GetMediaID(),
                             m_pIndexData->GetHomeDir(),
                             (int)(toffset / 60000));
    *poffset = toffset % 60000;
    return INDEX_OK;
}

int ClipSourceIndex::getNext(uint64_t lastTimeCodeRead,
                             int64_t lastbyteoffset,
                             uint64_t *pbegintimecode,
                             uint64_t *pendtimecode,
                             char **pfilename,
                             uint64_t *pnexttimecode,
                             uint64_t *poffset,
                             uint64_t *plength,
                             int *pdiscontinuity,
                             int *eom)
{

    *eom = 0;

    if (refreshIndex() == INDEX_ERROR)
        return INDEX_ERROR;

    // Check if startTimeCode is past the end
    if (lastTimeCodeRead >= m_pIndexData->getEndTime()) {
        if ( (m_pIndexData->InProgress() == 1) &&
                ( (lastTimeCodeRead - m_pIndexData->getEndTime()) < 60000) ) {
            uint64_t toffset = lastTimeCodeRead - m_pIndexData->getBeginTime();
            getFileOffset(toffset, pfilename, poffset);
            *pbegintimecode = lastTimeCodeRead;
            *plength = 60000 - (*poffset);
            *pendtimecode = (*pbegintimecode) + (*plength);
            *pdiscontinuity = m_bDiscontinuity;
            m_bDiscontinuity = 0;
            *pnexttimecode = m_nStartTimeCode; // not used by client (used for debug)
            return INDEX_RETRY;
        }
        else {
            m_pIndexData->setLastError("Requested timeCode past end of media");
            *eom = 1;
            return INDEX_ERROR;
        }
    }

    // Check if startTimeCode is in a gap
    Interval *gap = m_pIndexData->getGapContaining(m_nStartTimeCode);
    if (gap != NULL) {
        m_nStartTimeCode = gap->GetEndTime();
    }
    *pbegintimecode = m_nStartTimeCode;

    uint64_t toffset = m_nStartTimeCode - m_pIndexData->getBeginTime();
    getFileOffset(toffset, pfilename, poffset);
    *pdiscontinuity = m_bDiscontinuity;
    m_bDiscontinuity = 0;

    // Limit the length since each file is only 1 minute
    *plength = 60000 - (*poffset);
    *pendtimecode = (*pbegintimecode) + (*plength);

    // Check if the media ends before the length
    uint64_t remainingTime = m_pIndexData->getEndTime() - m_nStartTimeCode;
    if ((*plength) > remainingTime) {
        *plength = remainingTime;
        *pendtimecode = m_pIndexData->getEndTime();
    }

    // Set the next StartTimeCode
    uint64_t nextStartTimeCode = m_nStartTimeCode + (*plength);

    // Check if there is a gap in this length, if so set the length and
    // next StartTimeCode
    gap = m_pIndexData->getGapStartingIn(m_nStartTimeCode, *plength);
    if (gap != NULL) {
        (*plength) = gap->GetBeginTime() - m_nStartTimeCode;
        *pendtimecode = gap->GetBeginTime();
        nextStartTimeCode = gap->GetEndTime();
        m_bDiscontinuity = 1;
    }

    // End of Media detection
    if ( (m_pIndexData->InProgress() == 0) &&
            (*pbegintimecode >= m_pIndexData->getEndTime()) ) {
        *eom = 1;
    }

    // This will be used by the "next" getNext call
    m_nStartTimeCode = nextStartTimeCode;
    *pnexttimecode = m_nStartTimeCode; // not used by client (used for debug)

    return INDEX_OK;
}

int ClipSourceIndex::stop()
{
    return INDEX_OK;
}

char* ClipSourceIndex::getLastError()
{
    return m_pIndexData->getLastError();
}

int ClipSourceIndex::getTimecodeAtOffset(uint64_t offset, uint64_t *timecode)
{

    if (refreshIndex() == INDEX_ERROR)
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

int ClipSourceIndex::getOffsetFromTimecode(uint64_t timecode, uint64_t *offset)
{

    if (refreshIndex() == INDEX_ERROR)
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

int ClipSourceIndex::getBeginTimecode(uint64_t *timecode)
{

    if (refreshIndex() == INDEX_ERROR)
        return INDEX_ERROR;

    *timecode = m_pIndexData->getBeginTime();

    return INDEX_OK;
}

int ClipSourceIndex::getEndTimecode(uint64_t *timecode)
{

    if (refreshIndex() == INDEX_ERROR)
        return INDEX_ERROR;

    *timecode = m_pIndexData->getEndTime();

    return INDEX_OK;
}

void ClipSourceIndex::dumpGapInfo()
{
    char *filename = getIndexFile(m_pIndexData->GetMediaID(),
                                  m_pIndexData->GetHomeDir());
    if (filename != NULL) {
        printf("\n");
        printf("File: %s\n", filename);
        m_pIndexData->dumpGapInfo();
        free(filename);
    }
}

int ClipSourceIndex::getNextGap(uint64_t timecode, uint64_t *begintimecode, uint64_t *endtimecode) {
    Interval *gap = m_pIndexData->getGapAfter(timecode);
    if (gap == NULL)
        return 0;
    *begintimecode = gap->GetBeginTime();
    *endtimecode = gap->GetEndTime();
    return 1;
}

};


