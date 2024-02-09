
#include "CommonIncludes.hh"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>

#include "ebml/EbmlId.h"
#include <ebml/EbmlStream.h>
#include <ebml/EbmlHead.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlVersion.h>
#include <ebml/EbmlVoid.h>
#include "ebml/EbmlContexts.h"

#include <matroska/FileKax.h>
#include <matroska/KaxAttached.h>
#include <matroska/KaxAttachments.h>
#include <matroska/KaxBlock.h>
#include <matroska/KaxChapters.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxClusterData.h>
#include <matroska/KaxCues.h>
#include <matroska/KaxCuesData.h>
#include <matroska/KaxInfo.h>
#include <matroska/KaxInfoData.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxTags.h>
#include <matroska/KaxTag.h>
#include <matroska/KaxTracks.h>
#include <matroska/KaxTrackEntryData.h>
#include <matroska/KaxTrackAudio.h>
#include <matroska/KaxTrackVideo.h>
#include <matroska/KaxVersion.h>


#include <xercesc/dom/DOMDocument.hpp>

#include "MatroskaFileParser.hh"
#include "MatroskaExtensions.hh"
#include "Std64IOCallback.hh"
#include "MatroskaUtils.hh"


#include "XMLUtils.hh"
#include "SystemUtils.hh"

using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;

XERCES_CPP_NAMESPACE_USE

namespace mediaindex {

// Debug can be turned on locally if needed by defining
// DEBUG_CLASS (un-comment below hashdef)
//#define DEBUG_CLASS

#ifdef DEBUG_CLASS
#define ASSERT(x) assert(x)
#define print_log_debug printf
#else
inline void dummylog(const char *format, ...) {};
#define ASSERT(x) do {} while (0)
#define print_log_debug dummylog
#endif

#define PRINTF_DEBUG print_log_debug

struct CBookMarkInfo {
    uint64_t m_ClusterPos;
    uint64_t m_Timecode;
};

struct CMatroskaFileParser::Impl {

    std::string m_strInFile;
    std::string m_errorString;
    Std64IOCallback *m_pClusterFile;
    EbmlStream* m_pEbmlStream;
    uint64_t m_nFileSize;
    uint64 m_nTimecodeScale;
    uint64_t m_nMediaCreateDate;
    uint64_t m_nMediaDuration;
    std::string m_mediaTitle;
    std::string m_streamType;
    uint64 m_nRequestedTimeOffset;
    uint64 m_nCurrentTimeOffset;
    uint8_t m_nDataTrackNumber;
    uint8_t m_nBookmarksTrackNumber;
    uint8_t m_nThumbnailsTrackNumber;
    uint8_t m_nDesiredTrackNumber;
    uint64_t m_nInfoPosition;
    uint64_t m_nTrackPosition;
    uint64_t m_nTagPosition;
    uint64_t m_nCuePosition;
    uint64_t m_nSeekheadPosition;
    uint64_t m_nBlockNumber;
    uint64_t m_nClusterPosition;
    KaxCluster * m_pCurrentCluster;
    EbmlElement * m_pCurrentClusterElement;
    EbmlMaster * m_pCurrentClusterMaster;
    uint64_t m_nCurrentBlockPosition;
    uint8_t *m_pDataBuffer;
    std::list <CGap> m_lGaps;
    std::list<CMetadataHolder> m_metaDataList;
    std::list <CBookMarkInfo> m_lBMInfo;

    uint64_t m_nMediaBeginTimeStamp;
    uint64_t m_nMediaEndTimeStamp;

    int m_nMediaVersion;
    int m_nDataBufferLength;
    bool m_bFirstFrameFound;
    bool m_nFirstCluster;
    bool m_bError;
    bool m_bEndOfMedia;
    bool m_bMediaChangeSignalled;
    bool m_bHasThumbnails;
    bool m_bServeThumbnailsToo;

