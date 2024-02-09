#ifndef __NETVIZXPI_H_INCLUDE__
#define __NETVIZXPI_H_INCLUDE__

#ifndef _LINUX_IOCTL_H
#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/videodev2.h>
#else
#include <sys/ioctl.h>
#endif
#endif

#ifndef __FPGA_H_INCLUDED__
#include "fpga.h"
#endif

#define NETVIZXPI_MAJOR_RELEASE         (0)
#define NETVIZXPI_MINOR_RELEASE         (0)
#define NETVIZXPI_BUILD                 (1)

#define NETVIZXPI_DISPLAY_VERSION_CODE \
        ((NETVIZXPI_MAJOR_RELEASE << 16) | (NETVIZXPI_MINOR_RELEASE << 8) | \
         NETVIZXPI_BUILD)

#define PROC_FILE_SIZE                  0x2000

/* Bit settings for debug and flags */
#define NETVIZXPI_DEBUG_INFO            0x0001
#define NETVIZXPI_DEBUG_TRACE           0x0002
#define NETVIZXPI_DEBUG                 (NETVIZXPI_DEBUG_INFO | \
                                        NETVIZXPI_DEBUG_TRACE)
#define NETVIZXPI_DEBUG_PPS             0x0004
#define NETVIZXPI_DEBUG_VIDIN           0x0008
#define NETVIZXPI_DEBUG_VIDIN_TRACE     0x0010
#define NETVIZXPI_DEBUG_VIDOUT          0x0020
#define NETVIZXPI_DEBUG_VIDOUT_TRACE    0x0040
#define NETVIZXPI_DEBUG_LUT_INFO        0x0100
#define NETVIZXPI_DEBUG_LUT_TRACE       0x0200
#define NETVIZXPI_DEBUG_LUT             (NETVIZXPI_DEBUG_LUT_INFO | \
                                        NETVIZXPI_DEBUG_LUT_TRACE)
#define NETVIZXPI_DEBUG_AUTOLUT         0x0400

#define NETVIZXPI_FLAG_RESETFPGA        0x0001
#define NETVIZXPI_FLAG_RETRYRESET       0x0002
#define NETVIZXPI_FLAG_AUTOLUT          0x0004

/* ADV7604 system clock frequency */
#define ADV7604_fsc                     (28636360)

/* GPIO Pins */
#define GPIO_HPD_NOR_PROC               19
#define GPIO_DVI_TXS_PDN                32
#define GPIO_DVI_TXM_PDN                33
#define GPIO_7604_PDN                   34
#define GPIO_7604_RSTN                  35
#define GPIO_7612_RSTN                  36
#define GPIO_HPD_INT                    44
#define GPIO_HPD_CTRL                   45
#define GPIO_S6_RESET                   86
#define GPIO_S6_FMUX_SEL                88
#define GPIO_DVI1_RESET                 92
#define GPIO_V6FPGA_RSTn                93
#define GPIO_S6_PROGN                   95
#define GPIO_PHY0_RSTn                  96
#define GPIO_I2CSW_RSTN                 97
#define GPIO_DDC_I2CSW_SEL              98
#define GPIO_HDMI_SW_SEL                99
#define GPIO_RJ45_ORANGE                100
#define GPIO_RJ45_GREEN                 101
#define GPIO_RJ45_YELLOW                113

