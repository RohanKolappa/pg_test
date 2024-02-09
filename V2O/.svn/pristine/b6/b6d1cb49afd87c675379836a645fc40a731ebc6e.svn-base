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
#include "configfile_parser.h"
#include "vlog.h"
#include "vutils.h"
#include "cli_utils.h"


#define FIELD_URL               1
#define FIELD_URL_LENGTH        128


static int get_url_handler(char *name, char *value, void *ptr)
{
    char *buf = ptr;
    if (strcasecmp(name, "URL") == 0) {
        if (buf != NULL)
            strncpy(buf, value, FIELD_URL_LENGTH);
        return 1;
    }
    return 0;
}

static char *get_url(char *buf, void *usrptr, int ident)
{
    char buffer[FIELD_URL_LENGTH + 1];
    strncpy(buf, "http://", FIELD_URL_LENGTH);
    strncpy(buffer, "http://", FIELD_URL_LENGTH);
    if (parse_config_file("/V2O/config/urlcache.txt",
                          get_url_handler, buffer) < 0) {
        VLOG_WARNING("Could not load configuration file %s",
                      "/V2O/config/urlcache.txt");
        return buf;
    }
    strncpy(buf, buffer, FIELD_URL_LENGTH);
    return buf;
}

static int write_url_handler(char *name, char *value, void *ptr,
                             char flag)
{
    char *buf = ptr;
    if (flag) {
        if (buf != NULL && strlen(buf) <= FIELD_URL_LENGTH) {
            if (strcasecmp(name, "URL") == 0) {
                sprintf(value, "%s", buf);
                return 1;
            }
        }
    }
    else {
        strcpy(name, "URL");
        sprintf(value, "%s", buf);
    }
    return 0;
}

static int set_url(char *buf)
{
    if (rewrite_config_file("/V2O/config/urlcache.txt",
                            write_url_handler, buf) < 0) {
        VLOG_WARNING("Could not rewrite configuration file %s",
                      "/V2O/config/urlcache.txt");
        return RC_ERROR;
    }
    return RC_NORMAL;
}

static FORMITEMDEF itemdefs[] = {
    {FIELD_URL, "URL",
     "The URL where the image files are located",
     128, get_url, NULL,
     FTYPE_NONE, 0, 0, O_NULLOK, 0},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};


/* #define scroll_print(format, args...) {                         \ */
/*         sprintf(strbuf, format, ## args);                       \ */
/*         waddstr(minfo->minfo_menuscreen->window, strbuf);       \ */
/*         wrefresh(minfo->minfo_menuscreen->window);              \ */
/*         VLOG_INFO(format, ## args);                            \ */
/*     } */

