#ifndef __H_COMMAND__
#define __H_COMMAND__

#include <string>
#include <map>
#include <vector>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE


#include "timekeeper.hh"

/// A Single Command
#define OP_UNKNOWN      0
#define OP_ADD          1
#define OP_DEL          2
#define OP_START        3
#define OP_STOP         4
#define OP_SYNCSTART    5
#define OP_SYNCSTOP     6
#define OP_STATUS       7
#define OP_LIST         9
#define OP_QUIT         10
#define OP_TRACE        11
#define OP_STATS        12
#define OP_RESET        13
#define OP_UPDATE       14
#define OP_QUERY        15

#define OP_XLIST        51
#define OP_XSTATUS      52
#define OP_XQLIST       53
#define OP_XGLOBAL      54
#define OP_XHELP        55

#define MOD_RECEIVER    1
#define MOD_SENDER      2
#define MOD_READER      3
#define MOD_WRITER      4

#define RESULTLEN       8192

typedef std::map<std::string, std::string> CmdParam;
typedef CmdParam::value_type CmdEntry;

typedef std::vector<int> intlist;

class CCommand {
public:
    CCommand();
    ~CCommand();

    int Init(const char *pCmdString);
    int GetOperation() {return m_nOperation;};
    int SetOperation(int nOperation);
    int SetResult(int nResult){ m_nResult = nResult; return 0;};
    int SetStatus(int nStatus){ m_nStatus = nStatus; return 0;};
    int SetTimecode(timestamp_t nTimecode){ m_nTimecode = nTimecode; return 0;};
    int GetResult() {return m_nResult;};
    int GetStatus() {return m_nStatus;};
    timestamp_t GetTimecode() {return m_nTimecode;};
    std::string& GetResultString() {return m_pResultString;};
    int GetId() {return m_nId;};
    int GetCmdId() {return m_nCmdId;};
    int GetSchedCmdId() {return m_nSchedCmdId;};
    int GetQueueId() {return m_nQueueId;};
    int GetInputModule() {return m_nInputModule;};
    int GetOutputModule() {return m_nOutputModule;};
    int GetPortType() {return m_nPortType;};
    int GetIntVar(const char *data, int *value);
    int GetFloatVar(const char *data, double *value);
    int GetTimestampVar(const char *data, timestamp_t *value);
    int GetIntListVar(const char *data, intlist & a_intlist);
    int GetVar(const char *name, char *value, int len);
    int GetElapsed();
    int IncreaseRef() {return ++m_nRefCnt;};
    int DecreaseRef() {return --m_nRefCnt;};
    int GetRef() {return m_nRefCnt;};
    bool StatusLevel(int a_level) {return (a_level <= m_nLevel);}
    bool Initialized() {return m_initialized;};
    bool Processed() {return m_processed;};
    void SetProcessed();

    const std::string& GetResponse();
    static void Init();
    static void Finish();


    void SetCurrentObject(int a_objectId);
    void SetObjectParam(const char *a_name, int a_value);
    void SetObjectParam(const char *a_name, long a_value);
    void SetObjectParam(const char *a_name, unsigned long  a_value);
    void SetObjectParam(const char *a_name, uint32_t a_value);
    void SetObjectParam(const char *a_name, long long a_value);
    void SetObjectParam(const char *a_name, unsigned long long a_value);
    void SetObjectParam(const char *a_name, const char *a_value);
    void SetObjectParam(const char *a_name, const std::string & a_value);

    void SetObjectParam(const char *a_name, bool a_value);
    void SetObjectParam(const char *a_name, double a_value);
    void SetObjectConstantParam(const char *a_name, unsigned long a_value);
    void SetObjectCounterParam(const char *a_name, int a_value);
    void SetObjectCounterParam(const char *a_name, unsigned long a_value);
    void SetObjectCounterParam(const char *a_name, unsigned long long a_value);
    void SetObjectByteCounterParam(const char *a_name, int a_value);
    void SetObjectByteCounterParam(const char *a_name, unsigned long a_value);
    void SetObjectByteCounterParam(const char *a_name, unsigned long long a_value);
    void SetObjectErrorCounterParam(const char *a_name, unsigned long a_value);
    void SetObjectGuageParam(const char *a_name, unsigned long a_value);
    void SetObjectBitrateGuageParam(const char *a_name, unsigned long a_value);
    void SetObjectByterateGuageParam(const char *a_name, unsigned long a_value);
    void SetRootParam(const char *a_name, int a_value);
    void SetRootParam(const char *a_name, unsigned long a_value);
    void SetRootParam(const char *a_name, unsigned long long a_value);
    void SetRootParam(const char *a_name, double a_value);

private:
    CmdParam m_CmdParams;
    CmdParam::iterator m_Iter;
    int m_nOperation;
    int m_nInputModule;
    int m_nOutputModule;
    int m_nPortType;
    int m_nQueueId;
    int m_nId;
    int m_nCmdId;
    int m_nSchedCmdId;
    int m_nLevel;
    int m_nResult;
    int m_nStatus;
    timestamp_t  m_nCreationTime;
    timestamp_t m_nTimecode;
    std::string m_pResultString;
    int  m_nResultLen;
    int m_nRefCnt;
    bool m_initialized;
    bool m_processed;

    std::string m_response;


    DOMDocument* m_doc;
    bool m_xmlOutput;
    DOMElement* m_currentElement;

    static const XMLCh X_Core[];
    static const XMLCh X_result[];
    static const XMLCh X_objects[];
    static const XMLCh X_object[];
    static const XMLCh X_property[];
    static const XMLCh X_name[];
    static const XMLCh X_type[];
    static const XMLCh X_kind[];
    static const XMLCh X_constant[];
    static const XMLCh X_counter[];
    static const XMLCh X_bytecounter[];
    static const XMLCh X_errorcounter[];
    static const XMLCh X_guage[];
    static const XMLCh X_bitrateguage[];
    static const XMLCh X_byterateguage[];
    static const XMLCh X_command_id[];
    static const XMLCh X_result_code[];
    static const XMLCh X_integer[];
    static const XMLCh X_unsignedInteger[];
    static const XMLCh X_long[];
    static const XMLCh X_unsignedLong[];
    static const XMLCh X_boolean[];
    static const XMLCh X_string[];
    static const XMLCh X_float[];


    enum ParameterKind {
        Undefined = 1,
        Constant,
        Counter,
        ByteCounter,
        ErrorCounter,
        Guage,
        BitRateGuage,
        ByteRateGuage
    };

    void SetParamameter(const char *a_name,
                        const XMLCh *a_type,
                        const char *a_value,
                        ParameterKind a_kind = Undefined);

    void InitXMLInternal();
    void InitXML() {if (m_doc == NULL) InitXMLInternal(); };
};
#endif /* __H_COMMAND__ */
