#include "XMLUtils.hh"
#include "MatroskaUtils.hh"

XERCES_CPP_NAMESPACE_USE

using std::exception;
using std::cout;
using std::cerr;
using std::endl;

#define STRATTRIBUTES "_Attributes_"

namespace mediaindex {

XmlToEbmlTags::XmlToEbmlTags()
{
}

XmlToEbmlTags::~XmlToEbmlTags()
{
}

bool XmlToEbmlTags::GetTagElements(xercesc::DOMElement* a_pRootElem,
        KaxTagSimple& a_rParentTag)
{
    ParseDOMElement(a_pRootElem, 1, a_rParentTag);
    return true;
}

void XmlToEbmlTags::ParseDOMElement(DOMElement* a_pElem, const int a_nLevel,
                                    EbmlMaster& a_rParent)
{
    std::string strLevelIndent(a_nLevel, ' ');

    std::ostringstream summary;

    summary << strLevelIndent;

    StrX strElementName(a_pElem->getTagName());
    string strElementText;

    summary << "<" << strElementName.localForm() << ">" << endl;

    KaxTagSimple& rTag = AddSimpleTag(a_rParent, strElementName.localForm(), "");

    KaxTagSimple* pAttributesTag = NULL;
    // Walk down the attributes list if not empty
    DOMNamedNodeMap* pAttrMap = a_pElem->getAttributes();
    for(XMLSize_t n = 0; n < pAttrMap->getLength(); n++)
    {
        if(n == 0)
        {
            pAttributesTag = &(AddSimpleTag(rTag, STRATTRIBUTES, ""));
            summary << strLevelIndent << "<Attributes>" << endl;
        }

        DOMNode* pNode = pAttrMap->item(n);
        StrX strNodeName(pNode->getNodeName());
        StrX strNodeVal(pNode->getNodeValue());

        AddSimpleTag(*pAttributesTag, strNodeName.localForm(), strNodeVal.localForm());

        summary << strLevelIndent << strNodeName.localForm() << ":" <<
            strNodeVal.localForm() << endl;

        if(n == pAttrMap->getLength()-1)
            summary << strLevelIndent << "</Attributes>" << endl;
    }

    DOMNodeList* pChildrenList = a_pElem->getChildNodes() ;
    const XMLSize_t nNodeCount = pChildrenList->getLength() ;

    int nElementNodes = 0;
    for( XMLSize_t ix = 0 ; ix < nNodeCount ; ++ix )
    {

        DOMNode* pCurrentNode = pChildrenList->item( ix ) ;
        if( NULL == pCurrentNode )
        {
            // null node...
            continue ;
        }

        if( DOMNode::ELEMENT_NODE != pCurrentNode->getNodeType() )
        {
            // not an element node -> not of interest here
            continue ;
        }

        nElementNodes++;
        ParseDOMElement(dynamic_cast< DOMElement* >( pCurrentNode ),
            a_nLevel+1, rTag);
    }

    for( XMLSize_t ix = 0 ; (ix < nNodeCount); ++ix )
    {

        DOMNode* pCurrentNode = pChildrenList->item( ix ) ;
        if( NULL == pCurrentNode )
        {
            // null node...
            continue ;
        }

        if( DOMNode::TEXT_NODE == pCurrentNode->getNodeType() )
        {
            StrX strNodeVal(pCurrentNode->getNodeValue());
            strElementText += strNodeVal.localForm();
        }

    }

    string strElementTextTrimmed = TrimWhitespaces(strElementText);
    ChangeSimpleTagValue(rTag, strElementTextTrimmed);

    summary << strLevelIndent << strElementTextTrimmed << endl;
    summary << strLevelIndent << "</" << strElementName.localForm() << ">" <<
        endl;
   //  cout << summary.str() << endl;

}

EbmlTagsToXml::EbmlTagsToXml()
: m_pDomImpl(NULL)
, m_pDomDoc(NULL)
, m_nAttribLevel(0)
{
}

EbmlTagsToXml::~EbmlTagsToXml()
{
}


DOMDocument* EbmlTagsToXml::GetXmlDoc(KaxTagSimple* a_pKaxTagSimple)
{
    // The caller is responsible for releasing the document ptr returned
    try {

        XMLCh tempStr[100];
        XMLString::transcode("LS", tempStr, 99);
        m_pDomImpl = DOMImplementationRegistry::getDOMImplementation(tempStr);

        //assert(m_XmlElementArr.empty());
        HandleTagElements(2, a_pKaxTagSimple);
        //assert(m_XmlElementArr.empty());

        return m_pDomDoc;
    }
    catch (const OutOfMemoryException&)
    {
        return NULL;
    }
    catch (XMLException& e)
    {
        return NULL;
    }

}

#define HANDLE_MASTERTAGELEMENT \
{ \
    EbmlMaster* pMaster = dynamic_cast<EbmlMaster *>(a_pEl); \
    assert(pMaster); \
    for(size_t n = 0; n < pMaster->ListSize(); n++) \
        HandleTagElements(a_nLevel+1, (*pMaster)[n]); \
}

void EbmlTagsToXml::HandleTagElements(const int a_nLevel, EbmlElement *a_pEl)
{
    std::ostringstream summary;

    string strLevelIndent(a_nLevel, ' ');

    summary << strLevelIndent;

    if(ISKAXTAGID(a_pEl))
    {
        summary << "Tag" << endl;
        HANDLE_MASTERTAGELEMENT;
    }
    else if(ISKAXTAGTARGETSID(a_pEl))
    {
        summary << "Targets" << endl;
        HANDLE_MASTERTAGELEMENT;
    }
    else if(ISKAXTARGETTYPEVALUEID(a_pEl))
    {
        summary << "TargetTypeValue ";
        summary << uint64(*dynamic_cast<EbmlUInteger *>(a_pEl)) << endl;
    }
    else if(ISKAXTARGETTYPEID(a_pEl))
    {
        summary << "TargetType ";
        summary << string(*dynamic_cast<EbmlString *>(a_pEl)) << endl;
    }
    else if(ISKAXTAGTRACKUIDID(a_pEl))
    {
        //assert(false);
    }
    else if(ISKAXTAGEDITIONUIDID(a_pEl))
    {
        //assert(false);
    }
    else if(ISKAXTAGCHAPTERUIDID(a_pEl))
    {
        //assert(false);
    }
    else if(ISKAXTAGATTACHMENTUIDID(a_pEl))
    {
        //assert(false);
    }
    else if(ISKAXSIMPLETAGID(a_pEl))
    {
        summary << "SimpleTag" << endl;
        HANDLE_MASTERTAGELEMENT;

        if( !m_nAttribLevel || a_nLevel < m_nAttribLevel )
        {
            m_XmlElementArr.pop();
        }

        if(m_nAttribLevel == a_nLevel)
        {
            m_nAttribLevel = 0;
        }
    }
    else if(ISKAXTAGNAMEID(a_pEl))
    {
        string strTagName = GetUTF8StrFromEbmlUnicodeStr(
            *static_cast<EbmlUnicodeString *>(a_pEl));

        if (m_pDomDoc == NULL) {
            m_pDomDoc = m_pDomImpl->createDocument(0, XTranslateString(strTagName), 0);
            m_XmlElementArr.push( m_pDomDoc->getDocumentElement() );
        }
        else if (m_nAttribLevel) {
            m_strCurrentAttribName = strTagName;
        }
        else if (strTagName == STRATTRIBUTES) {
            //assert(m_nAttribLevel == 0);
            m_nAttribLevel = a_nLevel-1;
        }
        else {
            DOMElement* pDomEl = m_pDomDoc->createElement(XTranslateString(strTagName));
            m_XmlElementArr.top()->appendChild(pDomEl);
            m_XmlElementArr.push( pDomEl );
        }

        summary << "Name " << strTagName << endl;
    }
    else if (ISKAXTAGLANGUAGEID(a_pEl))
    {
        summary << "Language ";
        summary << string(*dynamic_cast<EbmlString *>(a_pEl)) << endl;
    }
    else if (ISKAXTAGDEFAULTID(a_pEl))
    {
        summary << "Default ";
        summary << uint64(*dynamic_cast<EbmlUInteger *>(a_pEl)) << endl;
    }
    else if(ISKAXTAGSTRINGID(a_pEl))
    {
        string strTagVal = GetUTF8StrFromEbmlUnicodeStr(
            *static_cast<EbmlUnicodeString *>(a_pEl));
        summary << "String " << strTagVal << endl;

        if ((m_nAttribLevel + 1 == a_nLevel)) {} // Tag name was STRATTRIBUTES
        else if(m_nAttribLevel)
        {
            m_XmlElementArr.top()->setAttribute(
                    XTranslateString(m_strCurrentAttribName),
                    XTranslateString(strTagVal));
        }
        else
        {
            DOMText* pTextVal = m_pDomDoc->createTextNode(XTranslateString(strTagVal));
            m_XmlElementArr.top()->appendChild(pTextVal);
        }

    }
    else if(ISKAXTAGBINARYID(a_pEl))
    {
        summary << "Binary tag" << endl;
    }
    else
    {
        cerr << "Found unknown element inside the <Tags> element" << endl;
    }
    //  cout << summary.str() << endl;

}

bool WriteDomDoc(DOMDocument* a_pDomDoc, const string& a_rstrOutFile)
{
    try {

        XMLCh tempStr[100];
        XMLString::transcode("LS", tempStr, 99);
        DOMImplementation* pDomImpl =
            DOMImplementationRegistry::getDOMImplementation(tempStr);

        DOMWriter* pDomSerializer = ((DOMImplementationLS*)pDomImpl)->createDOMWriter();

        pDomSerializer->setEncoding(NULL);
        // set feature if the serializer supports the feature/mode
        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, true))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, true);

        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTBOM, false))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTBOM, false);

        XMLFormatTarget* pXMLTarget = new LocalFileFormatTarget(a_rstrOutFile.c_str());
        pDomSerializer->writeNode(pXMLTarget, *a_pDomDoc);
        delete pDomSerializer;
        delete pXMLTarget;

    }
    catch (const OutOfMemoryException&)
    {
        return false;
    }
    catch (XMLException& e)
    {
        return false;
    }
    return true;
}

