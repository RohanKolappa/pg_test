#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <netinet/in.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "osd_control.h"
#include "commonutils.h"
#include "vlog.h"
#include "display_config.h"
#include "strmsrv_comm.h"
#include "displayGuideRuler.h"
#include "override_config.h"

#define FIELD_COLOR     1
#define FIELD_XPOS      2
#define FIELD_YPOS      3

#define FIELD_COLOR_LEN 9
#define FIELD_XPOS_LEN  4
#define FIELD_YPOS_LEN  4

static char *colorlist[] =
    { "Black", "White", "Gray", "Red", "Green", "Blue", "Yellow",
        "Magenta", "Cyan", NULL };

static char *get_color(char *buf, void *usrptr, int ident)
{
    sprintf(buf, "%s", DEFAULT_GUIDERULER_ICON_COLOR);
    return buf;
}
static char *get_xpos(char *buf, void *usrptr, int ident)
{
    sprintf(buf, "%d", DEFAULT_GUIDERULER_ICON_STARTX);
    return buf;
}
static char *get_ypos(char *buf, void *usrptr, int ident)
{
    sprintf(buf, "%d", DEFAULT_GUIDERULER_ICON_STARTX);
    return buf;
}

static FORMITEMDEF itemdefs[] = {

    {FIELD_COLOR, "Color",
     "Set guide ruler color",
     FIELD_COLOR_LEN, get_color, NULL,
     FTYPE_ENUM_IMMEDIATE, (int) colorlist, 0, O_NULLOK, 0},

    {FIELD_XPOS, "X Position",
     "Horizontal Pixel Start for Guide Ruler",
     FIELD_XPOS_LEN, get_xpos, NULL,
     FTYPE_INTEGER_IMMEDIATE, 0, 4095, O_NULLOK, 0},

    {FIELD_YPOS, "Y Position",
     "Vertical Pixel Start for Guide Ruler",
     FIELD_YPOS_LEN, get_ypos, NULL,
     FTYPE_INTEGER_IMMEDIATE, 0, 2047, O_NULLOK, 0},
    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};


int displayGuideRuler(MENUINFO * minfo)
{
    FORM *form = NULL;
    int rc;
    int retval = RC_NORMAL;
    FIELD *curr_field;
    FORMITEMDEF *fitem;
    char tmpbuf[64];
    int colorn, xstartn, ystartn;
    HAL *hp;
    struct strmsrv_rx_clnt_params clnt_params;

    hp = minfo->minfo_userptr;
    if (hp == NULL || hal_get_hardware_type(hp) == V2DI50_DEVICE)
        return RC_ERROR;

    show_title(minfo, NULL);

    form = create_form(itemdefs, 0, 0, NULL);

    set_form_windows(minfo, form);

    post_form(form);
    
    hal_init_guideruler_icon(hp, 1, 
            color_code_from_name(DEFAULT_GUIDERULER_ICON_COLOR));

    while (1) {
        rc = process_form(minfo, form, formmesg, 0);

        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            goto QUIT_LABEL;
        }
        if (rc == RC_QUIT) {
            retval = RC_NORMAL;
            break;
        }
        else if (rc == RC_EXCEPT_FIELD) {
            rc = RC_NORMAL;
            curr_field = current_field(form);
            fitem = field_userptr(curr_field);
            char *buf;
            buf = field_buffer(curr_field, 0);
            stripWhiteSpace(vBOTH, buf);
            sprintf(tmpbuf, "Set %s to %s?" YESNO_STR,
                fitem->formitem_name, buf);
            show_info(minfo, tmpbuf);
            if (get_yesno(minfo)) {
                switch (fitem->formitem_ident) {
                case FIELD_COLOR:
                    colorn = color_code_from_name(buf);
                    hal_set_icon_color(hp, colorn);
                    break;
                case FIELD_XPOS:
                    xstartn = atoi(buf);
                    hal_move_icon_xpos(hp,xstartn);
                    break;
                case FIELD_YPOS:
                    ystartn = atoi(buf);
                    hal_move_icon_ypos(hp,ystartn);
                    break;
                }
            }
        }
        else {
            show_info(minfo, "Continue Editing? " YESNO_STR);
            if (get_yesno(minfo)) {
                /* continue */
            }
            else {
                retval = RC_ERROR;
                break;
            }
        }
    }
  QUIT_LABEL:
    hal_set_icon_off(hp); 

    /*************************
     * Restore keyboard icon *
     *************************/
    memset(&clnt_params, 0x00, sizeof(clnt_params));
    clnt_params.rfb_refresh = 1;
    clnt_params.rfb_mousecursor = 1;
    strmsrv_set_rx_clnt_params(hal_get_channel_number(hp), &clnt_params);

    /************
     * Clean up *
     ************/
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    return retval;
}

