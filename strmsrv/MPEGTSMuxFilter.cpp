
#include "strmsrv_compatibility.h"

#include "common.hh"
#ifdef DOWIS
#include "wisencrx.hh"
#endif
#include "MPEGTSMuxFilter.hh"

using namespace std;

const uint32_t MPEGTSMuxFilter::crc_table[] = {
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
    0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
    0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
    0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
    0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
    0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
    0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
    0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
    0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
    0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
    0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
    0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
    0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
    0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
    0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
    0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
    0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
    0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
    0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
    0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
    0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
    0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

unsigned int MPEGTSMuxFilter::mpegts_crc32(uint8_t *data, int len)
{
    register int i;
    unsigned int crc = 0xffffffff;
    
    for (i=0; i<len; i++)
        crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];
    
    return crc;
}

/*********************************************/
/* mpegts section writer */

/* NOTE: 4 bytes must be left at the end for the crc32 */
void MPEGTSMuxFilter::mpegts_write_section(MpegTSSection *s, uint8_t *buf, int len)
{
    unsigned int crc;
    // this "new" will be "delete'd" by the "popTS'er"
    uint8_t *tspacket = new uint8_t [TS_PACKET_SIZE];
    uint8_t *buf_ptr;
    uint8_t *q;
    int first, b, len1, left;

    crc = mpegts_crc32(buf, len - 4);
    buf[len - 4] = (crc >> 24) & 0xff;
    buf[len - 3] = (crc >> 16) & 0xff;
    buf[len - 2] = (crc >> 8) & 0xff;
    buf[len - 1] = (crc) & 0xff;
    
    /* send each packet */
    buf_ptr = buf;
    while (len > 0) {
        first = (buf == buf_ptr);
        q = tspacket;
        *q++ = 0x47;
        b = (s->pid >> 8);
        if (first)
            b |= 0x40;
        *q++ = b;
        *q++ = s->pid;
        s->cc = (s->cc + 1) & 0xf;
        *q++ = 0x10 | s->cc;
        if (first)
            *q++ = 0; /* 0 offset */
        len1 = TS_PACKET_SIZE - (q - tspacket);
        if (len1 > len) 
            len1 = len;
        memcpy(q, buf_ptr, len1);
        q += len1;
        /* add known padding data */
        left = TS_PACKET_SIZE - (q - tspacket);
        if (left > 0)
            memset(q, 0xff, left);

        pushTSPacket((char *)tspacket);
        m_pStats.nMisc++;

        buf_ptr += len1;
        len -= len1;
    }
}

void MPEGTSMuxFilter::put16(uint8_t **q_ptr, int val)
{
    uint8_t *q;
    q = *q_ptr;
    *q++ = val >> 8;
    *q++ = val;
    *q_ptr = q;
}

int MPEGTSMuxFilter::mpegts_write_section1(MpegTSSection *s, int tid, int id, 
                                           int version, int sec_num, int last_sec_num,
                                           uint8_t *buf, int len)
{
    uint8_t section[1024], *q;
    unsigned int tot_len;
    
    tot_len = 3 + 5 + len + 4;
    /* check if not too big */
    if (tot_len > 1024)
        return -1;

    q = section;
    *q++ = tid;
    put16(&q, 0xb000 | (len + 5 + 4)); /* 5 byte header + 4 byte CRC */
    put16(&q, id);
    *q++ = 0xc1 | (version << 1); /* current_next_indicator = 1 */
    *q++ = sec_num;
    *q++ = last_sec_num;
    memcpy(q, buf, len);
    
    mpegts_write_section(s, section, tot_len);
    return 0;
}

/*********************************************/
/* mpegts writer */

void MPEGTSMuxFilter::mpegts_write_pat(void)
{
    MpegTSWrite *ts = priv_data;
    MpegTSService *service;
    uint8_t data[1012], *q;
    int i;
    
    q = data;
    for(i = 0; i < ts->nb_services; i++) {
        service = ts->services[i];
        put16(&q, service->sid);
        put16(&q, 0xe000 | service->pmt.pid);
    }
    mpegts_write_section1(&ts->pat, PAT_TID, ts->tsid, 0, 0, 0,
                          data, q - data);
}

