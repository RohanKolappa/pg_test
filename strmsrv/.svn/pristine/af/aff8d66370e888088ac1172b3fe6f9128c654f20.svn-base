/*
 * MPEG4LATMAudioRTPParser.hh
 *
 *  Created on: May 12, 2011
 *      Author: rkale
 */

#ifndef MPEG4LATMAUDIORTPPARSER_HH_
#define MPEG4LATMAUDIORTPPARSER_HH_

#include "RTPParser.hh"


class CMPEG4LATMAudioRTPParser : public CRTPParser {

public:
    CMPEG4LATMAudioRTPParser(unsigned char aPayloadType);
    virtual ~CMPEG4LATMAudioRTPParser();


    virtual void RetrieveCodecConfig(MediaSubsession *a_pSubsession,
            AVCodecContext * a_pCodecCtx);

protected:
    virtual int Depayload(
            AVPacket * pkt,
            uint32_t * timestamp,
            const uint8_t * buf,
            int len, int flags);

private:
    void *m_dynamicBuffer;
    uint8_t *m_pBuffer;
    int m_nPosition;
    int m_nLength;
    uint32_t m_nCurrentTimestamp;
};


#endif /* MPEG4LATMAUDIORTPPARSER_HH_ */
