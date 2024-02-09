/*
 * xmlUtils.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: rkale
 */


#include "stdio.h"
#include "xercesutils.h"
#include "xmlUtils.h"

/*
 * The beginnings of a general XML parser utility that currently
 * only exposes an API to return the text of a given element.
 * More functions will be added as necessary.
 */

class CXMLFileParser {

public:
    CXMLFileParser(const char *fileName);
    ~CXMLFileParser();
    bool HasError();
    const char * GetElementText(const char * elementName);

private:
    DOMDocument* m_doc;
    XercesDOMParser* m_parser;
    char * m_lastTrancodedString;
};


CXMLFileParser::CXMLFileParser(const char *fileName)
{

    m_doc = NULL;
    m_lastTrancodedString = NULL;
    XMLPlatformUtils::Initialize();

    m_parser = new XercesDOMParser();
    m_parser->setValidationScheme(XercesDOMParser::Val_Never);
    m_parser->setDoNamespaces(false);
    m_parser->setDoSchema(false);
    m_parser->setLoadExternalDTD(false);

    m_parser->parse(fileName);
    m_doc = m_parser->getDocument();
}

CXMLFileParser::~CXMLFileParser()
{
    XFree(m_lastTrancodedString);
    delete m_parser;
}

bool CXMLFileParser::HasError()
{
    if (m_doc == NULL)
        return true;
    DOMElement* root = m_doc->getDocumentElement();
    if (root == NULL)
        return true;
    return false;
}

const char * CXMLFileParser::GetElementText(const char * elementName)
{
    if (m_doc == NULL)
        return NULL;

    DOMNodeList* nodes = m_doc->getElementsByTagName(X(elementName));

    char * elementText = NULL;
    for (unsigned int i = 0; i < nodes->getLength(); i++) {
        DOMNode* element = nodes->item(i)->getFirstChild();
        if (element->getNodeType() == DOMNode::TEXT_NODE) {
            const XMLCh* str = element->getNodeValue();
            elementText = XTC(str);
            XFree(m_lastTrancodedString);
            m_lastTrancodedString = elementText;
            break;
        }
    }
    return elementText;
}

/*********************** C API implementation *************************/

XMLParser *createXMLParser(const char * filename)
{
    try
    {
        XMLParser *parser = new XMLParser;
        parser->handle = new CXMLFileParser(filename);
        return parser;
    }
    catch(...)
    {
    }
    return NULL;
}



static inline CXMLFileParser * getXMLFileParser(XMLParser * parser) {
    return static_cast<CXMLFileParser *>(parser->handle);
}

void destroyXMLParser(XMLParser * parser)
{
    delete getXMLFileParser(parser);
    delete parser;
}


const char * XMLParserGetElementText(XMLParser * parser, const char * elementName)
{
    return getXMLFileParser(parser)->GetElementText(elementName);
}

int XMLParserHasError(XMLParser * parser)
{
    if (getXMLFileParser(parser)->HasError())
        return 1;
    return 0;
}

