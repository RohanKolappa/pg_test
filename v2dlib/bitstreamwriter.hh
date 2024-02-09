/*
 * bitstreamwriter.hh
 *
 *  Created on: Oct 19, 2010
 *      Author: rkale
 */

#ifndef BITSTREAMWRITER_HH_
#define BITSTREAMWRITER_HH_

#ifdef _BIG_ENDIAN_
#define _BYTE_SWAP(x) (x)
#else // Little endian
#define _BYTE_SWAP(x) (uint32_t)(((x) << 24) + (((x)&0xff00) << 8) \
        + (((x) >> 8)&0xff00) + ((x) >> 24))
#endif

#include "codecdefs.h"

class CBitStreamWriter
{
public:
    CBitStreamWriter(unsigned char * a_outBuff);
    CBitStreamWriter();
    ~CBitStreamWriter() {};

    void Init(unsigned char * a_outBuff);
    void PutBits(unsigned long val, int n);
    void WriteLeftOverBits();
    void FlushToWordBoundary();
    void PadToByteBoundary();

    uint64_t GetByteCount();
    uint64_t GetBitCount();
    uint8_t * GetStartPointer();

    void GoulombWriteUE(unsigned int val);
    void GoulombWriteSE(int val);
    void GoulombWriteTE(int x, int val);

private:
    int32_t m_bit_offset;
    uint8_t *m_start_pointer;
    uint32_t *m_current_pointer;

};

inline CBitStreamWriter::CBitStreamWriter(unsigned char * a_outBuff)
{
    Init(a_outBuff);
}

inline CBitStreamWriter::CBitStreamWriter()
{
    Init(NULL);
}

inline void CBitStreamWriter::Init(unsigned char * a_outBuff)
{
    m_start_pointer = a_outBuff;
    if (m_start_pointer != NULL)
        *(uint32_t *)m_start_pointer = 0;
    m_bit_offset = 32;
    m_current_pointer = (uint32_t *)m_start_pointer;
}

inline void CBitStreamWriter::PutBits(unsigned long val, int len)
{
    int32_t tmpcnt = m_bit_offset - len;
    if (tmpcnt < 0) {
        uint32_t r_tmp = m_current_pointer[0] | (val >> (-tmpcnt));
        m_current_pointer[0] = _BYTE_SWAP(r_tmp);
        m_current_pointer++;
        m_current_pointer[0] = val << (32 + tmpcnt);
        m_bit_offset = 32 + tmpcnt;
    }
    else {
        m_current_pointer[0] |= val << tmpcnt;
        m_bit_offset = tmpcnt;
    }
}

inline void CBitStreamWriter::PadToByteBoundary()
{
    m_bit_offset &= ~0x7;
}

inline void CBitStreamWriter::WriteLeftOverBits()
{
    if (m_bit_offset != 32) {
        m_current_pointer[0] = _BYTE_SWAP(m_current_pointer[0]);
        m_bit_offset &= ~7;
    }
}

inline void CBitStreamWriter::FlushToWordBoundary()
{
    // Write remaining bits and flush to 4 byte boundary
    WriteLeftOverBits();
    m_bit_offset = 0;
}

inline uint8_t * CBitStreamWriter::GetStartPointer() {
    return m_start_pointer;
}

inline uint64_t CBitStreamWriter::GetByteCount() {
    return (uint64_t)((uint8_t *)m_current_pointer - (uint8_t *)m_start_pointer) +
        ((32 - m_bit_offset) >> 3);
}

inline uint64_t CBitStreamWriter::GetBitCount() {
    return (uint64_t)((uint8_t *)m_current_pointer - (uint8_t *)m_start_pointer) * 8 +
        (32 - m_bit_offset);
}

inline void CBitStreamWriter::GoulombWriteUE(unsigned int val )
{
    int i_size = 0;
    static const int i_size0_255[256] =
    {
        1,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
    };

    if( val == 0 ) {
        PutBits(1, 1);
    }
    else {
        unsigned int tmp = ++val;

        if( tmp >= 0x00010000 ) {
            i_size += 16;
            tmp >>= 16;
        }
        if( tmp >= 0x100 ) {
            i_size += 8;
            tmp >>= 8;
        }
        i_size += i_size0_255[tmp];

        PutBits(val, 2 * i_size - 1);
    }
}

