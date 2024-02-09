#include <stdlib.h>
#include "vframedecoder.hh"

typedef void* v2dDecHandle;

#ifdef __cplusplus
extern "C" {
#endif

    v2dDecHandle v2d_decode_init(v2dDecConfigurationPtr a_pDecConfig);

    int v2d_decode_frame(v2dDecHandle a_hDecoder, 
            unsigned char* a_pEncData, const size_t a_nEncDataLen, 
            unsigned char* a_pDecData[], const size_t a_nStride[]);

    void v2d_decode_close(v2dDecHandle a_hDecoder);

#ifdef __cplusplus
}
#endif


