#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <syslog.h>

#include "commonutils.h"
#include "version.h"
#include "device_detection.h"

// Return the contents of first line of given file
static char * get_file_line(char * filename)
{
    FILE *fp;
    char *cp;
    static char buf[256];

    fp = fopen(filename, "r");
    if (fp == NULL) {
        strcpy(buf, "");
    }
    else {
        fgets(buf, 255, fp);
        if ((cp = strchr(buf, '\n')) != 0)
            *cp = '\0';
        fclose(fp);
    }

    return buf;
}

int get_hardware_profile(void)
{
    struct stat statbuf;
    int type = HARDWARE_PROFILE_UNKNOWN;
    char * cp;

    if (stat("/etc/config/hw_profile_dms.txt", &statbuf) == 0) {
        cp = get_file_line("/etc/config/hw_profile_dms.txt");
        if (strcmp(cp, "DMS-1000-Device") == 0)
            type = HARDWARE_PROFILE_DMS1000;
        else if (strcmp(cp, "DMS-1500-Device") == 0)
            type = HARDWARE_PROFILE_DMS1500;
        else if (strcmp(cp, "DMS-1550-Device") == 0)
            type = HARDWARE_PROFILE_DMS1550;
        else if (strcmp(cp, "DMS-1600-Device") == 0)
            type = HARDWARE_PROFILE_DMS1600;
        else if (strcmp(cp, "DMS-1700-Device") == 0)
            type = HARDWARE_PROFILE_DMS1700;
        else if (strcmp(cp, "DMS-1200-Device") == 0)
            type = HARDWARE_PROFILE_DMS1200;
        else if (strcmp(cp, "DMS-200-Device") == 0)
            type = HARDWARE_PROFILE_DMS200;
        else if (strcmp(cp, "DMS-100-Device") == 0)
            type = HARDWARE_PROFILE_DMS100;
        else if (strcmp(cp, "DMS-120-Device") == 0)
            type = HARDWARE_PROFILE_DMS120;
        else if (strcmp(cp, "DMS-25-Device") == 0)
            type = HARDWARE_PROFILE_DMS25;
        else if (strcmp(cp, "DMS-150-Device") == 0)
            type = HARDWARE_PROFILE_DMS150;

        else
            type = HARDWARE_PROFILE_DMS;
    }
    else if (stat("/etc/config/hw_profile_ms.txt", &statbuf) == 0) {
        cp = get_file_line("/etc/config/hw_profile_ms.txt");
        if (strcmp(cp, "MS-25-Device") == 0)
            type = HARDWARE_PROFILE_MS25;
        else if (strcmp(cp, "MS-150-Device") == 0)
            type = HARDWARE_PROFILE_MS150;
        else
            type = HARDWARE_PROFILE_MS;
    }
    else if (stat("/etc/config/hw_profile_v2dtx.txt", &statbuf) == 0) {
        type = HARDWARE_PROFILE_V2D_TX;
    }
    else if (stat("/etc/config/hw_profile_v2drx.txt", &statbuf) == 0) {
        type = HARDWARE_PROFILE_V2D_RX;
    }
    else if (stat("/etc/config/hw_profile_v2dxp200.txt", &statbuf) == 0) {
        cp = get_file_line("/etc/config/hw_profile_v2dxp200.txt");
        if (strcmp(cp, "V2D-XP200-Device") == 0)
            type = HARDWARE_PROFILE_XP200;
        else if (strcmp(cp, "V2D-XP200S-Device") == 0)
            type = HARDWARE_PROFILE_XP200S;
        else if (strcmp(cp, "V2D-XP220-Device") == 0)
            type = HARDWARE_PROFILE_XP220;
        else if (strcmp(cp, "V2D-XP220S-Device") == 0)
            type = HARDWARE_PROFILE_XP220S;
        else if (strcmp(cp, "V2D-XP100-Device") == 0)
            type = HARDWARE_PROFILE_XP100;
        else
            type = HARDWARE_PROFILE_XP200;
    } 
    else if (stat("/etc/config/hw_profile_xpi.txt", &statbuf) == 0) {
        type = HARDWARE_PROFILE_XPI;
    }
    else if (stat("/etc/config/arm_system", &statbuf) == 0) {
        type = HARDWARE_PROFILE_XPI;
    }
    else if (stat("/etc/config/hw_profile_orb.txt", &statbuf) == 0) { 
        cp = get_file_line("/etc/config/hw_profile_orb.txt"); 
        if (strcmp(cp, "ORB-500-Device") == 0) 
            type = HARDWARE_PROFILE_ORB500; 
    } 
    else if (stat("/etc/config/hw_profile_generic.txt", &statbuf) == 0) {
        type = HARDWARE_PROFILE_GENERIC;
    }

    return type;
}

