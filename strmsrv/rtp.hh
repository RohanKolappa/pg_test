#ifndef __HH_RTP__
#define __HH_RTP__
typedef struct rtp3016 {
  unsigned short        ph_cc:4;
  unsigned short        ph_x:1;
  unsigned short        ph_p:1;
  unsigned short        ph_v:2;
  unsigned short        ph_pt:7;
  unsigned short        ph_m:1;
  unsigned short        ph_seq;
  unsigned long         ph_ts;
  unsigned long         ph_ssrc;
} rtp3016_t;
#define RTPPKTSIZE 1460
#endif /* __HH_RTP__ */