/* XPi IOCTL */
#define REGIO_TYPE_FPGA_GET                 1
#define REGIO_TYPE_FPGA_SET                 2
#define REGIO_TYPE_FPGA_ANDIN   	        3
#define REGIO_TYPE_FPGA_ORIN    	        4
#define REGIO_TYPE_ADV7612_GET		        5
#define REGIO_TYPE_ADV7612_SET		        6
#define REGIO_TYPE_ADV7604_GET		        7
#define REGIO_TYPE_ADV7604_SET		        8
#define REGIO_TYPE_SII7172_GET		        9
#define REGIO_TYPE_SII7172_SET		        10
#define REGIO_TYPE_MONEDID_GET	            11
#define REGIO_TYPE_MONEDID_SET	            12
#define REGIO_TYPE_SI5338_GET		        13
#define REGIO_TYPE_SI5338_SET		        14
#define REGIO_TYPE_LM63_GET	                15 
#define REGIO_TYPE_LM63_SET		            16 
#define REGIO_TYPE_ICS9FG104_GET	        17
#define REGIO_TYPE_ICS9FG104_SET	        18
#define REGIO_TYPE_MAX6650_GET		        19
#define REGIO_TYPE_MAX6650_SET		        20
#define REGIO_TYPE_EXPANSIONSWITCH_GET	    21
#define REGIO_TYPE_EXPANSIONSWITCH_SET	    22
#define REGIO_TYPE_SII7172_SLAVE_GET	    23
#define REGIO_TYPE_SII7172_SLAVE_SET        24
#define REGIO_TYPE_DDCI2C_GET               25
#define REGIO_TYPE_DDCI2C_SET               26
#define REGIO_TYPE_S6FPGA_GET               27
#define REGIO_TYPE_S6FPGA_SET     	        28
#define REGIO_TYPE_S6FPGA_ANDIN   	        29
#define REGIO_TYPE_S6FPGA_ORIN    	        30
#define REGIO_TYPE_S3FPGA_GET               31
#define REGIO_TYPE_S3FPGA_SET     	        32
#define REGIO_TYPE_S3FPGA_ANDIN   	        33
#define REGIO_TYPE_S3FPGA_ORIN    	        34
#define REGIO_TYPE_EXPANSIONEDID_GET	    35
#define REGIO_TYPE_EXPANSIONEDID_SET	    36
#define REGIO_TYPE_MAC_GET                  37
#define REGIO_TYPE_MAC_SET                  38

struct reg_io {
    int                 type;
    unsigned long       reg;
    unsigned long       value;
};

#define OMNITEKIO_TYPE_GET	            1
#define OMNITEKIO_TYPE_SET              2
#define OMNITEKIO_TYPE_ANDIN	        3
#define OMNITEKIO_TYPE_ORIN             4

struct omnitek_io {
    int                 type;
    int                 barno;
    unsigned long       reg;
    unsigned long       value;
};

#define NUM_I2C_BUSSES                  10
#define NUM_EXPANSION_I2C_BUSSES        8

#define EXPANSIONIO_TYPE_GET            1
#define EXPANSIONIO_TYPE_SET            2

struct expansion_io {
    int                 type;
    unsigned char       port;
    unsigned char       addr;
    unsigned char       reg;
    unsigned char       value;
};

#define PAGED_MAINMDIO_TYPE_GET         1
#define PAGED_EXPANSIONMDIO_TYPE_GET    2
#define PAGED_MAINMDIO_TYPE_SET         3 
#define PAGED_EXPANSIONMDIO_TYPE_SET    4

struct pagedmdio_io {
    int                 type;
    int                 page;    
    unsigned long       reg;
    unsigned long       value;
};

/* Expansion board I2C addresses */
#define TLV320AIC3254_I2C_PORT      0x00
#define TLV320AIC3254_I2C_ADDR      0x18
#define USBHOSTEEPROM_I2C_PORT      0x02
#define USBHOSTEEPROM_I2C_ADDR      0x50
#define SPARTAN3AN_I2C_PORT         0x04
#define SPARTAN3AN_I2C_ADDR         0x7F
#define BOARDIDEEPROM_I2C_PORT      0x05
#define BOARDIDEEPROM_I2C_ADDR      0x51
#define EXPANSIONLM63_I2C_PORT      0x06
#define EXPANSIONMAX6650_I2C_PORT   0x06
#define USBDEVEEPROM_I2C_PORT       0x07
#define USBDEVEEPROM_I2C_ADDR       0x50

#define LED_TYPE_ALLON                  1
#define LED_TYPE_ALLOFF                 2
#define LED_TYPE_NORMAL                 3

struct led_io {
    int                 lednumb;
    int                 leddata;
};

