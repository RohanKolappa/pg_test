#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <syslog.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "commonutils.h"
#include "device_detection.h"
#ifdef __XPI__
#include "netvizxpi/netvizxpi.h"
#include "xpi_i2c_control.h"
#endif

#include "vlog.h"

#define FPGAINFOBUF_SIZE        32
#define XPITYPE_ENCODER         0x0e
#define XPITYPE_DECODER         0x0d


int hal_get_board_number(HAL * hp)
{
    
    if (hp == NULL)
        return -1;

    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) < 0)
        return -1;

    return hp->board_number;
}

int hal_get_channel_number(HAL * hp)
{
    if (hp == NULL)
        return -1;
    return hal_get_board_number(hp) + 1;
}

int hal_get_number_of_boards(HAL * hp)
{
    if (hp == NULL)
        return -1;

    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) < 0)
        return -1;

    return hp->hw_info.number_of_boards;
}

int hal_get_hardware_type(HAL * hp)
{
   
    if (hp == NULL)
        return -1;

    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) < 0)
        return -1;
    return hp->hw_info.hardware_type;
}

char *monname[] = { "", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

int hal_get_xpi_info(HAL *hp, char *info, int maxinfo)
{
#ifdef __XPI__
    unsigned char buffer[FPGAINFOBUF_SIZE];
    int rc = 0, i, fd, err = -1, buildyear, buildmonth, buildday;
    int buildhour, buildminute, buildsecond;
    unsigned long regval;

    for (i = 0; i < (FPGAINFOBUF_SIZE + 3) / 4; i++) {
        regval = hal_get_omnitek_val_k(hp, i);
        memcpy(&buffer[4 * i], &regval, sizeof(unsigned long));
    }
    if (buffer[0] == XPITYPE_ENCODER || buffer[0] == XPITYPE_DECODER) {
        buildyear = buffer[11] << 24 | buffer[10] << 16 |
                buffer[9] << 8 | buffer[8];
        buildmonth = buffer[15] << 24 | buffer[14] << 16 |
                buffer[13] << 8 | buffer[12];
        buildday = buffer[19] << 24 | buffer[18] << 16 |
                buffer[17] << 8 | buffer[16];
        buildhour = buffer[23] << 24 | buffer[22] << 16 |
                buffer[21] << 8 | buffer[20];
        buildminute = buffer[27] << 24 | buffer[26] << 16 |
                buffer[25] << 8 | buffer[24];
        buildsecond = buffer[31] << 24 | buffer[30] << 16 |
                buffer[29] << 8 | buffer[28];
        if (buildmonth < 1 || buildmonth > 12) {
            snprintf(info, maxinfo, "%2d/%2d/%4d %02d:%02d:%02d\n",
                 buildmonth, buildday, buildyear, buildhour,
                 buildminute, buildsecond);
        }
        else {
            snprintf(info, maxinfo, "%s %2d %4d %02d:%02d:%02d\n",
                monname[buildmonth], buildday, buildyear, buildhour,
                buildminute, buildsecond);
        }
        return 0;
    }
#endif
    return -1;
}

int hal_get_hardware_version(HAL * hp)
{
    if (hp == NULL)
        return -1;

    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) < 0)
        return -1;

    return hp->hw_info.hardware_version;
}

int hal_get_hardware_revision(HAL *hp) {
   
    if (hp == NULL)
        return -1;
    return hal_get_hardware_version(hp) & 0x000000f;
}

int hal_get_board_type(HAL * hp)
{
    if (hp == NULL) 
        return -1;

    if (get_hardware_profile() == HARDWARE_PROFILE_XPI) {
        unsigned short idreg = hal_get_fpga_val_k(hp, FPGA_IDREG) & 0xf000;
        if (idreg == 0xa000)
            hp->hw_info.board_type = BOARDTYPE_TX;
        else if (idreg == 0xd000)  
            hp->hw_info.board_type = BOARDTYPE_RX;
    } else {
        if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) < 0)
           return -1;
    }
    return hp->hw_info.board_type;
}

int hal_get_hardware_detected(HAL * hp)
{
    if (hp == NULL)
        return -1;

    if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) < 0)
        return -1;
    return hp->hw_info.hardware_detected;
}

int hal_check_main_lm63(HAL * hp)
{
#ifdef __XPI__
    if (hp == NULL)
        return 0;
    return (hp->hw_info.hardware_flags & FLAG_XPI_MAIN_LM63) ? 1 : 0;
#else
    return 0;
#endif
}

