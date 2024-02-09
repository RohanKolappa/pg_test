#include <stdlib.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "linuxha_status_menu_gen.h"
#define LINUXHASCRIPT  "/etc/scripts/watchdog/watch_linuxha.sh"

static int getPeerStatus(void)
{
    int nPeerStatus = 0;
    char* pchMyNodeName = NULL;
    char* pchListStatus = NULL;
    char* pchPeerStatus = NULL;

    do {
        char *pContext = NULL;
        char *pchDelim = " \n";
        char *pchToken = NULL;

        pchMyNodeName = get_command_buffer("uname -n");
        if (pchMyNodeName == NULL)
            break;

        pchListStatus = get_command_buffer("cl_status listnodes -n");
        if (pchListStatus == NULL)
            break;

        pchToken = strtok_r(pchListStatus, pchDelim, &pContext);
        while (pchToken && strcmp(pchToken, pchMyNodeName) == 0) {
            pchToken = strtok_r(NULL, pchDelim, &pContext);
        }

        if (pchToken) {
            char strQuery[256];
            sprintf(strQuery, "cl_status nodestatus %s", pchToken);
            pchPeerStatus = get_command_buffer(strQuery);
            if (pchPeerStatus == NULL)
                break;

            nPeerStatus = (strstr(pchPeerStatus, "active")) ? 1 : 0;
        }

    } while (0);

    if (pchMyNodeName) {
        free(pchMyNodeName);
        pchMyNodeName = NULL;
    }

    if (pchListStatus) {
        free(pchListStatus);
        pchListStatus = NULL;
    }

    if (pchPeerStatus) {
        free(pchPeerStatus);
        pchPeerStatus = NULL;
    }

    return nPeerStatus;
}

int getHAStatus(struct ha_status_params *params)
{
    do {
        char* pchClStatus = NULL;
        char* pchRscStatus = NULL;

        pchClStatus = get_command_buffer("cl_status hbstatus");
        if (pchClStatus) {

            char* pchHBStatus = strstr(pchClStatus, "running");
            params->hb_status_value = (pchHBStatus) ? 1 : 0;

            free(pchClStatus);
            pchClStatus = NULL;
        }
        else {
            break;
        }

        if (params->hb_status_value == 0)
            break;

        params->peer_status_value = getPeerStatus();

        char strQuery[256];
        sprintf(strQuery, "%s status_resource", LINUXHASCRIPT);
        pchRscStatus = get_command_buffer(strQuery);
        if (pchRscStatus) {

            char* pchRscStatus2 = strtok(pchRscStatus, " \t\n");
            if (pchRscStatus2) {
                strncpy(params->rsc_status_value, pchRscStatus2,
                        FIELD_HA_STATUS_RSC_STATUS_LENGTH);
                params->rsc_status_value[FIELD_HA_STATUS_RSC_STATUS_LENGTH + 1]
                        = 0;
            }
            free(pchRscStatus);
            pchRscStatus = NULL;
        }
    } while (0);

    return RC_NORMAL;
}


