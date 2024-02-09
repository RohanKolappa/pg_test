
#include <stdio.h>
#include <sys/time.h>

#include "remoteStatus.h"
#include "remoteConfig.h"
#include "xercesutils.h"




class RemoteStatus {
public:
    RemoteStatus(bool isList);
    ~RemoteStatus();

    void SetStatusName(const char *a_value);
    void SetCommandName(const char *a_value);
    void SetStatus(int a_status);
    void SetSource(const char *a_value);
    int AddParameter(const char *a_name, 
                     const char *a_value,
                     const char *a_type,
                     const char *a_units,
                     const char *a_source);
    void AddCommandElement(const char *a_element_name, 
                           const char *a_value);
    void NewParameters(void);
    void TranslateParameters(const char *a_xml_string);


    void Print(bool bNotify = false);

private:
    static bool m_isInitialized;
    bool m_bStatusNameSet;
    bool m_bCommandNameSet;
    bool m_bStatusSet;
    bool m_bIsList;
    char m_timestampString[24];
    DOMDocument* m_doc;
    DOMElement* m_CommandElement;
    DOMElement* m_ParametersElement;
    DOMElement* m_ParametersListElement;
};

bool
RemoteStatus::m_isInitialized = false;

RemoteStatus::RemoteStatus(bool isList = false) {

    /* Init object variables */
    m_bCommandNameSet = false;
    m_bStatusNameSet = false;
    m_bStatusSet = false;
    m_bIsList = isList;

    /* Init Xerces library */
    if (m_isInitialized == false) {
        XMLPlatformUtils::Initialize();
        m_isInitialized = true;
    }

    /* Compute timestamp in milliseconds*/
    struct timeval ts;
    gettimeofday(&ts, NULL);
    unsigned long long timestamp =
            (((unsigned long long) ts.tv_sec) * 1000 + (ts.tv_usec/1000));
    sprintf(m_timestampString, "%llu", timestamp);

    /* Create basic doc structure */
    DOMImplementation* impl =  
        DOMImplementationRegistry::getDOMImplementation(X("Core"));

    m_doc = impl->createDocument(0, X("CLIStatus"), 0);

    if (m_doc != NULL) {
        DOMElement* root = m_doc->getDocumentElement();
        DOMElement* element = m_doc->createElement(X("Command"));
        root->appendChild(element);
        m_CommandElement = element;

        DOMElement* statuselement = m_doc->createElement(X("Status"));
        root->appendChild(statuselement);

        if (!m_bIsList) {
            element = m_doc->createElement(X("Parameters"));
            statuselement->appendChild(element);
            m_ParametersElement = element;
            m_ParametersElement->setAttribute(X("timestamp"), X(m_timestampString));
        }
        else {
            element = m_doc->createElement(X("ParametersList"));
            statuselement->appendChild(element);
            m_ParametersListElement = element;
            m_ParametersListElement->setAttribute(X("timestamp"), X(m_timestampString));
        }
    }

}

RemoteStatus::~RemoteStatus() {


    if (m_doc != NULL)
        m_doc->release();

    // XMLPlatformUtils::Terminate();
}

void
RemoteStatus::NewParameters() {
    if (m_doc == NULL || !m_bIsList)
        return;
    
    /* Create a new Parameters element and set that as the current element */
    DOMElement* element = m_doc->createElement(X("Parameters"));
    m_ParametersListElement->appendChild(element);
    m_ParametersElement = element;
    m_ParametersElement->setAttribute(X("timestamp"), X(m_timestampString));
    
}

void
RemoteStatus::SetStatusName(const char *a_value) {
    if (m_doc == NULL)
        return;
    m_bStatusNameSet = true;

    AddCommandElement("Name", a_value);
}

void
RemoteStatus::SetCommandName(const char *a_value) {
    if (m_doc == NULL)
        return;
    m_bCommandNameSet = true;
    AddCommandElement("CLICommand", a_value);
}

void
RemoteStatus::SetStatus(int a_status) {
    if (m_doc == NULL)
        return;
    m_bStatusSet = true;

    if (a_status == REMOTE_STATUS_PASS)
        AddCommandElement("Status", "Success");
    else
        AddCommandElement("Status", "Failure");
        
}

void
RemoteStatus::AddCommandElement(const char *a_element_name,
                                const char *a_value) {
    if (m_doc == NULL)
        return;
    
    DOMElement* element = m_doc->createElement(X(a_element_name));
    m_CommandElement->appendChild(element);
    DOMText* txtNode = m_doc->createTextNode(X(a_value));
    element->appendChild(txtNode);
}

