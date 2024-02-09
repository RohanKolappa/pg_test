#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <signal.h>
#include <getopt.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "configureVideo.h"
#include "loadnet.h"
#include "commonutils.h"
#include "validationroutines.h"
#include "vutils.h"
#include "vlog.h"
#include "remoteConfig.h"
#include "cli_utils.h"


#define FIELD_ENABLE_LEN        3
#define FIELD_POS_LEN           6


static char *add_auth_entry = "Add Authorization Entry";
static char *del_auth_entry = "Delete Authorization Entry";
static char *mod_auth_entry = "Modify Authorization Entry";
static char *get_auth_table = "Get Authorization Table";
static char *describe_auth_table = "Describe Authorization Table";
static char *auth_table_string = "AuthorizationTable";
static char *auth_entry_string = "AuthorizationEntry";
static struct option auth_options[] = {
    {"ip", 1, 0, 0},
    {"mask", 1, 0, 0},
    {"enable", 1, 0, 0},
    {"maxBandwidth", 1, 0, 0},
    {"burstRatio", 1, 0, 0},
    {"burstDuration", 1, 0, 0},
    {"lowCompression", 1, 0, 0},
    {"highCompression", 1, 0, 0},
    {"unicast", 1, 0, 0},
    {"preempt", 1, 0, 0},
    {"multicastInit", 1, 0, 0},
    {"multicastJoin", 1, 0, 0},
    {"clientData", 1, 0, 0},
    {"sysProfile", 1, 0, 0},
    {0, 0, 0, 0}
};

static struct option auth_delete_options[] = {
    {"clientData", 1, 0, 0},
    {0, 0, 0, 0}
};


static struct list_head authlist_table_head;

static int add_authlist_dialog(MENUINFO * minfo, int idx, int req_code);
static int add_authlist(int idx,
                        const char *network,
                        const char *netmask,
                        int enable,
                        const char *maxbw,
                        int minc,
                        int maxc,
                        const char *burstratio,
                        int burstduration,
                        int unicast,
                        int multicast_init,
                        int multicast_part, int preemptible);
static int delete_authlist(int idx, int *authidp);
static struct authlist_entry *get_duplicate_authlist_entry(const char
                                                           *network,
                                                           const char
                                                           *netmask,
                                                           int auth_index);

static char *authlisthelp =
    "Press </U>A<!U> to Add Entry, </U>D<!U> to Delete Entry,\n"
    "</U>M<!U> to Modify Selected Entry, </U>Enter<!U> to Browse Entries\n"
    "or </U>Q<!U> to Quit. ";

static char *authlisthelp2 =
    "Press </U>Enter<!U> to Browse Entries or </U>Q<!U> to Quit. ";

/*******************************************************
 * Find and return authlist entry matching given index *
 *******************************************************/
static struct authlist_entry *get_authlist_entry_by_index(int index)
{

    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, &authlist_table_head) {
        if (count++ == index)
            return list_entry(tmp, struct authlist_entry, authlist_list);

    }
    return NULL;
}

static char *authlistselector_cb(int item)
{
    struct authlist_entry *authlistentryp;

    authlistentryp = get_authlist_entry_by_index(item);

    sprintf(tempbuf, "Entry not found");
    if (authlistentryp != NULL && authlistentryp->network[0] != '\0') {
/*      sprintf(tempbuf, "%-16s %-16s %4s %2d %2d %s",  */
        sprintf(tempbuf, "%-16s %-16s %-8s %s",
                authlistentryp->network,
                authlistentryp->netmask, authlistentryp->max_bandwidth,
/*              authlistentryp->min_compression, */
/*              authlistentryp->max_compression, */
                authlistentryp->enable ? "E" : "D");
    }
    return tempbuf;
}

/* This menu has list_non_immediate kind of fields. */

int configureAuthList(MENUINFO * minfo)
{
    MENU *selector;
    DIALOG *dialog;
    char *help;
    int rc, idx;
    int authlistcount;
    char keylist[12];
    char titlebuf[120];
    int retval = RC_NORMAL;

    /********************
     * Print Menu title *
     ********************/
    sprintf(titlebuf, "%s\n  \n</U>    %-16s %-16s %s<!U>",
            minfo->minfo_title,
            "Host/Network", "Netmask", "BW    Enabled?");
    show_title(minfo, titlebuf);

    /************************************
     * Get current number of authlists. *
     ************************************/
    authlistcount = parse_authlist(&authlist_table_head);

    if (authlistcount == 0) {
        dialog = show_dialog(minfo, 8, 15,
                             "</K/U>Empty Connection Authorization Table!!<!K!U>");
        /****************************************************
         * Non-root users are not allowed to edit the table *
         ****************************************************/
        if (ruid != 0) {
            show_info(minfo, "You must be root to add entries. "
                      PRESSANYKEY_STR);
            get_anykey(minfo);
            remove_dialog(dialog);
            return retval;
        }

        /**************************************************
         * Ask for authlist addition if no authlists yet. * 
         **************************************************/
        show_info(minfo, "Add new Entry?" YESNO_STR);
        if (get_yesno(minfo)) {
            remove_dialog(dialog);
            rc = add_authlist_dialog(minfo, 0, 0);
            if (rc == RC_EXIT)
                return RC_EXIT;
            if (rc != RC_NORMAL)
                return retval;
            authlistcount = count_authlist(&authlist_table_head);
            if (authlistcount == 0)
                return retval;
            show_title(minfo, titlebuf);
        }
        else {
            remove_dialog(dialog);
            return retval;
        }
    }

    /**********************************************************
     * We have at least one authlist now. Create the selector *
     * widget.                                                *
     **********************************************************/
    selector = create_selector(authlistcount, authlistselector_cb);

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
    help = (ruid == 0) ? authlisthelp : authlisthelp2;
    show_info(minfo, help);

    /***************************
     * Let user select an item *
     ***************************/
    while (1) {
        strcpy(keylist, "dDaAmM");

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
         * Don't process requests other than browse   *
         * for non-root users                         * 
         **********************************************/
        if (ruid != 0 && keylist[0] != '\0')
            continue;

        /*******************************
         * Process custom key requests *
         *******************************/
        switch (keylist[0]) {

        case 'd':
        case 'D':
            /******************************
             * Delete selected authlist *
             ******************************/
            sprintf(tempbuf, "Delete Entry at Index %d ?" YESNO_STR,
                    rc + 1);
            show_info(minfo, tempbuf);
            if (get_yesno(minfo)) {
                int authid = 0;

                remote_config_start();
                remote_config_set_config_name(auth_entry_string);
                remote_config_set_command_name("del_auth_entry");
                remote_config_set_operation
                    (REMOTE_CONFIG_OPERATION_DELETE);
                remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);

                delete_authlist(rc, &authid);

                remote_config_set_index(authid);
                remote_config_send();

                authlistcount = count_authlist(&authlist_table_head);
                save_authlist(&authlist_table_head);    /* write out to file */

                if (authlistcount == 0) {
                    show_info(minfo,
                              "Deleted last Authorization Entry. "
                              PRESSANYKEY_STR);
                    get_anykey(minfo);
                    goto QUIT_LABEL;
                }               /* else show updated authlist list */
                unpost_menu(selector);
                refresh_selector(selector, authlistcount,
                                 authlistselector_cb);
                post_menu(selector);
            }
            show_info(minfo, help);
            break;

        case 'a':
        case 'A':
            rc = -1;            /* idx = 0 for Add entry */
        case 'm':
        case 'M':
            /**************************
             * Add or modify authlist *
             **************************/
            unpost_menu(selector);
            idx = rc + 1;
            rc = add_authlist_dialog(minfo, idx, 0);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            if (rc == RC_NORMAL) {      /* new authlist successfully added */
                authlistcount = count_authlist(&authlist_table_head);
                refresh_selector(selector, authlistcount,
                                 authlistselector_cb);
                if (idx > 0)
                    set_selector_item(selector, idx - 1);
            }
            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, help);
            break;

        case '\0':
            /********************
             * Display authlist *
             ********************/
            unpost_menu(selector);
            idx = rc + 1;
            rc = add_authlist_dialog(minfo, idx, 1);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, help);
            break;

        }

    }

  QUIT_LABEL:

    /************
     * Clean-up *
     ************/
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);
    return retval;
}