void MPEGTSMuxFilter::mpegts_write_pmt(MpegTSService *service)
{
    uint8_t data[1012], *q, *desc_length_ptr, *program_info_length_ptr;
    int val, stream_type, i;

    q = data;
    put16(&q, 0xe000 | service->pcr_pid);

    program_info_length_ptr = q;
    q += 2; /* patched after */

    /* put program info here */

    val = 0xf000 | (q - program_info_length_ptr - 2);
    program_info_length_ptr[0] = val >> 8;
    program_info_length_ptr[1] = val;
    
    for(i = 0; i < nb_streams; i++) {
        AVStream *st = streams[i];
        MpegTSWriteStream *ts_st = st->priv_data;
        stream_type = (st->nStreamtype & 0x00ff);
        *q++ = stream_type;
        put16(&q, 0xe000 | ts_st->pid);
        desc_length_ptr = q;
        q += 2; /* patched after */

        /* write optional descriptors here */

        val = 0xf000 | (q - desc_length_ptr - 2);
        desc_length_ptr[0] = val >> 8;
        desc_length_ptr[1] = val;
    }
    mpegts_write_section1(&service->pmt, PMT_TID, service->sid, 0, 0, 0,
                          data, q - data);
}   

/* NOTE: str == NULL is accepted for an empty string */
void MPEGTSMuxFilter::putstr8(uint8_t **q_ptr, char *str)
{
    uint8_t *q;
    int len;

    q = *q_ptr;
    if (!str)
        len = 0;
    else
        len = strlen(str);
    *q++ = len;
    if (str != NULL)
        memcpy(q, str, len);
    q += len;
    *q_ptr = q;
}

void MPEGTSMuxFilter::mpegts_write_sdt(void)
{
    MpegTSWrite *ts = priv_data;
    MpegTSService *service;
    uint8_t data[1012], *q, *desc_list_len_ptr, *desc_len_ptr;
    int i, running_status, free_ca_mode, val;
    
    q = data;
    put16(&q, ts->onid);
    *q++ = 0xff;
    for(i = 0; i < ts->nb_services; i++) {
        service = ts->services[i];
        put16(&q, service->sid);
        *q++ = 0xfc | 0x00; /* currently no EIT info */
        desc_list_len_ptr = q;
        q += 2;
        running_status = 4; /* running */
        free_ca_mode = 0;

        /* write only one descriptor for the service name and provider */
        *q++ = 0x48;
        desc_len_ptr = q;
        q++;
        *q++ = 0x01; /* digital television service */
        putstr8(&q, service->provider_name);
        putstr8(&q, service->name);
        desc_len_ptr[0] = q - desc_len_ptr - 1;

        /* fill descriptor length */
        val = (running_status << 13) | (free_ca_mode << 12) | 
            (q - desc_list_len_ptr - 2);
        desc_list_len_ptr[0] = val >> 8;
        desc_list_len_ptr[1] = val;
    }
    mpegts_write_section1(&ts->sdt, SDT_TID, ts->tsid, 0, 0, 0,
                          data, q - data);
}

/* send SDT, PAT and PMT tables regulary */
void MPEGTSMuxFilter::mpegts_write_tables(void)
{
    MpegTSWrite *ts = priv_data;

    if (++ts->sdt_packet_count == ts->sdt_packet_freq) {
        ts->sdt_packet_count = 0;
        mpegts_write_sdt();
    }
    if (++ts->pat_packet_count == ts->pat_packet_freq) {
        ts->pat_packet_count = 0;
        mpegts_write_pat();
        for(int i = 0; i < ts->nb_services; i++) {
            mpegts_write_pmt(ts->services[i]);
        }
    }
}

