

#include "bitstream.h"

CBitStream::CBitStream(const unsigned char* a_pucInBuf, size_t a_nBufSize)
{
    init(a_pucInBuf, a_nBufSize);
}

CBitStream::CBitStream()
{
}

CBitStream::~CBitStream()
{
}

void 
CBitStream::init(const unsigned char* a_pucInBuf, size_t a_nBufSize)
{
    m_start_ptr = a_pucInBuf;
    m_end_ptr = m_start_ptr + a_nBufSize;
    m_curr_ptr = m_start_ptr;
    m_bit_offset = 0;
}




