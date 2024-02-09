//Defines the class CMatroskaFileWriterImpl, BookmarkData & declares helper func inside.
#ifndef MATROSKA_FILE_WRITERIMPL_H
#define MATROSKA_FILE_WRITERIMPL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <iostream>
#include <list>

#include <ebml/EbmlHead.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlVersion.h>
#include <ebml/EbmlVoid.h>

#include <matroska/FileKax.h>
#include <matroska/KaxAttached.h>
#include <matroska/KaxAttachments.h>
#include <matroska/KaxBlock.h>
#include <matroska/KaxChapters.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxClusterData.h>
#include <matroska/KaxCues.h>
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


using std::string;
using std::wstring;
using std::vector;
using std::cerr;
using std::endl;
using std::list;

#include "libmediaconverter.hh"
#include "MatroskaUtils.hh"

#include "CommonIncludes.hh"
#include "MediaServer.hh"

#include "FileSystemUtils.hh"
#include "XMLUtils.hh"
#include "Std64IOCallback.hh"

using namespace LIBEBML_NAMESPACE;
using namespace mediaindex;


namespace mediaindex {

class CEditList;

class BookmarksData
{
public:
    BookmarksData(uint64_t offset, string filename) :
        m_nTimeOffset(offset), m_strFileName(filename) {};

    bool operator < (const BookmarksData & rhs)
    {
        return m_nTimeOffset < rhs.m_nTimeOffset;
    }

    uint64_t m_nTimeOffset;
    string m_strFileName;

};

class CMatroskaFileWriterImpl
{
public:
    CMatroskaFileWriterImpl(
        const string& a_rstrOutFile,
        const string& a_rstrDbDir,
        const string& a_rstrBlobDir);
    ~CMatroskaFileWriterImpl();

    eLIBMC_ERROR Write();
    void Silent();
    void FixedUUIDs();

    void SetEditList(CEditList * editList);

private:
    CMatroskaFileWriterImpl(const CMatroskaFileWriterImpl &a_rWriter);
    CMatroskaFileWriterImpl &operator=(const CMatroskaFileWriterImpl &a_rWriter);

    eLIBMC_ERROR WriteHeader();
    void FinishFile();
    void CreateOutputFile();

    void CreateMatroskaObjects();
    void DestroyMatroskaObjects();

    void WriteTrackHeaders();
    void WriteMediaData();
    void WriteRealMediaData();
    void CreateStreamMetaData();

    void WriteAllMediaFrames();
    void WriteEditedMediaFrames();

    void HandleMediaFrame(CCFrame & aFrame);
    void HandleNormalFrame(CCFrame & aFrame, uint64_t nTimeOffset);
    void HandleStartStopFrame(CCFrame & aFrame, uint64_t nTimeOffset);
    void HandleThumbnailFrame(CCFrame & aFrame, uint64_t nTimeOffset);
    void HandleSnapshotFrame(CCFrame & aFrame);
    bool GetFrame(CCFrame & aFrame);

    std::string GetStreamType();

    void InitializeMediaToTimeOffset(uint64_t timeOffset);
    void StartMedia(uint64_t nStartTime);
    void StopMedia(uint64_t nStopTime);
    void GenerateStopFrame(uint64_t nStopTime);

    void GetLastSCAP(uint64_t requestOffset);
    bool LookForScap(CCFrame & aFrame);
    void InsertSCAP(CCFrame & aFrame);

    uint64_t GetMediaBeginTimeStamp();
    uint64_t GetMediaEndTimeStamp();
    uint64_t GetMediaDuration();

    void GetNewBlockGroup();
    void GetNewCluster();
    void CloseCuster();
    void AddFrame(KaxTrackEntry * a_pTrackEntry, uint64_t a_nTimeOffset,
            uint8_t *a_pData, int a_nLength);
    bool InitMediaServer();
    void CreatePrivateData();
    void AddTimeStampsToAllGaps(uint8_t ** a_pData, uint64_t a_nStartTime, uint64_t a_nEndTime);
    void ProcessXmlFileElements(const XERCES_NS::DOMDocument* a_pDomDoc);
    void ProcessBookmarkFile(const XERCES_NS::DOMDocument* a_pDomDoc, string a_strFileName);
    bool AddBookmarkFrameFromFile(string filename, uint64_t a_nTimeOffset);
    void AddRemainingBookmarks(uint64_t a_nTimeOffset);
    bool AddBookmarkFrames(uint64_t a_nTimeOffset);
    bool NeedFixedUIDS();
    void GenerateTrackIds();