#define HARDWARE_FLAG_MAIN_LM63         0x0001
#define HARDWARE_FLAG_EXPANSION_LM63    0x0002
#define HARDWARE_FLAG_HAS_MAX6650       0x0004

struct hardware_info {
    int hardware_type;
    union {
        int hardware_flags;
        int hardware_version;
    };
    int board_type;
    int hardware_detected;
    int number_of_boards;
};

struct fc_param_io {
    unsigned long             frame_cnt;
    unsigned long             slice_cnt;
};

struct fc_wparam_io {
    unsigned long             wframe_cnt;
    unsigned long             wslice_cnt;
};

struct fc_queue_limits_io {
    unsigned long             video_read_queue_limit;
    unsigned long             audio_read_queue_limit;
    unsigned long             video_write_queue_limit;
    unsigned long             audio_write_queue_limit;
};

struct fc_av_device_stats {
    unsigned long             dma_requests;
    unsigned long             dma_busy;
    unsigned long             dma_lock_busy;
    unsigned long             dma_errors;
    unsigned long             dma_grants;
    unsigned long             fifo_empty;
    unsigned long             fifo_full;
    unsigned long             fifo_reset;
    unsigned long             fops_requests;
    unsigned long             fops_success;
    unsigned long             fops_errors;
    unsigned long             fops_bytes;
    unsigned long             sequence;
    unsigned long             queue_length;
    unsigned long             queue_full;
    unsigned long             queue_empty;
    unsigned long             queue_limit;
    unsigned long             buffer_drops;
    unsigned long             buffer_pool;
};

struct fc_stat_io {
    struct fc_av_device_stats video_stats;
    struct fc_av_device_stats audio_stats;
};

#define PC_SET_PARAM_XBLOCKS    0x00000001
#define PC_SET_PARAM_YLOW       0x00000002
#define PC_SET_PARAM_YHIGH      0x00000004
#define PC_SET_PARAM_RLOW       0x00000008
#define PC_SET_PARAM_RHIGH      0x00000010

struct pc_param_io {
    unsigned long             setmask;
    int                       pc_xblocks;
    int                       pc_ylow;
    int                       pc_yhigh;
    int                       pc_rlow;
    int                       pc_rhigh;
};

struct pixadjust_io {
    int                       pix_adjust;
    int                       pix_tweak_hi;
    int                       pix_tweak_lo;
};

struct kbm_param_io {
    unsigned long             setmask;
    unsigned long             kb_rbytes;
    unsigned long             kb_rdropped;
    unsigned long             ms_rbytes;
    unsigned long             ms_rdropped;
    unsigned long             kb_wbytes;
    unsigned long             kb_usb_wbytes;
    unsigned long             kb_wdropped;
    unsigned long             ms_wbytes;
    unsigned long             ms_usb_wbytes;
    unsigned long             ms_wdropped;
};

struct slice_dimension {
    int slice_width;
    int slice_height;
};

#define V4L2_SUBDEV_ADV7604	      1
#define V4L2_SUBDEV_ADV7612	      2
#define V4L2_SUBDEV_SII7172       3

/* Standard identification information */

struct stdi_readback {
   unsigned short bl, lcf, lcvs, fcl;
   unsigned char hs_pol, vs_pol;
   int interlaced;
};

struct edid_io {
    int                 subdev;
    unsigned char       edid[256];
};

struct stdi_io {
    int                     subdev;
    struct stdi_readback    stdi;
};

struct routing_io {
    int                 subdev;
    int                 input;
    int                 output;
    int                 config;
};
					
