

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "vlog.h"
#include "sample_menu_gen.h"


static void paramsInitSampleConfig(struct sample_params *params)
{
    //strcpy(params->domain_value, "pqr.com");
    //params->domain_set = 1;
}

static int validateSampleConfig(struct sample_params *params,
                                struct sample_params *orig_params,
                                char *error_string) {
    

    if (params->audio_port_set || params->video_port_set) {
        if (params->video_port_value > params->audio_port_value) {
            
            sprintf(error_string, "Audio Port should be greater than Video Port");
            return RC_INVALID_VALUE;
        }
    }
    VLOG_INFO("Done Sample validation...");

    if (params->audio_port_set) {
        if (params->audio_port_value == 5678) {
            sprintf(error_string, "Dangerous to use this value for port.");
            return RC_WARNING;
        }
    }

    return RC_NORMAL;
}


int applySampleConfig(struct sample_params *params,
                      struct sample_params *orig_params) {

    char buf[128];
    if (params->options_set == 1) {
        VLOG_INFO("Option Value = %d",
                   params->options_value);
    }
    
    sprintf(buf, "echo %s  > /tmp/sample.txt 2>&1", params->ipaddress_value);
    return RC_NORMAL;
}


void SampleConfigInit()
{
    registerSampleConfigValidateFunction(validateSampleConfig);
    registerSampleConfigParamsInitFunction(paramsInitSampleConfig);
}

