#include <getopt.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "configureDefaults.h"
#include "loadnet.h"
#include "hal.h"
#include "eeprom.h"
#include "commonutils.h"
#include "system_utils.h"
#include "device_detection.h"
#include "vlog.h"
#include "vutils.h"
#include "cli_utils.h"


int configureDefaults(MENUINFO * minfo)
{

    DIALOG *dialog;
    int retval = RC_NORMAL;
    bool reset_network = false;
    bool delete_media = false;
    char buff[128];
    char dialog_buffer[512];

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
                         "</K></U>Warning:<!U><!K> This will revert the current configuration\n"
                         "to its original factory settings.\n"
                         "The device will reboot automatically.\n\n");

    show_info(minfo, "Revert to Defaults?" YESNO_STR);

    if (get_yesno(minfo)) {
        remove_dialog(dialog);
        int is_dms = 0;
        if (get_hardware_category() == HARDWARE_CATEGORY_DMS) {
            if (get_hardware_profile() != HARDWARE_PROFILE_MS &&  get_hardware_profile() != HARDWARE_PROFILE_MS150)
                is_dms = 1;
        }

        show_info(minfo, "");
        dialog = show_dialog(minfo, 4, 0,
                             "Do you want to reset network settings?"
                             YESNO_STR);
        if (get_yesno(minfo)) {
            reset_network = true;
        }
        else {
            reset_network = false;
        }
        remove_dialog(dialog);
        if (is_dms) {
            dialog = show_dialog(minfo, 4, 0,
                                 "Delete all media on device?" YESNO_STR);
            if (get_yesno(minfo))
                delete_media = true;
            else
                delete_media = false;
        }
        else {
            delete_media = false;
        }

        strcpy(buff, "/etc/scripts/revert_to_defaults.sh");

        strcpy(dialog_buffer,
               "</K></U>Warning:<!U><!K> This will revert current configuration \n to their original factory settings.\n");

        if (reset_network) {
            strcat(dialog_buffer,
                   "The IP addresses and static routes will be deleted.\n");
            strcat(buff, " reset-network");
        }
        else {
            strcat(dialog_buffer,
                   "The IP addresses and static routes will be retained.\n");
        }

        if (is_dms) {
            if (delete_media) {
                strcat(dialog_buffer,
                       "All the media on device will be deleted.\n");
                strcat(buff, " delete-media");
            }
            else {
                strcat(dialog_buffer,
                       "All the media on device will be retained.\n");
            }
        }
        strcat(dialog_buffer,
               "Are you sure you want to continue?" YESNO_STR);
        dialog = show_dialog(minfo, 4, 0, dialog_buffer);

        if (get_yesno(minfo)) {
            remove_dialog(dialog);
            dialog = show_dialog(minfo, 4, 0,
                                 "</K></U>Warning:<!U><!K> The request is being processed ...\n"
                                 "Please do not interrupt the procedure.\n");

            retval = system_command(buff);
            remove_dialog(dialog);
            if (retval != 0) {
                show_info(minfo,
                          "Could not Restore Default values.\n"
                          PRESSANYKEY_STR);
                get_anykey(minfo);
            }
            else {
                // All ok, system should be rebooting shortly
                // Quickly get out of the way.
                return RC_EXIT;
            }
        }
        else {
            remove_dialog(dialog);
            return RC_NORMAL;
        }
    }
    else
        remove_dialog(dialog);

    return RC_NORMAL;
}

static void show_usage(void)
{
    printf("\nUsage: revert_config [OPTIONS]\n");
    printf("\nRevert configuration to default values\n");
    printf("Options:\n");
    printf("        -reset_network              yes/no\n");
    printf("        -delete_media               yes/no\n");
}
static struct option revert_config_options[] = {
    {"reset_network", 1, 0, 0},
    {"delete_media", 1, 0, 0},
    {0, 0, 0, 0}
};


int revert_config_main(int argc, char **argv)
{
    int retval;
    char command[256];
    char *reset_network = NULL;
    char *delete_media = NULL;
    int c, opt_index = 0;

    if (view_only() != 0) {
        printf("You must be root to revert configuration to default\n");
        return RC_ERROR;
    }
    while (1) {
        c = getopt_long_only(argc, argv, "h", revert_config_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            switch (opt_index) {
            case 0:
                reset_network = strdup(optarg);
                break;
            case 1:
                delete_media = strdup(optarg);
                break;
            }
        }
        else {
            printf("ERROR: Incorrect options arguments.");
            show_usage();
            return RC_ERROR;
        }
    }
    if (optind != argc) {
        printf("ERROR: Extra arguments.");
        show_usage();
        return RC_ERROR;
    }
    sprintf(command, "/etc/scripts/revert_to_defaults.sh");
    if (reset_network != NULL) {
        if (strcasecmp(reset_network, "Yes") == 0)
            strcat(command, " reset-network");
    }
    if (delete_media != NULL) {
        if (strcasecmp(delete_media, "Yes") == 0)
            strcat(command, " delete-media");
    }

    printf("The system will now reboot. Please wait...\n");
    retval = system_command(command);

    return retval;
}
