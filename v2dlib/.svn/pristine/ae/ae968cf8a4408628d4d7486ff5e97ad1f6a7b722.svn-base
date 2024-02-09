#include "v2dlib_compatibility.h"
#include "vframedecoder_c.h"

v2dDecHandle v2d_decode_init(v2dDecConfigurationPtr a_pDecConfig)
{
    CVideoFrameDecoder* pFrameDecoder = 
        CVideoFrameDecoder::CreateObj(a_pDecConfig);
    return (v2dDecHandle)(pFrameDecoder);
}

int v2d_decode_frame(v2dDecHandle a_hDecoder, 
        unsigned char* a_pEncData, const size_t a_nEncDataLen, 
        unsigned char* a_pDecData[], const size_t a_nStride[])
{
//    CVideoFrameDecoder* pFrameDecoder = (CVideoFrameDecoder*)(a_hDecoder);
//    return pFrameDecoder->Decode(a_pEncData, a_nEncDataLen, a_pDecData,
//            a_nStride);
    return 0;
}

void v2d_decode_close(v2dDecHandle a_hDecoder)
{
    delete (CVideoFrameDecoder*)(a_hDecoder);
}

