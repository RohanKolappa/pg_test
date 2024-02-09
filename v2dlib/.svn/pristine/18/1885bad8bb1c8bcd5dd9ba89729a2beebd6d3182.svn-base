#include "vframeparser_c.h"
#if (defined(WIN32) || defined(WINDOWS))
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

v2dParserHandle v2d_parser_init()
{
    v2dParserHandle pParser = (v2dParserHandle) malloc(sizeof(v2dParser_t));

    if(pParser)
        memset(pParser, 0, sizeof(v2dParser_t));

    pParser->pFrameBuf = (unsigned char*)malloc(1000000);
    return pParser;
}

int v2d_parse_frame(v2dParserHandle a_hParser, 
        const uint8_t** a_ppOutBuf, int* a_pnOutBufSize,
        const uint8_t* a_pInBuf, const int a_nInBufSize)
{
    int nLen = 0;
    const uint8_t *buff;

    if(a_hParser->nSOF) {
    	a_hParser->nState = htonl(a_hParser->nState);
    	memcpy(a_hParser->pFrameBuf + a_hParser->nIndex, &a_hParser->nState, 4);
    	a_hParser->nIndex += 4;
    }

    a_hParser->nSOF = 0;

    while (nLen < a_nInBufSize)
    {
        buff = a_pInBuf + nLen;
        if( ISSOFCODE(buff) ) {
            a_hParser->nState = htonl( *(const uint32_t *)(buff) );
            a_hParser->nSOF   = 1;
            break;
        }

        nLen += 4;
    }

    if(!a_hParser->nSOF) {
    	memcpy(a_hParser->pFrameBuf + a_hParser->nIndex, a_pInBuf, 
            a_nInBufSize);

    	a_hParser->nIndex += a_nInBufSize;
    	*a_ppOutBuf     = NULL;
    	*a_pnOutBufSize = 0;
    }
    else {
    	memcpy(a_hParser->pFrameBuf + a_hParser->nIndex, a_pInBuf, nLen);

    	a_hParser->nIndex += nLen;
    	*a_ppOutBuf = a_hParser->pFrameBuf;
    	*a_pnOutBufSize = a_hParser->nIndex;

    	a_hParser->nIndex = 0;
    }

    return (!a_hParser->nSOF) ? a_nInBufSize : (nLen + 4);
}

void v2d_parser_close(v2dParserHandle a_hParser)
{
    if(a_hParser)
    {
        free(a_hParser->pFrameBuf);
        free(a_hParser);
    }
}

