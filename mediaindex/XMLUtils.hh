#ifndef CXMLUTILS__C48BDF27_992A_43CA_9048_97359C06B851_H__
#define CXMLUTILS__C48BDF27_992A_43CA_9048_97359C06B851_H__

#include<xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include<string>
#include<iostream>
#include<sstream>
#include<stdexcept>
#include<stack>

#include <matroska/KaxTags.h>
#include <matroska/KaxTag.h>

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

//XERCES_CPP_NAMESPACE_USE

#ifdef __MINGW32__
#define XERCES_NS xercesc_2_8
#else
#define XERCES_NS xercesc_2_7
#endif

//namespace XERCES_NS {
//    class DOMDocument;
//}

#include "libmediaconverter.hh"

namespace mediaindex {

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  transcoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XERCES_NS::XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        XERCES_NS::XMLString::release(&fLocalForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  transcoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XERCES_NS::XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XERCES_NS::XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define XTranslate(str) XStr(str).unicodeForm()
#define XTranslateString(str) XStr((str).c_str()).unicodeForm()


inline std::string TrimWhitespaces(const std::string& a_rstr)
{
    const char WHITESPACE[] = " \t\n\v\f\r";
    const size_t nBegin = a_rstr.find_first_not_of(WHITESPACE);
    const size_t nEnd   = a_rstr.find_last_not_of(WHITESPACE);

    std::string strTrimmed;
    if (nBegin != std::string::npos)
        strTrimmed.assign( a_rstr.begin() + nBegin, a_rstr.begin() + nEnd + 1);

    return strTrimmed;
}

class XmlToEbmlTags
{
public:
    XmlToEbmlTags();
    ~XmlToEbmlTags();

    bool GetTagElements(xercesc::DOMElement* a_pRootElem,
            KaxTagSimple& a_rParentTag);

private:
    void ParseDOMElement(xercesc::DOMElement* a_pElem, const int a_nLevel,
        EbmlMaster& a_rParent);
};

class EbmlTagsToXml
{
public:
    EbmlTagsToXml();
    ~EbmlTagsToXml();

    XERCES_NS::DOMDocument* GetXmlDoc(KaxTagSimple* a_pKaxTagSimple);

private:
    XERCES_NS::DOMImplementation*      m_pDomImpl;
    XERCES_NS::DOMDocument*            m_pDomDoc;
    std::stack<XERCES_NS::DOMElement*> m_XmlElementArr;

    int     m_nAttribLevel;
    std::string  m_strCurrentAttribName;

    void HandleTagElements(const int a_nLevel, EbmlElement *a_pEl);
};

bool WriteDomDoc(XERCES_NS::DOMDocument* a_pDomDoc, const std::string& a_rstrOutFile);
bool DomDocToString(XERCES_NS::DOMDocument* a_pDomDoc, std::string& a_rstrOutString);
XERCES_NS::DOMDocument * DomDocFromString(const std::string & xmlString);
void StripWhiteSpaceFromDom(XERCES_NS::DOMElement* a_pElem);

void InitXmlLib();
void FreeXmlLib();

inline bool XmlToEbml(XERCES_NS::DOMElement* a_pRootElem, KaxTagSimple& a_rParentTag)
{
    return XmlToEbmlTags().GetTagElements(a_pRootElem, a_rParentTag);
}

inline XERCES_NS::DOMDocument* EbmlToXml(KaxTagSimple* a_pKaxTagSimple)
{
    return EbmlTagsToXml().GetXmlDoc(a_pKaxTagSimple);
}

const std::string GetTextValueOfChild(XERCES_NS::DOMElement *a_element, const char *a_tagname);
XERCES_NS::DOMElement * GetChildElement(XERCES_NS::DOMElement *a_element, const char *a_tagname);

};

#endif //CXMLUTILS__C48BDF27_992A_43CA_9048_97359C06B851_H__

