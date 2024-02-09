
#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlib.h"
#include "vlog.h"
#include "frame_counter_status_menu_gen.h"

#define DEFAULT_FRMCNT_INTERVAL   5
#define DEFAULT_FRMCNT_RETRIES    3
#define DEFAULT_FRMCNT_MULTIPLIER 3

int getFrameStatistics(struct frame_statistics_params *params) 
{
    unsigned char buf[FRAMECNT_FIFO_SIZE];
    char frametxt[FRAMECNT_TEXT_SIZE];

    HAL *hp;
    int  fifolevel = 0, i, interval;
    char * pager = "--PAGE BREAK--";
    
    hp = params->userptr;
    
    if (hp == NULL) {
        strcat (params->textarea_value, "Failed: Invalid  Channel number");
        return RC_NORMAL;
    }

    interval = DEFAULT_FRMCNT_INTERVAL;
    for (i = 0; i < DEFAULT_FRMCNT_RETRIES; i++) {
        fifolevel = hal_read_framecnt_fifo(hp, buf, interval);
        if (fifolevel > 5) break;
        interval *= DEFAULT_FRMCNT_MULTIPLIER;
    }
    if (fifolevel <= 5) {
        strcat (params->textarea_value, "Failed to read frame counter FIFO\n");
    } 
    else {
        (void)  print_framecnt_to_buffer(buf, frametxt, pager, fifolevel, interval, hal_get_board_type(hp));
        strcat (params->textarea_value, frametxt);
    }

    return RC_NORMAL;

}
