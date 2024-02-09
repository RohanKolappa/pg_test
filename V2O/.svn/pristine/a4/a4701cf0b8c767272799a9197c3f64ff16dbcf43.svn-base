#ifndef __FPGA_IOCTL_H_INCLUDED__
#define __FPGA_IOCTL_H_INCLUDED__

#include <linux/ioctl.h>
#include "lut_table.h"


#define FC_SET_PARAM_RATE          0x00000001
#define FC_SET_PARAM_LIMIT         0x00000002
#define FC_SET_PARAM_PEAKRATE      0x00000004
#define FC_SET_PARAM_BURST         0x00000008
#define FC_SET_PARAM_MAXFRAMERATE  0x00000010
#define FC_RESET_VIDEO_STATS       0x00000020
#define FC_RESET_AUDIO_STATS       0x00000040

#define FC_RESET_ALL_STATS         (FC_RESET_VIDEO_STATS |   \
                                   FC_RESET_AUDIO_STATS)

#define FC_FLAGS_VIDEO_ONLY        0x000000001  
#define FC_FLAGS_AUDIO_MONO        0x000000002  
#define FC_FLAGS_AUDIO_8BIT        0x000000004  
#define FC_FLAGS_AUDIO_8KHZ        0x000000008  
#define FC_FLAGS_AUDIO_ONLY        0x000000010  

struct fc_param_io {
    unsigned long                 frame_cnt;
    unsigned long                 slice_cnt;
};

struct fc_av_device_stats {
    unsigned long                 dma_requests;
    unsigned long                 dma_busy;
    unsigned long                 dma_lock_busy;
    unsigned long                 dma_errors;
    unsigned long                 dma_grants;
    unsigned long                 fifo_empty;
    unsigned long                 fifo_full;
    unsigned long                 fifo_reset;
    unsigned long                 fops_requests;
    unsigned long                 fops_success;
    unsigned long                 fops_errors;
    unsigned long                 fops_bytes;
    unsigned long                 sequence;
    unsigned long                 queue_length;
    unsigned long                 queue_full;
    unsigned long                 queue_empty;
    unsigned long                 queue_limit;
    unsigned long                 buffer_drops;
    unsigned long                 buffer_pool;
};

struct fc_stat_io {
    struct fc_av_device_stats     video_stats;
    struct fc_av_device_stats     audio_stats;
};

struct fc_queue_limits_io {
    unsigned long                 video_read_queue_limit;
    unsigned long                 audio_read_queue_limit;
    unsigned long                 video_write_queue_limit;
    unsigned long                 audio_write_queue_limit;
};

struct fc_wparam_io {
    unsigned long               wframe_cnt;
    unsigned long               wslice_cnt;
};

struct pc_param_io {
    unsigned long               setmask;
    int                         pc_xblocks;
    int                         pc_ylow;
    int                         pc_yhigh;
    int                         pc_rlow;
    int                         pc_rhigh;
};

struct pixadjust_io {
    int                         pix_adjust;
    int                         pix_tweak_hi;
    int                         pix_tweak_lo;
};

#define PC_SET_PARAM_XBLOCKS    0x00000001
#define PC_SET_PARAM_YLOW       0x00000002
#define PC_SET_PARAM_YHIGH      0x00000004
#define PC_SET_PARAM_RLOW       0x00000008
#define PC_SET_PARAM_RHIGH      0x00000010


struct kbm_param_io {
    unsigned long               setmask;
    unsigned long               kb_rbytes;
    unsigned long               kb_rdropped;
    unsigned long               ms_rbytes;
    unsigned long               ms_rdropped;
    unsigned long               kb_wbytes;
    unsigned long               kb_usb_wbytes;
    unsigned long               kb_wdropped;
    unsigned long               ms_wbytes;
    unsigned long               ms_usb_wbytes;
    unsigned long               ms_wdropped;
};

struct led_io {
    int         lednumb;
    int         leddata;
};

struct hardware_info {
    int hardware_type;
    int hardware_version;
    int board_type;
    int hardware_detected;
    int number_of_boards;
};

