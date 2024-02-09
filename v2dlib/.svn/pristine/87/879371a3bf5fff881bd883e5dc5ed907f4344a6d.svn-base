#if !defined __CV2D_BITSTREAM_H__
#define __CV2D_BITSTREAM_H__

#include "v2dlib_compatibility.h"

//#define DEBUG_CLASS

#define NEW_GB

#ifdef DEBUG_CLASS
#define PRINTF_BS printf
#else
#define PRINTF_BS(...) do {} while (0)
#endif

/*! 
CBitStream provides interface to get bits from the 
frame buffer.
 */
class CBitStream 
{
public:
	CBitStream(const unsigned char* a_pucInBuf, size_t a_nBufSize);
	CBitStream();

	~CBitStream();
  
    void init(const unsigned char* a_pucInBuf, size_t a_nBufSize);

	void getBits(unsigned int a_nBits, unsigned int &a_rnVal);
    unsigned int getBitsValue(unsigned int a_nBits);
    bool get32Bits(unsigned int &a_rnVal);
	void showBits(unsigned int a_nBits, unsigned int &a_rnVal);
    unsigned int showBitsValue(unsigned int a_nBits);
	void flushBits(unsigned int a_nBits);
    bool showHi9Bits(unsigned int &a_rnVal);
    void expand17Bits(unsigned int &a_rnVal);
    void expand25Bits(unsigned int &a_rnVal);
    int  totalBitsRead();
    void flushBitsToStartFourByteBoundary();
    const uint8_t * getStartPointer();
    const uint8_t * getCurrentPointer();
    size_t getLength();
    int getRemainingBytes();

    int GolombReadUE();
    int GolombReadSE();
    int GolombReadTE(int x);
    void ReduceLength(int len);

private:    

    const uint8_t *       m_curr_ptr;
    int32_t               m_bit_offset;
    const uint8_t *       m_start_ptr;
    const uint8_t *       m_end_ptr;

    CBitStream(const CBitStream& a_bStr);
    const CBitStream& operator=(const CBitStream& a_bStr);
};


inline int CBitStream::totalBitsRead() {
    return ((m_curr_ptr - m_start_ptr) << 3)  + m_bit_offset;
}

inline void CBitStream::showBits(unsigned int a_nBits, unsigned int &a_rnVal)
{
    uint32_t code = m_curr_ptr[0] << 24 |
            m_curr_ptr[1] << 16 | m_curr_ptr[2] << 8 | m_curr_ptr[3];
    a_rnVal = (code << m_bit_offset) >> (32 - a_nBits);
    PRINTF_BS("   ShowBits 0x%x %d\n", a_rnVal, a_nBits);
}

inline bool CBitStream::get32Bits(unsigned int &a_rnVal)
{
    uint32_t code = 0;
    getBits(16, code);
    a_rnVal = code << 16;
    getBits(16, code);
    a_rnVal |= code & 0xFFFF;
    return true;
}

inline unsigned int CBitStream::showBitsValue(unsigned int a_nBits)
{
    uint32_t code = m_curr_ptr[0] << 24 |
            m_curr_ptr[1] << 16 | m_curr_ptr[2] << 8;
    PRINTF_BS("   ShowBits 0x%x %d\n", (code << m_bit_offset) >> (32 - a_nBits), a_nBits);
    return (code << m_bit_offset) >> (32 - a_nBits);
}

inline bool CBitStream::showHi9Bits(unsigned int &a_rnVal)
{
    a_rnVal = (((m_curr_ptr[0] << 24) | (m_curr_ptr[1] << 16)) << m_bit_offset);
    return true;
}

inline void CBitStream::expand17Bits(unsigned int &a_rnVal) {
    a_rnVal |= m_curr_ptr[2] << (8 + m_bit_offset);
}

inline void CBitStream::expand25Bits(unsigned int &a_rnVal) {
    a_rnVal |= m_curr_ptr[3] << m_bit_offset;
}


inline void CBitStream::flushBits(unsigned int a_nBits)
{
    PRINTF_BS("   FlushBits %d\n", a_nBits);
    m_bit_offset += a_nBits;
    m_curr_ptr += (m_bit_offset >> 3);
    m_bit_offset &= 7;
}

inline void CBitStream::getBits(unsigned int a_nBits, unsigned int &a_rnVal)
{
    showBits(a_nBits, a_rnVal);
    flushBits(a_nBits);
}

inline unsigned int CBitStream::getBitsValue(unsigned int a_nBits)
{
    if (a_nBits == 0)
        return 0;
    unsigned int retval;
    showBits(a_nBits, retval);
    flushBits(a_nBits);
    return retval;
}

inline void CBitStream::flushBitsToStartFourByteBoundary()
{
    int bitsToRead = 32 - (totalBitsRead() % 32);
    if (bitsToRead != 32)
        flushBits(bitsToRead);
}

inline const uint8_t * CBitStream::getStartPointer()
{
    return m_start_ptr;
}

inline const uint8_t * CBitStream::getCurrentPointer()
{
    return m_curr_ptr;
}

inline size_t CBitStream::getLength()
{
    return m_end_ptr - m_start_ptr;
}

inline int CBitStream::getRemainingBytes()
{
    if (m_end_ptr >= m_curr_ptr)
        return m_end_ptr - m_curr_ptr;
    return -1;
}

inline int CBitStream::GolombReadUE()
{
    int i = 0;

    while (getBitsValue(1) == 0 && i < 32)
    {
        i++;
    }
    return (( 1 << i) - 1 + getBitsValue(i));
}

inline int CBitStream::GolombReadSE()
{
    int val = GolombReadUE();

    return val & 0x01 ? (val+1)/2 : -(val/2);
}

inline int CBitStream::GolombReadTE(int x)
{
    if( x == 1 )
    {
        return 1 - getBitsValue(1);
    }
    else if( x > 1 )
    {
        return GolombReadUE( );
    }
    return 0;
}

inline void CBitStream::ReduceLength(int len)
{

    if (m_end_ptr != NULL) {
        m_end_ptr -= len;
        m_curr_ptr = m_start_ptr;
        m_bit_offset = 0;
    }

}

#endif // __CV2D_BITSTREAM_H__
