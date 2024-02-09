#ifndef __H_SFRAME__
#define __H_SFRAME__

#include "mutex.hh"
#include "timekeeper.hh"
#include "sbuffer.hh"

#include "MediaFlags.hh"


class CSFrame {
public:

    enum MessageType {
        Normal = 1,
        OutOfBand,
        GapPresent,
        EndOfMedia,
        MediaError,
        StartOfStream,
        EndOfStream,
        Connected,
        Disconnected,
        SystemError,
        Custom,
    };

    CSFrame(CSBuffer *a_sBuffer, 
            int a_offset, 
            int a_len, 
            unsigned long a_bFlag, 
            timestamp_t a_nTimestamp);
        
    CSFrame(const char *a_cData, 
            int a_len, 
            unsigned long a_bFlag = 0, 
            timestamp_t a_nTimestamp = 0);

    CSFrame(timestamp_t a_Timestamp, MessageType a_nMessageType);

    ~CSFrame();

    timestamp_t GetTimeStamp() const { return m_nTimestamp; };
    unsigned long GetMediaFlag() const { return m_bFlag; };
    MessageType GetMessageType() const { return m_nMessageType; };
    unsigned long GetFrameType() const { return (m_bFlag & 0xFFFF); };
    unsigned long GetStreamSubType() const { return (CStreamMediaFlag(m_bFlag).GetStreamSubType()); };
    unsigned long GetStreamType() const { return (CStreamMediaFlag(m_bFlag).GetStreamType()); };
    unsigned long GetStreamType(unsigned long  a_StreamType) {return ((GetFrameVersion() == 0) ? a_StreamType : (CStreamMediaFlag(m_bFlag).GetStreamType())); };
    unsigned long GetFrameVersion() const { return (CStreamMediaFlag(m_bFlag).GetFrameVersion()); };
    int GetLength() const { return m_nLen; };
    CSBuffer * GetBuffer() const { return m_sBuffer; };
    char * GetData() { 
        return m_sBuffer == NULL ? 
            m_cData : m_sBuffer->GetBuffer(m_offset);
    };
    int GetOffset() const { return m_offset; };
    bool HasDiscontinuity() const { return m_discontinuity; };
    void SetDiscontinuity()  { m_discontinuity = true; };
    void IncreaseRef() { ++m_refCount; };
    void DecreaseRef() { --m_refCount; };
    int GetRef() const { return m_refCount; };

    void SetMediaFlag(unsigned long a_flag)  { m_bFlag = a_flag; };
    void SetCustomObject(void * a_pObject)  { m_bFlag = (unsigned long) a_pObject; };
    void * GetCustomObject() const { return (void *) m_bFlag; };

    void SetData(CSBuffer *a_sBuffer, int a_offset, int a_len);
    void SetData(const char *a_cData, int a_len);

    static unsigned long CreateMediaFlag(unsigned long a_nFrameType,
                                         unsigned long a_nStreamType,
                                         unsigned long a_nStreamSubType = 0) {

        return ((FRAMEVERSION_1 << FRAME_VERSION_SHIFT) | ((a_nStreamType & 0xFF) << STREAM_TYPE_SHIFT) | ((a_nStreamSubType & 0xF) << STREAM_SUB_TYPE_SHIFT) | (a_nFrameType & 0xFFFF));
    }

#ifdef CSFRAME_USE_MEMORY_POOL
    static void * operator new(size_t a_size);
    static void operator delete(void *a_object, size_t a_size);
#endif


private:
    int                     m_nLen;
    unsigned long           m_bFlag;
    MessageType             m_nMessageType;
    bool                    m_discontinuity; 
    timestamp_t             m_nTimestamp;
    CSBuffer *              m_sBuffer;
    int                     m_offset;
    int                     m_refCount;
    char *                  m_cData;

    CSFrame();
    CSFrame(const CSFrame& rhs);
    CSFrame& operator=(const CSFrame& rhs);

#ifdef CSFRAME_USE_MEMORY_POOL
    static const int        CHUNK_SIZE;
    static CMutex           m_sMutex;
    static CSFrame *        m_freeListHead;
    CSFrame *       m_pNext;
#endif

};


#endif
