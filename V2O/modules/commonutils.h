#ifndef __COMMONUTILS_H__
#define __COMMONUTILS_H__

#include <netinet/in.h>

#ifndef __HAL_H_INCLUDED__
#include "hal.h"
#endif

#ifndef __HAL_CONSOLE_H__
#include "hal_console.h"
#endif

#ifndef __HAL_HWINFO_H__
#include "hal_hwinfo.h"
#endif

#ifndef __HAL_EVENT_DEVICE_H__
#include "hal_event_device.h"
#endif

#ifndef __HAL_SERIAL_H__
#include "hal_serial.h"
#endif

#ifndef __HAL_LED_H__
#include "hal_LED.h"
#endif

#ifndef __HAL_VPD_H__
#include "hal_vpd.h"
#endif

#ifndef __FRAME_COUNTER_UTILS_H__
#include "frame_counter_utils.h"
#endif

/***********
 * Defines *
 ***********/

#define VIDEO_CONFIG_FILE "/V2O/config/vidconf.txt"

#define CONFIG_LOCK_FILE "/var/run/config_lock_file.lck"
#define REBOOT_LOCK_FILE "/var/run/reboot_lock_file.lck"
#define TEMPERATURE_PROC_FILE   "/proc/acpi/thermal_zone/THRM/temperature"

#define SERVER_PORT     6060
#define VIDEO_PORT      6060
#define AUDIO_PORT      (VIDEO_PORT + 1)

// TODO: Use from v2dlib
#define STATUSCODE_CLEAR                0
#define STATUSCODE_WRITEERR             1
#define STATUSCODE_READERR              2
#define STATUSCODE_CONNECTING           3
#define STATUSCODE_REFUSED              4
#define STATUSCODE_TIMEDOUT             5
#define STATUSCODE_UNREACHABLE          6
#define STATUSCODE_HOSTUNREACHABLE      7
#define STATUSCODE_DISCONNECTING        8
#define STATUSCODE_CONNECTED            9
#define STATUSCODE_WAITING              10
#define STATUSCODE_SERVERERR            11
#define STATUSCODE_MULTICASTERR         12
#define STATUSCODE_UNICASTERR           13
#define STATUSCODE_SERVERBUSY           14
#define STATUSCODE_AUTHERR              15
#define STATUSCODE_PROTERR              16
#define STATUSCODE_SOCKERR              17
#define STATUSCODE_STIMEDOUT            18
#define STATUSCODE_PREMPTED             19
#define STATUSCODE_MISMATCHERR          20
#define STATUSCODE_UNKNOWN              50

// END TODO

#define SERVER_ADDRESS_LEN              64

#define SHM_FLAG_MULTICAST              0x00000001
#define SHM_FLAG_VIDEO_ONLY             0x00000010
#define SHM_FLAG_AUDIO_ONLY             0x00000020
#define SHM_FLAG_AUDIO_BIDIR            0x00000040

#define DEBUG_FLAG_AUDIODUMP            0x0002

#define HARDWARE_PROFILE_UNKNOWN          0
#define HARDWARE_PROFILE_DMS              1
#define HARDWARE_PROFILE_V2D_TX           2
#define HARDWARE_PROFILE_V2D_RX           3
#define HARDWARE_PROFILE_MS               4
#define HARDWARE_PROFILE_XP200            5
#define HARDWARE_PROFILE_XP200S           6
#define HARDWARE_PROFILE_XP220            7
#define HARDWARE_PROFILE_XP220S           8
#define HARDWARE_PROFILE_XP100            9
#define HARDWARE_PROFILE_DMS100          10
#define HARDWARE_PROFILE_DMS200          11
#define HARDWARE_PROFILE_DMS120          12
#define HARDWARE_PROFILE_DMS1000         13
#define HARDWARE_PROFILE_DMS1200         14
#define HARDWARE_PROFILE_DMS1500         15
#define HARDWARE_PROFILE_DMS25           16
#define HARDWARE_PROFILE_MS25            17
#define HARDWARE_PROFILE_ORB500          18
#define HARDWARE_PROFILE_DMS1600         19
#define HARDWARE_PROFILE_DMS1700         20
#define HARDWARE_PROFILE_XPI             21
#define HARDWARE_PROFILE_DMS1550         23
#define HARDWARE_PROFILE_DMS150          24
#define HARDWARE_PROFILE_MS150           25
#define HARDWARE_PROFILE_GENERIC         50

