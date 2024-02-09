#ifndef __CLIENTUTILS_H__
#define __CLIENTUTILS_H__

#ifndef __KBMS_UTILS_H__
#include "kbms_utils.h"
#endif

#ifndef __HH_V2D_H__
#include "v2d.h"
#endif

#define RFB_STATUS_ENABLE   0x01
#define RFB_STATUS_DISPLAY  0x02

struct rfb_s {
    unsigned char status;       /* RFB enabled or not */
    unsigned long session_id;   /* RFB session ID */
    unsigned long query_id;     /* RFB query ID */
    unsigned short abs_x;       /* RFB mouse absolute X */
    unsigned short abs_y;       /* RFB mouse absolute Y */
    unsigned short size_x;      /* RFB mouse maximum X */
    unsigned short size_y;      /* RFB mouse maximum Y */
    unsigned char cursor_flag;  /* Local mouse cursor flag */
    unsigned char numlock;      /* RFB keyboard num lock state */
    unsigned char capslock;     /* RFB keyboard capslock state */
    unsigned char shift;        /* RFB keyboard shift state */
};

typedef struct _KeySymToDeadKeyChar
{
	unsigned int DeadKeyKeySym;// XK_KeySym for dead key in a particular keyboard layout.
	unsigned int CharKeySym; // XK_KeySym for a printable character in a particular keyboard layout.
	unsigned int DeadKeyCharKeySym;// XK_KeySym for the dead key character which is produced when ( DeadKeyKeySym + CharKeySym ) are being pressed for a particular keyboard layout.
}KeySymToDeadKeyChar;


typedef struct _KeySymToDeadKeyCharStruct
{
	KeySymToDeadKeyChar KSTDKC;
}KeySymToDeadKeyCharStruct;

struct xkb_s {
    struct xkb_context *context;
    struct xkb_keymap *keymap;
    struct xkb_state *state; // TODO: ove this to per keyboard state
	char keyboardLayout[10];
	uint32_t DeadKeyKeySym;
	uint32_t ModifierKeySym;
	int KeySymToDeadKeyCharStructSize;
	int ProcessAltGr;
	KeySymToDeadKeyCharStruct *ptrKeySymToDeadKeyCharStruct;
};


typedef struct clnt_s {
    struct rfb_s rfb_info;
    struct xkb_s xkb_info;
    int curr_kbmode;
    int console_escape_keycode[MAX_ESC_CHARS];
    int local_escape_keycode[MAX_ESC_CHARS];
    int remote_escape_keycode[MAX_ESC_CHARS];
    int local_keyboard_type;
    int local_mouse_type;
    int remote_keyboard_type;
    int remote_mouse_type;
    unsigned char kb_buf[KB_BUFFER_SIZE];
    unsigned char esc_buf[MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS];
    int esc_offset;
    int kb_len;
    int kb_offset;
    int last_keycode;
    unsigned char console_escape[MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS];
    int console_escape_len; 
    unsigned char local_escape[MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS];
    int local_escape_len; 
    unsigned char remote_escape[MAX_BYTES_PER_ESC_CHAR * MAX_ESC_CHARS];
    int remote_escape_len;
    int mouse_active;
    int local_mouse_pointer_enable;
} CLNT;

struct client_config {
  
    char                server_address[SERVER_ADDRESS_LEN];
    int                 server_port      ;
    int                 connect_enable   ;
    int                 multicast        ;
    int                 avoption         ;
    int                 videoport        ;
    int                 audioport        ;
    int                 bidir            ;
    int                 samplesize       ;  
    int                 monostereo       ;  
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
    unsigned short      splash_slicesize ; /* Slice Size */
    unsigned long       av_max_bw        ; /* Maximum bandwidth supported by client */
    int                 enable_splash    ; /* Turn on/off RX splash screen */
    int                 rx_splash        ; /* 0 - blue, 1 - gridlines */
    int                 serialport       ; /* Serial port # */
    int                 baudrate         ; /* Serial port baud rate*/
    int                 databits         ; /* Serial port data bits */
    int                 stopbits         ; /* Serial port stop bits */
    unsigned char       parity           ; /* Serial port parity */
    unsigned char       flowcontrol      ; /* Serial port flow control */
    int                 serialwait       ; /* Max waiting time before xmit */
    int                 audio_dscpclass  ; /* DSCP class to send reverse audio */
    int                 tcp_dscpclass    ; /* DSCP class to send tcp */
    int                 debugflags;      ; /* Flag bits for video and audio dumps */
};

struct client_conn_config {
  
    char                server_address[SERVER_ADDRESS_LEN];
    int                 server_port      ;
    int                 connect_enable   ;
    int                 multicast        ;
    int                 avoption         ;
    int                 videoport        ;
    int                 audioport        ;
    int                 bidir            ;
    int                 samplesize       ;  
    int                 monostereo       ;  
    unsigned long       av_max_bw        ; /* Maximum bandwidth supported by client */
    int                 rfb_connection;
    int                 rfb_port;
    char                rfb_password[33];
};