bool DomDocToString(DOMDocument* a_pDomDoc, string& a_rstrOutString)
{
    try {

        XMLCh tempStr[100];
        XMLString::transcode("LS", tempStr, 99);
        DOMImplementation* pDomImpl =
            DOMImplementationRegistry::getDOMImplementation(tempStr);

        DOMWriter* pDomSerializer = ((DOMImplementationLS*)pDomImpl)->createDOMWriter();

        pDomSerializer->setEncoding(NULL);
        // Set feature if the serializer supports the feature/mode
        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, true))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, true);

        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

        if (pDomSerializer->canSetFeature(XMLUni::fgDOMWRTBOM, false))
            pDomSerializer->setFeature(XMLUni::fgDOMWRTBOM, false);


        MemBufFormatTarget *memBufTarget = new MemBufFormatTarget();

        pDomSerializer->writeNode(memBufTarget, *a_pDomDoc);

        a_rstrOutString.assign((char *) memBufTarget->getRawBuffer());

        delete pDomSerializer;
        delete memBufTarget;
    }
    catch (const OutOfMemoryException&)
    {
        return false;
    }
    catch (XMLException& e)
    {
        return false;
    }
    return true;
}

DOMDocument * DomDocFromString(const string & xmlString)
{
    XercesDOMParser * domParser = new XercesDOMParser();
    domParser->setValidationScheme(XercesDOMParser::Val_Never);
    domParser->setDoNamespaces(false);
    domParser->setDoSchema(false);
    domParser->setLoadExternalDTD(false);

    MemBufInputSource inputSrcFromString((const XMLByte*)xmlString.c_str(), xmlString.length(),
            "dummy", false);
    domParser->parse(inputSrcFromString);

    return domParser->getDocument();
}

