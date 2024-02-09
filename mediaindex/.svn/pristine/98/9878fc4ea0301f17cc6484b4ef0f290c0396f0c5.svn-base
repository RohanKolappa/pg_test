
#include <fstream>


#include "ebml/EbmlHead.h"
#include "ebml/EbmlSubHead.h"
#include "ebml/EbmlVoid.h"

#include "matroska/KaxConfig.h"

#include <iostream>
#include <time.h>

#include "MatroskaFileWriterImpl.hh"
#include "MatroskaExtensions.hh"
#include "XMLUtils.hh"
#include "SystemUtils.hh"
#include <string.h>

#include "EditList.hh"

XERCES_CPP_NAMESPACE_USE

using std::exception;

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

namespace mediaindex {

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

CMatroskaFileWriterImpl::CMatroskaFileWriterImpl(
    const string& a_rstrOutFile,
    const string& a_rstrDbDir,
    const string& a_rstrBlobDir
    )
: m_strOutFile(a_rstrOutFile)
, m_strDbDir(a_rstrDbDir)
, m_strBlobDir(a_rstrBlobDir)
, m_pClusterFile(NULL)
{
    m_bFixedUUIDs = false;
    m_bSilent = false;
    m_pKaxCluster = NULL;
    m_pKaxSegment = NULL;
    m_pKaxCues = NULL;
    m_pKaxTags = NULL;
    m_pEbmlVoid = NULL;
    m_pKaxSeekHeadMain = NULL;
    m_pKaxSeekHeadCues = NULL;

    m_nMediaBeginTimeStamp = 0;
    m_nDuration = 0;
    m_pMediaServer = NULL;
    m_nFrameCount = 0;
    m_nTimeOffset = 0;
    m_nClusterTimeOffset = 0;
    m_nCueingTimeOffset = 0;
    m_nFramesInBlock = 0;
    m_nFramesInCluster = 0;
    m_nBytesInCluster = 0;
    m_nBytesSinceCued = 0;
    m_pCurrentBlockGroup = NULL;
    m_nPreviousClusterTimecode = 0;
    m_nBytesWritten = 0;
    m_nClusterCount = 0;
    m_pPrivateData = NULL;
    m_nPrivateDataLen = 0;
    m_strTitle = "Unknown";

    m_nMediaStartTime = 0;
    m_nMediaStopTime = 0;
    m_nCurrentSkipLength = 0;
    m_nLastSkipOffset = 0;

    m_pEditList = NULL;

    m_bInsertScap = false;
    m_scapBuffer = NULL;
    m_scapBufferLen = 0;
    m_bMediaStarted = false;

    m_nDataTrackNumber = 1;
    m_nBookmarksTrackNumber = 2;
    m_nThumbnailsTrackNumber = 3;
    GenerateTrackIds();

    m_bCueBlock = false;
    m_nNextBookmarkOffset = 0xFFFFFFFFFFFFFFFFULL;
    m_pCurrentBlockBlob = NULL;
    m_pTrackEntry = NULL;
    m_pBookmarksTrackEntry = NULL;
    m_bHasThumbnails = false;
}

CMatroskaFileWriterImpl::~CMatroskaFileWriterImpl()
{
    if (m_pMediaServer != NULL)
        delete m_pMediaServer;
    m_lBlockBlobList.clear();
    delete [] m_pPrivateData;

    delete m_pClusterFile;

    delete [] m_scapBuffer;

    DestroyMatroskaObjects();

}

// This ensures nothing goes to standard out
void CMatroskaFileWriterImpl::Silent()
{
    m_bSilent = true;
}

// Used for unit testing. This will create predictable file contents.
void CMatroskaFileWriterImpl::FixedUUIDs()
{
    m_bFixedUUIDs = true;

    GenerateTrackIds();
}

// For testing purpose, we check a special env variable,
// which if set, fixed (non-random) UIDs are used to create
// predictable matroska files.
bool  CMatroskaFileWriterImpl::NeedFixedUIDS()
{
    if (m_bFixedUUIDs)
        return true;

    char *env_value;
    env_value = getenv("FIXED_UID");
    if ((env_value == NULL) || (strcmp(env_value,"1") != 0))
        return false;
    else
        return true;
}

void CMatroskaFileWriterImpl::GenerateTrackIds()
{
    if (NeedFixedUIDS())
        m_nDataTrackUID = 1234;
    else
        m_nDataTrackUID = GenerateUID();

    if (NeedFixedUIDS())
        m_nBookmarkTrackUID = 6789;
    else
        m_nBookmarkTrackUID = GenerateUID();

    if (NeedFixedUIDS())
        m_nThumbnailsTrackUID = 5678;
    else
        m_nThumbnailsTrackUID = GenerateUID();
}

eLIBMC_ERROR CMatroskaFileWriterImpl::Write()
{
    if (!InitMediaServer())
        return ELIBMC_ERROR;
    eLIBMC_ERROR rc = ELIBMC_OK;

    InitXmlLib();

    try {

        CreateMatroskaObjects();
        CreateStreamMetaData();

        CreateOutputFile();

        WriteHeader();
        WriteTrackHeaders();

        WriteRealMediaData();
        FinishFile();

    }
    catch (const std::runtime_error &e) {
        PRINTF_INFO("Critical Error: %s\n", e.what());
        rc = ELIBMC_ERROR;
    }
    catch (exception & Ex) {
        PRINTF_INFO("Some Error: %s\n", Ex.what());
		rc = ELIBMC_ERROR;
    }
    catch (char* pchEx) {
        PRINTF_INFO("Some Exception: %s\n", pchEx);
        rc = ELIBMC_ERROR;
    }

    FreeXmlLib();

    return rc;
}


eLIBMC_ERROR CMatroskaFileWriterImpl::WriteHeader()
{
     try {
        EbmlHead head;

        GetChildAs<EDocType, EbmlString>(head)                = "matroska";
        GetChildAs<EDocTypeVersion, EbmlUInteger>(head)     = MATROSKA_VERSION;
        GetChildAs<EDocTypeReadVersion, EbmlUInteger>(head) = MATROSKA_VERSION;

        head.Render(*m_pClusterFile);

        m_pKaxSegment->WriteHead(*m_pClusterFile, 8);

        KaxInfo * m_pKaxInfos = &GetChild<KaxInfo>(*m_pKaxSegment);

        // Set the Muxing App
        GetChildAs<KaxMuxingApp, EbmlUnicodeString>(m_pKaxInfos)  =
            UTFstring(L"libebml 0.7.8, libmatroska 0.8.1");

        // Set the Writing App
        string writingApp("libmediaconverter ");
        writingApp.append(IPVS_APP_VERSION_STRING);
        UTFstring utfWritingApp;
        utfWritingApp.SetUTF8(writingApp);
        GetChildAs<KaxWritingApp, EbmlUnicodeString>(m_pKaxInfos) = utfWritingApp;

        // Set the Timecode Scale
        GetChildAs<KaxTimecodeScale, EbmlUInteger>(m_pKaxInfos) = G_IPVS_TIMECODESCALE;

        // Set the Duration of the media
        KaxDuration * m_pKaxDuration = new KaxDuration();
        m_pKaxDuration->SetPrecision(EbmlFloat::FLOAT_64);

        *(static_cast<EbmlFloat *>(m_pKaxDuration)) = GetMediaDuration();
        m_pKaxInfos->PushElement(*m_pKaxDuration);

        // Set the production date as the begin timestamp of the media
        // GetChild<KaxDateUTC>(*m_pKaxInfos).SetEpochDate(static_cast<int32>(m_nBeginTimeCode/1000));
        GetChild<MyKaxDateUTC>(*m_pKaxInfos).SetEpochDateInMS(GetMediaBeginTimeStamp());

        // Set the Title of the segment
        string title;
        if (m_strGroupTitle.size() > 0) {
            title.append(m_strGroupTitle);
            title.append(" - ");
        }
        title.append(m_strTitle);
        UTFstring utfTitle;
        utfTitle.SetUTF8(title);
        GetChildAs<KaxTitle, EbmlUnicodeString>(m_pKaxInfos) = utfTitle;

        // Set the segment UUID.
        binary uuid[16];

        if (NeedFixedUIDS())
          memset(uuid, 0xAB, 16);
        else
          GenerateUUID(uuid);

        GetChild<KaxSegmentUID>(*m_pKaxInfos).CopyBuffer(uuid, 16);

         // Set the Segment Filename (basename portion of the output file path)
         UTFstring fileName;
         fileName.SetUTF8(GetFileBaseName(m_strOutFile));
         GetChildAs<KaxSegmentFilename, EbmlUnicodeString>(m_pKaxInfos) = fileName;

        // Set the Segment Family UID
        if (NeedFixedUIDS())
          memset(uuid, 0xCD, 16);
        else
          GenerateUUID(uuid);
        GetChild<KaxSegmentFamily>(*m_pKaxInfos).CopyBuffer(uuid, 16);


        // Reserve some space for the meta seek stuff.
        m_pKaxSeekHeadMain = new KaxSeekHead();
        m_pEbmlVoid = new EbmlVoid();
        m_pEbmlVoid->SetSize(4096);
        m_pEbmlVoid->Render(*m_pClusterFile);


        m_pKaxSeekHeadCues = new KaxSeekHead();

        m_pKaxInfos->Render(*m_pClusterFile, true);
        m_pKaxSeekHeadMain->IndexThis(*m_pKaxInfos, *m_pKaxSegment);



    } catch (...) {
        PRINTF_INFO("The matroska headers could not be rendered correctly\n");
    }

    return ELIBMC_OK;
}

void CMatroskaFileWriterImpl::CreateMatroskaObjects()
{
    m_pKaxSegment       = new KaxSegment();
    m_pKaxCues          = new KaxCues();
    m_pKaxCues->SetGlobalTimecodeScale(G_IPVS_TIMECODESCALE);
    m_pKaxTags          = new KaxTags();

}

void CMatroskaFileWriterImpl::DestroyMatroskaObjects()
{
    delete m_pKaxSegment;
    delete m_pKaxCues;
    delete m_pKaxTags;
    delete m_pEbmlVoid;
    delete m_pKaxSeekHeadMain;
    delete m_pKaxSeekHeadCues;

    m_pKaxSegment = NULL;
    m_pKaxCues = NULL;
    m_pKaxTags = NULL;
    m_pEbmlVoid = NULL;
    m_pKaxSeekHeadMain = NULL;
    m_pKaxSeekHeadCues = NULL;
}

void CMatroskaFileWriterImpl::CreateOutputFile()
{
    m_pClusterFile = new Std64IOCallback(m_strOutFile.c_str(), MODE_CREATE);
}

void CMatroskaFileWriterImpl::FinishFile()
{

    m_pKaxCues->Render(*m_pClusterFile);

    // Render the meta seek information with the cues
    if (m_pKaxSeekHeadCues->ListSize() > 0) {
        m_pKaxSeekHeadCues->UpdateSize();
        m_pKaxSeekHeadCues->Render(*m_pClusterFile);
        m_pKaxSeekHeadMain->IndexThis(*m_pKaxSeekHeadCues, *m_pKaxSegment);
    }

    if (m_pKaxTags && m_pKaxTags->ListSize()) {
      FixMandatoryTagElements(m_pKaxTags);
      m_pKaxTags->UpdateSize();
      m_pKaxTags->Render(*m_pClusterFile, true);
    }

    // Write meta seek information if it is not disabled.
    m_pKaxSeekHeadMain->IndexThis(*m_pKaxCues, *m_pKaxSegment);

    if (m_pKaxTags && m_pKaxTags->ListSize()) {
        m_pKaxSeekHeadMain->IndexThis(*m_pKaxTags, *m_pKaxSegment);
    }

    if (m_pKaxSeekHeadMain->ListSize() > 0) {
        m_pKaxSeekHeadMain->UpdateSize();
        if (m_pEbmlVoid->ReplaceWith(*m_pKaxSeekHeadMain, *m_pClusterFile, true) == 0)
            PRINTF_INFO("This should REALLY not have happened. The space reserved for"
            " the first meta seek element was too small.\n");
    }

    // Set the correct size for the segment.
    if (m_pKaxSegment->ForceSize(m_pClusterFile->getFilePointer() -
        m_pKaxSegment->GetElementPosition() - m_pKaxSegment->HeadSize()))
        m_pKaxSegment->OverwriteHead(*m_pClusterFile);


    // The tracks element must not be deleted.
    for (unsigned int i = 0; i < m_pKaxSegment->ListSize(); ++i)
        if (NULL == dynamic_cast<KaxTracks *>((*m_pKaxSegment)[i]))
            delete (*m_pKaxSegment)[i];
    m_pKaxSegment->RemoveAll();


    delete m_pClusterFile;
    m_pClusterFile = NULL;
}

void CMatroskaFileWriterImpl::WriteTrackHeaders()
{

    // Create the Data track
    m_pTrackEntry = &GetChild<KaxTrackEntry>(m_Tracks);
    GetChildAs<KaxTrackNumber, EbmlUInteger>(m_pTrackEntry) = m_nDataTrackNumber;
    GetChildAs<KaxTrackUID, EbmlUInteger>(m_pTrackEntry) = m_nDataTrackUID;
    GetChildAs<KaxTrackType, EbmlUInteger>(m_pTrackEntry) = IPVS_DATA_TRACK_TYPE;

    m_pTrackEntry->SetGlobalTimecodeScale(G_IPVS_TIMECODESCALE);

    GetChildAs<KaxTrackName, EbmlUnicodeString>(m_pTrackEntry) = L"IPVS Data";

    string codecID = "D_IPVS/DATA/";
    if (m_pMediaServer->IsSnapshot()) {
        m_strStreamType = "SNAPSHOT";
    }
    codecID.append(m_strStreamType);
    GetChildAs<KaxCodecID, EbmlString>(m_pTrackEntry) = codecID;

    // Add Codec Private data
    EbmlBinary & privateData = GetChild<KaxCodecPrivate>(*m_pTrackEntry);
    privateData.CopyBuffer(m_pPrivateData, m_nPrivateDataLen);

    // Create the Bookmarks track
    m_pBookmarksTrackEntry = &GetNextChild<KaxTrackEntry>(m_Tracks, *m_pTrackEntry);
    GetChildAs<KaxTrackNumber, EbmlUInteger>(m_pBookmarksTrackEntry) = m_nBookmarksTrackNumber;
    GetChildAs<KaxTrackUID, EbmlUInteger>(m_pBookmarksTrackEntry) = m_nBookmarkTrackUID;
    GetChildAs<KaxTrackType, EbmlUInteger>(m_pBookmarksTrackEntry) = IPVS_BOOKMARKS_TRACK_TYPE;
    m_pBookmarksTrackEntry->SetGlobalTimecodeScale(G_IPVS_TIMECODESCALE);

    GetChildAs<KaxTrackName, EbmlUnicodeString>(m_pBookmarksTrackEntry) = L"IPVS Bookmarks";
    GetChildAs<KaxCodecID, EbmlString>(m_pBookmarksTrackEntry)     = "D_IPVS/BKMK10";

    // Add the same Codec Private data for bookmarks
    EbmlBinary & bookmarkPrivateData = GetChild<KaxCodecPrivate>(*m_pBookmarksTrackEntry);
    bookmarkPrivateData.CopyBuffer(m_pPrivateData, m_nPrivateDataLen);

    // Create the Thumbnails track
    if (m_bHasThumbnails) {
        m_pThumbnailsTrackEntry = &GetNextChild<KaxTrackEntry>(m_Tracks, *m_pBookmarksTrackEntry);
        GetChildAs<KaxTrackNumber, EbmlUInteger>(m_pThumbnailsTrackEntry) = m_nThumbnailsTrackNumber;
        GetChildAs<KaxTrackUID, EbmlUInteger>(m_pThumbnailsTrackEntry) = m_nThumbnailsTrackUID;
        GetChildAs<KaxTrackType, EbmlUInteger>(m_pThumbnailsTrackEntry) = IPVS_THUMBNAILS_TRACK_TYPE;
        m_pThumbnailsTrackEntry->SetGlobalTimecodeScale(G_IPVS_TIMECODESCALE);

        GetChildAs<KaxTrackName, EbmlUnicodeString>(m_pThumbnailsTrackEntry) = L"IPVS Thumbnails";
        GetChildAs<KaxCodecID, EbmlString>(m_pThumbnailsTrackEntry)     = "D_IPVS/THMB10";

        // Add the same Codec Private data for thumbnail
        EbmlBinary & thumbnailPrivateData = GetChild<KaxCodecPrivate>(*m_pThumbnailsTrackEntry);
        thumbnailPrivateData.CopyBuffer(m_pPrivateData, m_nPrivateDataLen);
    }

    // Render Tracks
    m_pKaxSegment->PushElement(m_Tracks);
    m_Tracks.Render(*m_pClusterFile, false);
    m_pKaxSeekHeadMain->IndexThis(m_Tracks, *m_pKaxSegment);
}


//#define AUTO_FRAMING
#define SIMPLE_BLOCK
void CMatroskaFileWriterImpl::GetNewBlockGroup()
{

#ifdef SIMPLE_BLOCK
    m_pCurrentBlockBlob = new KaxBlockBlob(BLOCK_BLOB_ALWAYS_SIMPLE);
    m_lBlockBlobList.push_back(m_pCurrentBlockBlob); // To be able to free later

    m_pCurrentBlockBlob->SetParent(*m_pKaxCluster);
    m_pKaxCluster->AddBlockBlob(m_pCurrentBlockBlob);


#else
//    PRINTF_DEBUG("New Block frames=%u %u clusters=%d tc=%llu\n",
//            m_nFramesInBlock, m_nFramesInCluster, m_nClusterCount, m_nTimeOffset);
    KaxBlockGroup &pNewBlockGroup = m_pKaxCluster->GetNewBlock();
    m_pCurrentBlockGroup = &pNewBlockGroup;
    m_nFramesInBlock = 0;
#endif
}


void CMatroskaFileWriterImpl::GetNewCluster()
{
    m_pKaxCluster = new KaxCluster();
    m_pKaxCluster->SetParent(*m_pKaxSegment);

//    m_pKaxCluster->SetPreviousTimecode((m_nPreviousClusterTimecode * G_IPVS_TIMECODESCALE),
//            G_IPVS_TIMECODESCALE);
    m_pKaxCluster->InitTimecode(m_nPreviousClusterTimecode, G_IPVS_TIMECODESCALE);
    PRINTF_DEBUG("New Cluster bytes = %llu tc = %llu, gtc=%llu\n",
            m_nBytesWritten, m_nTimeOffset, m_pKaxCluster->GlobalTimecode());
    m_nClusterTimeOffset = m_nTimeOffset;
    m_nFramesInCluster = 0;
    m_nBytesInCluster = 0;
    m_nClusterCount++;
    m_pCurrentBlockGroup = NULL;

    m_bCueBlock = true; // Fist block in a cluster is always cued

    if (m_nDuration > 0) {
        PRINTF_INFO("Progress: %.3f secs (%.2f%%)\n",
                ((m_nTimeOffset * G_IPVS_TIMECODESCALE) / M_NANOSEC),
                (m_nTimeOffset * 100.0)/m_nDuration);
    }

}

void CMatroskaFileWriterImpl::CloseCuster()
{
    // Write out cluster
    PRINTF_DEBUG("Closing cluster tc = %llu gtc=%llu size=%u\n",
            m_nTimeOffset, m_pKaxCluster->GlobalTimecode(), m_nBytesInCluster);
    m_pKaxCluster->Render(*m_pClusterFile, *m_pKaxCues);
    m_pKaxSeekHeadCues->IndexThis(*m_pKaxCluster, *m_pKaxSegment);

    m_nPreviousClusterTimecode = m_nTimeOffset;

    // Cleanup cluster
    m_pKaxCluster->ReleaseFrames();
    delete m_pKaxCluster;

#ifdef SIMPLE_BLOCK
    int listSize = m_lBlockBlobList.size();
    for (int i = 0; i < listSize; ++i) {
        // Cannot free resources. Seems like a bug in libmatroska
        // Fix looks like commenting out the implementation of
        // destructor of KaxBlockBlob (KaxBlock.h, line 316)

        delete m_lBlockBlobList[i];
    }
    m_lBlockBlobList.clear();
#endif
    m_pKaxCluster = NULL;
}

static bool FreeBufferCallback(const DataBuffer & aBuffer)
{
    binary *data = (binary *) aBuffer.Buffer();
    //PRINTF_INFO("deleting buffer of size %d\n", aBuffer.Size());
    if (data != NULL)
        delete [] data;
    return true;
}

void CMatroskaFileWriterImpl::AddFrame(KaxTrackEntry * a_pTrackEntry,
        uint64_t a_nTimeOffset, uint8_t *a_pData, int a_nLength)
{
    DataBuffer *pDataBuff =
            new DataBuffer((binary *)a_pData, a_nLength, FreeBufferCallback);
#ifdef AUTO_FRAMING
    KaxBlockGroup *bg = NULL;
    m_pKaxCluster->AddFrame(*m_pTrackEntry,
            a_nTimeOffset * G_IPVS_TIMECODESCALE, *pDataBuff, bg);
#else
#ifdef SIMPLE_BLOCK
    m_pCurrentBlockBlob->AddFrameAuto(*a_pTrackEntry,
            a_nTimeOffset * G_IPVS_TIMECODESCALE, *pDataBuff);

    // Cue this frame's block if needed
    if (m_bCueBlock) {
        m_pKaxCues->AddBlockBlob(*m_pCurrentBlockBlob);
        m_bCueBlock = false;
        m_nBytesSinceCued = 0;
        m_nCueingTimeOffset = a_nTimeOffset;
    }

#else
    m_pCurrentBlockGroup->AddFrame(*m_pTrackEntry,
            a_nTimeOffset * G_IPVS_TIMECODESCALE, *pDataBuff);
#endif
#endif
    m_nFramesInBlock++;
    m_nFramesInCluster++;
    m_nBytesInCluster += a_nLength;
    m_nBytesWritten += a_nLength;
    m_nBytesSinceCued += a_nLength;

}

bool CMatroskaFileWriterImpl::AddBookmarkFrameFromFile(string a_strFileName, uint64_t a_nTimeOffset)
{
    // Read the bookmark file and add contents as a frame
    std::ifstream file(a_strFileName.c_str(), std::ios::binary);
    if (file.is_open()) {
        file.seekg(0, ios::end);
        int length = (int)file.tellg();
        file.seekg(0, ios::beg);

        GetNewBlockGroup();
        m_nTimeOffset = a_nTimeOffset;

        m_bCueBlock = true; // bookmarks are always cued

        uint8_t *data = new uint8_t[length];
        file.read((char *)data, length);
        AddFrame(m_pBookmarksTrackEntry, m_nTimeOffset, data, length);

        file.close();
        return true;
    }
    return false;
}

void CMatroskaFileWriterImpl::AddRemainingBookmarks(uint64_t a_nTimeOffset)
{

    // Any bookmarks left?
    while (!m_lBookmarksList.empty()) {

        // Add the bookmark
        BookmarksData  bookmarkData = m_lBookmarksList.front();
        PRINTF_DEBUG("Handling a remaining bookmark at %llu : %llu\n",
                bookmarkData.m_nTimeOffset, a_nTimeOffset);

        AddBookmarkFrameFromFile(bookmarkData.m_strFileName, a_nTimeOffset);

        m_lBookmarksList.pop_front();
    }

}
bool CMatroskaFileWriterImpl::AddBookmarkFrames(uint64_t a_nTimeOffset)
{
    // Is next bookmark in the future?
    if (m_nNextBookmarkOffset > a_nTimeOffset)
        return false;

    // Anymore bookmarks left?
    if (m_lBookmarksList.empty()) {
        m_nNextBookmarkOffset = 0xFFFFFFFFFFFFFFFFULL;
        return false;
    }

    // Add next bookmark whose time is less than the current time.
    BookmarksData  bookmarkData = m_lBookmarksList.front();
    PRINTF_DEBUG("Handling a bookmark at %llu : %llu\n",
            bookmarkData.m_nTimeOffset, a_nTimeOffset);

    // Read the bookmark file and add contents as a frame
    AddBookmarkFrameFromFile(bookmarkData.m_strFileName, a_nTimeOffset);

    // Get ready for next bookmark
    m_lBookmarksList.pop_front();
    if (m_lBookmarksList.empty())
        m_nNextBookmarkOffset = 0xFFFFFFFFFFFFFFFFULL;
    else
        m_nNextBookmarkOffset = m_lBookmarksList.front().m_nTimeOffset;
    PRINTF_DEBUG("Next bookmark at %llu\n", m_nNextBookmarkOffset);

    return true;
}

void CMatroskaFileWriterImpl::AddTimeStampsToAllGaps(uint8_t ** a_pData, uint64_t a_nStartTime,
                                                     uint64_t a_nEndTime)
{
    std::list<CGap> :: iterator iter;
    for (iter = m_lGaps.begin(); iter != m_lGaps.end(); ++iter) {
        CGap gap = (*iter);
        if (a_nStartTime < gap.GetBeginTime()) {
            if (a_nEndTime > gap.GetEndTime()) {
                PRINTF_DEBUG("Gap: %llu-%llu\n", gap.GetBeginTime(), gap.GetEndTime());

                *(uint64_t *)(*a_pData) = gap.GetBeginTime();
                *a_pData += IPVS_TIMESTAMP_SIZE;
                *(uint64_t *)(*a_pData) = gap.GetEndTime();
                *a_pData += IPVS_TIMESTAMP_SIZE;
            }
        }
    }
}

void CMatroskaFileWriterImpl::CreatePrivateData()
{
    int nFrameSize = IPVS_FRAME_HEADER_SIZE + 2 * IPVS_TIMESTAMP_SIZE * (m_lGaps.size() + 1);
    if (m_pEditList != NULL) {
        nFrameSize += (2 * IPVS_TIMESTAMP_SIZE * m_pEditList->GetItemCount()) ;
    }

    m_pPrivateData = new uint8_t[nFrameSize];

    // Add frame header
    *(uint8_t *)m_pPrivateData = IPVS_PRIVATE_DATA_CURRENT_VERSION;
    uint8_t *data = m_pPrivateData + IPVS_PRIVATE_DATA_VERSION_LENGTH;

    // Add media start and end timestamps
    *(uint64_t *)(data) = GetMediaBeginTimeStamp();
    data += IPVS_TIMESTAMP_SIZE;
    *(uint64_t *)(data) = GetMediaEndTimeStamp();
    data += IPVS_TIMESTAMP_SIZE;

    // Add timestamps for all the gaps
    uint64_t nStartTimeStamp = 0, nEndTimeStamp = 0;
    if (m_pEditList != NULL) {
        int nTotalItemCount = m_pEditList->GetItemCount();
        for (int item = 0; item < nTotalItemCount; ++item) {
            nStartTimeStamp = m_pMediaServer->GetTimecodeAtOffset(m_pEditList->StartOffset(item));
            nEndTimeStamp = m_pMediaServer->GetTimecodeAtOffset(m_pEditList->EndOffset(item));

            // Add all the existing gaps, if these are in the the edit list range.
            AddTimeStampsToAllGaps(&data, nStartTimeStamp, nEndTimeStamp);

            // If there are more cut lists, then inserting gap in between.
            if (nTotalItemCount > item + 1) {
                *(uint64_t *)(data) = nEndTimeStamp;
                data += IPVS_TIMESTAMP_SIZE;
                *(uint64_t *)(data) = m_pMediaServer->GetTimecodeAtOffset(m_pEditList->StartOffset(item + 1));
                data += IPVS_TIMESTAMP_SIZE;
            }
        }
    }
    else {
        //Add all the existing gaps.
        AddTimeStampsToAllGaps(&data, GetMediaBeginTimeStamp(), GetMediaEndTimeStamp());
    }
    // Compute length of frame.
    m_nPrivateDataLen = data - m_pPrivateData;
}

void CMatroskaFileWriterImpl::WriteAllMediaFrames()
{
    InitializeMediaToTimeOffset(0);

    while (true) {
        CCFrame aFrame;
        if (!GetFrame(aFrame)) {
            break;
        }
        HandleMediaFrame(aFrame);
    }
}

void CMatroskaFileWriterImpl::WriteEditedMediaFrames()
{
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
                GenerateStopFrame(m_pMediaServer->GetTimecodeAtOffset(m_pEditList->EndOffset(item)));
                break;
            }
            HandleMediaFrame(aFrame);
        }
        if (bDone)
            break;
    }
}