#define FPGA_IOCMAGIC             0xB5
#define FPGA_IOC_RESET            _IO(FPGA_IOCMAGIC,   0)
#define FPGA_IOC_RESET_LUT	      _IOWR(FPGA_IOCMAGIC, 1, struct LUT_struct)
#define FPGA_IOC_SET_LUT	      _IOWR(FPGA_IOCMAGIC, 2, struct LUT_struct)
#define FPGA_IOC_GET_LUT	      _IOWR(FPGA_IOCMAGIC, 3, struct LUT_struct)
#define FPGA_IOC_SET_LEDSTATE     _IOWR(FPGA_IOCMAGIC, 4, struct led_io)
#define FPGA_IOC_SET_LEDALIVE     _IOWR(FPGA_IOCMAGIC, 5, struct led_io)
#define FPGA_IOC_SET_LED          _IOWR(FPGA_IOCMAGIC, 6, int)
#define FPGA_IOC_MOD_REG          _IOWR(FPGA_IOCMAGIC, 7, struct reg_io)
#define FPGA_IOC_MOD_OMNITEK      _IOWR(FPGA_IOCMAGIC, 8, struct omnitek_io)
#define FPGA_IOC_MOD_EXPANSION    _IOWR(FPGA_IOCMAGIC, 9, struct expansion_io)
#define FPGA_IOC_MOD_PAGEDMDIO    _IOWR(FPGA_IOCMAGIC, 10, struct pagedmdio_io)

#define FPGA_IOC_GET_FPGA_FLAGS   _IOWR(FPGA_IOCMAGIC, 15, int)
#define FPGA_IOC_SET_FPGA_FLAGS   _IOWR(FPGA_IOCMAGIC, 16, int)
#define FPGA_IOC_SET_PPSCAPACITY  _IOWR(FPGA_IOCMAGIC, 17, unsigned long)
#define FPGA_IOC_SET_FREE_RUN     _IOWR(FPGA_IOCMAGIC, 18, int)
#define FPGA_IOC_SET_LOGLVL       _IOWR(FPGA_IOCMAGIC, 19, int)
#define FPGA_IOC_LOG_STATUS       _IOWR(FPGA_IOCMAGIC, 20, int)
#define FPGA_IOC_INTR_FIRE        _IOWR(FPGA_IOCMAGIC, 21, int)
#define FPGA_IOC_QUERY_STDI       _IOWR(FPGA_IOCMAGIC, 22, struct stdi_io)
#define FPGA_IOC_SUBDEV_GET_EDID  _IOWR(FPGA_IOCMAGIC, 23, struct edid_io)
#define FPGA_IOC_SUBDEV_SET_EDID  _IOWR(FPGA_IOCMAGIC, 24, struct edid_io)
#define FPGA_IOC_SUBDEV_S_ROUTING _IOWR(FPGA_IOCMAGIC, 25, struct routing_io)

#define FPGA_IOC_RESET_ADDSIDE	  _IOWR(FPGA_IOCMAGIC, 31, int)
#define FPGA_IOC_RESET_DROPSIDE	  _IOWR(FPGA_IOCMAGIC, 32, int)
#define FPGA_IOC_RESET_OUTPUT	  _IOWR(FPGA_IOCMAGIC, 33, int)
#define FPGA_IOC_GET_ADDSIDELUT	  _IOWR(FPGA_IOCMAGIC, 34, struct LUT_struct)
#define FPGA_IOC_SET_ADDSIDELUT	  _IOWR(FPGA_IOCMAGIC, 35, int)
#define FPGA_IOC_GET_DROPSIDELUT  _IOWR(FPGA_IOCMAGIC, 36, struct LUT_struct)
#define FPGA_IOC_SET_DROPSIDELUT  _IOWR(FPGA_IOCMAGIC, 37, struct LUT_struct)
#define FPGA_IOC_GET_DISPLAYLUT   _IOWR(FPGA_IOCMAGIC, 38, struct LUT_struct)
#define FPGA_IOC_SET_DISPLAYLUT   _IOWR(FPGA_IOCMAGIC, 39, struct LUT_struct)
#define FPGA_IOC_SET_SPLASH	      _IOWR(FPGA_IOCMAGIC, 40, struct LUT_struct)
#define FPGA_IOC_GET_VID_AVL      _IOWR(FPGA_IOCMAGIC, 41, int)
#define FPGA_IOC_SET_VID_AVL      _IOWR(FPGA_IOCMAGIC, 42, int)
#define FPGA_IOC_GET_HDELAY       _IOWR(FPGA_IOCMAGIC, 43, int)
#define FPGA_IOC_SET_HDELAY       _IOWR(FPGA_IOCMAGIC, 44, int)
#define FPGA_IOC_GET_VDELAY       _IOWR(FPGA_IOCMAGIC, 45, int)
#define FPGA_IOC_SET_VDELAY       _IOWR(FPGA_IOCMAGIC, 46, int)
#define FPGA_IOC_GET_PSHIFT       _IOWR(FPGA_IOCMAGIC, 47, unsigned char)
#define FPGA_IOC_SET_PSHIFT       _IOWR(FPGA_IOCMAGIC, 48, unsigned char)

