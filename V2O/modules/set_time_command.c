
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <netinet/in.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "configureSystem.h"
#include "configureVideo.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vlog.h"
#include "vutils.h"
#include "cli_utils.h"

#define FIELD_MONTH  1
#define FIELD_DAY  2
#define FIELD_HOUR  3
#define FIELD_MIN  4
#define FIELD_SEC  5
#define FIELD_YEAR  6


struct tm tm_now;

static char *month_array[] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL };

static char *get_month(char *buf, void *usrptr, int ident)
{
    
    sprintf(buf, "%s", month_array[tm_now.tm_mon]);
    return buf;
}

static char *get_day(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%d", tm_now.tm_mday);
    return buf;
}

static char *get_hour(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%d", tm_now.tm_hour);
    return buf;
}

static char *get_min(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%d", tm_now.tm_min);
    return buf;
}

static char *get_sec(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%d", tm_now.tm_sec);
    return buf;
}

static char *get_year(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%d", tm_now.tm_year + 1900);
    return buf;
}


static FORMITEMDEF itemdefs[] = {
    {FIELD_HOUR, "Hour",
     "Hour 0-23",
     8, get_hour, NULL,
     FTYPE_INTEGER_RANGE, 0, 23, O_NULLOK, 0},

    {FIELD_MONTH, "Month",
     "Month 1-12",
     8, get_month, NULL,
     FTYPE_ENUM_RANGE, (int) month_array, 0, O_NULLOK, FFLAG_FOLLOW},

    {FIELD_MIN, "Min",
     "Min 0-59",
     8, get_min, NULL,
     FTYPE_INTEGER_RANGE, 0, 59, O_NULLOK, 0},

    {FIELD_DAY, "Day",
     "Day 1-31",
     8, get_day, NULL,
     FTYPE_INTEGER_RANGE, 1, 31, O_NULLOK, FFLAG_FOLLOW},

    {FIELD_SEC, "Sec",
     "Sec 0-59",
     8, get_sec, NULL,
     FTYPE_INTEGER_RANGE, 0, 59, O_NULLOK, 0},

    {FIELD_YEAR, "Year",
     "Year 1970-2037",
     8, get_year, NULL,
     FTYPE_INTEGER_RANGE, 1970, 2037, O_NULLOK, FFLAG_FOLLOW},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};


/* Menu command */
int setTime(MENUINFO *minfo) {

    DIALOG      *dialog;
    int         retval = RC_NORMAL;
    FORM *form = NULL;
    int rc = E_OK;
    time_t now;
    char buff[128];
    int mon, day, year, hour, min, sec, i;

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
                         "This utility sets the date/time on the system.\n"
                         "Time zones are not supported. Use the current UTC time.\n"
                         "UTC is Coordinated Universal Time.\n"
                         "</K></U>Warning:<!U><!K> This command will reboot the system.\n"
                         "All connected clients will be disconnected.");

    show_info(minfo, "Proceed with setting date/time? " YESNO_STR);

    if (!get_yesno(minfo)) {
        remove_dialog(dialog);
        return RC_NORMAL;
    }


    remove_dialog(dialog);

    // find the current time
    time(&now);
    if (gmtime_r(&now, &tm_now) == NULL)
        return RC_ERROR;
           
    // create the form 
    form = create_form(itemdefs, 1, 0, NULL);
    set_form_windows(minfo, form);
    post_form(form);

    while (1) {

        rc = process_form(minfo, form, formmesg, 0);

        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            break;
        }

        if (rc == RC_QUIT) {
            retval = RC_NORMAL;
            break;
        }
        else {
            
            if (get_form_status(form) == TRUE) {
                show_info(minfo, "Update Time?" YESNO_STR);
                if (get_yesno(minfo)) {
                    get_field_buffer(form, FIELD_HOUR, buff);
                    hour = atoi(buff);
                    get_field_buffer(form, FIELD_MIN, buff);
                    min = atoi(buff);
                    get_field_buffer(form, FIELD_SEC, buff);
                    sec = atoi(buff);
                    get_field_buffer(form, FIELD_YEAR, buff);
                    year = atoi(buff);
                    get_field_buffer(form, FIELD_DAY, buff);
                    day = atoi(buff);

                    get_field_buffer(form, FIELD_MONTH, buff);
                    
                    for (i = 0; i < 12; i++) {
                        if (month_array[i] == NULL)
                            break;
                        if (strcasecmp(month_array[i], buff) == 0)
                            break;
                    }
                    mon = i+1;

                    snprintf(buff, 63, "echo date -s %02d%02d%02d%02d%4d.%02d > /tmp/date.sh", 
                             mon, 
                             day,
                             hour,
                             min,
                             year,
                             sec);

                    system_direct_command(buff);

                    // we will be rebooting so exit from menu
                    return RC_EXIT;
                }
                break;
            }
            else {
                show_info(minfo, "No change in time. Quit?" YESNO_STR);
                if (get_yesno(minfo))
                    break;
            }

        }
    }

    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    return RC_NORMAL;
}

