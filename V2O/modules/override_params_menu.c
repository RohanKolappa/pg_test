#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "override_params_menu_gen.h"
#include "override_config.h"
#include "strmsrv_comm.h"

int get_override_config(HAL *hp, struct override_config *override_cfg) {

    struct override_parm_params params;

    memset(override_cfg, '\0', sizeof(struct override_config));
    params.userptr = hp;
    retrieveOverrideParametersValues(&params);
    override_cfg->ov_option = override_value_from_code(params.ov_option_value);

    return 0;
}

int applyOverrideParameters(struct override_parm_params *params, 
                struct override_parm_params *orig_params){

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    struct strmsrv_rx_clnt_params clnt_params;
    int set_clnt_params = 0;

    memset(&clnt_params, 0x00, sizeof(clnt_params));

    if (params->ov_option_set == 1) {
        clnt_params.ov_option_set = 1;
        clnt_params.ov_option = 
            override_value_from_code(params->ov_option_value);
        set_clnt_params = 1;
    }

    if (set_clnt_params) {
        strmsrv_set_rx_clnt_params(hal_get_channel_number(hp), &clnt_params);
    }

    return RC_NORMAL;
}
