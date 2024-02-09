#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "commonutils.h"
#include "device_detection.h"
#include "custmenu.h"
#include "vlib.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_vpd.h"
#include "firmware_image_status_menu_gen.h"

static char *mon_str[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char *constructHeaderFirmwareImageStatus(char *buffer)
{

    int hardware_category = get_hardware_category();
    if (hardware_category == HARDWARE_CATEGORY_XP) 
        sprintf(buffer, "%-12s %-22s %-12s","Firmware", "Date", "Board Serial");
    else 
        sprintf(buffer, "%-12s %-22s", "Firmware" ,"Date");
    

    return buffer;
}


char *constructRowFirmwareImageStatus(char *buffer,
                            struct firmware_image_status_params *params)
{
    int hardware_category = get_hardware_category();
    if (hardware_category == HARDWARE_CATEGORY_XP) 
        sprintf(buffer, "%-12s %-22s %-12s",
                params->board_firmware_value,
                params->firmware_date_value,
                params->board_serial_value);
    else 
        sprintf(buffer, "%-12s %-22s",
                params->board_firmware_value,
                params->firmware_date_value);
    
    return buffer;
}

static void get_hardware_data(char *buf, HAL *hp) {
    unsigned long mdate, serial[VPD_SERIAL_SIZE / 4], revision;
    int yy, mm, dd;

    if (hal_get_board_vpd(hp, &mdate, serial, &revision) < 0) {
        sprintf(buf, "N/A");
    }
    else {
        yy = (int) VPD_YEAR(mdate) + 2000;
        mm = (int) VPD_MONTH(mdate);
        dd = (int) VPD_DAY(mdate);
        if (mm > 11) mm = 11;
        sprintf(buf, "Rev %d.%d Manufactured %d %s %d",
            (int) ((revision & 0xffff0000) >> 16),
        (int) (revision & 0x0000ffff),
        dd, mon_str[mm], yy);
    }   
}

typedef struct _board_firmware_info {
    char board_serial[FIELD_FIRMWARE_IMAGE_STATUS_BOARD_SERIAL_LENGTH];
    char board_hardware[FIELD_FIRMWARE_IMAGE_STATUS_BOARD_HARDWARE_LENGTH];
} BoardFirmwareInfo;

int getFirmwareImageStatus(struct firmware_image_status_params *params) 
{
    int first_time = 0;

    int number_of_boards = 0, board = 0;
    HAL *hp = hal_acquire(board);
    int hardware_category = get_hardware_category();
    if (hardware_category == HARDWARE_CATEGORY_I50 ||
        hardware_category == HARDWARE_CATEGORY_XP ||
	hardware_category == HARDWARE_CATEGORY_XPI) {
        
        number_of_boards = hal_get_number_of_boards(hp);
        hal_release(hp);

        /* The below static array is going to store board serial and hardware 
         * value of firware version information. Populating these values is a 
         * very expensive call to do every 4 seconds and also this does not 
         * change inspite of changes the boardtype at runtime. The CPU utilization
         * increses drastically if this is called every 4 seconds hence the 
         * concept of caching in the static array and using chached values.
         */
        static BoardFirmwareInfo *boardFirmwareArray = NULL;
        if (boardFirmwareArray == NULL) {
            first_time = 1;
            boardFirmwareArray = malloc (sizeof(BoardFirmwareInfo) * number_of_boards);
        }

        for (board = 0; board < number_of_boards; board++) {
            hp = hal_acquire(board);
            if (hp == NULL)
                continue;

            params = getFirmwareImageStatusParamsStruct(params);
            hal_get_firmware_version(hp, params->board_firmware_value,
                    FIELD_FIRMWARE_IMAGE_STATUS_BOARD_FIRMWARE_LENGTH);
             
            hal_get_firmware_date(hp, params->firmware_date_value,
                    FIELD_FIRMWARE_IMAGE_STATUS_FIRMWARE_DATE_LENGTH);
            if (first_time == 1) {
                hal_get_board_serial_number(hp, params->board_serial_value,
                        FIELD_FIRMWARE_IMAGE_STATUS_BOARD_SERIAL_LENGTH);
                get_hardware_data(params->board_hardware_value, hp);

                strcpy(boardFirmwareArray[board].board_serial, params->board_serial_value);
                strcpy(boardFirmwareArray[board].board_hardware, params->board_hardware_value);
            }
            else {
                strcpy(params->board_serial_value, boardFirmwareArray[board].board_serial);
                strcpy(params->board_hardware_value, boardFirmwareArray[board].board_hardware);
            }
            hal_release(hp);
        }
        return RC_NORMAL;
    }
    else {
        return RC_ERROR;
    }
}
