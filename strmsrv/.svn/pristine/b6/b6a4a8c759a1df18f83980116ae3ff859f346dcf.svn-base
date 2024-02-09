
#include "strmsrv_compatibility.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

#include "command.hh"
#include "timekeeper.hh"
#include "common.hh"

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


const XMLCh CCommand::X_Core[] = { chLatin_C, chLatin_o, chLatin_r,
                                   chLatin_e, chNull };

const XMLCh CCommand::X_result[] = { chLatin_r, chLatin_e, chLatin_s,
                                     chLatin_u, chLatin_l, chLatin_t, chNull };

const XMLCh CCommand::X_object[] = { chLatin_o, chLatin_b, chLatin_j,
                                     chLatin_e, chLatin_c, chLatin_t,
                                     chNull };

const XMLCh CCommand::X_objects[] = { chLatin_o, chLatin_b, chLatin_j,
                                      chLatin_e, chLatin_c, chLatin_t,
                                      chLatin_s, chNull };

const XMLCh CCommand::X_property[] = { chLatin_p, chLatin_r, chLatin_o,
                                      chLatin_p, chLatin_e, chLatin_r,
                                      chLatin_t, chLatin_y, chNull };

const XMLCh CCommand::X_name[] = { chLatin_n, chLatin_a, chLatin_m,
                                   chLatin_e, chNull };

const XMLCh CCommand::X_type[] = { chLatin_t, chLatin_y, chLatin_p,
                                   chLatin_e, chNull };

const XMLCh CCommand::X_kind[] = { chLatin_k, chLatin_i, chLatin_n,
                                   chLatin_d, chNull };

const XMLCh CCommand::X_constant[] = { chLatin_c, chLatin_o, chLatin_n,
                                       chLatin_s, chLatin_t, chLatin_a,
                                       chLatin_n, chLatin_t, chNull };

const XMLCh CCommand::X_counter[] = { chLatin_c, chLatin_o, chLatin_u,
                                       chLatin_n, chLatin_t, chLatin_e,
                                       chLatin_r, chNull };

const XMLCh CCommand::X_bytecounter[] = { chLatin_b, chLatin_y, chLatin_t,
                                          chLatin_e, chLatin_c, chLatin_o,
                                          chLatin_u, chLatin_n, chLatin_t,
                                          chLatin_e, chLatin_r, chNull };

const XMLCh CCommand::X_errorcounter[] = { chLatin_e, chLatin_r, chLatin_r, chLatin_o,
                                          chLatin_r, chLatin_c, chLatin_o,
                                          chLatin_u, chLatin_n, chLatin_t,
                                          chLatin_e, chLatin_r, chNull };

const XMLCh CCommand::X_guage[] = { chLatin_g, chLatin_u, chLatin_a,
                                    chLatin_g, chLatin_e, chNull };

const XMLCh CCommand::X_bitrateguage[] = { chLatin_b, chLatin_i, chLatin_t,
                                           chLatin_r, chLatin_a, chLatin_t,
                                           chLatin_e,
                                           chLatin_g, chLatin_u, chLatin_a,
                                           chLatin_g, chLatin_e, chNull };

const XMLCh CCommand::X_byterateguage[] = { chLatin_b, chLatin_y, chLatin_t,
                                           chLatin_e, chLatin_r, chLatin_a,
                                            chLatin_t, chLatin_e,
                                           chLatin_g, chLatin_u, chLatin_a,
                                           chLatin_g, chLatin_e, chNull };

const XMLCh CCommand::X_command_id[] = { chLatin_c, chLatin_o, chLatin_m,
                                         chLatin_m, chLatin_a, chLatin_n,
                                         chLatin_d, chUnderscore,
                                         chLatin_i, chLatin_d, chNull };

const XMLCh CCommand::X_result_code[] = { chLatin_r, chLatin_e, chLatin_s,
                                          chLatin_u, chLatin_l, chLatin_t,
                                          chUnderscore, chLatin_c, chLatin_o,
                                          chLatin_d, chLatin_e, chNull };

