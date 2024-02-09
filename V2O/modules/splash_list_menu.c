#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "configureSystem.h"
#include "configureVideo.h"
#include "loadnet.h"
#include "commonutils.h"
#include "vlog.h"
#include "remoteConfig.h"
#include "lut_utils.h"
#include "lut_list_menu_gen.h"
#include "splash_list_menu.h"
#include "tx_splash_config.h"
#include "rx_splash_config.h"

#define FIELD_NAME      1
#define FIELD_ALUTREG      2
#define FIELD_STEREO      3
#define FIELD_ORIG_HRES      4
#define FIELD_ORIG_VRES      5
#define FIELD_REFRESH      6
#define FIELD_PCR      7
#define FIELD_HFP      8
#define FIELD_VFP      9
#define FIELD_HBP      10
#define FIELD_VBP      11
#define FIELD_HSYNC      12
#define FIELD_VSYNC      13
#define FIELD_PHASE      14
#define FIELD_VMARGIN      15
#define FIELD_HSHIFT      16
#define FIELD_VSHIFT      17
#define FIELD_CNOISE      18
#define FIELD_FNOISE      19
#define FIELD_CSPACE      20
#define FIELD_CPMP      21
#define FIELD_RGAIN      22
#define FIELD_ROFFSET      23
#define FIELD_GGAIN      24
#define FIELD_GOFFSET      25
#define FIELD_BGAIN      26
#define FIELD_BOFFSET      27

static int add_splash_list_dialog(MENUINFO * minfo, 
                                  struct lut_list_params *paramsp);
static char *add_splash_list_dialog_help =
    "Press </U>Enter<!U> to Select Entry for Splash or </U>Q<!U> to Quit.";

static int lutlistcount = 0;
static int board_number = 0;
struct lut_list_params lutlistparams;

static char * constructHeaderSplashLUTList(char * buffer) {
    sprintf(buffer, " %-12s %s  %s %s  %s %s  %s",
            "Name", "Hres", "Vres", "Rfrsh", "PixClk", "Str?", "LutId");
    return buffer;
}

static char * constructRowSplashLUTList(char *buffer, 
                           struct lut_list_params *params) {
    params->name_value[12] = 0;

    sprintf(buffer, "%-12s %-5d %-5d %-6.1f %-6.1f %-4s %4d",
        params->name_value,
        params->orig_hres_value,
        params->orig_vres_value,
        params->refresh_value,
        params->pcr_value,
        (params->stereo_value == LUT_LIST_STEREO_YES) ? "Yes" : 
        ((params->stereo_value == LUT_LIST_STEREO_NO) ? "No" : "Both"),
        params->alutreg_value);

    return buffer;
}

static char * splashlut_list_selector_cb(int item)
{
    struct lut_list_params params;

    sprintf(tempbuf, "????");
    if (get_lut_list_params_by_index(&params,item) > 0) {
        constructRowSplashLUTList(tempbuf, &params);
    }
    return tempbuf;
}

static int runSplashListExitFunction(void * userptr) 
{
    HAL *hp = (HAL *) userptr;
    int rc = RC_NORMAL;

    if (hal_get_board_type(hp) == BOARDTYPE_TX) {
        if (updateTxSplashLutParameters(hp, lutlistparams.alutreg_value) < 0)
            rc = RC_ERROR;
    }
    else if (updateRxSplashLutParameters(hp, lutlistparams.alutreg_value) < 0) {
        rc = RC_ERROR;
    }
    return rc;
}

int configureSplashList(MENUINFO * minfo)
{

    DIALOG *dialog;
    int rc;
    char titlebuf[120];
    int retval = RC_NORMAL;
    struct lut_list_params lutparams;
    HAL *hp = NULL;

    {
        hp = minfo->minfo_userptr;
        if (hp != NULL) {
            board_number = hal_get_board_number(hp);
        }
    }

    /*******************************************************
     * Retrieve and get count of lut list params only once *
     *******************************************************/
    lutlistcount = retrieveLUTListValues(hp);

    sprintf(titlebuf, "%s\n  \n   </U>", minfo->minfo_title);
    constructHeaderSplashLUTList(titlebuf + strlen(titlebuf));
    strcat(titlebuf, "<!U>");

    show_title(minfo, titlebuf);

    if (lutlistcount == 0) {
        dialog = show_dialog(minfo, 8, 15, 
                "</K/U>Empty List of Video LUTs Table!!<!K!U>");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return RC_ERROR;
    }

    rc = add_splash_list_dialog(minfo, &lutparams);

    if (rc == RC_EXIT) {
        return RC_EXIT;
    }
    if (rc != RC_NORMAL) {
        return RC_ERROR;
    }
    else {
        if (lutparams.alutreg_value == 0) {
            VLOG_WARNING("Invalid LUT ID 0 for Entry at Index %d.", 
                    lutparams.lut_list_id); 
            return RC_EXIT;
        }
        /* Copy lut list params to global variable */
        memcpy(&lutlistparams, &lutparams, sizeof(struct lut_list_params));
    }

    sprintf(tempbuf, "Copy parameters for LUT ID %d to Splash?" YESNO_STR, 
            lutparams.alutreg_value);
    show_info(minfo, tempbuf);

    if (get_yesno(minfo)) {
        retval = runSplashListExitFunction(minfo->minfo_userptr);
    }

    return retval;

}

static int add_splash_list_dialog(MENUINFO * minfo, 
                                    struct lut_list_params *paramsp) {
    MENU *selector;
    DIALOG *dialog;
    char titlebuf[120];
    char keylist[12];
    HAL *hp;
    int rc, retval = RC_NORMAL;

    hp = minfo->minfo_userptr;
    if (hp != NULL) {
        board_number = hal_get_board_number(hp);
    }

    /* Initialize returned lut list params */
    memset(paramsp, '\0', sizeof(struct lut_list_params));

    sprintf(titlebuf, "%s\n  \n   </U>", minfo->minfo_title);
    constructHeaderSplashLUTList(titlebuf + strlen(titlebuf));
    strcat(titlebuf, "<!U>");
    show_title(minfo, titlebuf);

    if (lutlistcount == 0) {
        dialog = show_dialog(minfo, 8, 15,
                             "</K/U>Empty List of Video LUTs Table!!<!K!U>");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return RC_ERROR;
    }

    selector = create_selector(lutlistcount, splashlut_list_selector_cb);
    set_menu_windows(minfo, selector);
    post_menu(selector);
    show_info(minfo, add_splash_list_dialog_help);

    while (1) {
        keylist[0] = '\0';
        rc = process_selector(minfo, selector, keylist);
        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            goto QUIT_LABEL;
        }
        if (rc == RC_QUIT) {
            retval = RC_NORMAL;
            goto QUIT_LABEL;
        }
        if (keylist[0] == '\0') {
            if (get_lut_list_params_by_index(paramsp, rc) == 1) {
                break;
            }
            else {
                VLOG_INFO("Failed to get LUT list params for Index %d", rc);
            }
        }
    }

QUIT_LABEL:
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);
    return retval;
}


