
#include "stdio.h"
#include "xercesutils.h"
#include "vlog.h"


class CStrmSrvXml {

public:
    CStrmSrvXml(const char *xml_string);
    ~CStrmSrvXml();

    int SetCurrentObject(const char *a_name, const char *value);
    int GetUnsignedLongValue(const char *a_name, unsigned long *a_value);
    int GetUnsignedLongLongValue(const char *a_name, unsigned long long*a_value);
    int GetIntegerValue(const char *a_name, int *a_value);
    int GetFloatValue(const char *a_name, double *a_value);
    int GetBooleanValue(const char *a_name, int *a_value);
    int GetTextValue(const char *a_name, char *a_value, int a_max_length);

private:
    static bool m_isInitialized;
    DOMDocument* m_doc;
    DOMNode* m_currentObject;
    MemBufInputSource* m_memBufIS;
    XercesDOMParser* m_parser;


    static const XMLCh X_object[];
    static const XMLCh X_property[];
    static const XMLCh X_name[];

    char * GetTextValue(const char *a_name);

};

const XMLCh CStrmSrvXml::X_object[] = { chLatin_o, chLatin_b, chLatin_j, 
                                        chLatin_e, chLatin_c, chLatin_t, 
                                        chNull };

const XMLCh CStrmSrvXml::X_property[] = { chLatin_p, chLatin_r, chLatin_o, 
                                          chLatin_p, chLatin_e, chLatin_r, 
                                          chLatin_t, chLatin_y, chNull };

const XMLCh CStrmSrvXml::X_name[] = { chLatin_n, chLatin_a, chLatin_m, 
                                      chLatin_e, chNull };

bool
CStrmSrvXml::m_isInitialized = false;

CStrmSrvXml::CStrmSrvXml(const char *xml_string)
{
    
    m_doc = NULL;
    m_currentObject = NULL;
    char *bufid = (char *) "fake";

    /* Init Xerces library */
    if (m_isInitialized == false) {
        XMLPlatformUtils::Initialize();
        m_isInitialized = true;
    }

    m_parser = new XercesDOMParser();
    m_parser->setValidationScheme(XercesDOMParser::Val_Never);
    m_parser->setDoNamespaces(false);
    m_parser->setDoSchema(false);
    m_parser->setLoadExternalDTD(false);
    
    m_memBufIS = 
        new MemBufInputSource((const XMLByte*)xml_string,
                              strlen(xml_string), bufid, false);
    
    
    m_parser->parse(*m_memBufIS);
    m_doc = m_parser->getDocument();


}

CStrmSrvXml::~CStrmSrvXml()
{

    delete m_parser;
    delete m_memBufIS;

    //XMLPlatformUtils::Terminate();

}


// Find the first object that matches the given name/value pair and
// cache it as the current object for future queries
int CStrmSrvXml::SetCurrentObject(const char *a_name, const char *a_value)
{
    m_currentObject = NULL;

    if (m_doc != NULL) {

        DOMNodeList* nodes = m_doc->getElementsByTagName(X_object);
        for (unsigned int i = 0; i < nodes->getLength(); i++) {

            DOMNodeList* propnodes = nodes->item(i)->getChildNodes();


            for (unsigned int j = 0; j < propnodes->getLength(); j++) {
                DOMNode * propnode = propnodes->item(j);
                if (propnode->getNodeType() == DOMNode::ELEMENT_NODE) {
                    
                    // Assuming all element names under this node are "property"
                    DOMElement * element = (DOMElement *) propnode;

                    /* Match on name */
                    char *namevalue = XTC(element->getAttribute(X_name));
                    if (strcmp(a_name, namevalue) != 0) {
                        XFree(namevalue);
                        continue;
                    }
                    XFree(namevalue);
                    
                    
                    /* Match on  value */
                    if ((element->getFirstChild() != NULL) &&
                        (element->getFirstChild()->getNodeType() == 
                         DOMNode::TEXT_NODE)) {
                        char *value = XTC(element->getFirstChild()->getNodeValue());
                        if (strcmp(value, a_value) == 0) {
                            m_currentObject = nodes->item(i);
                            XFree(value);
                            break;
                        }
                        XFree(value);
                    }
                    
                }
            }
        
            // If we have found our object, break out of outer loop
            if (m_currentObject != NULL)
                break;
        }
    }

    if (m_currentObject == NULL)
        return -1;

    return 0;
}

char * 
CStrmSrvXml::GetTextValue(const char *a_name)
{
    char *value = NULL;

    if (m_currentObject == NULL)
        return NULL;


    DOMNodeList* propnodes = m_currentObject->getChildNodes();

    for (unsigned int j = 0; j < propnodes->getLength(); j++) {
        DOMNode * propnode = propnodes->item(j);
        if (propnode->getNodeType() == DOMNode::ELEMENT_NODE) {

            // Assuming all element names under this node are "property"
            DOMElement * element = (DOMElement *) propnode;
                
            /* Match on name */
            char *namevalue = XTC(element->getAttribute(X_name));
            if (strcmp(a_name, namevalue) != 0) {
                XFree(namevalue);
                continue;
            }
            XFree(namevalue);

            
            /* Get value */
            if ((element->getFirstChild() != NULL) &&
                (element->getFirstChild()->getNodeType() == 
                 DOMNode::TEXT_NODE)) {
                value = XTC(element->getFirstChild()->getNodeValue());
            }
            break;
            
        }
    }
    
    return value;
}