const XMLCh CCommand::X_integer[] = { chLatin_i, chLatin_n, chLatin_t,
                                      chLatin_e, chLatin_g, chLatin_e,
                                      chLatin_r, chNull };

const XMLCh CCommand::X_unsignedInteger[] = { chLatin_u, chLatin_n, chLatin_s,
                                              chLatin_i, chLatin_g, chLatin_n,
                                              chLatin_e, chLatin_d,
                                              chLatin_I, chLatin_n, chLatin_t,
                                              chLatin_e, chLatin_g, chLatin_e,
                                              chLatin_r, chNull };

const XMLCh CCommand::X_long[] = { chLatin_l, chLatin_o, chLatin_n,
                                   chLatin_g, chNull };

const XMLCh CCommand::X_unsignedLong[] = { chLatin_u, chLatin_n, chLatin_s,
                                          chLatin_i, chLatin_g, chLatin_n,
                                          chLatin_e, chLatin_d,
                                          chLatin_L, chLatin_o, chLatin_n,
                                          chLatin_g, chNull };

const XMLCh CCommand::X_float[] = { chLatin_f, chLatin_l, chLatin_o,
                                    chLatin_a, chLatin_t, chNull };

const XMLCh CCommand::X_string[] = { chLatin_s, chLatin_t, chLatin_r,
                                     chLatin_i, chLatin_n, chLatin_g,
                                     chNull };

const XMLCh CCommand::X_boolean[] = { chLatin_b, chLatin_o, chLatin_o,
                                      chLatin_l, chLatin_e, chLatin_a,
                                      chLatin_n, chNull };


CCommand::CCommand()
{
    m_nCreationTime = CTimeKeeper::Now();
    m_nOperation = OP_UNKNOWN;
    m_nInputModule = 0;
    m_nPortType = 0;
    m_nOutputModule = 0;
    m_nQueueId = 0;
    m_nId = 0;
    m_nCmdId = 0;
    m_nSchedCmdId = 0;
    m_nResult = 0;
    m_nResultLen = 0;
    m_nStatus = 0;
    m_nTimecode = 0;
    m_nRefCnt = 0;
    m_doc = NULL;
    m_initialized = false;
    m_currentElement = NULL;
    m_xmlOutput = false;
    m_processed = false;
    m_nLevel = 0;
}