void MPEGTSMuxFilter::mpegts_timeval_mod( struct timeval *ptDst, struct timeval *ptSrc, signed int msec )
{
    if( msec > 0 ) {
        ptDst->tv_sec = ptSrc->tv_sec + (msec / 1000) + 
            ((ptSrc->tv_usec + ((msec % 1000) * 1000)) / 1000000);
        ptDst->tv_usec = ((ptSrc->tv_usec + ((msec % 1000) * 1000)) % 1000000);
    }
    else {
        msec = 0 - msec;
        ptDst->tv_sec = ptSrc->tv_sec - (msec / 1000) + 
            ((ptSrc->tv_usec - ((msec % 1000) * 1000)) / 1000000);
        ptDst->tv_usec = ((ptSrc->tv_usec - ((msec % 1000) * 1000)) % 1000000);
    }
}

void MPEGTSMuxFilter::mpegts_timeval_to_scr(struct timeval *tv, SCR *scr)
{
    scr->highBit = ((tv->tv_sec*45000 + (tv->tv_usec*9)/200)& 0x80000000) != 0;
    scr->remainingBits = tv->tv_sec*90000 + (tv->tv_usec*9)/100;
    scr->extension = (tv->tv_usec*9)%100;
}

int MPEGTSMuxFilter::mpegts_write_ts(AVStream *st, struct timeval *ptPCR)
{
    MpegTSWriteStream *ts_st = st->priv_data;
    // this "new" will be "delete'd" by the "popTS'er"
    uint8_t *tspacket = new uint8_t[TS_PACKET_SIZE];
    uint8_t *q, *p;
    uint8_t afc, afl, aff;
    int pes_len, data_len, header_len, afl_len, aff_len, pcr_len, pad_len;
    int i, val;
    SCR pcr;

  
    // send PAT PMT SDT if needed
    mpegts_write_tables();

    // get start and max length of data available for this packet
    p = ts_st->payload + ts_st->payload_start;
    pes_len = ts_st->payload_end - ts_st->payload_start;

    // calc the lengths of all the fields of this packet
    if( ptPCR != NULL ) {  // this packet will have PCR
        header_len = 4;  // 4 bytes basic TS header
        afl_len = 1; // 1 byte for Adaptation field length
        aff_len = 1; // 1 byte for adaptation field flag
        pcr_len = 6;
        pad_len = 0;
        data_len = TS_PACKET_SIZE - header_len - afl_len - aff_len - pcr_len;
        if( pes_len < data_len ) {  // not enough data - add padding
            pad_len = data_len - pes_len;
        }
        else {  // use as much pes data as will fit 
            pes_len = data_len;
        }
        afc = 0x30;
        afl = aff_len + pcr_len + pad_len;
        aff = 0x10;
    }
    else {  // no PCR case
        header_len = 4; // basic TS header
        afl_len = 0;
        aff_len = 0;
        pcr_len = 0;
        pad_len = 0;

        afc = 0x10;
        afl = aff_len + pcr_len + pad_len;
        aff = 0x00;

        data_len = TS_PACKET_SIZE - header_len - afl_len - aff_len - pcr_len;
        if( pes_len < data_len ) { // not enough data - turn on adaptation field, add afl byte
            afc = 0x30;
            afl_len = 1;
            data_len = TS_PACKET_SIZE - header_len - afl_len - aff_len - pcr_len;
            if( pes_len < data_len ) {  // still not enough - add aff byte
                aff_len = 1;
                data_len = TS_PACKET_SIZE - header_len - afl_len - aff_len - pcr_len;
                if( pes_len < data_len ) {  // still not enough - add padding
                    pad_len = data_len - pes_len;
                }
            }
            afl = aff_len + pcr_len + pad_len;
        }
        else {  // use as much pes data as will fit 
            pes_len = data_len;
        }
    }

    // now start filling the packet
    q = tspacket;

    // fixed TS header start byte
    *q++ = 0x47; 

    // start of new PES packet | high 5 bits of PID
    val = (ts_st->pid >> 8);
    if (ts_st->payload_start == 0) val |= 0x40;  
    *q++ = val;  

    // lower 8 bits of PID
    *q++ = ts_st->pid;  

    // adaptation field control | continuity counter
    *q++ = afc | ts_st->cc;  
    ts_st->cc = (ts_st->cc + 1) & 0xf;

    // adaptation field length
    if( afl_len != 0 ) *q++ = afl;  

    // adaptation field flags
    if( aff_len != 0 ) *q++ = aff;  

    // PCR
    if( pcr_len != 0 ) {
        mpegts_timeval_to_scr(ptPCR, &pcr);
        uint32_t pcrHigh32Bits = (pcr.highBit<<31) | (pcr.remainingBits>>1);
        uint8_t pcrLowBit = pcr.remainingBits&1;
        uint8_t extHighBit = (pcr.extension&0x100)>>8;

        *q++ = pcrHigh32Bits>>24;
        *q++ = pcrHigh32Bits>>16;
        *q++ = pcrHigh32Bits>>8;
        *q++ = pcrHigh32Bits;
        *q++ = (pcrLowBit<<7)|0x7E|extHighBit;
        *q++ = (uint8_t)pcr.extension; // low 8 bits of extension
    }

    // padding bytes
    for (i=0; i<pad_len; i++) *q++ = 0xFF;

    // at last put the data bytes in
    memcpy(q, p, pes_len);
    ts_st->payload_start += pes_len;

    // send it off
    pushTSPacket((char *)tspacket);

    // so the cbr padder can account for it
    return pad_len;
}

