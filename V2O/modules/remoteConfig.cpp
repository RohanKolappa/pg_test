#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <errno.h>

#include "stdio.h"
#include "remoteConfig.h"
#include "xercesutils.h"

//#define DPRINTF printf
#define DPRINTF(...) do {} while (0)

#define DEST_TYPE_NONE                  0
#define DEST_TYPE_FILE                  1
#define DEST_TYPE_TCPSERVER             2
#define DEST_TYPE_PIPE                  3
#define DEST_TYPE_PRINT                 4

// RemoteConfig class definition
class RemoteConfig {
public:
    RemoteConfig();
    RemoteConfig(char *a_event_source, char *a_event_type);
    RemoteConfig(char *buffer, int length);
    ~RemoteConfig();

    void GetConfigParams(char * a_params_file);
    void SetConfigName(const char *a_value);
    void SetCommandName(const char *a_value);
    void SetStatus(int a_status);
    void SetOperation(int operation);
    void SetSource(const char *a_value);
    void SetClientData(const char *a_value);
    int AddParameter(const char *a_name, const char *a_value);
    void AddCommandElement(const char *a_element_name, const char *a_value);
    void AddHeaderElement(const char *a_element_name, const char *a_value);
    void AddDataElement(const char *a_element_name, const char *a_value);
    void Send();
    void SendExternal();

private:
    DOMDocument* m_doc;
    DOMElement* m_CommandElement;
    DOMElement* m_ParametersElement;
    DOMElement* m_HeaderElement;
    DOMElement* m_DataElement;
    bool m_bConfigNameSet;
    bool m_bCommandNameSet;
    bool m_bStatusSet;
    bool m_bOperationSet;
    bool m_bSourceSet;
    bool m_bClientDataSet;
    int m_nSendFormat;
    char *m_sOutputFile;
    int m_nOutputFileMode;
    char *m_sOutputPipe;
    char *m_sOutputIPAddress;
    int m_sOutputPort;
    int m_nOutputPrint;
    bool m_bIsNotification;
    bool m_bIsExternal;
    char * m_externalBuffer;
    int m_externalBufferLen;

    char* GetTextValue(DOMElement *a_element, const char *a_tagname);
    void Output(char *buffer, int len);

};

// RemoteConfig class implementation
RemoteConfig::RemoteConfig()
{

    /* Init object variables */
    m_bCommandNameSet = false;
    m_bConfigNameSet = false;
    m_bStatusSet = false;
    m_bOperationSet = false;
    m_bSourceSet = false;
    m_bClientDataSet = false;
    m_bIsNotification = false;
    m_bIsExternal = false;
    m_externalBuffer = NULL;

    /* Init Xerces library */
    XMLPlatformUtils::Initialize();

    /* Create basic doc structure */
    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(
            X("Core"));

    m_doc = impl->createDocument(0, X("CLIConfig"), 0);

    if (m_doc != NULL) {
        DOMElement* root = m_doc->getDocumentElement();
        DOMElement* element = m_doc->createElement(X("Command"));
        root->appendChild(element);
        m_CommandElement = element;

        element = m_doc->createElement(X("Parameters"));
        root->appendChild(element);
        m_ParametersElement = element;

    }

    GetConfigParams((char *) "/etc/config/remote_config.xml");

}

// This Constructor used for sending Notifications
RemoteConfig::RemoteConfig(char * a_event_source, char *a_event_type)
{

    /* Init object variables */
    m_bIsNotification = true;
    m_bIsExternal = false;
    m_externalBuffer = NULL;

    /* Init Xerces library */
    XMLPlatformUtils::Initialize();

    /* Create basic doc structure */
    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(
            X("Core"));

    m_doc = impl->createDocument(0, X("SystemNotification"), 0);

    if (m_doc != NULL) {
        DOMElement* root = m_doc->getDocumentElement();
        DOMElement* element = m_doc->createElement(X("Header"));
        root->appendChild(element);
        m_HeaderElement = element;

        element = m_doc->createElement(X("Data"));
        root->appendChild(element);
        m_DataElement = element;

    }

    AddHeaderElement("EventSource", a_event_source);
    AddHeaderElement("EventType", a_event_type);

    struct timeval ts;
    gettimeofday(&ts, NULL);
    unsigned long long nCreationTime = ((unsigned long long) ts.tv_sec) * 1000
            + (ts.tv_usec / 1000);

    char buf[64];
    snprintf(buf, 64, "%llu", nCreationTime);
    AddHeaderElement("WallClock", buf);

    GetConfigParams((char *) "/etc/config/remote_notification.xml");

}

