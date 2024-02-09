#ifndef __HAL_H_INCLUDED__
#define __HAL_H_INCLUDED__

#ifdef __XPI__
#include <linux/videodev2.h>
#include "netvizxpi/netvizxpi.h"
#include "netvizxpi/lut_table.h"
#else
#include "fpga.h"
#include "fpga_ioctl.h"
#endif

#ifndef _LINUX_VT_H
#include <linux/vt.h>
#endif
                                                                                
#ifndef _TERMIOS_H
#include <termios.h>
#endif

#define GENLOCK_MODE_NONE           0
#define GENLOCK_MODE_MASTER         1
#define GENLOCK_MODE_SLAVE          2

#define MAXDEVLEN                   64

#define USBEVENT_FLAG_NEW           0x00000001
#define USBEVENT_FLAG_DELETED       0x00000002

#define DEBUG_FLAG_VIDEODUMP            0x0001
#define DEBUG_FLAG_AUDIODUMP            0x0002
#define DEBUG_FLAG_REVAUDIODUMP         0x0004

#define IFRAME_INTERVAL_NONE            0

#define FPGA_BARNO              1

#define XPI_FPGA_PACKET_SIZE    1078    /* 1024 + FPGA header */

#define RTPFILTER_INDEX_VIDEO   0
#define RTPFILTER_INDEX_AUDIO   1
#define RTPFILTER_CONTEXT_VIDEO 0
#define RTPFILTER_CONTEXT_AUDIO 1

#define MAX_PPSCAPACITY_LITE    62208000
#define MAX_PPSCAPACITY_PRO     140000000
#define NO_LICENSE              0
#define LITE_LICENSE            1
#define PRO_LICENSE             2
#define PRO3D_LICENSE           3

// from v2dlib/v2o.h
#define SPLASH_TYPE_GRID        0x00
#define SPLASH_TYPE_TX          0x01
#define SPLASH_TYPE_BLUE        0x02
#define SPLASH_TYPE_BLACK       0x04
#define SPLASH_TYPE_WHITE       0x08
#define SPLASH_TYPE_CBAR        0x10


/*********************
 * The HAL structure *
 *********************/
#ifndef _MY_LIST_H
#include "mylist.h"
#endif

struct serial_s {
    int serial_fd;
    char serial_name[MAXDEVLEN];
};

struct console_s {
    int console_fd;
    int kb_active;
};

struct event_s {
    int event_fd;
    char event_name[MAXDEVLEN];
    int event_state;
    struct list_head event_list; 
};

typedef struct hal_s {
    int kernel_fd;
    int board_number;
    int audio_fd;
    int keyboard_fd;
    int mouse_fd;
    int i2c_fd;
    struct console_s console_info;
    struct serial_s serial_info;
    struct list_head event_head;
    struct hardware_info hw_info;
} HAL;

typedef struct regval_s {
    int reg;
    int val;
} REGVAL;

