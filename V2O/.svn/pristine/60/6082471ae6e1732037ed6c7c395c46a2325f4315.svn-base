

#include <getopt.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "displayVersion.h"
#include "loadnet.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "commonutils.h"
#include "device_detection.h"
#include "vutils.h"
#include "cli_utils.h"
#include "license_verifier.h"


static char *versionhelp = PRESSANYKEY_STR;



#define FIELD_SOFTWARE          1
#define FIELD_DATE              2
#define FIELD_FIRMWARE          3
#define FIELD_HARDWARE          4
#define FIELD_SERIALNO          5
#define FIELD_MODEL             6
#define FIELD_LICENSEID         7

#define FIELD_SOFTWARE_LEN              32
#define FIELD_DATE_LEN                  32
#define FIELD_FIRMWARE_LEN              32
#define FIELD_HARDWARE_LEN              32
#define FIELD_SERIALNO_LEN              32
#define FIELD_MODEL_LEN                 32
#define FIELD_LICENSEID_LEN             40


static char *get_software_version_string(char *buf, void *usrptr,
                                         int ident)
{
    get_software_version(buf, FIELD_SOFTWARE_LEN);
    return buf;
}

static char *get_software_date_string(char *buf, void *usrptr, int ident)
{
    get_software_date(buf, FIELD_DATE_LEN);
    return buf;
}

static char *get_hardware_string(char *buf, void *usrptr, int ident)
{
    // Its ok to pass null on to hal_get_hardware_description
    hal_get_hardware_description((HAL *)usrptr, buf, FIELD_HARDWARE_LEN);
    return buf;
}

static char *get_device_serial_number_string(char *buf, void *usrptr, int ident)
{
    get_device_serial_number(buf, FIELD_SERIALNO_LEN);
    return buf;
}

#ifdef __XPI__
static char *get_model_string(char *buf, void *usrptr, int ident)
{

    if (have_stereo_license())
        sprintf(buf, "Pro3D");
    else if (have_duallink_license())
        sprintf(buf, "Pro");
    else 
        sprintf(buf, "Lite");
    return buf;
}

static char *get_sd_card_id_string(char *buf, void *usrptr, int ident)
{
    get_sd_card_id(buf, FIELD_LICENSEID_LEN);
    return buf;
}

#endif

static FORMITEMDEF versionfitemdefs[] = {

    {FIELD_SOFTWARE, "Software Version",
     "The current software version.",
     FIELD_SOFTWARE_LEN, get_software_version_string, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_DATE, "Software Date",
     "The current software date",
     FIELD_DATE_LEN, get_software_date_string, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_HARDWARE, "Hardware Version",
     "The current hardware version",
     FIELD_HARDWARE_LEN, get_hardware_string, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

     {FIELD_SERIALNO, "Device Serial Number ",
      "Serial Number of the device",
      FIELD_SERIALNO_LEN, get_device_serial_number_string, NULL,
      FTYPE_NONE, 0, 0, O_EDIT, 0},

#ifdef __XPI__
     {FIELD_MODEL, "Model ",
      "Model for NGS-D200 Device",
      FIELD_MODEL_LEN, get_model_string, NULL,
      FTYPE_NONE, 0, 0, O_EDIT, 0},

     {FIELD_LICENSEID, "License ID ",
      "The ID of device on which the license is based",
      FIELD_LICENSEID_LEN, get_sd_card_id_string, NULL,
      FTYPE_NONE, 0, 0, O_EDIT, 0},

#endif

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};

int displayVersion(MENUINFO * minfo)
{

    FORM *form = NULL;
    int rc = E_OK;
    int retval = RC_NORMAL;
    HAL *hp;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    /*************************************************************
     * Get HAL handle to the hardware. We forge ahead even if it *
     * returns null (as may occur on DMS)                        *
     *************************************************************/
    hp = hal_acquire(0);

    /*******************
     * Create the form *
     *******************/
    form = create_form(versionfitemdefs, 1, 0, hp);

    /**********************************
     * Associate windows for the form *
     **********************************/
    set_form_windows(minfo, form);

    /*****************
     * Post the form *
     *****************/
    post_form(form);

    /*************************
     * Process user requests *
     *************************/
    rc = process_form(minfo, form, versionhelp, FORM_DISPLAY_ONLY);

    if (rc == RC_EXIT)
        retval = RC_EXIT;
    else
        retval = RC_NORMAL;


    /************
     * clean-up *
     ************/
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    if (hp != NULL)
        hal_release(hp);

    return retval;
}


static char *get_version = "Get Version";
static char *version_parm = "V2DVersion";
static struct option version_options[] = {
    {"hardware", 1, 0, 0},
    {"serialnumber", 1, 0, 0},
    {"swvers", 1, 0, 0},
    {"swdate", 1, 0, 0},
#ifdef __XPI__
    {"model", 1, 0, 0},
    {"licenseid", 1, 0, 0},
#endif
    {0, 0, 0, 0},
};

#define HARDWARE_IDX          0
#define SERIALNUMBER_IDX      1
#define SWVERSION_IDX         2
#define SWDATE_IDX            3
#define MODEL_IDX             4
#define LICENSEID_IDX         5

int get_version_main(int argc, char **argv)
{
    char buf[65];
    int i, rc = RC_NORMAL;
    HAL *hp;

    hp = hal_acquire(0);

    if (argc == 1) {
        printf(XML_GROUP, get_version);
        print_cli_success(NULL);
        printf("  " XML_OBJENTRY, version_parm);
        for (i = 0;
             i < sizeof(version_options) / sizeof(struct option) - 1;
             i++) {
            if (i == HARDWARE_IDX) {
                get_hardware_string(buf, hp, 0);
            }
            else if (i == SERIALNUMBER_IDX) {
                (void) get_device_serial_number_string(buf, hp, 0);
            }
            else if (i == SWVERSION_IDX) {
                (void) get_software_version_string(buf, hp, 0);
            }
            else if (i == SWDATE_IDX) {
                (void) get_software_date_string(buf, hp, 0);
            }
#ifdef __XPI__
            else if (i == MODEL_IDX) {
                (void) get_model_string(buf, hp, 0);
            }
            else if (i == LICENSEID_IDX) {
                (void) get_sd_card_id_string(buf, hp, 0);
            }
#endif
            printf("    " XML_ATTRSTR, version_options[i].name, buf);
        }
        printf("  " XML_OBJEND);
        printf(XML_GROUPEND);
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(get_version, CLI_REASON_BAD_USAGE);
        printf("\nUsage: get_version\n");
        printf("\nGet software and firmware version from V2D\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}
