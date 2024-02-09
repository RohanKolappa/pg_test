#ifndef _INDEX_INTERFACE_HH
#define _INDEX_INTERFACE_HH

#include "CommonIncludes.hh"

#define INDEX_ERROR 0
#define INDEX_OK  1
#define INDEX_RETRY  2

namespace mediaindex {

/*!
  \class SourceIndexInterface an interface to be implemented by all sourceIndex
*/
class SourceIndexInterface {
public:
    virtual ~SourceIndexInterface() {}
    virtual int init(char *mediaID, char *homedir) = 0;
    virtual int start(uint64_t startTime, uint64_t startTimeCode) = 0;
    virtual int stop() = 0;
    virtual int getNext(uint64_t lastTimeCodeRead,
                        int64_t lastbyteoffset,
                        uint64_t *pbegintimecode,
                        uint64_t *pendtimecode,
                        char **pfilename,
                        uint64_t *pnexttimecode,
                        uint64_t *poffset,
                        uint64_t *plength,
                        int *pdiscontinuity,
                        int *eom) = 0;
    virtual char *getLastError() = 0;
    virtual int getTimecodeAtOffset(uint64_t offset, uint64_t *timecode) = 0;
    virtual int getOffsetFromTimecode(uint64_t timecode, uint64_t *offset) = 0;
    virtual void dumpGapInfo() = 0;
    virtual int getBeginTimecode(uint64_t *timecode) = 0;
    virtual int getEndTimecode(uint64_t *timecode) = 0;
    virtual int getNextGap(uint64_t timecode, uint64_t *begintimecode, uint64_t *endtimecode) = 0;

};

/*!
  \class DestIndexInterface an interface to be implemented by all destIndex
*/
class DestIndexInterface {
public:
    virtual ~DestIndexInterface() {}
    virtual int init(char *mediaID, char *homedir, uint64_t trimLength) = 0;
    virtual int start(uint64_t startTime, uint64_t startTimeCode) = 0;
    virtual int stop(uint64_t stopTimeCode) = 0;
    virtual int getNext(uint64_t timeCode, char **pfilename, uint64_t *poffset, uint64_t *plength) = 0;
    virtual char *getLastError() = 0;
    virtual int getTimecodeAtOffset(uint64_t offset, uint64_t *timecode) = 0;
    virtual int getOffsetFromTimecode(uint64_t timecode, uint64_t *offset) = 0;
    virtual int getBeginTimecode(uint64_t *timecode) = 0;
    virtual int getEndTimecode(uint64_t *timecode) = 0;
    virtual int getNextGap(uint64_t timecode, uint64_t *begintimecode, uint64_t *endtimecode) = 0;
    virtual int getProgress(bool & inProgress) = 0;
    virtual uint64_t getBytesDeleted() = 0;
};


};

#endif
