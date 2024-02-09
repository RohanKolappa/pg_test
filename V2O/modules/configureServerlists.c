#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <getopt.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "commonutils.h"
#include "device_detection.h"
#include "validationroutines.h"
#include "configfile_parser.h"
#include "vlog.h"
#include "vutils.h"
#include "custmenu.h"
#include "menuutils.h"
#include "configureServerlists.h"
#include "loadnet.h"
#include "clientutils.h"
#include "rx_connection_status_menu_gen.h"
#include "standalone.h"
#include "text_utils.h"
#include "cli_utils.h"

#define PORT_MAX                65500
#define PORT_MIN                1024

static int board_number = 0;
static char *get_config_file_name(void)
{
    static char buf[128];
    sprintf(buf, "/V2O/config/clientconnconf_%d.txt", board_number);
    return buf;
}


static int add_serverlist_dialog(MENUINFO * minfo, int idx);
static int add_serverlist(int idx,
                          const char *servername,
                          const int serverport,
                          const char *bandwidth,
                          int multicast,
                          int avoption,
                          int bidir);
static int delete_serverlist(int idx);
static int setup_connection(int idx);

static char *serverlisthelp =
    "Press </U>A<!U> to Add Server, </U>D<!U> to Delete Server,\n"
    "or </U>Q<!U> to Quit. ";

static char *serverlisthelp1 =
    "Use </U>Up/Down<!U> arrow keys to select Server to connect,\n"
    "Press </U>Enter<!U> to accept current value, </U>Q<!U> or </U><CTRL-Q><!U> to quit.";


static struct list_head serverlist_table_head;


static struct serverlist_entry *get_serverlist_entry_by_index(int index)
{

    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, &serverlist_table_head) {
        if (count++ == index)
            return list_entry(tmp, struct serverlist_entry,
                              serverlist_list);

    }
    return NULL;
}

static char *serverlistselector_cb(int item)
{
    struct serverlist_entry *serverlistentryp;

    serverlistentryp = get_serverlist_entry_by_index(item);

    sprintf(tempbuf, "Entry not found");
    if (serverlistentryp != NULL
        && serverlistentryp->servername[0] != '\0') {
        if (strcmp(serverlistentryp->servername, "0.0.0.0") == 0)
            sprintf(tempbuf, "Disconnect.");
        else {
            char temp[50];

            strcpy(temp, serverlistentryp->multicast ? "M," : "U,");
            strcat(temp,
                   (serverlistentryp->avoption ==
                    AVOPTION_AUDIO_VIDEO) ? "Both" : ((serverlistentryp->
                                                        avoption ==
                                                        AVOPTION_AUDIO) ?
                                                       "Audio" :
                                                       "Video"));
            strcat(temp, serverlistentryp->bidir ? ",R" : "");

            sprintf(tempbuf, "%-16s %-5d %-8s   %s",
                    serverlistentryp->servername,
                    serverlistentryp->serverport,
                    serverlistentryp->bandwidth, temp);
        }
    }
    return tempbuf;
}

/* This menu has list_non_immediate kind of fields. */

