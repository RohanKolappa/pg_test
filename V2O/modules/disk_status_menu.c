#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"
#include "commonutils.h"
#include "device_detection.h"
#include "disk_status_menu_gen.h"
#include "vlog.h"

char *constructHeaderDiskStatus(char *buffer)
{

        sprintf(buffer, "%-10s %-10s %-8s %-7s",
                            "Name", "RAID", "Battery", " % Used");

            return buffer;
}

char *constructRowDiskStatus(char *buffer,
                                    struct disk_status_params *params)
{
        sprintf (buffer, "%-10.10s %-10s %-8s %7.2f",
                params->name_value,
                params->raidstatus_value,
                params->bbustatus_value,
                params->percent_used_value);
    return buffer;
}
static int get_dsm1550_disk_details (int index, char *mount_point,
        struct disk_status_params *params) {

    FILE *inpipe;
    char inbuf[256];
    char sDiskSerialNumber[FIELD_DISK_STATUS_DISK_1_SERIAL_LENGTH+1];
    char sDiskStatus[FIELD_DISK_STATUS_DISK_1_STATUS_LENGTH +1];
    int  lineno = 0, nPort=0;
    char *cp;
    char cmdbuf[256];
    char unit[256];
    char raid[256];
    int  diskgroup = 2;
    int vdisk = 0;char buf[256];
    int i = 0;
    int found = 0;

    /* Issue the df command to find disk info */
    char *df_command = "/bin/df ";
    char *raidinfo_command = "/bin/perccli ";

    sprintf (cmdbuf, "%s %s 2>/dev/null", df_command, mount_point);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_ERROR;
    }

    /* Derive unit number from partition info in mount point*/


    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        lineno++;
        if (lineno > 1) {

            cp = strtok(inbuf, " \t");
            if (cp == NULL)
                continue;

            /* Values reported are in 1K-blocks so we scale all values
             * by 1024 */
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            params->total_value = atoll(cp) * 1024;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            params->used_value = atoll(cp) * 1024;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            params->available_value = atoll(cp) * 1024;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            strncpy (params->name_value, mount_point,
                    FIELD_DISK_STATUS_NAME_LENGTH);
            if (strncmp(cp, mount_point, strlen(mount_point)) != 0) {
                params->available_value = 0;
                params->used_value = 0;
                params->total_value = 0;
                continue;
            }


            /* We ignore the used value reported by the system and compute
               our own based on the available value */
            params->used_value = params->total_value - params->available_value;

            if (params->total_value > 0) {

                /* If we get this far, we found our hard drive and it is
                 * really mounted */
                params->status_value = 1;

                params->percent_used_value =
                    ((params->total_value - params->available_value) * 100.0)/params->total_value;
            }
            else {
                params->percent_used_value = 0.0;
            }

            break;
        }

    }
    pclose(inpipe);
    if ((get_hardware_profile() == HARDWARE_PROFILE_DMS25) ||
            (get_hardware_profile() == HARDWARE_PROFILE_MS25)) {
        strncpy(params->raidstatus_value,"NONE",
                FIELD_DISK_STATUS_RAIDSTATUS_LENGTH);
        return RC_NORMAL;
    }
    strncpy(params->raidstatus_value,"ERROR",
            FIELD_DISK_STATUS_RAIDSTATUS_LENGTH);

    sprintf (unit, "%d", index);
    sprintf (raid, "%d/%d", index, index);

    sprintf (cmdbuf, "%s /c0  show   2>/tmp/x", raidinfo_command);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_NORMAL;
    }
    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        cp = strtok(inbuf, " \t");
        if (cp == NULL)
            break;
        if (strcmp(cp, "FW") == 0) {
            cp = strtok(NULL, " \t");
            if (strcmp(cp, "Version") == 0)
                cp = strtok(NULL, "=");
            else 
                continue;
        }
        else {
            continue;
        }
        strncpy(params->firmwarever_value, cp,
                FIELD_DISK_STATUS_FIRMWAREVER_LENGTH);
        break;
    }
    pclose(inpipe);
    sprintf (cmdbuf, "%s /c0/bbu  show   2>/tmp/x", raidinfo_command);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_NORMAL;
    }
    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        cp = strtok(inbuf, " \t");
        if (cp == NULL)
            break;
        if (strcmp(cp, "BBU") == 0)
            cp = strtok(NULL, " \t");
        else 
            continue;

        strncpy(params->bbustatus_value,cp,
                FIELD_DISK_STATUS_BBUSTATUS_LENGTH);
    }
    pclose(inpipe);
    switch (get_hardware_profile()) {
        case HARDWARE_PROFILE_DMS1550:
            switch(index) {
                case 0:
                    strcpy(params->disk_1_status_value, "ERROR");
                    strcpy(params->disk_2_status_value, "ERROR");
                    vdisk = 0;
                    break;
                case 1:
                    strcpy(params->disk_3_status_value, "ERROR");
                    strcpy(params->disk_4_status_value, "ERROR");
                    vdisk = 2;
                    break;
                case 2:
                    strcpy(params->disk_5_status_value, "ERROR");
                    strcpy(params->disk_6_status_value, "ERROR");
                    vdisk = 4;
                    break;
                case 3:
                    strcpy(params->disk_7_status_value, "ERROR");
                    strcpy(params->disk_8_status_value, "ERROR");
                    vdisk = 6;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;

    }


    sprintf (cmdbuf, "%s /c0 show  2>/tmp/x", raidinfo_command);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_NORMAL;
    }

    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        cp = strtok(inbuf, " \t");
        if (cp == NULL)
            continue;
        if (strcmp(cp, raid)== 0) {
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            strncpy(params->raidtype_value, cp,
                    FIELD_DISK_STATUS_RAIDTYPE_LENGTH);

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            strncpy(params->raidstatus_value,cp,
                    FIELD_DISK_STATUS_RAIDSTATUS_LENGTH);
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            /* currently setting progressive value to empty, need to find cli */
            strncpy(params->progress_value, "-",
                    FIELD_DISK_STATUS_PROGRESS_LENGTH);
        }
        else if (*cp == '3') {
            cp++;
            if (*cp != '2')
                continue;
            // cp++;
            cp = strtok(NULL, " \t");
            strncpy(sDiskSerialNumber, cp,
                    FIELD_DISK_STATUS_DISK_1_SERIAL_LENGTH);
            sscanf (cp, "%d", &nPort);
            cp = strtok(NULL, " \t");

            if (cp == NULL)
                continue;
            strncpy(sDiskStatus,cp,
                    FIELD_DISK_STATUS_DISK_1_STATUS_LENGTH);

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            if (strcmp(cp, unit) != 0)
                continue;
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;

            switch(nPort) {
                case 0:
                    strcpy(params->disk_1_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_1_status_value, sDiskStatus);
                    break;
                case 1:
                    strcpy(params->disk_2_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_2_status_value, sDiskStatus);
                    break;
                case 2:
                    strcpy(params->disk_3_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_3_status_value, sDiskStatus);
                    break;
                case 3:
                    strcpy(params->disk_4_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_4_status_value, sDiskStatus);
                    break;
                case 4:
                    strcpy(params->disk_5_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_5_status_value, sDiskStatus);
                    break;
                case 5:
                    strcpy(params->disk_6_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_6_status_value, sDiskStatus);
                    break;
                case 6:
                    strcpy(params->disk_7_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_7_status_value, sDiskStatus);
                    break;
                case 7:
                    strcpy(params->disk_8_serial_value, sDiskSerialNumber);
                    strcpy(params->disk_8_status_value, sDiskStatus);
                    break;
            }
        }
    }
    pclose(inpipe);
    for (i = 0; i < diskgroup; i++) {
        sprintf (cmdbuf, "%s /c0/e32/s%d  show rebuild  2>/tmp/x", raidinfo_command, vdisk + i);
        inpipe = popen(cmdbuf, "r");
        if (inpipe == NULL) {
            return RC_NORMAL;
        }
        while (fgets(inbuf, sizeof(inbuf), inpipe)) {
            cp = strtok(inbuf, " \t");
            if (cp == NULL)
                break;
            sprintf(buf, "/c0/e32/s%d", vdisk + i);
            if (strcmp(cp, buf) == 0) {
                cp = strtok(NULL, " \t");
                strncpy(params->progress_value, cp,
                        FIELD_DISK_STATUS_PROGRESS_LENGTH);
                found = 1;
                if (strcmp(cp, "-") == 0)
                    found = 0;
            }

        }
        pclose(inpipe);
        if (found)
            break;
    }

    if (strcasecmp(params->raidstatus_value, "INOPERABLE") == 0)
        params->status_value = 0;

    return RC_NORMAL;
}