void CMatroskaFileWriterImpl::WriteRealMediaData()
{
    GetNewCluster();

    if (m_pEditList == NULL) {
        WriteAllMediaFrames();
    }
    else {
        WriteEditedMediaFrames();
    }

    AddRemainingBookmarks(m_nTimeOffset);

    PRINTF_INFO("Packed %"PRIu64" frames\n", m_nFrameCount);
    CloseCuster();
}

void CMatroskaFileWriterImpl::CreateStreamMetaData()
{
    if (m_strDbDir == "") {
        PRINTF_INFO("No metadata dir specified, skipping metadata...\n");
        return;
    }

    std::vector<string> strMetaDataFileList;
#if 1
    if (GetMetaDataFileList(strMetaDataFileList, m_strDbDir, m_strBlobDir)
            == false ) {
        const string strExMsg = "Either the metadata dir or it's subdir is"
            " missing";
        throw( std::runtime_error( strExMsg ) ) ;
    }
#else
        const string strExMsg = "Either the metadata dir or it's subdir is"
            " missing";
        throw( std::runtime_error( strExMsg ) ) ;
#endif

    //Sort all the files in ascending order.
    std::sort(strMetaDataFileList.begin(), strMetaDataFileList.end());

    // Creating Tag and TagTrackUID.
    KaxTag *pKaxTag = new KaxTag();
    KaxTagTargets *pKaxTarget = &GetChild<KaxTagTargets>(*pKaxTag);
    GetChildAs<KaxTagTrackUID, EbmlUInteger>(pKaxTarget) = m_nDataTrackUID;
    GetChildAs<KaxTagTargetType, EbmlString>(pKaxTarget) = IPVSTAG;


    // Processing each file from the file list .
    for (vector<string>::const_iterator citer = strMetaDataFileList.begin();
            citer < strMetaDataFileList.end(); ++citer) {
        string fileName = (*citer);
        XercesDOMParser domParser;
        domParser.setValidationScheme(XercesDOMParser::Val_Never);
        domParser.setDoNamespaces(false);
        domParser.setDoSchema(false);
        domParser.setLoadExternalDTD(false);
        domParser.parse(fileName.c_str());

        DOMDocument* pXmlDoc = domParser.getDocument() ;
        if (pXmlDoc == NULL) {
            const string strExMsg = "Invalid or missing XML file: " + (*citer);
            throw( std::runtime_error(strExMsg));
        }

        DOMElement* pElementConfig = pXmlDoc->getDocumentElement() ;
        if (NULL == pElementConfig) {
            continue;
        }

        // Deal with bookmark files
        if (XmlFileContainsBookmark(pXmlDoc)) {
            ProcessBookmarkFile(pXmlDoc, fileName);
            continue;
        }

        // Filter out meaningless files
        if (!XmlFileContainsMetaData(pXmlDoc))
            continue;

        ProcessXmlFileElements(pXmlDoc);

        PRINTF_INFO("Tagging metadata from file: %s\n", (*citer).c_str());

        // Create a simple tag with [name, val] set to the xml root element
        // name and file path resp.
        KaxTagSimple& rSimpleTag =
            CreateSimpleTagForXml(*pKaxTag, pXmlDoc, *citer);

        XmlToEbml(pElementConfig, rSimpleTag);
    }

    // Sort all the bookmarks by time offset
    m_lBookmarksList.sort();
    if (!m_lBookmarksList.empty()) {
        m_nNextBookmarkOffset = m_lBookmarksList.front().m_nTimeOffset;
    }

    m_pKaxTags->PushElement(*pKaxTag);
}