/* Shared memory definitions */
#define CLIENT_SHMKEY_ID        0x3ADE68B1   /* Some random key */

struct client_shm {

    char                        remote_address[SERVER_ADDRESS_LEN];
    int                         statuscode;
    unsigned long               shmflags;
    unsigned long               videoexpseqp;
    unsigned long               videopackets;
    unsigned long               videobytes;
    unsigned long               videodroppackets;
    unsigned long               videolosspackets;
    unsigned long               videoooopackets;
    unsigned long               audioexpseqp;
    unsigned long               audiopackets;
    unsigned long               audiobytes;
    unsigned long               audiodroppackets;
    unsigned long               audiolosspackets;
    unsigned long               audioooopackets;
    unsigned long               serialin;
    unsigned long               serialout;
    unsigned long               controlin;
    unsigned long               controlout;
    unsigned long               revaudiobytes;
};

#ifdef __cplusplus
extern "C" {
#endif
    extern int configure_dropside_video(HAL *hp, 
                                        CLNT *clnt,
                                        struct LUT_struct *lutp, 
                                        int slicesize, 
                                        int compmode,
                                        int motioncompmode);
    extern CLNT *clnt_acquire(HAL *hp);
    extern void clnt_release(CLNT *clnt);
    extern void clnt_set_override_parameters(HAL *hp, CLNT *clnt);
    extern int clnt_kbmode_console(CLNT *clnt);
    extern int clnt_kbmode_local(CLNT *clnt);
    extern int clnt_kbmode_remote(CLNT *clnt);
    extern void clnt_set_local_keyboard_type(HAL *hp, 
                                             CLNT *clnt, 
                                             int kbtype);
    extern void clnt_set_remote_keyboard_type(HAL *hp, 
                                              CLNT *clnt, 
                                              int kbtype);
    extern void clnt_set_local_mouse_type(HAL *hp, 
                                          CLNT *clnt, 
                                          int mstype);
    extern void clnt_set_remote_mouse_type(HAL *hp, 
                                           CLNT *clnt, 
                                           int mstype);
    extern char * clnt_get_keyboard_mode(HAL *hp,
                                      CLNT *clnt,
                                      char *kbbuf);
    extern void clnt_set_keyboard_mode(HAL *hp,
                                       CLNT *clnt,
                                       int mode);
    extern void clnt_set_console_escape(HAL *hp,
                                        CLNT *clnt, 
                                        int e1, 
                                        int e2, 
                                        int e3, 
                                        int e4);
    extern void clnt_set_local_escape(HAL *hp,
                                      CLNT *clnt, 
                                      int e1, 
                                      int e2, 
                                      int e3, 
                                      int e4);
    extern void clnt_set_remote_escape(HAL *hp,
                                       CLNT *clnt, 
                                       int e1, 
                                       int e2, 
                                       int e3, 
                                       int e4);
    extern int retrieve_client_keyboard_data(HAL *hp,
                                             CLNT *clnt,
                                             unsigned char *buf,
                                             int buflen);
    extern int retrieve_client_mouse_data(HAL *hp,
                                          CLNT *clnt,
                                          unsigned char *buf,
                                          int buflen);
    extern int retrieve_client_event_data(HAL *hp,
                                          CLNT *clnt,
                                          struct event_s *eventp,
                                          unsigned char *databuf, 
                                          int *databuflen,
                                          int maxdatabuflen,
                                          unsigned char *xkbbuf,
                                          int *xkblen);
    extern int clnt_keyboard_data_to_rfb(CLNT *clnt, 
                                         unsigned char *kbbuf,
                                         unsigned char *rfbbuf,
                                         int kblen);
    extern void clnt_init_mouse_cursor(HAL *hp, CLNT *clnt);
    extern void clnt_update_mouse_cursor_display(HAL *hp, CLNT *clnt);
    extern void clnt_set_mouse_cursor_enable(HAL *hp, CLNT *clnt, int enable);
    extern void clnt_update_mouse_cursor_pos(HAL *hp, CLNT *clnt);
    extern int get_client_config(struct client_config *c_cfg);
    extern int get_client_conn_config(int board_number, 
                                      struct client_conn_config *cc_cfg);
    
    extern int  client_shm_init(void);
    extern void client_shm_exit(void);
    extern void client_shm_cleanup(void);
    extern void clnt_update_xkb_kb_layout(HAL *hp, CLNT *clnt);
    extern void clnt_disable_mouse_pointer(HAL *hp, CLNT *clnt);
    extern void get_rx_local_mouse_pointer_config(HAL *hp, int *MouseInput, int *timeout);
    int get_supported_hardware(HAL *hp);

#ifdef __cplusplus
}
#endif

#endif