#define FIELD_AUTH_NETWORK              1
#define FIELD_AUTH_NETMASK              2
#define FIELD_AUTH_ENABLE               3
#define FIELD_AUTH_MAX_BANDWIDTH        4
#define FIELD_AUTH_MIN_COMPRESSION      5
#define FIELD_AUTH_MAX_COMPRESSION      6
#define FIELD_AUTH_UNICAST              7
#define FIELD_AUTH_MULTICAST_INIT       8
#define FIELD_AUTH_MULTICAST_PART       9
#define FIELD_AUTH_BURST_RATIO          10
#define FIELD_AUTH_BURST_DURATION       11
#define FIELD_AUTH_PREEMPT              12

#define FIELD_BURST_RATIO_LEN           7
#define FIELD_BURST_DURATION_LEN        6



static char *get_auth_network(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "");
    else
        sprintf(buf, "%s", authlistentryp->network);
    return buf;
}

static char *get_auth_netmask(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "255.255.255.0");
    else
        sprintf(buf, "%s", authlistentryp->netmask);
    return buf;
}

static char *get_auth_enable(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "Yes");
    else if (authlistentryp->enable > 0)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "No");
    return buf;
}

static char *get_auth_max_bandwidth(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "10M");
    else
        sprintf(buf, "%s", authlistentryp->max_bandwidth);
    return buf;
}

static char *get_auth_min_compression(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "6");
    else
        sprintf(buf, "%d", authlistentryp->min_compression);
    return buf;
}

static char *get_auth_max_compression(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "6");
    else
        sprintf(buf, "%d", authlistentryp->max_compression);
    return buf;

}

static char *get_auth_burst_ratio(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "1.0");
    else
        sprintf(buf, "%.2f", authlistentryp->burst_ratio);

    return buf;
}

static char *get_auth_burst_duration(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "50");
    else
        sprintf(buf, "%d", authlistentryp->burst_duration);

    return buf;
}

static char *get_allow_unicast(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "Yes");
    else if (authlistentryp->unicast > 0)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "No");
    return buf;
}

static char *get_preemptible(char *buf, void *usrptr, int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "No");
    else if (authlistentryp->unicast_preemptible > 0)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "No");
    return buf;
}

static char *get_allow_multicast_initiation(char *buf, void *usrptr,
                                            int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "Yes");
    else if (authlistentryp->multicast_init > 0)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "No");
    return buf;
}

static char *get_allow_multicast_participation(char *buf, void *usrptr,
                                               int ident)
{
    struct authlist_entry *authlistentryp =
        (struct authlist_entry *) usrptr;

    if (authlistentryp == NULL)
        strcpy(buf, "Yes");
    else if (authlistentryp->multicast_part > 0)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "No");
    return buf;
}

