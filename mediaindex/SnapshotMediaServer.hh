/*
 * SnapshotMediaServer.hh
 *
 *  Created on: Sep 27, 2012
 *      Author: rkale
 */

#ifndef SNAPSHOTMEDIASERVER_HH_
#define SNAPSHOTMEDIASERVER_HH_

#include "MediaServer.hh"


namespace mediaindex {

class CSnapshotMediaServer : public CMediaServer {
public:
    CSnapshotMediaServer(const char* a_mediaPath);
    virtual ~CSnapshotMediaServer();

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
    uint64_t    m_nStartTimestamp;
    bool m_bParsed;
    bool m_bServePT;
    int m_nCurrentFrameIndex;
    std::vector <CImageFileInfo> m_lSnapshotInfo;
    std::string m_ThumbnailDir;

    int ParseIndexFile();
    void InitThumbnails();
    bool ServePrimaryThumbnail(CCFrame & a_cFrame);

};

};

#endif /* SNAPSHOTMEDIASERVER_HH_ */