int hal_check_expansion_lm63(HAL * hp)
{
#ifdef __XPI__
    if (hp == NULL)
        return 0;
    return (hp->hw_info.hardware_flags & FLAG_XPI_EXPANSION_LM63) ? 1 : 0;
#else
    return 0;
#endif
}

int hal_check_max6650(HAL * hp)
{
#ifdef __XPI__
    if (hp == NULL)
        return 0;
    return (hp->hw_info.hardware_flags & FLAG_XPI_HAS_MAX6650) ? 1 : 0;
#else
    return 0;
#endif
}

unsigned char hal_get_fpga_version(HAL * hp) 
{
    if (hal_get_hardware_type(hp) != XPI_DEVICE) {
        if (ioctl(hp->kernel_fd, FPGA_IOC_GET_HARDWARE_INFO, &hp->hw_info) < 0)
            return -1;
    }
    return (unsigned char) (hal_get_fpga_val_k(hp, FPGA_IDREG) & 0xff);
}

/* Return a descriptive string for the current hardware. Also does a
 * hardware check to verify that the configured hardware (Device.xml)
 * matches dicovered hardware and returns error (-1) if any unexpected
 * hardware detected. */
int hal_get_hardware_description(HAL *hp, char *buf, int maxlen)
{
    int retval = 0;
    int hardware_profile;
    char buf1[32];
    char buf2[32];

    /* Note that the provided HAL pointer argument may or may not be NULL */

    hardware_profile = get_hardware_profile();

    strcpy(buf2, "");

    /* Determine hardware display string */
    switch (hardware_profile) {

    case HARDWARE_PROFILE_MS:
        strcpy(buf1, "MS-100");
        break;
    case HARDWARE_PROFILE_MS25:
        strcpy(buf1, "MS-25");
        break;
    case HARDWARE_PROFILE_MS150:
        strcpy(buf1, "MS-150");
        break;
    case HARDWARE_PROFILE_DMS25:
        strcpy(buf1, "DMS-25");
        break;
    case HARDWARE_PROFILE_DMS100:
        strcpy(buf1, "DMS-100");
        break;
    case HARDWARE_PROFILE_DMS150:
        strcpy(buf1, "DMS-150");
        break;
    case HARDWARE_PROFILE_DMS200:
        strcpy(buf1, "DMS-200");
        break;
    case HARDWARE_PROFILE_DMS1000:
        strcpy(buf1, "DMS-1000");
        break;
    case HARDWARE_PROFILE_DMS1200:
        strcpy(buf1, "DMS-1200");
        break;
    case HARDWARE_PROFILE_DMS1500:
        strcpy(buf1, "DMS-1500");
        break;
    case HARDWARE_PROFILE_DMS1550:
        strcpy(buf1, "DMS-1550");
        break;
    case HARDWARE_PROFILE_DMS1600:
        strcpy(buf1, "DMS-1600");
        break;
    case HARDWARE_PROFILE_DMS1700:
        strcpy(buf1, "DMS-1700");
        break;
    case HARDWARE_PROFILE_DMS120:
        strcpy(buf1, "DMS-120");
        break;
    case HARDWARE_PROFILE_DMS:
        strcpy(buf1, "DMS");
        break;
    case HARDWARE_PROFILE_V2D_TX:
        strcpy(buf1, "V2D-I50-Tx");
        break;
    case HARDWARE_PROFILE_V2D_RX:
        strcpy(buf1, "V2D-I50-Rx");
        break; 
    case HARDWARE_PROFILE_XPI:
        strcpy(buf1, "NGS-D200");
        break;
    case HARDWARE_PROFILE_ORB500: 
        strcpy(buf1, "ORB-500"); 
        break; 
    case HARDWARE_PROFILE_XP200:
        strcpy(buf1, "V2D-XP200-D");
        break;
    case HARDWARE_PROFILE_XP200S:
        strcpy(buf1, "V2D-XP200-S");
        break;
    case HARDWARE_PROFILE_XP220:
        strcpy(buf1, "V2D-XP220-D");
        break;
    case HARDWARE_PROFILE_XP220S:
        strcpy(buf1, "V2D-XP220-S");
        break;
    case HARDWARE_PROFILE_XP100:
        strcpy(buf1, "V2D-XP100");
        break;
    case HARDWARE_PROFILE_GENERIC:
        strcpy(buf1, "Generic");
        break;
    }

    /* Simple scan of device to see if hardware matches */
    switch (hardware_profile) {

    case HARDWARE_PROFILE_MS:
    case HARDWARE_PROFILE_MS25:
    case HARDWARE_PROFILE_DMS:
    case HARDWARE_PROFILE_DMS25:
    case HARDWARE_PROFILE_DMS100:
    case HARDWARE_PROFILE_DMS200:
    case HARDWARE_PROFILE_DMS120:
    case HARDWARE_PROFILE_DMS1000:
    case HARDWARE_PROFILE_DMS1200:
    case HARDWARE_PROFILE_DMS1500:
    case HARDWARE_PROFILE_DMS1600:
    case HARDWARE_PROFILE_DMS1700:
        if (hp != NULL) {
            /* Not expecting to find any V2D/XP boards on the device */
            strcpy(buf2, "Error: Unexpected Hardware.");
            retval = -1;
        }
        break;

    case HARDWARE_PROFILE_V2D_TX:
        if (hp == NULL) {
            strcpy(buf2, "Error: Tx board not found.");
            retval = -1;
        }
        else {
            if (hal_get_hardware_type(hp) == V2DI50_DEVICE) {
                if (!hal_get_hardware_detected(hp)) {
                    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
                        strcpy(buf2, "Virtual");
                    }
                    else {
                        strcpy(buf2, "Error: Virtual-Rx??");
                        retval = -1;
                    }
                }
                else if (hal_get_board_type(hp) == BOARDTYPE_TX) {
                    if (hal_get_hardware_version(hp) == HARDWARE_VERSION_A)
                        strcpy(buf2, "A");
                    else
                        strcpy(buf2, "DA");
                }
                else {
                    strcpy(buf2, "Error: I50-Rx??");
                    retval = -1;
                }
            }
            else {
                strcpy(buf2, "Error: XP200??");
                retval = -1;
            }
        }
        break;

    case HARDWARE_PROFILE_V2D_RX:
        if (hp == NULL) {
            strcpy(buf2, "Error: Rx board not found.");
            retval = -1;
        }
        else {
            if (hal_get_hardware_type(hp) == V2DI50_DEVICE) {
                if (!hal_get_hardware_detected(hp)) {
                    if (hal_get_board_type(hp) == BOARDTYPE_RX) {
                        strcpy(buf2, "Virtual");
                    }
                    else {
                        strcpy(buf2, "Error: Virtual-Tx??");
                        retval = -1;
                    }
                }
                else if (hal_get_board_type(hp) == BOARDTYPE_RX) {
                    strcpy(buf2, "");
                }
                else {
                    strcpy(buf2, "Error: I50-Tx??");
                    retval = -1;
                }
            }
            else {
                strcpy(buf2, "Error: XP200??");
                retval = -1;
            }
        }
        break;

    case HARDWARE_PROFILE_XP200:
    case HARDWARE_PROFILE_XP220:
        if (hp == NULL) {
            strcpy(buf2, "Error: XP board(s) not found.");
            retval = -1;
        }
        else {
            if (hal_get_hardware_type(hp) != V2DI50_DEVICE) {
                if (hal_get_number_of_boards(hp) != 2) {
                    strcpy(buf2, "Error: Wrong number of boards.");
                    retval = -1;
                }
            }
            else {
                strcpy(buf2, "Error: I50 board??");
                retval = -1;
            }
        }

        break;
    case HARDWARE_PROFILE_XP200S:
    case HARDWARE_PROFILE_XP220S:
    case HARDWARE_PROFILE_XP100:
        if (hp == NULL) {
            strcpy(buf2, "Error: XP board(s) not found.");
            retval = -1;
        }
        else {
            if (hal_get_hardware_type(hp) != V2DI50_DEVICE) {
                if (hal_get_number_of_boards(hp) != 1) {
                    strcpy(buf2, "Error: Wrong number of boards.");
                    retval = -1;
                }
            }
            else {
                strcpy(buf2, "Error: I50 board??");
                retval = -1;
            }
        }

        break;

    case HARDWARE_PROFILE_GENERIC:
        break;

    default:
        retval = -1;
        break;
    }


    if (buf != NULL && maxlen > 0) {
        strncpy(buf, buf1, maxlen);
        if (strlen(buf2) > 0) {
            strncat(buf, " ", maxlen - strlen(buf));
            strncat(buf, buf2, maxlen - strlen(buf));
        }
    }


    return retval;
}

char * hal_utils_port_name(int board_type) {
    switch (board_type) {
    case BOARDTYPE_TX:
        return "Tx";
    case BOARDTYPE_RX:
        return "Rx";
    default:
        return "";
    }
}
