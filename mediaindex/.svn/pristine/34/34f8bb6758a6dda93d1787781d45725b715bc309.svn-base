
#include "IndexInterface.hh"
#include "IndexData.hh"

namespace mediaindex {

// Thread safe strtok_r not supported on all platforms so
// copying in our own version it here. Ugly but works.
static char * safe_strtok(char *s,
    const char *delim,
    char **lasts)
{
    register char *spanp;
    register int c, sc;
    char *tok;


    if (s == NULL && (s = *lasts) == NULL)
        return (NULL);

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
        if (c == sc)
            goto cont;
    }

    if (c == 0) {       /* no non-delimiter characters */
        *lasts = NULL;
        return (NULL);
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;) {
        c = *s++;
        spanp = (char *)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *lasts = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}
IndexData::IndexData(char *mediaID, char *homedir)
{
    m_pClip = new Interval(0, 0);
    m_GapList.clear();
    m_pMediaID = strdup(mediaID);
    m_pHomeDir = NULL;
    if (homedir != NULL)
        m_pHomeDir = strdup(homedir);
    m_bInProgress = 0;
    m_pLastError = NULL;
}

IndexData::~IndexData()
{
    delete m_pClip;
    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        delete gap;
    }
    m_GapList.clear();

    if (m_pLastError != NULL)
        free(m_pLastError);
    if (m_pMediaID != NULL)
        free(m_pMediaID);
    if (m_pHomeDir != NULL)
        free(m_pHomeDir);

}

char *IndexData::toString()
{
    int ulonglongsize = 20;
    int countNumbers = 3 + m_GapList.size() * 2;

    char *buf = (char *)malloc((ulonglongsize + 1) * countNumbers + 2);
    sprintf(buf, "%"PRIu64"-%d\n%"PRIu64"-%"PRIu64"\n",
            getBeginTime(), m_bInProgress, getBeginTime(), getEndTime());
    char *tmp = (char *)malloc(ulonglongsize * 2 + 3);
    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        if (gap->GetBeginTime() == gap->GetEndTime()) // included in clip->end
            continue;
        sprintf(tmp, "%"PRIu64"-%"PRIu64"\n", gap->GetBeginTime(), gap->GetEndTime());
        strcat(buf, tmp);
    }
    free(tmp);
    return buf;
}

int IndexData::init(char *rangeStr)
{

    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        delete gap;
    }
    m_GapList.clear();

    char *saveStr = NULL;
    char *numStr = safe_strtok(rangeStr, "-", &saveStr);
    if (numStr == NULL)
        goto BAD_INDEX;

    numStr = safe_strtok(NULL, "\n", &saveStr);
    if (numStr == NULL)
        goto BAD_INDEX;

    m_bInProgress = atoi(numStr);
    if (!(m_bInProgress == 0 || m_bInProgress == 1))
        goto BAD_INDEX;

    numStr = safe_strtok(NULL, "-", &saveStr);
    if (numStr == NULL)
        goto BAD_INDEX;
    setBeginTime(STRTOULL(numStr, NULL, 0));
    numStr = safe_strtok(NULL, "\n", &saveStr);
    if (numStr == NULL)
        goto BAD_INDEX;
    setEndTime(STRTOULL(numStr, NULL, 0));
    while (1) {
        char *numStr1 = safe_strtok(NULL, "-", &saveStr);
        if (numStr1 == NULL)
            break;
        char *numStr2 = safe_strtok(NULL, "\n", &saveStr);
        if (numStr2 == NULL)
            break;
        uint64_t begin = STRTOULL(numStr1, NULL, 0);
        uint64_t end = STRTOULL(numStr2, NULL, 0);
        Interval *gap = new Interval(begin, end);
        m_GapList.push_back(gap);
    }

    return INDEX_OK;

BAD_INDEX:
    setLastError("Malformed Index File");
    return INDEX_ERROR;
}

uint64_t IndexData::getLength()
{
    if (m_bInProgress == 0) {
        return m_pClip->GetLength();
    }
    // If we are in progress, the most uptodate time stamp we have is
    // the end timeode of the last gap
    if (HasGaps()) {
        Interval *gap = m_GapList.back();
        return gap->GetEndTime() - getBeginTime();
    }
    // Should not reach here but just in case...
    return m_pClip->GetLength();
}

// TODO remove redundant "startTime" argument
int IndexData::updateStart(uint64_t startTime, uint64_t timeCode)
{
    if (getBeginTime() == 0)  {
        setBeginTime(timeCode);
        setEndTime(timeCode);
    }
    m_bInProgress = 1;
    if (HasGaps()) {
        Interval *gap = m_GapList.back();
        gap->SetEndTime(timeCode);
    }
    return INDEX_OK;
}

int IndexData::updateStop(uint64_t timeCode)
{
    m_bInProgress = 0;
    setEndTime(timeCode);
    Interval *gap = new Interval(timeCode, timeCode);
    m_GapList.push_back(gap);
    return INDEX_OK;
}

char * IndexData::getTimeString(int64_t a_milliseconds, char *a_buffer)
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
    sprintf(a_buffer + len, "%d.%03ds", seconds, ms);
    return a_buffer;
}

