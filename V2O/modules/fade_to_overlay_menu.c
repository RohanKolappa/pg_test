
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vlog.h"
#include "step_timer.h"
#include "display_params_menu_gen.h"
#include "fade_to_overlay_menu_gen.h"

#define VLOG_CATEGORY_NAME  "fade_to_overlay"

static int daemonize(void)
{
    if (fork() != 0) {
        return 1;
    }
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return 0;
}

#define LOCKFILEPREFIX "/tmp/board_lock"
static int get_board_lock(HAL * hp)
{
    int fd;
    char filename[64];
    sprintf(filename, "%s_%d.lck",
            LOCKFILEPREFIX, hal_get_board_number(hp));
    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return -1;
    }
    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            VLOG_INFO_CAT(VLOG_CATEGORY_NAME, 
                "Waiting to acquire lock for board %d...", 
                hal_get_board_number(hp));
            if (flock(fd, LOCK_EX) == -1) {
                return -1;
            }
            VLOG_INFO_CAT(VLOG_CATEGORY_NAME,
                "Acquired lock for board %d.", hal_get_board_number(hp));
        }
        else {
            return -1;
        }
    }
    return 0;
}

static int release_board_lock(HAL * hp)
{
    int fd;
    char filename[64];
    sprintf(filename, "%s_%d.lck",
            LOCKFILEPREFIX, hal_get_board_number(hp));
    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return -1;
    }
    if (flock(fd, LOCK_UN) == -1) {
        return -1;
    }
    return 0;
}

static void fade(HAL * hp, int initial_transparency, int final_transparency, 
                 int duration, double ramp) 
{
    struct step_timer step_timer;

    VLOG_INFO_CAT(VLOG_CATEGORY_NAME, 
        "Channel %d. Fade transparency %d --> %d in %d ms, ramp: %0.2f", 
        hal_get_channel_number(hp),
        initial_transparency, final_transparency, duration, ramp);
    step_timer_init(&step_timer, duration);
    while (step_timer_check(&step_timer)) {
        double scale = step_timer_get_scale(&step_timer);
        scale = pow(scale, ramp);
        int transparency = initial_transparency + 
            scale * (final_transparency - initial_transparency);

        // Do the transparency ioctl here
        hal_set_fpga_val_k(hp, FPGA_DOVLPALPHA1, 1024 - (1024 *transparency)/100);
        hal_set_fpga_val_k(hp, FPGA_DOVLPALPHA2, (1024*transparency)/100);
    }
}

static int get_transparency(HAL * hp)
{
    struct ov_param_io ov;
    hal_get_overlay_params(hp, &ov);
        return ov.oalpha;
}

static void apply_overlay(HAL * hp, struct fade_to_overlay_params *params)
{
    char cmdbuf[256], buf[64];

    // Create the command for overlay change 
    sprintf(cmdbuf, "set_display_parm -channelNumber %d", 
            hal_get_channel_number(hp));

    if (params->keep_aspect_set) {
        strcat(cmdbuf, " -keep_aspect");
        strcat(cmdbuf, (params->keep_aspect_value) ? " Yes" : " No");
    }
    if (params->horiz_scheme_set) {
        strcat(cmdbuf, " -horiz_scheme");
        strcat(cmdbuf, (params->horiz_scheme_value == 
            FADE_TO_OVERLAY_HORIZ_SCHEME_RESOLUTION) ? " Resolution" : 
            " Scale/Source");
    }
    if (params->vert_scheme_set) {
        strcat(cmdbuf, " -vert_scheme");
        strcat(cmdbuf, (params->vert_scheme_value == 
            FADE_TO_OVERLAY_VERT_SCHEME_RESOLUTION) ? " Resolution" : 
            " Scale/Source");
    }
    if (params->horiz_scale_set) {
        sprintf(buf, " -horiz_scale %d", params->horiz_scale_value);
        strcat(cmdbuf, buf);
    }
    if (params->vert_scale_set) {
        sprintf(buf, " -vert_scale %d", params->vert_scale_value);
        strcat(cmdbuf, buf);
    }
    if (params->horiz_res_set) {
        sprintf(buf, " -horiz_res %d", params->horiz_res_value);
        strcat(cmdbuf, buf);
    }
    if (params->vert_res_set) {
        sprintf(buf, " -vert_res %d", params->vert_res_value);
        strcat(cmdbuf, buf);
    }
    if (params->horiz_position_set) {
        strcat(cmdbuf, " -horiz_position");
        strcat(cmdbuf, (params->horiz_position_value == 
            FADE_TO_OVERLAY_HORIZ_POSITION_LOCATION) ? "Location" : 
            "Offset");
    }
    if (params->vert_position_set) {
        strcat(cmdbuf, " -vert_position");
        strcat(cmdbuf, (params->vert_position_value == 
            FADE_TO_OVERLAY_VERT_POSITION_LOCATION) ? "Location" : 
            "Offset");
    }
    if (params->horiz_location_set) {
        strcat(cmdbuf, " -horiz_location");
        strcat(cmdbuf, (params->horiz_location_value == 
            FADE_TO_OVERLAY_HORIZ_LOCATION_LEFT) ? " Left" : 
            ((params->horiz_location_value == 
            FADE_TO_OVERLAY_HORIZ_LOCATION_RIGHT) ? " Right" : 
            " Center"));
    }
    if (params->vert_location_set) {
        strcat(cmdbuf, " -vert_location");
        strcat(cmdbuf, (params->vert_location_value == 
            FADE_TO_OVERLAY_VERT_LOCATION_TOP) ? " Top" : 
            ((params->horiz_location_value == 
            FADE_TO_OVERLAY_VERT_LOCATION_BOTTOM) ? " Bottom" : 
            " Middle"));
    }
    if (params->horiz_offset_set) {
        sprintf(buf, " -horiz_offset %d", params->horiz_offset_value);
        strcat(cmdbuf, buf);
    }
    if (params->vert_offset_set) {
        sprintf(buf, " -vert_offset %d", params->vert_offset_value);
        strcat(cmdbuf, buf);
    }
    if (params->transparency_set) {
        sprintf(buf, " -transparency %d", params->transparency_value);
        strcat(cmdbuf, buf);
    }

    // Run the command for overlay change 
    VLOG_INFO_CAT(VLOG_CATEGORY_NAME, "Executing command %s", cmdbuf);
    system_command(cmdbuf);
}

