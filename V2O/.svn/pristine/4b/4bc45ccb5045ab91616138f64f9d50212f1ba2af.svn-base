#include <stdlib.h>
#include <string.h>
#include <getopt.h>
                                                                                
#define CUSTOM_MENU_DEFS
                                                                                
#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "text_utils.h"
#include "vlog.h"
#include "hal_edid.h"
#include "configfile_parser.h"
#include "validationroutines.h"
#include "text_utils.h"
#include "vlog.h"
#include "cli_utils.h"
#include "remoteConfig.h"
#include "lut_utils.h"
#include "edid_list_menu.h"
#include "edid_utils.h"
#include "system_utils.h"
#include "configureEdid.h"

/******************************************************
 * Routines to read/write EDID videoinput config file *
 ******************************************************/
static char *videoinput_array[] = {"Digital", "Analog"};

#define NUMB_ITEMS   (int)(sizeof(videoinput_array)/sizeof(videoinput_array[0]))

#define FIELD_EDID_VIDEOINPUT_LENGTH     8

static int board_number = 0;

static int video_input = 0;

enum edid_videoinput {
    EDID_VIDEOINPUT_DIGITAL,
    EDID_VIDEOINPUT_ANALOG,
};

struct edid_videoinput_params {
    enum edid_videoinput videoinput_value;
    int edid_videoinput_set;
    void *userptr;
    char edid_videoinput[FIELD_EDID_VIDEOINPUT_LENGTH + 1];
};

#ifndef LIBONLY

static char *get_edid_videoinput_string = "Get EDID Video Input";
static char *set_edid_videoinput_string = "Set EDID Video Input";
static char *edid_videoinput_string = "Video Input Type";

static struct option edid_videoinput_options[] = {
    {"videoinput", 1, 0, 0},
    {"clientData", 1, 0, 0},
    {"channelNumber", 1, 0, 0},
    {0, 0, 0, 0}
};

static char *get_config_file_name(void)
{
    static char buf[128];
    sprintf(buf, "/V2O/config/edidvideoinputconf_%d.txt", board_number);
    return buf;
}

static int get_edid_videoinput_handler(char *name, char *value, void *ptr)
{
    char *buf = ptr;
    if (strcasecmp(name, "VIDEOINPUT") == 0) {
        if (check_enum(value, videoinput_array) != RC_NORMAL)
            return 0;
        if (buf != NULL)
            strncpy(buf, value, FIELD_EDID_VIDEOINPUT_LENGTH + 1);
        return 1;
    }
    return 0;
}

static char *get_edid_videoinput(char *buf, void *usrptr, int ident)
{
    char buffer[FIELD_EDID_VIDEOINPUT_LENGTH + 1];

    strncpy(buf, "Digital", FIELD_EDID_VIDEOINPUT_LENGTH + 1);
    strncpy(buffer, "Digital", FIELD_EDID_VIDEOINPUT_LENGTH + 1);
    if (parse_configuration_file(get_config_file_name(), 
                get_edid_videoinput_handler, buffer) < 0) {
        VLOG_WARNING("Could not load configuration file %s",
                     get_config_file_name());
    }
    else {
        strncpy(buf, buffer, FIELD_EDID_VIDEOINPUT_LENGTH + 1);
    }
    if (usrptr != NULL) {
        struct edid_videoinput_params *params = 
            (struct edid_videoinput_params *) usrptr;
        if (params->edid_videoinput_set == 1)
            strncpy(buf, params->edid_videoinput, FIELD_EDID_VIDEOINPUT_LENGTH + 1);
    }

    return buf;
}

static int write_edid_videoinput_handler(char *name, char *value, void *ptr,
                                   char flag)
{
    char *buf = ptr;
    if (flag) {
        if (buf != NULL
            && strlen(buf) <= FIELD_EDID_VIDEOINPUT_LENGTH) {
            if (strcasecmp(name, "VIDEOINPUT") == 0) {
                sprintf(value, "%s", buf);
                return 1;
            }
        }
    }
    else {
        strcpy(name, "VIDEOINPUT");
        sprintf(value, "%s", buf);
    }
    return 0;
}

static int set_edid_videoinput(char *buf)
{
    if (rewrite_configuration_file(get_config_file_name(),
                                   write_edid_videoinput_handler, buf) < 0) {
        VLOG_WARNING("Could not rewrite configuration file %s",
                     get_config_file_name());
        return RC_ERROR;
    }
    return RC_NORMAL;
}

/**********************************
 * EDID Video Input Configuration *
 **********************************/