#define MAX_QUAL_RULES                  5

struct qar_param_io {
    int         id;
    long        hwm;
    long        lwm;
    int         low_comp_delta;
    int         high_comp_delta;
    int         divider_delta;
    int         fpga_drop;
};


#define REGIO_TYPE_FPGA_GET             1
#define REGIO_TYPE_FPGA_SET             2
#define REGIO_TYPE_FPGA_ANDIN           3
#define REGIO_TYPE_FPGA_ORIN            4

#define REGIO_TYPE_AD9888_GET           8
#define REGIO_TYPE_AD9888_SET           9

#define REGIO_TYPE_AD9852_GET           12
#define REGIO_TYPE_AD9852_SET           13

#define REGIO_TYPE_ID18125_GET          16
#define REGIO_TYPE_ID18125_SET          17

#define REGIO_TYPE_BAR2_GET             21
#define REGIO_TYPE_BAR2_SET             22
#define REGIO_TYPE_BAR3_GET             23
#define REGIO_TYPE_BAR3_SET             24
#define REGIO_TYPE_PLX_GET              31
#define REGIO_TYPE_PLX_SET              32
#define REGIO_TYPE_PCICONFIG_GET        36      
#define REGIO_TYPE_PCICONFIG_SET        37
#define REGIO_TYPE_EEPROM_GET           40
#define REGIO_TYPE_EEPROM_SET           41
#define REGIO_TYPE_VPD_READ             44
#define REGIO_TYPE_VPD_WRITE            45
#define REGIO_TYPE_PLX_TEST_DMA         50


#define ALARM_TYPE_LOS          1
#define ALARM_TYPE_LOP          2
#define ALARM_TYPE_LOF          3
#define ALARM_TYPE_AISL         4
#define ALARM_TYPE_AISP         5
#define ALARM_TYPE_RDIL         6
#define ALARM_TYPE_RDIP         7

#define LED_TYPE_ALLON          1
#define LED_TYPE_ALLOFF         2
#define LED_TYPE_NORMAL         3

#define MOUSE_ID_PS2            0
#define MOUSE_ID_IMPS2          3
#define MOUSE_PS2_PKTSIZE       6
#define MOUSE_IMPS2_PKTSIZE     8

#define ACTIVE_INTF_AUTO        1
#define ACTIVE_INTF_DIGITAL     2
#define ACTIVE_INTF_ANALOG      3

#define FPGA_IOCMAGIC 0xB5