#define HARDWARE_CATEGORY_UNKNOWN         100
#define HARDWARE_CATEGORY_DMS             101
#define HARDWARE_CATEGORY_I50             102
#define HARDWARE_CATEGORY_XP              103
#define HARDWARE_CATEGORY_XPI             104

#define CLIENT_CONFIG_FILE "/V2O/config/clientconf.txt"
#define SERVER_CONFIG_FILE "/V2O/config/serverconf.txt"

#define S_MCAST_ADDRESS         "MULTICAST_ADDRESS"
#define S_CTL_PORT              "CONTROL_PORT"
#define S_VIDEO_PORT            "VIDEO_PORT"
#define S_AUDIO_PORT            "AUDIO_PORT"
#define S_REV_AUDIO_PORT        "REV_AUDIO_PORT"

#define S_MCAST_TTL             "MULTICAST_TTL"
#define S_SERVER_ADDRESS        "SERVER_ADDRESS"
#define S_SERVER_PORT           "SERVER_PORT"
#define S_MAX_BW                "MAX_BANDWIDTH"
#define S_MULTICAST             "MULTICAST"
#define S_AVOPTION              "AVOPTION"
#define S_AUDIO_BIDIR           "BIDIR_AUDIO"
#define S_SAMPLESIZE            "SAMPLESIZE"
#define S_MONOSTEREO            "MONOSTEREO"
#define S_RFBCONNECTION         "RFBCONNECTION"
#define S_RFB_PORT              "RFBPORT"
#define S_RFB_PASSWORD          "RFBPASSWORD"
#define S_ENABLE_444            "ENABLE_444"
#define S_ALLOW_KBMSWITCH       "ALLOW_KBMSWITCH"
#define S_KBM_IDLETIME          "KBM_IDLETIME"
#define S_KBM_PACING_DELAY      "KBM_PACING_DELAY"
#define S_SPLASH_SCREEN         "SPLASH_SCREEN"
#define S_SPLASH_HRES           "SPLASH_HRES"
#define S_SPLASH_VRES           "SPLASH_VRES"
#define S_SPLASH_REFRESH        "SPLASH_REFRESH"
#define S_SPLASH_PCR            "SPLASH_PIXEL_CLOCK"
#define S_SPLASH_HFP            "SPLASH_HFP"
#define S_SPLASH_HBP            "SPLASH_HBP"
#define S_SPLASH_HSYNC          "SPLASH_HSYNC"
#define S_SPLASH_VFP            "SPLASH_VFP"
#define S_SPLASH_VBP            "SPLASH_VBP"
#define S_SPLASH_VSYNC          "SPLASH_VSYNC"
#define S_SPLASH_STEREO         "SPLASH_STEREO"
#define S_SPLASH_MQUANT         "SPLASH_MQUANT"
#define S_SPLASH_DCQUANT        "SPLASH_DCQUANT"
#define S_SPLASH_RATE           "SPLASH_RATE"
#define S_ENABLE_SPLASH         "ENABLE_SPLASH"
#define S_SERIAL_PORT           "SERIAL_PORT"
#define S_SERIAL_BAUDRATE       "SERIAL_BAUDRATE"
#define S_SERIAL_PARITY         "SERIAL_PARITY"
#define S_SERIAL_DATABITS       "SERIAL_DATABITS"
#define S_SERIAL_STOPBITS       "SERIAL_STOPBITS"
#define S_SERIAL_FLOWCONTROL    "SERIAL_FLOWCONTROL"
#define S_SERIAL_WAIT           "SERIAL_WAIT"
#define S_FPGA_RESETTIME        "FPGA_RESETTIME"
#define S_DDS_RESETTIME         "DDS_RESETTIME"
#define S_KB_TYPE               "KEYBOARD_TYPE" 
#define S_MS_TYPE               "MOUSE_TYPE"
#define S_BOARD_CONFIG          "BOARD_CONFIG"
#define S_VIDEOCHECK            "VIDEOCHECK"
#define S_DRIVERIDLE            "DRIVERIDLE"
#define S_VIDEO_DUMP            "VIDEO_DUMP"
#define S_AUDIO_DUMP            "AUDIO_DUMP"
#define S_REV_AUDIO_DUMP        "REV_AUDIO_DUMP"
#define S_TRANSMITTER           "TRANSMITTER"
#define S_RECEIVER              "RECEIVER"