// Get first child element of given element matching the given tag name
DOMElement *
GetChildElement(DOMElement *a_element, const char *a_tagname)
{
    if (a_element == NULL)
        return NULL;
    // For each child of root
    DOMNodeList* nodes = a_element->getChildNodes();
    for (int i = 0; i < (int) nodes->getLength(); i++) {
        DOMNode* node = nodes->item(i);
        if (node == NULL)
            continue;
        if (node->getNodeType() != DOMNode::ELEMENT_NODE)
            continue;
        DOMElement *child = dynamic_cast<DOMElement*>(node);
        StrX name(child->getTagName());
        // Match element name of child with given tag
        if (strcmp(name.localForm(), a_tagname) == 0)
            return child;
    }
    return NULL;
}

// Get the text value of first child of given element matching
// the given tag name
const string
GetTextValueOfChild(DOMElement *a_element, const char *a_tagname)
{
    DOMElement *element = GetChildElement(a_element, a_tagname);
    if (element == NULL)
        return string("");

    // Accumulate text value of the child
    string textvalue;
    DOMNodeList* nodelist = element->getChildNodes();
    for (int j = 0; j < (int) nodelist->getLength(); j++) {
        DOMNode* aNode = nodelist->item(j);
        if (aNode == NULL)
            continue;
        if (aNode->getNodeType() != DOMNode::TEXT_NODE)
            continue;
        StrX strNodeVal(aNode->getNodeValue());
        textvalue += strNodeVal.localForm();
    }
    return TrimWhitespaces(textvalue);
}

