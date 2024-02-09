#ifndef MEDIAWRITER_CWRAPPER__HH
#define MEDIAWRITER_CWRAPPER__HH

#include <stdint.h>

struct mediawriterWrapper_t;

struct IPVSHeaderInfo
{
    uint64_t nWallClock;
    uint32_t nFlags;
};

union uData 
{
    uint64_t i64;
    uint32_t i32[2];
};

#ifdef __cplusplus
extern "C" {
#endif
    
    struct mediawriterWrapper_t * createMediaWriter(char* a_pDestPath);
    void destroyMediaWriter(struct mediawriterWrapper_t * a_pMediaWriter);

    void startMediaWriter(struct mediawriterWrapper_t * pWriter, uint64_t nStartTime, int nHLS, int nTN);
    void stopMediaWriter(struct mediawriterWrapper_t * pWriter,  uint64_t nStopTime);
    void putMediaFrame(struct mediawriterWrapper_t * pWriter, uint8_t* pData, int nSize, uint64_t nTimeStamp, unsigned long nMediaFlags);

    void startMediaWriterForHLS(struct mediawriterWrapper_t * pWriter, uint64_t nStartTime, char *pBlobDir, char *pSDPFilePath);
    void stopMediaWriterForHLS(struct mediawriterWrapper_t * pWriter, uint64_t nStopTime);
    void putMediaFrameForHLS(struct mediawriterWrapper_t *pWriter, uint8_t *pData, int nSize, uint64_t nTimeStamp, unsigned long nMediaFlags);

    void startMediaWriterForTN(struct mediawriterWrapper_t * pWriter, uint64_t nStartTime, char *pBlobDir, char *pSDPFilePath);
    void stopMediaWriterForTN(struct mediawriterWrapper_t * pWriter, uint64_t nStopTime);
    void putMediaFrameForTN(struct mediawriterWrapper_t * pWriter, uint8_t * pData, int nSize, uint64_t nTimeStamp, unsigned long nMediaFlags);
 
    uint32_t getRTPMediaFlags(int nVideo);

    uint64_t htonll (uint64_t hostFormatInt64);
    uint64_t ntohll (uint64_t networkFormatInt64);

#ifdef __cplusplus
}
#endif

#endif // MEDIAWRITER_CWRAPPER__HH
