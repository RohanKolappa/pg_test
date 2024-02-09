#ifndef _CLIPDESTINDEX_HH
#define _CLIPDESTINDEX_HH

#include "IndexInterface.hh"
#include "IndexData.hh"

namespace mediaindex {

class ClipDestIndex : public DestIndexInterface {
private:
    IndexData *m_pIndexData;
    uint64_t m_nTrimLength;
    uint64_t m_nBytesDeleted;

    int trimBlob(uint64_t begin, uint64_t end);
    int trim(uint64_t timeCode);
    int writeIndexFile();

public:
    ClipDestIndex();
    virtual ~ClipDestIndex();
    virtual int init(char *mediaID,
                     char *homedir,
                     uint64_t trimLength);
    virtual int start(uint64_t startTime, uint64_t startTimeCode);
    virtual int stop(uint64_t stopTimeCode);
    virtual int getNext(uint64_t timeCode, char **pfilename,
                        uint64_t *poffset, uint64_t *plength);
    virtual char *getLastError();
    virtual int getTimecodeAtOffset(uint64_t offset, uint64_t *timecode);
    virtual int getOffsetFromTimecode(uint64_t timecode, uint64_t *offset);
    virtual int getBeginTimecode(uint64_t *timecode);
    virtual int getEndTimecode(uint64_t *timecode);
    virtual int getNextGap(uint64_t timecode, uint64_t *begintimecode, uint64_t *endtimecode);
    virtual int getProgress(bool & inProgress);
    virtual uint64_t getBytesDeleted();
};

};

#endif
