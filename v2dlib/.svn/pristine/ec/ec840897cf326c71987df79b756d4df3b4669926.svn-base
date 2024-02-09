#include "yuv4mpeg2writer.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <io.h>
#include <fcntl.h>
#endif

CYuv4Mpeg2Writer::CYuv4Mpeg2Writer(const string& a_rStrFilename)
: m_strFilename(a_rStrFilename)
, m_nSavedTranslationMode(0)
{
    if(m_strFilename == "-")
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
        m_nSavedTranslationMode = _setmode(_fileno(stdout), _O_BINARY);
#endif      
    }
    else
    {
        m_OutFile.open(m_strFilename.c_str(), ios_base::out | ios_base::binary);
    }
}

CYuv4Mpeg2Writer::~CYuv4Mpeg2Writer()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    if(m_strFilename == "-")
    {
        _setmode(_fileno(stdout), m_nSavedTranslationMode);
    }
#endif
}

bool CYuv4Mpeg2Writer::WriteHeader(const CYUV4MPEG2HEADER& a_rHeader)
{
    ostream& rstream = (m_strFilename == "-") ? cout : m_OutFile;
    if(rstream)
    {
        rstream << "YUV4MPEG2";
        rstream << " " << "W" << a_rHeader.m_nWidth;
        rstream << " " << "H" << a_rHeader.m_nHeight;
        rstream << " " << "F" << a_rHeader.m_nFPSNum << ":" << 
            a_rHeader.m_nFPSDen;
        rstream << " " << "l" << a_rHeader.m_cInterlacing;
        rstream << " " << "A" << a_rHeader.m_nPARNum << ":" <<
            a_rHeader.m_nPARNum;
        rstream << " " << "C" << a_rHeader.m_strChromaFormat;
        rstream << "\n";
        return true;
    }
    else
    {
        return false;
    }

}

bool CYuv4Mpeg2Writer::WriteFrame(const unsigned char* a_pucYUVFrame, 
                                    const size_t a_nFrameLen)
{
    ostream& rstream = (m_strFilename == "-") ? cout : m_OutFile;
    if(rstream)
    {
        rstream << "FRAME" << "\n";
        rstream.write(reinterpret_cast<const char *>(a_pucYUVFrame), 
            a_nFrameLen);
        return true;
    }
    else
    {
        return false;
    }

}