    ~Impl();
    void InitializeVariables();
    void NeedBookmarkFrames(bool bEnable);
    void NeedThumbnailFrames(bool bEnable);
    bool IsSnapshot();
    void SetErrorString(const char *a_string);
    void OpenFile();
    void ComputeFileSize();
    bool ParseEbmlHead();
    EbmlElement *GetKaxSegment();
    bool CheckElement(EbmlElement *pElement);
    CMetadataHolder CreatePair(nameval_pair nv, DOMDocument* pXmlDoc);
    void HandleIpvsTag(KaxTag& a_rTag);
    void ParseTags(EbmlElement *&pTagsElement);
    bool RetrieveSegmentInfo();
    bool ParseSegmentInfo(EbmlElement *&pInfoElement);
    bool RetrieveTracksInfo();
    bool ParseTracksInfo(EbmlElement *&pTrackElement);
    bool ParseTrackElement(EbmlElement *&pTrackElement);
    bool RetrieveMetaData();
    eLIBMC_ERROR Parse();
    bool ParseSeekHead(EbmlElement *&pSegmentElement);
    void ParseSeekEntries(EbmlElement *&pSeekHeadElement);
    void ParseSeekEntry(EbmlElement *&pSeekEntryElement);
    void SaveSeekPositions(uint64 aPosition, unsigned char *aIdBuff);
    bool FoundAllSeekPositions();
    bool InitializeMediaToTimeOffset(uint64_t a_nOffset);
    bool InitializeMediaForNextBookmark(uint64_t a_nOffset);
    uint64_t GetFirstClusterForBookmark(uint64_t nTimeOffset);
    uint64_t GetNextClusterForBookmark(uint64_t nTimeOffset);
    uint64_t GetClusterTimeCodeFromCuePoint(EbmlElement *&pCuePointElement);
    void GetClusterInfoFromCuePoint(EbmlElement *&pCuePointElement,
            uint64_t & clusterTimecode, uint64_t & clusterPosition, int & trackNumber);
    void GetClusterInfoFromCueTrackPosition(EbmlElement *&pCuePointSubElement,
            uint64_t & clusterPosition, int & trackNumber);
    EbmlMaster * ReadMaster(EbmlElement *rootElement);
    void InitializeCluster();
    bool InitializeClusterFileToPosition(uint64 aPosition);
    void HandleFrame(DataBuffer &a_DataBuffer, uint64_t a_nTimeOffset,CCFrame &a_frame);
    bool SendMediaStartIfNeeded(CCFrame& cframe);
    void HandleBookmarkFrame(DataBuffer &a_DataBuffer, uint64_t a_nTimeOffset,CCFrame &a_frame);
    void SetDataInFrame(CCFrame& cframe, uint8_t * pData);
    int GetFrameType(uint8_t *a_pData);
    void ProcessTrackPrivateData(uint8_t *pData, int len);
    bool GetFrame(CCFrame &cframe);
    void FindFirstFrame();
    void InitializeClusterTimecode();
    void SkipBlocksTillFirstFrameLocated();
    bool GetNextFrame(CCFrame& cframe);
    bool ShouldSkipBlock(uint16_t blockTrackNumber);
    void MoveToNextBlock(EbmlElement *&pSimpleBlockElement);
    void InitializeNextCluster(EbmlElement *&pSimpleBlockElement);
    uint64_t GetTimecodeAtOffset(uint64_t offset);
    uint64_t GetOffsetFromTimecode(uint64_t timecode);

};


static const unsigned char InfoId[] = {0x15, 0x49, 0xa9, 0x66};
static const unsigned char TrackId[] = {0x16, 0x54, 0xae, 0x6b};
static const unsigned char TagsId[] = {0x12, 0x54, 0xc3, 0x67};
static const unsigned char CuesId[] = {0x1c, 0x53, 0xbb, 0x6b};

CMatroskaFileParser::CMatroskaFileParser(const string& a_rstrInFile)
{
    InitXmlLib();

    m_pimpl = new CMatroskaFileParser::Impl();
    m_pimpl->InitializeVariables();

    m_pimpl->m_strInFile = a_rstrInFile;
}

CMatroskaFileParser::~CMatroskaFileParser(void)
{
    delete m_pimpl;
    FreeXmlLib();
}

eLIBMC_ERROR  CMatroskaFileParser::Parse()
{
    return m_pimpl->Parse();
}

void CMatroskaFileParser::NeedBookmarkFrames(bool bEnable)
{
    m_pimpl->NeedBookmarkFrames(bEnable);
}

void CMatroskaFileParser::ServeThumbnails(bool bEnable)
{
    m_pimpl->NeedThumbnailFrames(bEnable);
}

bool CMatroskaFileParser::HasThumbnails()
{
    return m_pimpl->m_bHasThumbnails;
}

bool CMatroskaFileParser::IsSnapshot()
{
    return m_pimpl->IsSnapshot();
}

uint64_t CMatroskaFileParser::GetMediaCreateDate()
{
    return m_pimpl->m_nMediaCreateDate;
}

uint64_t CMatroskaFileParser::GetMediaDuration()
{
    return m_pimpl->m_nMediaDuration;
}

std::string CMatroskaFileParser::GetMediaTitle()
{
    return m_pimpl->m_mediaTitle;
}

std::string CMatroskaFileParser::GetStreamType()
{
    return m_pimpl->m_streamType;
}

uint64_t CMatroskaFileParser::GetMediaBeginTimestamp()
{
    return m_pimpl->m_nMediaBeginTimeStamp;
}
uint64_t CMatroskaFileParser::GetMediaEndTimestamp()
{
    return m_pimpl->m_nMediaEndTimeStamp;
}

uint64_t CMatroskaFileParser::GetTimecodeAtOffset(uint64_t offset)
{
    return m_pimpl->GetTimecodeAtOffset(offset);
}

uint64_t CMatroskaFileParser::GetOffsetFromTimecode(uint64_t timecode)
{
    return m_pimpl->GetOffsetFromTimecode(timecode);
}

uint64_t CMatroskaFileParser::Impl::GetTimecodeAtOffset(uint64_t offset)
{
    uint64_t startTimeCode = m_nMediaBeginTimeStamp;
    std::list<CGap> :: iterator iter;
    for (iter = m_lGaps.begin(); iter != m_lGaps.end(); ++iter) {
        CGap gap = (*iter);
        if (gap.GetBeginTime() > startTimeCode + offset) {
            break;
        }
        offset -= gap.GetBeginTime() - startTimeCode;
        startTimeCode = gap.GetEndTime();
    }

    uint64_t timecode =  startTimeCode + offset;
    if (timecode > m_nMediaEndTimeStamp)
        timecode = m_nMediaEndTimeStamp;
    return timecode;
}

uint64_t CMatroskaFileParser::Impl::GetOffsetFromTimecode(uint64_t timecode)
{
    uint64_t startTimeCode = m_nMediaBeginTimeStamp;
    uint64_t offset = 0;

    if (timecode <= startTimeCode)
        return 0;

    if (timecode > m_nMediaEndTimeStamp)
        timecode = m_nMediaEndTimeStamp;

    std::list<CGap> :: iterator iter;
    for (iter = m_lGaps.begin(); iter != m_lGaps.end(); ++iter) {
        CGap gap = (*iter);
        if (gap.GetBeginTime() > timecode) {
            break;
        }
        offset += gap.GetBeginTime() - startTimeCode;
        startTimeCode = gap.GetEndTime();
    }
    uint64_t finalOffset = 0;
    if (timecode < startTimeCode) // request time code within gap
        finalOffset = offset;
    else
        finalOffset =  (timecode - startTimeCode) + offset;
    return finalOffset;
}

std::list <CGap> CMatroskaFileParser::GetGaps() {
    return m_pimpl->m_lGaps;
}

CMatroskaFileParser::Impl::~Impl()
{
    delete m_pEbmlStream;
    delete m_pClusterFile;
    std::list<CMetadataHolder>::iterator iter;
    for (iter = m_metaDataList.begin(); iter != m_metaDataList.end(); ++iter) {
        iter->m_pXmlDoc->release();
    }
    delete m_pCurrentClusterElement;
    delete [] m_pDataBuffer;
}

void CMatroskaFileParser::Impl::InitializeVariables() {
    m_pClusterFile = NULL;
    m_pEbmlStream = NULL;
    m_nTimecodeScale = G_IPVS_TIMECODESCALE;
    m_nMediaCreateDate = 0;
    m_nMediaDuration = 0;
    m_nDataTrackNumber = 1;
    m_nBookmarksTrackNumber = 2;
    m_nThumbnailsTrackNumber = 3;
    m_nDesiredTrackNumber = m_nDataTrackNumber;
    m_nSeekheadPosition = 0;
    m_nInfoPosition = 0;
    m_nTrackPosition = 0;
    m_nTagPosition = 0;
    m_nCuePosition = 0;
    m_nFileSize = 0;
    m_nBlockNumber = 0;
    m_bFirstFrameFound = false;
    m_nFirstCluster = true;
    m_nRequestedTimeOffset = 0;
    m_nCurrentTimeOffset = 0;
    m_pCurrentCluster = NULL;
    m_pCurrentClusterElement = NULL;
    m_pCurrentClusterMaster = NULL;
    m_nCurrentBlockPosition = 0;
    m_nClusterPosition = 0;
    m_bError = true;
    m_bEndOfMedia = false;
    m_pDataBuffer = NULL;
    m_nDataBufferLength = 0;
    m_nMediaVersion = 0;
    m_streamType = "";
    m_bMediaChangeSignalled = false;
    m_bHasThumbnails = false;
    m_bServeThumbnailsToo = false;
}

eLIBMC_ERROR  CMatroskaFileParser::Impl::Parse()
{

    EbmlElement *pSegmentElement = NULL;

    m_bError = false;
    eLIBMC_ERROR rc = ELIBMC_OK;
    try {

        OpenFile();

        ComputeFileSize();

        InitializeClusterFileToPosition(0);

        if (!ParseEbmlHead()) {
            throw "No EBML head found";
        }

        if ((pSegmentElement = GetKaxSegment()) == NULL) {
            throw "No segment/level 0 element found";
        }

        if (!ParseSeekHead(pSegmentElement)) {
            throw "No seekhead element found";
        }

        if (!RetrieveSegmentInfo()) {
            throw "No segment info element found";
        }

        if (!RetrieveTracksInfo()) {
            throw "No tracks element found";
        }

        if (!RetrieveMetaData()) {
            throw "No tags element found";
        }

        if (!InitializeMediaToTimeOffset(0)) {
            throw "No ClusterPosition found";
        }

    }
    catch (exception & Ex)
    {
        SetErrorString(Ex.what());
        m_bError = true;
        rc = ELIBMC_ERROR;
    }
    catch (const char* pcException)
    {
        SetErrorString(pcException);
        m_bError = true;
        rc = ELIBMC_ERROR;
    }

    delete pSegmentElement;

    return rc;
}

void CMatroskaFileParser::Impl::NeedBookmarkFrames(bool bEnable)
{
    if (bEnable)
        m_nDesiredTrackNumber = m_nBookmarksTrackNumber;
    else
        m_nDesiredTrackNumber = m_nDataTrackNumber;
}

void CMatroskaFileParser::Impl::NeedThumbnailFrames(bool bEnable)
{
    m_bServeThumbnailsToo = bEnable;
}

bool CMatroskaFileParser::Impl::IsSnapshot()
{
    return (m_streamType == "SNAPSHOT");
}

void CMatroskaFileParser::Impl::OpenFile()
{
    m_pClusterFile = new Std64IOCallback(m_strInFile.c_str(), MODE_READ);
}

void CMatroskaFileParser::Impl::ComputeFileSize()
{
    m_pClusterFile->setFilePointer(0, seek_end);
    m_nFileSize = m_pClusterFile->getFilePointer();
    m_pClusterFile->setFilePointer(0, seek_beginning);
}

bool CMatroskaFileParser::Impl::ParseEbmlHead()
{
    EbmlElement *pEl0 = m_pEbmlStream->FindNextID(EbmlHead::ClassInfos, 0xFFFFFFFFL);
    if (NULL == pEl0)
        return false;
    delete pEl0;
    return true;
}


EbmlElement *CMatroskaFileParser::Impl::GetKaxSegment()
{
    EbmlElement *pEl0 = NULL;

    while (true) {
        pEl0 = m_pEbmlStream->FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFFFFFFFFFLL);
        if (pEl0 == NULL)
            break;
        if (ISKAXSEGMENTID(pEl0)) {
            return pEl0;
        }
        pEl0->SkipData(*m_pEbmlStream, pEl0->Generic().Context);
        delete pEl0;
    }
    return NULL;

}