int configureServerlists(MENUINFO * minfo)
{
    MENU *selector;
    DIALOG *dialog;
    char *help;
    int rc;
    int serverlistcount;
    char keylist[12];
    char titlebuf[120];
    char tbuf[64];
    int retval = RC_NORMAL;
    struct serverlist_entry *serverlistentryp;

    /********************
     * Print Menu title *
     ********************/
    get_title(minfo, tbuf);
    sprintf(titlebuf, "%s\n  \n</U>   %-16s %-5s %-8s  %s<!U>",
            tbuf,
            "Server Name", "Port", "Bandwidth", "Connection Type");
    show_title(minfo, titlebuf);

    // Get board number
    {
        HAL *hp;
        hp = minfo->minfo_userptr;
        if (hp != NULL) {
            board_number = hal_get_board_number(hp);
        }
    }

    /*********************************************************
     * Get current number of serverlists. Ask for serverlist *
     * addition if no serverlists yet.                       *
     *********************************************************/
    serverlistcount = parse_serverlist(&serverlist_table_head);

    if (serverlistcount == 0) {
        dialog = show_dialog(minfo, 8, 15,
                             "</K/U>Empty Server Connection Table!!<!K!U>");
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

        show_info(minfo, "Add new Connection Entry?" YESNO_STR);
        if (get_yesno(minfo)) {
            remove_dialog(dialog);
            rc = add_serverlist_dialog(minfo, 0);
            if (rc == RC_EXIT)
                return RC_EXIT;
            if (rc != RC_NORMAL)
                return retval;
            serverlistcount = count_serverlist(&serverlist_table_head);
            if (serverlistcount == 0)
                return retval;
            show_title(minfo, titlebuf);
        }
        else {
            remove_dialog(dialog);
            return retval;
        }
    }

    /************************************************************
     * We have at least one serverlist now. Create the selector *
     * widget.                                                  *
     ************************************************************/
    selector = create_selector(serverlistcount, serverlistselector_cb);

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
    help = (ruid == 0) ? serverlisthelp : serverlisthelp1;
    show_info(minfo, help);

    /***************************
     * Let user select an item *
     ***************************/
    while (1) {
        if (ruid == 0)
            strcpy(keylist, "dDaAcC");
        else
            strcpy(keylist, "cC");

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

        case 'd':
        case 'D':
            /******************************
             * Delete selected serverlist *
             ******************************/
            sprintf(tempbuf, "Delete Server at Index %d ?" YESNO_STR,
                    rc + 1);
            show_info(minfo, tempbuf);
            if (get_yesno(minfo)) {
                delete_serverlist(rc);
                serverlistcount = count_serverlist(&serverlist_table_head);
                save_serverlist(&serverlist_table_head); /* write out to file */
                if (serverlistcount == 0) {
                    show_info(minfo,
                              "Deleted last Server. " PRESSANYKEY_STR);
                    get_anykey(minfo);
                    goto QUIT_LABEL;
                }               /* else show updated serverlist list */
                unpost_menu(selector);
                refresh_selector(selector, serverlistcount,
                                 serverlistselector_cb);
                post_menu(selector);
            }
            show_info(minfo, help);
            break;

        case 'a':
        case 'A':
            /**********************
             * Add new serverlist *
             **********************/
            unpost_menu(selector);
            rc = add_serverlist_dialog(minfo, 0);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            if (rc == RC_NORMAL) {      /* new serverlist successfully added */
                serverlistcount = count_serverlist(&serverlist_table_head);
                refresh_selector(selector, serverlistcount,
                                 serverlistselector_cb);
            }
            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, help);
            break;

        case '\0':
        case 'c':
        case 'C':
            /**********************************
             * Connect to the selected server *
             **********************************/
            serverlistentryp = get_serverlist_entry_by_index(rc);
            if (serverlistentryp == NULL)
                break;

            sprintf(tempbuf, " ");
            show_info(minfo, tempbuf);

            if (ruid != 0 && setuid(0) == -1) {
                VLOG_WARNING("Cannot setuid to root. %m");
            }

            if (setup_connection(rc) == RC_ERROR) {
                unpost_menu(selector);
                show_info(minfo,
                          "Error: Check Standalone Mode.\n"
                          PRESSANYKEY_STR);
                get_anykey(minfo);
                show_title(minfo, titlebuf);
                post_menu(selector);
                show_info(minfo, help);
            }
            else {
                unpost_menu(selector);

                rc = displayRxConnectionStatus(minfo);
                if (rc == RC_EXIT) {
                    retval = RC_EXIT;
                    goto QUIT_LABEL;
                }

                show_title(minfo, titlebuf);
                post_menu(selector);
                show_info(minfo, help);

            }
            break;
        }
    }

  QUIT_LABEL:

    /************
     * Clean-up *
     ************/
    if (ruid != 0)
        setuid(ruid);
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);
    return retval;
}


#define FIELD_SERVERNAME        1
#define FIELD_BANDWIDTH         2
#define FIELD_MULTICAST         3
#define FIELD_AVOPTION          4
#define FIELD_BIDIR             7
#define FIELD_SERVERPORT        15

#define MULTICAST_LEN           3
#define AVOPTION_LEN            6
#define AUOPTION_LEN            7
#define FIELD_SERVERPORT_LEN    5

static char *avoptionlist[] =
    { "audio", "Audio", "video", "Video", "both", "Both", NULL };

static char *get_servername(char *buf, void *usrptr, int ident)
{
    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) usrptr;

    if (serverlistentryp == NULL)
        strcpy(buf, "");
    else
        sprintf(buf, "%s", serverlistentryp->servername);
    return buf;
}

static char *get_serverport(char *buf, void *usrptr, int ident)
{
    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) usrptr;

    if (serverlistentryp == NULL)
        sprintf(buf, "%d", SERVER_PORT);
    else
        sprintf(buf, "%d", serverlistentryp->serverport);
    return buf;
}

static char *get_bandwidth(char *buf, void *usrptr, int ident)
{
    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) usrptr;

    if (serverlistentryp == NULL)
        strcpy(buf, "10M");
    else
        sprintf(buf, "%s", serverlistentryp->bandwidth);
    return buf;
}

static char *get_multicast(char *buf, void *usrptr, int ident)
{
    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) usrptr;

    if (serverlistentryp == NULL)
        strcpy(buf, "No");
    else if (serverlistentryp->multicast > 0)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "No");
    return buf;
}

