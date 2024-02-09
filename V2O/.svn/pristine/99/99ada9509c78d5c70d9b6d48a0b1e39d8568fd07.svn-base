

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "disk_rebuild_menu_gen.h"


static int get_unit_from_diskId(int diskId) {
    int unitId = 0;

    int hardware_profile = get_hardware_profile();
    if (hardware_profile == HARDWARE_PROFILE_DMS1500) {
        if (diskId > 3 && diskId < 8)
            unitId = 1;
    }
    return unitId;
}

static int get_disk_group_from_diskid(int diskId, int *array, int *row) {

    int diskgroup = 0;

    // run perccli /c0 show all for DG Arr & Row

    switch(diskId) {

        case 0:
        case 1:
            diskgroup = 0;
            *array = 0;
            *row = (diskId == 0) ? 0 : 1;
                break;

        case 2:
        case 3:
            diskgroup = 1;
            *array = 0;
            *row = (diskId == 2) ? 0 : 1;
                break;

        case 4:
        case 5:
            diskgroup = 2;
            *array = 0;
            *row = (diskId == 4) ? 0 : 1;
                break;

        case 6:
        case 7:
            diskgroup = 3;
            *array = 0;
            *row = (diskId == 6) ? 0 : 1;
                break;
        default:
            break;

    }

    return diskgroup;

}

int applyDiskRebuild(struct disk_rebuild_params *params,
                   struct disk_rebuild_params *orig_params)
{
    char buff[64];
    int nDiskId;
    int nUnitId;
    int rc = 0;
    int diskgroup = 0;
    int array = 0;
    int row = 0;

    nDiskId = params->diskid_value;
    nUnitId = get_unit_from_diskId(nDiskId);

    printf("\n");
    printf("Warning: This operation will destroy all data on disk %d.\n", nDiskId);
    printf("\n");
    printf("Type \"Yes\" to continue... ");

    int confirm = 0;
    if (fgets(buff, 63, stdin)) {
        if (strncasecmp(buff, "yes", 3) == 0)
            confirm = 1;
    }
    if (!confirm) {
        printf("\n");
        printf("Rebuild operation aborted.\n");
        printf("\n");
        return RC_ERROR;
    }

    if (get_hardware_profile() != HARDWARE_PROFILE_DMS1550) {
        snprintf(buff, 63, "tw_cli maint remove c0 p%d",  nDiskId);
        system_direct_command(buff);
        snprintf(buff, 63, "tw_cli maint rescan c0");
        system_direct_command(buff);
        snprintf(buff, 63, "tw_cli maint rebuild c0 u%d p%d", nUnitId, nDiskId);
        rc = system_direct_command(buff);
    } else if (get_hardware_profile() == HARDWARE_PROFILE_DMS1550) {

        diskgroup = get_disk_group_from_diskid(nDiskId, &array, &row);
        snprintf(buff, 63, "perccli /c0/e%d/s%d insert dg=%d array=%d row=%d", 32, nDiskId, diskgroup, array, row);
        rc = system_direct_command(buff);
        snprintf(buff, 63, "perccli /c0/e%d/s%d start rebuild", 32, nDiskId);
        rc = system_direct_command(buff);
    }
    if (rc != 0) {
        return RC_ERROR;
    }

    return RC_NORMAL;

}

void DiskRebuildInit(void)
{
}