int
CStrmSrvXml::GetUnsignedLongValue(const char *a_name, unsigned long *a_value) {

    char *value = GetTextValue(a_name);
    if (value == NULL)
        return -1;

    *a_value = strtoul(value, NULL, 10);

    XFree(value);
    return 0;
}

int
CStrmSrvXml::GetUnsignedLongLongValue(const char *a_name, unsigned long long *a_value) {

    char *value = GetTextValue(a_name);
    if (value == NULL)
        return -1;

    *a_value = strtoull(value, NULL, 10);

    XFree(value);
    return 0;
}

int
CStrmSrvXml::GetIntegerValue(const char *a_name, int *a_value) {

    char *value = GetTextValue(a_name);
    if (value == NULL)
        return -1;

    *a_value = atoi(value);

    XFree(value);
    return 0;
}

int
CStrmSrvXml::GetFloatValue(const char *a_name, double *a_value) {

    char *value = GetTextValue(a_name);
    if (value == NULL)
        return -1;

    *a_value = strtod(value, NULL);

    XFree(value);
    return 0;
}

int 
CStrmSrvXml::GetTextValue(const char *a_name, 
                          char *a_value, 
                          int a_max_length)
{

    char *value = GetTextValue(a_name);
    if (value == NULL)
        return -1;

    strncpy(a_value, value, a_max_length);

    XFree(value);
    return 0;

}

int 
CStrmSrvXml::GetBooleanValue(const char *a_name, 
                             int *a_value)
{

    char *value = GetTextValue(a_name);
    if (value == NULL)
        return -1;

    if (strcmp(value, "True") == 0)
        *a_value = 1;
    else
        *a_value = 0;

    XFree(value);
    return 0;

}


/* Wrapper API for accessing CStrmSrvXml facility from C code. */
static CStrmSrvXml *ss_xml = NULL;

extern "C" {


    int strmsrv_xml_create(const char *xml_string)
    {
        if (ss_xml != NULL)
            return -1;
        ss_xml = new CStrmSrvXml(xml_string);
        if (ss_xml == NULL)
            return -1;
        return 0;
    }

    int strmsrv_xml_select_object_by_id(int id) 
    {
        if (ss_xml == NULL)
            return -1;
        char idstr[32];
        sprintf(idstr, "%d", id);
        return ss_xml->SetCurrentObject("Id", idstr);
    }

    int strmsrv_xml_select_object_by_port(int ioport) 
    {
        if (ss_xml == NULL)
            return -1;
        char ioportstr[32];
        sprintf(ioportstr, "%d", ioport);
        return ss_xml->SetCurrentObject("FPGAPort", ioportstr);
    }

    int strmsrv_xml_select_rev_audio_object_by_port(int ioport)
    {
        if (ss_xml == NULL)
            return -1;
        char ioportstr[32];
        sprintf(ioportstr, "%d", ioport);
        return ss_xml->SetCurrentObject("FPGAAudioPort", ioportstr);
    }

    int strmsrv_xml_select_object_by_queueid(int queueid) {
        if (ss_xml == NULL)
            return -1;
        char queueidstr[32];
        sprintf(queueidstr, "%d", queueid);
        return ss_xml->SetCurrentObject("QueueId", queueidstr);
    }

    int strmsrv_xml_get_integer_parameter(const char * a_name, int *a_value) 
    {
        if (ss_xml == NULL)
            return -1;
        return ss_xml->GetIntegerValue(a_name, a_value);
    }

    int strmsrv_xml_get_unsigned_long_parameter(const char * a_name, 
                                                unsigned long *a_value) 
    {
        if (ss_xml == NULL)
            return -1;
        return ss_xml->GetUnsignedLongValue(a_name, a_value);
    }

    int strmsrv_xml_get_unsigned_long_long_parameter(const char * a_name,
                                                unsigned long long *a_value)
    {
        if (ss_xml == NULL)
            return -1;
        return ss_xml->GetUnsignedLongLongValue(a_name, a_value);
    }

    int strmsrv_xml_get_float_parameter(const char * a_name, double *a_value) 
    {
        if (ss_xml == NULL)
            return -1;
        return ss_xml->GetFloatValue(a_name, a_value);
    }
    
    int strmsrv_xml_get_text_parameter(const char * a_name, 
                                       char *a_value, 
                                       int a_max_length) 
    {
        if (ss_xml == NULL) 
            return -1;
        return ss_xml->GetTextValue(a_name, a_value, a_max_length);
    }

    int strmsrv_xml_get_boolean_parameter(const char * a_name, 
                                          int *a_value)
    {
        if (ss_xml == NULL)
            return -1;
        return ss_xml->GetBooleanValue(a_name, a_value);
    }

    void strmsrv_xml_destroy()
    {
        if (ss_xml == NULL)
            return;
        delete ss_xml;
        ss_xml = NULL;
    }
}