int applyFadeToOverlay(struct fade_to_overlay_params *params,
                       struct fade_to_overlay_params *orig_params)
{

    HAL *hp = params->userptr;
    int target_transparency;

    if (hp == NULL)
        return RC_ERROR;

    //  Daemonize to run all further activity in the background
    if (daemonize() != 0) {
        return RC_NORMAL;
    }

    // Get board lock so that only one of this process can run
    get_board_lock(hp);

    // Get fade in/out duration from command line
    // Default duration is 2 secs
    int fadeout_duration = 2000;
    if (params->duration_set == 1)
        fadeout_duration = params->duration_value;
    int fadein_duration = fadeout_duration;
    if (params->fadein_duration_set) {
        fadein_duration = params->fadein_duration_value;
    }

    // Get fade in/out ramp from command line
    // Default fade out ramp is 1.0 (linear)
    // Default fade in ramp is 2.0 (slow start)
    double fadeout_ramp = 1.0;
    if (params->ramp_set == 1)
        fadeout_ramp = params->ramp_value;
    double fadein_ramp = 2.0;
    if (params->fadein_ramp_set == 1)
        fadein_ramp = params->fadein_ramp_value;

    // Get current transparency
    int initial_transparency = get_transparency(hp);
    VLOG_INFO("Initial transparency = %d", initial_transparency);

    // FADE OUT from current to 100 % transparency
    fade(hp, initial_transparency, 100, fadeout_duration, fadeout_ramp);
    
    // Save transparency value from command 
    if (params->transparency_set) 
        target_transparency = params->transparency_value;
    else 
        target_transparency = initial_transparency;

    // Apply configured parameters but keep transparency at 100 %
    params->transparency_set = 1;
    params->transparency_value = 100;
    apply_overlay(hp, params);
    
    // FADE IN from 100 % to target transparency
    VLOG_INFO_CAT(VLOG_CATEGORY_NAME, 
        "Channel %d. Target transparency = %d", 
        hal_get_channel_number(hp), target_transparency);
    fade(hp, 100, target_transparency, fadein_duration, fadein_ramp);
    
    // Apply target transparency
    params->keep_aspect_set = params->horiz_scheme_set = 
        params->vert_scheme_set = params->horiz_scale_set =
        params->vert_scale_set = params->horiz_res_set = params->vert_res_set =
        params->horiz_position_set = params->vert_position_set = 
        params->horiz_location_set = params->vert_location_set = 
        params->horiz_offset_set = params->vert_offset_set = 0;
    params->transparency_set = 1;
    params->transparency_value = target_transparency;
    apply_overlay(hp, params);

    // We are done
    VLOG_INFO_CAT(VLOG_CATEGORY_NAME, "Done Fading");
    release_board_lock(hp);

    return RC_NORMAL;
}