bool CMatroskaFileParser::Impl::ParseSeekHead(EbmlElement *&pSegmentElement)
{
    int iUpperLevel = 0;
    EbmlElement * pSeekHeadElement = m_pEbmlStream->FindNextElement(pSegmentElement->Generic().Context,
            iUpperLevel, 0xFFFFFFFFL, true);

    if (!CheckElement(pSeekHeadElement)) {
        delete pSeekHeadElement;
        return false;
    }

    if (!ISKAXSEEKHEADID(pSeekHeadElement)) {
        delete pSeekHeadElement;
        return false;
    }

    ParseSeekEntries(pSeekHeadElement);

    delete pSeekHeadElement;

    if (!FoundAllSeekPositions())
        return false;

    return true;
}


// Seek gives the ID and Position of each Kax Element.
void CMatroskaFileParser::Impl::ParseSeekEntries(EbmlElement *&pSeekHeadElement)
{
    m_nSeekheadPosition = pSeekHeadElement->GetElementPosition();
    EbmlMaster *pSeekHeadMaster = ReadMaster(pSeekHeadElement);

    for (size_t i1 = 0; i1 < pSeekHeadMaster->ListSize(); ++i1) {
        EbmlElement *pSeekEntryElement = (*pSeekHeadMaster)[i1];
        ParseSeekEntry(pSeekEntryElement);
    }
}

void CMatroskaFileParser::Impl::ParseSeekEntry(EbmlElement *&pSeekEntryElement)
{
    EbmlMaster *pSeekEntryMaster = ReadMaster(pSeekEntryElement);

    unsigned char idbuff[4];
    memset(idbuff, 0x00, 4);
    uint64 position = 0;

    for (size_t i = 0; i < pSeekEntryMaster->ListSize(); ++i) {
        EbmlElement *pSeekEntrySubElement = NULL;
        pSeekEntrySubElement = (*pSeekEntryMaster)[i];
        if (ISKAXSEEKPOSITION(pSeekEntrySubElement)) {
            KaxSeekPosition &spos = *static_cast<KaxSeekPosition *> (pSeekEntrySubElement);
            position = uint64(spos);
        }
        else if (ISKAXSEEKID(pSeekEntrySubElement)) {
            KaxSeekID &seekid = *static_cast<KaxSeekID *> (pSeekEntrySubElement);
            unsigned char *buf = seekid.GetBuffer();
            int idsize = seekid.GetSize();
            if (idsize <= 4)
                memcpy(idbuff, buf, idsize);
        }
    }

    SaveSeekPositions(position, idbuff);
}

void CMatroskaFileParser::Impl::SaveSeekPositions(uint64 aPosition, unsigned char *aIdBuff)
{
    if (aPosition <= 0)
        return;

    if (memcmp(aIdBuff, InfoId, 4) == 0) {
        // Gives the position where KaxInfo begins which has the global info.
        m_nInfoPosition = aPosition + m_nSeekheadPosition;
    }
    if (memcmp(aIdBuff, TrackId, 4) == 0) {
        // Gives the position where KaxInfo begins which has the global info.
        m_nTrackPosition = aPosition + m_nSeekheadPosition;
    }
    else if (memcmp(aIdBuff, TagsId, 4) == 0) {
        // Gives the position where KaxTags begins which has the Tag details.
        m_nTagPosition = aPosition + m_nSeekheadPosition;
    }
    else if (memcmp(aIdBuff, CuesId, 4) == 0) {
        // Gives the Cue Position which has the positions of all the cluster beginnings.
        m_nCuePosition = aPosition + m_nSeekheadPosition;
    }
}