static void show_usage(void) {
    printf("\nUsage: set_time [OPTIONS]\n");
    printf("Set Date/Time on device.\n");
    printf("Options:\n");
    printf("        -hour number               Hour 0-23\n");
    printf("        -min number                Minutes 0-59\n");
    printf("        -seconds number            Seconds 0-59\n");
    printf("        -month number              Month 1-12\n");
    printf("        -day number                Day 1-31\n");
    printf("        -year number               Year 1970-2037\n");
}


static struct option set_time_options[] = {
    {"hour", 1, 0, 0},
    {"min", 1, 0, 0},
    {"seconds", 1, 0, 0},
    {"month", 1, 0, 0},
    {"day", 1, 0, 0},
    {"year", 1, 0, 0},
    {0, 0, 0, 0}
};

/* CLI command */
int set_time_main(int argc, char **argv) {
    char buff[64];
    char buffer[64];
    int retcode;
    int c, opt_index = 0;
    int error;

    int hour, min, seconds, month, day, year;

    hour = min = seconds = month = day = year = -1;

    strcpy(buffer, "");

    if (view_only() != 0) {
        printf("You must have admin privileges to set the time.\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", set_time_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 0:
                hour = atoi(optarg);
                break;
            case 1:
                min = atoi(optarg);
                break;
            case 2:
                seconds = atoi(optarg);
                break;
            case 3:
                month = atoi(optarg);
                break;
            case 4:
                day = atoi(optarg);
                break;
            case 5:
                year = atoi(optarg);
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

    /* Check if all parameters have been supplied and are in correct range */
    error = 0;
    if (hour < 0 || hour > 23)
        error = 1 ;
    if (min < 0 || min > 59)
        error = 1;
    if (seconds < 0 || seconds > 59)
        error = 1;
    if (month < 1 || month > 12)
        error = 1;
    if (day < 1 || day > 31)
        error = 1;
    if (year < 1970 || year > 2037)
        error = 1;

    if (error == 1) {
        printf("ERROR: Insufficient or out of range arguments.");
        show_usage();
        return RC_ERROR;
    }

    /* Create the date string */
    sprintf(buffer, "%02d%02d%02d%02d%4d.%02d", 
            month, day, hour, min, year, seconds);

    /* Make sure that the date string is valid */
    snprintf(buff, 63, "date -d %s", buffer);

    retcode = system_command(buff);
    if (retcode != 0) {
        printf("\nError: Invalid date/time string.\n");
        show_usage();
        return RC_ERROR;
    }

    /* Write the date string to the file. The watchdog process will
     * pick it up from here and set the time */
    printf("The system will now reboot. Please wait...\n");
    snprintf(buff, 63, "echo date -s %s > /tmp/date.sh", buffer);
    system_direct_command(buff);

    return RC_NORMAL;
}