static FORMITEMDEF authlistfitemdefs[] = {

    {FIELD_AUTH_NETWORK, "Host/Network IP",
     "Enter host name, IP address or Network for allowed hosts.\n"
     "This is the name, IP address or domain name of the server to be connected to.",
     INET_NAME_LEN, get_auth_network, check_inetname,
     FTYPE_NONE, 0, 0, O_NULLOK, 0},

    {FIELD_AUTH_NETMASK, "Netmask",
     "Enter the netmask for the network from which clients will be allowed to connect."
     "This is the netmask for the static serverlist to be added.",
     INET_ADDR_LEN, get_auth_netmask, check_netmask,
     FTYPE_IPV4, 0, 0, O_NULLOK, 0},

    {FIELD_AUTH_ENABLE, "Enable Rule?",
     "Enter Yes to enable this Authorization rule. Else enter No.\n",
     FIELD_ENABLE_LEN, get_auth_enable, NULL,
     FTYPE_ENUM_RANGE, (int) yesnolist, 0, O_NULLOK, 0},

    {FIELD_AUTH_MAX_BANDWIDTH, "Max Bandwidth",
     "Enter the maximum bandwidth this rule will be limited to."
     "This is the netmask for  the static serverlist to be added.",
     BANDWIDTH_LEN, get_auth_max_bandwidth, NULL,
     FTYPE_REGEXP, (int) regexp_bandwidth, 0, O_NULLOK, 0},

    {FIELD_AUTH_BURST_RATIO, "Burst Ratio",
     "Enter the Burst Ratio between 1.0 and 100.0. This is the factor by which\n"
     "the configured flow rate will be allowed to burst.",
     FIELD_BURST_RATIO_LEN, get_auth_burst_ratio, NULL,
     FTYPE_NUMERIC_LOW, 1, 100, O_NULLOK, 0},

    {FIELD_AUTH_BURST_DURATION, "Burst Duration",
     "Enter the Burst Duration in milliseconds.",
     FIELD_BURST_DURATION_LEN, get_auth_burst_duration, NULL,
     FTYPE_INTEGER_RANGE, 0, 10000, O_NULLOK, FFLAG_FOLLOW},

    {FIELD_AUTH_MIN_COMPRESSION, "Low Compression",
     "Enter the minimum compression value as an integer.",
     FIELD_POS_LEN, get_auth_min_compression, NULL,
     FTYPE_INTEGER_RANGE, 0, 10, O_NULLOK, 0},

    {FIELD_AUTH_MAX_COMPRESSION, "High Compression",
     "Enter the maximum compression value as an integer.",
     FIELD_POS_LEN, get_auth_max_compression, NULL,
     FTYPE_INTEGER_RANGE, 0, 10, O_NULLOK, FFLAG_FOLLOW},

    {FIELD_AUTH_UNICAST, "Unicast?",
     "Enter Yes to allow client to do Unicast. Else enter No.",
     FIELD_ENABLE_LEN, get_allow_unicast, NULL,
     FTYPE_ENUM_RANGE, (int) yesnolist, 0, O_NULLOK, 0},

    {FIELD_AUTH_PREEMPT, "Preemptible?",
     "Enter Yes to allow other clients to preempt this client's\n"
     "connection. Else enter No.",
     FIELD_ENABLE_LEN, get_preemptible, NULL,
     FTYPE_ENUM_RANGE, (int) yesnolist, 0, O_NULLOK, FFLAG_FOLLOW},

    {FIELD_AUTH_MULTICAST_INIT, "Multicast Init.?",
     "Enter Yes to allow client to do Unicast. Else enter No.",
     FIELD_ENABLE_LEN, get_allow_multicast_initiation, NULL,
     FTYPE_ENUM_RANGE, (int) yesnolist, 0, O_NULLOK, 0},

    {FIELD_AUTH_MULTICAST_PART, "Multicast Join.?",
     "Enter Yes to allow client to do Unicast. Else enter No.",
     FIELD_ENABLE_LEN, get_allow_multicast_participation, NULL,
     FTYPE_ENUM_RANGE, (int) yesnolist, 0, O_NULLOK, FFLAG_FOLLOW},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}

};


/******************************************************************
 * Inserts a new authlist at given index or modifies the selected *
 * entry                                                          *
 ******************************************************************/
