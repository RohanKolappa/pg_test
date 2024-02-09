/*
 * rtpplayertx.hh
 *
 *  Created on: Apr 16, 2011
 *      Author: rkale
 */

#ifndef RTPPLAYERTX_HH_
#define RTPPLAYERTX_HH_

#include "sender.hh"
#include "codecdefs.h"

class TaskScheduler;
class UsageEnvironment;
class MediaSession;
class MediaSubsession;
class MediaSubsession;

class CRTPVideoDecoderThread;
class CRTPAudioDecoderThread;

class CRtpPlayerTx: public CSender {
public:
    CRtpPlayerTx(int aId,
            char *sSDPFile, int nPixelFormat);

    virtual ~CRtpPlayerTx();

    virtual void DeleteStream();
    virtual int ProcessStream();

    virtual void UpdateCustomStatus(CCommand *a_pCmd);

    static bool ParseSDPFile(std::string a_sdpFile,
            int & videoPort, int & audioPort, std::string & a_ipaddress);

private:
    void ParseSDP();

    ePixelFormat m_ePixelFormat;
    uint64_t m_nAudioPkts;
    uint64_t m_nAudioBytes;
    uint64_t m_nVideoPkts;
    uint64_t m_nVideoBytes;
    uint32_t m_nDiscontinuities;

    CSQueueSink * m_qVideoDecoderSink;
    CSQueueSink * m_qAudioDecoderSink;

    std::string m_sSDPFile;

    CRTPVideoDecoderThread  * m_pVideoDecoderThread;
    CRTPAudioDecoderThread  * m_pAudioDecoderThread;

    TaskScheduler *m_pTaskScheduler;
    UsageEnvironment *m_pUsageEnv;
    MediaSession *m_pSession;
    MediaSubsession *m_pVideoSubsession;
    MediaSubsession *m_pAudioSubsession;

};

#endif /* RTPPLAYERTX_HH_ */