    uint64_t AdjustTimeOffset(uint64_t a_nTimeOffset);

    const string m_strOutFile;
    const string m_strDbDir;
    const string m_strBlobDir;

    Std64IOCallback* m_pClusterFile;
    KaxSegment*     m_pKaxSegment;
    KaxCues*        m_pKaxCues;
    KaxSeekHead*    m_pKaxSeekHeadMain;
    EbmlVoid*       m_pEbmlVoid;
    KaxSeekHead*    m_pKaxSeekHeadCues;
    KaxTracks       m_Tracks;
    KaxTrackEntry*  m_pTrackEntry;
    KaxTrackEntry*  m_pBookmarksTrackEntry;
    KaxTrackEntry*  m_pThumbnailsTrackEntry;
    KaxCluster*     m_pKaxCluster;
    KaxTags*        m_pKaxTags;
    KaxBlockGroup*  m_pCurrentBlockGroup;
    KaxBlockBlob*   m_pCurrentBlockBlob;
    std::vector<KaxBlockBlob *> m_lBlockBlobList;
    std::list<BookmarksData> m_lBookmarksList;

    string          m_strTitle;
    string          m_strGroupTitle;
    string          m_strStreamType;

    uint8_t         m_nDataTrackNumber;
    uint8_t         m_nBookmarksTrackNumber;
    uint8_t         m_nThumbnailsTrackNumber;
    uint64_t        m_nDataTrackUID;
    uint64_t        m_nBookmarkTrackUID;
    uint64_t        m_nThumbnailsTrackUID;
    bool            m_bCueBlock;
    bool            m_bSilent;
    bool            m_bFixedUUIDs;

    CMediaServer*   m_pMediaServer;
    uint64_t        m_nMediaBeginTimeStamp;
    uint64_t        m_nDuration;
    uint64_t        m_nTimeOffset;
    uint64_t        m_nClusterTimeOffset;
    uint64_t        m_nPreviousClusterTimecode;
    uint64_t        m_nFrameCount;
    uint64_t        m_nCueingTimeOffset;
    uint64_t        m_nNextBookmarkOffset;
    uint64_t        m_nBytesWritten;
    uint32_t        m_nFramesInBlock;
    uint32_t        m_nFramesInCluster;
    uint32_t        m_nBytesInCluster;
    uint32_t        m_nClusterCount;
    uint32_t        m_nBytesSinceCued;

    uint64_t        m_nMediaStartTime;
    uint64_t        m_nMediaStopTime;
    uint64_t        m_nCurrentSkipLength;
    uint64_t        m_nLastSkipOffset;

    CEditList*      m_pEditList;

    bool            m_bInsertScap;
    uint8_t*        m_scapBuffer;
    int             m_scapBufferLen;
    bool            m_bMediaStarted;
    bool            m_bHasThumbnails;

    list <CGap>     m_lGaps;
    uint8_t         *m_pPrivateData;
    uint32_t        m_nPrivateDataLen;

};

inline bool XmlFileContainsMetaData(const XERCES_NS::DOMDocument* a_pDomDoc)
{
    if(!a_pDomDoc || !a_pDomDoc->getDocumentElement())
        return false;

    XERCES_NS::DOMElement* pRootElem = a_pDomDoc->getDocumentElement();
    StrX strElName(pRootElem->getTagName());

    return ( XERCES_NS::XMLString::equals(strElName.localForm(), INDEX_ELEMENT) ||
            XERCES_NS::XMLString::equals(strElName.localForm(), MSFRL_ELEMENT) ||
                    XERCES_NS::XMLString::equals(strElName.localForm(), MSFILE_ELEMENT) );

}

inline bool XmlFileContainsBookmark(const XERCES_NS::DOMDocument* a_pDomDoc)
{
    if(!a_pDomDoc || !a_pDomDoc->getDocumentElement())
        return false;

    XERCES_NS::DOMElement* pRootElem = a_pDomDoc->getDocumentElement();
    StrX strElName(pRootElem->getTagName());

    return XERCES_NS::XMLString::equals(strElName.localForm(), BOOKMARK_ELEMENT);

}

KaxTagSimple& CreateSimpleTagForXml(
        KaxTag& a_rKaxTag,
        const XERCES_NS::DOMDocument* a_pDomDoc,
        const string& a_rstrXmlFile);


};
#endif //MATROSKA_FILE_WRITERIMPL_H