int
RemoteStatus::AddParameter(const char *a_name, 
                           const char *a_value,
                           const char *a_type,
                           const char *a_units,
                           const char *a_source)
    {
    if (m_doc == NULL)
        return -1;


    /* Add a new Paremeter element */
    DOMElement* parameter_element = 
        m_doc->createElement(X("Parameter"));
    m_ParametersElement->appendChild(parameter_element);

    parameter_element->setAttribute(X("name"), X(a_name)); 
    parameter_element->setAttribute(X("type"), X(a_type)); 
    if (a_units != NULL)
        parameter_element->setAttribute(X("units"), X(a_units)); 
    if (a_source != NULL)
        parameter_element->setAttribute(X("source"), X(a_source)); 


    DOMText* valueText = m_doc->createTextNode(X(a_value));
    parameter_element->appendChild(valueText);

    return 0;
        
}

void
RemoteStatus::TranslateParameters(const char *a_xml_string) 
{

    char *bufid = (char *) "fake";
    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Never);
    parser->setDoNamespaces( false );
    parser->setDoSchema( false );
    parser->setLoadExternalDTD( false );
    
    MemBufInputSource* memBufIS = new MemBufInputSource(
        (const XMLByte*)a_xml_string,
        strlen(a_xml_string), bufid, false);
    
    
    parser->parse(*memBufIS);
    DOMDocument* doc = parser->getDocument();

    if (doc != NULL) {

        DOMNodeList* nodes = doc->getElementsByTagName(X("object"));
        for (int i = 0; i < (int)nodes->getLength(); i++) {
            NewParameters();
            DOMNodeList* propnodes = nodes->item(i)->getChildNodes();

            for (int j = 0; j < (int)propnodes->getLength(); j++) {
                DOMNode * propnode = propnodes->item(j);
                if (propnode->getNodeType() == DOMNode::ELEMENT_NODE) {
                    DOMElement * element = (DOMElement *) propnode;
                    if (XMLString::equals(element->getTagName(), 
                                          X("property"))) {

                        /* Get name */
                        char *name = XTC(element->getAttribute(X("name")));

                        /* Get value */
                        char *value;
                        if ((element->getFirstChild() != NULL) &&
                            (element->getFirstChild()->getNodeType() == 
                             DOMNode::TEXT_NODE)) {
                            value = XTC(element->getFirstChild()->getNodeValue());
                        } else {
                            value = XTC(X(""));
                        }
                    
                        /* Get type **/
                        char *origtype = XTC(element->getAttribute(X("type")));
                        char *type = (char *) "String";
                        
                        if (origtype != NULL) {
                            if (strcmp(origtype, "integer") == 0) {
                                type = (char *) "Integer";
                            }
                            else if (strcmp(origtype, "unsignedInteger") == 0) {
                                type = (char *) "UnsignedInteger";
                            }
                            else if (strcmp(origtype, "long") == 0) {
                                type = (char *) "Long";
                            }
                            else if (strcmp(origtype, "unsignedLong") == 0) {
                                type = (char *) "UnsignedLong";
                            }
                            else if (strcmp(origtype, "float") == 0) {
                                type = (char *) "Float";
                            }
                            else if (strcmp(origtype, "boolean") == 0) {
                                type = (char *) "Boolean";
                            }
                        }


                        char *units = NULL;
                        char *source = NULL;
                        char *kind = XTC(element->getAttribute(X("kind")));
                        if (kind != NULL) {
                            if (strcmp(kind, "counter") == 0) {
                                units = (char *) "number";
                                source = (char *) "counter";
                            }
                            else if (strcmp(kind, "bytecounter") == 0) {
                                units = (char *) "bytes";
                                source = (char *) "counter";
                            }
                            else if (strcmp(kind, "bitcounter") == 0) {
                                units = (char *) "bits";
                                source = (char *) "counter";
                            }
                            else if (strcmp(kind, "errorcounter") == 0) {
                                units = (char *) "number";
                                source = (char *) "errorcounter";
                            }
                            else if (strcmp(kind, "guage") == 0) {
                                units = (char *) "number";
                                source = (char *) "guage";
                            }
                            else if (strcmp(kind, "bitrateguage") == 0) {
                                units = (char *) "bits";
                                source = (char *) "guage";
                            }
                            else if (strcmp(kind, "byterateguage") == 0) {
                                units = (char *) "bytes";
                                source = (char *) "guage";
                            }
                            else if (strcmp(kind, "constant") == 0) {
                                source = (char *) "constant";
                            }
                        }
                    
                        AddParameter(name, value, type, units, source);

                        XFree(name);
                        XFree(value);
                        XFree(origtype);
                        XFree(kind);
                    }
                }
            }
        }

    }

    delete parser;
    delete memBufIS;
}

