/*
 * reverse_audio_menu.c
 *
 *  Created on: Feb 28, 2011
 *      Author: rkale
 */


#include <stdlib.h>
#include <string.h>
#include <crypt.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "reverse_audio_menu_gen.h"
#include "standalone.h"

extern int applyReverseAudioParameters(struct reverse_audio_params *params,
                                       struct reverse_audio_params
                                       *orig_params)
{
    return RC_NORMAL;
}


static void reverseAudioParametersSaved(void *userptr) {
    HAL *hp = userptr;
    if (hp == NULL)
        return;
    standalone_toggle_reverse_audio(hal_get_channel_number(hp));
}

void ReverseAudioParametersInit() {
    registerReverseAudioParametersSavedFunction(reverseAudioParametersSaved);
}