static int add_authlist_dialog(MENUINFO * minfo, int idx, int req_code)
{

    FORM *form = NULL;
    int rc = 0;
    char networkbuf[INET_ADDR_LEN + 1];
    char tmpbuf[INET_NAME_LEN + 1];
    char netmaskbuf[INET_ADDR_LEN + 1];
    char enablebuf[FIELD_ENABLE_LEN + 1];
    char maxbwbuf[BANDWIDTH_LEN + 1];
    char compbuf[FIELD_POS_LEN + 1];
    char burstratiobuf[FIELD_BURST_RATIO_LEN + 1];
    int minc, maxc, burstduration;
    int retval = RC_NORMAL;
    int enable = 0, unicast = 0, multicast_init = 0, multicast_part = 0;
    int preemptible = 0;
    struct authlist_entry *authlistentryp = NULL;
    struct authlist_entry *authlistentrydup = NULL;
    int authlistcount;
    char titlebuf[80];
    int auth_idx = 0;

    authlistcount = count_authlist(&authlist_table_head);

    /********************
     * Print Menu title *
     ********************/
  redraw:

    if (req_code == 1) {
        sprintf(titlebuf, "Display Entry at Index %d", idx);
        show_title(minfo, titlebuf);
    }
    else if (idx == 0)
        show_title(minfo, "Add New Entry");
    else
        show_title(minfo, "Modify Entry");


    /*******************
     * Create the form *
     *******************/
    if (idx > 0) {
        authlistentryp = get_authlist_entry_by_index(idx - 1);
        auth_idx = authlistentryp->authid;
    }
    form = create_form(authlistfitemdefs, 0, 0, authlistentryp);

    /**********************************
     * Associate windows for the form *
     **********************************/
    set_form_windows(minfo, form);

    /*****************
     * Post the form *
     *****************/
    post_form(form);


    /***************************************************
     * Just show and exit in case of a Display request *
     ***************************************************/
    if (req_code == 1) {
        wrefresh(minfo->minfo_menuscreen->window);
        show_info(minfo,
                  "Press </U>Space<!U> or </U>N<!U> for next entry, </U>P<!U> for previous entry\n"
                  "Or </U>Q<!U> to Quit.");
        rc = get_anykey(minfo);

        unpost_form(form);
        unset_form_windows(form);
        delete_form(form);

        if (rc == '')
            return RC_EXIT;
        else if (rc == 'Q' || rc == 'q')
            return RC_ERROR;    /* Nothing wrong really */
        else if (rc == ' ' || rc == 'N' || rc == 'n' || rc == KEY_DOWN) {
            if (idx < authlistcount)
                idx = idx + 1;
        }
        else if (rc == 'P' || rc == 'p' || rc == KEY_UP) {
            if (idx > 1)
                idx = idx - 1;
        }

        goto redraw;
    }

    /*************************
     * Process user requests *
     *************************/
    while (1) {
        rc = process_form(minfo, form, formmesg, 0);
        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            break;
        }
        if (rc == RC_QUIT) {
            retval = RC_ERROR;
            break;
        }
        else {
            if (get_form_status(form) == TRUE) {
                if (idx == 0)
                    show_info(minfo, "Add this Entry?" YESNO_STR);
                else
                    show_info(minfo, "Update this Entry?" YESNO_STR);

                if (get_yesno(minfo)) {

                    get_field_buffer(form, FIELD_AUTH_NETWORK, tmpbuf);
                    if (isdigit(tmpbuf[0])) {
                        strncpy(networkbuf, tmpbuf, INET_ADDR_LEN);
                        networkbuf[INET_ADDR_LEN] = '\0';
                    }
                    else {
                        show_info(minfo,
                                  "Resolving name to IP address. Please wait...");
                        print_ip_addr(networkbuf, tmpbuf);
                    }
                    get_field_buffer(form, FIELD_AUTH_NETMASK, netmaskbuf);
                    get_field_buffer(form, FIELD_AUTH_ENABLE, enablebuf);
                    if (strcasecmp(enablebuf, "yes") == 0)
                        enable = 1;
                    get_field_buffer(form, FIELD_AUTH_MAX_BANDWIDTH,
                                     maxbwbuf);
                    get_field_buffer(form, FIELD_AUTH_MIN_COMPRESSION,
                                     compbuf);
                    minc = atoi(compbuf);
                    get_field_buffer(form, FIELD_AUTH_MAX_COMPRESSION,
                                     compbuf);
                    maxc = atoi(compbuf);
                    get_field_buffer(form, FIELD_AUTH_BURST_RATIO,
                                     burstratiobuf);
                    get_field_buffer(form, FIELD_AUTH_BURST_DURATION,
                                     compbuf);
                    burstduration = atoi(compbuf);
                    get_field_buffer(form, FIELD_AUTH_UNICAST, enablebuf);
                    if (strcasecmp(enablebuf, "yes") == 0)
                        unicast = 1;
                    get_field_buffer(form, FIELD_AUTH_MULTICAST_INIT,
                                     enablebuf);
                    if (strcasecmp(enablebuf, "yes") == 0)
                        multicast_init = 1;
                    get_field_buffer(form, FIELD_AUTH_MULTICAST_PART,
                                     enablebuf);
                    if (strcasecmp(enablebuf, "yes") == 0)
                        multicast_part = 1;
                    get_field_buffer(form, FIELD_AUTH_PREEMPT, enablebuf);
                    if (strcasecmp(enablebuf, "yes") == 0)
                        preemptible = 1;

                    remote_config_start();
                    remote_config_set_config_name(auth_entry_string);
                    if (idx == 0) {
                        remote_config_set_command_name("add_auth_entry");
                        remote_config_set_operation
                            (REMOTE_CONFIG_OPERATION_ADD);
                    }
                    else {
                        remote_config_set_command_name("mod_auth_entry");
                        remote_config_set_operation
                            (REMOTE_CONFIG_OPERATION_MODIFY);
                    }
                    remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);

                    /* Fail on duplicate */
                    (void) parse_authlist(&authlist_table_head);
                    authlistentrydup =
                        get_duplicate_authlist_entry(networkbuf,
                                                     netmaskbuf, auth_idx);
                    if (authlistentrydup == NULL) {
                        rc = add_authlist(idx, networkbuf,
                                          netmaskbuf, enable,
                                          maxbwbuf, minc, maxc,
                                          burstratiobuf, burstduration,
                                          unicast, multicast_init,
                                          multicast_part, preemptible);
                    }
                    else {
                        rc = RC_DUPLICATE;
                    }

                    if (rc == RC_NORMAL) {
                        if (idx == 0)
                            show_info(minfo,
                                      "Entry added. " PRESSANYKEY_STR);
                        else
                            show_info(minfo,
                                      "Entry updated. " PRESSANYKEY_STR);

                        save_authlist(&authlist_table_head);

                        remote_config_send();
                    }
                    else {
                        if (rc == RC_DUPLICATE) {
                            show_info(minfo,
                                      "Failed to add Entry: Duplicate. "
                                      PRESSANYKEY_STR);
                            remote_config_send_error(CLI_REASON_DUPLICATE);
                        }
                        else {
                            show_info(minfo, "Failed to add Entry. "
                                      PRESSANYKEY_STR);
                            remote_config_send_error
                                (CLI_REASON_SYSTEM_ERR);
                        }
                        retval = RC_ERROR;
                    }
                    get_anykey(minfo);
                    break;
                }
                else {
                    show_info(minfo, "Continue Editing?" YESNO_STR);
                    if (get_yesno(minfo)) {
                        /* continue */
                    }
                    else {
                        retval = RC_ERROR;
                        break;
                    }
                }
            }
            else {
                show_info(minfo, "No change in data. Quit?" YESNO_STR);
                if (get_yesno(minfo)) {
                    retval = RC_ERROR;
                    break;
                }

            }
        }

    }
    /************
     * Clean-up *
     ************/
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    return retval;


}

/****************************************************************
 * Find and return authlist entry matching given IP and netmask *
 ****************************************************************/
static struct authlist_entry *get_duplicate_authlist_entry(const char
                                                           *network,
                                                           const char
                                                           *netmask, 
                                                           int auth_index)
{

    struct list_head *tmp;
    struct authlist_entry *authlistentryp;
    int auth_id;

    if ((network == NULL) || (strlen(network) == 0))
        return NULL;

    list_for_each(tmp, &authlist_table_head) {
        authlistentryp =
            list_entry(tmp, struct authlist_entry, authlist_list);
        auth_id = authlistentryp->authid;
        if (auth_id != auth_index) {
            if (authlistentryp &&
                (strcmp(authlistentryp->network, network) == 0 &&
                 strcmp(authlistentryp->netmask, netmask) == 0)) {
                return authlistentryp;
            }
        }
    }

    return NULL;
}

/*****************************************
 * Inserts a new authlist at given index *
 *****************************************/
