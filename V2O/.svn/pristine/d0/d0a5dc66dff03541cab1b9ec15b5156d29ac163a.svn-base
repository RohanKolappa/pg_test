

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "vlog.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "disk_init_menu_gen.h"



int applyDiskInitialization(struct disk_init_params *params,
                   struct disk_init_params *orig_params)
{
    int nError=0;
    char buff[64];
    if ( !params->device_type_set) {
        VLOG_INFO("Must Specify Device Type \n");
        printf("Error: Must Specify Device Type \n");
        return RC_ERROR;
    }
    sprintf (buff, "/etc/scripts/diskutils.sh init ");

    switch ( params->device_type_value) {
        case DISK_INIT_DEVICE_TYPE_MS:
            strcat (buff, "MS");
            break;
        case DISK_INIT_DEVICE_TYPE_MSR2:
            strcat (buff, "MS-100-R2");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS100:
            strcat (buff, "DMS-100");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS100R2:
            strcat (buff, "DMS-100-R2");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS200:
            strcat (buff, "DMS-200");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS200R2:
            strcat (buff, "DMS-200-R2");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS1200:
            strcat (buff, "DMS-1200");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS1500:
            strcat (buff, "DMS-1500");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS1600:
            strcat (buff, "DMS-1600");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS1700:
            strcat (buff, "DMS-1700");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS1500R2:
            strcat (buff, "DMS-1500-R2");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS1600R2:
            strcat (buff, "DMS-1600-R2");
            break;
        case DISK_INIT_DEVICE_TYPE_DMS1700R2:
            strcat (buff, "DMS-1700-R2");
            break;
    }
    nError = system_command(buff);
    if (nError != 0) {
        VLOG_INFO("Failed to Initialize storage for %s : %d", params->device_type, nError);
        return RC_ERROR;
    }
    VLOG_INFO("Initialized storage for %s", params->device_type);
    return RC_NORMAL;

}

void DiskInitializationInit(void)
{
}