static char *get_avoption(char *buf, void *usrptr, int ident)
{
    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) usrptr;

    if (serverlistentryp == NULL)
        strcpy(buf, "Video");
    else if (serverlistentryp->avoption == AVOPTION_AUDIO_VIDEO)
        strcpy(buf, "Both");
    else if (serverlistentryp->avoption == AVOPTION_AUDIO)
        strcpy(buf, "Audio");
    else
        strcpy(buf, "Video");
    return buf;
}

static char *get_bidir(char *buf, void *usrptr, int ident)
{
    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) usrptr;

    if (serverlistentryp == NULL)
        strcpy(buf, "No");
    else if (serverlistentryp->bidir)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "No");
    return buf;
}


static FORMITEMDEF serverlistfitemdefs[] = {

    {FIELD_SERVERNAME, "Server Name",
     "Enter Server Name or IP address.\n"
     "This is the host name or IP address of the server to be connected to.",
     SERVERNAME_LEN, get_servername, check_inetname,
     FTYPE_NONE, 0, 0, O_NULLOK, 0},

    {FIELD_SERVERPORT, "Server Port",
     "Enter TCP port on Server to connect to.\n"
     "This is the host name or IP address of the server to be connected to.",
     FIELD_SERVERPORT_LEN, get_serverport, NULL,
     FTYPE_INTEGER, PORT_MIN, PORT_MAX, O_NULLOK, 0},

    {FIELD_BANDWIDTH, "Bandwidth",
     "Enter The desired bandwidth when connecting to this server."
     "This is the netmask for  the static serverlist to be added.",
     BANDWIDTH_LEN, get_bandwidth, NULL,
     FTYPE_REGEXP, (int) regexp_bandwidth, 0, O_NULLOK, 0},

    {FIELD_MULTICAST, "Multicast?",
     "Enter Yes if this you want to connect to Server in Multicast mode. Else enter No.\n",
     MULTICAST_LEN, get_multicast, NULL,
     FTYPE_ENUM, (int) yesnolist, 0, O_NULLOK, 0},

    {FIELD_AVOPTION, "Video/Audio",
     "Enter Video, Audio or Both for Video-only, Audio-only or Both.\n",
     AVOPTION_LEN, get_avoption, NULL,
     FTYPE_ENUM, (int) avoptionlist, 0, O_NULLOK, 0},

    {FIELD_BIDIR, "Reverse Audio",
     "Enter Yes if you want audio from RX to TX. Else enter No.\n",
     MULTICAST_LEN, get_bidir, NULL,
     FTYPE_ENUM, (int) yesnolist, 0, O_NULLOK, 0},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}

};

/*******************************************
 * Inserts a new serverlist at given index *
 *******************************************/