bool CMatroskaFileParser::Impl::FoundAllSeekPositions() {
    if (m_nSeekheadPosition == 0)
        return false;

    if (m_nInfoPosition == 0)
        return false;

    if (m_nCuePosition == 0)
        return false;

    return true;
}

bool CMatroskaFileParser::Impl::CheckElement(EbmlElement *pElement) {
    if (pElement == NULL)
        return false;
    if (pElement->GetElementPosition() + pElement->HeadSize() +
            pElement->GetSize() > m_nFileSize) {
        return false;
    }
    return true;
}


EbmlMaster * CMatroskaFileParser::Impl::ReadMaster(EbmlElement *rootElement)
{
    EbmlMaster *pEbmlMaster = static_cast<EbmlMaster *> (rootElement);
    int nUpperElementFound = 0;
    EbmlElement *pFoundElement;
    pEbmlMaster->Read(*m_pEbmlStream, rootElement->Generic().Context,
            nUpperElementFound, pFoundElement, true);
    return pEbmlMaster;
}

uint64_t CMatroskaFileParser::GetFileSize()
{
    return m_pimpl->m_nFileSize;
}

uint64_t CMatroskaFileParser::GetCurrentBlockPosition()
{
    return m_pimpl->m_nCurrentBlockPosition;
}

uint64_t CMatroskaFileParser::GetCurrentClusterPosition()
{
    if (m_pimpl->m_pCurrentClusterElement != NULL)
        return m_pimpl->m_pCurrentClusterElement->GetElementPosition();
    return 0;
}

uint64_t CMatroskaFileParser::GetCurrentTimeOffset()
{
    return m_pimpl->m_nCurrentTimeOffset;
}

bool CMatroskaFileParser::Impl::RetrieveSegmentInfo()
{
    if (!InitializeClusterFileToPosition(m_nInfoPosition)) {
        return false;
    }

    EbmlElement *pInfoElement = m_pEbmlStream->FindNextID(KaxInfo::ClassInfos, 0xFFFFFFFFL);
    if (!CheckElement(pInfoElement)) {
        delete pInfoElement;
        return false;
    }

    if (!ISKAXINFOID(pInfoElement)) {
        delete pInfoElement;
        return false;
    }

    if (!ParseSegmentInfo(pInfoElement)) {
        delete pInfoElement;
        return false;
    }

    delete pInfoElement;
    return true;
}

bool CMatroskaFileParser::Impl::ParseSegmentInfo(EbmlElement *&pInfoElement)
{
    EbmlMaster *pInfoMaster = ReadMaster(pInfoElement);

    int elementsDectected = 0;
    for (size_t index = 0; index < pInfoMaster->ListSize(); ++index) {

        EbmlElement * pInfoSubElement = (*pInfoMaster)[index];

        if (ISKAXTIMECODESCALEID(pInfoSubElement)) {
            KaxTimecodeScale &ktc_scale = *static_cast<KaxTimecodeScale *>(pInfoSubElement);
            uint64_t nTimecodeScale = uint64(ktc_scale);
            if (nTimecodeScale > 0)
                m_nTimecodeScale = nTimecodeScale;
            elementsDectected++;
        }
        else if (ISKAXDATEUTCID(pInfoSubElement)) {
            MyKaxDateUTC &dateutc = *static_cast<MyKaxDateUTC *>(pInfoSubElement);
            m_nMediaCreateDate = dateutc.GetEpochDateInMS();
            elementsDectected++;
        }
        else if (ISKAXTITLEID(pInfoSubElement)) {
            KaxTitle &title = *static_cast<KaxTitle *>(pInfoSubElement);
            m_mediaTitle = GetUTF8StrFromEbmlUnicodeStr(title);
            elementsDectected++;
        }
        else if (ISKAXDURATIONID(pInfoSubElement)) {
            KaxDuration &duration = *static_cast<KaxDuration *>(pInfoSubElement);
            m_nMediaDuration = uint64_t(double(duration));
            elementsDectected++;
        }
        else if (ISKAXWRITINGAPPID(pInfoSubElement)) {
            KaxWritingApp &writingApplication = *static_cast<KaxWritingApp *>(pInfoSubElement);
            std::string writingAppString = GetUTF8StrFromEbmlUnicodeStr(writingApplication);
            int major_version, minor_version;
            if (sscanf(writingAppString.c_str(), "libmediaconverter %3d.%3d",
                    &major_version, &minor_version) == 2) {
                elementsDectected++;
                m_nMediaVersion = ((major_version) << 8 | (minor_version));
            }
        }
    }
    // We need to detect all our key elements to consider this a valid IPVS media
    if (elementsDectected != 5) {
        return false;
    }
    return true;
}

bool CMatroskaFileParser::Impl::RetrieveTracksInfo()
{
    if (!InitializeClusterFileToPosition(m_nTrackPosition)) {
        return false;
    }

    EbmlElement *pTrackElement = m_pEbmlStream->FindNextID(KaxTracks::ClassInfos, 0xFFFFFFFFL);
    if (!CheckElement(pTrackElement)) {
        delete pTrackElement;
        return false;
    }

    if (!ISKAXTRACKSID(pTrackElement)) {
        delete pTrackElement;
        return false;
    }

    if (!ParseTracksInfo(pTrackElement)) {
        delete pTrackElement;
        return false;
    }

    delete pTrackElement;
    return true;
}

bool CMatroskaFileParser::Impl::ParseTracksInfo(EbmlElement *&pTracksElement)
{
    EbmlMaster *pTracksMaster = ReadMaster(pTracksElement);

    for (size_t index = 0; index < pTracksMaster->ListSize(); ++index) {
        EbmlElement * pTracksSubElement = (*pTracksMaster)[index];
        if (!ISKAXTRACKENTRYID(pTracksSubElement))
            continue;
        if (!ParseTrackElement(pTracksSubElement))
            return false;
    }
    m_nDesiredTrackNumber = m_nDataTrackNumber;
    return true;
}

