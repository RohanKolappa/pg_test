/*
 * MediaWriter.hh
 *
 *  Created on: Feb 24, 2010
 *      Author: rkale
 */

#ifndef MEDIAWRITER_HH_
#define MEDIAWRITER_HH_

#include "MediaIndexUtils.hh"
#include "MediaFlags.hh"

#include <string>

namespace mediaindex {

// MediaWriter class that provides an interface to write frames
// into IPVS specific media files. Accepts individual time sequenced
// frames and creates appropriate files in blob directory
// (minute files, index files, gaps).

class CMediaWriter {
public:
    CMediaWriter(const char* a_mediaDirectory);
    ~CMediaWriter();

    int Init(uint64_t trimLength = 0, bool syncWrites = false);
    int Start(uint64_t aTimeCode);
    int Stop(uint64_t aTimeCode);
    void SetAppendMode(bool appendMode);
    void SetDropBuffers(bool dropBuffers);

    int PutFrame(CCFrame & a_cFrame);
    int AddSnapshot(CCFrame & a_cFrame);
    int AddThumbnail(CCFrame & a_cFrame);
    void EnableThumbnails(bool bEnable);
    void EnableMedia(bool bEnable);

    const char *GetErrorString() {
        return m_errorString;
    }

    const char *GetMediaDirectory() {
        return m_mediaDirectory;
    }
    uint64_t GetBytesWritten();
    uint64_t GetBytesDeleted();

private:
    enum PlayState {
        Stopped = 1,
        Started,
        Errored
    };

    char * m_mediaDirectory;
    char * m_errorString;
    DestIndexInterface* m_pDestinationIndex;
    DestIndexInterface* m_pTNDestinationIndex;
    uint8_t * m_pWriteBuffer;
    int m_nBufferSize;
    int m_nSnapshotIndex;
    PlayState m_eState;
    uint64_t m_nTimestamp;
    uint64_t m_nFileStartTime;
    uint64_t m_nIndexTimestamp;
    uint64_t m_nLastWriteTime;
    uint64_t m_nMediaStartTime;

    uint32_t m_nFileRemainingLength;
    uint32_t m_nIndexOffset;
    uint32_t m_nLastIndexOffset;

    int m_nWriteOffset;
    int m_nFileFD;
    int m_nIndexFileFD;
    int m_nSync;

    char * m_pFileName;
    char * m_pIndexFileName;
    std::string  m_ThumbnailDir;

    uint64_t m_nWriteOperations;
    uint64_t m_nBytesWritten;
    bool m_bAppendMode;
    bool m_bDropBuffers;
    bool m_bInitThumbnails;
    bool m_bDoThumbnails;
    bool m_bDoMedia;

    int CleanMediaDirectory();
    void SetErrorString(const char *a_errorString);
    int CheckFile();
    int FlushBuffer();
    uint64_t GetCurrentTimeStamp();
    int GetNextFile(uint64_t a_nTimestamp);
    void CloseFiles();
    int InitThumbnails();
    void WriteFileFromBuffer(std::string fileName,
            uint8_t * buffer, int bufferLen, bool append = false);
    void UpdateIndexFile(std::string tnFileName, uint64_t nTimestamp);
    std::string GetImageExtension(int imageType);
    int InitMedia(uint64_t trimLength);
    int StartMedia(uint64_t aTimeCode);
    int StopMedia(uint64_t aTimeCode);
    int StartThumbnails(uint64_t nTimecode);
    int StopThumbnails(uint64_t aTimeCode);

};

};
#endif /* MEDIAWRITER_HH_ */
