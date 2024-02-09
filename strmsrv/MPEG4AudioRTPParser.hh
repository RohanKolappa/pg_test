/*
 * MPEG4AudioRTPParser.hh
 *
 *  Created on: Apr 21, 2011
 *      Author: rkale
 */

#ifndef MPEG4AUDIORTPPARSER_HH_
#define MPEG4AUDIORTPPARSER_HH_

#include "RTPParser.hh"

class CMPEG4AudioRTPParser : public CRTPParser {

public:
    CMPEG4AudioRTPParser(unsigned char aPayloadType);
    virtual ~CMPEG4AudioRTPParser();


    virtual void RetrieveCodecConfig(MediaSubsession *a_pSubsession,
            AVCodecContext * a_pCodecCtx);

protected:
    virtual int Depayload(
            AVPacket * pkt,
            uint32_t * timestamp,
            const uint8_t * buf,
            int len, int flags);

private:
    int ParseAu(const uint8_t *buf);

    int m_nSizeLength;
    int m_nIndexLength;
    int m_nIndexDeltaLength;

    struct AUHeaders {
        int size;
        int index;
        int cts_flag;
        int cts;
        int dts_flag;
        int dts;
        int rap_flag;
        int streamstate;
    } *m_au_headers;

    int m_au_headers_allocated;
    int m_nb_au_headers;
    int m_au_headers_length_bytes;
    int m_cur_au_index;

};


#endif /* MPEG4AUDIORTPPARSER_HH_ */