/* Menu command */
int upgradeSoftware(MENUINFO *minfo) {

    DIALOG      *dialog;
    int         retval = RC_NORMAL;
    char strbuf[129];
    char command[300];
    char inbuf[128];
    FORM *form = NULL;
    int rc = E_OK;
    int failed = 0;

    FILE *inpipe;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    /**********************************
     * Don't let non-root users do it *
     **********************************/
    if (ruid != 0) {
        dialog = show_dialog(minfo, 7, 0,
                             "You do not have admin privileges to perform this operation\n");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return retval;
    }

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

            
            get_field_buffer(form, FIELD_URL, strbuf);

            set_url(strbuf); /* cache the URL */

            sprintf(command, "/bin/software_upgrade.sh %s 2>&1", strbuf);

            unpost_form(form);
            unset_form_windows(form);
            delete_form(form);
            
            
            dialog = show_dialog(minfo, 4, 0,
                                 "This utility will allow you to upgrade software\n"
                                 "on this device. "
                                 "The device will reboot automatically.\n\n");
            
            show_info(minfo, "Proceed with software upgrade?" YESNO_STR);
            
            if (get_yesno(minfo)) {

                retval = acquire_config_lock();
                if (retval != RC_NORMAL) {
                    if (retval == RC_LOCKED_OUT)
                        show_info(minfo, 
                                "Cannot Upgrade: System busy with other config change.\n" 
                                PRESSANYKEY_STR);
                    else
                        show_info(minfo, 
                                "Cannot Upgrade: System busy.\n" 
                                PRESSANYKEY_STR);

                    get_anykey(minfo);
                    return RC_ERROR;
                }

                system_command("/bin/touch " REBOOT_LOCK_FILE);

                show_info(minfo, " ");
                
                remove_dialog(dialog);
                
                scrollok(minfo->minfo_menuscreen->window, TRUE);
                idlok(minfo->minfo_menuscreen->window, TRUE);

                scroll_print(minfo, "\n");
                scroll_print(minfo, "Upgrading Software...\n");

                inpipe = popen(command, "r");
                if (inpipe == NULL) {
                    system_command("/bin/rm -f " REBOOT_LOCK_FILE);
                    release_config_lock();
                    return retval;
                }

                while (fgets(inbuf, sizeof(inbuf), inpipe)) {

                    if (strncmp(inbuf, "INFO:", 5) == 0) {
                        scroll_print(minfo, "%s", inbuf+6); 
                    }
                    if (strncmp(inbuf, "ERROR:", 6) == 0) {
                        scroll_print(minfo, "%s", inbuf); 
                        failed = 1;
                    }
                }
                
                if (pclose(inpipe) != 0) {
                    failed = 1;
                }

                if (failed) {
                    system_command("/bin/rm -f " REBOOT_LOCK_FILE);
                    release_config_lock();
                    show_info(minfo, "Upgrade Failed. " PRESSANYKEY_STR);
                    get_anykey(minfo);
                }
                else {
                    release_config_lock();
                    show_info(minfo, "Done.\n");
                    return RC_EXIT;
                }
            }
            return retval;
        }
    }

    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    return retval;
}

static void show_usage(void) {
    printf("\nUsage: upgrade_device [OPTIONS]\n");
    printf("Upgrade Device from given URL.\n");
    printf("Options:\n");
    printf("        -url string               The URL to upgrade from. If not provided, uses cached URL.\n");
}


static struct option upgrade_device_options[] = {
    {"url", 1, 0, 0},
    {0, 0, 0, 0}
};

/* CLI command */
int upgrade_device_main(int argc, char **argv) {
    char command[300];
    FILE *inpipe;
    char inbuf[129];
    char url[129];
    char *url_arg = NULL;
    int c, opt_index = 0;
    int rc = RC_NORMAL;
    int failed = 0;

    if (view_only() != 0) {
        printf("You must have admin privileges to upgrade device.\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", upgrade_device_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 0:
                url_arg = strdup (optarg);
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




    /* Get cahced URL */
    get_url(url, NULL, 0);
    
    /* Override from command line */
    if (url_arg != NULL) {
        strncpy(url, url_arg, 127);
    } else {
        printf("WARNING: Using cached URL: \"%s\"...\n", url);
        sleep(1);
    }
    
    /* Cache the URL used */
    set_url(url);
    
    /* Now try the upgrade */
    sprintf(command, "/bin/software_upgrade.sh %s 2>&1", url);

    rc = acquire_config_lock();
    if (rc != RC_NORMAL) {
        if (rc == RC_LOCKED_OUT) 
            printf("ERROR: Cannot Upgrade: System busy with other config change.\n");
        else
            printf("ERROR: Cannot Upgrade: System busy.\n");

        return RC_ERROR;
    }
    
    system_command("/bin/touch " REBOOT_LOCK_FILE);

    inpipe = popen(command, "r");
    if (inpipe == NULL) {
        system_command("/bin/rm -f " REBOOT_LOCK_FILE);
        release_config_lock();
        return RC_ERROR;
    }
    

    while (fgets(inbuf, sizeof(inbuf), inpipe)) {

        if (strncmp(inbuf, "INFO:", 5) == 0) {
            printf("%s", inbuf+6); 
        }
        if (strncmp(inbuf, "ERROR:", 6) == 0) {
            printf("%s", inbuf); 
            failed = 1;
        }
        
    }
    pclose(inpipe);

    if(failed == 1)
        system_command("/bin/rm -f " REBOOT_LOCK_FILE);

    release_config_lock();

    if (failed == 1)
        return RC_ERROR;

    return RC_NORMAL;
}
