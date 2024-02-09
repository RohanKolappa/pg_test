#ifndef _CLIPSOURCEINDEX_HH
#define _CLIPSOURCEINDEX_HH

#include "IndexInterface.hh"
#include "IndexData.hh"

namespace mediaindex {

class ClipSourceIndex : public SourceIndexInterface {
private:
    IndexData *m_pIndexData;
    uint64_t m_nStartTimeCode;
    int m_bDiscontinuity;
    int m_nIndexFileTimestamp;
    int refreshIndex();
    virtual int getFileOffset(uint64_t offset, char **pfilename,
                              uint64_t *poffset);


public:
    ClipSourceIndex();
    virtual ~ClipSourceIndex();
    virtual int init(char *mediaID, char *homedir);
    virtual int start(uint64_t startTime, uint64_t startTimeCode);
    virtual int stop();
    virtual int getNext(uint64_t lastTimeCodeRead, int64_t lastbyteoffset,
                        uint64_t *pbegintimecode, uint64_t *pendtimecode,
                        char **pfilename, uint64_t *pnexttimecode,
                        uint64_t *poffset, uint64_t *plength,
                        int *pdiscontinuity, int *eom);
    virtual char *getLastError();
    virtual int getTimecodeAtOffset(uint64_t offset, uint64_t *timecode);
    virtual int getOffsetFromTimecode(uint64_t timecode, uint64_t *offset);
    virtual int getBeginTimecode(uint64_t *timecode);
    virtual int getEndTimecode(uint64_t *timecode);
    virtual void dumpGapInfo();
    virtual int getNextGap(uint64_t timecode, uint64_t *begintimecode, uint64_t *endtimecode);
};

};

#endif