// This Constructor used for sending "Status" Notifications
RemoteConfig::RemoteConfig(char *buffer, int length)
{

    /* Init object variables */
    m_bIsNotification = false;
    m_bIsExternal = true;

    /* The document to send comes from outside */
    m_externalBuffer = new char[length];
    memcpy(m_externalBuffer, buffer, length);
    m_externalBufferLen = length;
    m_doc = NULL;

    /* Init Xerces library */
    XMLPlatformUtils::Initialize();

    GetConfigParams((char *) "/etc/config/remote_notification.xml");
}

RemoteConfig::~RemoteConfig()
{

    Free(m_sOutputIPAddress);
    Free(m_sOutputFile);
    Free(m_sOutputPipe);

    if (m_doc != NULL)
        m_doc->release();
    if (m_externalBuffer != NULL)
        delete [] m_externalBuffer;

    XMLPlatformUtils::Terminate();
}

void RemoteConfig::GetConfigParams(char * a_params_file)
{

    m_nSendFormat = DEST_TYPE_TCPSERVER;
    m_sOutputIPAddress = strdup("127.0.0.1");
    m_sOutputPort = 7007;
    m_sOutputFile = strdup("/tmp/configfile");
    m_nOutputFileMode = 0;
    m_sOutputPipe = strdup("/tmp/configpipe");
    m_nOutputPrint = 1; /* stdout */

    /* Read Config file to decide how to send data */
    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Never);
    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setLoadExternalDTD(false);

    char* xmlFile = a_params_file;

    parser->parse(xmlFile);
    DOMDocument* doc = parser->getDocument();
    if (doc != NULL) {
        DOMElement* root = doc->getDocumentElement();
        char * use = GetTextValue(root, "Use");
        if (use != NULL) {
            char * value;
            if (strcmp(use, "TCP") == 0) {
                m_nSendFormat = DEST_TYPE_TCPSERVER;
                DOMNodeList* nodes = doc->getElementsByTagName(X("TCP"));
                if (nodes->getLength() == 1) {
                    value
                            = GetTextValue((DOMElement*) nodes->item(0),
                                    "Server");
                    if (value != NULL) {
                        Free(m_sOutputIPAddress);
                        m_sOutputIPAddress = strdup(value);
                        XFree(value);
                        DPRINTF("Server = %s\n", m_sOutputIPAddress);
                    }
                    value = GetTextValue((DOMElement*) nodes->item(0), "Port");
                    if (value != NULL) {
                        m_sOutputPort = atoi(value);
                        XFree(value);
                        DPRINTF("Port = %d\n", m_sOutputPort);
                    }
                }
            }
            else if (strcmp(use, "File") == 0) {
                m_nSendFormat = DEST_TYPE_FILE;
                DOMNodeList* nodes = doc->getElementsByTagName(X("File"));
                if (nodes->getLength() == 1) {
                    value = GetTextValue((DOMElement*) nodes->item(0), "Name");
                    if (value != NULL) {
                        Free(m_sOutputFile);
                        m_sOutputFile = strdup(value);
                        XFree(value);
                        DPRINTF("File = %s\n", m_sOutputFile);
                    }
                    value = GetTextValue((DOMElement*) nodes->item(0), "Mode");
                    if (value != NULL) {
                        if (strcasecmp(value, "append") == 0)
                            m_nOutputFileMode = 1;
                        XFree(value);
                    }
                }
            }
            else if (strcmp(use, "Pipe") == 0) {
                m_nSendFormat = DEST_TYPE_PIPE;
                value = GetTextValue(root, "Pipe");
                if (value != NULL) {
                    Free(m_sOutputPipe);
                    m_sOutputPipe = strdup(value);
                    XFree(value);
                    DPRINTF("Pipe = %s\n", m_sOutputPipe);
                }
            }
            else if (strcmp(use, "Print") == 0) {
                m_nSendFormat = DEST_TYPE_PRINT;
                value = GetTextValue(root, "Print");
                if (value != NULL) {
                    if (strcasecmp(value, "stdout") == 0)
                        m_nOutputPrint = 1;
                    else if (strcasecmp(value, "stderr") == 0)
                        m_nOutputPrint = 2;
                    XFree(value);
                }
            }

            XFree(use);
        }
    }
    delete parser;

}

char*
RemoteConfig::GetTextValue(DOMElement *a_element, const char *a_tagname)
{
    char* value = NULL;
    DOMNodeList* nodes = a_element->getElementsByTagName(X(a_tagname));
    if (nodes->getLength() == 1) {
        DOMNode* element = nodes->item(0)->getFirstChild();
        if (element->getNodeType() == DOMNode::TEXT_NODE) {
            const XMLCh* str = element->getNodeValue();
            value = XTC(str);
            //XMLString::release(&str);
        }
    }
    return value;
}

