
#include "strmsrv_compatibility.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
XERCES_CPP_NAMESPACE_USE

#include "streamer.hh"
#include "notify.hh"

using namespace std;

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
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
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
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


#define X(str) XStr(str).unicodeForm()

CNotify::CNotify(int a_nEvent,
                 int a_nId,
                 int a_nPortType,
                 int a_nQueueId,
                 int a_nStatus)
{
    m_HeaderElement = NULL;
    m_DataElement = NULL;

    m_nCreationTime = CTimeKeeper::Now();

    DOMImplementation* impl =
        DOMImplementationRegistry::getDOMImplementation(X("Core"));
    m_doc = impl->createDocument(0, X("SSNotification"), 0);
    if (m_doc != NULL) {
        DOMElement* root = m_doc->getDocumentElement();
        DOMElement* element = m_doc->createElement(X("Header"));
        root->appendChild(element);
        m_HeaderElement = element;

        element = m_doc->createElement(X("Data"));
        root->appendChild(element);
        m_DataElement = element;
    }

    char buf[64];

    AddHeaderElement("EventType", GetEventString(a_nEvent));

    snprintf(buf, 32, "%d", a_nId);
    AddHeaderElement("Id", buf);

    AddHeaderElement("Type", CStreamer::GetTypeString(a_nPortType));

    snprintf(buf, 32, "%d", a_nQueueId);
    AddDataElement("QueueId", buf);

    snprintf(buf, 32, "%d", a_nStatus);
    AddHeaderElement("Status", buf);

    string status;
    AddDataElement("Status", CStreamer::GetStateString(a_nStatus, status));

    snprintf(buf, 64, "%"PRIu64"", m_nCreationTime);
    AddHeaderElement("WallClock", buf);


    m_bMessageCreated = false;

}


CNotify::CNotify(const char *a_message)
{
    m_HeaderElement = NULL;
    m_DataElement = NULL;
    m_message.append(a_message);
    m_message.append("\r\n");
    m_nCreationTime = CTimeKeeper::Now();
    m_doc = NULL;
    m_bMessageCreated = false;
}

CNotify::~CNotify()
{
    if (m_doc != NULL)
        m_doc->release();
}


void CNotify::CreateXML() {

    if ((m_doc != NULL) &&
        (m_bMessageCreated == false)) {
        // Create the output XML
        DOMImplementation* impl =
            DOMImplementationRegistry::getDOMImplementation(X("Core"));
        DOMWriter *writer = impl->createDOMWriter();
        if (writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
            writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

        MemBufFormatTarget *memBufTarget = new MemBufFormatTarget();

        writer->writeNode(memBufTarget, *m_doc->getDocumentElement());
        m_message.append((char *)memBufTarget->getRawBuffer());
        delete memBufTarget;
        writer->release();
        m_bMessageCreated = true;
    }
}

void
CNotify::AddHeaderElement(const char *a_element_name,
                          const char *a_value) {
    if (m_doc == NULL)
        return;

    DOMElement* element = m_doc->createElement(X(a_element_name));
    m_HeaderElement->appendChild(element);
    DOMText* txtNode = m_doc->createTextNode(X(a_value));
    element->appendChild(txtNode);
}

void
CNotify::AddDataElement(const char *a_element_name,
                        const char *a_value) {
    if (m_doc == NULL)
        return;

    DOMElement* element = m_doc->createElement(X(a_element_name));
    m_DataElement->appendChild(element);
    DOMText* txtNode = m_doc->createTextNode(X(a_value));
    element->appendChild(txtNode);
}

const char *
CNotify::GetEventString(int a_nEventType) {

    switch (a_nEventType) {
    case NOTIFICATION_EVENT_NOP:
        return "None";
    case NOTIFICATION_EVENT_ERROR:
        return "Error";
    case NOTIFICATION_EVENT_CONNECTED:
        return "Connected";
    case NOTIFICATION_EVENT_DISCONNECTED:
        return "Disconnected";
    case NOTIFICATION_EVENT_BEGIN:
        return "Started";
    case NOTIFICATION_EVENT_END:
        return "Stopped";
    case NOTIFICATION_EVENT_ENDOFMEDIA:
        return "EndOfMedia";
    case NOTIFICATION_EVENT_PROGRESS:
        return "Progress";
    default:
        return "Unknown";
    }
}

void
CNotify::SetNotificationData(const char *a_name, unsigned long long a_value)
{
    char buf[64];
    snprintf(buf, 64, "%"PRIu64"", a_value);
    AddDataElement(a_name, buf);
}

void
CNotify::SetNotificationData(const char *a_name, const char *a_value)
{
    AddDataElement(a_name, a_value);
}

void
CNotify::CreateErrorNotification(int a_nErrorCode, string& a_pErrorString)
{
    char buf[64];
    sprintf(buf, "%d", a_nErrorCode);
    AddHeaderElement("ErrorCode", buf);

    switch (a_nErrorCode) {
    case NOTIFICATION_ERROR_FILE_OPEN:
        strcpy(buf, "FileOpen");
        break;
    case NOTIFICATION_ERROR_FILE_READ:
        strcpy(buf, "FileRead");
        break;
    case NOTIFICATION_ERROR_FILE_WRITE:
        strcpy(buf, "FileWrite");
        break;
    case NOTIFICATION_ERROR_OUT_OF_MEMORY:
        strcpy(buf, "OutOfMemory");
        break;
    case NOTIFICATION_ERROR_IOPORT:
        strcpy(buf, "IOPort");
        break;
    default:
        strcpy(buf, "Unknown");
        break;
    }

    AddHeaderElement("ErrorString", buf);


    AddDataElement("ErrorDetail", a_pErrorString.c_str());
}


