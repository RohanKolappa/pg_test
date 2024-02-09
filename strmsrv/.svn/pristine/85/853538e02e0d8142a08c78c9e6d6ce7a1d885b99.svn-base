#ifndef __HH_MPEGTSMUXFILTER_H__
#define __HH_MPEGTSMUXFILTER_H__

#include <stdint.h>
#include <list>
#include "sframe.hh"


// TS defs - network id, transport stream id
#define DEFAULT_ONID            0x0001
#define DEFAULT_TSID            0x0001
// PAT,SDT pids
#define PAT_PID                 0x0000
#define SDT_PID                 0x0011
/* we retransmit the SI info at this rate */
#define SDT_RETRANS_TIME 500
#define PAT_RETRANS_TIME 100
#define PAT_TID   0x00
#define PMT_TID   0x02 
#define SDT_TID   0x42
// service defs
#define DEFAULT_SID             0x0001
#define DEFAULT_PROVIDER_NAME   "IPVS"
#define DEFAULT_SERVICE_NAME    "iptv"
#define DEFAULT_PMT_START_PID   0x1000
// stream defs
#define DEFAULT_START_PID       0x01E1

#define STREAM_TYPE_VIDEO_MPEG1     0x01
#define STREAM_TYPE_VIDEO_MPEG2     0x02
#define STREAM_TYPE_AUDIO_MPEG1     0x03
#define STREAM_TYPE_AUDIO_MPEG2     0x04
#define STREAM_TYPE_PRIVATE_SECTION 0x05
#define STREAM_TYPE_PRIVATE_DATA    0x06
#define STREAM_TYPE_AUDIO_AAC       0x0f
#define STREAM_TYPE_VIDEO_MPEG4     0x10
#define STREAM_TYPE_VIDEO_H264      0x1a
#define STREAM_TYPE_AUDIO_AC3       0x81

#define STREAM_TYPE_OTHER     0x0000
#define STREAM_TYPE_VIDEO     0x0100
#define STREAM_TYPE_AUDIO     0x0200


// PES packet defs
#define MAX_PES_PACKET_SIZE 59058 // (188-4)*320 + (188-4-8)
#define BASE_PES_HEADER_LEN 5

// TS packet defs
#define TS_PACKET_SIZE 188

#define NO_OF_TS_PKTS 7//Please do not change it, optimised value

typedef struct SCR {
    uint8_t highBit;
    uint32_t remainingBits;
    uint16_t extension;
} SCR;

typedef struct MpegTSStats {
  unsigned long nPCR;
  int nMisc;
  int nNull;
} MpegTSStats;

typedef struct MpegTSStreamStats {
  unsigned long nPTS;
  int nFrame;
  int nPES;
  int nTS;
  int nPad;
} MpegTSStreamStats;

// MpegTS related structs
typedef struct MpegTSWriteStream {
    int pid; /* stream associated pid */
    int cc;
    int payload_start;
    int payload_end;
    int64_t payload_pts;
    uint8_t payload[MAX_PES_PACKET_SIZE];
} MpegTSWriteStream;

typedef struct AVStream {
  int nStreamtype;
  int nFrametype;
  int nBitrate;
  MpegTSWriteStream *priv_data;
  MpegTSStreamStats pStats;
} AVStream;

typedef struct MpegTSSection {
    int pid;
    int cc;
} MpegTSSection;

typedef struct MpegTSService {
    MpegTSSection pmt;
    int pcr_pid;
    int sid;
    char *name;
    char *provider_name;
} MpegTSService;

typedef struct MpegTSWrite {
    MpegTSSection pat;
    MpegTSSection sdt;
    MpegTSService **services;
    int sdt_packet_count;
    int sdt_packet_freq;
    int pat_packet_count;
    int pat_packet_freq;
    int nb_services;
    int onid;
    int tsid;
} MpegTSWrite;


#define MAX_NB_STREAMS        10
#define STREAM_IDX_VIDEO      0
#define STREAM_IDX_AUDIO      1

class MPEGTSMuxFilter {

private:
  //TS Writer info
  MpegTSWrite *priv_data;

  static const uint32_t crc_table[256];

  unsigned int mpegts_crc32(uint8_t *data, int len);
  void put16(uint8_t **q_ptr, int val);
  void putstr8(uint8_t **q_ptr, char *str);

  void mpegts_write_section(MpegTSSection *s, uint8_t *buf, int len);
  int mpegts_write_section1(MpegTSSection *s, int tid, int id, 
                            int version, int sec_num, int last_sec_num,
                            uint8_t *buf, int len);
  void mpegts_write_pmt(MpegTSService *service);
  void mpegts_write_pat(void);
  void mpegts_write_sdt(void);
  MpegTSService *mpegts_add_service(MpegTSWrite *ts, 
                                    int sid, const char *provider_name, const char *name);
  int mpegts_write_header(void);
  int mpegts_write_end(void);

  void mpegts_timeval_mod( struct timeval *ptDst, struct timeval *ptSrc, signed int msec );
  void mpegts_timeval_to_scr(struct timeval *tv, SCR *scr);

  void mpegts_write_tables(void);
  int mpegts_write_ts(AVStream *st, struct timeval *ptPCR);
  int mpegts_write_pes(AVStream *st, uint8_t *inbuf, int insize, struct timeval *ptPTS);
  int mpegts_write_frame(int stream_index, uint8_t *buf, int size,
                         struct timeval *tFrame);

  //output TS packet list info
  std::list<char *> pList;
  std::list<char *> :: iterator pIter;

  void pushTSPacket(char *tspacket);

public:
  //input AV Stream info
  int nb_streams;
  AVStream *streams[MAX_NB_STREAMS];

  MpegTSStats m_pStats;

  MPEGTSMuxFilter(unsigned long nVideoFrametype, int nVideoBitrate,
                  unsigned long nAudioFrametype, int AudioBitrate);
  ~MPEGTSMuxFilter();

  // input AV Frame
  int pushAVFrame(char *pData, 
                   int nLen, 
                   int nFrameType, 
                   timestamp_t nTimestamp);

  // output TS packet
  char *popTSPacket(void);
};

#endif /*__HH_MPEGTSMUXFILTER_H__*/

