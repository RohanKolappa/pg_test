#ifndef MATROSKA_FILE_PARSER_H
#define MATROSKA_FILE_PARSER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include <list>

#include "libmediaconverter.hh"

#ifdef __MINGW32__
#define XERCES_NS xercesc_2_8
#else
#define XERCES_NS xercesc_2_7
#endif

namespace XERCES_NS {
    class DOMDocument;
}

namespace mediaindex {

class CMetadataHolder
{
public:
    std::string m_name;
    std::string m_val;
    XERCES_NS::DOMDocument* m_pXmlDoc;
};

class CMatroskaFileParser
{
public:
    CMatroskaFileParser(const std::string& a_rstrInFile);
    ~CMatroskaFileParser();

    eLIBMC_ERROR  Parse();
    std::list<CMetadataHolder> GetMetadata(std::string a_filter);
    bool InitializeMediaToTimeOffset(uint64_t a_nOffset);
    void NeedBookmarkFrames(bool bEnable);
    void ServeThumbnails(bool bEnable);
    bool HasThumbnails();
    bool IsSnapshot();
    bool GetFrame(CCFrame &a_frame);
    uint64_t GetFileSize();
    uint64_t GetCurrentClusterPosition();
    uint64_t GetCurrentBlockPosition();
    uint64_t GetCurrentTimeOffset();
    uint64_t GetMediaCreateDate();
    uint64_t GetMediaDuration();
    std::string GetMediaTitle();
    std::string GetStreamType();
    uint64_t GetMediaBeginTimestamp();
    uint64_t GetMediaEndTimestamp();
    uint64_t GetTimecodeAtOffset(uint64_t offset);
    uint64_t GetOffsetFromTimecode(uint64_t timecode);
    std::string GetErrorString();
    std::list <CGap> GetGaps();

private:
    struct Impl;
    Impl* m_pimpl;

    CMatroskaFileParser(const CMatroskaFileParserImpl &a_rParser);
    CMatroskaFileParser &operator=(const CMatroskaFileParserImpl &a_rParser);
};

};



#endif //MATROSKA_FILE_PARSER_H