/* Make one PES packet from data at inbuf */
int MPEGTSMuxFilter::mpegts_write_pes(AVStream *st, 
                                      uint8_t *inbuf, int insize, 
                                      struct timeval *ptPTS)
{
    MpegTSWriteStream *ts_st = st->priv_data;
    uint8_t *q;
    int len, ext_header_len, data_len;
    SCR pts, dts;
    struct timeval *ptDTS = NULL;

    /* migth need to delay the PCR as well if the DTS is specd behind the PTS
    // set the DTS a little behind PTS
    if( ptPTS != NULL ) {
    struct timeval tDTS;
    mpegts_timeval_mod( &tDTS, ptPTS, -33 );
    ptDTS = &tDTS;
    }
    */
    q = ts_st->payload;
    ts_st->payload_start = 0;
    /* write base PES header */
    *q++ = 0x00;
    *q++ = 0x00;
    *q++ = 0x01;
    if ((st->nStreamtype & 0xff00) == STREAM_TYPE_VIDEO) {
        *q++ = 0xe0;
    }
    else {
        *q++ = 0xc0;
    }

    ext_header_len = 3;
    if (ptPTS != NULL) {
        ext_header_len += 5;
        if(ptDTS != NULL ) {
            ext_header_len += 5;
        }
    }

    data_len = MAX_PES_PACKET_SIZE - BASE_PES_HEADER_LEN - ext_header_len;
    if( data_len > insize ) {
        data_len = insize;
    }
    len = ext_header_len + data_len;
    *q++ = len >> 8;
    *q++ = len;

    /* write ext PES header */
    *q++ = 0x80;
    if (ptPTS != NULL) {
        if(ptDTS != NULL) {
            *q++ = 0x80 | 0x40; /* PTS and DTS present */
            *q++ = 0x0a; /* extra header len */
        }
        else {
            *q++ = 0x80; /* PTS present */
            *q++ = 0x05; /* extra header len */
        }

        /* write the PTS */
        mpegts_timeval_to_scr(ptPTS, &pts);
        *q++ = 0x20|(pts.highBit<<3)|(pts.remainingBits>>29)|0x01;
        *q++ = pts.remainingBits>>22;
        *q++ = (pts.remainingBits>>14)|0x01;
        *q++ = pts.remainingBits>>7;
        *q++ = (pts.remainingBits<<1)|0x01;

        /* write the DTS */
        if(ptDTS != NULL) {
            mpegts_timeval_to_scr(ptDTS, &dts);
            *q++ = 0x20|(dts.highBit<<3)|(dts.remainingBits>>29)|0x01;
            *q++ = dts.remainingBits>>22;
            *q++ = (dts.remainingBits>>14)|0x01;
            *q++ = dts.remainingBits>>7;
            *q++ = (dts.remainingBits<<1)|0x01;
        }
    } else {
        *q++ = 0x00; /* no PTS */
        *q++ = 0x00;
    }

