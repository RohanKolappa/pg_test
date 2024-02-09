#include "strmsrv_compatibility.h"
#include "common.hh"
#include "sbuffer.hh"

// CSBuffer Class
int CSBuffer::m_numberOfBuffers;
int CSBuffer::m_sizeOfBuffers;
CMutex CSBuffer::m_statMutex;
uint32_t CSBuffer::m_buffersUnavailable;

#define SBUFSIZE_SCALE  10

CSBuffer::CSBuffer(int a_length, int a_alignTo, bool bEnforceLimits) :
    m_length(a_length), m_alignmentOffset(0), m_maxRefcount(0)
{
    m_refCount = 0;
    if (bEnforceLimits == false) {
        m_buffer = new char[a_length + a_alignTo];
        m_alignmentOffset = GetAlignmentOffset(a_alignTo);
        {
            CScopedLock lock(m_statMutex);
            m_numberOfBuffers++;
        }
    }
    else if ((uint64_t)m_sizeOfBuffers <= ((uint64_t)((1024ULL * 1024 * 500) >> SBUFSIZE_SCALE))) {
        // memory limit 500 megs
        m_buffer = new char[a_length + a_alignTo];
        m_alignmentOffset = GetAlignmentOffset(a_alignTo);
        {
            CScopedLock lock(m_statMutex);
            m_sizeOfBuffers += (m_length >> SBUFSIZE_SCALE);
            m_numberOfBuffers++;
        }
    }
    else {
        m_buffer = NULL;
        m_buffersUnavailable++;
    }

}

CSBuffer::~CSBuffer()
{
    if (m_buffer != NULL) {
        CScopedLock lock(m_statMutex);
        m_sizeOfBuffers -= (m_length >> SBUFSIZE_SCALE);
        m_numberOfBuffers--;
    }
    delete[] m_buffer;
}

void CSBuffer::AddReference()
{
    CScopedLock lock(m_mutex);
    m_refCount++;
}

void CSBuffer::RemoveReference()
{
    int refCount;
    {
        CScopedLock lock(m_mutex);
        m_refCount--;
        refCount = m_refCount;
    }
    if (refCount <= 0) {
        delete this;
    }
}

int CSBuffer::GetReferenceCount()
{
    return m_refCount;
}

int CSBuffer::GetAlignmentOffset(int alignTo)
{
    if (alignTo <= 0)
        return 0;
    char * ptr = m_buffer;
    while ((unsigned int) (ptr) % alignTo != 0)
        ptr++;
    return (ptr - m_buffer);
}

int CSBuffer::BufferCount()
{
    return m_numberOfBuffers;
}

int CSBuffer::BufferSize()
{
    return (m_sizeOfBuffers << SBUFSIZE_SCALE);
}

uint32_t CSBuffer::BuffersUnavailableCount()
{
    return m_buffersUnavailable;
}