/*********************************
 * List of HAL exposed functions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

    extern HAL *hal_acquire(int board_number);
    extern void hal_release(HAL * hp);
    extern int hal_get_video_fd(HAL * hp);
    extern int hal_get_audio_fd(HAL * hp);
    extern int hal_get_keyboard_fd(HAL * hp);
    extern int hal_get_mouse_fd(HAL * hp);
    extern char * hal_get_serial_name(HAL * hp);

    extern void hal_set_fpga_val_k(HAL * hp, unsigned short reg,
                                   unsigned short val);
    extern unsigned short hal_get_fpga_val_k(HAL * hp, unsigned short reg);
    extern void hal_and_in_fpga_val_k(HAL * hp, unsigned short reg,
                                      unsigned long val);
    extern void hal_or_in_fpga_val_k(HAL * hp, unsigned short reg, 
                                     unsigned long val);
    extern void hal_set_fpga_val32_k(HAL * hp, unsigned long reg,
                                     unsigned long val);
    extern unsigned long hal_get_fpga_val32_k(HAL * hp, unsigned long reg);
    extern void hal_and_in_fpga_val32_k(HAL * hp, unsigned long reg,
                                      unsigned long val);
    extern void hal_or_in_fpga_val32_k(HAL * hp, unsigned long reg, 
                                     unsigned long val);
    extern void hal_set_mdio_val_k(HAL *hp, int offset, int page, 
                                    int regad, unsigned long val);
    extern int hal_get_mdio_val_k(HAL *hp, int offset, int page, 
                                    int regad, unsigned long *val);
    extern void hal_set_mac_val_k(HAL *hp, int dummy, int dummy2, int regad, 
                                    unsigned long val);
    extern int hal_get_mac_val_k(HAL *hp, int dummy, int dummy2, int regad, 
                                    unsigned long *val);
    extern unsigned long hal_get_s6fpga_val32_k(HAL * hp, unsigned long reg);
    extern void hal_set_s6fpga_val32_k(HAL * hp, unsigned long reg, 
                                    unsigned long val);
    extern void hal_and_in_s6fpga_val32_k(HAL * hp, unsigned long reg,
                                          unsigned long val);
    extern void hal_or_in_s6fpga_val32_k(HAL * hp, unsigned long reg, 
                                         unsigned long val);
    extern unsigned long hal_get_s3fpga_val32_k(HAL * hp, unsigned long reg);
    extern void hal_set_s3fpga_val32_k(HAL * hp, unsigned long reg, 
                                    unsigned long val);
    extern void hal_and_in_s3fpga_val32_k(HAL * hp, unsigned long reg,
                                          unsigned long val);
    extern void hal_or_in_s3fpga_val32_k(HAL * hp, unsigned long reg, 
                                         unsigned long val);
    extern int hal_get_s6fpga_version(HAL * hp, char *buf, int maxlen);
    extern unsigned long hal_get_s6fpga_date(HAL *hp, char *buf, int maxlen);
    extern int hal_get_s3fpga_version(HAL * hp, char *buf, int maxlen);
    extern unsigned long hal_get_s3fpga_date(HAL *hp, char *buf, int maxlen);
    extern void hal_set_ad9888_val_k(HAL * hp, int reg,
                                    unsigned char value);
    extern unsigned char hal_get_ad9888_val_k(HAL * hp, int reg);
    extern int hal_id18125_write_k(HAL * hp, int empty_offset,
                                   int full_offset);
    extern int hal_id18125_read_k(HAL * hp, int *empty_offset,
                                  int *full_offset);
    extern void hal_set_ad9852_val_k(HAL * hp, unsigned short reg,
                                     unsigned char val);
    extern unsigned char hal_get_ad9852_val_k(HAL * hp,
                                              unsigned short reg);
    extern int hal_getAudioVideoFlag(HAL *hp);
    extern void hal_setAudioVideoFlag(HAL *hp, int flag);
    extern int hal_getAudioMixerLocalVolume(HAL *hp);
    extern int hal_getAudioMixerRemoteVolume(HAL *hp);
    extern void hal_resetAddsideAudio(HAL *hp);
    extern void hal_resetDropsideAudio(HAL *hp);
    extern void hal_setAudioMixer(HAL *hp, int nLocalVolume, int nRemoteVolume, int AudioInMic);
    extern int hal_setVideoCheck(HAL *hp, int mode);
    extern unsigned long hal_getVerticalFrequencyVal(HAL * hp);
    extern unsigned long hal_getHorizontalFrequencyVal(HAL * hp);
    extern unsigned long hal_getHorizontalLinesVal(HAL * hp);
    extern double hal_getVerticalFrequency(HAL * hp);
    extern double hal_getHorizontalFrequency(HAL * hp);
    extern unsigned long hal_getHorizontalLines(HAL * hp);
    extern int hal_getStereo(HAL * hp);
    extern int hal_get_framerate_divider(HAL * hp);
    extern void hal_set_framerate_divider(HAL * hp, int val);
    extern int hal_resetAddsideVideo(HAL * hp);
    extern int hal_resetDropsideVideo(HAL * hp);
    extern int hal_resetOutputVideo(HAL *hp);
    extern int hal_setDropsideVideo(HAL * hp, 
                                    struct LUT_struct *lutp,
                                    struct LUT_struct *dlutp);
    extern void hal_adjustCompression(HAL * hp, int low_comp,
                                      int low_comp_delta, int high_comp,
                                      int high_comp_delta);
    extern int hal_getKeyboardType(HAL *hp);
    extern void hal_setKeyboardType(HAL *hp, int kbtype);
    extern int hal_getMouseType(HAL *hp);
    extern int hal_getGenlockMode(HAL * hp);
    extern void hal_setGenlockMode(HAL * hp, int mode);
    extern int hal_getActiveIntf(HAL * hp);
    extern int hal_setActiveIntf(HAL * hp, int intf);
    extern int hal_forceActiveIntf(HAL * hp, int intf);
    extern void hal_adjust_hd_k(HAL * hp, int shift);
    extern void hal_adjust_vd_k(HAL * hp, int shift);
    extern int hal_get_hd_k(HAL * hp);
    extern int hal_get_vd_k(HAL * hp);
    extern int hal_get_charge_pump_curr_k(HAL * hp);
    extern void hal_set_default_charge_pump_curr_k(HAL * hp, int val);
    extern int hal_get_default_charge_pump_curr_k(HAL * hp);
    extern void hal_set_phase_adjust_k(HAL * hp, unsigned char val);
    extern unsigned char hal_get_phase_adjust_k(HAL * hp);
    extern int hal_compute_dds_frequency(struct LUT_struct *lutp,
                                         int tweak,
                                         double *ddsfreq);
    extern int hal_get_pixel_adjust_k(HAL * hp);
    extern void hal_set_pixel_adjust_k(HAL * hp, int pixels);
    extern unsigned long hal_get_xpi_videowords(HAL * hp);
    extern unsigned long hal_get_xpi_audiowords(HAL * hp, int transmit);
    extern unsigned long hal_get_xpi_videopkts(HAL * hp);
    extern unsigned long hal_get_xpi_audiopkts(HAL * hp);
    extern unsigned long hal_get_xpi_videoloss(HAL * hp);
    extern unsigned long hal_get_xpi_videodrop(HAL * hp);
    extern unsigned long hal_get_xpi_audioloss(HAL * hp);
    extern unsigned long hal_get_xpi_audiodrop(HAL * hp);
    extern unsigned long hal_get_frame_cnt(HAL *hp, unsigned long base);
    extern unsigned long hal_get_slice_cnt(HAL *hp, unsigned long base);
    extern int hal_real_video_available(HAL * hp);
    extern int hal_set_add_driveridle(HAL * hp, int val);
    extern int hal_get_addside_lut(HAL * hp, struct LUT_struct *lutp);
    extern void hal_configure_addside_video(HAL *hp, int enable);
    extern int hal_configure_addside_lut(HAL *hp,
                                         unsigned short hsyncc,
                                         unsigned short amhreg,
                                         unsigned short amvreg,
                                         unsigned short amtreg,
                                         unsigned short duallink,
                                         struct LUT_struct *lutp);
    extern int hal_get_dropside_lut(HAL * hp, struct LUT_struct *lutp);
    extern int hal_set_dropside_lut(HAL * hp, struct LUT_struct *lutp);
    extern int hal_set_dropside_video(HAL * hp, struct LUT_struct *lutp);
    extern int hal_check_display_lut(HAL * hp);
    extern int hal_get_display_lut(HAL * hp, struct LUT_struct *lutp);
    extern int hal_set_display_lut(HAL * hp, struct LUT_struct *lutp);
    extern int hal_set_display_video(HAL * hp, struct LUT_struct *lutp);
    extern int hal_get_fpga_flags(HAL *hp);
    extern int hal_set_fpga_flags(HAL *hp, int flags);
    extern int hal_set_ppscapacity(HAL *hp, unsigned long ppscapacity);
    extern int hal_retrieve_lut(HAL * hp, struct LUT_struct *lutp);
    extern int hal_insert_lut(HAL * hp, struct LUT_struct *lutp);
    extern int hal_reset_lut(HAL * hp);
    extern int hal_refresh_lut(HAL * hp);
    extern int hal_get_addside_mquant(HAL * hp, int *val);
    extern int hal_set_addside_mquant(HAL * hp, int val);
    extern int hal_get_addside_dcquant(HAL * hp, int *val);
    extern int hal_set_addside_dcquant(HAL * hp, int val);
    extern int hal_enable_addside_video(HAL *hp);
    extern int hal_disable_addside_video(HAL *hp);
    extern int hal_enable_addside_audio(HAL *hp);
    extern int hal_disable_addside_audio(HAL *hp);
    extern int hal_enable_dropside_audio(HAL *hp);
    extern int hal_disable_dropside_audio(HAL *hp);
    extern void hal_enable_splash_in_driver(HAL *hp, int type);
    extern void hal_enable_splash_video(HAL *hp, 
                                        int type, 
                                        struct LUT_struct *lutp);
    extern void hal_disable_splash_video(HAL *hp);
    extern int hal_set_adc_free_run(HAL *hp, int lutid);
    extern void hal_set_addside_bandwidth(HAL * hp, int val, 
                                          float burst_ratio, int burst_duration);
    extern int hal_set_addside_low_compression(HAL * hp, int val);
    extern int hal_get_addside_low_compression(HAL * hp, int *val);
    extern int hal_set_addside_high_compression(HAL * hp, int val);
    extern int hal_get_addside_high_compression(HAL * hp, int *val);
    extern int hal_set_addside_noslicedrop(HAL * hp, int framecnt);
    extern int hal_set_addside_nobpslice(HAL * hp, int val);
    extern int hal_get_addside_nobpslice(HAL * hp, int *val);
    extern int hal_set_dropside_nobpslice(HAL * hp, int val);
    extern int hal_get_dropside_nobpslice(HAL * hp, int *val);
    extern int hal_set_ac_noise_threshold(HAL * hp, int val);
    extern int hal_get_ac_noise_threshold(HAL * hp, int *val);
    extern int hal_set_dc_noise_threshold(HAL * hp, int val);
    extern int hal_get_dc_noise_threshold(HAL * hp, int *val);
    extern int hal_get_firmware_version(HAL * hp, char *buf, int maxlen);
    extern int hal_get_firmware_date(HAL *hp, char *buf, int maxlen);
    extern int hal_get_fanout_fpga_version(HAL * hp, char *buf, int maxlen);
    extern unsigned long  hal_get_fanout_fpga_date(HAL *hp, char *buf, 
            int maxlen);
    extern int hal_set_video_available(HAL * hp, int val);
    extern int hal_get_addside_compmode(HAL * hp, 
                                        int *compmode, 
                                        int *motioncompmode);
    extern int hal_get_dropside_compmode(HAL * hp, 
                                         int *compmode,
                                         int *motioncompmode);
    extern int hal_set_addside_compmode(HAL * hp, 
                                        int compmode, 
                                        int motioncompmode);
    extern int hal_set_dropside_compmode(HAL * hp, 
                                         int compmode,
                                         int motioncompmode);
    extern int hal_get_horiz_polarity(HAL *hp, int *val);
    extern int hal_set_horiz_polarity(HAL *hp, int val);
    extern int hal_get_vert_polarity(HAL *hp, int *val);
    extern int hal_set_vert_polarity(HAL *hp, int val);
    extern int hal_get_video_output(HAL *hp);
    extern int hal_set_video_output(HAL *hp, int mode);

    extern int hal_force_panic(HAL * hp);
    extern int hal_fire_interrupt(HAL * hp, int val);
    extern int hal_set_log_level(HAL * hp, int val);

    extern int hal_get_pc_params(HAL * hp, struct pc_param_io *pcpiop);
    extern int hal_set_pc_params(HAL * hp, struct pc_param_io *pcpiop);
    extern int hal_get_kbm_params(HAL * hp, struct kbm_param_io *kbmpiop);
    extern int hal_reset_kbm_params(HAL * hp);
    extern int hal_flush_kbm_queues_k(HAL * hp);
    extern int hal_get_fc_read_stats(HAL *hp, struct fc_stat_io *fcsiop);
    extern int hal_get_fc_write_stats(HAL *hp, struct fc_stat_io *fcsiop);
    extern int hal_get_fc_params(HAL * hp, struct fc_param_io *fcpiop);
    extern int hal_get_fc_wparams(HAL * hp, struct fc_wparam_io *fcwpiop);
    extern int hal_flush_fc_queues_k(HAL * hp);
    extern int hal_get_fc_queue_limits(HAL * hp, struct fc_queue_limits_io *fcqlimitiop);
    extern int hal_set_fc_queue_limits(HAL * hp, struct fc_queue_limits_io *fcqlimitiop);
    extern int hal_set_ext_sync(HAL *hp, int val);
    extern int hal_set_overlay_params (HAL * hp, struct ov_param_io *ov);
    extern int hal_get_overlay_params(HAL * hp, struct ov_param_io *ov);
    extern int hal_get_overlay_info(HAL * hp, struct ov_param_io *ov);
    extern int hal_get_overlay_mode(HAL * hp);
    extern int hal_enable_cropping(HAL *hp,
                            int hleft,
                            int hright,
                            int vtop,
                            int vbottom);
    extern int hal_disable_cropping(HAL *hp);
    extern int hal_get_cropping_params(HAL * hp,
                                       int * hleft,
                                       int * hright,
                                       int * vtop,
                                       int * vbottom);
    extern int hal_set_framecnt_enable(HAL * hp, int val);
    extern int hal_set_progressive_compression(HAL * hp, int val);

    extern int hal_set_iframe_interval(HAL *hp, unsigned long interval);
    extern int hal_set_dvimode_auto(HAL *hp, int dvimode_auto);
    extern int hal_set_optimize_latency(HAL *hp, int optimize_latency);

    extern int hal_get_fpga_type(HAL *hp);
    extern void kill_hald_current_channel(HAL *hp, int stop_strmsrv);
    extern int hal_get_codec_version(HAL *hp);
    extern void  hal_set_codec_version(HAL *hp, int codecversion);
    extern int hal_set_slice_dimensions(HAL *hp, int width, int height);
    extern int hal_codec_version_changed_rx(HAL *hp, int codecversion);
    extern int hal_configure_rx_codec_version(HAL *hp, int codecversion);
    extern int hal_get_configured_rx_codec_version(HAL *hp);
    int hal_get_configured_license(HAL *hp);
    extern unsigned long hal_get_omnitek_val_k(HAL * hp, unsigned long reg);
    extern void hal_set_omnitek_val_k(HAL * hp, unsigned long reg, 
                                unsigned long val);
#ifdef __cplusplus
}
#endif

/********************************
 * Macros exposed by HAL module *
 ********************************/
