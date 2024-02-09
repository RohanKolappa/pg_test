#ifndef __H_NOTIFY__
#define __H_NOTIFY__

#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE 

#include "timekeeper.hh"


// Event IDs

// Global events
#define NOTIFICATION_EVENT_NOP               0

// Common Object  events
#define NOTIFICATION_EVENT_ERROR             10
#define NOTIFICATION_EVENT_CONNECTED         11
#define NOTIFICATION_EVENT_DISCONNECTED      12
#define NOTIFICATION_EVENT_BEGIN             13
#define NOTIFICATION_EVENT_END               14
#define NOTIFICATION_EVENT_PROGRESS          15

// Onject Specific events

// Reader
#define NOTIFICATION_EVENT_ENDOFMEDIA        51

// Writers


// Error Codes
#define NOTIFICATION_ERROR_FILE_OPEN       10
#define NOTIFICATION_ERROR_FILE_READ       11
#define NOTIFICATION_ERROR_FILE_WRITE      12
#define NOTIFICATION_ERROR_OUT_OF_MEMORY   13
#define NOTIFICATION_ERROR_IOPORT          14

class CNotify
{
public:
    CNotify(int a_nEvent, int a_nId, int a_nPortType, int a_nQueueId, int a_nStatus);
    CNotify(const char *a_message);
    ~CNotify();
    const char * GetMessage() {CreateXML(); return m_message.c_str();};
    int GetMessageLength() {CreateXML(); return m_message.size();};
    int GetElapsed() {return (int) (CTimeKeeper::Now() - m_nCreationTime); };
    static const char * GetEventString(int a_nEvent);
    void SetNotificationData(const char *a_name, unsigned long long a_value);
    void SetNotificationData(const char *a_name, const char *a_value);
    void CreateErrorNotification(int a_nErrorCode, std::string& a_pErrorString);

private:
    std::string       m_message;
    bool         m_bMessageCreated;
    timestamp_t  m_nCreationTime;
    DOMDocument* m_doc;
    DOMElement*  m_HeaderElement;
    DOMElement*  m_DataElement;

    void AddHeaderElement(const char *a_element_name, 
                          const char *a_value);

    void AddDataElement(const char *a_element_name, 
                        const char *a_value);
    
    void CreateXML();
};


#endif
