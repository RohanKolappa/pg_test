#ifndef __H_SBUFFER__
#define __H_SBUFFER__

#include "atomic.hh"
#include "mutex.hh"

class CSBuffer {
public:
    CSBuffer(int a_length, int a_alignTo = 0, bool a_bEnforceLimits = true);
    void AddReference();
    void RemoveReference();
    int GetReferenceCount();
    char * GetBuffer(int a_offset) {return (m_buffer + m_alignmentOffset + a_offset);};
    static int BufferCount();
    static int BufferSize();
    static uint32_t BuffersUnavailableCount();

protected:

private:
    char *m_buffer;
    int m_refCount;
    unsigned long m_length;
    CMutex m_mutex;
    int m_alignmentOffset;

    int m_maxRefcount;
    static int m_numberOfBuffers;
    static int m_sizeOfBuffers;
    static CMutex m_statMutex;
    static uint32_t m_buffersUnavailable;

    ~CSBuffer();
    int GetAlignmentOffset(int alignTo);

    CSBuffer();
    CSBuffer(const CSBuffer& rhs);
    CSBuffer& operator=(const CSBuffer& rhs);
};

#endif