static int get_disk_details (int index, char *mount_point,
                          struct disk_status_params *params) {

    FILE *inpipe;
    char inbuf[256];
    char sDiskSerialNumber[FIELD_DISK_STATUS_DISK_1_SERIAL_LENGTH+1];
    char sDiskStatus[FIELD_DISK_STATUS_DISK_1_STATUS_LENGTH +1];
    int  lineno = 0, nPort=0;
    char *cp;
    char cmdbuf[256];
    char unit[256];

    /* Issue the df command to find disk info */
    char *df_command = "/bin/df ";
    char *raidinfo_command = "/bin/tw_cli ";

    sprintf (cmdbuf, "%s %s 2>/dev/null", df_command, mount_point);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_ERROR;
    }

    /* Derive unit number from partition info in mount point*/


    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        lineno++;
        if (lineno > 1) {

            cp = strtok(inbuf, " \t");
            if (cp == NULL)
                continue;

            /* Values reported are in 1K-blocks so we scale all values
             * by 1024 */
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            params->total_value = atoll(cp) * 1024;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            params->used_value = atoll(cp) * 1024;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            params->available_value = atoll(cp) * 1024;

            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            cp = strtok(NULL, " \t");
            if (cp == NULL)
                continue;
            strncpy (params->name_value, mount_point,
                    FIELD_DISK_STATUS_NAME_LENGTH);
            if (strncmp(cp, mount_point, strlen(mount_point)) != 0) {
                params->available_value = 0;
                params->used_value = 0;
                params->total_value = 0;
                continue;
            }


            /* We ignore the used value reported by the system and compute
            our own based on the available value */
            params->used_value = params->total_value - params->available_value;

            if (params->total_value > 0) {

                /* If we get this far, we found our hard drive and it is
                 * really mounted */
                params->status_value = 1;

                params->percent_used_value =
                    ((params->total_value - params->available_value) * 100.0)/params->total_value;
            }
            else {
                params->percent_used_value = 0.0;
            }

            break;
        }

    }
    pclose(inpipe);
    if ((get_hardware_profile() == HARDWARE_PROFILE_DMS25) ||
            (get_hardware_profile() == HARDWARE_PROFILE_MS25)) {
        strncpy(params->raidstatus_value,"NONE",
            FIELD_DISK_STATUS_RAIDSTATUS_LENGTH);
        return RC_NORMAL;
    }
    strncpy(params->raidstatus_value,"ERROR",
            FIELD_DISK_STATUS_RAIDSTATUS_LENGTH);

    sprintf (unit, "u%d", index);

    sprintf (cmdbuf, "%s /c0 show firmware 2>/tmp/x", raidinfo_command);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_NORMAL;
    }
    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        cp = strtok(inbuf, "=");
        if (cp == NULL)
            break;
        cp = strtok(NULL, "=");
        if (cp == NULL)
            break;
        strncpy(params->firmwarever_value, cp,
            FIELD_DISK_STATUS_FIRMWAREVER_LENGTH);
    }
    pclose(inpipe);
    sprintf (cmdbuf, "%s /c0/bbu show status 2>/tmp/x", raidinfo_command);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_NORMAL;
    }
    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        cp = strtok(inbuf, "=");
        if (cp == NULL)
            break;
        cp = strtok(NULL, "= \n");
        if (cp == NULL)
            break;
        strncpy(params->bbustatus_value,cp,
                FIELD_DISK_STATUS_BBUSTATUS_LENGTH);
    }
    pclose(inpipe);
    switch (get_hardware_profile()) {
        case HARDWARE_PROFILE_MS:
        case HARDWARE_PROFILE_DMS100:
            strcpy(params->disk_1_status_value, "ERROR");
            strcpy(params->disk_2_status_value, "ERROR");
            break;
        case HARDWARE_PROFILE_DMS200:
            strcpy(params->disk_1_status_value, "ERROR");
            strcpy(params->disk_2_status_value, "ERROR");
            strcpy(params->disk_3_status_value, "ERROR");
            strcpy(params->disk_4_status_value, "ERROR");
            break;
        case HARDWARE_PROFILE_DMS1200:
        case HARDWARE_PROFILE_DMS1600:
        case HARDWARE_PROFILE_DMS1700:
            strcpy(params->disk_1_status_value, "ERROR");
            strcpy(params->disk_2_status_value, "ERROR");
            strcpy(params->disk_3_status_value, "ERROR");
            strcpy(params->disk_4_status_value, "ERROR");
            strcpy(params->disk_5_status_value, "ERROR");
            strcpy(params->disk_6_status_value, "ERROR");
            strcpy(params->disk_7_status_value, "ERROR");
            strcpy(params->disk_8_status_value, "ERROR");
            break;
        case HARDWARE_PROFILE_DMS1500:
            switch(index) {
                case 0:
                    strcpy(params->disk_1_status_value, "ERROR");
                    strcpy(params->disk_2_status_value, "ERROR");
                    strcpy(params->disk_3_status_value, "ERROR");
                    strcpy(params->disk_4_status_value, "ERROR");
                    break;
                case 1:
                    strcpy(params->disk_5_status_value, "ERROR");
                    strcpy(params->disk_6_status_value, "ERROR");
                    strcpy(params->disk_7_status_value, "ERROR");
                    strcpy(params->disk_8_status_value, "ERROR");
                    break;
                default:
                    break;
            }
            break;
        default:
            break;

    }


    sprintf (cmdbuf, "%s /c0 show unitstatus drivestatus 2>/tmp/x", raidinfo_command);
    inpipe = popen(cmdbuf, "r");
    if (inpipe == NULL) {
        return RC_NORMAL;
    }

    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
            cp = strtok(inbuf, " \t");
            if (cp == NULL)
                continue;
            if (strcmp(cp, unit)== 0) {
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                strncpy(params->raidtype_value, cp,
                    FIELD_DISK_STATUS_RAIDTYPE_LENGTH);

                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                strncpy(params->raidstatus_value,cp,
                    FIELD_DISK_STATUS_RAIDSTATUS_LENGTH);
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                strncpy(params->progress_value,cp,
                    FIELD_DISK_STATUS_PROGRESS_LENGTH);
            }
            else if (*cp == 'p') {
                sscanf (cp, "p%d", &nPort);
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                strncpy(sDiskStatus,cp,
                        FIELD_DISK_STATUS_DISK_1_STATUS_LENGTH);
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                if (strcmp(cp, unit) != 0)
                    continue;
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;
                cp = strtok(NULL, " \t");
                if (cp == NULL)
                    continue;

                strncpy(sDiskSerialNumber, cp,
                        FIELD_DISK_STATUS_DISK_1_SERIAL_LENGTH);
                switch(nPort) {
                    case 0:
                        strcpy(params->disk_1_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_1_status_value, sDiskStatus);
                        break;
                    case 1:
                        strcpy(params->disk_2_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_2_status_value, sDiskStatus);
                        break;
                    case 2:
                        strcpy(params->disk_3_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_3_status_value, sDiskStatus);
                    break;
                    case 3:
                        strcpy(params->disk_4_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_4_status_value, sDiskStatus);
                        break;
                    case 4:
                        strcpy(params->disk_5_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_5_status_value, sDiskStatus);
                        break;
                    case 5:
                        strcpy(params->disk_6_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_6_status_value, sDiskStatus);
                        break;
                    case 6:
                        strcpy(params->disk_7_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_7_status_value, sDiskStatus);
                        break;
                    case 7:
                        strcpy(params->disk_8_serial_value, sDiskSerialNumber);
                        strcpy(params->disk_8_status_value, sDiskStatus);
                        break;
                }
            }
    }
    pclose(inpipe);

    if (strcasecmp(params->raidstatus_value, "INOPERABLE") == 0)
        params->status_value = 0;

    return RC_NORMAL;
}
int getDiskStatus(struct disk_status_params *params) {

    FILE *fp;
    char buff[256];
    char *mount_point;
    int idx=0;

    /* The hard disk mount point is stored in a special file during
       device bootup */
    /* Warning: For changing this file name see DRY.3 */
    fp = fopen("/etc/config/hd_mount_point.txt", "r");
    if ( fp == NULL) {
        // no hard drive present
        return RC_NORMAL;
    }
    while(fgets(buff, 255, fp)) {
        mount_point = strtok(buff, "\r\n \t");
        params = getDiskStatusParamsStruct(params);
        if (get_hardware_profile() == HARDWARE_PROFILE_DMS1550)
            get_dsm1550_disk_details(idx++, mount_point, params);
        else
            get_disk_details(idx++, mount_point, params);

    }

    fclose(fp);
    return RC_NORMAL;
}