void RemoteConfig::AddCommandElement(const char *a_element_name,
        const char *a_value)
{
    if (m_doc == NULL)
        return;

    DOMElement* element = m_doc->createElement(X(a_element_name));
    m_CommandElement->appendChild(element);
    DOMText* txtNode = m_doc->createTextNode(X(a_value));
    element->appendChild(txtNode);
}

void RemoteConfig::AddHeaderElement(const char *a_element_name,
        const char *a_value)
{
    if (m_doc == NULL)
        return;

    DOMElement* element = m_doc->createElement(X(a_element_name));
    m_HeaderElement->appendChild(element);
    DOMText* txtNode = m_doc->createTextNode(X(a_value));
    element->appendChild(txtNode);
}

void RemoteConfig::AddDataElement(const char *a_element_name,
        const char *a_value)
{
    if (m_doc == NULL)
        return;

    DOMElement* element = m_doc->createElement(X(a_element_name));
    m_DataElement->appendChild(element);
    DOMText* txtNode = m_doc->createTextNode(X(a_value));
    element->appendChild(txtNode);
}

void RemoteConfig::SetConfigName(const char *a_value)
{
    if (m_doc == NULL)
        return;
    m_bConfigNameSet = true;

    AddCommandElement("Config", a_value);
}

void RemoteConfig::SetCommandName(const char *a_value)
{
    if (m_doc == NULL)
        return;
    m_bCommandNameSet = true;
    AddCommandElement("CLICommand", a_value);
}

void RemoteConfig::SetStatus(int a_status)
{
    if (m_doc == NULL)
        return;
    m_bStatusSet = true;

    if (a_status == REMOTE_CONFIG_STATUS_PASS)
        AddCommandElement("Status", "Success");
    else
        AddCommandElement("Status", "Failure");

}

void RemoteConfig::SetSource(const char *a_value)
{
    if (m_doc == NULL)
        return;
    m_bSourceSet = true;
    AddCommandElement("Source", a_value);
}

void RemoteConfig::SetClientData(const char *a_value)
{
    if (m_doc == NULL)
        return;
    m_bClientDataSet = true;
    AddCommandElement("ClientData", a_value);
}

void RemoteConfig::SetOperation(int operation)
{
    if (m_doc == NULL)
        return;
    m_bOperationSet = true;
    switch (operation) {
    case REMOTE_CONFIG_OPERATION_MODIFY:
        AddCommandElement("Operation", "Modify");
        break;
    case REMOTE_CONFIG_OPERATION_ADD:
        AddCommandElement("Operation", "Add");
        break;
    case REMOTE_CONFIG_OPERATION_DELETE:
        AddCommandElement("Operation", "Delete");
        break;
    }

}

int RemoteConfig::AddParameter(const char *a_name, const char *a_value)
{
    if (m_doc == NULL)
        return -1;

    /* Add a new Paremeter element */
    DOMElement* parameter_element = m_doc->createElement(X("Parameter"));
    m_ParametersElement->appendChild(parameter_element);

    /* Add the Parameter name/value pair */
    DOMElement* name_element = m_doc->createElement(X("Name"));
    parameter_element->appendChild(name_element);
    DOMText* nameText = m_doc->createTextNode(X(a_name));
    name_element->appendChild(nameText);

    DOMElement* value_element = m_doc->createElement(X("Value"));
    parameter_element->appendChild(value_element);
    DOMText* valueText = m_doc->createTextNode(X(a_value));
    value_element->appendChild(valueText);

    return 0;

}

