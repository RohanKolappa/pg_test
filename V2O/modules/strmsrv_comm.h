#ifndef __STRMSRV_H__
#define __STRMSRV_H__

#include <inttypes.h>

#define STREAMING_SERVER_PORT   10001

struct strmsvr_result {
    char * result;
};

struct strmsrv_icon_params {
    int esc_char[4];
    int esc_char_set[4];
};

struct strmsrv_tx_kbm_params {
    int kbtype_set;
    int kbtype;
    int mstype_set;
    int mstype;
    struct strmsrv_icon_params console_icon;
    struct strmsrv_icon_params local_icon;
};

struct strmsrv_tx_kb_layout_params {
    int kblayout_set;
    char kblanguage[60];
};
struct strmsrv_rx_kb_layout_params {
    int kblayout_set;
    char kblanguage[60];
};

struct strmsrv_rx_mouse_input_params {
    int mouse_input_enable;    
    int timeout_value;
};

struct strmsrv_tx_params {
    int bandwidth;
    int bandwidth_set;

    double burst_ratio;
    int burst_ratio_set;

    int burst_duration;
    int burst_duration_set;

    int kbm_switch;
    int kbm_switch_set;

    int kbm_idle_limit;
    int kbm_idle_limit_set;

    int av_option;
    int av_option_set;
};

struct strmsrv_tx_rfb_connection_params {

    char rfb_server[32];
    int rfb_server_set;

    int rfb_ready;
    int rfb_ready_set;

    unsigned long rfb_keyboard_out;
    unsigned long rfb_mouse_out;
};

struct strmsrv_tx_connection_params {
    char client[32];
    char lut[40];
    int bandwidth;
    uint64_t video_bytes;
    uint64_t audio_bytes;
    uint64_t reverse_audio_bytes;
    int connected;
    int multicast;
    int multicast_count;
};

struct strmsrv_rx_connection_params {
    char server[32+1];
    char status[64+1];
    int bandwidth;
    unsigned long total_bytes;
    unsigned long queue_drops;
    unsigned long video_bytes;
    unsigned long video_packets;
    unsigned long video_loss;
    unsigned long video_outoforder;
    unsigned long audio_bytes;
    unsigned long audio_packets;
    unsigned long audio_loss;
    unsigned long audio_outoforder;
    uint64_t reverse_audio_bytes;
    int connected;
    int multicast;
    int client_unavailable;
};

struct strmsrv_tx_rfb_params {
    int enable;
    char ipaddress[64];
    char password[64];
    int port;
    int mouse_offset_x;
    int mouse_offset_y;
};

struct strmsrv_tx_io_params {
    unsigned long control_in;
    unsigned long control_out;
    unsigned long keyboard_in;
    unsigned long mouse_in;
    unsigned long serial_in;
    unsigned long serial_out;
    unsigned long usbkb_in_bytes;
    unsigned long usbms_in_bytes;
    int kb_type;
    int ms_type;
    char kb_mode[32];
    int multicast_count;
};

struct strmsrv_rx_clnt_params {
    int kbmode;
    int kbmode_set;
    struct strmsrv_icon_params console_icon;
    struct strmsrv_icon_params local_icon;
    struct strmsrv_icon_params remote_icon;
    int rfb_mousecursor;
    int rfb_refresh;
    int local_kbtype;
    int local_kbtype_set;
    int local_mstype;
    int local_mstype_set;
    int ov_option;
    int ov_option_set;
    int ov_color;
    int ov_color_set;
    int ov_keepaspect;
    int ov_keepaspect_set;
};

struct strmsrv_hotplug_params {
    int add_event_device;
    int add_event_device_set;
    int remove_event_device;
    int remove_event_device_set;
    int add_serial_device;
    int add_serial_device_set;
    int remove_serial_device;
    int remove_serial_device_set;
};

struct strmsrv_rx_io_params {
    unsigned long control_in;
    unsigned long control_out;
    unsigned long serial_in;
    unsigned long serial_out;
    unsigned long usbkb_in_bytes;
    unsigned long usbms_in_bytes;
    char kb_mode[32];
    int local_cursor;
};

struct strmsrv_tx_thumbnail_params {
	int enable_thumbnails;
    int enable_native;
    int enable_large_thumbnail;
    int enable_small_thumbnail;
    int large_thumbnail_width;
    int large_thumbnail_height;
    int small_thumbnail_width;
    int small_thumbnail_height;
    int thumbnail_interval;
};
extern struct strmsvr_result *strmsvr_command(char *command);
extern void strmsvr_release_result(struct strmsvr_result *ss_result);
extern char * strmsvr_get_result_xml(struct strmsvr_result *ss_result);
extern int strmsvr_get_status(struct strmsvr_result *ss_result);
extern int strmsvr_execute_command(char *command);
extern int strmsrv_get_tx_params(int ioport, struct strmsrv_tx_params *params);
extern int strmsrv_set_tx_params(int ioport, struct strmsrv_tx_params *params);
extern int strmsrv_set_tx_kbm_params(int ioport, struct strmsrv_tx_kbm_params *params);
extern int strmsrv_get_tx_rfb_connection_params(int ioport,
                      struct strmsrv_tx_rfb_connection_params *params);
extern int strmsrv_get_tx_connection_params(int ioport,
                      struct strmsrv_tx_connection_params *params);
extern int strmsrv_get_rx_connection_params(int ioport,
                      struct strmsrv_rx_connection_params *params);

extern int strmsrv_set_tx_rfb_params(int ioport,
                                     struct strmsrv_tx_rfb_params *params);
extern int strmsrv_set_tx_splash_params(int ioport);
extern int strmsrv_set_rx_splash_params(int ioport);

extern int strmsrv_get_tx_io_params(int ioport,
                                    struct strmsrv_tx_io_params *params);
extern int strmsrv_set_rx_clnt_params(int ioport,
                                      struct strmsrv_rx_clnt_params *params);
extern int strmsrv_set_hotplug_params(int ioport,
                                      struct strmsrv_hotplug_params *params);
extern int strmsrv_get_rx_io_params(int ioport,
                                    struct strmsrv_rx_io_params *params);
extern int strmsrv_tx_disconnect(int ioport);
extern int strmsrv_stop_fpga(int ioport);
extern int strmsrv_rx_fullscreen_refresh(int ioport);
extern int strmsrv_set_tx_thumbnail_params(int ioport,
                                           struct strmsrv_tx_thumbnail_params *params);
extern int strmsrv_set_tx_kb_layout_params(int ioport, 
                                           struct strmsrv_tx_kb_layout_params *params);

extern int strmsrv_set_rx_kb_layout_params(int ioport, 
                             struct strmsrv_rx_kb_layout_params *params);
extern int strmsrv_set_rx_mouse_input_params( int ioport, 
                              struct strmsrv_rx_mouse_input_params *params);
#endif