/* bin files for version 1 codec */
#define S_LX100_TX_FPGA_FILE        "/V2O/firmware/v2d_tx_lx100.bin"
#define S_LX100_RX_FPGA_FILE        "/V2O/firmware/v2d_rx_lx100.bin"
#define S_LX80_TX_FPGA_FILE         "/V2O/firmware/v2d_tx_lx80.bin"
#define S_LX80_RX_FPGA_FILE         "/V2O/firmware/v2d_rx_lx80.bin"
/* bin files for version 2 codec */
#define S_LX100_TX_SVC_FPGA_FILE    "/V2O/firmware/v2d_tx_svc_lx100.bin"
#define S_LX100_RX_SVC_FPGA_FILE    "/V2O/firmware/v2d_rx_svc_lx100.bin"
#define S_LX80_TX_SVC_FPGA_FILE     "/V2O/firmware/v2d_tx_svc_lx80.bin"
#define S_LX80_RX_SVC_FPGA_FILE     "/V2O/firmware/v2d_rx_svc_lx80.bin"

#define UNKNOWN_CODEC_VERSION 0

// #define GET_REMOTE_IP(client) inet_ntoa((client)->remote_sa.sin_addr)
#define GET_REMOTE_IP(client) ((client)->remote_address)

#define LED_SCRLOCK        0x01
#define LED_NUMLOCK        0x02
#define LED_CAPSLOCK       0x04

#define FAN_SPEED_NONE     -1
#define FAN_SPEED_AUTO     0
#define FAN_SPEED_LOW      1
#define FAN_SPEED_MEDIUM   2
#define FAN_SPEED_HIGH     3
#define FAN_SPEED_OFF      4

#define TEMPMON_INTERVAL         60   /* Temperature monitor interval in secs */
#define TEMPMON_FLAG_RESET       0x01
#define TEMPMON_FLAG_PRINT       0x02

#define TEMP_SETPOINT_HIGH       70 
#define TEMP_SETPOINT_MEDIUM     67 
#define TEMP_SETPOINT_LOW        65 
#define TEMP_SETPOINT_VERY_LOW   62 

#define TEMP_RANGE_HIGH          5         
#define TEMP_RANGE_MEDIUM_2      4      
#define TEMP_RANGE_MEDIUM_1      3      
#define TEMP_RANGE_LOW_2         2      
#define TEMP_RANGE_LOW_1         1      
#define TEMP_NOCHANGE            0      

#define TIMEVAL_DIFF(tv1, tv2)                          \
    ({                                                  \
        int __delta_sec = (tv1).tv_sec - (tv2).tv_sec;  \
        int __delta = (tv1).tv_usec - (tv2).tv_usec;    \
        if (__delta_sec) {                              \
            switch (__delta_sec) {                      \
            default:                                    \
                __delta = 0;                            \
            case 9:                                     \
                __delta += 1000000;                     \
            case 8:                                     \
                __delta += 1000000;                     \
            case 7:                                     \
                __delta += 1000000;                     \
            case 6:                                     \
                __delta += 1000000;                     \
            case 5:                                     \
                __delta += 1000000;                     \
            case 4:                                     \
                __delta += 1000000;                     \
            case 3:                                     \
                __delta += 1000000;                     \
            case 2:                                     \
                __delta += 1000000;                     \
            case 1:                                     \
                __delta += 1000000;                     \
            }                                           \
        }                                               \
        __delta;                                        \
    })

#ifdef __cplusplus
extern "C" {
#endif
extern int audio_pack(unsigned char *obuf, unsigned char *ibuf, int ilen,
        int atype);
extern int audio_unpack(unsigned char *obuf, unsigned char *ibuf, int ilen,
        int atype);

extern int get_time_diff(struct timeval *last_time);
extern int is_standalone_enabled(void);
extern int is_xmppserver_enabled(void);
extern int is_linuxha_enabled(void);
extern int get_fan_speed_config(HAL *hp);
extern int get_configured_codec_version(HAL *hp);
#ifdef __cplusplus
}
#endif

#endif // __COMMONUTILS_H__