static char *videoinputhelp = \
"Current selection is highlighted. Use </U>Up/Down<!U> arrow\n"
"keys to select desired video input and press </U>Enter<!U>.\n";

static char * videoinputselector_cb(int item) {
    if (item >= 0 && item < NUMB_ITEMS)
        sprintf(tempbuf, "%s", videoinput_array[item]);
    else
        sprintf(tempbuf, "???");
    return tempbuf;
}

/*****************************************************
 * Configure EDID EEPROM for Digital or Analog Input *
 *****************************************************/
int configureEdidVideoInput(MENUINFO *minfo) {
    DIALOG        *dialog;
    MENU        *selector;
    int         rc;
    char        keylist[12];
    int         retval = RC_NORMAL;
    int         done = FALSE;
    char        buff[50];
    HAL         *hp;

    hp = minfo->minfo_userptr;
    if (hp != NULL) {
        board_number = hal_get_board_number(hp);
    }

    /*********************
     * Print Menu Title  *
     *********************/
    sprintf(buff, "Channel %d EDID Video Input", board_number+1);

    show_title(minfo, buff);

    /************************************
     * Don't let non-root users do this *
     ************************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 8,
                         "You must be root to configure EDID EEPROM\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }
                                                                                 
    /******************************
     * Create the selector widget *
     ******************************/
    selector = create_selector(NUMB_ITEMS, videoinputselector_cb);

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
    show_info(minfo, videoinputhelp);

    /***************************
     * Get current video input *
     ***************************/
    {
        unsigned char edidbuf[EDID_DATA_LEN];

        if (hal_read_edid_eeprom(hp, edidbuf, sizeof(edidbuf)) < 0) {
            dialog = show_dialog(minfo, 7, 8, "Failed to read EDID EEPROM\n");
            show_info(minfo, PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
            return RC_ERROR;
        }
        video_input = (edidbuf[20] & 0x80) ? 0 : 1;
    }

    set_selector_item(selector, video_input);

   /***************************
     * Let user select an item *
     ***************************/
    while (done == FALSE) {
        strcpy(keylist, "sSqQ");
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

       /*******************************
        * Process custom key requests *
        *******************************/
        switch (keylist[0]) {
        case 'q':
        case 'Q':
            done = TRUE;
            break;
        case '\0':
        case 's':
        case 'S':
            if (rc != video_input) {
                sprintf(tempbuf, "Set Input to %s? " YESNO_STR, 
                        videoinput_array[rc]);
                show_info(minfo, tempbuf);
                if (get_yesno(minfo)) {
                    video_input = rc;
                }
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

    /***********************************
     * Ask if should write to EEPROM   *
     ***********************************/
    sprintf(tempbuf, "Write Video Input to EEPROM?" YESNO_STR);
    show_info(minfo, tempbuf);

    if (get_yesno(minfo)) {
        /************************
         * Show warning message *
         ************************/
        dialog = show_dialog(minfo, 4, 0,
            "Writing to EDID EEPROM with DVI cable connected to a \n"
            "host computer may corrupt the EEPROM.\n"
            "</K></U>Warning:<!U><!K> Make sure the DVI cable is disconnected\n"
            "before proceeding.<!K><!U>");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);

        /*******************************************************************
         * Write to EEPROM w/video input set to Digital not DFP compatible *
         *******************************************************************/
        if (load_edid_eeprom_with_descriptor(hp,
                    (video_input == 0) ? 0x80 : 0x08) < 0) {
            show_info(minfo, "Failed to write EDID EEPROM. " PRESSANYKEY_STR);
            retval = RC_ERROR;
        }
        else {
            set_edid_videoinput(videoinput_array[video_input]);
            show_info(minfo, "EDID EEPROM Updated. " PRESSANYKEY_STR);
        }
        get_anykey(minfo);
    }

    return retval;
}

static int fill_message_buffer(HAL *hp,
                               struct edid_lut_map edid_lut[], 
                               struct LUT_struct lutp[],
                               char *msgbuf,
                               int max_msglen)
{
    int i, msglen = 0;
    struct LUT_struct tmplut;

    for (i = 0; i < MAX_EDID_DESC_COUNT; i++) {
        if (edid_lut[i].type == EDID_LUT_TYPE_LUTID) {
            if (get_lut_entry_by_lutid(hp, &tmplut, edid_lut[i].value)) {
                sprintf(tempbuf, "Desc %d (%dx%d@%d) matches LUT %d.\n", 
                    i+1, tmplut.a_orig_hres, tmplut.a_orig_vres, 
                    (int) (tmplut.a_refresh + 0.1), edid_lut[i].value);
            }
        }
        else if (edid_lut[i].type == EDID_LUT_TYPE_NEWLUT) {
            int j = edid_lut[i].value;
            sprintf(tempbuf, "Desc %d (%dx%d@%d) has no matching LUT\n",
                i+1, lutp[j].a_orig_hres, lutp[j].a_orig_vres,
               (int) (lutp[j].a_refresh + 0.1));
        }
        else {
            sprintf(tempbuf, "Desc %d is not a timing descriptor.\n",i+1);
        }
        strncpy(&msgbuf[msglen], tempbuf, max_msglen - msglen);
        if ((msglen = strlen(msgbuf)) >= max_msglen - 1)
            break;
    }
 
    msgbuf[max_msglen - 1] = '\0';

    return strlen(msgbuf);
}

/**************************************************************
 * Configure EDID EEPROM with Monitor EDID from Loopback Port *
 * TODO: need to add handling for extended EDID descriptors   *
 **************************************************************/
int configureEdidFromMonitor(MENUINFO *minfo) {
                                                                                
    DIALOG              *dialog;
    int                 retval = RC_NORMAL, rc, hwtype, i;
    int                 CEAblocks, msglen, newluts;
    char                title[50], msgbuf[1024];
    struct LUT_struct   lut[MAX_EDID_DESC_COUNT];
    struct edid_lut_map edid_lut[MAX_EDID_LUT_COUNT];
    unsigned char       monbuf[EDID_DATA_LEN];
    HAL                 *hp;

    hp = minfo->minfo_userptr;
    if (hp != NULL) {
        board_number = hal_get_board_number(hp);
    }

    /*********************
     * Print Menu Title  *
     *********************/
    if ((hwtype = hal_get_board_type(hp)) == HARDWARE_V2D_TX)
        sprintf(title, "Board %d EDID EEPROM", board_number+1);
    else
        sprintf(title, "Board %d Loopback Port EDID EEPROM", board_number+1);

    show_title(minfo, title);

    /************************************
     * Don't let non-root users do this *
     ************************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 10,
                       "You must be root to configure EDID EEPROM.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    /*********************
     * Read monitor EDID *
     *********************/
    if (hal_read_mon_edid(hp, monbuf, EDID_DATA_LEN) < 0) {
        dialog = show_dialog(minfo, 7, 15, 
            "Failed to read Monitor EDID.\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

    get_edid_lut_map(hp, monbuf, 0, lut, edid_lut, MAX_EDID_DESC_COUNT);

    msglen = fill_message_buffer(hp, edid_lut, lut, msgbuf, sizeof(msgbuf));

    dialog = show_dialog(minfo, 3, 0, msgbuf);

    /***********************************
     * Check if should write to EEPROM *
     ***********************************/
    sprintf(tempbuf, 
            "Update LUT and Write Monitor EDID to EEPROM?" YESNO_STR);

    show_info(minfo, tempbuf);

    if (get_yesno(minfo)) {

        remove_dialog(dialog);

        /************************
         * Show warning message *
         ************************/
        if (hwtype == HARDWARE_V2D_TX) {
            dialog = show_dialog(minfo, 3, 0,
                "Writing to EDID EEPROM with DVI cable connected\n"
                "to a host computer may corrupt the EEPROM.\n"
                "</K></U>Warning:<!U><!K> Make sure the DVI cable is "
                "disconnected\nbefore proceeding.\n"
                "Note: If problem in reading or writing EDID\n"
                "EEPROM persists, you may need to power cycle\n"
                "the V2D to recover.");
        }
        else {
            dialog = show_dialog(minfo, 3, 0,
                "Writing to EDID EEPROM with DVI cable connected\n"
                "to the local input port may corrupt the EEPROM.\n"
                "</K></U>Warning:<!U><!K> Make sure the DVI cable on "
                "the local\ninput is disconnected before proceeding.\n"
                "Note: If problem in reading or writing EDID\n"
                "EEPROM persists, you may need to power cycle\n"
                "the V2D to recover.");
        }
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);

        /***************************************
         * Update LUT with monitor descriptors *
         ***************************************/
        update_lut_from_buffer(hp, monbuf);

        /**********************
         * Write EDID to FPGA *
         **********************/
        if ((rc = load_edid_eeprom_from_monitor(hp)) < 0) {
            if (rc == -1)
                show_info(minfo, 
                        "Failed to write EDID EEPROM " PRESSANYKEY_STR);
            else
                show_info(minfo, 
                        "Failed to read Monitor EDID " PRESSANYKEY_STR);
            get_anykey(minfo);
            return RC_ERROR;
        }

        show_info(minfo, "EDID EEPROM Updated. " PRESSANYKEY_STR);
        get_anykey(minfo);
    }

    return retval;
}

/**************************************
 * CLI for configure EDID video input *
 **************************************/
static void setup_parameter_values(struct edid_videoinput_params *params) {
    do {
        if (strcasecmp(params->edid_videoinput, "Digital") == 0) {
            params->videoinput_value = EDID_VIDEOINPUT_DIGITAL;
            break;
        }
        if (strcasecmp(params->edid_videoinput, "Analog") == 0) {
            params->videoinput_value = EDID_VIDEOINPUT_ANALOG;
            break;
        }
    } while (0);
}

static int applyEdidVideoInput(struct edid_videoinput_params *params,
                               struct edid_videoinput_params *orig_params) {
    HAL *hp = params->userptr;
    unsigned char edidbuf[EDID_DATA_LEN];
    int video_input;

    if (hp == NULL)
        return RC_ERROR;
    if (params->edid_videoinput_set) {
        if (hal_read_edid_eeprom(hp, edidbuf, sizeof(edidbuf)) < 0) {
            VLOG_INFO("%s: failed to read edid eeprom. errno %d", 
                    __func__, errno);
            return RC_ERROR;
        }
        video_input = (edidbuf[20] & 0x80) ? EDID_VIDEOINPUT_DIGITAL : 
            EDID_VIDEOINPUT_ANALOG;
        if (video_input != params->videoinput_value) { 
            if (load_edid_eeprom_with_descriptor(hp, 
                        (params->videoinput_value == EDID_VIDEOINPUT_DIGITAL) ?
                        0x80 : 0x08) < 0) {
                VLOG_INFO("%s: failed to load edid eeprom. errno %d", 
                    __func__, errno);
                return RC_ERROR;
            }
            else
                return RC_NORMAL;
        }
    }
    return RC_NORMAL;
}

int describe_edid_videoinput_main(int argc, char **argv)
{

    int rc = RC_NORMAL;
    char buf[8192];
    int ctr = 0;
    printf(XML_MENUDESCR_ROOTBEGIN);
    printf("\n");
    printf("   " XML_MENUDESCR_ITEMTYPE, "config");
    printf("   " XML_MENUDESCR_TITLE, "EDIDVideoInput");
    printf("   " XML_MENUDESCR_DESCRIPTION, "Video Input Type");
    printf("   " XML_MENUDESCR_BOARDACCESS, 1 ? "true" : "false");
    printf("   " XML_MENUDESCR_CLICOMMANDNAME, "edid_videoinput");
    printf("   " XML_MENUDESCR_CONFIGFILENAME,
           "/V2O/config/edidvideoinputconf");
    printf("\n");
    printf("   " XML_MENUDESCR_ITEMSROOTBEGIN);
    printf("      " XML_FIELD_START);
    printf(XML_FIELD,
           "videoinput", "EDID Video Input", "string", "Digital", "Analog");
    printf(XML_FIELD_ENUMERATION,
           "enumeration", "Digital , Analog ,");
    printf(XML_FIELD_END);
    printf("\n");
    printf("   " XML_MENUDESCR_ITEMSROOTEND);
    printf("\n");
    printf(XML_MENUDESCR_ROOTEND);
    return rc;
}

int get_edid_videoinput_main(int argc, char **argv)
{
    int opt_idx = 1;
    int rc = RC_NORMAL;
    char buf[8192];
    int ctr = 0;
    struct edid_videoinput_params init_params;
    void *init_param_ptr = NULL;
    HAL *hp = NULL;
    int c, opt_index = 0;

    while (1) {
        c = getopt_long_only(argc, argv, "h", edid_videoinput_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            switch (opt_index) {
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
        print_cli_failure(get_edid_videoinput_string, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }

    memset(&init_params, 0x00, sizeof(init_params));
    if (opt_idx == argc) {
        printf(XML_GROUP, get_edid_videoinput_string);
        print_cli_success(NULL);
        printf("  " XML_OBJENTRY, edid_videoinput_string);
        get_edid_videoinput(buf, init_param_ptr, 0);
        printf("      " XML_ATTRSTR, edid_videoinput_options[ctr++].name, buf);
        printf("  " XML_OBJEND);
        printf(XML_GROUPEND);
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(get_edid_videoinput_string, CLI_REASON_BAD_USAGE);
        printf("\nUsage: get_edid_videoinput -channelNumber Number\n");
        printf("Get EDID Video Input\n");
    }
    if (hp != NULL)
        hal_release(hp);
    return rc;
}

int set_edid_videoinput_main(int argc, char **argv)
{
    int c, opt_index = 0;
    int rc = RC_NORMAL;
    int value;
    char error_string[128];
    struct edid_videoinput_params params;
    struct edid_videoinput_params orig_params;
    struct edid_videoinput_params init_params;
    void *init_param_ptr = NULL;
    char *bad_value_string;
    char buf[64];
    HAL *hp = NULL;

    value = 0;
    invoked_via_cli = 1;
    remote_config_start();
    remote_config_set_config_name(edid_videoinput_string);
    remote_config_set_command_name("set_edid_videoinput");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    memset(&params, 0x00, sizeof(params));
    memset(&orig_params, 0x00, sizeof(orig_params));

    if (view_only() != 0) {
        print_cli_failure(set_edid_videoinput_string, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to set EDID video input parameters\n");
        return RC_ERROR;
    }

    sprintf(buf, "%s:", CLI_REASON_BAD_VALUE);
    bad_value_string = strdup("\n");

    while (1) {
        c = getopt_long_only(argc, argv, "h", edid_videoinput_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 0:
                if (strlen(optarg) > FIELD_EDID_VIDEOINPUT_LENGTH) {
                    rc = RC_INVALID_VALUE;
                    sprintf(buf, "Field 'videoinput' length exceeds %d\n",
                            FIELD_EDID_VIDEOINPUT_LENGTH);
                    bad_value_string = strgrowcat(bad_value_string, buf);
                }
                else if (check_enum(optarg, videoinput_array) == RC_NORMAL) {
                    params.edid_videoinput_set = 1;
                    strncpy(params.edid_videoinput, optarg,
                            FIELD_EDID_VIDEOINPUT_LENGTH + 1);
                }
                else {
                    sprintf(buf,
                            "Field 'videoinput' failed to match enumerated value\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 1:
                remote_config_set_client_data(optarg);
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

    hp = hal_acquire(board_number);
    remote_config_set_board_number(board_number);
    remote_config_set_channel_number(board_number + 1);
    if (hp == NULL) {
        print_cli_failure(set_edid_videoinput_string, CLI_REASON_WRONG_HW);
        remote_config_send_error(CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }

    params.userptr = hp;
    orig_params.userptr = hp;

    memset(&init_params, 0x00, sizeof(init_params));
    if (optind == argc && rc == RC_NORMAL) {
        get_edid_videoinput(orig_params.edid_videoinput, init_param_ptr, 0);
        orig_params.edid_videoinput_set = 1;
        if (params.edid_videoinput_set == 1) {
            remote_config_add_parameter(edid_videoinput_options[0].name,
                                        params.edid_videoinput);
        }
        else {
            strncpy(params.edid_videoinput, orig_params.edid_videoinput,
                    FIELD_EDID_VIDEOINPUT_LENGTH + 1);
        }

        setup_parameter_values(&orig_params);
        setup_parameter_values(&params);
        sprintf(error_string, "Invalid Data");

        rc = acquire_config_lock();
        if (rc != RC_NORMAL) {
            char system_busy_string[64];
            if (rc == RC_LOCKED_OUT)
                sprintf(system_busy_string,
                        "\n Cannot Update: System busy with other config change.\n");
            else
                sprintf(system_busy_string,
                        "\n Cannot Update: System busy.\n");
            print_cli_failure_with_detail(set_edid_videoinput_string,
                                          CLI_REASON_SYSTEM_BUSY,
                                          system_busy_string);
            remote_config_send_error(CLI_REASON_SYSTEM_BUSY);
            return RC_ERROR;
        }

        rc = applyEdidVideoInput(&params, &orig_params);
        release_config_lock();
        if (rc == RC_NORMAL) {

            if (params.edid_videoinput_set == 1) {
                set_edid_videoinput(params.edid_videoinput);
            }
            print_cli_success(set_edid_videoinput_string);
            remote_config_send();
        }
        else {
            print_cli_failure(set_edid_videoinput_string, CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
            rc = RC_ERROR;
        }
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure_with_detail(set_edid_videoinput_string, 
                                      CLI_REASON_BAD_VALUE, bad_value_string);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(set_edid_videoinput_string, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\nUsage: set_edid_videoinput [OPTIONS]\n");
        printf("Set EDID Video Input \n");
        printf("Options:\n");
        printf
            ("        -videoinput Value          Set videoinput: Choice of Digital,  \n");
        printf("                                                     Analog\n");
        printf("        -channelNumber Number \n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}

#endif                  /* LIBONLY */