void StripWhiteSpaceFromDom(DOMElement* a_pElem)
{
    if (a_pElem == NULL)
        return;

    // First recurse down child elements
    DOMNodeList* pChildrenList = a_pElem->getChildNodes();
    int elementCount = 0;
    for (XMLSize_t ix = 0; ix < pChildrenList->getLength(); ++ix) {
        DOMNode* pCurrentNode = pChildrenList->item(ix);
        if (NULL == pCurrentNode)
            continue;

        if (DOMNode::ELEMENT_NODE == pCurrentNode->getNodeType()) {
            elementCount++;
            StripWhiteSpaceFromDom(dynamic_cast<DOMElement*> (pCurrentNode));
        }
    }

    // Find empty text nodes to remove
    std::list<DOMNode *> nodeList;
    for (XMLSize_t ix = 0; ix < pChildrenList->getLength(); ++ix) {
        DOMNode* pCurrentNode = pChildrenList->item(ix);
        if (NULL == pCurrentNode)
            continue;

        if (DOMNode::TEXT_NODE == pCurrentNode->getNodeType()) {
            StrX strNodeVal(pCurrentNode->getNodeValue());
            string trimmedText = TrimWhitespaces(strNodeVal.localForm());
            // If this element has child elements, mark all empty
            // text nodes for removal
            if (elementCount > 0 && trimmedText.size() == 0) {
                nodeList.push_back(pCurrentNode);
            }
        }
    }
    // Remove the nodes found in above loop
    while (nodeList.size() > 0) {
        a_pElem->removeChild(nodeList.front());
        nodeList.pop_front();
    }
}

void InitXmlLib()
{
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (XMLException& e) {

        char* message = XMLString::transcode( e.getMessage() ) ;
        cerr << "XML toolkit initialization error: "
            << message
            << endl
            ;

        XMLString::release( &message ) ;
    }

}

void FreeXmlLib()
{
    try {
        XMLPlatformUtils::Terminate();
    }
    catch (XMLException& e) {

        char* message = XMLString::transcode( e.getMessage() ) ;

        cerr << "XML toolkit teardown error: "
                << message
                << endl
                ;

        XMLString::release( &message ) ;
    }

}

};