    /* copy the data */
    memcpy (q, inbuf, data_len);
    ts_st->payload_end = (int)(q - ts_st->payload) + data_len;
 
    /* return amount of data used */
    return data_len;
}

int MPEGTSMuxFilter::mpegts_write_frame(int stream_index,
                                        uint8_t *buf, int size,
                                        struct timeval *tFrame)
{
    AVStream *st = streams[stream_index];
    MpegTSWriteStream *ts_st = st->priv_data;
    int len;

    struct timeval tMod, *ptPTS, *ptPCR;

    st->pStats.nFrame++;
    st->pStats.nPES = 0;
    st->pStats.nTS = 0;

    // if video
    //   padsize = pad_to_cbr(size)

    len = 0;
    ptPTS = tFrame;
    st->pStats.nPTS = (CTimeKeeper::GetTimeStamp(ptPTS) * 90);
    /*
      if( dbgfd ) {
      fprintf((FILE *)dbgfd, "PTS : %1d %u %02d : ", 
      pts.highBit, pts.remainingBits, pts.extension);
      }
    */
    while (size > len) {
        // form frame data into PES packet(s)
        // put PTS in the header of the first PES packet
        len += mpegts_write_pes(st, (uint8_t *)(buf+len), (size-len), ptPTS);
        st->pStats.nPES++;

        // write each PES packet out as TS packet
        // put PCR in the first TS packet of the first PES packet of Video
        if( ((st->nStreamtype & 0xff00) == STREAM_TYPE_VIDEO) && (ptPTS != NULL) ) {
            // delay the PCR a little to allow client side AV buffer
            mpegts_timeval_mod( &tMod, tFrame,-100 );
            ptPCR = &tMod;
            m_pStats.nPCR = (CTimeKeeper::GetTimeStamp(ptPCR) * 90);
        }
        else {
            ptPCR = NULL;
        }
        while( ts_st->payload_end > ts_st->payload_start ) {
            mpegts_write_ts(st, ptPCR);
            st->pStats.nTS++;
            ptPCR = NULL;
        }
        ptPTS = NULL;

        // padsize -= ts_padding
    }

    return 0;
}

int MPEGTSMuxFilter::mpegts_write_end(void)
{
    MpegTSWrite *ts = priv_data;
    MpegTSWriteStream *ts_st;
    MpegTSService *service;
    AVStream *st;
    int i;

    /* flush current packets */
    for(i = 0; i < nb_streams; i++) {
        st = streams[i];
        ts_st = st->priv_data;
        if (ts_st->payload_end > 0) {
            /* should not be needed - if yes - need to figure out the right thing to do
               mpegts_write_pes(st, ts_st->payload, ts_st->payload_end,
               ts_st->payload_pts);
            */
        }
    }
        
    for(i = 0; i < ts->nb_services; i++) {
        service = ts->services[i];
        delete[] (service->provider_name);
        delete[] (service->name);
        delete (service);
    }
    delete[] ts->services;

    for(i = 0; i < nb_streams; i++) {
        st = streams[i];
        delete (st->priv_data);
    }
    return 0;
}

