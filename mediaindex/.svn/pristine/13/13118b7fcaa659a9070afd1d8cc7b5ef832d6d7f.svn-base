/*
 * MatroskaMediaServer.hh
 *
 *  Created on: Dec 15, 2011
 *      Author: rkale
 */

#ifndef MATROSKAMEDIASERVER_HH_
#define MATROSKAMEDIASERVER_HH_

#include "MediaServer.hh"

namespace mediaindex {

class CMatroskaFileParser;

class CMatroskaMediaServer : public CMediaServer {
public:
    CMatroskaMediaServer(const char* a_mediaPath);
    ~CMatroskaMediaServer();

    int Init();
    int Start(uint64_t startTimeCode);
    int Stop();
    int Abort();
    bool IsSnapshot();

    int GetFrame(CCFrame &a_cFrame);

    uint64_t GetMediaStartTimestamp();
    uint64_t GetMediaEndTimestamp();
    uint64_t GetTimecodeAtOffset(uint64_t a_nOffset);
    uint64_t GetOffsetFromTimecode(uint64_t a_nTimecode);
    uint64_t GetMediaDuration();
    uint64_t GetMediaSize(uint64_t a_nBeginOffset, uint64_t a_nEndOffset);

    std::list <CGap> GetGaps();

private:
    CMatroskaFileParser *m_pMatroskaParser;

};

};

#endif /* MATROSKAMEDIASERVER_HH_ */