static int add_serverlist_dialog(MENUINFO * minfo, int idx)
{

    FORM *form = NULL;
    int rc = 0;
    char servernamebuf[SERVERNAME_LEN + 1];
    int serverport;
    char ctlportbuf[FIELD_SERVERPORT_LEN + 1];
    char bandwidthbuf[BANDWIDTH_LEN + 1];
    char multicastbuf[MULTICAST_LEN + 1];
    char avoptionbuf[AVOPTION_LEN + 1];
    char bidirbuf[MULTICAST_LEN + 1];
#ifdef XXX
    char auoptionbuf[AUOPTION_LEN + 1];
#endif
    char tmpbuf[INET_NAME_LEN + 1];
    int retval = RC_NORMAL;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, "Add New Connection Entry");

    /*******************
     * Create the form *
     *******************/
    form = create_form(serverlistfitemdefs, 1, 0, NULL);

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
    while (1) {
        rc = process_form(minfo, form, formmesg, 0);
        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            break;
        }
        if (rc == RC_QUIT) {
            show_info(minfo,
                      "Quit without adding Connection Entry?" YESNO_STR);
            if (get_yesno(minfo)) {
                retval = RC_ERROR;
                break;
            }
        }
        else {
            if (get_form_status(form) == TRUE) {
                show_info(minfo, "Add this Connection Entry?" YESNO_STR);
                if (get_yesno(minfo)) {
                    int is_multi = 0, is_bidir = 0, avoption =
                        DEFAULT_AVOPTION;
                    get_field_buffer(form, FIELD_SERVERNAME, tmpbuf);
                    if (isdigit(tmpbuf[0])) {
                        strncpy(servernamebuf, tmpbuf, SERVERNAME_LEN);
                    }
                    else {
                        show_info(minfo,
                                  "Converting to IP address. Please wait...");
                        print_ip_addr(servernamebuf, tmpbuf);
                    }
                    get_field_buffer(form, FIELD_SERVERPORT, ctlportbuf);
                    serverport = atoi(ctlportbuf);
                    get_field_buffer(form, FIELD_BANDWIDTH, bandwidthbuf);
                    get_field_buffer(form, FIELD_MULTICAST, multicastbuf);
                    if (strcasecmp(multicastbuf, "yes") == 0)
                        is_multi = 1;
                    get_field_buffer(form, FIELD_AVOPTION, avoptionbuf);
                    if (strcasecmp(avoptionbuf, "both") == 0)
                        avoption = AVOPTION_AUDIO_VIDEO;
                    else if (strcasecmp(avoptionbuf, "audio") == 0)
                        avoption = AVOPTION_AUDIO;
                    get_field_buffer(form, FIELD_BIDIR, bidirbuf);
                    if (strcasecmp(bidirbuf, "yes") == 0)
                        is_bidir = 1;

                    rc = add_serverlist(idx, servernamebuf, serverport,
                                        bandwidthbuf, is_multi, avoption,
                                        is_bidir);

                    if (rc == RC_NORMAL) {
                        show_info(minfo,
                                  "Connection Entry added. "
                                  PRESSANYKEY_STR);
                        save_serverlist(&serverlist_table_head);
                    }
                    else {
                        show_info(minfo,
                                  "Failed to add Connection Entry. "
                                  PRESSANYKEY_STR);
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


static int add_serverlist(int idx,
                          const char *servername,
                          int serverport,
                          const char *bandwidth,
                          int multicast,
                          int avoption,
                          int bidir)
{

    struct serverlist_entry *serverlistentryp, serverlistentry;

    if ((servername == NULL) || (strlen(servername) == 0))
        return RC_ERROR;

    /****************************************
     * Check for duplicate serverlists TODO *
     ****************************************/
    strncpy(serverlistentry.servername, servername, SERVERNAME_LEN);
    serverlistentry.servername[SERVERNAME_LEN] = 0;
    serverlistentry.serverport = serverport;
    strncpy(serverlistentry.bandwidth, bandwidth, BANDWIDTH_LEN);
    serverlistentry.bandwidth[BANDWIDTH_LEN] = 0;

    /****************************************
     * Allow max 40Mbps bandwidth for XP100 *
     ****************************************/
    if (get_hardware_profile() == HARDWARE_PROFILE_XP100) {
        if (bits_to_bytes(serverlistentry.bandwidth) > 5242880)
            return RC_ERROR;
    }

    serverlistentry.multicast = multicast;
    serverlistentry.avoption = avoption;
    serverlistentry.bidir = bidir;

    if (idx == 0) {
        serverlistentryp = malloc(sizeof(struct serverlist_entry));
        if (serverlistentryp != NULL) {
            memcpy(serverlistentryp, &serverlistentry,
                   sizeof(struct serverlist_entry));
            serverlistentryp->serverid =
                get_unique_serverid(&serverlist_table_head);
            list_add_tail(&(serverlistentryp->serverlist_list),
                          &serverlist_table_head);
        }
        else
            return RC_ERROR;
    }
    else {
        serverlistentryp = get_serverlist_entry_by_index(idx - 1);
        serverlistentry.serverlist_list = serverlistentryp->serverlist_list;    /* save this */
        memcpy(serverlistentryp, &serverlistentry,
               sizeof(struct serverlist_entry));
    }

    return RC_NORMAL;
}

static int delete_serverlist(int idx)
{
    int serverlistcount;
    struct serverlist_entry *serverlistentryp;

    serverlistcount = count_serverlist(&serverlist_table_head);

    if (idx >= serverlistcount)
        return RC_ERROR;

    serverlistentryp = get_serverlist_entry_by_index(idx);
    if (serverlistentryp == NULL)
        return RC_ERROR;


    list_del(&(serverlistentryp->serverlist_list));

    return RC_NORMAL;
}

static int client_serveraddress_handler(char *name, char *value, void *ptr,
                                        char flag)
{

    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) ptr;

    if (flag) {
        if (strcasecmp(name, S_SERVER_ADDRESS) == 0) {
            strcpy(value, serverlistentryp->servername);
            return 1;
        }
    }
    else {
        strcpy(name, S_SERVER_ADDRESS);
        strcpy(value, serverlistentryp->servername);
    }
    return 0;
}

static int client_serverport_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) ptr;

    if (flag) {
        if (strcasecmp(name, S_SERVER_PORT) == 0) {
            sprintf(value, "%d", serverlistentryp->serverport);
            return 1;
        }
    }
    else {
        strcpy(name, S_SERVER_PORT);
        sprintf(value, "%d", serverlistentryp->serverport);
    }

    return 0;
}

static int client_bandwidth_handler(char *name, char *value, void *ptr,
                                    char flag)
{

    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) ptr;

    if (flag) {
        if (strcasecmp(name, S_MAX_BW) == 0) {
            sprintf(value, "%d",
                    bits_to_bytes(serverlistentryp->bandwidth));
            return 1;
        }
    }
    else {
        strcpy(name, S_MAX_BW);
        sprintf(value, "%d", bits_to_bytes(serverlistentryp->bandwidth));
    }
    return 0;
}

