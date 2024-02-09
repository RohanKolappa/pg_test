

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "vlog.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "sample_immediate_menu_gen.h"


static int validateSampleImmediateConfig(struct sample_immediate_params
                                         *params,
                                         struct sample_immediate_params
                                         *orig_params, char *error_string)
{

    if (params->float_test_set == 1) {
        VLOG_INFO("Validation of float test value is --%f--",
                   params->float_test_value);
    }
    if (params->enable_set == 1) {
        VLOG_INFO("Validation of enable new value is --%d--",
                   params->enable_value);
    }
    if (params->integer_range_set == 1) {
        VLOG_INFO("Validation of integer_range new value is --%d--",
                   params->integer_range_value);
    }
    if (params->integer_neg_range_set == 1) {
        VLOG_INFO("Validation of integer_neg_range new value is --%d--",
                   params->integer_neg_range_value);
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

int applySampleImmediateConfig(struct sample_immediate_params *params,
                               struct sample_immediate_params *orig_params)
{

    if (params->float_test_set == 1) {
        VLOG_INFO("Apply of float test value is --%f--",
                   params->float_test_value);
    }
    if (params->enable_set == 1) {
        VLOG_INFO("Apply of enable new value is --%d--",
                   params->enable_value);
    }
    if (params->integer_range_set == 1) {
        VLOG_INFO("Apply of integer_range new value is --%d--",
                   params->integer_range_value);
    }
    if (params->integer_neg_range_set == 1) {
        VLOG_INFO("Apply of integer_neg_range new value is --%d--",
                   params->integer_neg_range_value);
    }
    if (params->domain_set == 1) {
        VLOG_INFO("Apply of domain_name new value is --%s--",
                   params->domain_value);
    }
    if (params->options_set == 1) {
        VLOG_INFO("Apply of options new value is --%d--",
                   params->options_value);
    }
    
    return RC_NORMAL;
}


void SampleImmediateConfigInit(void)
{
    registerSampleImmediateConfigValidateFunction
        (validateSampleImmediateConfig);
}