int
CCommand::Init(const char *pCmdString)
{
    char *CmdString = NULL;
    int nLen;

    // Init has been called
    m_initialized = true;

    /* Create a temporary copy of the input string since our parsing
       is going to trash it */
    if ((nLen = strlen(pCmdString)) == 0 ||
        (CmdString = strdup(pCmdString)) == NULL) {
        return -1;
    }

    // Parse the command string and stuff all name-value pairs into our
    // m_CmdParams list
    char *pName = NULL;
    char *pValue = NULL;
    char *pTmp = NULL;
    bool bStartName = false;
    bool bQuotedValue = false;
    int i;
    char c;
    for (i=0; i < nLen; i++) {
        pTmp = CmdString + i;
        c = *pTmp;
        if (bQuotedValue && pValue != NULL) { // in case of quoted value
            if (c == '"' && pValue != pTmp+1) { // found end of quote
                *pTmp = 0; // terminate the value
                if (bStartName) { // we have started a name
                    m_CmdParams.insert (CmdEntry(pName, pValue));
                }
                bStartName = false;
                bQuotedValue = false;
            }
        }
        else if ((isalnum(c)) || (c == ',') ||
            (c == '.') || (c == '=') ||
            (c == '_') || (c == '-')) {
            if (c == '=') {
                pValue = pTmp+1; // a value starts after '='
                *pTmp = 0; // terminate the name
                if (*pValue == '"') {
                    // found a quoted value. We should terminate the
                    // value only after we see an end of quote
                    pValue++;
                    bQuotedValue = true;
                }
            }
            if (!bStartName) { // first non-white space starts a name
                pName = pTmp;
                bStartName = true;
            }
        }
        else { // found a whitespace or some strange charcater (i.e. not ,.=_)
            *pTmp = 0;
            if (bStartName) { // we have started a name
                if (pValue != NULL) {// we have started a value also
                    m_CmdParams.insert (CmdEntry(pName, pValue));
                }

            }
            bStartName = false;
        }
    }

    free(CmdString);

    // check for standard known name-value pairs
    m_Iter = m_CmdParams.find("cmdid");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        m_nCmdId = atoi(pValue);
    }

    m_Iter = m_CmdParams.find("schedcmdid");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        m_nSchedCmdId = atoi(pValue);
    }

    m_Iter = m_CmdParams.find("action");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        if (strcmp (pValue, "add") == 0) {
            m_nOperation = OP_ADD;
        }
        else if (strcmp (pValue, "update") == 0) {
            m_nOperation = OP_UPDATE;
        }
        else if (strcmp (pValue, "delete") == 0) {
            m_nOperation = OP_DEL;
        }
        else if (strcmp (pValue, "start") == 0) {
            m_nOperation = OP_START;
        }
        else if (strcmp (pValue, "stop") == 0) {
            m_nOperation = OP_STOP;
        }
        else if (strcmp (pValue, "syncstart") == 0) {
            m_nOperation = OP_SYNCSTART;
        }
        else if (strcmp (pValue, "syncstop") == 0) {
            m_nOperation = OP_SYNCSTOP;
        }
        else if (strcmp (pValue, "status") == 0) {
            m_nOperation = OP_STATUS;
        }
        else if (strcmp (pValue, "stats") == 0) {
            m_nOperation = OP_STATS;
        }
        else if (strcmp (pValue, "quit") == 0) {
            m_nOperation = OP_QUIT;
        }
        else if (strcmp (pValue, "list") == 0) {
            m_nOperation = OP_LIST;
        }
        else if (strcmp (pValue, "trace") == 0) {
            m_nOperation = OP_TRACE;
        }
        else if (strcmp (pValue, "reset") == 0) {
            m_nOperation = OP_RESET;
        }
        else if (strcmp (pValue, "xlist") == 0) {
            m_nOperation = OP_XLIST;
        }
        else if (strcmp (pValue, "xqlist") == 0) {
            m_nOperation = OP_XQLIST;
        }
        else if (strcmp (pValue, "xstatus") == 0) {
            m_nOperation = OP_XSTATUS;
        }
        else if (strcmp (pValue, "xglobal") == 0) {
            m_nOperation = OP_XGLOBAL;
        }
        else if (strcmp (pValue, "xhelp") == 0) {
            m_nOperation = OP_XHELP;
        }
        else if (strcmp (pValue, "query") == 0) {
            m_nOperation = OP_QUERY;
        }
        else {
            /* invalid action */
            return -1;
        }
    }
    else {
        /* no action found */
        return -1;
    }

    m_Iter = m_CmdParams.find("srctype");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        if (strcmp (pValue, "WISEncoderPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = WISENCODERPORT;
        }
        else if (strcmp (pValue, "V2DEncoderPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = V2DENCODERPORT;
        }
        else if(strcmp (pValue, "V2DAudioEncoderPort") == 0){
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = V2DAUDIOENCODERPORT;
        }
        else if (strcmp (pValue, "RTPNetworkPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = RTPNETWORKPORT;
        }
        else if (strcmp (pValue, "V2DNetworkPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = V2DNETWORKPORT;
        }
        else if (strcmp (pValue, "VNCNetworkPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = VNCNETWORKPORT;
        }
        else if (strcmp (pValue, "WINFBEncoderPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = WINFBENCODERPORT;
        }
        else if (strcmp (pValue, "UDPNetworkPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = UDPNETWORKPORT;
        }
        else if (strcmp (pValue, "MPEGTSNetworkPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = MPEGTSNETWORKPORT;
        }
        else if (strcmp (pValue, "SchedulerPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
            m_nPortType = SCHEDULERPORT;
        }
        else if (strcmp (pValue, "V2DMediaPort") == 0) {
            // backwards compatibility: same as DMSMediaPort
            m_nPortType = DMSMEDIAPORT;
            m_nInputModule = MOD_READER;
        }
        else if (strcmp (pValue, "DMSMediaPort") == 0) {
            m_nPortType = DMSMEDIAPORT;
            m_nInputModule = MOD_READER;
        }
        else if (strcmp (pValue, "AnyMediaPort") == 0) {
            m_nInputModule = MOD_READER;
        }
        else if (strcmp (pValue, "AnyNetworkPort") == 0) {
            m_nInputModule = MOD_RECEIVER;
        }
    }

    m_Iter = m_CmdParams.find("dsttype");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        if (strcmp (pValue, "V2DDecoderPort") == 0) {
            m_nOutputModule = MOD_SENDER;
            m_nPortType = V2DDECODERPORT;
        }
        else if (strcmp (pValue, "MPEGTSDecoderPort") == 0) {
            m_nOutputModule = MOD_SENDER;
            m_nPortType = MPEGTSDECODERPORT;
        }
        else if (strcmp (pValue, "RTPDecoderPort") == 0) {
            m_nOutputModule = MOD_SENDER;
            m_nPortType = RTPDECODERPORT;
        }
        else if (strcmp (pValue, "UDPDecoderPort") == 0) {
            m_nOutputModule = MOD_SENDER;
            m_nPortType = UDPDECODERPORT;
        }
        else if(strcmp (pValue, "V2DAudioDecoderPort") == 0){
            m_nOutputModule = MOD_SENDER;
            m_nPortType = V2DAUDIODECODERPORT;
        }
        else if (strcmp (pValue, "DMSRecorderPort") == 0) {
            m_nPortType = DMSRECORDERPORT;
            m_nOutputModule = MOD_WRITER;
        }
        else if (strcmp (pValue, "DeleterPort") == 0) {
            m_nPortType = DELETERPORT;
            m_nOutputModule = MOD_WRITER;
        }
        else if (strcmp (pValue, "V2DFastFwdPort") == 0) {
            m_nOutputModule = MOD_SENDER;
            m_nPortType = V2DFFPORT;
        }
        else if(strcmp (pValue, "V2DHardWarePort") == 0){
            m_nOutputModule = MOD_SENDER;
            m_nPortType = V2DHWPORT;
        }
        else if (strcmp (pValue, "V2DPlayerPort") == 0) {
            m_nOutputModule = MOD_SENDER;
            m_nPortType = V2DPLAYERPORT;
        }
        else if (strcmp (pValue, "RTPPlayerPort") == 0) {
            m_nOutputModule = MOD_SENDER;
            m_nPortType = RTPPLAYERPORT;
        }
        else if (strcmp (pValue, "AnyRecorderPort") == 0) {
            m_nOutputModule = MOD_WRITER;
        }
        else if (strcmp (pValue, "AnyDecoderPort") == 0) {
            m_nOutputModule = MOD_SENDER;
        }
        else if (strcmp (pValue, "ImageWriterPort") == 0) {
            m_nOutputModule = MOD_WRITER;
            m_nPortType = IMAGEWRITERPORT;
        }
    }

    m_Iter = m_CmdParams.find("queueid");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        errno = 0;
        int val = strtol(pValue, NULL, 10);
        if (errno == 0)
            m_nQueueId = val;
    }

    m_Iter = m_CmdParams.find("id");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        errno = 0;
        int val = strtol(pValue, NULL, 10);
        if (errno == 0)
            m_nId = val;
    }

    m_Iter = m_CmdParams.find("tc");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        errno = 0;
        timestamp_t val = strtoull(pValue, NULL, 10);
        if (errno == 0)
            m_nTimecode = val;
    }

    m_Iter = m_CmdParams.find("level");
    if (m_Iter != m_CmdParams.end()) {
        pValue = (char *)(*m_Iter).second.c_str();
        errno = 0;
        int val = strtol(pValue, NULL, 10);
        if (errno == 0)
            m_nLevel = val;
    }

    return 0;
}


CCommand::~CCommand()
{
    m_CmdParams.clear();

    if (m_doc != NULL)
        m_doc->release();

}

// lazy creation of m_doc
void
CCommand::InitXMLInternal()
{
    ASSERT(m_doc == NULL);

    DOMImplementation* impl =
        DOMImplementationRegistry::getDOMImplementation(X_Core);

    m_doc = impl->createDocument(0, X_result, 0);
    if (m_doc != NULL) {
        DOMElement*  objectsElement = m_doc->createElement(X_objects);
        m_currentElement = m_doc->getDocumentElement();
        m_currentElement->appendChild(objectsElement);
    }
}

int CCommand::GetVar( const char *pName, char *pValue, int len)
{
    int retval=-1;
    m_Iter = m_CmdParams.find(pName);
    if (m_Iter != m_CmdParams.end()) {
        strncpy(pValue, (char *)(*m_Iter).second.c_str(), len);
        pValue[len-1] = '\0';
        retval = 0;
    }
    return retval;
}
int CCommand::GetElapsed()
{
    return (int) (CTimeKeeper::Now() - m_nCreationTime);
}

void
CCommand::SetProcessed()
{
    m_processed = true;

    // Reuse m_nCreationTime to estimate how long it has been since we
    // sent a reponse to the the client
    m_nCreationTime = CTimeKeeper::Now();

}

int CCommand::GetIntVar(const char *pName, int *pnValue)
{
    int retval=-1;
    char pValue[80];
    if (GetVar(pName, pValue, 80) != -1) {
        errno = 0;
        int val = strtol(pValue, NULL, 10);
        if (errno == 0) {
            *pnValue = val;
            retval = 0;
        }
    }
    return retval;
}

int CCommand::GetFloatVar(const char *pName, double *pnValue)
{
    int retval=-1;
    char pValue[80];
    if (GetVar(pName, pValue, 80) != -1) {
        errno = 0;
        double val = strtod(pValue, NULL);
        if (errno == 0) {
            *pnValue = val;
            retval = 0;
        }
    }
    return retval;
}

int CCommand::GetTimestampVar(const char *pName, timestamp_t *pnValue)
{
    int retval=-1;
    char pValue[80];
    if (GetVar(pName, pValue, 80) != -1) {
        errno = 0;
        timestamp_t val = strtoull(pValue, NULL, 10);
        if (errno == 0) {
            *pnValue = val;
            retval = 0;
        }
    }
    return retval;
}

int CCommand::GetIntListVar(const char *pName, intlist & a_intlist)
{
    int retval=-1;
    char pValue[1024];
    char *pTmp = NULL, *cp;
    if (GetVar(pName, pValue, 1024) != -1) {
        retval = 0;
        for (cp = pValue; *cp; cp++) {
            if (pTmp == NULL) pTmp = cp;
            if (!isalnum(*cp)) {
                if (pTmp) {
                    a_intlist.push_back(atoi(pTmp));
                    retval++;
                    pTmp=NULL;
                }
            }
        }
        if (pTmp) {
            a_intlist.push_back(atoi(pTmp));
            retval++;
            pTmp = NULL;
        }
    }
    return retval;
}

int CCommand::SetOperation( int nOperation)
{
    m_nOperation = nOperation;
    return m_nOperation;
}

void CCommand::Init()
{
    XMLPlatformUtils::Initialize();
}

void CCommand::Finish()
{

    XMLPlatformUtils::Terminate();
}


void
CCommand::SetParamameter(const char *a_name,
                         const XMLCh *a_type,
                         const char *a_value,
                         ParameterKind a_kind) {

    ASSERT(m_currentElement != NULL);
    ASSERT(m_doc != NULL);
    if (m_currentElement == NULL)
        return;
    DOMElement*  paramElem = m_doc->createElement(X_property);

    m_currentElement->appendChild(paramElem);
    paramElem->setAttribute(X_name, X(a_name));
    paramElem->setAttribute(X_type, a_type);
    switch(a_kind) {
    case Constant:
        paramElem->setAttribute(X_kind, X_constant);
        break;
    case Counter:
        paramElem->setAttribute(X_kind, X_counter);
        break;
    case ByteCounter:
        paramElem->setAttribute(X_kind, X_bytecounter);
        break;
    case ErrorCounter:
        paramElem->setAttribute(X_kind, X_errorcounter);
        break;
    case Guage:
        paramElem->setAttribute(X_kind, X_guage);
        break;
    case BitRateGuage:
        paramElem->setAttribute(X_kind, X_bitrateguage);
        break;
    case ByteRateGuage:
        paramElem->setAttribute(X_kind, X_byterateguage);
        break;
    case Undefined:
        // not setting the "kind" attribute
        break;
    }

    DOMText*    prodDataVal = m_doc->createTextNode(X(a_value));
    paramElem->appendChild(prodDataVal);

    m_xmlOutput = true;
}

void
CCommand::SetCurrentObject(int a_objectId)
{

    ASSERT(a_objectId > 0);

    InitXML();
    DOMElement*  paramElem = m_doc->createElement(X_object);
    try {
        DOMElement* rootElem = m_doc->getDocumentElement();
        rootElem->appendChild(paramElem);
        m_currentElement = paramElem;
        char buf[32];
        sprintf(buf, "%d", a_objectId);
        SetParamameter("Id", X_integer, buf, Constant);
    }
    catch (...){
        SYSLOG_ERROR("Some error in XML document creation");
    }
}

void
CCommand::SetObjectParam(const char *a_name, int a_value)
{
    char buf[32];
    snprintf(buf, 32, "%ld", (long)a_value);
    SetParamameter(a_name, X_integer, buf);
}

void
CCommand::SetObjectParam(const char *a_name, long a_value)
{
    char buf[32];
    snprintf(buf, 32, "%ld", a_value);
    SetParamameter(a_name, X_integer, buf);
}

void
CCommand::SetObjectParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf);
}

void
CCommand::SetObjectParam(const char *a_name, uint32_t a_value)
{
    char buf[32];
    snprintf(buf, 32, "%"PRIu32"", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf);
}

void
CCommand::SetObjectParam(const char *a_name, const char *a_value)
{
    SetParamameter(a_name, X_string, a_value);
}

void
CCommand::SetObjectParam(const char *a_name, const string & a_value)
{
    SetParamameter(a_name, X_string, a_value.c_str());
}

void
CCommand::SetObjectParam(const char *a_name, bool a_value)
{
    if (a_value)
        SetParamameter(a_name, X_boolean, "True");
    else
        SetParamameter(a_name, X_boolean, "False");
}

void
CCommand::SetObjectParam(const char *a_name, double a_value)
{
    char buf[64];
    snprintf(buf, 64, "%f", a_value);
    SetParamameter(a_name, X_float, buf);
}

void
CCommand::SetObjectParam(const char *a_name, unsigned long long a_value)
{
    char buf[64];
    snprintf(buf, 64, "%"PRIu64"", a_value);
    SetParamameter(a_name, X_unsignedLong, buf);
}

void
CCommand::SetObjectParam(const char *a_name, long long a_value)
{
    char buf[64];
    snprintf(buf, 64, "%"PRId64"", a_value);
    SetParamameter(a_name, X_long, buf);
}

void
CCommand::SetObjectConstantParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf, Constant);
}