inline void CBitStreamWriter::GoulombWriteSE(int val )
{
    GoulombWriteUE(val <= 0 ? -val * 2 : val * 2 - 1);
}

inline void CBitStreamWriter::GoulombWriteTE(int x, int val )
{
    if (x == 1) {
        PutBits(val, 1);
    }
    else if(x > 1) {
        GoulombWriteUE(val );
    }
}




// This class used for collecting statistics from a CBitStreamWriter
#define NUMB_BUCKETS 16
class CBitStreamStats
{
public:
    CBitStreamStats();
    ~CBitStreamStats();

    void SetBitStream(CBitStreamWriter * pBitStream);
    void FillBucket(int bucket);
    void FillBucketToo(int bucket);
    void FillCounter(int bucket);
    void AddtoBucket(int bucket, uint32_t value);
    uint64_t GetTotal();
    uint64_t GetBucket(int idx);
    uint64_t GetBucketToo(int idx);
    uint64_t GetCounter(int idx);

private:
    CBitStreamWriter * m_pBitStream;
    uint64_t m_nBitBuckets[NUMB_BUCKETS];
    uint64_t m_nBitBucketsToo[NUMB_BUCKETS];
    uint64_t m_nCounter[NUMB_BUCKETS];
    uint64_t m_nLastBitCount;
    uint64_t m_nLastBitTooCount;
};

inline CBitStreamStats::CBitStreamStats() {
    m_pBitStream = 0;
    for (int i = 0; i < NUMB_BUCKETS; i++) {
        m_nBitBuckets[i] = 0;
        m_nBitBucketsToo[i] = 0;
        m_nCounter[i] = 0;
    }
    m_nLastBitCount = 0;
    m_nLastBitTooCount = 0;
}

inline CBitStreamStats::~CBitStreamStats() {
}

inline void CBitStreamStats::SetBitStream(CBitStreamWriter * pBitStream) {
    m_pBitStream = pBitStream;
    m_nLastBitCount = m_pBitStream->GetBitCount();
    m_nLastBitTooCount = m_pBitStream->GetBitCount();
}

inline void CBitStreamStats::FillBucket(int bucket) {
#ifdef COLLECT_STATS
    if (m_pBitStream == NULL)
        return;
    m_nBitBuckets[bucket] += m_pBitStream->GetBitCount() - m_nLastBitCount;
    m_nLastBitCount = m_pBitStream->GetBitCount();
#endif
}

inline void CBitStreamStats::AddtoBucket(int bucket, uint32_t value) {
#ifdef COLLECT_STATS
    m_nBitBuckets[bucket] += value;
#endif

}

inline void CBitStreamStats::FillBucketToo(int bucket) {
#ifdef COLLECT_STATS
    if (m_pBitStream == NULL)
        return;
    m_nBitBucketsToo[bucket] += m_pBitStream->GetBitCount() - m_nLastBitTooCount;
    m_nLastBitTooCount = m_pBitStream->GetBitCount();
#endif
}

inline void CBitStreamStats::FillCounter(int bucket) {
#ifdef COLLECT_STATS
    m_nCounter[bucket]++;
#endif
}

inline uint64_t CBitStreamStats::GetTotal() {
    uint64_t total = 0;
    for (int i = 0; i < NUMB_BUCKETS; i++) {
        total += m_nBitBuckets[i];
    }
    return total;
}

inline uint64_t CBitStreamStats::GetBucket(int idx) {
    return m_nBitBuckets[idx];
}

inline uint64_t CBitStreamStats::GetBucketToo(int idx) {
    return m_nBitBucketsToo[idx];
}

inline uint64_t CBitStreamStats::GetCounter(int idx) {
    return m_nCounter[idx];
}

#endif /* BITSTREAMWRITER_HH_ */