static int client_multicast_handler(char *name, char *value, void *ptr,
                                    char flag)
{

    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) ptr;

    if (flag) {
        if (strcasecmp(name, S_MULTICAST) == 0) {
            sprintf(value, "%s",
                    serverlistentryp->multicast ? "Yes" : "No");
            return 1;
        }
    }
    else {
        strcpy(name, S_MULTICAST);
        sprintf(value, "%s", serverlistentryp->multicast ? "Yes" : "No");
    }
    return 0;
}

static int client_avoption_handler(char *name, char *value, void *ptr,
                                   char flag)
{

    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) ptr;

    if (flag) {
        if (strcasecmp(name, S_AVOPTION) == 0) {
            sprintf(value, "%s",
                    (serverlistentryp->avoption ==
                     AVOPTION_AUDIO_VIDEO) ? "Both" : ((serverlistentryp->
                                                        avoption ==
                                                        AVOPTION_AUDIO) ?
                                                       "Audio" : "Video"));
            return 1;
        }
    }
    else {
        strcpy(name, S_AVOPTION);
        sprintf(value, "%s",
                (serverlistentryp->avoption ==
                 AVOPTION_AUDIO_VIDEO) ? "Both" : ((serverlistentryp->
                                                    avoption ==
                                                    AVOPTION_AUDIO) ?
                                                   "Audio" : "Video"));
    }
    return 0;
}

static int client_bidir_handler(char *name, char *value, void *ptr,
                                char flag)
{

    struct serverlist_entry *serverlistentryp =
        (struct serverlist_entry *) ptr;

    if (flag) {
        if (strcasecmp(name, S_AUDIO_BIDIR) == 0) {
            sprintf(value, "%s", serverlistentryp->bidir ? "Yes" : "No");
            return 1;
        }
    }
    else {
        strcpy(name, S_AUDIO_BIDIR);
        sprintf(value, "%s", serverlistentryp->bidir ? "Yes" : "No");
    }
    return 0;
}


static int client_rfb_handler(char *name, char *value, void *ptr,
                                     char flag)
{

    if (flag) {
        if (strcasecmp(name, S_RFBCONNECTION) == 0) {
            sprintf(value, "No");
            return 1;
        }
    }
    else {
        strcpy(name, S_RFBCONNECTION);
        sprintf(value, "No");
    }
    return 0;
}

static int setup_connection(int idx)
{
    int serverlistcount;
    struct serverlist_entry *serverlistentryp;

    // Abort if not in standalone mode
    if (!is_standalone_enabled())
        return RC_ERROR;

    serverlistcount = count_serverlist(&serverlist_table_head);

    if (idx >= serverlistcount)
        return RC_ERROR;

    serverlistentryp = get_serverlist_entry_by_index(idx);

    if (serverlistentryp == NULL)
        return RC_ERROR;

    if (rewrite_config_file(get_config_file_name(),
                            client_serveraddress_handler,
                            serverlistentryp) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_serverport_handler,
                               serverlistentryp) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_multicast_handler,
                               serverlistentryp) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_bandwidth_handler,
                               serverlistentryp) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_avoption_handler,
                               serverlistentryp) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_bidir_handler,
                               serverlistentryp) < 0
        || rewrite_config_file(get_config_file_name(),
                               client_rfb_handler,
                               serverlistentryp) < 0 )
    {
        VLOG_WARNING("Could not rewrite configuration file %s",
                      get_config_file_name());
    }
    standalone_client_reconnect(board_number);
    return RC_NORMAL;
}


/***********************************
 * Command Line Interface Routines *
 ***********************************/

static char *add_conn_entry = "Add Connection Entry";
static char *del_conn_entry = "Delete Connection Entry";
static char *mod_conn_entry = "Modify Connection Entry";
static char *get_conn_table = "Get Connection Table";
static char *describe_conn_table = "Describe Connection Table";
static char *make_conn = "Make Connection";
static char *break_conn = "Break Connection";
static char *conn_table_string = "ConnectionTable";
static char *conn_entry_string = "ConnectionEntry";
static struct option conn_options[] = {
    {"ip", 1, 0, 0},
    {"port", 1, 0, 0},
    {"maxBW", 1, 0, 0},
    {"multicast", 1, 0, 0},
    {"avoption", 1, 0, 0},
    {"bidir", 1, 0, 0},
    {"channelNumber", 1, 0, 0},
    {0, 0, 0, 0}
};

