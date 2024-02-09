#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <getopt.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "routeList.h"
#include "vutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "validationroutines.h"
#include "vlog.h"
#include "remoteConfig.h"
#include "cli_utils.h"


static char *add_route_entry = "Add Routing Entry";
static char *del_route_entry = "Delete Routing Entry";
static char *mod_route_entry = "Modify Routing Entry";
static char *get_route_table = "Get Routing Table";
static char *route_table_string = "RoutingTable";
static char *route_entry_string = "RoutingEntry";
static struct option route_options[] = {
    {"dst", 1, 0, 0},
    {"mask", 1, 0, 0},
    {"gw", 1, 0, 0},
    {"clientData", 1, 0, 0},
    {0, 0, 0, 0}
};
static struct option route_delete_options[] = {
    {"clientData", 1, 0, 0},
    {0, 0, 0, 0}
};


/***************
 * Routes menu *
 ***************/

static int add_route_dialog(MENUINFO *minfo, int idx);
static int delete_route(int idx, int *routeidp);
static int add_route(int idx, 
                     const char *destination,
                     const char *netmask,
                     const char *gateway);

static char *routehelp = \
    "Press </U>A<!U> to Add route, </U>D<!U> to Delete route, </U>Q<!U> to Quit.";

static struct list_head route_table_head;

/****************************************************
 * Find and return route entry matching given index *
 ****************************************************/
static struct route_entry *get_route_entry_by_index(int index) {

    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, &route_table_head) {
        if (count++ == index)
            return list_entry(tmp, struct route_entry, route_list);

    }
    return NULL;
}

static char * routeselector_cb(int item) {
    struct route_entry *routeentryp;

    routeentryp = get_route_entry_by_index(item);

    sprintf(tempbuf, "Entry not found");
    if (routeentryp != NULL && routeentryp->gateway[0] != '\0') {
        sprintf(tempbuf, "%-16s %-16s %-16s", 
                routeentryp->destination, 
                routeentryp->gateway, 
                routeentryp->netmask);
    }
    return tempbuf;
}