#define FPGA_IOC_GET_AMQUANT      _IOWR(FPGA_IOCMAGIC, 55, int)
#define FPGA_IOC_SET_AMQUANT      _IOWR(FPGA_IOCMAGIC, 56, int)
#define FPGA_IOC_GET_ADCQUANT     _IOWR(FPGA_IOCMAGIC, 57, int)
#define FPGA_IOC_SET_ADCQUANT     _IOWR(FPGA_IOCMAGIC, 58, int)
#define FPGA_IOC_SET_ASLICEDROP   _IOWR(FPGA_IOCMAGIC, 59, int)
#define FPGA_IOC_GET_ANOBPSLICE   _IOWR(FPGA_IOCMAGIC, 60, int)
#define FPGA_IOC_SET_ANOBPSLICE   _IOWR(FPGA_IOCMAGIC, 61, int)
#define FPGA_IOC_GET_DNOBPSLICE   _IOWR(FPGA_IOCMAGIC, 62, int)
#define FPGA_IOC_SET_DNOBPSLICE   _IOWR(FPGA_IOCMAGIC, 63, int)
#define FPGA_IOC_GET_ACNOISET     _IOWR(FPGA_IOCMAGIC, 64, int)
#define FPGA_IOC_SET_ACNOISET     _IOWR(FPGA_IOCMAGIC, 65, int)
#define FPGA_IOC_GET_DCNOISET     _IOWR(FPGA_IOCMAGIC, 66, int)
#define FPGA_IOC_SET_DCNOISET     _IOWR(FPGA_IOCMAGIC, 67, int)
#define FPGA_IOC_GET_ACOMPMODE    _IOWR(FPGA_IOCMAGIC, 68, int)
#define FPGA_IOC_SET_ACOMPMODE    _IOWR(FPGA_IOCMAGIC, 69, int)
#define FPGA_IOC_GET_DCOMPMODE    _IOWR(FPGA_IOCMAGIC, 70, int)
#define FPGA_IOC_SET_DCOMPMODE    _IOWR(FPGA_IOCMAGIC, 71, int)
#define FPGA_IOC_SET_VIDEOCHECK   _IOWR(FPGA_IOCMAGIC, 72, int)
#define FPGA_IOC_SET_EXTSYNC      _IOWR(FPGA_IOCMAGIC, 73, int)
#define FPGA_IOC_SET_DIVIDER      _IOWR(FPGA_IOCMAGIC, 74, int)
#define FPGA_IOC_SET_ADD_DRIVERIDLE     _IOWR(FPGA_IOCMAGIC, 75, int)

