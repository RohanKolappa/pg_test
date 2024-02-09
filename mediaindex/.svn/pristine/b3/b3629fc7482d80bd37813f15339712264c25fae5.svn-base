
#include "CommonIncludes.hh"

#include "MinuteFileWriter.hh"
#include "MatroskaFileParser.hh"
#include "XMLUtils.hh"
#include "FileSystemUtils.hh"
#include "MatroskaUtils.hh"
#include "SystemUtils.hh"
#include "EditList.hh"
#include "MediaProcessorInterface.hh"


#ifdef _BIG_ENDIAN_
#define REVERSE_WORD(x) (x)
#else // Little endian
#define _HI_BYTE_(x) (0x00FF & ((x) >> 8) )
#define _LOW_BYTE_(x) (0x00FF & (x))
#define REVERSE_WORD(x) ((_LOW_BYTE_(x)<<8) | (_HI_BYTE_(x)))
#endif

// We do not have access to v2dlib so we hash define some minimal things
#define GET_PROTOCOL_UNIT_ID(c) \
    REVERSE_WORD(*(unsigned short *)((c) + 2));
#define IDENT_SERVER_CAP        203

// Debug can be turned on locally if needed by defining
// DEBUG_CLASS (un-comment below hashdef)
//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define ASSERT(x) assert(x)
#define print_log_info printf
#define print_log_debug printf
#else
inline void dummylog(const char *format, ...) {};
#define ASSERT(x) do {} while (0)
#define print_log_info if (!m_bSilent) printf
#define print_log_debug dummylog
#endif

#define PRINTF_INFO print_log_info
#define PRINTF_DEBUG print_log_debug


namespace mediaindex {

struct CMinuteFileWriter::Impl {
    string m_strInFile;
    string m_strDbDir;
    string m_strBlobDir;

    CMediaWriter *m_pMediaWriter;
    CMatroskaFileParser * m_pMatroskaFileParser;
    CMediaServer * m_pMinuteFileServer;
    CEditList * m_pEditList;

    list<CMediaProcessorInterface*>m_pMediaProcessorList;

    uint64_t m_nFrameCount;
    uint64_t m_nMediaBeginTimeStamp;
    uint64_t m_nMediaEndTimeStamp;
    uint64_t m_nCurrentGapLength;
    uint64_t m_nCurrentSkipLength;
    uint64_t m_nLastSkipOffset;
    uint64_t m_nMediaStartTime;
    uint64_t m_nMediaStopTime;
    uint64_t m_nCurrentTimeOffset;
    uint64_t m_nFrameErrors;
    uint64_t m_nLastFilePosition;
    uint8_t * m_scapBuffer;
    int m_scapBufferLen;
    int m_nBookmarkIndex;
    bool m_bSilent;
    bool m_bFixedUUIDs;
    bool m_bExtractMetadata;
    bool m_bMediaStarted;
    bool m_bInsertScap;

    ~Impl();
    void InitializeVariables();
    int Write();
    int WriteFromMatroskaFile();
    int WriteFromMinuteFiles();
    void SetEditList(CEditList * aEditList);
    bool InitMediaWriter();
    bool SourceHasThumbnails();
    bool SourceIsSnapshot();
    bool InitMetaDataDirectories();
    void WriteMediaFramesToBlobDir();
    void WriteAllMediaFrames();
    void WriteEditedMediaFrames();
    void GetLastSCAP(uint64_t requestTimestamp);
    bool LookForScap(CCFrame & aFrame);
    void InsertSCAP(CCFrame & aFrame);
    void HandleMediaFrame(CCFrame & aFrame);
    void StartMedia(uint64_t nStartTime);
    void StopMedia(uint64_t nStopTime);
    void InitializeMediaToTimeOffset(uint64_t timeOffset);
    uint64_t GetTimecodeAtOffset(uint64_t offset);
    bool GetFrame(CCFrame & aFrame);
    std::string GetStreamType();

    void WriteMetaDataToDbDir();
    bool SaveXmlToFile(
            XERCES_NS::DOMDocument* a_pDomDoc,
            std::string a_metaDataType,
            std::string a_metaFileName);
    void WriteBookmarksToDbDir();
    void WriteAllBookmarks();
    void WriteEditedBookmarks();
    void HandleBookmarkFrame(CCFrame & aFrame);
    void PrintMediaInfo();
    void PrintProgressInfo();

