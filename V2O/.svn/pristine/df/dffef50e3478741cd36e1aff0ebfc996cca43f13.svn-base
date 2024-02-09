#include <stdio.h>

#include "hal.h"
#include "custmenu.h"
#include "force_splash_screen_menu_gen.h"

int applyForceSplashScreen(struct splash_mode_params *params, 
                             struct splash_mode_params *orig_params) 
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;
    if (params->force_splash_value) {
        hal_set_video_available(hp, 1);
    }
    else{
        hal_set_video_available(hp, 0);
        hal_resetAddsideVideo(hp);       /* Restart add works better */
    }

    return RC_NORMAL;
}