#define FPGA_IOC_SET_DEFAULT_CPMP       _IOWR(FPGA_IOCMAGIC, 80, int)
#define FPGA_IOC_GET_DEFAULT_CPMP       _IOWR(FPGA_IOCMAGIC, 81, int)
#define FPGA_IOC_GET_INTERFACE          _IOWR(FPGA_IOCMAGIC, 82, int)
#define FPGA_IOC_SET_INTERFACE          _IOWR(FPGA_IOCMAGIC, 83, int)
#define FPGA_IOC_FORCE_INTERFACE        _IOWR(FPGA_IOCMAGIC, 84, int)
#define FPGA_IOC_GET_MONEDID            _IOWR(FPGA_IOCMAGIC, 85, int)
#define FPGA_IOC_KBM_FLUSH              _IOWR(FPGA_IOCMAGIC, 86, int)
#define FPGA_IOC_GET_LATENCY            _IOWR(FPGA_IOCMAGIC, 87, unsigned long)
#define FPGA_IOC_SET_FPGA_DROP          _IOWR(FPGA_IOCMAGIC, 88, int)
#define FPGA_IOC_GET_AVFLAG             _IOWR(FPGA_IOCMAGIC, 89, int)
#define FPGA_IOC_SET_AVFLAG             _IOWR(FPGA_IOCMAGIC, 90, int)
#define FPGA_IOC_GET_DISPLAY_HPOL       _IOWR(FPGA_IOCMAGIC, 91, int)
#define FPGA_IOC_SET_DISPLAY_HPOL       _IOWR(FPGA_IOCMAGIC, 92, int)
#define FPGA_IOC_GET_DISPLAY_VPOL       _IOWR(FPGA_IOCMAGIC, 93, int)
#define FPGA_IOC_SET_DISPLAY_VPOL       _IOWR(FPGA_IOCMAGIC, 94, int)

#define FPGA_IOC_PC_PARAM_GET       _IOWR(FPGA_IOCMAGIC, 100, struct pc_param_io)
#define FPGA_IOC_PC_PARAM_SET       _IOWR(FPGA_IOCMAGIC, 101, struct pc_param_io)
#define FPGA_IOC_BOARD_STATUS       _IOWR(FPGA_IOCMAGIC, 102, int)
#define FPGA_IOC_BOARD_REVISION     _IOWR(FPGA_IOCMAGIC, 103, int)
#define FPGA_IOC_SET_DRIVERENABLE   _IOWR(FPGA_IOCMAGIC, 104, int)
#define FPGA_IOC_BOARD_INIT         _IOWR(FPGA_IOCMAGIC, 105, int)
#define FPGA_IOC_BOARD_EXIT         _IOWR(FPGA_IOCMAGIC, 106, int)
#define FPGA_IOC_LOCK_ADDSIDELUT    _IOWR(FPGA_IOCMAGIC, 107, int)
#define FPGA_IOC_LOCK_DROPSIDELUT   _IOWR(FPGA_IOCMAGIC, 108, int)
#define FPGA_IOC_LOCK_DISPLAYLUT    _IOWR(FPGA_IOCMAGIC, 109, int)
#define FPGA_IOC_LOCK_RDPIXEL       _IOWR(FPGA_IOCMAGIC, 110, int)
#define FPGA_IOC_RESET_FPGA         _IOWR(FPGA_IOCMAGIC, 111, int)
#define FPGA_IOC_GET_HARDWARE_INFO  _IOWR(FPGA_IOCMAGIC, 112, struct hardware_info)
#define FPGA_IOC_GET_HWCOUNT        _IOWR(FPGA_IOCMAGIC, 113, int)

#define FPGA_IOC_GET_OV_PARAM   _IOWR(FPGA_IOCMAGIC, 120, struct ov_param_io)
#define FPGA_IOC_SET_OV_PARAM   _IOWR(FPGA_IOCMAGIC, 121, struct ov_param_io)
#define FPGA_IOC_GET_OV_INFO    _IOWR(FPGA_IOCMAGIC, 122, struct ov_param_io)
#define FPGA_IOC_FRAMECNT_ENABLE        _IOWR(FPGA_IOCMAGIC, 123, int)
#define FPGA_IOC_PROGCMPRS_ENABLE       _IOWR(FPGA_IOCMAGIC, 124, int)
#define FPGA_IOC_SET_SLICE_DIMENSION    _IOWR(FPGA_IOCMAGIC, 125, struct slice_dimension)
#define FPGA_IOC_GET_CODEC_VERSION       _IOWR(FPGA_IOCMAGIC, 126, int)

/* XPi MDIO Defines */
// Divider formula:  fMDC = fHOSTCLK / ((1 + CLOCK_DIVIDE[5:0])Ã2) with 
// fHOSTClK = 62.5MHz. 
// Workaround for slow MDIO bus with level converters for I2C --> '
// set clock divider to max --> 1MHz
//
#define MDIO_DIVIDER (0x0D)

