
#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>
#include <time.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"
#include "commonutils.h"
#include "text_utils.h"

#include "system_clock_status_menu_gen.h"

int getSystemClockStatus(struct system_clock_status_params *params) 
{

    char *cp;
    time_t t;
    int disabled = 0;

    /* Get the current time */
    if (time(&t) != -1) {
        sprintf(params->utctime_value, "%s", asctime(gmtime(&t)));
        if ((cp = strchr(params->utctime_value, '\n')) != 0)
            *cp = '\0';
    }

    /* Get the ntp status */
    FILE *fp = fopen("/tmp/ntp_status.txt", "r");
    if (fp == NULL){
        strcpy(params->ntp_status_value,"Disabled");
        disabled = 1;
    }
    else {
        fgets(params->ntp_status_value, 
              FIELD_SYSTEM_CLOCK_STATUS_NTP_STATUS_LENGTH, fp);
        if ((cp = strchr(params->ntp_status_value, '\n')) != 0)
            *cp = '\0';
        fclose(fp);
    }

    if (!disabled) {
        fp = fopen("/tmp/ntp_offset.txt", "r");
        if (fp == NULL){
            strcpy(params->ntp_offset_value, "");
        }
        else {
            char buff[64];
            double time_offset;
            fgets(buff, 63, fp);
            fclose(fp);
            time_offset = strtod(buff, NULL);
            time_offset = time_offset / 1000.0; // Convert to seconds
            snprintf(params->ntp_offset_value, FIELD_SYSTEM_CLOCK_STATUS_NTP_OFFSET_LENGTH,
                    "%f", time_offset);
        }
    }
    else {
        strcpy(params->ntp_offset_value, "");
    }


    /* Get ntp info */

    if (disabled) {
        params->ntp_info_value = strdup("");
        return RC_NORMAL;
    }

    FILE *inpipe;
    char inbuf[256];

    char *command = "/home/onair/bin/ntpq -p -n 2>&1";

    inpipe = popen(command, "r");
    
    params->ntp_info_value = strdup("\n");
    if (inpipe != NULL) {
        while (fgets(inbuf, sizeof(inbuf), inpipe)) {
            params->ntp_info_value = strgrowcat(params->ntp_info_value, inbuf);
        }
    }
    pclose(inpipe);

    return RC_NORMAL;
}
