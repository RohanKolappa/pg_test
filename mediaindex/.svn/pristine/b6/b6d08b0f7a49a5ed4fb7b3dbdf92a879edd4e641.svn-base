/*
 * MatroskaFileInfo.h
 *
 *  Created on: Jan 24, 2011
 *      Author: rkale
 */

#ifndef MATROSKAFILEINFO_H_
#define MATROSKAFILEINFO_H_

#include <string>

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

class CMatroskaFileParser;
class CMediaServer;
class CEditList;
class CGaps;

class CMatroskaFileInfo
{
public:

   CMatroskaFileInfo(const std::string& a_rstrInFile, bool bInternalStore = false);
   ~CMatroskaFileInfo();
   bool PrintInfo(const std::string & a_filter, std::string a_strValue = "");
   void SetEditList(CEditList * editList);
   uint64_t GetMediaSize();

   const std::string & GetResultString();

private:
   void AddMetadataInfo(XERCES_NS::DOMDocument * domDocument, const std::string& filter);
   void AddBookmarksInfo(XERCES_NS::DOMDocument * domDocument);
   void AddSizeInfo(XERCES_NS::DOMDocument * domDocument);
   void AddGapInfo(XERCES_NS::DOMDocument * domDocument);
   void AddTimeCodeAtOffset(XERCES_NS::DOMDocument * domDocument, uint64_t & nOffset);
   void AddOffsetAtTimeCode(XERCES_NS::DOMDocument * domDocument, uint64_t & nTimeCode);
   uint64_t ComputeSize();
   std::list <CGap> GetGapsInfo(uint64_t & nDuration, uint64_t & nTimeSpan,
           uint64_t & nBeginTime, uint64_t & nEndTime);
   std::list <CGap> GetGapsInTotalMedia(uint64_t & nDuration, uint64_t & nTimeSpan,
           uint64_t & nBeginTime, uint64_t & nEndTime);
   std::list <CGap> GetGapsInEditedMedia(uint64_t & nDuration, uint64_t & nTimeSpan,
           uint64_t & nBeginTime, uint64_t & nEndTime);
   std::list <CGap> GetTotalGaps();
   uint64_t ComputeTotalSize();
   uint64_t ComputeEditedSize();
   uint64_t ComputeMatroskaTotalSize();
   uint64_t ComputeMatroskaEditedSize();
   uint64_t ComputeMinuteTotalSize();
   uint64_t ComputeMinuteEditedSize();
   void GetBookMarks(std::list<std::string> & bookMarksList);
   void GetEditedBookMarks(std::list<std::string> & bookMarksList);
   void GetAllBookMarks(std::list<std::string> & bookMarksList);

   CMatroskaFileInfo();
   CMatroskaFileInfo(const CMatroskaFileInfo &a_pInfoProvider);
   CMatroskaFileInfo &operator=(const CMatroskaFileInfo &a_pInfoProvider);

   CMatroskaFileParser * m_pMatroskaParser;
   CMediaServer * m_pMinuteFileServer;
   bool m_bInternalStore;
   std::string m_resultString;
   CEditList * m_pEditList;
};

};

#endif /* MATROSKAFILEINFO_H_ */