void RemoteConfig::Send()
{

    /* Make sure that the document is complete */
    if (m_bIsNotification == false) {
        if (!m_bConfigNameSet)
            SetConfigName("Unknown");
        if (!m_bCommandNameSet)
            SetCommandName("Unknown");
        if (!m_bOperationSet)
            SetOperation(REMOTE_CONFIG_OPERATION_MODIFY);
        if (!m_bSourceSet)
            SetSource("Unknown");
        if (!m_bClientDataSet)
            SetClientData("");
        if (!m_bStatusSet)
            SetStatus(REMOTE_CONFIG_STATUS_PASS);
    }

    /* Create XML from document */
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(
            X("Core"));

    DOMWriter *writer = impl->createDOMWriter();

    if (writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    MemBufFormatTarget *memBufTarget = new MemBufFormatTarget();

    writer->writeNode(memBufTarget, *m_doc);

    char * buffer = (char *) memBufTarget->getRawBuffer();
    int len = memBufTarget->getLen();

    /* We have the document as an XML string. Send to the desired
     * destination */
    Output(buffer, len);

    delete memBufTarget;
    writer->release();

}

void RemoteConfig::SendExternal()
{
    if (m_bIsExternal == false)
        return;
    Output(m_externalBuffer, m_externalBufferLen);
}

void RemoteConfig::Output(char *buffer, int len)
{
    /* We have the document as an XML string. Send to the desired
     * destination */
    if (m_nSendFormat == DEST_TYPE_FILE) {
        FILE *fp;
        if (m_nOutputFileMode == 1)
            fp = fopen(m_sOutputFile, "a");
        else
            fp = fopen(m_sOutputFile, "w");
        if (fp != NULL) {
            fwrite(buffer, len, 1, fp);
            fclose(fp);
        }
    }
    else if (m_nSendFormat == DEST_TYPE_TCPSERVER) {

        int s;
        bool connected = false;
        /* Send to TCP server as fast as possible. We do non-blocking
           since we do not want to wait too long if server is not available */
        do {
            /* Create socket */
            s = socket(AF_INET, SOCK_STREAM, 0);
            if (s <= 0)
                break;

            /* Make socket non blocking */
            int rc = fcntl(s, F_SETFL, O_NONBLOCK);
            if (rc == -1) {
                break;
            }

            /* Create destination address */
            struct sockaddr_in dest;
            memset(&dest, 0x0, sizeof(dest));
            dest.sin_family = AF_INET;
            dest.sin_addr.s_addr = inet_addr(m_sOutputIPAddress);
            dest.sin_port = ntohs(m_sOutputPort);

            /* Connect. Non-blocking so could return an error */
            rc = connect(s, (struct sockaddr *) &dest, sizeof(dest));
            if (rc == -1) {
                if (errno != EINPROGRESS)
                    break;
            }
            /* Select on socket. While loop waits till connected, and
             * then sends data */
            int send_count = 0;
            while (1) {
                fd_set write_fdset;
                FD_ZERO(&write_fdset);
                FD_SET(s, &write_fdset);
                struct timeval timeout;
                timeout.tv_sec = 0;
                timeout.tv_usec = 100000; /* 100 msec limit on all activity */

                rc = select(s + 1, NULL, &write_fdset, NULL, &timeout);
                if (rc <= 0) {
                    /* select error or timeout */
                    DPRINTF("Timed out.\n");
                    break;
                }
                /* Socket is ready */
                if (!connected) {
                    /* Not yet connected */
                    int sockoptval = 0;
                    int sockoptlen = sizeof(sockoptval);
                    rc = getsockopt(s, SOL_SOCKET, SO_ERROR, &sockoptval,
                            (socklen_t *) &sockoptlen);
                    if (rc == -1 || sockoptval != 0) {
                        /* Connect failed */
                        DPRINTF("Connect failed\n");
                        break;
                    }
                    connected = true;
                    DPRINTF("Connected\n");
                }
                else {
                    /* We are connected */
                    rc = write(s, buffer + send_count, len);
                    if (rc < 0) {
                        DPRINTF("Send error\n");
                        break;
                    }
                    DPRINTF("Sent %d\n", rc);
                    send_count += rc;
                    len -= rc;
                    if (len <= 0) {
                        DPRINTF("Done %d\n", send_count);
                        break;
                    }
                }
            }

        } while (0);

        /* We are done so close socket */
        if (s > 0) {
            shutdown(s, SHUT_RDWR);
            close(s);
        }

    }
    else if (m_nSendFormat == DEST_TYPE_PIPE) {
        int fd = open(m_sOutputPipe, O_WRONLY | O_NONBLOCK);
        if (fd > 0) {
            write(fd, buffer, len);
            close(fd);
        }
    }
    else if (m_nSendFormat == DEST_TYPE_PRINT) {
        if (m_nOutputPrint == 1)
            fprintf(stdout, buffer);
        else
            fprintf(stderr, buffer);
    }
}

static RemoteConfig *remoteConfig = NULL;
static RemoteConfig *remoteNotification = NULL;

/* Wrapper API for accessing remoteConfig facility from C code. */

/* Clients need to call remote_config_start(), followed by any number
 of remote_config_xxx commands so set header/parameters. Finally
 client needs to call remote_config_send() or
 remote_config_send_error() to send the data out. Any other sequence
 will be silenty ignored. */

extern "C" {

int remote_config_start()
{
    if (remoteConfig != NULL)
        return -1;

    remoteConfig = new RemoteConfig();
    return 0;
}

int remote_config_start_dummy()
{
    if (remoteConfig != NULL)
        return -1;
    // Dummy version does create RemoteConfig() object
    // All other commands thus return silently
    return 0;
}

extern void remote_config_set_config_name(const char *name)
{
    if (remoteConfig == NULL)
        return;
    remoteConfig->SetConfigName(name);
}

extern void remote_config_set_command_name(const char *name)
{
    if (remoteConfig == NULL)
        return;
    remoteConfig->SetCommandName(name);
}

extern void remote_config_set_status(int status)
{
    if (remoteConfig == NULL)
        return;
    remoteConfig->SetStatus(status);
}

extern void remote_config_set_operation(int operation)
{
    if (remoteConfig == NULL)
        return;
    remoteConfig->SetOperation(operation);
}

extern void remote_config_set_index(int index)
{
    if (remoteConfig == NULL)
        return;

    char buffer[32];
    sprintf(buffer, "%d", index);
    remoteConfig->AddCommandElement("Index", buffer);
}

extern void remote_config_set_board_number(int boardNumber)
{
    if (remoteConfig == NULL)
        return;

    char buffer[32];
    sprintf(buffer, "%d", boardNumber);
    remoteConfig->AddCommandElement("BoardNumber", buffer);
}

extern void remote_config_set_channel_number(int channelNumber)
{
    if (remoteConfig == NULL)
        return;

    char buffer[32];
    sprintf(buffer, "%d", channelNumber);
    remoteConfig->AddCommandElement("ChannelNumber", buffer);
}

extern void remote_config_set_source(int source)
{
    if (remoteConfig == NULL)
        return;
    switch (source) {
    case REMOTE_CONFIG_SOURCE_CLI:
        remoteConfig->SetSource("CLI");
        break;
    case REMOTE_CONFIG_SOURCE_MENU:
        remoteConfig->SetSource("Menu");
        break;
    default:
        remoteConfig->SetSource("Unknown");
        break;
    }
}

extern void remote_config_set_client_data(const char *value)
{
    if (remoteConfig == NULL)
        return;
    remoteConfig->SetClientData(value);
}

int remote_config_add_parameter(const char *parameter, const char *value)
{
    if (remoteConfig == NULL)
        return -1;

    return remoteConfig->AddParameter(parameter, value);

}

int remote_config_add_bool_parameter(const char *parameter, int value)
{
    if (remoteConfig == NULL)
        return -1;

    if (value)
        return remoteConfig->AddParameter(parameter, "Yes");
    else
        return remoteConfig->AddParameter(parameter, "No");

}

int remote_config_add_int_parameter(const char *parameter, int value)
{
    if (remoteConfig == NULL)
        return -1;

    char buffer[32];
    sprintf(buffer, "%d", value);
    return remoteConfig->AddParameter(parameter, buffer);
}

int remote_config_add_float_parameter(const char *parameter, float value)
{
    if (remoteConfig == NULL)
        return -1;

    char buffer[32];
    sprintf(buffer, "%f", value);
    return remoteConfig->AddParameter(parameter, buffer);
}

void remote_config_send()
{

    if (remoteConfig == NULL)
        return;

    remoteConfig->Send();

    delete remoteConfig;
    remoteConfig = NULL;

}

void remote_config_send_error(const char *reason)
{

    if (remoteConfig == NULL)
        return;

    remoteConfig->SetStatus(REMOTE_CONFIG_STATUS_FAIL);
    remoteConfig->AddCommandElement("FailureReason", reason);
    remoteConfig->Send();

    delete remoteConfig;
    remoteConfig = NULL;

}

// Functions to be used for sending Notifications
int remote_notification_start(char * a_event_source, char *a_event_type)
{
    if (remoteNotification != NULL)
        return -1;

    remoteNotification = new RemoteConfig(a_event_source, a_event_type);
    return 0;
}

void remote_notification_add_data(char *a_name, char *a_value)
{
    if (remoteNotification == NULL)
        return;
    remoteNotification->AddDataElement(a_name, a_value);

}

void remote_notification_send()
{
    if (remoteNotification == NULL)
        return;

    remoteNotification->Send();

    delete remoteNotification;
    remoteNotification = NULL;
}

int remote_external_buffer_send(char *buffer, int len)
{
    if (remoteNotification != NULL)
        return -1;

    remoteNotification = new RemoteConfig(buffer, len);
    remoteNotification->SendExternal();

    delete remoteNotification;
    remoteNotification = NULL;
    return 0;
}

}