MpegTSService *MPEGTSMuxFilter::mpegts_add_service(MpegTSWrite *ts, 
                                                   int sid, 
                                                   const char *provider_name,
                                                   const char *name)
{
    int i;
    MpegTSService *service;

    service = new MpegTSService;
    if (!service)
        return NULL;
    service->pmt.pid = DEFAULT_PMT_START_PID + ts->nb_services - 1;
    service->pmt.cc = 0;
    service->sid = sid;

    //    service->provider_name = av_strdup(provider_name);
    service->provider_name = new char[strlen(provider_name) + 1];
    strcpy(service->provider_name, provider_name);

    //    service->name = av_strdup(name);
    service->name = new char[strlen(name) + 1];
    strcpy(service->name, name);

    service->pcr_pid = 0x1fff;

    //    dynarray_add(&ts->services, &ts->nb_services, service);
    MpegTSService **temp = new MpegTSService *[ts->nb_services + 1];
    for(i=0; i<ts->nb_services; i++) {
        temp[i] = ts->services[i];
    }
    temp[i] = service;
    delete [] ts->services;
    ts->services = temp;
    ts->nb_services++;

    return service;
}

int MPEGTSMuxFilter::mpegts_write_header(void)
{
    MpegTSWrite *ts = priv_data;
    MpegTSWriteStream *ts_st;
    MpegTSService *service;
    AVStream *st;
    int i, total_bit_rate;

    ts->tsid = DEFAULT_TSID;
    ts->onid = DEFAULT_ONID;
    /* allocate a single DVB service */
    service = mpegts_add_service(ts, DEFAULT_SID, 
                                 DEFAULT_PROVIDER_NAME, DEFAULT_SERVICE_NAME);
    ts->pat.pid = PAT_PID;
    ts->pat.cc = 0;

    ts->sdt.pid = SDT_PID;
    ts->sdt.cc = 0;

    /* assign pids to each stream */
    total_bit_rate = 0;
    for(i = 0;i < nb_streams; i++) {
      
        st = streams[i];
        switch((st->nFrametype & FRAMETYPE_AVCODEC_MASK)) {
            //        case (FRAMETYPE_VIDEO | FRAMETYPE_MPEG1):
        case (FRAMETYPE_VIDEO | FRAMETYPE_MP2V):
            st->nStreamtype = STREAM_TYPE_VIDEO | STREAM_TYPE_VIDEO_MPEG2;
            break;
        case (FRAMETYPE_VIDEO | FRAMETYPE_MP4V):
            st->nStreamtype = STREAM_TYPE_VIDEO | STREAM_TYPE_VIDEO_MPEG4;
            break;
        case (FRAMETYPE_VIDEO | FRAMETYPE_H264):
            st->nStreamtype = STREAM_TYPE_VIDEO | STREAM_TYPE_VIDEO_H264;
            break;
            //        case (FRAMETYPE_AUDIO | FRAMETYPE_MP2):
            //        case (FRAMETYPE_AUDIO | FRAMETYPE_MP3):
        case (FRAMETYPE_AUDIO | FRAMETYPE_MPA):
            st->nStreamtype = STREAM_TYPE_AUDIO | STREAM_TYPE_AUDIO_MPEG1;
            break;
        case (FRAMETYPE_AUDIO | FRAMETYPE_AAC):
            st->nStreamtype = STREAM_TYPE_AUDIO | STREAM_TYPE_AUDIO_AAC;
            break;
            //        case (FRAMETYPE_AUDIO | FRAMETYPE_AC3):
            //            st->nStreamtype = STREAM_TYPE_AUDIO | STREAM_TYPE_AUDIO_AC3;
            //            break;
        default:
            st->nStreamtype = STREAM_TYPE_PRIVATE_DATA;
            break;
        }
        ts_st = new MpegTSWriteStream;
        if (!ts_st)
            goto fail;
        st->priv_data = ts_st;
        ts_st->pid = DEFAULT_START_PID + i;
        ts_st->cc = 0;
        ts_st->payload_pts = 0;
        /* update PCR pid if needed */
        if ((st->nStreamtype & 0xff00) == STREAM_TYPE_VIDEO && 
            service->pcr_pid == 0x1fff)
            service->pcr_pid = ts_st->pid;
        total_bit_rate += st->nBitrate;
    }
    if (total_bit_rate <= 8 * 1024)
        total_bit_rate = 8 * 1024;
    ts->sdt_packet_freq = (total_bit_rate * SDT_RETRANS_TIME) / 
        (TS_PACKET_SIZE * 8 * 1000);
    ts->pat_packet_freq = (total_bit_rate * PAT_RETRANS_TIME) / 
        (TS_PACKET_SIZE * 8 * 1000);
#if 0
    printf("%d %d %d\n", 
           total_bit_rate, ts->sdt_packet_freq, ts->pat_packet_freq);
#endif

    /* write info at the start of the file, so that it will be fast to
       find them */
    // set it up so that the tables will get written with the first AV TS packet
    ts->sdt_packet_count = ts->sdt_packet_freq - 1;
    ts->pat_packet_count = ts->pat_packet_freq - 1;

    return 0;

 fail:
    for(i = 0;i < nb_streams; i++) {
        st = streams[i];
        delete (st->priv_data);
    }
    return -1;
}

