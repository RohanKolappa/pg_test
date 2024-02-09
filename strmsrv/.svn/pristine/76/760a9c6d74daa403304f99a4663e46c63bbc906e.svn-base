/*
 * H264VideoRTPParser.hh
 *
 *  Created on: Apr 21, 2011
 *      Author: rkale
 */

#ifndef H264RTPPARSER_HH_
#define H264RTPPARSER_HH_

#include "RTPParser.hh"

class CH264VideoRTPParser : public CRTPParser {

public:
    CH264VideoRTPParser(unsigned char aPayloadType);
    virtual ~CH264VideoRTPParser();


    virtual void RetrieveCodecConfig(MediaSubsession *a_pSubsession,
            AVCodecContext * a_pCodecCtx);

protected:
    virtual int Depayload(
            AVPacket * pkt,
            uint32_t * timestamp,
            const uint8_t * buf,
            int len, int flags);

private:

};


#endif /* H264RTPPARSER_HH_ */
