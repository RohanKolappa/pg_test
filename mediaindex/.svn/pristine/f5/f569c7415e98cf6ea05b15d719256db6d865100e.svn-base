/*
 * MatroskaFileInfo.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: rkale
 */

#include "libmediaconverter.hh"
#include "MatroskaFileInfo.hh"
#include "MatroskaFileParser.hh"
#include "MediaServer.hh"
#include "XMLUtils.hh"
#include "EditList.hh"
#include "FileSystemUtils.hh"

XERCES_CPP_NAMESPACE_USE

namespace mediaindex {

using namespace std;

CMatroskaFileInfo::CMatroskaFileInfo(const std::string& a_rstrInFile,
        bool bInternalStore)
{
    InitXmlLib();
    m_pMinuteFileServer = NULL;
    m_pMatroskaParser = NULL;
    if (IsDirectory(a_rstrInFile)) {
        m_pMinuteFileServer = CMediaServer::Create(a_rstrInFile.c_str());
    }
    else {
        m_pMatroskaParser = new CMatroskaFileParser(a_rstrInFile);
    }
    m_bInternalStore = bInternalStore;
    m_pEditList = NULL;
}


CMatroskaFileInfo::~CMatroskaFileInfo()
{
    delete m_pMatroskaParser;
    delete m_pMinuteFileServer;
    FreeXmlLib();
}


bool CMatroskaFileInfo::PrintInfo(const std::string & a_filter, std::string a_strValue)
{
    int rc;
    if (m_pMinuteFileServer != NULL) {
        m_pMinuteFileServer->Init();
    }
    else {
        rc = m_pMatroskaParser->Parse();
        if (rc != ELIBMC_OK)
            return false;
    }
    // Create a new top level document that will hold all the info xmls
    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(
            XTranslate("Core"));
    DOMDocument *doc = impl->createDocument(0, XTranslate("FileMetaData"), 0);

    if (a_filter == "all" || a_filter == "size") {
        AddSizeInfo(doc);
    }

    if (a_filter == "all" || a_filter == "gaps") {
        AddGapInfo(doc);
    }

    if (a_filter == "all" || a_filter == "bookmarks") {
        AddBookmarksInfo(doc);
    }

    if (a_filter == "all" || a_filter == "metadata") {
        AddMetadataInfo(doc, "all");
    }
    else if (a_filter == "blobdb") {
        AddMetadataInfo(doc, "IPVS_BLOB");
    }
    else if (a_filter == "msfrl") {
        AddMetadataInfo(doc, "IPVS_DB_MSFRL");
    }

    if (a_filter == "gettimecodeatoffset") {
        uint64_t nOffset = 0;
        if (a_strValue.empty() == false) {
            nOffset = STRTOULL(a_strValue.c_str(), NULL, 10);
        }
        AddTimeCodeAtOffset(doc, nOffset);
    }

    if (a_filter == "getoffsetattimecode") {
        uint64_t nTimeCode = 0;
        if (a_strValue.empty() == false) {
            nTimeCode = STRTOULL(a_strValue.c_str(), NULL, 10);
        }
        AddOffsetAtTimeCode(doc, nTimeCode);
    }

    // Create final document string
    DomDocToString(doc, m_resultString);

    // Dump to standard out
    if (!m_bInternalStore)
        cout << m_resultString;

    delete doc;

    return true;
}

void CMatroskaFileInfo::AddBookmarksInfo(DOMDocument * domDocument)
{
    if (m_pMatroskaParser == NULL)
        return;

    // Add a top level "Bookmarks" node under which we will add all the
    // bookmarks found
    DOMElement * bookMarksElement = domDocument->createElement(XTranslate("Bookmarks"));
    domDocument->getDocumentElement()->appendChild(bookMarksElement);


    // Create xml parser for parsing bookmarks
    XercesDOMParser * domParser = new XercesDOMParser();
    domParser->setValidationScheme(XercesDOMParser::Val_Never);
    domParser->setDoNamespaces(false);
    domParser->setDoSchema(false);
    domParser->setLoadExternalDTD(false);

    // Get Bookmarks
    list<string> bookMarksList;
    GetBookMarks(bookMarksList);

    // Add bookmarks to the xml
    for (list<string>::iterator iter = bookMarksList.begin();
            iter != bookMarksList.end(); ++iter) {
        MemBufInputSource inputSrcFromString((const XMLByte*)(*iter).c_str(), (*iter).size(),
                "dummy", false);
        domParser->parse(inputSrcFromString);

        DOMDocument * bookMarkDoc = domParser->getDocument();

        StripWhiteSpaceFromDom(bookMarkDoc->getDocumentElement());
        if (bookMarkDoc != NULL && bookMarkDoc->getDocumentElement() != NULL) {
            DOMNode *childNode = domDocument->importNode(bookMarkDoc->getDocumentElement(), true);
            bookMarksElement->appendChild(childNode);
        }
    }

    delete domParser;
}

void CMatroskaFileInfo::GetBookMarks(std::list<std::string> & bookMarksList)
{
    if (m_pEditList != NULL)
        return GetEditedBookMarks(bookMarksList);
    else
        return GetAllBookMarks(bookMarksList);

}

void CMatroskaFileInfo::GetEditedBookMarks(std::list<std::string> & bookMarksList)
{
    if (m_pMatroskaParser == NULL)
        return;
    m_pMatroskaParser->NeedBookmarkFrames(true);
    bool bDone = false;
    for (int item = 0; item < m_pEditList->GetItemCount(); ++item) {
        m_pMatroskaParser->InitializeMediaToTimeOffset(m_pEditList->StartOffset(item));
        while (true) {
            CCFrame aFrame;
            if (!m_pMatroskaParser->GetFrame(aFrame)) {
                bDone = true;
                break;
            }
            if (aFrame.GetTimeOffset() > m_pEditList->EndOffset(item)) {
                break;
            }
            bookMarksList.push_back(std::string((char *) aFrame.GetData(), aFrame.GetLength()));
        }
        if (bDone)
            break;
    }
}

void CMatroskaFileInfo::GetAllBookMarks(std::list<std::string> & bookMarksList)
{
    if (m_pMatroskaParser == NULL)
        return;

    m_pMatroskaParser->NeedBookmarkFrames(true);
    m_pMatroskaParser->InitializeMediaToTimeOffset(0);
    while (true) {
        CCFrame aFrame;
        if (!m_pMatroskaParser->GetFrame(aFrame)) {
            break;
        }

        bookMarksList.push_back(std::string((char *) aFrame.GetData(), aFrame.GetLength()));
    }
}

void CMatroskaFileInfo::AddMetadataInfo(DOMDocument * domDocument, const std::string& filter)
{
    if (m_pMatroskaParser == NULL)
        return;

    list<CMetadataHolder> metaDataList = m_pMatroskaParser->GetMetadata(filter);
    for (list<CMetadataHolder>::iterator iter = metaDataList.begin();
            iter != metaDataList.end(); ++iter) {
        DOMNode *childNode = domDocument->importNode(
                iter->m_pXmlDoc->getDocumentElement(), true);
        domDocument->getDocumentElement()->appendChild(childNode);
    }
}

void CMatroskaFileInfo::SetEditList(CEditList * editList)
{
    if (m_pEditList == NULL)
        m_pEditList = editList;
}

void CMatroskaFileInfo::AddSizeInfo(DOMDocument * domDocument)
{

    DOMElement * infoElement = domDocument->createElement(XTranslate("MediaSize"));
    domDocument->getDocumentElement()->appendChild(infoElement);

    DOMElement * sizeElement = domDocument->createElement(XTranslate("Size"));
    infoElement->appendChild(sizeElement);

    uint64_t mediaSize = ComputeSize();
    char sizeStringBuffer[32];
    sprintf(sizeStringBuffer, "%"PRIu64"", mediaSize);

    DOMText* txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
    sizeElement->appendChild(txtNode);
}

void CMatroskaFileInfo::AddGapInfo(DOMDocument * domDocument)
{
    DOMElement * infoElement = domDocument->createElement(XTranslate("GapsInfo"));
    domDocument->getDocumentElement()->appendChild(infoElement);

    DOMElement * durationElement = domDocument->createElement(XTranslate("Duration"));
    infoElement->appendChild(durationElement);

    DOMElement * timeSpanElement = domDocument->createElement(XTranslate("TimeSpan"));
    infoElement->appendChild(timeSpanElement);

    DOMElement * totalGapsElement = domDocument->createElement(XTranslate("TotalGaps"));
    infoElement->appendChild(totalGapsElement);

    DOMElement * mediaBeginElement = domDocument->createElement(XTranslate("MediaBeginTime"));
    infoElement->appendChild(mediaBeginElement);

    DOMElement * mediaEndElement = domDocument->createElement(XTranslate("MediaEndTime"));
    infoElement->appendChild(mediaEndElement);

    uint64_t nDuration = 0, nTimeSpan = 0, nTotalGaps = 0, nBeginTime = 0, nEndTime = 0;
    char sizeStringBuffer[32];

    //GetGapsInfo function fills nDuration, nTimeSpan and returns the list of gaps.
    list <CGap> lGaps = GetGapsInfo(nDuration, nTimeSpan, nBeginTime, nEndTime);

    std::list<CGap> :: iterator iter;
    for (iter = lGaps.begin(); iter != lGaps.end(); ++iter) {
        CGap gap = (*iter);
        nTotalGaps += (gap.GetEndTime() - gap.GetBeginTime());

        DOMElement * gapsElement = domDocument->createElement(XTranslate("Gap"));
        infoElement->appendChild(gapsElement);

        DOMElement * gapsBeginElement = domDocument->createElement(XTranslate("Begin"));
        gapsElement->appendChild(gapsBeginElement);

        sprintf(sizeStringBuffer, "%"PRIu64"", gap.GetBeginTime());
        DOMText* txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
        gapsBeginElement->appendChild(txtNode);

        DOMElement * gapsEndElement = domDocument->createElement(XTranslate("End"));
        gapsElement->appendChild(gapsEndElement);

        sprintf(sizeStringBuffer, "%"PRIu64"", gap.GetEndTime());
        txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
        gapsEndElement->appendChild(txtNode);
    }

    sprintf(sizeStringBuffer, "%"PRIu64"", nDuration);
    DOMText* txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
    durationElement->appendChild(txtNode);

    sprintf(sizeStringBuffer, "%"PRIu64"", nTotalGaps);
    txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
    totalGapsElement->appendChild(txtNode);

    sprintf(sizeStringBuffer, "%"PRIu64"", nTimeSpan);
    txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
    timeSpanElement->appendChild(txtNode);

    sprintf(sizeStringBuffer, "%"PRIu64"", nBeginTime);
    txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
    mediaBeginElement->appendChild(txtNode);

    sprintf(sizeStringBuffer, "%"PRIu64"", nEndTime);
    txtNode = domDocument->createTextNode(XTranslate(sizeStringBuffer));
    mediaEndElement->appendChild(txtNode);

}

std::list <CGap> CMatroskaFileInfo::GetGapsInfo(uint64_t & nDuration, uint64_t & nTimeSpan,
        uint64_t & nBeginTime, uint64_t & nEndTime)
{
    if (m_pEditList == NULL)
        return GetGapsInTotalMedia(nDuration, nTimeSpan, nBeginTime, nEndTime);
    else
        return GetGapsInEditedMedia(nDuration, nTimeSpan, nBeginTime, nEndTime);
}

std::list <CGap> CMatroskaFileInfo::GetGapsInTotalMedia(uint64_t & nDuration, uint64_t & nTimeSpan,
        uint64_t & nBeginTime, uint64_t & nEndTime)
{
    if (m_pMinuteFileServer != NULL) {
        nDuration = m_pMinuteFileServer->GetOffsetFromTimecode(m_pMinuteFileServer->GetMediaEndTimestamp());
        nTimeSpan = m_pMinuteFileServer->GetMediaEndTimestamp() - m_pMinuteFileServer->GetMediaStartTimestamp();
        nBeginTime = m_pMinuteFileServer->GetMediaStartTimestamp();
        nEndTime = m_pMinuteFileServer->GetMediaEndTimestamp();
    }
    else {
        nDuration = m_pMatroskaParser->GetOffsetFromTimecode(m_pMatroskaParser->GetMediaEndTimestamp());
        nTimeSpan = m_pMatroskaParser->GetMediaEndTimestamp() - m_pMatroskaParser->GetMediaBeginTimestamp();
        nBeginTime = m_pMatroskaParser->GetMediaBeginTimestamp();
        nEndTime = m_pMatroskaParser->GetMediaEndTimestamp();
    }

    return GetTotalGaps();
}

//This is common for both minute file media and Matroska media.
list<CGap> CMatroskaFileInfo::GetGapsInEditedMedia(uint64_t & nDuration, uint64_t & nTimeSpan,
        uint64_t & nBeginTime, uint64_t & nEndTime)
{
    list <CGap> lGapsOrg;
    list <CGap> lGaps;
    uint64_t nStartTimeStamp = 0, nEndTimeStamp = 0;
    nTimeSpan = nDuration = 0;

    lGapsOrg = GetTotalGaps();

    std::list<CGap> :: iterator iter;
    int nTotalItems = m_pEditList->GetItemCount();
    for (int item = 0; item < nTotalItems; ++item) {
        if (m_pMinuteFileServer != NULL) {
            nStartTimeStamp = m_pMinuteFileServer->GetTimecodeAtOffset(m_pEditList->StartOffset(item));
            nEndTimeStamp = m_pMinuteFileServer->GetTimecodeAtOffset(m_pEditList->EndOffset(item));
        }
        else {
            nStartTimeStamp = m_pMatroskaParser->GetTimecodeAtOffset(m_pEditList->StartOffset(item));
            nEndTimeStamp = m_pMatroskaParser->GetTimecodeAtOffset(m_pEditList->EndOffset(item));
        }

        nDuration += (m_pEditList->EndOffset(item) - m_pEditList->StartOffset(item));
        for (iter = lGapsOrg.begin(); iter != lGapsOrg.end(); ++iter) {
            CGap orgGap = (*iter);

            if (nStartTimeStamp < orgGap.GetBeginTime()) {
                if (nEndTimeStamp > orgGap.GetEndTime()) {
                    CGap gap(orgGap.GetBeginTime(), orgGap.GetEndTime());
                    lGaps.push_back(gap);
                }
            }
        }

        //If there are more cut list, we have to insert gaps in between.
        if (nTotalItems > (item + 1)) {
            uint64_t nGapEndTime = 0;
            if (m_pMinuteFileServer != NULL) {
                nGapEndTime = m_pMinuteFileServer->GetTimecodeAtOffset(m_pEditList->StartOffset(item + 1));
            }
            else {
                nGapEndTime = m_pMatroskaParser->GetTimecodeAtOffset(m_pEditList->StartOffset(item + 1));
            }

            CGap gap(nEndTimeStamp, nGapEndTime);
            lGaps.push_back(gap);
        }
    }

    if (m_pMinuteFileServer != NULL) {
        nStartTimeStamp = m_pMinuteFileServer->GetTimecodeAtOffset(m_pEditList->StartOffset(0));
        nEndTimeStamp = m_pMinuteFileServer->GetTimecodeAtOffset(m_pEditList->EndOffset(nTotalItems - 1));
    }
    else {
        nStartTimeStamp = m_pMatroskaParser->GetTimecodeAtOffset(m_pEditList->StartOffset(0));
        nEndTimeStamp = m_pMatroskaParser->GetTimecodeAtOffset(m_pEditList->EndOffset(nTotalItems - 1));
    }

    nTimeSpan = nEndTimeStamp - nStartTimeStamp;
    nBeginTime = nStartTimeStamp;
    nEndTime = nEndTimeStamp;

    return lGaps;
}

list <CGap> CMatroskaFileInfo::GetTotalGaps()
{
    if (m_pMinuteFileServer != NULL) {
        return (m_pMinuteFileServer->GetGaps());
    }
    else {
        return (m_pMatroskaParser->GetGaps());
    }
}

void CMatroskaFileInfo::AddTimeCodeAtOffset(XERCES_NS::DOMDocument * domDocument,
                                            uint64_t & a_nOffset)
{
    uint64_t nTimeCode = 0;
    if (m_pMinuteFileServer != NULL) {
        nTimeCode = m_pMinuteFileServer->GetTimecodeAtOffset(a_nOffset);
    }
    else {
        nTimeCode = m_pMatroskaParser->GetTimecodeAtOffset(a_nOffset);
    }

    DOMElement * timeBaseElement = domDocument->createElement(XTranslate("TimeCodeAtOffset"));
    domDocument->getDocumentElement()->appendChild(timeBaseElement);

    char strBuffer[32];

    DOMElement * offsetElement = domDocument->createElement(XTranslate("Offset"));
    timeBaseElement->appendChild(offsetElement);

    sprintf(strBuffer, "%"PRIu64"", a_nOffset);
    DOMText* txtOffsetNode = domDocument->createTextNode(XTranslate(strBuffer));
    offsetElement->appendChild(txtOffsetNode);

    DOMElement * timeElement = domDocument->createElement(XTranslate("TimeCode"));
    timeBaseElement->appendChild(timeElement);

    sprintf(strBuffer, "%"PRIu64"", nTimeCode);
    DOMText* txtNode = domDocument->createTextNode(XTranslate(strBuffer));
    timeElement->appendChild(txtNode);
}

void CMatroskaFileInfo::AddOffsetAtTimeCode(XERCES_NS::DOMDocument * domDocument,
                                            uint64_t & a_nTimeCode)
{
    uint64_t nOffset = 0;
    if (m_pMinuteFileServer != NULL) {
        nOffset = m_pMinuteFileServer->GetOffsetFromTimecode(a_nTimeCode);
    }
    else {
        nOffset = m_pMatroskaParser->GetOffsetFromTimecode(a_nTimeCode);
    }

    DOMElement * offsetBaseElement = domDocument->createElement(XTranslate("OffsetAtTimeCode"));
    domDocument->getDocumentElement()->appendChild(offsetBaseElement);

    char strBuffer[32];

    DOMElement * timeElement = domDocument->createElement(XTranslate("TimeCode"));
    offsetBaseElement->appendChild(timeElement);

    sprintf(strBuffer, "%"PRIu64"", a_nTimeCode);
    DOMText* txtTimeNode = domDocument->createTextNode(XTranslate(strBuffer));
    timeElement->appendChild(txtTimeNode);

    DOMElement * offsetElement = domDocument->createElement(XTranslate("Offset"));
    offsetBaseElement->appendChild(offsetElement);

    sprintf(strBuffer, "%"PRIu64"", nOffset);
    DOMText* txtOffsetNode = domDocument->createTextNode(XTranslate(strBuffer));
    offsetElement->appendChild(txtOffsetNode);
}

uint64_t CMatroskaFileInfo::GetMediaSize()
{
    return ComputeSize();
}

uint64_t CMatroskaFileInfo::ComputeSize()
{
    if (m_pEditList == NULL)
        return ComputeTotalSize();
    else
        return ComputeEditedSize();
}

uint64_t CMatroskaFileInfo::ComputeTotalSize()
{
    if (m_pMinuteFileServer != NULL)
        return ComputeMinuteTotalSize();
    else
        return ComputeMatroskaTotalSize();
}

uint64_t CMatroskaFileInfo::ComputeMatroskaTotalSize()
{
    uint64_t beginFileOffset = 0;
    uint64_t endFileOffset = 0;
    uint64_t totalSize = 0;
    CCFrame aFrame;

    m_pMatroskaParser->NeedBookmarkFrames(false);
    m_pMatroskaParser->InitializeMediaToTimeOffset(0);
    if (m_pMatroskaParser->GetFrame(aFrame))
        beginFileOffset = m_pMatroskaParser->GetCurrentBlockPosition();

    m_pMatroskaParser->InitializeMediaToTimeOffset(m_pMatroskaParser->GetMediaDuration());
    if (m_pMatroskaParser->GetFrame(aFrame))
        endFileOffset = m_pMatroskaParser->GetCurrentBlockPosition();
    if (beginFileOffset > 0 && endFileOffset >= beginFileOffset)
        totalSize = endFileOffset - beginFileOffset;
    if (totalSize == 0) {
        totalSize = m_pMatroskaParser->GetFileSize();
    }
    return totalSize;
}

uint64_t CMatroskaFileInfo::ComputeMinuteTotalSize()
{
    return m_pMinuteFileServer->GetMediaSize(0, m_pMinuteFileServer->GetMediaDuration());
}

uint64_t CMatroskaFileInfo::ComputeEditedSize()
{
    if (m_pMinuteFileServer != NULL)
        return ComputeMinuteEditedSize();
    else
        return ComputeMatroskaEditedSize();
}

uint64_t CMatroskaFileInfo::ComputeMatroskaEditedSize()
{
    uint64_t totalSize = 0;
    m_pMatroskaParser->NeedBookmarkFrames(false);
    for (int item = 0; item < m_pEditList->GetItemCount(); ++item) {
        CCFrame aFrame;
        uint64_t beginFileOffset = 0;
        uint64_t endFileOffset = 0;

        m_pMatroskaParser->InitializeMediaToTimeOffset(m_pEditList->StartOffset(item));
        if (m_pMatroskaParser->GetFrame(aFrame))
            beginFileOffset = m_pMatroskaParser->GetCurrentBlockPosition();

        uint64_t endOffset = m_pEditList->EndOffset(item);
        if (endOffset > m_pMatroskaParser->GetMediaDuration())
            endOffset = m_pMatroskaParser->GetMediaDuration();
        m_pMatroskaParser->InitializeMediaToTimeOffset(endOffset);
        if (m_pMatroskaParser->GetFrame(aFrame))
            endFileOffset = m_pMatroskaParser->GetCurrentBlockPosition();
        if (beginFileOffset > 0 && endFileOffset >= beginFileOffset)
            totalSize += endFileOffset - beginFileOffset;
    }
    return totalSize;
}

uint64_t CMatroskaFileInfo::ComputeMinuteEditedSize()
{
    uint64_t totalSize = 0;
    for (int item = 0; item < m_pEditList->GetItemCount(); ++item) {
        totalSize += m_pMinuteFileServer->GetMediaSize(m_pEditList->StartOffset(item),
                m_pEditList->EndOffset(item));
    }
    return totalSize;
}

const std::string & CMatroskaFileInfo::GetResultString()
{
    return m_resultString;
}

}