char *MPEGTSMuxFilter::popTSPacket(void)
{
    char *tspacket = NULL;
    if(! pList.empty() ) {
        tspacket = pList.front();
        pList.pop_front();
    }

    return tspacket;
}

void MPEGTSMuxFilter::pushTSPacket(char *tspacket)
{
    pList.push_back(tspacket);
}

// split incoming AV frame data into list of 188 byte MPEGTS packets
// insert PAT/PMT etc as needed to produce valid TS mux stream
int MPEGTSMuxFilter::pushAVFrame(char *pData, 
                                 int nLen, 
                                 int nFrameType, 
                                 timestamp_t nTimestamp)
{

    //frametype to stream index
    int stream_idx = -1;
    switch(nFrameType & FRAMETYPE_AV_MASK) {
    case FRAMETYPE_VIDEO:
        stream_idx = STREAM_IDX_VIDEO;
        break;
    case FRAMETYPE_AUDIO:
        stream_idx = STREAM_IDX_AUDIO;
        break;
    default:
        return -1;
        break;
    }

    //timestamp to timeval
    struct timeval tv;
    CTimeKeeper::GetTimeVal(&tv, nTimestamp);

    //write frame
    int r = mpegts_write_frame(stream_idx, (uint8_t *)pData, nLen, &tv);
   
    return r;
}

MPEGTSMuxFilter::MPEGTSMuxFilter(unsigned long nVideoFrametype, int nVideoBitrate,
                                 unsigned long nAudioFrametype, int nAudioBitrate) {

    // init output packet list
    pList.clear();

    //init AVstream info
    nb_streams = 0;

    AVStream *pVideoStream = new AVStream;
    memset(pVideoStream, 0x0, sizeof(AVStream));
    pVideoStream->nFrametype = nVideoFrametype;
    pVideoStream->nBitrate = nVideoBitrate;
    pVideoStream->priv_data = NULL;
    streams[STREAM_IDX_VIDEO] = pVideoStream;
    nb_streams++;

    AVStream *pAudioStream = new AVStream;
    memset(pAudioStream, 0x0, sizeof(AVStream));
    pAudioStream->nFrametype = nAudioFrametype;
    pAudioStream->nBitrate = nAudioBitrate;
    pAudioStream->priv_data = NULL;
    streams[STREAM_IDX_AUDIO] = pAudioStream;
    nb_streams++;

    memset(&m_pStats, 0x0, sizeof(MpegTSStats));
    //init and start writing TS
    MpegTSWrite *ts = priv_data = new MpegTSWrite;
    ts->nb_services = 0;
    ts->services = NULL;

    mpegts_write_header();
    // handle error?
}

MPEGTSMuxFilter::~MPEGTSMuxFilter() {

    // end TS write
    mpegts_write_end();
    delete (priv_data);

    // delete any output packets left in list
    char *tspacket;
    while((tspacket = popTSPacket()) != NULL) {
        delete (tspacket);
    }

    delete streams[STREAM_IDX_VIDEO];
    delete streams[STREAM_IDX_AUDIO];
}
