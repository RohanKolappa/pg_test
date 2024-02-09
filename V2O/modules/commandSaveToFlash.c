

#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <netinet/in.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commandSaveToFlash.h"
#include "loadnet.h"
#include "hal.h"
#include "commonutils.h"
#include "system_utils.h"
#include "configfile_parser.h"
#include "vutils.h"
#include "cli_utils.h"


int saveToFlash(MENUINFO * minfo)
{

    DIALOG *dialog;
    int retval = RC_NORMAL;
    int rc;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);


    /**********************************
     * Don't let non-root users do it *
     **********************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 0,
                             "You must be root to save the current configuration\n"
                             "to flash.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    /************************
     * Show initial message *
     ************************/
    dialog = show_dialog(minfo, 4, 0,
                         "This utility allows you to save the current system\n"
                         "configuration to flash\n");
    
    show_info(minfo, "Save to Flash?" YESNO_STR);

    if (get_yesno(minfo)) {
        /******************
         * Write to flash *
         ******************/
        retval = acquire_config_lock();
        if(retval != RC_NORMAL) {
            if (retval == RC_LOCKED_OUT)
                show_info(minfo,
                    "Cannot Save to Flash: System busy with other config change.\n"
                    PRESSANYKEY_STR);
            else
                show_info(minfo,
                    "Cannot Save to Flash: System busy.\n"
                    PRESSANYKEY_STR);
                    
            get_anykey(minfo);
            remove_dialog(dialog);
            return RC_ERROR;
        }

        remove_dialog(dialog);

        dialog = show_dialog(minfo, 4, 0,
                             "</K></U>Warning:<!U><!K> The flash is being updated...\n"
                             "Please do not interrupt the procedure.\n");

        rc = system_command("/etc/scripts/save_to_flash.sh");

        remove_dialog(dialog);

        release_config_lock();

        if (rc == 0)
            show_info(minfo,
                      "Saved configuration to Flash.\n" PRESSANYKEY_STR);
        else
            show_info(minfo, "Error writing to Flash. " PRESSANYKEY_STR);
        get_anykey(minfo);
    }
    else
        remove_dialog(dialog);
    return retval;
}

/*************************
 * Command Line Routines *
 *************************/
static char *save_config = "Save Config";

int save_config_main(int argc, char **argv)
{
    int retval = RC_NORMAL;

    if (view_only() != 0) {
        printf("You must be root to save configuration to flash\n");
        return RC_ERROR;
    }
    
    if (argc == 1) {
        retval = acquire_config_lock();
        if (retval != RC_NORMAL) {
            if (retval == RC_LOCKED_OUT)
                printf("ERROR: Cannot Save to Flash: System busy with other config change.\n");
            else
                printf("ERROR: Cannot Save to Falsh: System busy.\n");
            return RC_ERROR;
        }

        retval = system_command("/etc/scripts/save_to_flash.sh");

        release_config_lock();

        print_cli_success(save_config);
    }
    else {
        retval = RC_ERROR;
        print_cli_failure(save_config, CLI_REASON_BAD_USAGE);
        printf("\nUsage: save_config\n");
        printf("\nSave configuration to compact flash\n");
    }

    return retval;
}