bool CMatroskaFileParser::Impl::ParseTrackElement(EbmlElement *&pTrackElement)
{
    std::ostringstream summary;
    uint64 kax_track_number = -1;
    uint64 kax_track_type = -1;
    std::string kax_codec_id;
    KaxCodecPrivate * kax_codec_private = NULL;
    bool retVal = true;

    EbmlMaster *pTrackMaster = ReadMaster(pTrackElement);

    for (size_t index = 0; index < pTrackMaster->ListSize(); ++index) {
        EbmlElement *pTrackSubElement = (*pTrackMaster)[index];

        if (ISKAXTRACKNUMBERID(pTrackSubElement)) {
            KaxTrackNumber &tnum = *static_cast<KaxTrackNumber *>(pTrackSubElement);
            kax_track_number     = uint64(tnum);
            summary << "Track number is " << kax_track_number << "\n";
        } else if (ISKAXTRACKUIDID(pTrackSubElement)) {
            KaxTrackUID &tuid = *static_cast<KaxTrackUID *>(pTrackSubElement);
            summary << "Track UID is " << uint64(tuid) << "\n";
        } else if (ISKAXTRACKTYPEID(pTrackSubElement)) {
            KaxTrackType &ttype = *static_cast<KaxTrackType *>(pTrackSubElement);
            kax_track_type = uint64(ttype);
            summary << "Track type# is " << uint64(ttype) << "\n";
        } else if (ISKAXTRACKNAMEID(pTrackSubElement)) {
            KaxTrackName &name = *static_cast<KaxTrackName *>(pTrackSubElement);
            summary << "Track name is " << GetUTF8StrFromEbmlUnicodeStr(name)
                << "\n";
        } else if (ISKAXTRACKCODECPRIVATEID(pTrackSubElement)) {
            KaxCodecPrivate &c_priv = *static_cast<KaxCodecPrivate *>(pTrackSubElement);
            kax_codec_private = &c_priv;
        } else if (ISKAXTRACKCODECID(pTrackSubElement)) {
            KaxCodecID &codec_id = *static_cast<KaxCodecID *>(pTrackSubElement);
            kax_codec_id = std::string(codec_id);
            summary << "Track Codec ID is " << kax_codec_id << "\n";
        }
    }
    // Determine track number for known track types
    if (kax_codec_id.compare(0, 11, "D_IPVS/DATA") == 0) {
        m_nDataTrackNumber = kax_track_number;
        if (kax_track_type != IPVS_DATA_TRACK_TYPE) {
            retVal = false;
        }
        // Extract StreamType from codec id
        m_streamType = kax_codec_id.substr(12, kax_codec_id.length());

        // Extract private data
        if (kax_codec_private != NULL)
            ProcessTrackPrivateData(kax_codec_private->GetBuffer(), kax_codec_private->GetSize());
    }
    else if (kax_codec_id.compare(0, 11, "D_IPVS/BKMK") == 0) {
        m_nBookmarksTrackNumber = kax_track_number;
        if (kax_track_type != IPVS_BOOKMARKS_TRACK_TYPE) {
            retVal = false;
        }
    }
    else if (kax_codec_id.compare(0, 11, "D_IPVS/THMB") == 0) {
        m_nThumbnailsTrackNumber = kax_track_number;
        m_bHasThumbnails = true;
        if (kax_track_type != IPVS_THUMBNAILS_TRACK_TYPE) {
            retVal = false;
        }
    }

    //cout << summary.str() << endl;
    return retVal;
}

void CMatroskaFileParser::Impl::ProcessTrackPrivateData(uint8_t *pData, int len)
{
    PRINTF_DEBUG("Got private data of size %d\n", len);
    if (!(*(pData) == IPVS_PRIVATE_DATA_CURRENT_VERSION))
        return;

    if (len < IPVS_PRIVATE_DATA_VERSION_LENGTH + IPVS_TIMESTAMP_SIZE * 2)
        return;

    uint8_t *data = pData + IPVS_PRIVATE_DATA_VERSION_LENGTH;

    // Find media start and end timestamps
    m_nMediaBeginTimeStamp = *(uint64_t *)(data);
    data += IPVS_TIMESTAMP_SIZE;
    m_nMediaEndTimeStamp = *(uint64_t *)(data);
    data += IPVS_TIMESTAMP_SIZE;
    PRINTF_DEBUG("Media Begin/End: %llu-%llu\n", m_nMediaBeginTimeStamp, m_nMediaEndTimeStamp);

    // Find timestamps for all the gaps
    len -= IPVS_PRIVATE_DATA_VERSION_LENGTH + IPVS_TIMESTAMP_SIZE * 2;
    m_lGaps.clear();
    while (len > 0) {
        CGap gap(0,0);
        gap.SetBeginTime(*(uint64_t *)(data));
        data += IPVS_TIMESTAMP_SIZE;
        gap.SetEndTime(*(uint64_t *)(data));
        data += IPVS_TIMESTAMP_SIZE;
        len -= IPVS_TIMESTAMP_SIZE * 2;
        PRINTF_DEBUG("Gap: %llu-%llu\n", gap.GetBeginTime(), gap.GetEndTime());
        m_lGaps.push_back(gap);
    }
}


bool CMatroskaFileParser::Impl::RetrieveMetaData()
{
    if (!InitializeClusterFileToPosition(m_nTagPosition))
        return false;

    EbmlElement *pTagsElement = m_pEbmlStream->FindNextID(KaxTags::ClassInfos, 0xFFFFFFFFL);
    if (!CheckElement(pTagsElement)) {
        delete pTagsElement;
        return false;
    }
    if (ISKAXTAGSID(pTagsElement)) {
        ParseTags(pTagsElement);
    }

    delete pTagsElement;
    return true;
}


void CMatroskaFileParser::Impl::ParseTags(EbmlElement *&pTagsElement)
{
    EbmlMaster *pTagsMaster = ReadMaster(pTagsElement);

    for (size_t index = 0; index < pTagsMaster->ListSize(); ++index) {

        KaxTag* pKaxTag = dynamic_cast<KaxTag *>( (*pTagsMaster)[index] );

        if (pKaxTag == NULL) {
            PRINTF_DEBUG("Expected TAG element in TAGS, but found something else, skipping ...");
            continue;
        }

        if (CheckIfIPVSTagType(*pKaxTag) == false) {
            PRINTF_DEBUG("Non IPVS tag found, skipping ...");
            continue;
        }
        HandleIpvsTag(*pKaxTag);
    }

}

void   CMatroskaFileParser::Impl::HandleIpvsTag(KaxTag& a_rTag)
{
    // The IPVS Tag consists of multiple simple tags, the simple tag
    // at level 3 is as follows: name - xml resource type, value =
    // xml file name,
    // Within each simple tag at level 3 is the xml tree, i.e. the
    // root element of each xml begins in a simple tag at level 4.

    for (size_t i = 0; i < a_rTag.ListSize(); ++i)
    {
        KaxTagSimple* pSimpleTag = dynamic_cast<KaxTagSimple*>(a_rTag[i]);
        if (pSimpleTag == NULL)
            continue;

        // Parse the [name, value] of this tag to know the xml type and
        // file location.
        const nameval_pair nv = GetNameValFromSimpleTag(*pSimpleTag);
        KaxTagSimple* pSimpleTag2 = FindChild<KaxTagSimple>(*pSimpleTag);
        DOMDocument* pXmlDoc = EbmlToXml(pSimpleTag2);
        if (pXmlDoc != NULL) {
            m_metaDataList.push_back(CreatePair(nv, pXmlDoc));
        }
    }
}