#define MDIO_OP_SETADDR             (0x0)
#define MDIO_OP_WRITE               (0x1)
#define MDIO_OP_READINC             (0x2)
#define MDIO_OP_READ                (0x2)

/* MDIO offsets */
#ifndef XPI_MDIO_OFFSET
#define XPI_MDIO_OFFSET             0x2800
#endif
#ifndef XPI_MDIO_PHYAD
#define XPI_MDIO_PHYAD              1
#endif
#ifndef MAIN_OFFSET
#define MAIN_OFFSET                 0
#endif
#ifndef EXPANSION_OFFSET
#define EXPANSION_OFFSET            0x400
#endif

/* MDIO registers */

#define BARCO_MDIO_OPCODE           (0x00)
#define BARCO_MDIO_ADDRESS          (0x02)
#define BARCO_MDIO_WRITEDATA        (0x04)
#define BARCO_MDIO_READDATA         (0x06)
#define BARCO_MDIO_MIIMSEL          (0x08)
#define BARCO_MDIO_REQUEST          (0x0A)
#define BARCO_MDIO_MIIMREADY        (0x0C)
#define BARCO_MDIO_DRP_ADDRESS      (0x0E)
#define BARCO_MDIO_DRP_ENABLE       (0x10)
#define BARCO_MDIO_DRP_WRITEDATA    (0x12)
#define BARCO_MDIO_DRP_WRITEENABLE  (0x14)
#define BARCO_MDIO_DRP_READDATA     (0x16)
#define BARCO_MDIO_DRP_READY        (0x18)
#define BARCO_MDIO_MAC_IFG          (0x1A)
#define BARCO_MDIO_MAC_10GSEL       (0x1C)

/* Host bus select */
#define PRIMARY_MAC                 (0)
#define EXPANSION_MAC               (1)

/* ADV7604 and ADV7612 modes of operation */
#define NETVIZXPI_MODE_NONE         0
#define NETVIZXPI_MODE_COMP         1
#define NETVIZXPI_MODE_GR           2
#define NETVIZXPI_MODE_HDMI         3
#define NETVIZXPI_MODE_STDI         4
#define NETVIZXPI_MODE_TEST         5
#define NETVIZXPI_MODE_HDMI_COMP    ADV7612_MODE_HDMI_COMP
#define NETVIZXPI_MODE_HDMI_GR      ADV7612_MODE_HDMI_GR

/* NetvizXpi driver flag bits */
#define FLAG_XPI_TX                 (1 << 0)
#define FLAG_XPI_RX                 (1 << 1)
#define FLAG_XPI_DRIVER_ENABLE      (1 << 2)
#define FLAG_XPI_MAIN_LM63          (1 << 3)
#define FLAG_XPI_EXPANSION_LM63     (1 << 4)
#define FLAG_XPI_HAS_MAX6650        (1 << 5)
#define FLAG_XPI_DUALLINK_CHECK     (1 << 6)
#define FLAG_XPI_SPLASH_LOOPBACK    (1 << 7)
#define FLAG_XPI_LICENSE_VIOLATION  (1 << 8)
#define FLAG_XPI_FREE_RUNNING       (1 << 9)
#define FLAG_XPI_REFRESH_LUT        (1 << 11)
#define FLAG_XPI_VIDPROC_BASIC      (1 << 12)
#define FLAG_XPI_VIDPROC_DUALLINK   (1 << 13)
#define FLAG_XPI_VIDPROC_STEREO     (1 << 14)
#define FLAG_XPI_VIDPROC_PPSCONTROL (1 << 15)
#define FLAG_XPI_SETMASK            (FLAG_XPI_DRIVER_ENABLE | \
                                     FLAG_XPI_LICENSE_VIOLATION | \
                                     FLAG_XPI_REFRESH_LUT | \
                                     FLAG_XPI_VIDPROC_BASIC | \
                                     FLAG_XPI_VIDPROC_STEREO | \
                                     FLAG_XPI_VIDPROC_DUALLINK | \
                                     FLAG_XPI_VIDPROC_PPSCONTROL)

