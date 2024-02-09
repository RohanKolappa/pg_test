#ifndef CYUV4MPEG2WRITER__H_
#define CYUV4MPEG2WRITER__H_ 

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using std::string;
using std::ofstream;
using std::ostream;
using std::ios_base;
using std::cout;

class CYUV4MPEG2HEADER
{
public:
    unsigned short m_nWidth;
    unsigned short m_nHeight;
    unsigned short m_nFPSNum;
    unsigned short m_nFPSDen;
    unsigned short m_nPARNum;
    unsigned short m_nPARDen;
    string m_strChromaFormat;
    char m_cInterlacing;
};

class CYuv4Mpeg2Writer 
{
public:
    CYuv4Mpeg2Writer(const string& a_rStrFilename); 
    ~CYuv4Mpeg2Writer();
    bool WriteHeader(const CYUV4MPEG2HEADER& a_rHeader);
    bool WriteFrame(const unsigned char* a_pucYUVFrame, 
                    const size_t a_nFrameLen);

private:
    string      m_strFilename;
    ofstream    m_OutFile;    
    int         m_nSavedTranslationMode;
};

#endif //CYUV4MPEG2WRITER__H_ 
