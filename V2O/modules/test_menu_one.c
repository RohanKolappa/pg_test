

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "vlog.h"
#include "test_menu_one_gen.h"


static void paramsInitTestMenuOneConfig(struct test_menu_one_params *params)
{
}

static int validateTestMenuOneConfig(struct test_menu_one_params *params,
                                struct test_menu_one_params *orig_params,
                                char *error_string) {


    if (params->audio_port_set) {
        if (params->audio_port_value == 5678) {
            sprintf(error_string, "Dangerous to use this value for port.");
            return RC_WARNING;
        }
    }

    if (params->float_range_set == 1) {
        VLOG_INFO("Validation of float test value is --%f--",
                   params->float_range_value);
    }
    if (params->integer_range_set == 1) {
        VLOG_INFO("Validation of integer_range new value is --%d--",
                   params->integer_range_value);
    }
    if (params->domain_set == 1) {
       VLOG_INFO("Validation of domain_name new value is --%s--",
                  params->domain_value);
    }
    if (params->options_set == 1) {
        VLOG_INFO("Validation of options new value is --%d--",
                   params->options_value);
    }

    return RC_NORMAL;
}

int applyTestMenuOneConfig(struct test_menu_one_params *params,
                      struct test_menu_one_params *orig_params) {


    if (params->float_range_set == 1) {
        VLOG_INFO("Apply of float test value is --%f--",
                   params->float_range_value);
    }
    if (params->integer_range_set == 1) {
        VLOG_INFO("Apply of integer_range new value is --%d--",
                   params->integer_range_value);
    }
    if (params->options_set == 1) {
        VLOG_INFO("Apply of options new value is --%d--",
                   params->options_value);
    }    
    if (params->domain_set == 1) {
        VLOG_INFO("Apply of domain_name new value is --%s--",
                   params->domain_value);
    }

    char buf[128];
    if (params->ipaddress_set == 0) {
        VLOG_INFO("IP address Value = %s",
                   params->ipaddress_value);
    }
    
    sprintf(buf, "echo %s  > /tmp/test.txt 2>&1", params->netmask_value);
    return RC_NORMAL;
}


void TestMenuOneConfigInit()
{
    registerTestMenuOneConfigValidateFunction(validateTestMenuOneConfig);
    registerTestMenuOneConfigParamsInitFunction(paramsInitTestMenuOneConfig);
}

