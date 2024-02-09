/*
 * MPEG4VideoRTPParser.hh
 *
 *  Created on: May 3, 2011
 *      Author: rkale
 */

#ifndef MPEG4VIDEORTPPARSER_HH_
#define MPEG4VIDEORTPPARSER_HH_

#include "RTPParser.hh"

class CMPEG4VideoRTPParser : public CRTPParser {

public:
    CMPEG4VideoRTPParser(unsigned char aPayloadType);
    virtual ~CMPEG4VideoRTPParser();


    virtual void RetrieveCodecConfig(MediaSubsession *a_pSubsession,
            AVCodecContext * a_pCodecCtx);

protected:

private:

};


#endif /* MPEG4VIDEORTPPARSER_HH_ */