void
CCommand::SetObjectCounterParam(const char *a_name, int a_value)
{
    char buf[32];
    snprintf(buf, 32, "%ld", (long)a_value);
    SetParamameter(a_name, X_integer, buf, Counter);
}

void
CCommand::SetObjectCounterParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf, Counter);
}

void
CCommand::SetObjectCounterParam(const char *a_name, unsigned long long a_value)
{
    char buf[32];
    snprintf(buf, 32, "%"PRIu64"", a_value);
    SetParamameter(a_name, X_unsignedLong, buf, Counter);

}

void
CCommand::SetObjectByteCounterParam(const char *a_name, int a_value)
{
    char buf[32];
    snprintf(buf, 32, "%ld", (long)a_value);
    SetParamameter(a_name, X_integer, buf, ByteCounter);
}

void
CCommand::SetObjectByteCounterParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf, ByteCounter);
}


void
CCommand::SetObjectByteCounterParam(const char *a_name, unsigned long long a_value)
{
    char buf[32];
    snprintf(buf, 32, "%"PRIu64"", a_value);
    SetParamameter(a_name, X_unsignedLong, buf, ByteCounter);
}

void
CCommand::SetObjectErrorCounterParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf, ErrorCounter);
}

void
CCommand::SetObjectGuageParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf, Guage);
}