#define ac_noise_level_to_value(level) ((level) - 1)
#define dc_noise_level_to_value(level) ((level) - 1)
#define sampling_phase_level_to_value(level) ((level) + 16)
#define ac_noise_value_to_level(value) ((value) + 1)
#define dc_noise_value_to_level(value) ((value) + 1)
#define sampling_phase_value_to_level(value) ((value) - 16)

/*****************************************************************
 * Macro to convert between charge pump current index and values *
 *****************************************************************/
#define cpmp_index_to_value(value) ((value)==0 ? 50 :  \
                                   ((value)==1 ? 100 : \
                                   ((value)==2 ? 150 : \
                                   ((value)==3 ? 250 : \
                                   ((value)==4 ? 350 : \
                                   ((value)==5 ? 500 : \
                                   ((value)==6 ? 750 : 1500)))))))
#define cpmp_value_to_index(value) ((value)<=50 ? 0:  \
                                   ((value)<=100 ? 1: \
                                   ((value)<=150 ? 2: \
                                   ((value)<=250 ? 3: \
                                   ((value)<=350 ? 4: \
                                   ((value)<=500 ? 5: \
                                   ((value)<=750 ? 6 : 7)))))))

/*********************************************************
 * Macro to convert between compression and quant values *
 *********************************************************/
