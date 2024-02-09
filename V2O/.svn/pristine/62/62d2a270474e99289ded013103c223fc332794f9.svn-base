#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "cpu_status_menu_gen.h"



static const int FSHIFT = 16;              /* nr of bits of precision */
#define FIXED_1         (1<<FSHIFT)     /* 1.0 as fixed-point */
#define LOAD_INT(x) ((x) >> FSHIFT)
#define LOAD_FRAC(x) LOAD_INT(((x) & (FIXED_1-1)) * 100)

int getCPUStatus(struct cpu_status_params *params) 
{

    int updays, uphours, upminutes, upseconds;
    struct sysinfo info;
    char buff[128];

    /* Get system info */
    sysinfo(&info);
    
    /* Calculate uptime */
    strcpy(params->uptime_value, "");
    updays = (int) info.uptime / (60*60*24);
    if (updays) {
        sprintf(buff, "%d day%s, ", updays, (updays != 1) ? "s" : "");
        strcat(params->uptime_value, buff);
    }
    upminutes = (int) info.uptime / 60;
    uphours = (upminutes / 60) % 24;
    upminutes %= 60;
    upseconds = (int) (info.uptime % 60);
    if (uphours)
        sprintf(buff, "%d hour%s, %d mins, %d secs", 
                uphours, (uphours != 1) ? "s" : "", upminutes, upseconds);
    else
        sprintf(buff, "%d mins, %d secs", upminutes, upseconds);

    strcat(params->uptime_value, buff);
    
    /* Calculate load average */
    params->load_value = LOAD_INT(info.loads[0]) + 
        (LOAD_FRAC(info.loads[0]) / 100.0);

    /* Get System/User/Idle Time from Streaming Server */

    return RC_NORMAL;
}
