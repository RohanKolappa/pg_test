#include "v2dlib_compatibility.h"


#include "test_utils.hh"
#include "../colortransform.h"
#include "../bitmap.hh"

using namespace V2dLibColorTransform;

namespace TestUtils {


void WriteYUVToBmpFile(const unsigned char *a_pYUV, const size_t a_nWidth,
        const size_t a_nHeight, YUVToRGBConverter::SubSample a_nSubsample,
        const std::string a_strFile)
{
    YUVToRGBConverter oRGB;

    const unsigned char* pY = a_pYUV;
    const unsigned char* pU = pY + (a_nWidth * a_nHeight);
    const unsigned char* pV = pU + (a_nWidth * a_nHeight) / 2;

    unsigned char* pRGB = new unsigned char[a_nWidth * a_nHeight * 3];
    unsigned char* pRGBA = new unsigned char[a_nWidth * a_nHeight * 4];

    oRGB.yuvToRgb(pY, pU, pV, pRGB, a_nWidth, a_nHeight, a_nSubsample);

    ConvertRGBToRGBA(pRGB, pRGBA, a_nWidth, a_nHeight);

    CBitmap oSaveBmp;
    oSaveBmp.SetBitsRGBA(pRGBA, a_nWidth, a_nHeight);
    oSaveBmp.Save(a_strFile.c_str(), 32);

    delete[] pRGB;
    delete[] pRGBA;
}


}

// End namespace