CMetadataHolder  CMatroskaFileParser::Impl::CreatePair(nameval_pair nv, DOMDocument* pXmlDoc)
{
   CMetadataHolder a_Holder;
   a_Holder.m_name = nv.name;
   a_Holder.m_val = nv.val;
   a_Holder.m_pXmlDoc = pXmlDoc;
   return a_Holder;
}

std::list<CMetadataHolder> CMatroskaFileParser::GetMetadata(string a_filter)
{

    std::list<CMetadataHolder> a_L;
    CMetadataHolder a_Holder;
    std::list<CMetadataHolder>::iterator iter;
    for (iter = m_pimpl->m_metaDataList.begin(); iter != m_pimpl->m_metaDataList.end(); ++iter) {
        if (a_filter == "all" || (*iter).m_name == a_filter) {
            a_Holder.m_name = (*iter).m_name;
            a_Holder.m_val = (*iter).m_val;
            a_Holder.m_pXmlDoc = (*iter).m_pXmlDoc;
            a_L.push_back(a_Holder);
        }
    }
    return a_L;
}

bool CMatroskaFileParser::InitializeMediaToTimeOffset(uint64_t a_nOffset)
{
    return m_pimpl->InitializeMediaToTimeOffset(a_nOffset);
}

bool CMatroskaFileParser::Impl::InitializeMediaToTimeOffset(uint64_t a_nOffset)
{
    m_nClusterPosition = 0;
    m_nRequestedTimeOffset = a_nOffset;
    m_bFirstFrameFound = false;
    m_bMediaChangeSignalled = false;

    if (m_bError)
        return false;

    if (!InitializeClusterFileToPosition(m_nCuePosition))
        return false;

    EbmlElement * pCuesElement = m_pEbmlStream->FindNextID(KaxCues::ClassInfos, 0xFFFFFFFFL);
    if (!ISKAXCUESID(pCuesElement)) {
        delete pCuesElement;
        return false;
    }

    EbmlMaster *pCuesMaster = ReadMaster(pCuesElement);
    m_lBMInfo.clear();

    for (size_t index = 0; index < pCuesMaster->ListSize(); ++index) {
        EbmlElement * pCuePointElement = (*pCuesMaster)[index];

        uint64_t clusterTimecode = 0;
        uint64_t clusterPosition = 0;
        int trackNumber = 0;

        GetClusterInfoFromCuePoint(pCuePointElement, clusterTimecode, clusterPosition, trackNumber);

        // We cache all discovered bookmarks.
        if (m_nDesiredTrackNumber == m_nBookmarksTrackNumber) {
            if (trackNumber == m_nDesiredTrackNumber) {
                CBookMarkInfo bmInfo;
                bmInfo.m_ClusterPos = clusterPosition;
                bmInfo.m_Timecode = clusterTimecode;
                m_lBMInfo.push_back(bmInfo);
            }
            continue;
        }

        // Break out when the time offset gets beyond the desired value.
        if (clusterTimecode >= m_nRequestedTimeOffset &&
                trackNumber == m_nDesiredTrackNumber) {
            if (m_nClusterPosition == 0)
                m_nClusterPosition = clusterPosition;
            break;
        }

        // As long as we are below the target offset, keep saving the latest
        // cluster position
        m_nClusterPosition = clusterPosition;
    }

    delete pCuesElement;

    if (m_nDesiredTrackNumber == m_nBookmarksTrackNumber) {
        m_nClusterPosition = GetFirstClusterForBookmark(m_nRequestedTimeOffset);
    }

    if (m_nClusterPosition > 0)
        return true;
    return false;
}

bool CMatroskaFileParser::Impl::InitializeMediaForNextBookmark(uint64_t a_nOffset)
{
    m_nClusterPosition = 0;
    m_nRequestedTimeOffset = a_nOffset;
    m_bFirstFrameFound = false;
    m_bMediaChangeSignalled = false;

    if (m_bError)
        return false;

    m_nClusterPosition = GetNextClusterForBookmark(m_nRequestedTimeOffset);
    if (m_nClusterPosition > 0)
        return true;
    return false;
}

uint64_t CMatroskaFileParser::Impl::GetNextClusterForBookmark(uint64_t nTimeOffset) {
    uint64_t nClusterPosition = 0;
    std::list<CBookMarkInfo> :: iterator iter;
    for (iter = m_lBMInfo.begin(); iter != m_lBMInfo.end(); ++iter) {
        CBookMarkInfo bmInfo = (*iter);
        if (bmInfo.m_Timecode > nTimeOffset) {
            nClusterPosition = bmInfo.m_ClusterPos;
            break;
        }
    }
    return nClusterPosition;
}

uint64_t CMatroskaFileParser::Impl::GetFirstClusterForBookmark(uint64_t nTimeOffset) {
    uint64_t nClusterPosition = 0;
    std::list<CBookMarkInfo> :: iterator iter;
    for (iter = m_lBMInfo.begin(); iter != m_lBMInfo.end(); ++iter) {
        CBookMarkInfo bmInfo = (*iter);
        if (bmInfo.m_Timecode >= nTimeOffset) {
            nClusterPosition = bmInfo.m_ClusterPos;
            break;
        }
    }
    return nClusterPosition;
}

void CMatroskaFileParser::Impl::GetClusterInfoFromCuePoint(
        EbmlElement *&pCuePointElement,
        uint64_t & clusterTimecode, uint64_t & clusterPosition, int & trackNumber)

{
    EbmlElement *pCuePointSubElement = NULL;
    EbmlMaster *pCuePointMaster = ReadMaster(pCuePointElement);
    clusterTimecode = 0;
    clusterPosition = 0;
    trackNumber = 0;
    for (size_t i = 0; i < pCuePointMaster->ListSize(); ++i) {
        pCuePointSubElement = (*pCuePointMaster)[i];
        if (ISKAXCUETIME(pCuePointSubElement)) {
            KaxCueTime &ctc = *static_cast<KaxCueTime *> (pCuePointSubElement);
            clusterTimecode = uint64(ctc);
        }
        GetClusterInfoFromCueTrackPosition(pCuePointSubElement, clusterPosition, trackNumber);
    }
}

void CMatroskaFileParser::Impl::GetClusterInfoFromCueTrackPosition(
        EbmlElement *&pCuePointSubElement,
        uint64_t & clusterPosition, int & trackNumber)
{
    clusterPosition = 0;
    trackNumber = 0;
    if (!ISKAXCUETRACKPOSITION(pCuePointSubElement))
        return;

    EbmlMaster *pCueTrackPositionMaster = ReadMaster(pCuePointSubElement);
    for (size_t i = 0; i < pCueTrackPositionMaster->ListSize(); ++i) {
        EbmlElement *pCueTrackPositionSubElement = (*pCueTrackPositionMaster)[i];
        if (ISKAXCUECLUSTERPOSITION(pCueTrackPositionSubElement)) {
            KaxCueClusterPosition &ccp =
                    *static_cast<KaxCueClusterPosition *> (pCueTrackPositionSubElement);
            clusterPosition = uint64(ccp) + m_nSeekheadPosition;
        }
        else if (ISKAXCUETRACK(pCueTrackPositionSubElement)) {
            KaxCueTrack &track =
                    *static_cast<KaxCueTrack *> (pCueTrackPositionSubElement);
            trackNumber = uint64(track);
        }
    }
}