void CMatroskaFileWriterImpl::ProcessXmlFileElements(const DOMDocument* a_pXmlDoc)
{
    // Process each xml document to extract any info we need to put
    // elsewhere in our Matroska container
    DOMElement* pRootElem = a_pXmlDoc->getDocumentElement();
    StrX strElName(pRootElem->getTagName());


    if (XMLString::equals(strElName.localForm(), MSFILE_ELEMENT)) {
        string title = GetTextValueOfChild(pRootElem, "Title");
        if (title.size() > 0) {
            m_strTitle = title;
            PRINTF_DEBUG("Found Title = %s\n", m_strTitle.c_str());
        }
        string streamType = GetTextValueOfChild(pRootElem, "StreamType");
        if (streamType.size() > 0) {
            m_strStreamType = streamType;
            PRINTF_DEBUG("Found StreamType = %s\n", m_strStreamType.c_str());
        }
    }

    if (XMLString::equals(strElName.localForm(), MSFRL_ELEMENT)) {
        DOMElement* element = GetChildElement(GetChildElement(pRootElem, "Info"), "MediaStoreFileInfo");
        if (element != NULL) {
            string type = GetTextValueOfChild(element, "Type");
            if (type.size() > 0) {
                if (strcmp(type.c_str(), "MediaGroup") == 0) {
                    string title = GetTextValueOfChild(GetChildElement(pRootElem, "Info"), "Title");
                    m_strGroupTitle = title;
                    PRINTF_DEBUG("Found Title = %s\n", m_strGroupTitle.c_str());
                }
            }
        }
    }


}