static int add_authlist(int idx,
                        const char *network,
                        const char *netmask,
                        int enable,
                        const char *maxbw,
                        int minc,
                        int maxc,
                        const char *burstratio,
                        int burstduration,
                        int unicast,
                        int multicast_init,
                        int multicast_part, int preemptible)
{

    struct authlist_entry *authlistentryp, authlistentry;

    if ((network == NULL) || (strlen(network) == 0))
        return RC_ERROR;

    memset(&authlistentry, 0x00, sizeof(authlistentry));
    strncpy(authlistentry.network, network, INET_ADDR_LEN);
    strncpy(authlistentry.netmask, netmask, INET_ADDR_LEN);
    strncpy(authlistentry.max_bandwidth, maxbw, BANDWIDTH_LEN);
    authlistentry.enable = enable;
    authlistentry.min_compression = minc;
    authlistentry.max_compression = maxc;
    authlistentry.burst_ratio = atof(burstratio);
    authlistentry.burst_duration = burstduration;
    authlistentry.unicast = unicast;
    authlistentry.multicast_init = multicast_init;
    authlistentry.multicast_part = multicast_part;
    authlistentry.unicast_preemptible = preemptible;

    if (idx == 0) {
        /*****************************
         * Add new entry to the list *
         *****************************/
        authlistentryp = malloc(sizeof(struct authlist_entry));
        if (authlistentryp != NULL) {
            memcpy(authlistentryp, &authlistentry,
                   sizeof(struct authlist_entry));
            authlistentryp->authid =
                get_unique_authid(&authlist_table_head);
            list_add_tail(&(authlistentryp->authlist_list),
                          &authlist_table_head);
        }
        else
            return RC_ERROR;
    }
    else {                      /* Modify existing entry */
        authlistentryp = get_authlist_entry_by_index(idx - 1);
        authlistentry.authlist_list = authlistentryp->authlist_list;    /* save this */
        authlistentry.authid = authlistentryp->authid;  /* save this */
        memcpy(authlistentryp, &authlistentry,
               sizeof(struct authlist_entry));
    }

    remote_config_set_index(authlistentryp->authid);
    remote_config_add_parameter(auth_options[0].name, network);
    remote_config_add_parameter(auth_options[1].name, netmask);
    remote_config_add_bool_parameter(auth_options[2].name, enable);
    remote_config_add_parameter(auth_options[3].name, maxbw);
    remote_config_add_parameter(auth_options[4].name, burstratio);
    remote_config_add_int_parameter(auth_options[5].name, burstduration);
    remote_config_add_int_parameter(auth_options[6].name, minc);
    remote_config_add_int_parameter(auth_options[7].name, maxc);
    remote_config_add_bool_parameter(auth_options[8].name, unicast);
    remote_config_add_bool_parameter(auth_options[9].name, preemptible);
    remote_config_add_bool_parameter(auth_options[10].name,
                                     multicast_init);
    remote_config_add_bool_parameter(auth_options[11].name,
                                     multicast_part);

    return RC_NORMAL;
}

static int delete_authlist(int idx, int *authidp)
{
    int authlistcount;
    struct authlist_entry *authlistentryp;

    authlistcount = count_authlist(&authlist_table_head);

    if (idx >= authlistcount)
        return RC_ERROR;

    authlistentryp = get_authlist_entry_by_index(idx);
    if (authlistentryp == NULL)
        return RC_ERROR;

    *authidp = authlistentryp->authid;
    list_del(&(authlistentryp->authlist_list));

    return RC_NORMAL;
}