int add_conn_entry_main(int argc, char **argv)
{
    int opt_index = 0;
    int c, rc = RC_NORMAL;
    char *networkbuf = NULL;
    int serverport = SERVER_PORT;
    char maxbwbuf[BANDWIDTH_LEN + 1];
    int multicast = 0;
    int avoption = DEFAULT_AVOPTION;
    int bidir = 0;

    if (view_only() != 0) {
        print_cli_failure(add_conn_entry, CLI_REASON_NO_PERMISSION);
        printf("You must be root to add connection entries\n");
        return RC_ERROR;
    }

    strcpy(maxbwbuf, "10.0M");
    while (1) {
        c = getopt_long_only(argc, argv, "h", conn_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            if (opt_index == 0) {
                if (check_inetname(optarg) == RC_NORMAL) {
                    networkbuf = optarg;
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
            else if (opt_index == 1) {
                int value = atoi(optarg);
                if (value <= PORT_MAX && value >= PORT_MIN) {
                    serverport = value;
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
            else if (opt_index == 2) {
                if (check_regex(optarg, regexp_bandwidth) == RC_NORMAL) {
                    strncpy(maxbwbuf, optarg, BANDWIDTH_LEN);
                    maxbwbuf[BANDWIDTH_LEN] = '\0';
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
            else if (opt_index == 3) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    multicast = (strcasecmp(optarg, "yes")) ? 0 : 1;
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
            else if (opt_index == 4) {
                if (check_enum(optarg, avoptionlist) == RC_NORMAL) {
                    if (strcasecmp(optarg, "both") == 0)
                        avoption = AVOPTION_AUDIO_VIDEO;
                    else if (strcasecmp(optarg, "audio") == 0)
                        avoption = AVOPTION_AUDIO;
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
            else if (opt_index == 5) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    bidir = (strcasecmp(optarg, "yes")) ? 0 : 1;
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }

    if (optind == argc && rc == RC_NORMAL && networkbuf != NULL) {
        (void) parse_serverlist(&serverlist_table_head);
        rc = add_serverlist(0, networkbuf, serverport, maxbwbuf,
                            multicast, avoption, bidir);
        if (rc == RC_NORMAL)
            save_serverlist(&serverlist_table_head);
        print_cli_success(add_conn_entry);
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(add_conn_entry, CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(add_conn_entry, CLI_REASON_BAD_USAGE);
        printf("\nUsage: add_conn_entry [OPTIONS]\n");
        printf("Add a connection manager entry on V2D RX\n");
        printf("Options\n");
        printf("\t-ip Address\t\t\tSet server IP address to Address\n");
        printf("\t-port Port\t\t\tSet server port to Port (%d-%d)\n",
               PORT_MIN, PORT_MAX);
        printf("\t-maxBW Bandwidth\t\t\tSet max bandwidth to Bandwidth\n");
        printf
            ("\t-multicast Yes|No\t\tSet allow multicast to Yes or No\n");
        printf("\t-avoption Value\t Set avoption to \"Value\"\n");
        printf("\t-bidir Yes|No\t\tSet bidirection audio to Yes or No\n");
    }
    return rc;
}

int del_conn_entry_main(int argc, char **argv)
{
    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;
    int server_id, count = 0;

    if (view_only() != 0) {
        print_cli_failure(del_conn_entry, CLI_REASON_NO_PERMISSION);
        printf("You must be root to add connection entries\n");
        return RC_ERROR;
    }

    if (argc == 2) {
        server_id = atoi(argv[1]);

        (void) parse_serverlist(&serverlist_table_head);
        list_for_each(tmp, &serverlist_table_head) {
            serverlistentryp = list_entry(tmp, struct serverlist_entry,
                                          serverlist_list);
            if (serverlistentryp->serverid == server_id) {
                if (delete_serverlist(count) == RC_NORMAL)
                    save_serverlist(&serverlist_table_head);    /* write to file */
                break;
            }
            count++;
        }
        print_cli_success(del_conn_entry);
    }
    else {
        print_cli_failure(del_conn_entry, CLI_REASON_BAD_USAGE);
        printf("\nUsage: del_conn_entry CONN_ID\n");
        printf("Delete the connection manager entry with id CONN_ID\n");
        return RC_ERROR;
    }
    return RC_NORMAL;
}

int mod_conn_entry_main(int argc, char **argv)
{
    int opt_index = 0;
    int c, rc = RC_NORMAL;
    char *network = NULL, *maxbw = NULL;
    int serverport = SERVER_PORT;
    int multicast = -1;
    int bidir = -1;
    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;
    int server_id, count = 0;
    int avoption = AVOPTION_VIDEO;

    if (view_only() != 0) {
        print_cli_failure(mod_conn_entry, CLI_REASON_NO_PERMISSION);
        printf("You must be root to modify connection entries\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", conn_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            if (opt_index == 0) {
                if (check_inetname(optarg) == RC_NORMAL)
                    network = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 1) {
                int value = atoi(optarg);
                if (value <= PORT_MAX && value >= PORT_MIN)
                    serverport = value;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 2) {
                if (check_regex(optarg, regexp_bandwidth) == RC_NORMAL)
                    maxbw = optarg;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 3) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    multicast = (strcasecmp(optarg, "yes")) ? 0 : 1;
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 4) {
                if (check_enum(optarg, avoptionlist) == RC_NORMAL) {
                    if (strcasecmp(optarg, "both") == 0)
                        avoption = AVOPTION_AUDIO_VIDEO;
                    else if (strcasecmp(optarg, "audio") == 0)
                        avoption = AVOPTION_AUDIO;
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
            else if (opt_index == 5) {
                if (check_enum(optarg, yesnolist) == RC_NORMAL)
                    bidir = (strcasecmp(optarg, "yes")) ? 0 : 1;
                else {
                    rc = RC_INVALID_VALUE;
                }
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }

    if (rc == RC_NORMAL && optind == argc - 1) {

        server_id = atoi(argv[optind]);
        (void) parse_serverlist(&serverlist_table_head);

        list_for_each(tmp, &serverlist_table_head) {
            serverlistentryp = list_entry(tmp, struct serverlist_entry,
                                          serverlist_list);
            if (serverlistentryp->serverid == server_id) {
                if (network)
                    strncpy(serverlistentryp->servername, network,
                            INET_ADDR_LEN);
                serverlistentryp->serverport = serverport;
                if (maxbw)
                    strncpy(serverlistentryp->bandwidth, maxbw,
                            BANDWIDTH_LEN);
                serverlistentryp->multicast = multicast;
                serverlistentryp->avoption = avoption;
                serverlistentryp->bidir = bidir;
                break;
            }
            count++;
        }
        save_serverlist(&serverlist_table_head);        /* write to file */
        print_cli_success(mod_conn_entry);
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(mod_conn_entry, CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(mod_conn_entry, CLI_REASON_BAD_USAGE);
        printf("\nUsage: mod_conn_entry [OPTIONS] CONN_ID\n");
        printf("Modify the connection manager entry CONN_ID on V2D TX\n");
        printf("Options\n");
        printf("\t-ip Address\t\t\tSet server IP address to Address\n");
        printf("\t-port Port\t\t\tSet server port to Port (%d-%d)\n",
               PORT_MIN, PORT_MAX);
        printf("\t-maxBW Bandwidth\tSet max bandwidth to Bandwidth\n");
        printf
            ("\t-multicast Yes/No\t\tSet allow multicast to Yes or No\n");
        printf("\t-avoption Value\t Set avoption to \"Value\"\n");
        printf
            ("\t-bidir Yes/No\t\tSet bidirectional audio to Yes or No\n");
    }
    return rc;
}

int describe_conn_table_main(int argc, char **argv)
{
    int rc = RC_NORMAL;
    printf ("<MenuDescription>\n");
    printf ("\t<Type>configlist</Type>\n");
    printf ("\t<Title>ConnectionManager</Title>\n");
    printf ("\t<Description>Connection Manager Table </Description>\n");
    printf ("\t<BoardAccess>true</BoardAccess>\n");
    printf ("\t<CliCommandName>conn_table</CliCommandName>\n");
    printf ("\t<ConfigFile>/V2O/config/serverlistconf</ConfigFile>\n");
    printf ("\t<MenuItems>\n");
    printf ("\t\t<Attribute name=\"ip\" label=\"Server\" type=\"string\" size=\"32\" validation=\"ipaddress\"/>\n");
    printf ("\t\t<Attribute name=\"port\" label=\"Port\" type=\"integer\" size=\"8\" validation=\"range\" min=\"0\" max=\"65535\"/>\n");
    printf ("\t\t<Attribute name=\"maxBW\" label=\"Bandwidth\" type=\"string\" size=\"10\" />\n");
    printf ("\t\t<Attribute name=\"multicast\" label=\"Multicast\" type=\"boolean\" size=\"4\" />\n");
    printf ("\t\t<Attribute name=\"avoption\" label=\"Audio/Video\" type=\"string\" size=\"5\" validation=\"enumeration\" values=\"Video , Audio , Both ,\"/>\n");
    printf ("\t\t<Attribute name=\"bidir\" label=\"Bidirectional\" type=\"boolean\" size=\"5\"  />\n");
    printf ("\t</MenuItems>\n");
    printf ("</MenuDescription>\n");
    return rc;
}
int get_conn_table_main(int argc, char **argv)
{
    int rc = RC_NORMAL;
    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;
    if (argc == 1) {

        (void) parse_serverlist(&serverlist_table_head);

        printf(XML_GROUP, get_conn_table);
        print_cli_success(NULL);
        printf("  " XML_TABLE, conn_table_string);

        list_for_each(tmp, &serverlist_table_head) {
            serverlistentryp = list_entry(tmp, struct serverlist_entry,
                                          serverlist_list);
            printf("    " XML_OBJINDEX, serverlistentryp->serverid,
                   conn_entry_string);
            printf("      " XML_ATTRSTR, conn_options[0].name,
                   serverlistentryp->servername);
            printf("      " XML_ATTRINT, conn_options[1].name,
                   serverlistentryp->serverport);
            printf("      " XML_ATTRSTR, conn_options[2].name,
                   serverlistentryp->bandwidth);
            printf("      " XML_ATTRSTR, conn_options[3].name,
                   serverlistentryp->multicast ? "Yes" : "No");
            printf("      " XML_ATTRSTR, conn_options[4].name,
                   ((serverlistentryp->avoption ==
                     AVOPTION_VIDEO) ? "Video" : ((serverlistentryp->
                                                   avoption ==
                                                   AVOPTION_AUDIO) ?
                                                  "Audio" : "Both")));
            printf("      " XML_ATTRSTR, conn_options[5].name,
                   serverlistentryp->bidir ? "Yes" : "No");
            printf("    " XML_OBJEND);
        }
        printf("  " XML_TABEND);
        printf(XML_GROUPEND);
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(get_conn_table, CLI_REASON_BAD_USAGE);
        printf("\nUsage: get_conn_table \n");
        printf("Get connection manager table from V2D RX\n");
    }
    return rc;
}

int make_conn_main(int argc, char **argv)
{
    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;
    int server_id, count = 0, rc = RC_NORMAL;
    int board_type;
    HAL *hp;

    int c, opt_index = 0;
    while (1) {
        c = getopt_long_only(argc, argv, "h", conn_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
             switch (opt_index) {
             case 6:
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
    if (hp == NULL) {
        print_cli_failure(make_conn, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }
    board_type = hal_get_board_type(hp);

    if (board_type == BOARDTYPE_TX) {
        print_cli_failure(make_conn, CLI_REASON_WRONG_HW);
        hal_release(hp);
        return RC_ERROR;
    }

    if (optind == argc - 1) {
        server_id = atoi(argv[optind]);

        (void) parse_serverlist(&serverlist_table_head);
        list_for_each(tmp, &serverlist_table_head) {
            serverlistentryp = list_entry(tmp, struct serverlist_entry,
                                          serverlist_list);
            if (serverlistentryp->serverid == server_id) {
                rc = setup_connection(count);
                break;
            }
            count++;
        }
        if (rc == RC_NORMAL)
            print_cli_success(make_conn);
        else
            print_cli_failure(make_conn, CLI_REASON_SYSTEM_ERR);
    }
    else {
        print_cli_failure(make_conn, CLI_REASON_BAD_USAGE);
        printf("\nUsage: make_conn -channelNumber Number CONN_ID\n");
        printf("Make the connection specified by CONN_ID\n");
        return RC_ERROR;
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}

int break_conn_main(int argc, char **argv)
{
    int rc = RC_NORMAL;
    struct serverlist_entry serverlistentry;
    int board_type;
    HAL *hp;

    int c, opt_index = 0;

    while (1) {
        c = getopt_long_only(argc, argv, "h", conn_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
             switch (opt_index) {
             case 6:
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
    if (hp == NULL) {
        print_cli_failure(break_conn, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }
    board_type = hal_get_board_type(hp);

    if (board_type == BOARDTYPE_TX) {
        print_cli_failure(break_conn, CLI_REASON_WRONG_HW);
        hal_release(hp);
        return RC_ERROR;
    }

    if (optind == argc) {
        strcpy(serverlistentry.servername, "0.0.0.0");
        strcpy(serverlistentry.bandwidth, "10.0M");
        serverlistentry.multicast = 0;

        if (rewrite_config_file(get_config_file_name(),
                                client_serveraddress_handler,
                                &serverlistentry) < 0
            || rewrite_config_file(get_config_file_name(),
                                   client_serverport_handler,
                                   &serverlistentry) < 0
            || rewrite_config_file(get_config_file_name(),
                                   client_multicast_handler,
                                   &serverlistentry) < 0
            || rewrite_config_file(get_config_file_name(),
                                   client_bandwidth_handler,
                                   &serverlistentry) < 0) {
            VLOG_WARNING("Could not rewrite configuration file %s",
                          get_config_file_name());
        }
        standalone_client_reconnect(board_number);
        print_cli_success(break_conn);
    }
    else {
        print_cli_failure(break_conn, CLI_REASON_BAD_USAGE);
        printf("\nUsage: break_conn -channelNumber Number\n");
        printf("Break the current connection (if any)\n");
        return RC_ERROR;
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}

