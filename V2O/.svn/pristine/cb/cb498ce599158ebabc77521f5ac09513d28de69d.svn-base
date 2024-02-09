/*
 * ha_takeover_command.c
 *
 *  Created on: Sep 2, 2010
 *      Author: rkale
 */

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
int haTakeover(MENUINFO *minfo) {

    DIALOG      *dialog;
    int         retval = RC_NORMAL;
    struct stat statbuf;
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
                             "You must be root\n"
                             "to use this feature.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    // Is HA configured ??
    if (!is_linuxha_enabled()) {
        dialog = show_dialog(minfo, 7, 0,
                             "High Availability is not currently enabled\n"
                             "on this system.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    // Are we in standby mode??
    if (stat("/var/run/cluster.conf", &statbuf) == 0) {

        dialog = show_dialog(minfo, 7, 0,
                             "This system is currently not in standby mode.");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    /***********************
     * Show warning dialog *
     ***********************/
    dialog = show_dialog(minfo, 4, 0,
                         "</K></U>Warning:<!U><!K> This command will cause this system\n"
                         "to takeover as the active Management Server.");

    show_info(minfo, "Takeover?" YESNO_STR);

    if (get_yesno(minfo)) {
        retval = acquire_config_lock();
        if (retval != RC_NORMAL) {
            if (retval == RC_LOCKED_OUT)
                show_info(minfo,
                        "Cannot Takeover: System busy with other config change.\n"
                        PRESSANYKEY_STR);
            else
                show_info(minfo,
                        "Cannot Takeover: System busy.\n"
                        PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
            return RC_ERROR;
        }

        remove_dialog(dialog);

        // Issue the takeover command
        show_info(minfo, "Taking Over...");
        rc = system_command("/usr/share/heartbeat/hb_takeover");
        release_config_lock();

        if (rc != 0) {
            char errbuff[64];
            sprintf(errbuff, "Command failed with error code %d.", rc);
            dialog = show_dialog(minfo, 7, 0, errbuff);
            show_info(minfo, PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
        }
        else {
            show_info(minfo, PRESSANYKEY_STR);
            get_anykey(minfo);
        }

        return RC_NORMAL;
    }
    else
        remove_dialog(dialog);

    return RC_NORMAL;
}

/* CLI command */
int ha_takeover_main(int argc, char **argv) {

    int rc = RC_NORMAL;
    struct stat statbuf;

    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    // Is HA configured ??
    if (!is_linuxha_enabled()) {
        printf("ERROR: High Availability is not currently enabled on this system.\n");
        return RC_ERROR;
    }

    // Are we in standby mode??
    if (stat("/var/run/cluster.conf", &statbuf) == 0) {
        printf("ERROR: This system is currently not in standby mode.\n");
        return RC_ERROR;
    }


    rc = acquire_config_lock();
    if (rc != RC_NORMAL) {
        if (rc == RC_LOCKED_OUT)
             printf("ERROR: Cannot Takeover: System busy with other config change.\n");
        else
            printf("ERROR: Cannot Takeover: System busy.\n");
        return RC_ERROR;
    }

    // Issue the takeover command
    (void) system_command("/usr/share/heartbeat/hb_takeover");
    release_config_lock();

    if (rc != 0) {
        return RC_ERROR;
    }
    return RC_NORMAL;
}

/* Menu command */
int haStandby(MENUINFO *minfo) {

    DIALOG      *dialog;
    int         retval = RC_NORMAL;
    struct stat statbuf;
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
                             "You must be root\n"
                             "to use this feature.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    // Is HA configured ??
    if (!is_linuxha_enabled()) {
        dialog = show_dialog(minfo, 7, 0,
                             "High Availability is not currently enabled\n"
                             "on this system.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    // Are we in active mode??
    if (stat("/var/run/cluster.conf", &statbuf) != 0) {

        dialog = show_dialog(minfo, 7, 0,
                             "This system is currently not in active mode.");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    /***********************
     * Show warning dialog *
     ***********************/
    dialog = show_dialog(minfo, 4, 0,
                         "</K></U>Warning:<!U><!K> This command will cause this system\n"
                         "to go to standby mode.");

    show_info(minfo, "Standby?" YESNO_STR);

    if (get_yesno(minfo)) {
        retval = acquire_config_lock();
        if (retval != RC_NORMAL) {
            if (retval == RC_LOCKED_OUT)
                show_info(minfo,
                        "Cannot Standby: System busy with other config change.\n"
                        PRESSANYKEY_STR);
            else
                show_info(minfo,
                        "Cannot Standby: System busy.\n"
                        PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
            return RC_ERROR;
        }

        remove_dialog(dialog);

        // Issue the standby command
        show_info(minfo, "Standing by...");
        rc = system_command("/usr/share/heartbeat/hb_standby");
        release_config_lock();

        if (rc != 0) {
            char errbuff[64];
            sprintf(errbuff, "Command failed with error code %d.", rc);
            dialog = show_dialog(minfo, 7, 0, errbuff);
            show_info(minfo, PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
        }
        else {
            show_info(minfo, PRESSANYKEY_STR);
            get_anykey(minfo);
        }

        return RC_NORMAL;
    }
    else
        remove_dialog(dialog);

    return RC_NORMAL;
}

/* CLI command */
int ha_standby_main(int argc, char **argv) {

    int rc = RC_NORMAL;
    struct stat statbuf;

    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    // Is HA configured ??
    if (!is_linuxha_enabled()) {
        printf("ERROR: High Availability is not currently enabled on this system.\n");
        return RC_ERROR;
    }

    // Are we in active mode??
    if (stat("/var/run/cluster.conf", &statbuf) != 0) {
        printf("ERROR: This system is currently not in active mode.\n");
        return RC_ERROR;
    }


    rc = acquire_config_lock();
    if (rc != RC_NORMAL) {
        if (rc == RC_LOCKED_OUT)
             printf("ERROR: Cannot Standby: System busy with other config change.\n");
        else
            printf("ERROR: Cannot Standby: System busy.\n");
        return RC_ERROR;
    }

    // Issue the standby command
    (void) system_command("/usr/share/heartbeat/hb_standby");
    release_config_lock();

    if (rc != 0) {
        return RC_ERROR;
    }
    return RC_NORMAL;
}