int add_auth_entry_main(int argc, char **argv)
{
    int opt_index = 0;
    int c, rc = RC_NORMAL;
    char *networkbuf = NULL, *maskbuf = NULL;
    int enable = 1, minc = 2, maxc = 6, preempt = 0, unicast = 1;
    int burstduration = 50, multicastinit = 1, multicastjoin = 1;
    char *maxbwbuf, *burstratio;
    int value;
    float fvalue;
    struct authlist_entry *authlistentryp;

    remote_config_start();
    remote_config_set_config_name(auth_entry_string);
    remote_config_set_command_name("add_auth_entry");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    if (view_only() != 0) {
        print_cli_failure(add_auth_entry, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to add authorization entries\n");
        return RC_ERROR;
    }


    maxbwbuf = strdup("10.0M");
    burstratio = strdup("1.0");

    while (1) {
        c = getopt_long_only(argc, argv, "h", auth_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            if (opt_index == 0) {
                if (check_inetname(optarg) == RC_NORMAL)
                    networkbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 1) {
                if (check_netmask(optarg) == RC_NORMAL)
                    maskbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 2) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    enable = strcasecmp(optarg, "yes") ? 0 : 1;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 3) {
                if (check_regex(optarg, regexp_bandwidth) == RC_NORMAL)
                    maxbwbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 4) {
                fvalue = atof(optarg);
                if (fvalue >= 1.0 && fvalue <= 100.0) {
                    burstratio = optarg;
                }
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 5) {
                value = atoi(optarg);
                if (value >= 0 && value <= 10000)
                    burstduration = value;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 6) {
                value = atoi(optarg);
                if (value >= 0 && value <= 10)
                    minc = value;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 7) {
                value = atoi(optarg);
                if (value >= 0 && value <= 10)
                    maxc = value;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 8) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    unicast = strcasecmp(optarg, "yes") ? 0 : 1;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 9) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    preempt = strcasecmp(optarg, "yes") ? 0 : 1;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 10) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    multicastinit = strcasecmp(optarg, "yes") ? 0 : 1;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 11) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    multicastjoin = strcasecmp(optarg, "yes") ? 0 : 1;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 12) {
                remote_config_set_client_data(optarg);
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }
    if (optind == argc && rc == RC_NORMAL &&
        networkbuf != NULL && maskbuf != NULL) {

        /* Fail on duplicate */
        (void) parse_authlist(&authlist_table_head);
        authlistentryp = get_duplicate_authlist_entry(networkbuf, maskbuf, 0);
        if (authlistentryp != NULL) {
            print_cli_failure(add_auth_entry, CLI_REASON_DUPLICATE);
            remote_config_send_error(CLI_REASON_DUPLICATE);
            return RC_DUPLICATE;
        }

        rc = add_authlist(0, networkbuf,
                          maskbuf, enable,
                          maxbwbuf, minc, maxc,
                          burstratio, burstduration,
                          unicast, multicastinit, multicastjoin, preempt);
        if (rc == RC_NORMAL) {
            save_authlist(&authlist_table_head);

            print_cli_success(add_auth_entry);
            remote_config_send();
        }
        else {
            print_cli_failure(add_auth_entry, CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
        }
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(add_auth_entry, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(add_auth_entry, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\nUsage: add_auth_entry [OPTIONS]\n");
        printf("Add a authorization entry on V2D TX\n");
        printf("Options\n");
        printf("\t-ip Network\t\tSet IP address for entry to Network\n");
        printf("\t-mask Mask\t\tSet network mask for entry to Mask\n");
        printf
            ("\t-enable Yes/No\t\tSet enable rule for entry to Yes/No\n");
        printf
            ("\t-maxBandwidth Bandwidth\tSet max bandwidth to Bandwidth\n");
        printf("\t-burstRatio Value\tSet burst ratio to Value\n");
        printf("\t-burstDuration Value\tSet burst duration to Value\n");
        printf("\t-lowCompression Value\tSet low compression to Value\n");
        printf
            ("\t-highCompression Value\tSet high compression to Value\n");
        printf("\t-unicast Yes/No\t\tSet allow unicast to Yes or No\n");
        printf("\t-sysProfile Yes/No\t\tSet allow use system profile to Yes or No\n");
        printf
            ("\t-preempt Yes/No\t\tSet allow unicast preempt to Yes or No\n");
        printf
            ("\t-multicastInit Yes/No\tSet allow multicast init to Yes or No\n");
        printf
            ("\t-multicastJoin Yes/No\tSet allow multicast join to Yes or No\n");
    }
    return rc;
}

int del_auth_entry_main(int argc, char **argv)
{
    struct list_head *tmp;
    struct authlist_entry *authlistentryp;
    int auth_id, count = 0;
    int rc = RC_NORMAL;
    int c;
    int opt_index = 0;
    int found;
    int unused;

    remote_config_start();
    remote_config_set_config_name(auth_entry_string);
    remote_config_set_command_name("del_auth_entry");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_DELETE);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    if (view_only() != 0) {
        print_cli_failure(del_auth_entry, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to delete authorization entries\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", auth_delete_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            if (opt_index == 0) {
                remote_config_set_client_data(optarg);
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }

    if (rc == RC_NORMAL && optind == argc - 1) {
        auth_id = atoi(argv[optind]);
        remote_config_set_index(auth_id);
        (void) parse_authlist(&authlist_table_head);
        found = 0;
        list_for_each(tmp, &authlist_table_head) {
            authlistentryp = list_entry(tmp, struct authlist_entry,
                                        authlist_list);
            if (authlistentryp->authid == auth_id) {
                found = 1;
                if ((rc = delete_authlist(count, &unused)) == RC_NORMAL) {
                    save_authlist(&authlist_table_head);        /* write to file */

                }
                else {
                    print_cli_failure(add_auth_entry,
                                      CLI_REASON_SYSTEM_ERR);
                    remote_config_send_error(CLI_REASON_SYSTEM_ERR);
                    return rc;
                }
                break;
            }
            count++;
        }
        if (found == 1) {
            print_cli_success(del_auth_entry);
            remote_config_send();
        }
        else {
            print_cli_failure(add_auth_entry, CLI_REASON_BAD_INDEX);
            remote_config_send_error(CLI_REASON_BAD_INDEX);
            return RC_BAD_INDEX;
        }
    }
    else {
        print_cli_failure(del_auth_entry, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\nUsage: del_auth_entry AUTH_ID\n");
        printf("Delete the authorization entry with id AUTH_ID\n");
        return RC_ERROR;
    }

    return RC_NORMAL;
}

int mod_auth_entry_main(int argc, char **argv)
{
    int opt_index = 0;
    int c, rc = RC_NORMAL;
    char *networkbuf = NULL, *maskbuf = NULL;
    char *tmpnetworkbuf = NULL, *tmpmaskbuf = NULL;
    char *enablebuf = NULL, *maxbwbuf = NULL, *burstratiobuf = NULL;
    char *burstdurationbuf = NULL, *mincbuf = NULL;
    char *maxcbuf = NULL, *unicastbuf = NULL, *preemptbuf = NULL;
    char *multicastinitbuf = NULL, *multicastjoinbuf = NULL;
    int value;
    float fvalue;
    struct list_head *tmp;
    struct authlist_entry *authlistentryp;
    struct authlist_entry *authlistentrydup;
    int auth_id, count = 0;
    int found;


    remote_config_start();
    remote_config_set_config_name(auth_entry_string);
    remote_config_set_command_name("mod_auth_entry");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    if (view_only() != 0) {
        print_cli_failure(mod_auth_entry, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to modify authorization entries\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", auth_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            if (opt_index == 0) {
                if (check_inetname(optarg) == RC_NORMAL)
                    networkbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 1) {
                if (check_netmask(optarg) == RC_NORMAL)
                    maskbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 2) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    enablebuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 3) {
                if (check_regex(optarg, regexp_bandwidth) == RC_NORMAL)
                    maxbwbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 4) {
                fvalue = atof(optarg);
                if (fvalue >= 1.0 && fvalue <= 100.0) {
                    burstratiobuf = optarg;
                }
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 5) {
                value = atoi(optarg);
                if (value >= 0 && value <= 10000)
                    burstdurationbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 6) {
                value = atoi(optarg);
                if (value >= 0 && value <= 10)
                    mincbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 7) {
                value = atoi(optarg);
                if (value >= 0 && value <= 10)
                    maxcbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 8) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    unicastbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 9) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    preemptbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 10) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    multicastinitbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 11) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    multicastjoinbuf = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }


    if (rc == RC_NORMAL && optind == argc - 1) {

        auth_id = atoi(argv[optind]);
        remote_config_set_index(auth_id);

        (void) parse_authlist(&authlist_table_head);


        found = 0;
        list_for_each(tmp, &authlist_table_head) {
            authlistentryp = list_entry(tmp, struct authlist_entry,
                                        authlist_list);
            if (authlistentryp->authid == auth_id) {
                found = 1;
                /* Fail on duplicate */
                tmpnetworkbuf = networkbuf;
                tmpmaskbuf = maskbuf;
                if (tmpnetworkbuf == NULL)
                    tmpnetworkbuf = authlistentryp->network;
                if (tmpmaskbuf == NULL)
                    tmpmaskbuf = authlistentryp->netmask;

                authlistentrydup =
                    get_duplicate_authlist_entry(tmpnetworkbuf,
                                                 tmpmaskbuf, auth_id);
                if (authlistentrydup != NULL
                    && authlistentrydup->authid != auth_id) {
                    print_cli_failure(add_auth_entry,
                                      CLI_REASON_DUPLICATE);
                    remote_config_send_error(CLI_REASON_DUPLICATE);
                    return RC_INVALID_VALUE;
                }

                if (networkbuf)
                    strncpy(authlistentryp->network, networkbuf,
                            INET_ADDR_LEN);
                if (maskbuf)
                    strncpy(authlistentryp->netmask, maskbuf,
                            INET_ADDR_LEN);
                if (enablebuf)
                    authlistentryp->enable =
                        strcasecmp(enablebuf, "yes") ? 0 : 1;
                if (maxbwbuf)
                    strncpy(authlistentryp->max_bandwidth, maxbwbuf,
                            BANDWIDTH_LEN);
                if (mincbuf)
                    authlistentryp->min_compression = atoi(mincbuf);
                if (maxcbuf)
                    authlistentryp->max_compression = atoi(maxcbuf);
                if (burstratiobuf)
                    authlistentryp->burst_ratio = atof(burstratiobuf);
                if (burstdurationbuf)
                    authlistentryp->burst_duration =
                        atoi(burstdurationbuf);
                if (unicastbuf)
                    authlistentryp->unicast =
                        strcasecmp(unicastbuf, "yes") ? 0 : 1;
                if (preemptbuf)
                    authlistentryp->unicast_preemptible =
                        strcasecmp(preemptbuf, "yes") ? 0 : 1;
                if (multicastinitbuf)
                    authlistentryp->multicast_init =
                        strcasecmp(multicastinitbuf, "yes") ? 0 : 1;
                if (multicastjoinbuf)
                    authlistentryp->multicast_part =
                        strcasecmp(multicastjoinbuf, "yes") ? 0 : 1;


                remote_config_add_parameter(auth_options[0].name,
                                            authlistentryp->network);
                remote_config_add_parameter(auth_options[1].name,
                                            authlistentryp->netmask);
                remote_config_add_bool_parameter(auth_options[2].name,
                                                 authlistentryp->enable);
                remote_config_add_parameter(auth_options[3].name,
                                            authlistentryp->max_bandwidth);
                remote_config_add_float_parameter(auth_options[4].name,
                                                  authlistentryp->
                                                  burst_ratio);
                remote_config_add_int_parameter(auth_options[5].name,
                                                authlistentryp->
                                                burst_duration);
                remote_config_add_int_parameter(auth_options[6].name,
                                                authlistentryp->
                                                min_compression);
                remote_config_add_int_parameter(auth_options[7].name,
                                                authlistentryp->
                                                max_compression);
                remote_config_add_bool_parameter(auth_options[8].name,
                                                 authlistentryp->unicast);
                remote_config_add_bool_parameter(auth_options[9].name,
                                                 authlistentryp->
                                                 unicast_preemptible);
                remote_config_add_bool_parameter(auth_options[10].name,
                                                 authlistentryp->
                                                 multicast_init);
                remote_config_add_bool_parameter(auth_options[11].name,
                                                 authlistentryp->
                                                 multicast_part);

                break;
            }
            count++;
        }
        if (found == 1) {
            save_authlist(&authlist_table_head);        /* write to file */
            print_cli_success(mod_auth_entry);
            remote_config_send();
        }
        else {
            print_cli_failure(mod_auth_entry, CLI_REASON_BAD_INDEX);
            remote_config_send_error(CLI_REASON_BAD_INDEX);
            rc = RC_BAD_INDEX;
        }
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(mod_auth_entry, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(mod_auth_entry, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\nUsage: mod_auth_entry [OPTIONS] AUTH_ID\n");
        printf("Modify the authorization entry AUTH_ID on V2D TX\n");
        printf("Options\n");
        printf("\t-ip Network\t\tSet IP address for entry to Network\n");
        printf("\t-mask Mask\t\tSet network mask for entry to Mask\n");
        printf
            ("\t-enable Yes/No\t\tSet enable rule for entry to Yes/No\n");
        printf
            ("\t-maxBandwidth Bandwidth\tSet max bandwidth to Bandwidth\n");
        printf("\t-burstRatio Value\tSet burst ratio to Value\n");
        printf("\t-burstDuration Value\tSet burst duration to Value\n");
        printf("\t-lowCompression Value\tSet low compression to Value\n");
        printf
            ("\t-highCompression Value\tSet high compression to Value\n");
        printf("\t-unicast Yes/No\t\tSet allow unicast to Yes or No\n");
        printf
            ("\t-preempt Yes/No\t\tSet allow unicast preempt to Yes or No\n");
        printf
            ("\t-multicastInit Yes/No\tSet allow multicast init to Yes or No\n");
        printf
            ("\t-multicastJoin Yes/No\tSet allow multicast join to Yes or No\n");
    }

    return rc;
}

int describe_auth_table_main(int argc, char **argv)
{
    int rc = RC_NORMAL;
    printf ("<MenuDescription>\n");
    printf ("\t<Type>configlist</Type>\n");
    printf ("\t<Title>ConnectionAuthorization</Title>\n");
    printf ("\t<Description>Authorization Table</Description>\n");
    printf ("\t<BoardAccess>true</BoardAccess>\n");
    printf ("\t<CliCommandName>auth_table</CliCommandName>\n");
    printf ("\t<ConfigFile>/V2O/config/authlistconf</ConfigFile>\n");
    printf ("\t<MenuItems>\n");
    printf ("\t\t<Attribute name=\"ip\" label=\"Network\" type=\"string\" size=\"32\" validation=\"ipaddress\"/>\n");
    printf ("\t\t<Attribute name=\"mask\" label=\"Netmask\" type=\"string\" size=\"32\" validation=\"netmask\"/>\n");
    printf ("\t\t<Attribute name=\"enable\" label=\"Enable\" type=\"boolean\" size=\"4\" />\n");
    printf ("\t\t<Attribute name=\"maxBandwidth\" label=\"Bandwidth\" type=\"string\" size=\"10\" />\n");

    printf ("\t\t<Attribute name=\"lowCompression\" label=\"Low Compression\" type=\"integer\" size=\"5\" validation=\"range\" min=\"0\" max=\"10\" />\n");
    printf ("\t\t<Attribute name=\"highCompression\" label=\"High Compression\" type=\"integer\" size=\"5\" validation=\"range\" min=\"0\" max=\"10\" />\n");
    printf ("\t\t<Attribute name=\"burstRatio\" label=\"Burst Ratio\" type=\"float\" size=\"8\"  validation=\"range\" min=\"1.0\" max=\"100.0\"/>\n");
    printf ("\t\t<Attribute name=\"burstDuration\" label=\"Burst Duration\" type=\"integer\" size=\"5\"  validation=\"range\" min=\"1\" max=\"1000\" />\n");
    printf ("\t\t<Attribute name=\"unicast\" label=\"Unicast\" type=\"boolean\" size=\"4\" />\n");
    printf ("\t\t<Attribute name=\"preempt\" label=\"Preemptible\" type=\"boolean\" size=\"4\" />\n");
    printf ("\t\t<Attribute name=\"multicastInit\" label=\"Init Multicast\" type=\"boolean\" size=\"4\" />\n");
    printf ("\t\t<Attribute name=\"multicastJoin\" label=\"Join Multicast\" type=\"boolean\" size=\"4\" />\n");

    printf ("\t</MenuItems>\n");
    printf ("</MenuDescription>\n");
    return rc;
}
int get_auth_table_main(int argc, char **argv)
{
    int rc = RC_NORMAL;
    struct list_head *tmp;
    struct authlist_entry *authlistentryp;


    if (argc == 1) {

        (void) parse_authlist(&authlist_table_head);

        printf(XML_GROUP, get_auth_table);
        print_cli_success(NULL);
        printf("  " XML_TABLE, auth_table_string);

        list_for_each(tmp, &authlist_table_head) {
            authlistentryp =
                list_entry(tmp, struct authlist_entry, authlist_list);
            printf("    " XML_OBJINDEX, authlistentryp->authid,
                   auth_entry_string);
            printf("      " XML_ATTRSTR, auth_options[0].name,
                   authlistentryp->network);
            printf("      " XML_ATTRSTR, auth_options[1].name,
                   authlistentryp->netmask);
            printf("      " XML_ATTRSTR, auth_options[2].name,
                   authlistentryp->enable ? "Yes" : "No");
            printf("      " XML_ATTRSTR, auth_options[3].name,
                   authlistentryp->max_bandwidth);
            printf("      " XML_ATTRINT, auth_options[6].name,
                   authlistentryp->min_compression);
            printf("      " XML_ATTRINT, auth_options[7].name,
                   authlistentryp->max_compression);
            printf("      " XML_ATTRSTR, auth_options[8].name,
                   authlistentryp->unicast ? "Yes" : "No");
            printf("      " XML_ATTRSTR, auth_options[9].name,
                   authlistentryp->unicast_preemptible ? "Yes" : "No");
            printf("      " XML_ATTRSTR, auth_options[10].name,
                   authlistentryp->multicast_init ? "Yes" : "No");
            printf("      " XML_ATTRSTR, auth_options[11].name,
                   authlistentryp->multicast_part ? "Yes" : "No");
            printf("      " XML_ATTRFLOAT, auth_options[4].name,
                   authlistentryp->burst_ratio);
            printf("      " XML_ATTRINT, auth_options[5].name,
                   authlistentryp->burst_duration);
            printf("    " XML_OBJEND);
        }
        printf("  " XML_TABEND);
        printf(XML_GROUPEND);
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(get_auth_table, CLI_REASON_BAD_USAGE);
        printf("\nUsage: get_auth_table\n");
        printf("Get authorization table from V2D TX\n");
    }
    return rc;
}



int dump_auth_table()
{

    struct list_head *tmp;
    struct authlist_entry *authlistentryp;

    (void) parse_authlist(&authlist_table_head);

    list_for_each(tmp, &authlist_table_head) {

        authlistentryp =
            list_entry(tmp, struct authlist_entry, authlist_list);


        remote_config_start();
        remote_config_set_config_name(auth_entry_string);
        remote_config_set_command_name("add_auth_entry");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

        remote_config_set_index(authlistentryp->authid);
        remote_config_add_parameter(auth_options[0].name,
                                    authlistentryp->network);
        remote_config_add_parameter(auth_options[1].name,
                                    authlistentryp->netmask);
        remote_config_add_bool_parameter(auth_options[2].name,
                                         authlistentryp->enable);
        remote_config_add_parameter(auth_options[3].name,
                                    authlistentryp->max_bandwidth);
        remote_config_add_float_parameter(auth_options[4].name,
                                          authlistentryp->burst_ratio);
        remote_config_add_int_parameter(auth_options[5].name,
                                        authlistentryp->burst_duration);
        remote_config_add_int_parameter(auth_options[6].name,
                                        authlistentryp->min_compression);
        remote_config_add_int_parameter(auth_options[7].name,
                                        authlistentryp->max_compression);
        remote_config_add_bool_parameter(auth_options[8].name,
                                         authlistentryp->unicast);
        remote_config_add_bool_parameter(auth_options[9].name,
                                         authlistentryp->
                                         unicast_preemptible);
        remote_config_add_bool_parameter(auth_options[10].name,
                                         authlistentryp->multicast_init);
        remote_config_add_bool_parameter(auth_options[11].name,
                                         authlistentryp->multicast_part);

        remote_config_send();
    }

    return 0;
}
