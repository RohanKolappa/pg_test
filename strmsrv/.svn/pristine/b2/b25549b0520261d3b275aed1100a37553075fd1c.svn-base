#include "strmsrv_compatibility.h"

#include "common.hh"
#include "sframe.hh"
#include "sbuffer.hh"

#ifdef CSFRAME_USE_MEMORY_POOL
// Definitions of static class members
CSFrame *CSFrame::m_freeListHead;
const int CSFrame::CHUNK_SIZE = 1024*8;
CMutex CSFrame::m_sMutex;

// Operator new : we do custom memory management since this
// object is created and destroyed in the data path (for every frame)
void * CSFrame::operator new(size_t a_size) {

    if (a_size != sizeof(CSFrame))
        return ::operator new(a_size);

    CScopedLock lock(m_sMutex);

        
    // return the current head pointer
    CSFrame *returnPointer = m_freeListHead;

    if (returnPointer != NULL) { // frames left in the pool
        m_freeListHead = returnPointer->m_pNext;
    }
    else {  // pool is empty, allocate another
        CSFrame *chunk = 
            static_cast<CSFrame *>(::operator new (CHUNK_SIZE *
                                                   sizeof(CSFrame)));
        for (int i=1; i < CHUNK_SIZE-1; i++) {
            chunk[i].m_pNext =  &chunk[i+1];
        }
        chunk[CHUNK_SIZE-1].m_pNext = NULL;
        returnPointer = &chunk[0];
        m_freeListHead = &chunk[1];
    }
        
    return returnPointer;
}

/// operator delete
void CSFrame::operator delete(void *a_object, size_t a_size) {

    if (a_object == NULL) // nothing to free
        return; 

    if (a_size != sizeof(CSFrame)) { // safety valve
        ::operator delete(a_object);
        return;
    }

    // cast void pointer to this class
    CSFrame *pFrame = static_cast<CSFrame *>(a_object);

    // put the object back in our pool stack
    {
        CScopedLock lock(m_sMutex);
        pFrame->m_pNext = m_freeListHead;
        m_freeListHead = pFrame;
    }
}
#endif

CSFrame::CSFrame(CSBuffer *a_sBuffer, 
                 int a_offset, 
                 int a_len, 
                 unsigned long a_bFlag, 
                 timestamp_t a_nTimestamp) :
    m_nLen(a_len), 
    m_bFlag(a_bFlag), 
    m_nMessageType(Normal),
    m_discontinuity(false),
    m_nTimestamp(a_nTimestamp),
    m_sBuffer(a_sBuffer),
    m_offset(a_offset),
    m_refCount(0),
    m_cData(NULL)
{
    ASSERT(m_sBuffer != NULL);
    m_sBuffer->AddReference();
}

CSFrame::CSFrame(const char *a_cData, 
                 int a_len, 
                 unsigned long a_bFlag, 
                 timestamp_t a_nTimestamp) :
    m_nLen(a_len), 
    m_bFlag(a_bFlag), 
    m_nMessageType(Normal),
    m_discontinuity(false),
    m_nTimestamp(a_nTimestamp),
    m_sBuffer(NULL),
    m_offset(0),
    m_refCount(0),
    m_cData(NULL)
{
    m_cData = new char [m_nLen+1];
    memcpy(m_cData, a_cData, m_nLen);
    m_cData[m_nLen] = 0;
}

CSFrame::CSFrame(timestamp_t a_nTimestamp, MessageType a_nMessageType) :
    m_nLen(0), 
    m_bFlag(0), 
    m_nMessageType(a_nMessageType),
    m_discontinuity(false),
    m_nTimestamp(a_nTimestamp),
    m_sBuffer(NULL),
    m_offset(0),
    m_refCount(0),
    m_cData(NULL)
{
}

CSFrame::~CSFrame()
{
    if (m_sBuffer != NULL)
        m_sBuffer->RemoveReference();
    delete [] m_cData;
}


// Set a CSBuffer as data after object created
void
CSFrame::SetData(CSBuffer *a_sBuffer,
                 int a_offset, 
                 int a_len)
{
    ASSERT(m_cData == NULL);
    if (m_sBuffer != NULL)
        m_sBuffer->RemoveReference();

    ASSERT(a_sBuffer != NULL);
    m_nLen = a_len;
    m_offset = a_offset;
    m_sBuffer = a_sBuffer;
    m_sBuffer->AddReference();

}

// Set a char pointer after object created
void
CSFrame::SetData(const char *a_cData,
                 int a_len) 
{

    ASSERT(m_sBuffer == NULL);

    delete [] m_cData;
    m_nLen = a_len;
    m_cData = new char [m_nLen + 1];
    memcpy(m_cData, a_cData, m_nLen);
    m_cData[m_nLen] = 0;

}