void RemoteStatus::Print(bool bNotify)
{
    /* Make sure that the document is complete */
    if (!m_bStatusNameSet)
        SetStatusName("Unknown");
    if (!m_bCommandNameSet)
        SetCommandName("Unknown");
    if (!m_bStatusSet)
        SetStatus(REMOTE_STATUS_PASS);

    /* Create XML from document */
    DOMImplementation *impl = 
        DOMImplementationRegistry::getDOMImplementation(X("Core"));
    
    DOMWriter *writer = impl->createDOMWriter();

    if (writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    
    MemBufFormatTarget *memBufTarget = new MemBufFormatTarget();
    
    writer->writeNode(memBufTarget, *m_doc);
    char * buffer = (char *)memBufTarget->getRawBuffer();


    // Print status to std out
    fprintf(stdout, buffer);

    // In addition, publish status if so requested
    if (bNotify) {
        remote_external_buffer_send(buffer, memBufTarget->getLen());
    }

    delete memBufTarget;
    writer->release();

    return;
}

static RemoteStatus *remoteStatus = NULL;


/* Wrapper API for accessing remoteStatus facility from C code. */

extern "C" {

    int remote_status_start(int isList) {
        if (remoteStatus != NULL)
            return -1;
        
        remoteStatus = new RemoteStatus(isList);
        return 0;
    }
    

    extern void remote_status_set_name(const char *name) {
        if (remoteStatus == NULL)
            return;
        remoteStatus->SetStatusName(name);
    }

    extern void remote_status_set_command_name(const char *name) {
        if (remoteStatus == NULL)
            return;
        remoteStatus->SetCommandName(name);
    }

    extern void remote_status_set_channel_number(int channelNumber)
    {
        if (remoteStatus == NULL)
            return;

        char buffer[32];
        sprintf(buffer, "%d", channelNumber);
        remoteStatus->AddCommandElement("ChannelNumber", buffer);
    }

    extern void remote_status_set_status(int status) {
        if (remoteStatus == NULL)
            return;
        remoteStatus->SetStatus(status);
    }

    extern void remote_status_new_parameters() {
        if (remoteStatus == NULL)
            return;
        remoteStatus->NewParameters();
    }

    int remote_status_add_string_parameter(const char *parameter, 
                                           const char *value,
                                           const char *units,
                                           const char *source) {
        if (remoteStatus == NULL)
            return -1;
        
        return remoteStatus->AddParameter(parameter, value, "String", 
                                          units, source);

    }

    int remote_status_add_integer_parameter(const char *parameter, 
                                            int value,
                                            const char *units,
                                            const char *source) {
        if (remoteStatus == NULL)
            return -1;
        
        char buf[32];
        snprintf(buf, 32, "%ld", (long)value);
        return remoteStatus->AddParameter(parameter, buf, "Integer", 
                                          units, source);

    }

    int remote_status_add_unsigned_integer_parameter(const char *parameter, 
                                                     unsigned long value,
                                                     const char *units,
                                                     const char *source) {
        if (remoteStatus == NULL)
            return -1;
        
        char buf[32];
        snprintf(buf, 32, "%lu", value);
        return remoteStatus->AddParameter(parameter, buf, "UnsignedInteger", 
                                          units, source);

    }

    int remote_status_add_long_parameter(const char *parameter, 
                                         long long value,
                                         const char *units,
                                         const char *source) {
        if (remoteStatus == NULL)
            return -1;
        
        char buf[32];
        snprintf(buf, 32, "%lld", value);
        return remoteStatus->AddParameter(parameter, buf, "Long", 
                                          units, source);

    }

    int remote_status_add_unsigned_long_parameter(const char *parameter, 
                                                  unsigned long long value,
                                                  const char *units,
                                                  const char *source) {
        if (remoteStatus == NULL)
            return -1;
        
        char buf[32];
        snprintf(buf, 32, "%llu", value);
        return remoteStatus->AddParameter(parameter, buf, "UnsignedLong", 
                                          units, source);

    }

    int remote_status_add_float_parameter(const char *parameter, 
                                          double value,
                                          const char *units,
                                          const char *source,
                                          int precision) {
        if (remoteStatus == NULL)
            return -1;
        
        char buf[64];
        snprintf(buf, 64, "%.*f", precision, value);
        return remoteStatus->AddParameter(parameter, buf, "Float", 
                                          units, source);

    }

    int remote_status_add_boolean_parameter(const char *parameter, 
                                            int value,
                                            const char *units,
                                            const char *source) {
        if (remoteStatus == NULL)
            return -1;

        if (value)
            return remoteStatus->AddParameter(parameter, "True", "Boolean", 
                                              units, source);
        else 
            return remoteStatus->AddParameter(parameter, "False", "Boolean", 
                                              units, source);

    }


    int remote_status_translate_parameters(const char *xml_string) {
        if (remoteStatus == NULL)
            return -1;
        
        remoteStatus->TranslateParameters(xml_string);
        return 0;
    }


    void remote_status_print() {

        if (remoteStatus == NULL)
            return;

        remoteStatus->Print(false);

        delete remoteStatus;
        remoteStatus = NULL;

    }

    void remote_status_print_notify() {

        if (remoteStatus == NULL)
            return;

        remoteStatus->Print(true);

        delete remoteStatus;
        remoteStatus = NULL;

    }

    void remote_status_print_error() {

        if (remoteStatus == NULL)
            return;
        
        remoteStatus->SetStatus(REMOTE_STATUS_FAIL);
        remoteStatus->Print();

        delete remoteStatus;
        remoteStatus = NULL;

    }

}