#define FPGA_IOC_RESET          _IO(FPGA_IOCMAGIC,   0)
#define FPGA_IOC_RESET_LUT      _IOWR(FPGA_IOCMAGIC, 1, int)
#define FPGA_IOC_SET_LUT        _IOWR(FPGA_IOCMAGIC, 2, struct LUT_struct)
#define FPGA_IOC_GET_LUT        _IOWR(FPGA_IOCMAGIC, 3, struct LUT_struct)
#define FPGA_IOC_SET_LEDSTATE   _IOWR(FPGA_IOCMAGIC, 4, struct led_io)
#define FPGA_IOC_SET_LEDALIVE   _IOWR(FPGA_IOCMAGIC, 5, struct led_io)
#define FPGA_IOC_MOD_REG        _IOWR(FPGA_IOCMAGIC, 6, struct reg_io)
#define FPGA_IOC_SET_LED        _IOWR(FPGA_IOCMAGIC, 7, int)
#define FPGA_IOC_SET_LOGLVL     _IOWR(FPGA_IOCMAGIC, 8, int)
#define FPGA_IOC_GET_INTR_STAT  _IOWR(FPGA_IOCMAGIC, 9, struct interrupt_io)
#define FPGA_IOC_GET_INTR_RST   _IOWR(FPGA_IOCMAGIC, 10, int)
#define FPGA_IOC_INTR_FIRE      _IOWR(FPGA_IOCMAGIC, 11, int)
#define FPGA_IOC_SET_EEPROM     _IOWR(FPGA_IOCMAGIC, 12, struct eeprom_io)
#define FPGA_IOC_GET_EEPROM     _IOWR(FPGA_IOCMAGIC, 13, struct eeprom_io)
#define FPGA_IOC_RESET_ADDSIDE  _IOWR(FPGA_IOCMAGIC, 14, int)
#define FPGA_IOC_RESET_DROPSIDE _IOWR(FPGA_IOCMAGIC, 15, int)
#define FPGA_IOC_GET_ADDSIDELUT _IOWR(FPGA_IOCMAGIC, 16, struct LUT_struct)
#define FPGA_IOC_SET_ADDSIDELUT _IOWR(FPGA_IOCMAGIC, 17, struct video_param_io)
#define FPGA_IOC_GET_DROPSIDELUT    _IOWR(FPGA_IOCMAGIC, 18, struct LUT_struct)
#define FPGA_IOC_SET_DROPSIDELUT    _IOWR(FPGA_IOCMAGIC, 19, struct LUT_struct)
#define FPGA_IOC_GET_DISPLAYLUT _IOWR(FPGA_IOCMAGIC, 20, struct LUT_struct)
#define FPGA_IOC_SET_DISPLAYLUT _IOWR(FPGA_IOCMAGIC, 21, struct LUT_struct)
#define FPGA_IOC_SET_VID_AVL    _IOWR(FPGA_IOCMAGIC, 22, int)
#define FPGA_IOC_GET_VID_AVL    _IOWR(FPGA_IOCMAGIC, 23, int)
#define FPGA_IOC_GET_HDELAY     _IOWR(FPGA_IOCMAGIC, 24, int)
#define FPGA_IOC_SET_HDELAY     _IOWR(FPGA_IOCMAGIC, 25, int)
#define FPGA_IOC_GET_VDELAY     _IOWR(FPGA_IOCMAGIC, 26, int)
#define FPGA_IOC_SET_VDELAY     _IOWR(FPGA_IOCMAGIC, 27, int)
#define FPGA_IOC_GET_PSHIFT     _IOWR(FPGA_IOCMAGIC, 28, unsigned char)
#define FPGA_IOC_SET_PSHIFT     _IOWR(FPGA_IOCMAGIC, 29, unsigned char)
#define FPGA_IOC_GET_PIXADJUST  _IOWR(FPGA_IOCMAGIC, 30, struct pixadjust_io)
#define FPGA_IOC_SET_PIXADJUST  _IOWR(FPGA_IOCMAGIC, 31, struct pixadjust_io)
#define FPGA_IOC_FC_PARAM_GET   _IOWR(FPGA_IOCMAGIC, 32, struct fc_param_io)
#define FPGA_IOC_FC_RSTAT_GET   _IOWR(FPGA_IOCMAGIC, 33, struct fc_stat_io)
#define FPGA_IOC_FC_WPARAM_GET  _IOWR(FPGA_IOCMAGIC, 34, struct fc_wparam_io)
#define FPGA_IOC_FC_WSTAT_GET   _IOWR(FPGA_IOCMAGIC, 35, struct fc_stat_io)
#define FPGA_IOC_FC_FLUSH       _IOWR(FPGA_IOCMAGIC, 36, int)
#define FPGA_IOC_FC_QUEUE_GET   _IOWR(FPGA_IOCMAGIC, 37, struct fc_queue_limits_io)
#define FPGA_IOC_FC_QUEUE_SET   _IOWR(FPGA_IOCMAGIC, 38, struct fc_queue_limits_io)
#define FPGA_IOC_PC_PARAM_GET   _IOWR(FPGA_IOCMAGIC, 39, struct pc_param_io)
#define FPGA_IOC_PC_PARAM_SET   _IOWR(FPGA_IOCMAGIC, 40, struct pc_param_io)
#define FPGA_IOC_KBM_PARAM_GET  _IOWR(FPGA_IOCMAGIC, 41, struct kbm_param_io)
#define FPGA_IOC_KBM_PARAM_SET  _IOWR(FPGA_IOCMAGIC, 42, struct kbm_param_io)
#define FPGA_IOC_KB_TYPE_GET    _IOWR(FPGA_IOCMAGIC, 43, int)
#define FPGA_IOC_KB_TYPE_SET    _IOWR(FPGA_IOCMAGIC, 44, int)
#define FPGA_IOC_MS_TYPE_GET    _IOWR(FPGA_IOCMAGIC, 45, int)
#define FPGA_IOC_GET_AMQUANT    _IOWR(FPGA_IOCMAGIC, 46, int)
#define FPGA_IOC_SET_AMQUANT    _IOWR(FPGA_IOCMAGIC, 47, int)
#define FPGA_IOC_GET_ADCQUANT   _IOWR(FPGA_IOCMAGIC, 48, int)
#define FPGA_IOC_SET_ADCQUANT   _IOWR(FPGA_IOCMAGIC, 49, int)
#define FPGA_IOC_SET_ASLICEDROP _IOWR(FPGA_IOCMAGIC, 50, int)
#define FPGA_IOC_GET_ANOBPSLICE _IOWR(FPGA_IOCMAGIC, 51, int)
#define FPGA_IOC_SET_ANOBPSLICE _IOWR(FPGA_IOCMAGIC, 52, int)
#define FPGA_IOC_GET_DNOBPSLICE _IOWR(FPGA_IOCMAGIC, 53, int)
#define FPGA_IOC_SET_DNOBPSLICE _IOWR(FPGA_IOCMAGIC, 54, int)
#define FPGA_IOC_GET_ACNOISET   _IOWR(FPGA_IOCMAGIC, 55, int)
#define FPGA_IOC_SET_ACNOISET   _IOWR(FPGA_IOCMAGIC, 56, int)
#define FPGA_IOC_GET_DCNOISET   _IOWR(FPGA_IOCMAGIC, 57, int)
#define FPGA_IOC_SET_DCNOISET   _IOWR(FPGA_IOCMAGIC, 58, int)
#define FPGA_IOC_GET_ACOMPMODE  _IOWR(FPGA_IOCMAGIC, 59, int)
#define FPGA_IOC_SET_ACOMPMODE  _IOWR(FPGA_IOCMAGIC, 60, int)
#define FPGA_IOC_GET_DCOMPMODE  _IOWR(FPGA_IOCMAGIC, 61, int)
#define FPGA_IOC_SET_DCOMPMODE  _IOWR(FPGA_IOCMAGIC, 62, int)
#define FPGA_IOC_SET_VIDEOCHECK _IOWR(FPGA_IOCMAGIC, 63, int)
#define FPGA_IOC_SET_EXTSYNC    _IOWR(FPGA_IOCMAGIC, 64, int)
#define FPGA_IOC_SET_DIVIDER    _IOWR(FPGA_IOCMAGIC, 65, int)
#define FPGA_IOC_SET_ADD_DRIVERIDLE     _IOWR(FPGA_IOCMAGIC, 66, int)
#define FPGA_IOC_SET_DEFAULT_CPMP       _IOWR(FPGA_IOCMAGIC, 67, int)
#define FPGA_IOC_GET_DEFAULT_CPMP       _IOWR(FPGA_IOCMAGIC, 68, int)
#define FPGA_IOC_GET_INTERFACE          _IOWR(FPGA_IOCMAGIC, 69, int)
#define FPGA_IOC_SET_INTERFACE          _IOWR(FPGA_IOCMAGIC, 70, int)
#define FPGA_IOC_FORCE_INTERFACE        _IOWR(FPGA_IOCMAGIC, 71, int)
#define FPGA_IOC_GET_DDC_EDID           _IOWR(FPGA_IOCMAGIC, 72, int)
#define FPGA_IOC_KBM_FLUSH              _IOWR(FPGA_IOCMAGIC, 73, int)
#define FPGA_IOC_GET_LATENCY    _IOWR(FPGA_IOCMAGIC, 74, unsigned long)
#define FPGA_IOC_SET_FPGA_DROP  _IOWR(FPGA_IOCMAGIC, 75, int)
#define FPGA_IOC_QAR_PARAM_GET  _IOWR(FPGA_IOCMAGIC, 76, struct qar_param_io)
#define FPGA_IOC_QAR_PARAM_SET  _IOWR(FPGA_IOCMAGIC, 77, struct qar_param_io)
#define FPGA_IOC_SET_NOISETHRESMODE _IOWR(FPGA_IOCMAGIC, 78, int)
#define FPGA_IOC_GET_AVFLAG         _IOWR(FPGA_IOCMAGIC, 79, int)
#define FPGA_IOC_SET_AVFLAG         _IOWR(FPGA_IOCMAGIC, 80, int)
#define FPGA_IOC_GET_DISPLAY_HPOL   _IOWR(FPGA_IOCMAGIC, 81, int)
#define FPGA_IOC_SET_DISPLAY_HPOL   _IOWR(FPGA_IOCMAGIC, 82, int)
#define FPGA_IOC_GET_DISPLAY_VPOL   _IOWR(FPGA_IOCMAGIC, 83, int)
#define FPGA_IOC_SET_DISPLAY_VPOL   _IOWR(FPGA_IOCMAGIC, 84, int)