bool CMatroskaFileParser::Impl::InitializeClusterFileToPosition(uint64 aPosition)
{
    if (aPosition > m_nFileSize) {
        return false;
    }
    m_pClusterFile->setFilePointer(aPosition, seek_beginning);

    // Create a new stream
    delete m_pEbmlStream;
    m_pEbmlStream = new EbmlStream(*(m_pClusterFile));

    return true;
}

bool CMatroskaFileParser::GetFrame(CCFrame &a_frame) {
    return m_pimpl->GetFrame(a_frame);
}

bool CMatroskaFileParser::Impl::GetFrame(CCFrame &cframe)
{
    if (m_bError)
        return false;

    if (m_nClusterPosition == 0)
        return false;

    if (!m_bFirstFrameFound) {
        FindFirstFrame();
    }

    if (m_bEndOfMedia)
        return false;

    return GetNextFrame(cframe);
}

void CMatroskaFileParser::Impl::FindFirstFrame()
{
    InitializeClusterFileToPosition(m_nClusterPosition);

    m_bEndOfMedia = false;

    delete m_pCurrentClusterElement;
    m_pCurrentClusterElement = m_pEbmlStream->FindNextID(KaxCluster::ClassInfos,
            0xFFFFFFFFL);

    InitializeCluster();

    SkipBlocksTillFirstFrameLocated();

}

void CMatroskaFileParser::Impl::InitializeCluster()
{
    m_nBlockNumber = 0;
    m_pCurrentCluster = reinterpret_cast<KaxCluster *> (m_pCurrentClusterElement);
    m_pCurrentClusterMaster = ReadMaster(m_pCurrentClusterElement);
    InitializeClusterTimecode();
}

void CMatroskaFileParser::Impl::InitializeClusterTimecode()
{
    EbmlElement *pClusterTimecodeElement = (*m_pCurrentClusterMaster)[0];
    if (!ISKAXCLUSTERTIMECODEID(pClusterTimecodeElement))
        return;

    KaxClusterTimecode &ctc =
            *static_cast<KaxClusterTimecode *> (pClusterTimecodeElement);
    uint64_t nClusterTimecode = uint64(ctc);
    m_pCurrentCluster->InitTimecode(nClusterTimecode, m_nTimecodeScale);
    m_nCurrentTimeOffset = nClusterTimecode;
    m_nBlockNumber++; // becomes 1
}

void CMatroskaFileParser::Impl::SkipBlocksTillFirstFrameLocated()
{
    EbmlElement * pSimpleBlockElement = NULL;
    for (size_t blockNumber = 1; blockNumber < m_pCurrentClusterMaster->ListSize(); ++blockNumber) {
        pSimpleBlockElement = (*m_pCurrentClusterMaster)[blockNumber];
        if (!ISKAXSIMPLEBLOCKID(pSimpleBlockElement))
            continue;
        KaxSimpleBlock &block = *static_cast<KaxSimpleBlock *> (pSimpleBlockElement);
        block.SetParent(*m_pCurrentCluster);
        uint64_t nStartTime = block.GlobalTimecode() / m_nTimecodeScale;
        if (nStartTime >= m_nRequestedTimeOffset) {
            m_nBlockNumber = blockNumber;
            m_bFirstFrameFound = true;
            m_nCurrentBlockPosition = pSimpleBlockElement->GetElementPosition();
            break;
        }
    }
    if (!m_bFirstFrameFound && pSimpleBlockElement != NULL) {
        InitializeNextCluster(pSimpleBlockElement);
        m_bFirstFrameFound = true;
    }
}

bool CMatroskaFileParser::Impl::GetNextFrame(CCFrame& cframe)
{
    while (true) {
        if (m_bEndOfMedia)
            return false;

        EbmlElement *pSimpleBlockElement = (*m_pCurrentClusterMaster)[m_nBlockNumber];
        if (ISKAXSIMPLEBLOCKID(pSimpleBlockElement)) {
            KaxSimpleBlock &block = *static_cast<KaxSimpleBlock *> (pSimpleBlockElement);
            block.SetParent(*m_pCurrentCluster);
            m_nCurrentBlockPosition = pSimpleBlockElement->GetElementPosition();

            if (ShouldSkipBlock(block.TrackNum())) {
                MoveToNextBlock(pSimpleBlockElement);
                continue;
            }

            for (unsigned int i = 0; i < block.NumberFrames(); ++i) {
                HandleFrame(block.GetBuffer(i),
                        block.GlobalTimecode() / m_nTimecodeScale,
                        cframe);
            }
        }

        if (!m_bMediaChangeSignalled && m_nDesiredTrackNumber == m_nDataTrackNumber) {
            m_bMediaChangeSignalled = true;
            break;
        }

        MoveToNextBlock(pSimpleBlockElement);

        break;
    }
    return true;
}

bool CMatroskaFileParser::Impl::ShouldSkipBlock(uint16 blockTrackNumber)
{
    // Serve only bookmarks or only data or only data and thumbnails
    if (m_nDesiredTrackNumber == m_nBookmarksTrackNumber &&
            blockTrackNumber != m_nBookmarksTrackNumber)
        return true;

    if (m_nDesiredTrackNumber == m_nDataTrackNumber &&
            blockTrackNumber == m_nBookmarksTrackNumber)
        return true;

    if (!m_bServeThumbnailsToo &&
            blockTrackNumber == m_nThumbnailsTrackNumber)
        return true;

    return false;
}

void CMatroskaFileParser::Impl::MoveToNextBlock(EbmlElement *&pSimpleBlockElement)
{
    m_nBlockNumber++;
    if (m_nBlockNumber == m_pCurrentClusterMaster->ListSize()) {
        InitializeNextCluster(pSimpleBlockElement);
    }
}