    void AddMediaProcessor(
            CMediaProcessorInterface *a_pMediaProcessor);
};

CMinuteFileWriter::CMinuteFileWriter(
        const string& a_rstrInFile,
        const string& a_rstrDbDir,
        const string& a_rstrBlobDir)
{
	Initialize(a_rstrInFile,
               a_rstrDbDir, a_rstrBlobDir);
}

CMinuteFileWriter::~CMinuteFileWriter(void)
{
    delete m_pimpl;
    FreeXmlLib();
}

void CMinuteFileWriter::Initialize(
                               const string& a_rstrInFile,
                               const string& a_rstrDbDir,
                               const string& a_rstrBlobDir)
{
	InitXmlLib();
	m_pimpl = new CMinuteFileWriter::Impl();

	m_pimpl->InitializeVariables();

	m_pimpl->m_strInFile = a_rstrInFile;
	m_pimpl->m_strBlobDir = a_rstrBlobDir;
	m_pimpl->m_strDbDir = a_rstrDbDir;
}

int CMinuteFileWriter::Write()
{
    return m_pimpl->Write();
}

void CMinuteFileWriter::Silent()
{
    // This ensures nothing goes to standard out
    m_pimpl->m_bSilent = true;
}

void CMinuteFileWriter::FixedUUIDs()
{
    // Used for testing. This will create predictable file names for bookmarks.
    m_pimpl->m_bFixedUUIDs = true;
}

void CMinuteFileWriter::SetEditList(CEditList * editList)
{
    m_pimpl->SetEditList(editList);
}

void CMinuteFileWriter::AddMediaProcessor(
                           CMediaProcessorInterface *a_pMediaProcessor)
{
	m_pimpl->AddMediaProcessor(a_pMediaProcessor);
}

CMinuteFileWriter::Impl::~Impl()
{
    delete m_pMediaWriter;
    delete m_pMatroskaFileParser;
    delete m_pMinuteFileServer;
    delete [] m_scapBuffer;
}

void CMinuteFileWriter::Impl::InitializeVariables()
{
    m_pMediaWriter = NULL;
    m_pMatroskaFileParser = NULL;
    m_pMinuteFileServer = NULL;
    m_pEditList = NULL;
    m_nFrameCount = 0;
    m_nMediaBeginTimeStamp = 0;
    m_nMediaEndTimeStamp = 0;
    m_nCurrentGapLength = 0;
    m_nCurrentSkipLength = 0;
    m_nLastSkipOffset = 0;
    m_nMediaStartTime = 0;
    m_nMediaStopTime = 0;
    m_bMediaStarted = false;
    m_nFrameErrors = 0;
    m_bExtractMetadata = false;
    m_bSilent = false;
    m_bFixedUUIDs = false;
    m_nBookmarkIndex = 0;
    m_nLastFilePosition = 0;
    m_nCurrentTimeOffset = 0;
    m_bInsertScap = false;
    m_scapBuffer = NULL;
}

int CMinuteFileWriter::Impl::Write()
{
    if (IsDirectory(m_strInFile))
        return WriteFromMinuteFiles();
    else
        return WriteFromMatroskaFile();
}

int CMinuteFileWriter::Impl::WriteFromMatroskaFile()
{
    m_pMatroskaFileParser = new CMatroskaFileParser(m_strInFile);

    m_pMatroskaFileParser->ServeThumbnails(true);

    if (m_pMatroskaFileParser->Parse() != ELIBMC_OK) {
        return ELIBMC_ERROR;
    }

    PrintMediaInfo();

    if (!InitMediaWriter())
        return ELIBMC_ERROR;

    if (!InitMetaDataDirectories())
        return ELIBMC_ERROR;

    WriteMediaFramesToBlobDir();

    WriteMetaDataToDbDir();

    WriteBookmarksToDbDir();

    return 0;
}

int CMinuteFileWriter::Impl::WriteFromMinuteFiles()
{
    m_pMinuteFileServer = CMediaServer::Create(m_strInFile.c_str());

    m_pMinuteFileServer->ServeThumbnails(true);

    if (m_pMinuteFileServer->Init() != MSRC_OK) {
        return ELIBMC_ERROR;
    }

    PrintMediaInfo();

    if (!InitMediaWriter())
        return ELIBMC_ERROR;

    WriteMediaFramesToBlobDir();

    return 0;
}

bool CMinuteFileWriter::Impl::InitMediaWriter() {

    // Create the MediaWriter object
    m_pMediaWriter = new CMediaWriter(m_strBlobDir.c_str());
    if (m_pMediaWriter == NULL) {
        PRINTF_INFO("Could not create MediaWriter object!!\n");
        return false;
    }

    if (SourceHasThumbnails()) {
        m_pMediaWriter->EnableThumbnails(true);
        if (SourceIsSnapshot())
            m_pMediaWriter->EnableMedia(false);
    }

    // Initialize the MediaWriter object
    if (m_pMediaWriter->Init() != MSRC_OK) {
        PRINTF_INFO("MediaWriter Init Failed: %s\n",
               m_pMediaWriter->GetErrorString());
        return false;
    }

    return true;
}

bool CMinuteFileWriter::Impl::SourceHasThumbnails() {
    if (m_pMinuteFileServer != NULL) {
        return m_pMinuteFileServer->HasThumbnails();
    }
    return m_pMatroskaFileParser->HasThumbnails();
}

bool CMinuteFileWriter::Impl::SourceIsSnapshot() {
    if (m_pMinuteFileServer != NULL) {
        return m_pMinuteFileServer->IsSnapshot();
    }
    return m_pMatroskaFileParser->IsSnapshot();
}

bool CMinuteFileWriter::Impl::InitMetaDataDirectories() {

    if (m_strDbDir == "") {
        PRINTF_INFO("Skipping metadata extraction...\n");
    }
    else {
        if (CreateDbDirectories(m_strDbDir)) {
            m_bExtractMetadata = true;
        }
        else {
            PRINTF_INFO("Failed to create the metadata directory under: %s\n",
                    m_strDbDir.c_str());
            return false;
        }
    }

    return true;
}

void CMinuteFileWriter::Impl::WriteMediaFramesToBlobDir() {
    if (m_pEditList == NULL)
        WriteAllMediaFrames();
    else
        WriteEditedMediaFrames();
}


void CMinuteFileWriter::Impl::WriteAllMediaFrames() {
    InitializeMediaToTimeOffset(0);
    while (true) {
        CCFrame aFrame;
        if (!GetFrame(aFrame)) {
            break;
        }
        HandleMediaFrame(aFrame);
        PrintProgressInfo();
    }
}

void CMinuteFileWriter::Impl::WriteEditedMediaFrames() {
    bool bDone = false;
    for (int item = 0; item < m_pEditList->GetItemCount(); ++item) {
        if (GetStreamType() == "V2D" || GetStreamType() == "") {
            GetLastSCAP(m_pEditList->StartOffset(item));
        }
        InitializeMediaToTimeOffset(m_pEditList->StartOffset(item));
        if (!m_bMediaStarted) {
            m_nCurrentSkipLength += m_pEditList->StartOffset(item) - m_nLastSkipOffset;
        }
        while (true) {
            CCFrame aFrame;
            if (!GetFrame(aFrame)) {
                bDone = true;
                break;
            }
            if (aFrame.GetTimeOffset() > m_pEditList->EndOffset(item)) {
                m_nLastSkipOffset = m_pEditList->EndOffset(item);
                StopMedia(GetTimecodeAtOffset(m_pEditList->EndOffset(item)));
                break;
            }
            HandleMediaFrame(aFrame);
            PrintProgressInfo();
        }
        if (bDone)
            break;
    }
}

void CMinuteFileWriter::Impl::InitializeMediaToTimeOffset(uint64_t timeOffset) {
    if (m_pMinuteFileServer != NULL) {
        m_pMinuteFileServer->Stop();
        m_pMinuteFileServer->Start(m_pMinuteFileServer->GetTimecodeAtOffset(timeOffset));
    }
    else {
        m_pMatroskaFileParser->InitializeMediaToTimeOffset(timeOffset);
    }
}

bool CMinuteFileWriter::Impl::GetFrame(CCFrame & aFrame) {
    if (m_pMinuteFileServer != NULL) {
        return (m_pMinuteFileServer->GetFrame(aFrame) == MSRC_OK);
    }
    return m_pMatroskaFileParser->GetFrame(aFrame);
}

uint64_t CMinuteFileWriter::Impl::GetTimecodeAtOffset(uint64_t offset) {
    if (m_pMinuteFileServer != NULL) {
        return m_pMinuteFileServer->GetTimecodeAtOffset(offset);
    }
    return m_pMatroskaFileParser->GetTimecodeAtOffset(offset);
}

std::string CMinuteFileWriter::Impl::GetStreamType() {
    if (m_pMinuteFileServer != NULL) {
        // Minute files are assumed to be V2D
        // Searching for SCAP breaks out early if any non V2D packet is detected
        return "V2D";
    }
    return m_pMatroskaFileParser->GetStreamType();
}

void CMinuteFileWriter::Impl::HandleMediaFrame(CCFrame & aFrame) {
    switch (aFrame.GetMessageType()) {
    case CCFrame::Normal:
    {
        if (!m_bMediaStarted) {
            StartMedia(aFrame.GetTimeStamp());

            //break;
        }
        uint64_t nTimeOffset = aFrame.GetTimeOffset();
        uint64_t nTimestamp = m_nMediaBeginTimeStamp + nTimeOffset +
                m_nCurrentGapLength - m_nCurrentSkipLength;
        uint64_t nExpectedTimestamp = aFrame.GetTimeStamp();
        m_nCurrentTimeOffset = nTimeOffset;
        if (nTimestamp != nExpectedTimestamp) {
            PRINTF_INFO("Timestamp Mismatch %"PRIu64" != %"PRIu64" %"PRIu64" %"PRIu64"\n",
                    nTimestamp, nExpectedTimestamp, nTimeOffset, m_nCurrentGapLength);
            m_nFrameErrors++;
        }
        else {
            if (m_bInsertScap) {
                InsertSCAP(aFrame);
            }
            m_pMediaWriter->PutFrame(aFrame);

            if (!m_pMediaProcessorList.empty())
            {
                list<CMediaProcessorInterface*>::iterator it;
                for(it = m_pMediaProcessorList.begin();
                        it != m_pMediaProcessorList.end(); ++it)
                {
                    (*it)->ProcessMediaFrame(aFrame);
                }
            }
        }
        break;
    }
    case CCFrame::MediaStarted:
    {
        uint64_t nStartTime = aFrame.GetTimeStamp();
        StartMedia(nStartTime);

        break;
    }
    case CCFrame::MediaStopped:
    {
        uint64_t nStopTime = aFrame.GetTimeStamp();
        StopMedia(nStopTime);

        break;
    }
    case CCFrame::Snapshot:
    {
        m_pMediaWriter->AddSnapshot(aFrame);
        break;
    }
    case CCFrame::Thumbnail:
    {
        m_pMediaWriter->AddThumbnail(aFrame);
        break;
    }
    }
}


void CMinuteFileWriter::Impl::GetLastSCAP(uint64_t requestOffset) {
    uint64_t rollBackOffset = 0;
    if (requestOffset > 10000)
        rollBackOffset = requestOffset - 10000;
    InitializeMediaToTimeOffset(rollBackOffset);
    m_bInsertScap = false;
    while (true) {
        CCFrame aFrame;
        if (!GetFrame(aFrame)) {
            break;
        }
        if (aFrame.GetTimeOffset() >= requestOffset) {
            break;
        }
        if (!LookForScap(aFrame))
            break;
    }
}

bool CMinuteFileWriter::Impl::LookForScap(CCFrame & aFrame) {

    if (aFrame.GetMessageType() != CCFrame::Normal)
        return true;
    if (aFrame.GetStreamType(STREAMTYPE_V2D) != STREAMTYPE_V2D)
        return false;
    CV2DStreamMediaFlag v2dSMediaFlag(aFrame.GetMediaFlag());

    if (v2dSMediaFlag.GetChannel() != FRAMETYPE_V2D_CHAN_CTL)
        return true;
    uint8_t * data = aFrame.GetData();
    int protocol_id = GET_PROTOCOL_UNIT_ID(data);
    if (protocol_id != IDENT_SERVER_CAP) {
        return true;
    }
    delete [] m_scapBuffer;
    m_scapBuffer = new uint8_t[aFrame.GetLength()];
    memcpy(m_scapBuffer, data, aFrame.GetLength());
    m_scapBufferLen = aFrame.GetLength();
    m_bInsertScap = true;
    return true;
}

void CMinuteFileWriter::Impl::InsertSCAP(CCFrame & aFrame) {
    CCFrame newFrame;
    newFrame.SetData(m_scapBuffer);
    newFrame.SetLength(m_scapBufferLen);
    newFrame.SetMessageType(aFrame.GetMessageType());
    newFrame.SetTimeStamp(aFrame.GetTimeStamp());
    newFrame.SetTimeOffset(aFrame.GetTimeOffset());

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
    newFrame.SetMediaFlag(v2dSMediaFlag.GetMediaFlag());

    m_pMediaWriter->PutFrame(newFrame);

    m_bInsertScap = false;
}

void CMinuteFileWriter::Impl::StartMedia(uint64_t nStartTime) {
    if (m_nMediaBeginTimeStamp == 0) {
        m_nMediaBeginTimeStamp = nStartTime;
    }
    m_nMediaStartTime = nStartTime;
    if (m_nMediaStopTime > 0)
        m_nCurrentGapLength += m_nMediaStartTime - m_nMediaStopTime;
    m_pMediaWriter->Start(nStartTime);
    m_bMediaStarted = true;

    if (!m_pMediaProcessorList.empty())
    {
        list<CMediaProcessorInterface*>::iterator it;
        for(it = m_pMediaProcessorList.begin();
                it != m_pMediaProcessorList.end(); ++it)
        {
            (*it)->StartMedia(nStartTime);
        }
    }
}

void CMinuteFileWriter::Impl::StopMedia(uint64_t nStopTime) {
    if (nStopTime < m_nMediaStartTime)
        return;
    m_nMediaStopTime = nStopTime;
    m_pMediaWriter->Stop(nStopTime);
    m_bMediaStarted = false;

    if (!m_pMediaProcessorList.empty())
    {
        list<CMediaProcessorInterface*>::iterator it;
        for(it = m_pMediaProcessorList.begin();
                it != m_pMediaProcessorList.end(); ++it)
        {
            (*it)->StopMedia(nStopTime);
        }
    }
}

void CMinuteFileWriter::Impl::PrintMediaInfo() {
    if (m_pMinuteFileServer != NULL) {
        PRINTF_INFO("Duration: %"PRIu64"\n", m_pMinuteFileServer->GetMediaDuration());
        return;
    }
    PRINTF_INFO("Media Title: %s\n", m_pMatroskaFileParser->GetMediaTitle().c_str());
    PRINTF_INFO("Duration: %"PRIu64"\n", m_pMatroskaFileParser->GetMediaDuration());
    PRINTF_INFO("Stream Type: %s\n", m_pMatroskaFileParser->GetStreamType().c_str());
}

void CMinuteFileWriter::Impl::PrintProgressInfo() {
    if (m_pMinuteFileServer != NULL) {
        if (m_nCurrentTimeOffset - m_nLastFilePosition > 5000) {
            uint64_t mediaDuration = m_pMinuteFileServer->GetMediaDuration();
            if (mediaDuration > 0) {
                PRINTF_INFO("Progress: %.3f secs (%.2f%%)\n",
                        (1.0 * m_nCurrentTimeOffset) / 1000.0,
                        (m_nCurrentTimeOffset * 100.0)/mediaDuration);
            }
            m_nLastFilePosition = m_nCurrentTimeOffset;
        }
        return;
    }
    if (m_nLastFilePosition != m_pMatroskaFileParser->GetCurrentClusterPosition()) {
        PRINTF_INFO("Progress: %.3f secs (%.2f%%)\n",
                (1.0 * m_nCurrentTimeOffset) / 1000.0,
                (m_pMatroskaFileParser->GetCurrentClusterPosition() * 100.0) /
                m_pMatroskaFileParser->GetFileSize());
        m_nLastFilePosition = m_pMatroskaFileParser->GetCurrentClusterPosition();
    }
}

void CMinuteFileWriter::Impl::WriteMetaDataToDbDir()
{
    if (m_pMatroskaFileParser == NULL)
        return;
    if (!m_bExtractMetadata)
        return;
    list<CMetadataHolder> metaDataList = m_pMatroskaFileParser->GetMetadata("all");
    for (list<CMetadataHolder>::iterator iter = metaDataList.begin();
            iter != metaDataList.end(); ++iter) {
        SaveXmlToFile(iter->m_pXmlDoc, iter->m_name, iter->m_val);
    }
}

bool CMinuteFileWriter::Impl::SaveXmlToFile(
        XERCES_NS::DOMDocument* a_pDomDoc,
        std::string a_metaDataType,
        std::string a_metaFileName)
{
    string strAbsFilePath;

    if (a_metaDataType == BLOB_METADATA) {
        strAbsFilePath = m_strDbDir + "/" + a_metaFileName;
    }
    else if(a_metaDataType == INDEX_METADATA) {
        strAbsFilePath = m_strDbDir + "/" + DBDIR + "/" + a_metaFileName;
    }
    else if(a_metaDataType == MSFRL_METADATA) {
        strAbsFilePath = m_strDbDir + "/" + DBDIR + "/" + IPVSDEFAULTDIR +
            "/" + a_metaFileName;
    }
    else {
        return false;
    }

    WriteDomDoc(a_pDomDoc, strAbsFilePath);
    return true;
}

void CMinuteFileWriter::Impl::WriteBookmarksToDbDir()
{
    if (!m_bExtractMetadata)
        return;

    if (m_pEditList == NULL)
        WriteAllBookmarks();
    else
        WriteEditedBookmarks();

}

void CMinuteFileWriter::Impl::WriteAllBookmarks()
{
    if (m_pMatroskaFileParser == NULL)
        return;

    m_pMatroskaFileParser->NeedBookmarkFrames(true);
    InitializeMediaToTimeOffset(0);
    while (true) {
        CCFrame aFrame;
        if (!GetFrame(aFrame)) {
            break;
        }
        HandleBookmarkFrame(aFrame);
    }
}

void CMinuteFileWriter::Impl::WriteEditedBookmarks()
{
    if (m_pMatroskaFileParser == NULL)
        return;
    m_pMatroskaFileParser->NeedBookmarkFrames(true);

    bool bDone = false;
    for (int item = 0; item < m_pEditList->GetItemCount(); ++item) {
        InitializeMediaToTimeOffset(m_pEditList->StartOffset(item));
        while (true) {
            CCFrame aFrame;
            if (!GetFrame(aFrame)) {
                bDone = true;
                break;
            }
            if (aFrame.GetTimeOffset() > m_pEditList->EndOffset(item)) {
                break;
            }
            HandleBookmarkFrame(aFrame);
        }
        if (bDone)
            break;
    }

}

void CMinuteFileWriter::Impl::HandleBookmarkFrame(CCFrame & aFrame) {
    ostringstream filename;
    uint64_t random_number = GenerateUID();
    if (m_bFixedUUIDs) {
        random_number = m_nBookmarkIndex++;
    }
    filename << m_strDbDir << "/" << DBDIR << "/" << IPVSDEFAULTDIR <<
                "/" << "bookmark_" << random_number << ".xml";
    ofstream outfile (filename.str().c_str(), ofstream::binary);
    if (outfile.is_open()) {
        outfile.write((char *)aFrame.GetData(), aFrame.GetLength());
        outfile.close();
    }
}

void CMinuteFileWriter::Impl::SetEditList(CEditList * editList)
{
    if (m_pEditList == NULL)
        m_pEditList = editList;
}

void CMinuteFileWriter::Impl::AddMediaProcessor(
                              CMediaProcessorInterface *a_pMediaProcessor)
{
	m_pMediaProcessorList.push_back(a_pMediaProcessor);
}

}