/* This menu has list_non_immediate kind of fields.  */
int configureRoutes(MENUINFO *minfo) {
    MENU                *selector;
    DIALOG              *dialog;
    char                *help;
    int                 rc;
    int                 routecount;
    char                keylist[12];
    char                titlebuf[120];
    int                 retval = RC_NORMAL;
    int                 routeid;

    /********************
     * Print Menu title *
     ********************/
    sprintf(titlebuf, "%s\n  \n</U>    %-16s %-16s %-16s<!U>",
            minfo->minfo_title,
            "Destination", "Gateway", "Netmask");
    show_title(minfo, titlebuf);

    /**************************************************************
     * Get current number of routes. Ask for route addition if no *
     * routes yet.                                                *
     **************************************************************/
    routecount = parse_routes(&route_table_head, NULL);

    if (routecount == 0) {
        dialog = show_dialog(minfo, 8, 15, 
                             "</K/U>Empty Route Table!!<!K!U>");

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

        show_info(minfo, "Add new static route?" YESNO_STR);
        if (get_yesno(minfo)) {
            remove_dialog(dialog);
            rc = add_route_dialog(minfo, 0);
            if (rc == RC_EXIT)
                return RC_EXIT;
            if (rc != RC_NORMAL)
                return retval;
            routecount = count_routes(&route_table_head);
            if (routecount == 0)
                return retval;
            show_title(minfo, titlebuf);
        }
        else {
            remove_dialog(dialog);
            free_route(&route_table_head);
            return retval;
        }
    }

    /**************************************************************
     * We have at least one route now. Create the selector widget *
     **************************************************************/
    selector = create_selector(routecount, routeselector_cb);

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
    help = (ruid == 0) ? routehelp : formmesg2;
    show_info(minfo, help);

    /***************************
     * Let user select an item *
     ***************************/
    while (1) {
        strcpy(keylist, "dDaA");
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
         * Don't process requests from non-root users * 
         **********************************************/
        if (ruid != 0)
            continue;

        /*******************************
         * Process custom key requests *
         *******************************/
        switch (keylist[0]) {

        case '\0':
            show_info(minfo, help);
            break;

        case 'd':
        case 'D':
            /* Delete selected route */
            sprintf(tempbuf, "Delete Route at Index %d ?" YESNO_STR, rc+1);
            show_info(minfo, tempbuf);
            if (get_yesno(minfo)) {

                remote_config_start();
                remote_config_set_config_name(route_entry_string);
                remote_config_set_command_name("del_route_entry");
                remote_config_set_operation(REMOTE_CONFIG_OPERATION_DELETE);
                remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);

                delete_route(rc, &routeid);

                remote_config_set_index(routeid);
                remote_config_send();

                routecount = count_routes(&route_table_head);
                save_routes(&route_table_head);  /* write out to file */
                if (routecount == 0) {
                    show_info(minfo, "Deleted last route. " PRESSANYKEY_STR);
                    get_anykey(minfo);
                    goto QUIT_LABEL;
                } /* else show updated route list */
                unpost_menu(selector);
                refresh_selector(selector, routecount, routeselector_cb); 
                post_menu(selector);
            }
            show_info(minfo, help);
            break;

        case 'a':
        case 'A':
            /*****************
             * Add new route *
             *****************/
            unpost_menu(selector);
            rc = add_route_dialog(minfo, rc);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            if (rc == RC_NORMAL) {  /* new route successfully added */
                routecount = count_routes(&route_table_head);
                refresh_selector(selector, routecount, routeselector_cb);
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
    free_route(&route_table_head);
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);
    return retval;
}


static char *get_default_mask(char *buf, void *usrptr, int ident) {
    strcpy(buf, "255.255.255.0");
    return buf;
}

#define FIELD_DESTINATION   1
#define FIELD_NETMASK           2
#define FIELD_GATEWAY           3

static FORMITEMDEF routefitemdefs[] = {

    {FIELD_DESTINATION, "Destination", 
     "Enter destination in dotted notation (e.g. 192.168.1.1).\n" 
     "This is the destination network of the static route to be added.",
     INET_ADDR_LEN, NULL, check_ip, FTYPE_IPV4, 0, 0, 
     O_NULLOK, 0}, 
    {FIELD_NETMASK, "Netmask", 
     "Enter Netmask in dotted notation (Example: 255.255.255.0)."
     "This is the netmask for  the static route to be added.",
     INET_ADDR_LEN, get_default_mask, check_netmask, FTYPE_IPV4, 0, 0, 
     O_NULLOK, 0},
    {FIELD_GATEWAY, "Gateway", 
     "Enter the gateway name or IP address in dotted notation.\n"
     "This is the gateway for  the static route to be added.",
     INET_NAME_LEN, NULL, check_inetname, FTYPE_NONE, 0, 0, 
     O_NULLOK, 0}, 
    {0, NULL, NULL, 0, NULL, NULL, 0, 0, 0, 0, 0}
};

/**************************************
 * Inserts a new route at given index *
 **************************************/
static int add_route_dialog(MENUINFO *minfo, int idx) {

    FORM                *form = NULL;
    int                 rc = 0;
    char                destinationbuf[INET_ADDR_LEN+1];
    char                netmaskbuf[INET_ADDR_LEN+1];
    char                gatewaybuf[INET_ADDR_LEN+1];
    char                tmpbuf[INET_NAME_LEN+1];
    int                 retval = RC_NORMAL;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, "Add New Route");


    /*******************
     * Create the form *
     *******************/
    form = create_form(routefitemdefs, 3, 0, NULL);

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
            show_info(minfo, "Quit without adding new static route?" YESNO_STR);
            if (get_yesno(minfo)) {
                retval = RC_ERROR;
                break;
            }
        }
        else {
            if (get_form_status(form) == TRUE) {
                show_info(minfo, "Add Static Route?" YESNO_STR);
                if (get_yesno(minfo)) {

                    get_field_buffer(form, FIELD_DESTINATION, destinationbuf);
                    get_field_buffer(form, FIELD_NETMASK, netmaskbuf);
                    get_field_buffer(form, FIELD_GATEWAY, tmpbuf);
                    if (isdigit(tmpbuf[0])) {
                        strncpy(gatewaybuf, tmpbuf, INET_ADDR_LEN);
                        gatewaybuf[INET_ADDR_LEN] = '\0';
                    }
                    else {
                        show_info(minfo, 
                                  "Resolving name to IP address. Please wait...");
                        print_ip_addr(gatewaybuf, tmpbuf);
                    }

                    remote_config_start();
                    remote_config_set_config_name(route_entry_string);
                    remote_config_set_command_name("add_route_entry");
                    remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
                    remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);
                    
                    rc = add_route(idx, destinationbuf, netmaskbuf, gatewaybuf);

                    if (rc == RC_NORMAL) {
                        show_info(minfo, "Static route added. "
                                  PRESSANYKEY_STR);
                        save_routes(&route_table_head);
                        remote_config_send();
                    }
                    else if (rc == RC_DUPLICATE) {
                        show_info(minfo, "Failed to add route: Duplicate. "
                                  PRESSANYKEY_STR);
                        remote_config_send_error(CLI_REASON_DUPLICATE);
                        retval = RC_ERROR;
                    }
                    else {
                        show_info(minfo, "Failed to add route. "
                                  PRESSANYKEY_STR);
                        remote_config_send_error(CLI_REASON_SYSTEM_ERR);
                        retval = RC_ERROR;
                    }
                    get_anykey(minfo);
                    break;
                }
                else {
                    show_info(minfo, "Continue Editing?" 
                              YESNO_STR);
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

/*************
 * Add route *
 *************/
static int add_route(int idx, 
                     const char *destination,
                     const char *netmask,
                     const char *gateway) {
    
    struct route_entry  *routeentryp, routeentry;
    
    if ((destination == NULL) || (strlen(destination) == 0) ||
        (netmask == NULL) || (strlen(netmask) == 0) ||
        (gateway == NULL) || (strlen(gateway) == 0))
        return RC_ERROR;

    /******************************
     * Check for duplicate routes *
     ******************************/
    strncpy(routeentry.destination, destination, INET_ADDR_LEN);
    strncpy(routeentry.netmask, netmask, INET_ADDR_LEN);
    strncpy(routeentry.gateway, gateway, INET_ADDR_LEN);
    if (check_route(&routeentry, &route_table_head, -1) < 0)
        return RC_DUPLICATE;
    
    /*************************************************************
     * Add route to OS. Refuse to add route if the OS rejects it *
     *************************************************************/
    if (apply_route(destination, netmask, gateway) < 0)
        return RC_ERROR;


    routeentryp = malloc(sizeof(struct route_entry));
    if (routeentryp != NULL) {
        memcpy(routeentryp, &routeentry, sizeof(struct route_entry));
        routeentryp->routeid = get_unique_routeid(&route_table_head);
        list_add_tail(&(routeentryp->route_list), &route_table_head);
    }
    else {
        return RC_ERROR;
    }

    remote_config_set_index(routeentryp->routeid);
    remote_config_add_parameter(route_options[0].name, destination);
    remote_config_add_parameter(route_options[1].name, netmask);
    remote_config_add_parameter(route_options[2].name, gateway);

    return RC_NORMAL;
}

static int delete_route(int idx, int *routeidp) {
    int routecount;
    struct route_entry *routeentryp;

    routecount = count_routes(&route_table_head);
    
    if (idx >= routecount)
        return RC_ERROR;

    routeentryp = get_route_entry_by_index(idx);
    if (routeentryp == NULL)
        return RC_ERROR;

    *routeidp = routeentryp->routeid;

    /***************************************************
     * Remove route from the OS. Ignore return value.  *
     ***************************************************/
    remove_route(routeentryp->destination, 
                 routeentryp->netmask, 
                 routeentryp->gateway);

    list_del(&(routeentryp->route_list));

    return RC_NORMAL;
}


int add_route_entry_main(int argc, char **argv) {
    int opt_index = 0;
    int c, rc = RC_NORMAL;
    char *dstbuf = NULL, *maskbuf = NULL, *gwbuf = NULL;

    remote_config_start();
    remote_config_set_config_name(route_entry_string);
    remote_config_set_command_name("add_route_entry");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    if (view_only() != 0) {
        print_cli_failure(add_route_entry, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to add route entries\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", route_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            if (opt_index == 0) {
                if (check_inetname(optarg) == RC_NORMAL) {
                    dstbuf = optarg;  
                }
                else 
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 1) {
                if (check_netmask(optarg) == RC_NORMAL) {
                    maskbuf = optarg;  
                }
                else 
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 2) {
                if (check_inetname(optarg) == RC_NORMAL) {
                    gwbuf = optarg;  
                }
                else 
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 3) {
                remote_config_set_client_data(optarg);
            }
        }
        else {
            rc = RC_ERROR;
            break;        
        }
    }

    if (optind == argc && rc == RC_NORMAL && 
        dstbuf != NULL && maskbuf != NULL && gwbuf != NULL) {
        (void) parse_routes(&route_table_head, NULL);
        rc = add_route(0, dstbuf, maskbuf, gwbuf); 
        if (rc == RC_NORMAL) {
            save_routes(&route_table_head); 
            print_cli_success(add_route_entry);
            remote_config_send();
        }
        else if (rc == RC_DUPLICATE) {
            print_cli_failure(add_route_entry, CLI_REASON_DUPLICATE);
            remote_config_send_error(CLI_REASON_DUPLICATE);
        } 
        else {
            print_cli_failure(add_route_entry, CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
            rc = RC_ERROR;
        }
        free_route(&route_table_head);
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(add_route_entry, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(add_route_entry, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\nUsage: add_route_entry OPTIONS\n");
        printf("Add a routing entry on V2D\n");
        printf("Options\n");
        printf("\t-dst IpAddr\tSet destination IP for entry to IpAddr\n");
        printf("\t-mask Mask\tSet network mask to Mask\n");
        printf("\t-gw IpAddr\tSet gateway IP to IpAddr\n");
    }

    return rc;
}

int del_route_entry_main(int argc, char **argv) {
    struct list_head *tmp;
    struct route_entry *routeentryp;
    int route_id, count = 0;
    int found;
    int c;
    int opt_index = 0;
    int rc = RC_NORMAL;
    int unused;

    remote_config_start();
    remote_config_set_config_name(route_entry_string);
    remote_config_set_command_name("del_route_entry");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_DELETE);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    if (view_only() != 0) {
        print_cli_failure(del_route_entry, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to delete route entries\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", route_delete_options, &opt_index);
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
        route_id = atoi(argv[optind]);
        remote_config_set_index(route_id);
        (void) parse_routes(&route_table_head, NULL);
        found = 0;
        list_for_each(tmp, &route_table_head) {
            routeentryp = list_entry(tmp, struct route_entry, route_list);
            if (routeentryp->routeid == route_id) {
                found = 1;
                if (delete_route(count, &unused) == RC_NORMAL) {
                    save_routes(&route_table_head);  /* write to file */
                    print_cli_success(del_route_entry);
                    remote_config_send();
                }
                else {
                    print_cli_failure(del_route_entry, CLI_REASON_SYSTEM_ERR);
                    remote_config_send_error(CLI_REASON_SYSTEM_ERR);
                    return RC_ERROR;
                }
                break;
            }
            count++;
        }
        if (found == 0) {
            print_cli_failure(del_route_entry, CLI_REASON_BAD_INDEX);
            remote_config_send_error(CLI_REASON_BAD_INDEX);
            return RC_BAD_INDEX;
        }
        free_route(&route_table_head);
    } 
    else {
        print_cli_failure(del_route_entry, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\nUsage: del_route_entry ROUTE_ID\n");
        printf("Delete the routing entry with id ROUTE_ID\n");
        return RC_ERROR;
    }
    
    return RC_NORMAL; 
}

int mod_route_entry_main(int argc, char **argv) {
    int opt_index = 0;
    int c, rc = RC_NORMAL;
    char *dstbuf = NULL, *maskbuf = NULL, *gwbuf = NULL;
    struct list_head *tmp;
    struct route_entry *routeentryp, routeentry;
    int route_id, count = 0;
    int found;

    remote_config_start();
    remote_config_set_config_name(route_entry_string);
    remote_config_set_command_name("mod_route_entry");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    if (view_only() != 0) {
        print_cli_failure(mod_route_entry, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to modify route entries\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", route_options, &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            if (opt_index == 0) {
                if (check_inetname(optarg) == RC_NORMAL) 
                    dstbuf = optarg;  
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
                if (check_inetname(optarg) == RC_NORMAL) 
                    gwbuf = optarg;  
                else
                    rc = RC_INVALID_VALUE;
            }
            else if (opt_index == 3) {
                remote_config_set_client_data(optarg);
            }
        }
        else {
            rc = RC_ERROR;
            break;        
        }
    }

    if (rc == RC_NORMAL && optind == argc - 1) {

        route_id = atoi(argv[optind]);
        remote_config_set_index(route_id);
        (void) parse_routes(&route_table_head, NULL);
        found = 0;
        list_for_each(tmp, &route_table_head) {
            routeentryp = list_entry(tmp, struct route_entry, route_list);  
            if (routeentryp->routeid == route_id) {
                /* Is this a duplicate */
                if (dstbuf)
                    strncpy(routeentry.destination, dstbuf, INET_ADDR_LEN);
                else
                    strncpy(routeentry.destination, routeentryp->destination, INET_ADDR_LEN);
                if (maskbuf)
                    strncpy(routeentry.netmask, maskbuf, INET_ADDR_LEN);
                else
                    strncpy(routeentry.netmask, routeentryp->netmask, INET_ADDR_LEN);
                if (gwbuf)
                    strncpy(routeentry.gateway, gwbuf, INET_ADDR_LEN);
                else
                    strncpy(routeentry.gateway, routeentryp->gateway, INET_ADDR_LEN);

                if (check_route(&routeentry, &route_table_head, route_id) < 0) {
                    print_cli_failure(mod_route_entry, CLI_REASON_DUPLICATE);
                    remote_config_send_error(CLI_REASON_DUPLICATE);
                    return RC_DUPLICATE;
                }
                
                /* Does the system accept this?  Remove old route and
                 * add the new route. If new route addition fails, put
                 * back the old route */
                if (remove_route(routeentryp->destination, 
                                 routeentryp->netmask, routeentryp->gateway) < 0) {
                    /* Ignore failure */
                }
                if (apply_route(routeentry.destination, 
                                routeentry.netmask, routeentry.gateway) < 0) {
                    /* Route add to OS failed, apply old route back */
                    apply_route(routeentryp->destination, 
                                routeentryp->netmask, routeentryp->gateway);
                    
                    print_cli_failure(mod_route_entry, CLI_REASON_SYSTEM_ERR);
                    remote_config_send_error(CLI_REASON_SYSTEM_ERR);
                    return RC_ERROR;
                }

                /* Ok to modify */
                found = 1;

                if (dstbuf) 
                    strncpy(routeentryp->destination, dstbuf, INET_ADDR_LEN);
                if (maskbuf) 
                    strncpy(routeentryp->netmask, maskbuf, INET_ADDR_LEN);
                if (gwbuf) 
                    strncpy(routeentryp->gateway, gwbuf, INET_ADDR_LEN);


                remote_config_add_parameter(route_options[0].name, 
                                            routeentryp->destination);
                remote_config_add_parameter(route_options[1].name, 
                                            routeentryp->netmask);
                remote_config_add_parameter(route_options[2].name, 
                                            routeentryp->gateway);

                break;
            }
            count++;
        }
        if (found == 1) {
            save_routes(&route_table_head);  /* write to file */
            print_cli_success(mod_route_entry);
            remote_config_send();
        }
        else {
            print_cli_failure(mod_route_entry, CLI_REASON_BAD_INDEX);
            remote_config_send_error(CLI_REASON_BAD_INDEX);
            rc = RC_BAD_INDEX;
        }
        free_route(&route_table_head);
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(mod_route_entry, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(mod_route_entry, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\nUsage: mod_rotue_entry [OPTIONS] ROUTE_ID\n");
        printf("Modify the routing entry ROUTE_ID on V2D\n");
        printf("Options\n");
        printf("\t-dst IpAddr\tSet destination IP for entry to IpAddr\n");
        printf("\t-mask Mask\tSet network mask to Mask\n");
        printf("\t-gw IpAddr\tSet gateway IP to IpAddr\n");
    }

    return rc;
}

int describe_route_table_main(int argc, char **argv) {
    int rc = RC_NORMAL;
    printf ("<MenuDescription>\n");
    printf ("\t<Type>configlist</Type>\n");
    printf ("\t<Title>RoutingTable</Title>\n");
    printf ("\t<Description>Static Routes </Description>\n");
    printf ("\t<BoardAccess>true</BoardAccess>\n");
    printf ("\t<CliCommandName>route_table</CliCommandName>\n");
    printf ("\t<MenuItems>\n");
    printf ("\t\t<Attribute name=\"dst\" label=\"Destination\" type=\"string\" size=\"32\" validation=\"ipaddress\"/>\n");
    printf ("\t\t<Attribute name=\"mask\" label=\"Netmask\" type=\"string\" size=\"32\" validation=\"netmask\"/>\n");
    printf ("\t\t<Attribute name=\"gw\" label=\"Gateway\" type=\"string\" size=\"32\" validation=\"ipaddress\"/>\n");
    printf ("\t</MenuItems>\n");
    printf ("</MenuDescription>\n");
    return rc;
}
int get_route_table_main(int argc, char **argv) {
    int rc = RC_NORMAL;
    struct list_head *tmp;
    struct route_entry *routeentryp;

    if (argc == 1) {

        (void) parse_routes(&route_table_head, NULL);

        printf(XML_GROUP, get_route_table);
        print_cli_success(NULL);
        printf("  " XML_TABLE, route_table_string);

        list_for_each(tmp, &route_table_head) {
            routeentryp = list_entry(tmp, struct route_entry, route_list);
            printf("    " XML_OBJINDEX, routeentryp->routeid, route_entry_string);
            printf("      " XML_ATTRSTR, route_options[0].name, routeentryp->destination);
            printf("      " XML_ATTRSTR, route_options[1].name, routeentryp->netmask);
            printf("      " XML_ATTRSTR, route_options[2].name, routeentryp->gateway);
            printf("    " XML_OBJEND);
        } 
        printf("  " XML_TABEND);
        printf(XML_GROUPEND);
        free_route(&route_table_head);
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(get_route_table, CLI_REASON_BAD_USAGE);
        printf("\nUsage: get_route_table\n");
        printf("Get routing table from V2D\n");
    }
    return rc;
}


int dump_route_table() {

    struct list_head *tmp;
    struct route_entry *routeentryp;

    
    (void) parse_routes(&route_table_head, NULL);
    
    list_for_each(tmp, &route_table_head) {

        routeentryp = list_entry(tmp, struct route_entry, route_list);

        remote_config_start();
        remote_config_set_config_name(route_entry_string);
        remote_config_set_command_name("add_route_entry");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);
        
        remote_config_set_index(routeentryp->routeid);
        remote_config_add_parameter(route_options[0].name, 
                                    routeentryp->destination);
        remote_config_add_parameter(route_options[1].name, 
                                    routeentryp->netmask);
        remote_config_add_parameter(route_options[2].name, 
                                    routeentryp->gateway);

        remote_config_send();
    } 


    return 0;
}