// Bookmark gives the Timestamp. We make a list of offset .
void CMatroskaFileWriterImpl::ProcessBookmarkFile(const DOMDocument* a_pXmlDoc, string a_strFileName)
{
    DOMElement* pRootElem = a_pXmlDoc->getDocumentElement();
    StrX strElName(pRootElem->getTagName());

    // Get the timecode from the bookmark
    string  strTimeCode = GetTextValueOfChild(pRootElem, "AtTC");
    uint64_t nTimecode = STRTOULL(strTimeCode.c_str(), NULL, 10);

    // Convert to offset from beginning
    uint64_t offset = m_pMediaServer->GetOffsetFromTimecode(nTimecode);
    if (offset > m_nDuration)
        offset = m_nDuration;

    // Insert into list (will be later sorted by offset)
    PRINTF_DEBUG("Got a bookmark in %s at %s %llu\n",
            a_strFileName.c_str(), strTimeCode.c_str(), offset);
    BookmarksData boomarksData(offset, a_strFileName);
    m_lBookmarksList.push_back(boomarksData);
}

bool CMatroskaFileWriterImpl::InitMediaServer() {

    // Create the MediaServer object
    m_pMediaServer = CMediaServer::Create(m_strBlobDir.c_str());
    if (m_pMediaServer == NULL) {
        PRINTF_INFO("Could not create MediaServer object!!\n");
        return false;
    }
    m_pMediaServer->ServeThumbnails(true);

    // Initialize the MediaServer object
    if (m_pMediaServer->Init() != MSRC_OK) {
        PRINTF_INFO("MediaServer Init Failed for %s: %s\n",
                m_strBlobDir.c_str(),
                m_pMediaServer->GetErrorString());
        return false;
    }

    m_nMediaBeginTimeStamp = m_pMediaServer->GetMediaStartTimestamp();
    m_nDuration = m_pMediaServer->GetOffsetFromTimecode(m_pMediaServer->GetMediaEndTimestamp());
    m_lGaps = m_pMediaServer->GetGaps();
    m_bHasThumbnails = m_pMediaServer->HasThumbnails();

    CreatePrivateData();
    return true;
}

