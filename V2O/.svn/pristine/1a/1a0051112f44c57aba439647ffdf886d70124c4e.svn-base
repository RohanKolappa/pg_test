#include <getopt.h>

#define CUSTOM_MENU_DEFS
#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "lut_utils.h"
#include "edid_utils.h"
#include "edid_list_menu.h"
#include "clear_edid_list_menu.h"
#include "commonutils.h"
#include "system_utils.h"
#include "configfile_parser.h"
#include "vlog.h"
#include "vutils.h"
#include "cli_utils.h"

#define VLOG_CATEGORY_NAME  "clear_edid_list"

static char *clear_edid_list = "Clear List of EDID Descriptors";

static int board_number = 0;

static struct option clear_options[] = {
        {"all", 0, 0, 0},
        {"clientData", 1, 0, 0},
        {"channelNumber", 1, 0, 0},
        {0, 0, 0, 0}
};

static char *get_config_file_name(void)
{
    static char buf[128];
    sprintf(buf, "/V2O/config/edidlistconf_%d.txt", board_number);
    return buf;
}

int clearEdidDescriptorList(MENUINFO * minfo) {
    DIALOG *dialog;
    int retval = RC_NORMAL;
    int rc, video_input = 0x80;
    HAL *hp = NULL;
    char cmdbuf[128];

    {
        hp = minfo->minfo_userptr;
        if (hp != NULL) { 
            board_number = hal_get_board_number(hp);
        }
    }

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    /**********************************
     * Don't let non-root users do it *
     **********************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 0, 
                "You must be root to clear EDID descriptors from EEPROM.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }
    /************************
     * Show initial message *
     ************************/
    dialog = show_dialog(minfo, 4, 0, 
            "This utility allows you to clear all configured EDID\n"
            "descriptors from EEPROM\n");
    show_info(minfo, "Clear EDID descriptors?" YESNO_STR);
    if (get_yesno(minfo)) {
        /******************
         * Write to flash *
         ******************/
        retval = acquire_config_lock();
        if (retval != RC_NORMAL) {
            if (retval == RC_LOCKED_OUT)
                show_info(minfo,
                    "Cannot clear EEPROM: System busy with other config change.\n"
                    PRESSANYKEY_STR);
            else
                show_info(minfo,
                    "Cannot clear EEPROM: System busy.\n" PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
            return RC_ERROR;
        }
        remove_dialog(dialog);
        dialog = show_dialog(minfo, 4, 0, 
                "</K></U>Warning:<!U><!K> The EEPROM is being updated...\n" 
                "Please do not interrupt the procedure.\n");
        if ((rc = load_edid_eeprom_with_default(hp, video_input) < 0)) {
            show_info(minfo, "Failed to write EDID EEPROM. " PRESSANYKEY_STR);
            retval = RC_ERROR;
        }
        else {
            show_info(minfo, 
                    "Cleared EDID descriptors from EEPROM.\n" PRESSANYKEY_STR);
            sprintf(cmdbuf, "/bin/rm -f %s", get_config_file_name());
            retval = system_command(cmdbuf);
        }

        remove_dialog(dialog);
        release_config_lock();
        get_anykey(minfo);
    }
    return retval;
}

int clear_edid_list_main(int argc, char **argv)
{
    int opt_idx = 1;
    int rc = RC_NORMAL;
    HAL *hp = NULL;
    int c, opt_index = 0, do_all = 0, video_input = 0x80;

    while (1) {
        c = getopt_long_only(argc, argv, "h", clear_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            switch (opt_index) {
            case 0:
                do_all = 1;
                break;
            case 2:
                board_number = atoi(optarg) - 1;
                break;
            }
        }
        else {
            rc = RC_ERROR;
            break;

        }
    }
    opt_idx = optind;

    hp = hal_acquire(board_number);
    if (hp == NULL) {
        print_cli_failure(clear_edid_list, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }
    if (opt_idx == argc) {
        rc = acquire_config_lock();
        if (rc != RC_NORMAL) {
            if (rc == RC_LOCKED_OUT) 
                printf("ERROR: Cannot clear EDID list: System busy with other config change.\n");
            else 
                printf("ERROR: Cannot clear EDID list: System busy.\n");
            return RC_ERROR;
        }
        if (do_all) {
            int i, number_of_boards = hal_get_number_of_boards(hp);
            hal_release(hp);
            for (i = 0; i < number_of_boards; i++) {
                if ((hp = hal_acquire(i)) == NULL) {
                    print_cli_failure(clear_edid_list, CLI_REASON_SYSTEM_ERR);
                    rc = RC_ERROR;
                }
                else {
                    rc = load_edid_eeprom_with_default(hp, video_input);
                    if (rc < 0) {
                        print_cli_failure(clear_edid_list, 
                                CLI_REASON_SYSTEM_ERR);
                        rc = RC_ERROR;
                    }
                    hal_release(hp);
                    hp = NULL;
                }
            }
        }
        else {
            rc = load_edid_eeprom_with_default(hp, video_input);
            if (rc < 0) {
                print_cli_failure(clear_edid_list, CLI_REASON_SYSTEM_ERR);
                rc = RC_ERROR;
            }
        }
        if (rc != RC_ERROR) {
            char cmdbuf[128];
            sprintf(cmdbuf, "/bin/rm -f %s", get_config_file_name());
            rc = system_command(cmdbuf);
        }
        release_config_lock();
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(clear_edid_list, CLI_REASON_BAD_USAGE);
        printf("\nUsage: clear_edid_list -boardNumber Number\n");
        printf("Clear List of EDID Descriptors\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}
