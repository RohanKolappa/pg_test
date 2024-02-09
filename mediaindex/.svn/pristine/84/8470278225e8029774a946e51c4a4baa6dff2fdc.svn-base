#ifndef _MEDIASERVER_HH
#define _MEDIASERVER_HH

#include <list>
#include <string>
#include <vector>

//using namespace std;

#include "MediaIndexUtils.hh"
#include "MediaFlags.hh"

#define MSRC_OK                 0
#define MSRC_NO_RESOURCES       1
#define MSRC_MEDIA_ERROR        2
#define MSRC_END_OF_MEDIA       3
#define MSRC_OBJECT_ERROR       4

namespace mediaindex {

// Class representing data returned by the MediaServer's GetFrame method
class CCFrame {
public:
    enum MessageType {
        Normal = 1,
        MediaStarted,
        MediaStopped,
        Snapshot,
        Thumbnail,
    };

    CCFrame()
    : m_nLength(0),
      m_nFlags(0),
      m_nMessageType(Normal),
      m_nTimestamp(0),
      m_nTimeOffset(0),
      m_pData(NULL)

	{
	};

    CCFrame(uint8_t *a_pData,
            uint32_t a_nLength,
            uint32_t a_nFlags,
            uint64_t a_nTimeStamp)
    : m_nLength(a_nLength),
      m_nFlags(a_nFlags),
      m_nMessageType(Normal),
      m_nTimestamp(a_nTimeStamp),
      m_nTimeOffset(0),
      m_pData(a_pData)
    {
    };

    ~CCFrame() {};

    MessageType GetMessageType() const { return m_nMessageType; };
    uint64_t GetTimeStamp() const { return m_nTimestamp; };
    uint64_t GetTimeOffset() const { return m_nTimeOffset; };
    uint32_t GetMediaFlag() const { return m_nFlags; };
    uint32_t GetLength() const { return m_nLength; };
    uint8_t * GetData() { return m_pData;};

	unsigned long GetFrameVersion() const { return (CStreamMediaFlag(m_nFlags).GetFrameVersion()); };
	unsigned long GetStreamType() const { return (CStreamMediaFlag(m_nFlags).GetStreamType()); };
    unsigned long GetStreamType(unsigned long  a_StreamType) {return ((GetFrameVersion() == 0) ? a_StreamType : (CStreamMediaFlag(m_nFlags).GetStreamType())); };

    bool HasDiscontinuity() const { return false; };

    void SetMessageType(MessageType a_nMessageType) { m_nMessageType = a_nMessageType; };
    void SetTimeStamp(uint64_t a_nTimestamp) { m_nTimestamp = a_nTimestamp; };
    void SetTimeOffset(uint64_t a_nTimeOffset) { m_nTimeOffset = a_nTimeOffset; };
    void SetMediaFlag(uint32_t a_nFlags)  { m_nFlags = a_nFlags; };
    void SetLength(uint32_t a_nLength)  { m_nLength = a_nLength; };
    void SetData(uint8_t * a_pData) { m_pData = a_pData; };


private:
    uint32_t            m_nLength;
    uint32_t            m_nFlags;
    MessageType         m_nMessageType;
    uint64_t            m_nTimestamp;
    uint64_t            m_nTimeOffset;
    uint8_t *           m_pData;
};

class CGap {
public:
    CGap(uint64_t a_nBegin, uint64_t a_nEnd) {
        m_nBegin = a_nBegin;
        m_nEnd = a_nEnd;
    }
    uint64_t GetBeginTime() {
        return m_nBegin;
    }
    uint64_t GetEndTime() {
        return m_nEnd;
    }
    void SetBeginTime(uint64_t a_nBegin) {
        m_nBegin = a_nBegin;
    }
    void SetEndTime(uint64_t a_nEnd) {
        m_nEnd = a_nEnd;
    }
    uint64_t GetLength() {
        if (m_nEnd < m_nBegin)
            return 0;
        return m_nEnd - m_nBegin;
    }
private:
    uint64_t m_nBegin;
    uint64_t m_nEnd;
};

class CImageFileInfo {
public:
    CImageFileInfo(uint64_t nTimestamp, int nType, std::string fileName) :
    m_nTimestamp(nTimestamp), m_nType(nType), m_fileName(fileName)
    {}
    CImageFileInfo() :
    m_nTimestamp(0), m_nType(0), m_fileName("")
    {};
    uint64_t m_nTimestamp;
    int m_nType;
    std::string m_fileName;
};


// MediaServer Abstract class that provides an interface to retrieve frames
// from IPVS specific media files.
class CMediaServer {
public:
    static CMediaServer * Create(const char* a_mediaDirectory);
    virtual ~CMediaServer();

    virtual int Init() = 0;
    virtual int Start(uint64_t startTimeCode) = 0;
    virtual int Stop() = 0;
    virtual int Abort() = 0;
    virtual bool HasThumbnails();
    virtual bool IsSnapshot();
    void ServeThumbnails(bool bEnable);

	virtual int GetFrame(CCFrame &a_cFrame) = 0;


	virtual uint64_t GetMediaStartTimestamp() = 0;
	virtual uint64_t GetMediaEndTimestamp() = 0;
	virtual uint64_t GetTimecodeAtOffset(uint64_t a_nOffset) = 0;
	virtual uint64_t GetOffsetFromTimecode(uint64_t a_nTimecode) = 0;
    virtual uint64_t GetMediaDuration() = 0;
    virtual uint64_t GetMediaSize(uint64_t a_nBeginOffset, uint64_t a_nEndOffset) = 0;


	virtual std::list <CGap> GetGaps() = 0;

	const char *GetErrorString();

    // Functions to get debug/stat info
	unsigned long GetReadOperations() { return m_readOperations;};

protected:
    enum PlayState {
        Stopped = 1,
        Started,
        Paused
    };

    char * m_mediaPath;
    char * m_errorString;
    PlayState m_eState;
    uint32_t  m_readOperations;
    bool m_bServeThumbnails;
    bool m_bHaveThumbnails;
    int m_nImageFrameBufferSize;
    uint8_t * m_pImageFrameBuffer;

    void SetErrorString(const char *a_errorString);
    int GetImageType(const char *filename);
    int ReadImageFile(std::string fileName);

    CMediaServer(const char* a_mediaDirectory);

private:

    static bool MediaDirHasSnapshot(const char* a_mediaPath);
};

};

#endif // _MEDIASERVER_HH