char * IndexData::getDateString(uint64_t a_milliseconds, char *a_buffer)
{

    strcpy(a_buffer, "");
#ifdef WINDOWS
    // TODO figure out how to do this on windows
#else
    time_t seconds = (time_t) (a_milliseconds / 1000);
    int ms = a_milliseconds % 1000;

    struct tm result;
    if (gmtime_r(&seconds, &result) != NULL) {
        int len = strftime(a_buffer, 63, "%a %b %e %G %T", &result);
        sprintf(a_buffer + len, ".%03d", ms);
    }

#endif
    return a_buffer;
}

void IndexData::dumpGapInfo()
{

    char buffer1[64];
    char buffer2[64];
    char buffer3[64];

    // Still in progress??
    if (m_bInProgress) {
        printf("\n");
        printf("**** In Progress ****\n");
    }


    // Find total gap length
    int64_t gapLength = 0;
    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        gapLength += gap->GetLength();
    }

    int64_t totalLength = getLength();
    int64_t playLength = totalLength - gapLength;
    printf("\n");
    printf("Start Time: %s\n", getDateString(getBeginTime(), buffer1));
    printf("End Time:   %s\n", getDateString(getEndTime(), buffer1));
    printf("\n");
    printf("Time Span = %s\n", getTimeString(totalLength, buffer1));
    printf("Play Length = %s\n", getTimeString(playLength, buffer1));
    printf("Total Gap Time = %s\n", getTimeString(gapLength, buffer1));
    printf("\n");

    // For each gap, we have one media segment and one gap segment.
    // Media segment extends from the 'last' startTime to the
    // beginning of the gap.  Gap segment extends from the beginning
    // of the gap to the end of the gap
    uint64_t startTime = getBeginTime();
    int64_t absoluteOffset = 0;
    int64_t mediaOffset = 0;
    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        printf("Offset: %s [%s] Media: %s\n",
               getTimeString(absoluteOffset, buffer1),
               getTimeString(mediaOffset, buffer2),
               getTimeString(gap->GetBeginTime() - startTime, buffer3));
        mediaOffset += gap->GetBeginTime() - startTime;
        absoluteOffset = gap->GetBeginTime() - getBeginTime();
        printf("Offset: %s [%s] Gap: %s\n",
               getTimeString(absoluteOffset, buffer1),
               getTimeString(mediaOffset, buffer2),
               getTimeString(gap->GetEndTime() - gap->GetBeginTime(), buffer3));
        startTime = gap->GetEndTime();
        absoluteOffset = gap->GetEndTime() - getBeginTime();
    }

    // Finally we will have one media segment extending from the
    // 'last' startTime to the end of media.
    if (getEndTime() >= startTime) {
        printf("Offset: %s [%s] Media: %s\n",
               getTimeString(absoluteOffset, buffer1),
               getTimeString(mediaOffset, buffer2),
               getTimeString(getEndTime() - startTime, buffer3));
        mediaOffset += getEndTime() - startTime;
        absoluteOffset = getLength();
        printf("End Of Media At Offset: %s [%s]\n",
               getTimeString(absoluteOffset, buffer1),
               getTimeString(mediaOffset, buffer2));
    }
    else {
        printf("Offset: %s [%s] Media: In Progress\n",
               getTimeString(absoluteOffset, buffer1),
               getTimeString(mediaOffset, buffer2));


    }
    printf("\n");

}

bool IndexData::HasGaps()
{

    if (m_GapList.empty())
        return false;
    return true;
}

void IndexData::AddGap(Interval *gap)
{
    m_GapList.push_back(gap);
}

Interval *IndexData::GetFirstGap()
{
    return m_GapList.front();
}

Interval * IndexData::getGapContaining(uint64_t startTimeCode)
{
    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        if ( (startTimeCode >= gap->GetBeginTime()) &&
                (startTimeCode < gap->GetEndTime()) )  {
            return gap;
        }
    }
    return NULL;
}

Interval * IndexData::getGapStartingIn(uint64_t startTimeCode, uint64_t length)
{
    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        if ( (gap->GetBeginTime()  >=  startTimeCode) &&
                (gap->GetBeginTime()  <=  (startTimeCode + length) ) ) {
            return gap;
        }
    }
    return NULL;
}

Interval * IndexData::getGapAfter(uint64_t startTimeCode)
{
    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        if (startTimeCode < gap->GetBeginTime()) {
            return gap;
        }
    }
    return NULL;
}

uint64_t IndexData::getTimecodeAtOffset(uint64_t offset)
{
    uint64_t startTimeCode = getBeginTime();

    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        if (gap->GetBeginTime() > startTimeCode + offset) {
            break;
        }
        offset -= gap->GetBeginTime() - startTimeCode;
        startTimeCode = gap->GetEndTime();
    }
    return startTimeCode + offset;
}

uint64_t IndexData::getOffsetFromTimecode(uint64_t timeCode)
{
    uint64_t startTimeCode = getBeginTime();
    uint64_t offset = 0;

    if (timeCode <= startTimeCode)
        return 0;

    for (m_gIter = m_GapList.begin(); m_gIter != m_GapList.end(); ++m_gIter) {
        Interval *gap = (*m_gIter);
        if (gap->GetBeginTime() > timeCode) {
            break;
        }
        offset += gap->GetBeginTime() - startTimeCode;
        startTimeCode = gap->GetEndTime();
    }
    if (timeCode < startTimeCode) // request time code within gap
        return offset;
    return (timeCode - startTimeCode) + offset;
}

int IndexData::setLastError(const char *error)
{
    if (m_pLastError != NULL)
        free(m_pLastError);
    m_pLastError = strdup(error);
    return 0;
}

char *IndexData::getLastError()
{
    return m_pLastError;
}


};