int get_hardware_category(void)
{
    static int hardware_profile = 0;
    int hardware_category = HARDWARE_CATEGORY_UNKNOWN;

    if (hardware_profile == 0) {
        hardware_profile = get_hardware_profile();
    }
    switch (hardware_profile) {
    case HARDWARE_PROFILE_MS:
    case HARDWARE_PROFILE_MS25:
    case HARDWARE_PROFILE_MS150:
    case HARDWARE_PROFILE_DMS:
    case HARDWARE_PROFILE_DMS25:
    case HARDWARE_PROFILE_DMS100:
    case HARDWARE_PROFILE_DMS150:
    case HARDWARE_PROFILE_DMS200:
    case HARDWARE_PROFILE_DMS120:
    case HARDWARE_PROFILE_DMS1000:
    case HARDWARE_PROFILE_DMS1200:
    case HARDWARE_PROFILE_DMS1500:
    case HARDWARE_PROFILE_DMS1550:
    case HARDWARE_PROFILE_DMS1600:
    case HARDWARE_PROFILE_DMS1700:
    case HARDWARE_PROFILE_GENERIC:
        hardware_category = HARDWARE_CATEGORY_DMS;
        break;
    case HARDWARE_PROFILE_V2D_TX:
    case HARDWARE_PROFILE_V2D_RX:
        hardware_category = HARDWARE_CATEGORY_I50;
        break;
    case HARDWARE_PROFILE_ORB500:
    case HARDWARE_PROFILE_XP200:
    case HARDWARE_PROFILE_XP200S:
    case HARDWARE_PROFILE_XP220:
    case HARDWARE_PROFILE_XP220S:
    case HARDWARE_PROFILE_XP100:
        hardware_category = HARDWARE_CATEGORY_XP;
        break;
    case HARDWARE_PROFILE_XPI:
        hardware_category = HARDWARE_CATEGORY_XPI;
        break;
    default:
        hardware_category = HARDWARE_CATEGORY_UNKNOWN;
        break;
    }

    return hardware_category;
}

int get_software_version(char *buf, int maxlen)
{
    FILE *fp;
    char *cp;

    /* Get the version */
    fp = fopen(V2O_VERSION_FILE, "r");
    if (fp == NULL) {
        strncpy(buf, "", maxlen);
    }
    else {
        fgets(buf, maxlen, fp);
        if ((cp = strchr(buf, '\n')) != 0)
            *cp = '\0';
        fclose(fp);
    }
    return 0;
}
int get_software_serviceversion(char *buf, int maxlen)
{

    FILE *fp;
    char *cp;

    /* Get the service version */
    fp = popen(SERVICE_VERSION_CMD, "r");
    if (fp == NULL) {
        strncpy(buf, "", maxlen);
    }
    else {
        fgets(buf, maxlen, fp);
        if ((cp = strchr(buf, '\n')) != 0)
            *cp = '\0';
        fclose(fp);
    }

    return 0;
}

int get_software_date(char *buf, int maxlen)
{
    FILE *fp;
    char *cp;

    /* Get the version */
    fp = fopen(V2O_VERSION_DATE_FILE, "r");
    if (fp == NULL) {
        strncpy(buf, "", maxlen);
    }
    else {
        fgets(buf, maxlen, fp);
        if ((cp = strchr(buf, '\n')) != 0)
            *cp = '\0';
        fclose(fp);
    }
    return 0;
}

int get_device_serial_number(char *buf, int maxlen)
{

    FILE *fp;
    char *cp;

    fp = fopen(SERIAL_NUMBER_FILE, "r");
    if (fp == NULL) {
        strncpy(buf, "", maxlen);
    }
    else {
        fgets(buf, maxlen, fp);
        if ((cp = strchr(buf, '\n')) != 0)
            *cp = '\0';
        fclose(fp);
    }

    return 0;
}

int get_sd_card_id(char *buf, int maxlen)
{

    char * id_buf = get_file_line("/sys/block/mmcblk0/device/cid");
    strncpy(buf, id_buf, maxlen);
    return 0;
}