#define FPGA_IOC_MOD_PLX            _IOWR(FPGA_IOCMAGIC, 90, struct reg_io)
#define FPGA_IOC_GET_PLXCAP         _IOWR(FPGA_IOCMAGIC, 91, int)
#define FPGA_IOC_RESET_PLX          _IOWR(FPGA_IOCMAGIC, 92, int)
#define FPGA_IOC_PLX_TEST           _IOWR(FPGA_IOCMAGIC, 93, int)
#define FPGA_IOC_BOARD_STATUS       _IOWR(FPGA_IOCMAGIC, 94, int)
#define FPGA_IOC_BOARD_REVISION     _IOWR(FPGA_IOCMAGIC, 95, int)
#define FPGA_IOC_SET_DRIVERENABLE   _IOWR(FPGA_IOCMAGIC, 96, int)
#define FPGA_IOC_BOARD_INIT         _IOWR(FPGA_IOCMAGIC, 97, int)
#define FPGA_IOC_BOARD_EXIT         _IOWR(FPGA_IOCMAGIC, 98, int)
#define FPGA_IOC_LOCK_ADDSIDELUT    _IOWR(FPGA_IOCMAGIC, 99, int)
#define FPGA_IOC_LOCK_DROPSIDELUT   _IOWR(FPGA_IOCMAGIC, 100, int)
#define FPGA_IOC_LOCK_DISPLAYLUT    _IOWR(FPGA_IOCMAGIC, 101, int)
#define FPGA_IOC_LOCK_RDPIXEL       _IOWR(FPGA_IOCMAGIC, 102, int)
#define FPGA_IOC_RESET_FPGA         _IOWR(FPGA_IOCMAGIC, 103, int)
#define FPGA_IOC_GET_HARDWARE_INFO  _IOWR(FPGA_IOCMAGIC, 104, struct hardware_info)
#define FPGA_IOC_GET_HWCOUNT    _IOWR(FPGA_IOCMAGIC, 105, int)

#define FPGA_IOC_GET_OV_PARAM   _IOWR(FPGA_IOCMAGIC, 120, struct ov_param_io)
#define FPGA_IOC_SET_OV_PARAM   _IOWR(FPGA_IOCMAGIC, 121, struct ov_param_io)
#define FPGA_IOC_GET_OV_INFO    _IOWR(FPGA_IOCMAGIC, 122, struct ov_param_io)

#define FPGA_IOC_FRAMECNT_ENABLE        _IOWR(FPGA_IOCMAGIC, 124, int)
#define FPGA_IOC_PROGCMPRS_ENABLE       _IOWR(FPGA_IOCMAGIC, 125, int)
#define FPGA_IOC_SET_SLICE_DIMENSION    _IOWR(FPGA_IOCMAGIC, 126, struct slice_dimension)
#define FPGA_IOC_GET_CODEC_VERSION       _IOWR(FPGA_IOCMAGIC, 127, int)
#endif  /* __FPGA_IOCTL_H_INCLUDED__ */
