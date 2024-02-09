
#include "v2dlib_compatibility.h"
//#include <stdlib.h>
#include "vframedecoder.hh"


#ifdef __cplusplus
extern "C" {
#endif

    typedef struct v2dParser {
        uint8_t* pFrameBuf;
        size_t   nIndex;
        uint32_t nState;
        uint32_t nSOF;
    } v2dParser_t, *v2dParserHandle;

    v2dParserHandle v2d_parser_init();

    int v2d_parse_frame(v2dParserHandle a_hParser, 
            const uint8_t** a_ppOutBuf, int* a_pnOutBufSize,
            const uint8_t* a_pInBuf, const int a_nInBufSize);

    void v2d_parser_close(v2dParserHandle a_hParser);

#ifdef __cplusplus
}
#endif


