#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "configureSystem.h"
#include "commonutils.h"
#include "system_utils.h"
#include "configfile_parser.h"
#include "vlog.h"
#include "vutils.h"
#include "cli_utils.h"


/* Menu command */
int rebootSystem(MENUINFO *minfo) {

    DIALOG      *dialog;
    int         retval = RC_NORMAL;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    /**********************************
     * Don't let non-root users do it *
     **********************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 0,
                             "You must be root to revert the current configuration\n"
                             "to their original settings.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    /***********************
     * Show warning dialog *
     ***********************/
    dialog = show_dialog(minfo, 4, 0,
                         "</K></U>Warning:<!U><!K> This command will reboot "
                         "the system.\n"
                         "All connected clients will be disconnected.");

    show_info(minfo, "Reboot System?" YESNO_STR);

    if (get_yesno(minfo)) {
        retval = acquire_config_lock();
        if (retval != RC_NORMAL) {
            if (retval == RC_LOCKED_OUT)
                show_info(minfo,
                        "Cannot Reboot: System busy with other config change.\n"
                        PRESSANYKEY_STR);
            else 
                show_info(minfo,
                        "Cannot Reboot: System busy.\n"
                        PRESSANYKEY_STR);

            get_anykey(minfo);
            remove_dialog(dialog);
            return RC_ERROR;
        }
        system_command("/bin/touch " REBOOT_LOCK_FILE);

        system_command("/bin/touch /tmp/reset");

        release_config_lock();
        
        remove_dialog(dialog);
        return RC_EXIT;
    }
    else
        remove_dialog(dialog);

    return RC_NORMAL;
}

/* CLI command */
int reboot_system_main(int argc, char **argv) {

    int rc = RC_NORMAL;
    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }
    rc = acquire_config_lock();
    if (rc != RC_NORMAL) {
        if (rc == RC_LOCKED_OUT)
             printf("ERROR: Cannot Reboot: System busy with other config change.\n");
        else 
            printf("ERROR: Cannot Reboot: System busy.\n");
        return RC_ERROR;
    }

    system_command("/bin/touch " REBOOT_LOCK_FILE);

    system_command("/bin/touch /tmp/reset");

    release_config_lock();
    
    return RC_NORMAL;
}

/* Menu command */
int shutdownSystem(MENUINFO *minfo) {

    DIALOG      *dialog;
    int         retval = RC_NORMAL;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    /**********************************
     * Don't let non-root users do it *
     **********************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 0,
                             "You must be root to revert the current configuration\n"
                             "to their original settings.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    /***********************
     * Show warning dialog *
     ***********************/
    dialog = show_dialog(minfo, 4, 0,
                         "</K></U>Warning:<!U><!K> This command will shutdown "
                         "the system.\n"
                         "All connected clients will be disconnected.\n"
                         "You may have to manually poweroff the system.");

    show_info(minfo, "Shutdown System?" YESNO_STR);

    if (get_yesno(minfo)) {
        retval = acquire_config_lock();
        if (retval != RC_NORMAL) {
            if (retval == RC_LOCKED_OUT)
                show_info(minfo,
                        "Cannot Shutdown: System busy with other config change.\n"
                        PRESSANYKEY_STR);
            else 
                show_info(minfo,
                        "Cannot Shutdown: System busy.\n"
                        PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
            return RC_ERROR;
        }
        
        system_command("/bin/touch " REBOOT_LOCK_FILE);

        system_command("/bin/touch /tmp/shutdown");

        release_config_lock();

        remove_dialog(dialog);
        return RC_EXIT;
    }
    else
        remove_dialog(dialog);

    return RC_NORMAL;
}

/* CLI command */
int shutdown_system_main(int argc, char **argv) {

    int rc = RC_NORMAL;
    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    rc = acquire_config_lock();
    if (rc != RC_NORMAL) {
        if (rc == RC_LOCKED_OUT)
             printf("ERROR: Cannot Shutdown: System busy with other config change.\n");
        else 
            printf("ERROR: Cannot Shutdown: System busy.\n");
        return RC_ERROR;
    }

    system_command("/bin/touch " REBOOT_LOCK_FILE);

    system_command("/bin/touch /tmp/shutdown");
        
    release_config_lock();
    
    return RC_NORMAL;
}