void
CCommand::SetObjectBitrateGuageParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf, BitRateGuage);
}

void
CCommand::SetObjectByterateGuageParam(const char *a_name, unsigned long  a_value)
{
    char buf[32];
    snprintf(buf, 32, "%lu", a_value);
    SetParamameter(a_name, X_unsignedInteger, buf, ByteRateGuage);
}

void
CCommand::SetRootParam(const char *a_name, int a_value)
{

    char buf[32];
    snprintf(buf, 32, "%ld", (long) a_value);
    InitXML();
    m_currentElement = m_doc->getDocumentElement();
    SetParamameter(a_name, X_integer, buf);
}

void
CCommand::SetRootParam(const char *a_name, unsigned long a_value)
{

    char buf[32];
    snprintf(buf, 32, "%ld", (long) a_value);
    InitXML();
    m_currentElement = m_doc->getDocumentElement();
    SetParamameter(a_name, X_unsignedInteger, buf);
}

void
CCommand::SetRootParam(const char *a_name, unsigned long long a_value)
{

    char buf[64];
    snprintf(buf, 64, "%"PRIu64"", a_value);
    InitXML();
    m_currentElement = m_doc->getDocumentElement();
    SetParamameter(a_name, X_unsignedLong, buf);
}


void
CCommand::SetRootParam(const char *a_name, double a_value)
{

    char buf[32];
    snprintf(buf, 32, "%f", a_value);
    InitXML();
    m_currentElement = m_doc->getDocumentElement();
    SetParamameter(a_name, X_float, buf);
}