#define mquant_from_compression(value)  ((value)==0 ? 0: \
                                        ((value)<=2 ? 1: \
                                        ((value)<=5 ? 2: \
                                        ((value)<=7 ? 3: \
                                        ((value)==8 ? 4: \
                                        ((value)==9 ? 5: 6))))))
#define dcquant_from_compression(value) ((value)<=1 ? 0: \
                                        ((value)==2 ? 1: \
                                        ((value)==3 ? 0: \
                                        ((value)==4 ? 1: \
                                        ((value)<=6 ? 2: 3)))))
#define compression_from_quant(mq, dcq) \
({ \
    int __comp = -1; \
    switch (dcq) { \
        case 0: \
            if (mq == 0) __comp = 0; \
            else if (mq == 1) __comp = 1; \
            else if (mq == 2) __comp = 3; \
            break; \
        case 1: \
            if (mq == 1) __comp = 2; \
            else if (mq == 2) __comp = 4; \
            break; \
        case 2: \
            if (mq == 2) __comp = 5; \
            else if (mq == 3) __comp = 6; \
            break; \
        case 3: \
            if (mq == 3) __comp = 7; \
            else if (mq == 4) __comp = 8; \
            else if (mq == 5) __comp = 9; \
            else if (mq == 6) __comp = 10; \
            break; \
    } \
    __comp; \
})



#endif                          // __HAL_H_INCLUDED__