void CMatroskaFileParser::Impl::HandleFrame(DataBuffer &a_DataBuffer,
        uint64_t a_nTimeOffset, CCFrame& cframe)
{
    m_nCurrentTimeOffset = a_nTimeOffset;

    if (m_nDesiredTrackNumber == m_nBookmarksTrackNumber) {
        return HandleBookmarkFrame(a_DataBuffer, a_nTimeOffset, cframe);
    }
    // Deal with incoming frame depending on its type
    uint8_t *data = a_DataBuffer.Buffer();
    switch (GetFrameType(data)) {
    case IPVS_FRAME_TYPE_MEDIA_DATA: {
        if (SendMediaStartIfNeeded(cframe))
            break;
        uint64_t nFrameTimestamp = *(uint64_t *) (data + IPVS_FRAME_HEADER_SIZE);
        cframe.SetMessageType(CCFrame::Normal);
        cframe.SetTimeStamp(nFrameTimestamp);
        cframe.SetLength(a_DataBuffer.Size() - (IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE));
        cframe.SetTimeOffset(a_nTimeOffset);
        cframe.SetMediaFlag(*(uint32_t *) (data));
        SetDataInFrame(cframe, data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE);
        break;
    }
    case IPVS_FRAME_TYPE_MEDIA_START: {
        uint64_t nFrameTimestamp = *(uint64_t *) (data + IPVS_FRAME_HEADER_SIZE);
        cframe.SetMessageType(CCFrame::MediaStarted);
        cframe.SetTimeStamp(nFrameTimestamp);
        cframe.SetLength(0);
        cframe.SetTimeOffset(a_nTimeOffset);
        cframe.SetMediaFlag(0);
        cframe.SetData(NULL);
        m_bMediaChangeSignalled = true;
        break;
    }
    case IPVS_FRAME_TYPE_MEDIA_STOP: {
        uint64_t nFrameTimestamp = *(uint64_t *) (data + IPVS_FRAME_HEADER_SIZE);
        cframe.SetMessageType(CCFrame::MediaStopped);
        cframe.SetTimeStamp(nFrameTimestamp);
        cframe.SetTimeOffset(a_nTimeOffset);
        cframe.SetLength(0);
        cframe.SetMediaFlag(0);
        cframe.SetData(NULL);
        m_bMediaChangeSignalled = true;
        break;
    }
    case IPVS_FRAME_TYPE_MEDIA_SNAPSHOT: {
        m_bMediaChangeSignalled = true;
        uint64_t nFrameTimestamp = *(uint64_t *) (data + IPVS_FRAME_HEADER_SIZE);
        cframe.SetMessageType(CCFrame::Snapshot);
        cframe.SetTimeStamp(nFrameTimestamp);
        cframe.SetLength(a_DataBuffer.Size() -
                (IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE + IPVS_IMAGE_HEADER_SIZE));
        cframe.SetTimeOffset(a_nTimeOffset);
        cframe.SetMediaFlag(*(uint32_t *) (data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE));
        SetDataInFrame(cframe, data +
                IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE + IPVS_IMAGE_HEADER_SIZE);
        break;
    }
    case IPVS_FRAME_TYPE_MEDIA_THUMBNAIL: {
        if (SendMediaStartIfNeeded(cframe))
            break;
        uint64_t nFrameTimestamp = *(uint64_t *) (data + IPVS_FRAME_HEADER_SIZE);
        cframe.SetMessageType(CCFrame::Thumbnail);
        cframe.SetTimeStamp(nFrameTimestamp);
        cframe.SetLength(a_DataBuffer.Size() -
                (IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE + IPVS_IMAGE_HEADER_SIZE));
        cframe.SetTimeOffset(a_nTimeOffset);
        cframe.SetMediaFlag(*(uint32_t *) (data + IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE));
        SetDataInFrame(cframe, data +
                IPVS_FRAME_HEADER_SIZE + IPVS_TIMESTAMP_SIZE + IPVS_IMAGE_HEADER_SIZE);
        break;
    }
    }

}

bool CMatroskaFileParser::Impl::SendMediaStartIfNeeded(CCFrame& cframe)
{
    if (!m_bMediaChangeSignalled) {
        // "MediaStarted" needs to go out as the first frame
        cframe.SetMessageType(CCFrame::MediaStarted);
        cframe.SetTimeStamp(GetTimecodeAtOffset(m_nRequestedTimeOffset));
        cframe.SetLength(0);
        cframe.SetTimeOffset(m_nRequestedTimeOffset);
        cframe.SetMediaFlag(0);
        cframe.SetData(NULL);
        return true;
    }
    return false;
}

void CMatroskaFileParser::Impl::HandleBookmarkFrame(DataBuffer &a_DataBuffer,
        uint64_t a_nTimeOffset, CCFrame& cframe)
{
    uint8_t *data = a_DataBuffer.Buffer();
    cframe.SetMessageType(CCFrame::Normal);
    cframe.SetTimeStamp(0);
    cframe.SetLength(a_DataBuffer.Size());
    cframe.SetTimeOffset(a_nTimeOffset);
    cframe.SetMediaFlag(0);
    SetDataInFrame(cframe, data);
}

void CMatroskaFileParser::Impl::SetDataInFrame(CCFrame& cframe, uint8_t * pData)
{
    if (m_nDataBufferLength < (int)cframe.GetLength()) {
        delete [] m_pDataBuffer;
        m_pDataBuffer = new uint8_t[cframe.GetLength() + 1];
        m_nDataBufferLength = cframe.GetLength();
    }
    memcpy(m_pDataBuffer, pData, cframe.GetLength());
    cframe.SetData(m_pDataBuffer);
}

int CMatroskaFileParser::Impl::GetFrameType(uint8_t *a_pData) {

    if (*(a_pData + 1) == 'I' &&
            strncmp((char *)(a_pData + 1), "IPV", 3) == 0) {
          return *(uint8_t *)a_pData;
    }
    return IPVS_FRAME_TYPE_MEDIA_DATA;
}

void CMatroskaFileParser::Impl::InitializeNextCluster(EbmlElement *&pSimpleBlockElement)
{
    m_nBlockNumber = 0;
    int iUpperLevel = 0;
    EbmlElement *pNextClusterElement = m_pEbmlStream->FindNextElement(pSimpleBlockElement->Generic().Context,
            iUpperLevel, 0xFFFFFFFFL, true);

    while ((NULL != pNextClusterElement)) {

        if (ISKAXCLUSTERID(pNextClusterElement)) {
            delete m_pCurrentClusterElement;
            m_pCurrentClusterElement = pNextClusterElement;
            InitializeCluster();

            if (m_nDesiredTrackNumber == m_nBookmarksTrackNumber) {
                // For bookmarks, instead of using the very next cluster, we
                // restart the media (using cues) to find the next bookmark frame.
                // This avoids having to read all the clusters.
                if (!InitializeMediaForNextBookmark(m_nCurrentTimeOffset)) {
                    m_bEndOfMedia = true;
                    break;
                }
                FindFirstFrame();
            }
            break;
        }
        delete pNextClusterElement;
        m_bEndOfMedia = true;
        break;
    }

}

void CMatroskaFileParser::Impl::SetErrorString(const char *a_string)
{
    m_errorString.assign(a_string);
}

std::string CMatroskaFileParser::GetErrorString()
{
    return m_pimpl->m_errorString;
}


} // end namespace
