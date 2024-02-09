#include "rtp.h"
#include "v2d.h"

#ifndef __RTPUTILS__H_
#define __RTPUTILS__H_

char * WriteAudioPacketIntoBuffer(char* a_pRtpPkt,
                            const int a_nRtpPktLen,
                            char* a_pRevAudioBuffer,
                            int* a_pnBuffLen);

char* ConvertAudioPacketToFPGA(char* a_pRtpPkt,
                                const int a_nRtpPktLen,
                                char* a_pV2dPkt,
                                int* a_pnV2dPktLen);

char* ConvertAudioPacketToFPGAI50Tx(char* a_pRtpPkt,
                                const int a_nRtpPktLen,
                                char* a_pV2dPkt,
                                int* a_pnV2dPktLen);

char* ConvertAudioPacketFromV2DToRTP(char* a_pV2dPkt, 
                                        const int a_nV2dPktLen, 
                                        char* a_pRtpPkt,
                                        int* a_pnRtpPktLen,
                                        RtpContext_t* a_pRtpContext);

char* ConvertAudioPacketFromV2DToRTPRx(char* a_pV2dPkt, 
                                        const int a_nV2dPktLen, 
                                        char* a_pRtpPkt,
                                        int* a_pnRtpPktLen,
                                        RtpContext_t* a_pRtpContext);

char* ConvertAudioPacketFromRTPToV2D(char* a_pRtpPkt, 
                                        const int a_nRtpPktLen,
                                        char* a_pV2dPkt,
                                        int* a_pnV2dPktLen);

char* ConvertVideoPacketFromV2DToRTP(char *a_pV2dPkt, 
                                        const int a_nV2dPktLen, 
                                        char* a_pRtpPkt,
                                        int* a_pnRtpPktLen,
                                        RtpContext_t* a_pRtpContext);

char* ConvertVideoPacketFromRTPToV2D(char *a_pRtpPkt, 
                                        const int a_nRtpPktLen,
                                        char* a_pV2dPkt,
                                        int* a_pnV2dPktLen);

#endif