KaxTagSimple& CreateSimpleTagForXml(
        KaxTag& a_rKaxTag,
        const DOMDocument* a_pDomDoc,
        const string& a_rstrXmlFile)
{
    string strTagName;
    string strTagVal( GetFileBaseName(a_rstrXmlFile) );

    const string strElName =
        StrX(a_pDomDoc->getDocumentElement()->getTagName()).localForm();

    if (strElName == INDEX_ELEMENT) {
        strTagName = INDEX_METADATA;
    }
    else if (strElName == MSFRL_ELEMENT) {
        strTagName = MSFRL_METADATA;
    }
    else if (strElName == MSFILE_ELEMENT) {
        strTagName = BLOB_METADATA;
    }
    else {
        assert(0);
    }

    return AddSimpleTag(a_rKaxTag, strTagName, strTagVal);
}

}

void CMatroskaFileWriterImpl::SetEditList(CEditList * editList)
{
    if (m_pEditList == NULL)
        m_pEditList = editList;
}

std::string CMatroskaFileWriterImpl::GetStreamType()
{
    if (m_pMediaServer != NULL) {
        // Minute files are assumed to be V2D
        // Searching for SCAP breaks out early if any non V2D packet is detected
        return "V2D";
    }
    return "";
}

void CMatroskaFileWriterImpl::InitializeMediaToTimeOffset(uint64_t timeOffset)
{
    if (m_pMediaServer != NULL) {
        m_pMediaServer->Stop();
        m_pMediaServer->Start(m_pMediaServer->GetTimecodeAtOffset(timeOffset));
    }
}