const string&
CCommand::GetResponse()
{

    /* If response has not been created yet, create it */
    if (m_response.length() <= 0) {
        char buf[256];
        memset (buf, 0, 256);
        snprintf(buf, 255,
                "%s i:%d r:%d s:%d t:%"PRIu64" cid:%d result:\"",
                RESPMSG,
                m_nId, m_nResult,
                m_nStatus, m_nTimecode,
                m_nCmdId);
        m_response.append(buf);

        /* Create XML result string if needed */
        if (m_xmlOutput) {
            char strbuf[32];
            DOMElement* rootElem = m_doc->getDocumentElement();

            DOMElement* elem;
            DOMNode *child;
            DOMText* txtNode;

            elem = m_doc->createElement(X_result_code);
            child = rootElem->getFirstChild();
            rootElem->insertBefore(elem, child);
            sprintf(strbuf, "%d", m_nResult);
            txtNode = m_doc->createTextNode(X(strbuf));
            elem->appendChild(txtNode);

            elem = m_doc->createElement(X_command_id);
            rootElem->appendChild(elem);
            sprintf(strbuf, "%d", m_nCmdId);
            txtNode = m_doc->createTextNode(X(strbuf));
            elem->appendChild(txtNode);


            DOMImplementation *impl =
                DOMImplementationRegistry::getDOMImplementation(X_Core);

            DOMWriter *writer = impl->createDOMWriter();

            MemBufFormatTarget *memBufTarget = new MemBufFormatTarget();

            writer->writeNode(memBufTarget, *m_doc);
            m_pResultString.append((char *)memBufTarget->getRawBuffer());
            delete memBufTarget;
            writer->release();
        }

        m_response.append(m_pResultString);
        m_response.append("\"");
    }
    return m_response;
}


