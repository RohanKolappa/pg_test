#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "vlog.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "xp_override_params_menu_gen.h"
#include "override_config.h"
#include "strmsrv_comm.h"

int xp_get_override_config(HAL *hp, struct override_config *override_cfg) {
    struct xp_override_parm_params params;

    memset(override_cfg, '\0', sizeof(struct override_config));
    params.userptr = hp;
    retrieveXpOverrideParametersValues(&params);
    override_cfg->ov_option = 
        xp_override_value_from_code(params.ov_option_value);
    override_cfg->ov_color = color_value_from_code(params.ov_color_value);
    override_cfg->ov_keepaspect = params.ov_keepaspect_value;
    return 0;
}

int applyXpOverrideParameters(struct xp_override_parm_params *params,
                        struct xp_override_parm_params *orig_params) {
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    // No need to apply since hald will handle everything by itself
    return RC_NORMAL;
}