void CMatroskaFileWriterImpl::GetLastSCAP(uint64_t requestOffset)
{
    uint64_t rollBackOffset = 0;
    if (requestOffset > 10000) {
        rollBackOffset = requestOffset - 10000;
    }

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

bool CMatroskaFileWriterImpl::LookForScap(CCFrame & aFrame)
{
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

void CMatroskaFileWriterImpl::InsertSCAP(CCFrame & aFrame)
{
    CCFrame newFrame;
    newFrame.SetData(m_scapBuffer);
    newFrame.SetLength(m_scapBufferLen);
    newFrame.SetMessageType(aFrame.GetMessageType());
    newFrame.SetTimeStamp(aFrame.GetTimeStamp());
    newFrame.SetTimeOffset(aFrame.GetTimeOffset());

    CV2DStreamMediaFlag v2dSMediaFlag;
    v2dSMediaFlag.SetChannel(FRAMETYPE_V2D_CHAN_CTL);
    newFrame.SetMediaFlag(v2dSMediaFlag.GetMediaFlag());

    uint8_t *data = new uint8_t[m_scapBufferLen + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE];
    *(uint32_t *)(data) = newFrame.GetMediaFlag(); // little-endian
    *(uint64_t *)(data + IPVS_FRAME_HEADER_SIZE) = newFrame.GetTimeStamp(); // little-endian
    memcpy(data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE, m_scapBuffer, m_scapBufferLen);

    AddFrame(m_pTrackEntry, m_nTimeOffset, data,
            m_scapBufferLen + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE);

    m_bInsertScap = false;
}

void CMatroskaFileWriterImpl::StartMedia(uint64_t nStartTime)
{
    if (m_nMediaBeginTimeStamp == 0) {
        m_nMediaBeginTimeStamp = nStartTime;
    }
    m_nMediaStartTime = nStartTime;

    m_bMediaStarted = true;
}

void CMatroskaFileWriterImpl::StopMedia(uint64_t nStopTime)
{
    if (nStopTime < m_nMediaStartTime)
        return;
    m_nMediaStopTime = nStopTime;

    m_bMediaStarted = false;
}

void CMatroskaFileWriterImpl::GenerateStopFrame(uint64_t nStopTime)
{
    CCFrame newFrame;
    newFrame.SetData(NULL);
    newFrame.SetLength(0);
    newFrame.SetMessageType(CCFrame::MediaStopped);
    newFrame.SetTimeStamp(nStopTime);
    newFrame.SetTimeOffset(m_nTimeOffset + m_nCurrentSkipLength);

    HandleMediaFrame(newFrame);
}

void CMatroskaFileWriterImpl::HandleMediaFrame(CCFrame & cframe)
{
    if (cframe.GetMessageType() == CCFrame::Snapshot) {
        return HandleSnapshotFrame(cframe);
    }

REPEAT:
    // Got a new frame
    m_nFrameCount++;
    if (!m_bMediaStarted) {
        StartMedia(cframe.GetTimeStamp());
    }

    uint64_t nTimeOffset = AdjustTimeOffset(cframe.GetTimeOffset());

    if (nTimeOffset > m_nNextBookmarkOffset) {
        nTimeOffset = m_nNextBookmarkOffset;
    }

    // Move on to new cluster if needed
    if ((nTimeOffset - m_nClusterTimeOffset > G_IPVS_MAX_CLUSTER_DURATION)
            || m_nBytesInCluster > G_IPVS_MAX_CLUSTER_SIZE) {

        CloseCuster();
        if ((nTimeOffset - m_nPreviousClusterTimecode) > 1000) {
            // Too much silence
            PRINTF_DEBUG("Adjusting next cluster's timecode due to silence: \n");
            m_nPreviousClusterTimecode = nTimeOffset - 1000;
        }
        GetNewCluster();
    }

    // Determine if we need to cue the next frame
    if ((nTimeOffset - m_nCueingTimeOffset > G_IPVS_CUEING_PERIOD)
            || m_nBytesSinceCued > G_IPVS_CUEING_SIZE) {
        m_bCueBlock = true;
    }

    if (AddBookmarkFrames(nTimeOffset)) {
        goto REPEAT;
    }

    if (cframe.GetMessageType() == CCFrame::Normal) {
        if (nTimeOffset < m_nTimeOffset) {
            PRINTF_INFO("Media Error: Time rolled back\n");
            return;
        }
        HandleNormalFrame(cframe, nTimeOffset);
    }
    else if (cframe.GetMessageType() == CCFrame::Thumbnail) {
        m_bCueBlock = true; // Thumbnails are always cued
        HandleThumbnailFrame(cframe, nTimeOffset);
    }
    else {
        HandleStartStopFrame(cframe, nTimeOffset);
    }
}

void CMatroskaFileWriterImpl::HandleNormalFrame(CCFrame & cframe, uint64_t nTimeOffset)
{
    // Normal frame stores the data into the current block
    // Keep all frames with the same timestamp in the same
    // block
    if (nTimeOffset > m_nTimeOffset ||
            m_nFramesInBlock >= G_IPVS_MAX_FRAMES_PER_BLOCK ||
            m_pCurrentBlockGroup == NULL)
        GetNewBlockGroup();
    m_nTimeOffset = nTimeOffset;

    uint8_t *data = new uint8_t[cframe.GetLength() + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE];
    *(uint32_t *)(data) = cframe.GetMediaFlag(); // little-endian
    *(uint64_t *)(data + IPVS_FRAME_HEADER_SIZE) = cframe.GetTimeStamp(); // little-endian
    memcpy(data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE, cframe.GetData(), cframe.GetLength());

    if (m_bInsertScap) {
        InsertSCAP(cframe);
    }

    AddFrame(m_pTrackEntry, m_nTimeOffset, data,
            cframe.GetLength() + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE);
}

void CMatroskaFileWriterImpl::HandleStartStopFrame(CCFrame & cframe, uint64_t nTimeOffset)
{
    if (cframe.GetMessageType() == CCFrame::MediaStopped) {
        // Store stop timestamp
        PRINTF_DEBUG("MediaReader Stopped at %llu\n", cframe.GetTimeStamp());
        GetNewBlockGroup();
        m_nTimeOffset = nTimeOffset;
        uint8_t *data = new uint8_t[IPVS_TIMESTAMP_SIZE + IPVS_FRAME_HEADER_SIZE];
        *(uint8_t *)data = IPVS_FRAME_TYPE_MEDIA_STOP;
        memcpy(data + 1, "IPV", 3);
        *(uint64_t *)(data + IPVS_FRAME_HEADER_SIZE) = cframe.GetTimeStamp(); // little-endian
        AddFrame(m_pTrackEntry, m_nTimeOffset, data, IPVS_TIMESTAMP_SIZE + IPVS_FRAME_HEADER_SIZE);
        StopMedia(cframe.GetTimeStamp());
    }
    else if (cframe.GetMessageType() == CCFrame::MediaStarted) {
        // Store start timestamp
        PRINTF_DEBUG("MediaReader Started at %llu\n", cframe.GetTimeStamp());
        if (m_nFrameCount == 1) {
            if (m_nMediaBeginTimeStamp != cframe.GetTimeStamp()) {
                PRINTF_INFO("Unexpected Start Timestamp\n");
                m_nMediaBeginTimeStamp = cframe.GetTimeStamp();
            }
        }
        GetNewBlockGroup();
        m_nTimeOffset = nTimeOffset;
        uint8_t *data = new uint8_t[IPVS_TIMESTAMP_SIZE + IPVS_FRAME_HEADER_SIZE];
        *(uint8_t *)data = IPVS_FRAME_TYPE_MEDIA_START;
        memcpy(data + 1, "IPV", 3);
        *(uint64_t *)(data + IPVS_FRAME_HEADER_SIZE) = cframe.GetTimeStamp(); // little-endian
        AddFrame(m_pTrackEntry, m_nTimeOffset, data, IPVS_TIMESTAMP_SIZE + IPVS_FRAME_HEADER_SIZE);
    }
}

void CMatroskaFileWriterImpl::HandleThumbnailFrame(CCFrame & cframe, uint64_t nTimeoffset)
{
    if (cframe.GetMessageType() != CCFrame::Thumbnail)
        return;
    m_nFrameCount++;

    // Offset is always zero for snapshots
    m_nTimeOffset = nTimeoffset;
    GetNewBlockGroup();
    uint8_t *data = new uint8_t[cframe.GetLength() +
                                IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE +
                                IPVS_IMAGE_HEADER_SIZE];
    *(uint8_t *)data = IPVS_FRAME_TYPE_MEDIA_THUMBNAIL;
    memcpy(data + 1, "IPV", 3);
    *(uint64_t *)(data + IPVS_FRAME_HEADER_SIZE) = cframe.GetTimeStamp(); // little-endian
    *(uint32_t *)(data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE) = cframe.GetMediaFlag(); // little-endian
    memcpy(data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE + IPVS_IMAGE_HEADER_SIZE,
            cframe.GetData(), cframe.GetLength());

    AddFrame(m_pThumbnailsTrackEntry, m_nTimeOffset, data,
            IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE +
            IPVS_IMAGE_HEADER_SIZE + cframe.GetLength());
}

void CMatroskaFileWriterImpl::HandleSnapshotFrame(CCFrame & cframe)
{
    m_nFrameCount++;
    if (!m_bMediaStarted) {
        StartMedia(cframe.GetTimeStamp());
    }

    if (cframe.GetMessageType() != CCFrame::Snapshot)
        return;

    // Offset is always zero for snapshots
    m_nTimeOffset = 0;

    GetNewBlockGroup();
    uint8_t *data = new uint8_t[cframe.GetLength() +
                                IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE +
                                IPVS_IMAGE_HEADER_SIZE];
    *(uint8_t *)data = IPVS_FRAME_TYPE_MEDIA_SNAPSHOT;
    memcpy(data + 1, "IPV", 3);
    *(uint64_t *)(data + IPVS_FRAME_HEADER_SIZE) = cframe.GetTimeStamp(); // little-endian
    *(uint32_t *)(data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE) = cframe.GetMediaFlag(); // little-endian
    memcpy(data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE + IPVS_IMAGE_HEADER_SIZE,
            cframe.GetData(), cframe.GetLength());

    AddFrame(m_pTrackEntry, m_nTimeOffset, data,
            IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE +
            IPVS_IMAGE_HEADER_SIZE + cframe.GetLength());
}


bool CMatroskaFileWriterImpl::GetFrame(CCFrame & aFrame)
{
    return (m_pMediaServer->GetFrame(aFrame) == MSRC_OK);
}

uint64_t CMatroskaFileWriterImpl::GetMediaBeginTimeStamp()
{
    if (m_pEditList != NULL) {
        return m_pMediaServer->GetTimecodeAtOffset(m_pEditList->StartOffset(0));
    }
    else {
        return m_pMediaServer->GetTimecodeAtOffset(0);
    }
}

uint64_t CMatroskaFileWriterImpl::GetMediaEndTimeStamp()
{
    uint64_t nEndOffset = 0;
    if (m_pEditList != NULL) {
        for (int item = 0; item < m_pEditList->GetItemCount(); ++item) {
            nEndOffset = m_pEditList->EndOffset(item);
        }
        return (m_pMediaServer->GetTimecodeAtOffset(nEndOffset));
    }
    else {
        return m_pMediaServer->GetMediaEndTimestamp();
    }
}

uint64_t CMatroskaFileWriterImpl::GetMediaDuration()
{
    if (m_pEditList != NULL) {
        uint64_t nDuration = 0;
        for (int item = 0; item < m_pEditList->GetItemCount(); ++item) {
            nDuration += (m_pEditList->EndOffset(item) - m_pEditList->StartOffset(item));
        }
        return nDuration;
    }
    else {
        return (m_pMediaServer->GetOffsetFromTimecode(m_pMediaServer->GetMediaEndTimestamp()));
    }
}

uint64_t CMatroskaFileWriterImpl::AdjustTimeOffset(uint64_t a_nTimeOffset)
{
    return (a_nTimeOffset - m_nCurrentSkipLength);
}
