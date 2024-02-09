

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "configureLog.h"
#include "log_control.h"


static char *loghelp =
    "Current log level is highlighted. Use </U>Up/Down<!U> arrow keys\n"
    "to select  new log level and press </U>Enter<!U>.\n";

static char *loghelp2 =
    "Current log level is highlighted.\n"
    "Press </U>Q<!U> or </U>CTRL-Q<!U> to quit";

static char *log_array[6] = { LOG_STR_NONE,
    LOG_STR_CRITICAL,
    LOG_STR_WARNING,
    LOG_STR_INFO,
    LOG_STR_DEBUG,
    LOG_STR_DETAIL
};

static char *logselector_cb(int item)
{

    if (item >= 0 && item < 6)
        sprintf(tempbuf, "%s", log_array[item]);
    else
        sprintf(tempbuf, "???");
    return tempbuf;
}


int configureLog(MENUINFO * minfo)
{
    MENU *selector;
    int rc;
    char keylist[6];
    int retval = RC_NORMAL;
    int cur_lev = 0;
    int done = FALSE;
    HAL *hp;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    hp = hal_acquire(0);
    if (hp == NULL) {
        return show_fault_dialog(minfo, "Missing Hardware!\n");
    }

    /******************************
     * Create the selector widget *
     ******************************/
    selector = create_selector(6, logselector_cb);

    /**************************************
     * Associate windows for the selector *
     **************************************/
    set_menu_windows(minfo, selector);

    /*********************
     * Post the selector *
     *********************/
    post_menu(selector);

    /**************
     * Print help *
     **************/
    show_info(minfo, (ruid == 0) ? loghelp : loghelp2);

    /**********************************
     * Find and set current log level *
     **********************************/
    cur_lev = get_persitent_int("LogLevel");
    if (cur_lev < 0 || cur_lev > 5)
        cur_lev = DEFAULT_LOG_LEVEL;

    set_selector_item(selector, cur_lev);

    /***************************
     * Let user select an item *
     ***************************/
    while (done == FALSE) {
        strcpy(keylist, "sS");
        rc = process_selector(minfo, selector, keylist);

        /* Exit request */
        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            break;
        }
        /* Quit request */
        if (rc == RC_QUIT) {
            retval = RC_NORMAL;
            break;
        }

        /**********************************************
         * Don't process requests from non-root users *
         **********************************************/
        if (ruid != 0)
            continue;

        /*******************************
         * Process custom key requests *
         *******************************/
        switch (keylist[0]) {

        case '\0':
        case 's':
        case 'S':
            sprintf(tempbuf, "Set Log Level to %s? " YESNO_STR,
                    log_array[rc]);
            show_info(minfo, tempbuf);
            if (get_yesno(minfo)) {

                set_persitent_int("LogLevel", rc);
                hal_set_log_level(hp, rc);
            }
            done = TRUE;
            break;
        }

    }

    /************
     * Clean-up *
     ************/
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);

    hal_release(hp);
    return retval;
}
