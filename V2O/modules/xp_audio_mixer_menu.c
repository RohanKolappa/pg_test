#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "vlog.h"
#include "commonutils.h"
#include "xp_audio_mixer_menu_gen.h"

static void paramsInitXPAudioMixerParameters(struct xp_audio_mixer_params 
                                             *params) {
    HAL *hp = params->userptr;

    if (hp == NULL)
        return;

    params->local_volume_set = 1;
    params->local_volume_value = (double) hal_getAudioMixerLocalVolume(hp) * 
        (double) 100 / (double) 1024;
    params->remote_volume_set = 1;
    params->remote_volume_value = (double) hal_getAudioMixerRemoteVolume(hp) * 
        (double) 100 / (double) 1024;
}

int applyXPAudioMixerParameters (struct xp_audio_mixer_params *params,
                           struct xp_audio_mixer_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    double local = params->local_volume_value;
    double remote = params->remote_volume_value;
    params->local_volume_value = local * (double) 1024 / (double) 100;
    params->remote_volume_value = remote * (double) 1024 / (double) 100;
    if (params->audio_input_value == XP_AUDIO_MIXER_AUDIO_INPUT_LINE)
    hal_setAudioMixer(hp, params->local_volume_value, 
            params->remote_volume_value, 0);
    else
    hal_setAudioMixer(hp, params->local_volume_value, 
            params->remote_volume_value, 1); 

    return RC_NORMAL;
}

void XPAudioMixerParametersInit(void) 
{
    registerXPAudioMixerParametersParamsInitFunction(
            paramsInitXPAudioMixerParameters);
}

