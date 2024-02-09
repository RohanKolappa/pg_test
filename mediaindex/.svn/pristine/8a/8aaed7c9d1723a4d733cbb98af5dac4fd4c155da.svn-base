
#include "CommonIncludes.hh"
#include "MediaServer.hh"
#include "MinuteFileMediaServer.hh"
#include "MatroskaMediaServer.hh"
#include "SnapshotMediaServer.hh"
#include "MediaHeader.hh"
#include "FileSystemUtils.hh"

#include <string>
using namespace std;

namespace mediaindex {

CMediaServer * CMediaServer::Create(const char* a_mediaPath)
{
    if (IsDirectory(a_mediaPath)) {
        if (MediaDirHasSnapshot(a_mediaPath))
            return new CSnapshotMediaServer(a_mediaPath);
        else
            return new CMinuteFileMediaServer(a_mediaPath);
    }
    else {
        return new CMatroskaMediaServer(a_mediaPath);
    }
}

CMediaServer::CMediaServer(const char* a_mediaPath)
{
    m_mediaPath = strdup(a_mediaPath);
    m_errorString = strdup("");
    m_eState = Stopped;
    m_bServeThumbnails = false;
    m_bHaveThumbnails = false;
    m_nImageFrameBufferSize = 0;
    m_pImageFrameBuffer = NULL;

}


CMediaServer::~CMediaServer()
{
    delete [] m_pImageFrameBuffer;
    free(m_mediaPath);
    free(m_errorString);
}

void CMediaServer::ServeThumbnails(bool bEnable)
{
    m_bServeThumbnails = bEnable;
}

bool CMediaServer::HasThumbnails()
{
    return m_bHaveThumbnails;
}

bool CMediaServer::IsSnapshot() {
    return false;
}

const char *CMediaServer::GetErrorString() {
    return m_errorString;
}

void CMediaServer::SetErrorString(const char *a_errorString)
{

    if (m_errorString)
        free(m_errorString);
    m_errorString = strdup(a_errorString);
}

bool CMediaServer::MediaDirHasSnapshot(const char* a_mediaPath)
{
    string indexfile(a_mediaPath);
    indexfile.append(FILESEPERATOR);
    indexfile.append("SSINDEX");

    struct stat statbuf;
    if (LSTAT(indexfile.c_str(), &statbuf) == -1)
        return false;

    return true;
}

int CMediaServer::GetImageType(const char *filename) {
    int imageType = IMAGETYPE_UNKNOWN;
    const char *cp = strrchr(filename, '.');
    if (cp != NULL) {
        if (strcasecmp(cp, ".jpg") == 0)
            imageType = IMAGETYPE_JPEG;
        if (strcasecmp(cp, ".png") == 0)
            imageType = IMAGETYPE_PNG;
        if (strcasecmp(cp, ".bmp") == 0)
            imageType = IMAGETYPE_BMP;
        if (strcasecmp(cp, ".gif") == 0)
            imageType = IMAGETYPE_GIF;
        if (strcasecmp(cp, ".tif") == 0)
            imageType = IMAGETYPE_TIFF;
    }
    return imageType;
}

int CMediaServer::ReadImageFile(std::string fileName)
{
    // Find size of file
    struct stat statbuf;
    if (LSTAT(fileName.c_str(), &statbuf) == -1) {
        return -1;
    }

    // Allocate buffer if needed
    int nBufferSize = statbuf.st_size;
    if (nBufferSize > m_nImageFrameBufferSize || m_pImageFrameBuffer == NULL) {
        delete [] m_pImageFrameBuffer;
        m_pImageFrameBuffer = new uint8_t[nBufferSize];
        m_nImageFrameBufferSize = nBufferSize;
    }

    // Read file into buffer;
    FILE *fp = NULL;

#ifdef WINDOWS
    fp = fopen(fileName.c_str(), "rb");
#else
    fp = fopen(fileName.c_str(), "r");
#endif

    if (fp == NULL) {
        return -1;
    }

    fread(m_pImageFrameBuffer, nBufferSize, 1, fp);

    fclose(fp);

    return nBufferSize;
}

}
