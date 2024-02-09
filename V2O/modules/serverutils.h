#ifndef __SERVERUTILS_H__
#define __SERVERUTILS_H__

#ifndef __LOAD_NET_H__
#include "loadnet.h"
#endif

#ifndef __KBMS_UTILS_H__
#include "kbms_utils.h"
#endif

#ifndef __HH_V2D_H__
#include "v2d.h"
#endif
                                                                                
typedef struct srvr_s {
    unsigned char kb_buf[KB_BUFFER_SIZE];
    int kb_len;
    int kb_offset;
    int console_escape_keycode[MAX_ESC_CHARS];
    int local_escape_keycode[MAX_ESC_CHARS];
    int last_keycode;
    unsigned char console_escape[MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS];
    int console_escape_len;
    unsigned char local_escape[MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS];
    int local_escape_len;
    unsigned char numlock;
    unsigned char capslock;
    unsigned char shift;
} SRVR;
                                                                                
struct server_config {
  
    char                mcast_address[SERVER_ADDRESS_LEN];
    unsigned short      ctl_port         ; /* TCP port for control channel */
    unsigned short      rev_audio_port   ; /* UDP port for reverse audio */
    int                 multicast        ; /* Multicast capable */
    int                 mcast_ttl        ; /* Multicast capable */
    int                 enable_444       ; /* 1 to turn on 444 compression */
    int                 allow_kbmswitch  ; /* Allow auto KBM switching */
    int                 kbm_idletime     ; /* KBM switch idle time */
    int                 kbm_pacing_delay ; /* KBM data to host pacing delay */
    int                 kb_type          ; /* Server keyboard type */
    int                 ms_type          ; /* Server mouse type */
    unsigned short      splash_stereo    ; /* Stereo? */
    unsigned short      splash_hres      ; /* Horizontal Resolution */
    unsigned short      splash_vres      ; /* Vertical Resolution */
    unsigned short      splash_orig_hres ; /* Horiz Resolution for display */
    unsigned short      splash_orig_vres ; /* Vert Resolution for display */
    double              splash_refresh   ; /* Refresh Rate (Hz) */
    double              splash_pcr       ; /* Pixel Clock Rate (MHz) */
    unsigned short      splash_hfp       ; /* Horiz Front Porch */
    unsigned short      splash_hsync     ; /* Horiz Sync */
    unsigned short      splash_hbp       ; /* Horiz Back Porch */
    unsigned short      splash_vfp       ; /* Vert Front Porch */
    unsigned short      splash_vsync     ; /* Vert Sync */
    unsigned short      splash_vbp       ; /* Vert Back Porch */
    unsigned short      splash_porality  ; /* Sync Porarity */
    unsigned short      splash_mquant    ; /* MQUANT */
    unsigned short      splash_dcquant   ; /* DCQUANT */
    unsigned short      splash_slicesize ; /* Slice Size */
    char                splash_rate[BANDWIDTH_LEN + 1];
    int                 enable_splash    ; /* Enable generating splash screen */
    int                 tx_splash        ; /* 0 - blue, 1 - gridlines */
    int                 serialport       ; /* Serial port # */
    int                 baudrate         ; /* Serial port baudrate */
    int                 databits         ; /* Serial port data bits */
    int                 stopbits         ; /* Serial port stop bits */
    unsigned char       parity           ; /* Serial port parity */
    unsigned char       flowcontrol      ; /* Serial port flow control */
    int                 serialwait       ; /* Max wait time before xmit data */
    int                 tcp_dscpclass    ; /* DSCP class to send TCP packets */
    int                 video_dscpclass  ; /* DSCP class to send video packets */
    int                 audio_dscpclass  ; /* DSCP class to send audio packets */
    int                 debugflags       ; /* Flag bits for video and audio dump */
};


// TODO: possbley form v2dlib??
/* Shared memory definitions */
#define SERVER_SHMKEY_ID        0x68813ADE      /* Some random key */

struct server_shm {
    char                remote_address[SERVER_ADDRESS_LEN];
    int                 statuscode;
    unsigned long       controlin;
    unsigned long       controlout;
    unsigned long       serialin;
    unsigned long       serialout;
    unsigned long       shmflags;
    unsigned long       audioexpseqp;
    unsigned long       audiopackets;
    unsigned long       audiobytes;
    unsigned long       audiodroppackets;
    unsigned long       audiolosspackets;
    unsigned long       audioooopackets;
};
// END TODO

#ifdef __cplusplus
extern "C" {
#endif

extern SRVR *srvr_acquire(HAL *hp);
extern char *dscpclass_value_to_name(int value);
extern void srvr_release(SRVR *srvr);
extern int get_server_config(struct server_config *s_cfg);
extern void srvr_set_console_escape(HAL *hp,
                                    SRVR *srvr, 
                                    int e1, 
                                    int e2, 
                                    int e3, 
                                    int e4,
                                    int kbtype);
extern void srvr_set_local_escape(HAL *hp,
                                  SRVR *srvr, 
                                  int e1, 
                                  int e2, 
                                  int e3, 
                                  int e4,
                                  int kbtype);
extern int process_server_event_data(HAL *hp, 
                                     SRVR *srvr, 
                                     struct event_s *eventp,
                                     unsigned char *databuf,
                                     int kbtype,
                                     int mstype,
                                     int *datbuflen);
extern int srv_keyboard_data_to_rfb( SRVR *srvr,
                                     unsigned char *kbbuf, 
                                     unsigned char *rfbuf, 
                                     int kblen);
extern int get_raw_keyboard_data( SRVR *srvr,
                                   unsigned char *kbbuf, 
                                   unsigned char *rfbbuf,
                                   int kblen,
                                   unsigned int *scanCode,
                                   unsigned char *keydown);

#ifdef __cplusplus
}
#endif

#endif // __SERVERUTILS_H__