/* NetvizXpi V4L2 control identifiers */

#define NETVIZXPI_CID_BASE              V4L2_CID_PRIVATE_BASE
#define NETVIZXPI_CID_BRIGHTNESS        (NETVIZXPI_CID_BASE+0)
#define NETVIZXPI_CID_CONTRAST          (NETVIZXPI_CID_BASE+1)
#define NETVIZXPI_CID_SATURATION        (NETVIZXPI_CID_BASE+2)
#define NETVIZXPI_CID_HUE               (NETVIZXPI_CID_BASE+3)
#define NETVIZXPI_CID_AUDIO_MUTE        (NETVIZXPI_CID_BASE+4)
#define NETVIZXPI_CID_RX_RGB_RANGE      (NETVIZXPI_CID_BASE+5)
#define NETVIZXPI_CID_SAMPLING_PHASE    (NETVIZXPI_CID_BASE+6)
#define NETVIZXPI_CID_VIDIOC_MAX        NETVIZXPI_CID_SAMPLING_PHASE

#define NETVIZXPI_CID_HORIZ_POS         (NETVIZXPI_CID_BASE+7)
#define NETVIZXPI_CID_VERT_POS          (NETVIZXPI_CID_BASE+8)
#define NETVIZXPI_CID_MAX               NETVIZXPI_CID_VERT_POS

/* NetvizXpi V4L2 RGB range control values */

#define NETVIZXPI_RGB_RANGE_AUTO        0
#define NETVIZXPI_RGB_RANGE_LIMITED     1
#define NETVIZXPI_RGB_RANGE_FULL        2

#ifdef __KERNEL__

struct cropping_param_st {
    int hcrop;
    int vcrop;
    int dummy_pixels;
    int addr_in_line;
    int hcropleft;
    int hcropright;
    int vcroptop;
    int vcropbottom;
};


struct netvizxpi_state {
    struct v4l2_device v4l2_dev;
    struct video_device *vdev;
    struct platform_device *pdev;
    struct _OMNITEK_INTERFACE_EXTENSION *pExt;
    struct netvizxpi_platform_data *pdata;
    struct mutex lock;
    int flags;
    int users;
    struct proc_dir_entry *pdr;
    unsigned long reg6e0_value;

    struct i2c_adapter *i2c_adapters[NUM_I2C_BUSSES];
    struct i2c_client *i2c_expansion_client;
    struct i2c_adapter *i2c_expansion_adapters[NUM_EXPANSION_I2C_BUSSES];
    struct i2c_client *i2c_audiocodec_client;

    struct work_struct work;
    unsigned long av_timer_count; 
    struct timer_list av_timer; 

    /* maximum pixels per second for PPSCONTROL */
    unsigned long pps_capacity;

    /* mutex to ensure LUT accesses are seialized */
    struct mutex lut_lock;
    int lut_size;
    struct list_head lut_table_head;

    /* current add and/or drop interface flags & LUT */
    u32 current_interface;    
    struct LUT_struct *current_addside_lut;
    union {
        struct LUT_struct *current_dropside_lut;
        struct LUT_struct *current_splash_lut;
    };
    struct LUT_struct *current_display_lut;

    /* overlay configuration and overlay settings */
    struct ov_param_io ocfg;
    struct ov_param_io oinfo;

    struct v4l2_subdev *hdmi_adc_receiver;
    struct v4l2_subdev *hdmi_receiver;
    struct v4l2_subdev *hdmi_transmitter;

    struct workqueue_struct *work_queues;
    struct delayed_work delayed_work_sii7172;

    /* bandwidth count if video is enabled */
    unsigned long network_bw ;
    unsigned char hang_retry_cnt;
    unsigned long hang_reset_cnt;
};

#endif          /* __KERNEL__ */

#include "fpga.h"

#endif          /* __NETVIZXPI_H_INCLUDE__ */
