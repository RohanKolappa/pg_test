

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "portSelector.h"

static char *portselectorhelp = "Select port for this operation.\n";

struct portselector {
    int port_number;
    char display_string[64];
};

static struct portselector *portselector_array;

static char *portselector_cb(int item)
{
    sprintf(tempbuf, "%s", portselector_array[item].display_string);
    return tempbuf;
}

/**********************************
 * Enumerate all available boards *
 **********************************/
static int enumerate_ports(unsigned long menuitem_flags)
{
    HAL *hp;
    int numb_ports;
    int numb_items;
    int port;
    char *port_name;

//#define PSEUDO_BOARDS

#ifdef PSEUDO_BOARDS
    numb_ports = 6;
    numb_items = 0;
    hp = NULL;
    portselector_array = malloc(sizeof(struct portselector) * numb_ports);

    for (port = 0; port < numb_ports; port++) {
#if 0
        if (rand() % 2 == 0) {    /* Set random port to be Tx */
#else
        if (port % 2 == 0) {    /* Set every alternate port to be Tx */
#endif
            if (!(menuitem_flags & MFLAG_PORTTYPE_TX))
                continue;
            port_name = hal_utils_port_name(BOARDTYPE_TX);
        }
        else {
            if (!(menuitem_flags & MFLAG_PORTTYPE_RX))
                continue;
            port_name = hal_utils_port_name(BOARDTYPE_RX);
        }
        portselector_array[numb_items].port_number = port;
        sprintf(portselector_array[numb_items].display_string,
                "Channel %d [%s]", port + 1, port_name);
        numb_items++;
    }


#else
    hp = hal_acquire(0);
    numb_items = 0;
    if (hp != NULL) {
        numb_ports = hal_get_number_of_boards(hp);
        portselector_array =
            malloc(sizeof(struct portselector) * numb_ports);

        for (port = 0; port < numb_ports; port++) {
            int board_type = hal_get_board_type(hp);
            if (port > 0) {
                HAL *thp = hal_acquire(port);
                if (thp == NULL)
                    continue;
                board_type = hal_get_board_type(thp);
                hal_release(thp);
            }
            if (board_type == BOARDTYPE_TX) {
                if (!(menuitem_flags & MFLAG_PORTTYPE_TX))
                    continue;
                port_name = hal_utils_port_name(BOARDTYPE_TX);
            }
            else if (board_type == BOARDTYPE_RX) {
                if (!(menuitem_flags & MFLAG_PORTTYPE_RX))
                    continue;
                port_name = hal_utils_port_name(BOARDTYPE_RX);
            }
            else {
                continue;
            }
            portselector_array[numb_items].port_number = port;
            sprintf(portselector_array[numb_items].display_string,
                    "Channel %d [%s]", port+1, port_name);
            numb_items++;
        }

        hal_release(hp);
    }
#endif

    return numb_items;
}


/*********************************************************************
 * This menu item sits between all menu functions that may need the  *
 * user to select a specific board. It find all the boards matching  *
 * given criteria. If only one board found, it directly invokes the  *
 * intended function, else it shows the user a port selection choice *
 * menu.                                                             *
 *********************************************************************/
int portSelector(MENUINFO * minfo)
{


    MENU *selector;
    int rc;
    int idx = 0;
    char keylist[12];
    int retval = RC_NORMAL;
    int done = FALSE;
    int port_number;
    char titlebuf[128];
    int (*func_ptr) (MENUINFO *);
    HAL *hp = NULL;;
    int numb_items;
    char tempbuf[64];
    MENUITEMDEF *itemdef = NULL;
    int show_error = 0;

    /*******************************************
     * Retrieve the original intended function *
     *******************************************/
    itemdef = minfo->minfo_userptr;
    minfo->minfo_userptr = NULL;
    func_ptr = itemdef->menuitem_func;

    portselector_array = NULL;

    /***************************************************************
     * Enumerate all available boards matching the requested flags *
     ***************************************************************/
    numb_items = enumerate_ports(itemdef->menuitem_flags);


    /*********************************************************
     * If only one matching port found, jump directly to the *
     * relevent menu function                                *
     *********************************************************/
    if (numb_items == 1) {
        port_number = portselector_array[0].port_number;
        hp = hal_acquire(port_number);
        if (hp != NULL) {
            minfo->minfo_userptr = hp;
            rc = (*func_ptr) (minfo);
            hal_release(hp);
            return rc;
        }
        else {
            sprintf(tempbuf, "Hardware Error: Port %d not detected.",
                    port_number);
            show_error = 1;
        }
    }

    /*******************************************
     * If no matching ports found, throw error *
     *******************************************/
    if (numb_items == 0) {
        sprintf(tempbuf, "No matching ports found for this operation.");
        show_error = 1;
    }

    /********************
     * Print Menu title *
     ********************/
    sprintf(titlebuf, "%s:   Select Port", minfo->minfo_title);
    show_title(minfo, titlebuf);

    /**********************************************************
     * If port enumeration had an error, show it now and exit *
     **********************************************************/
    if (show_error) {
        return show_fault_dialog(minfo, tempbuf);
    }

    /******************************
     * Create the selector widget *
     ******************************/
    selector = create_selector(numb_items, portselector_cb);

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
    show_info(minfo, portselectorhelp);

    /************************
     * Set the current mode *
     ************************/
    set_selector_item(selector, 0);

    /***************************
     * Let user select a board *
     ***************************/
    while (done == FALSE) {
        strcpy(keylist, "");
        rc = process_selector(minfo, selector, keylist);

        /* Exit request */
        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            break;
        }
        /* Quit request */
        if (rc == RC_QUIT) {
            retval = RC_QUIT;
            break;
        }

        /*******************************
         * Process custom key requests *
         *******************************/
        switch (keylist[0]) {

        case '\0':
            /***************************************************
             * User selected a specific board. Acquire the HAL *
             * handle. This should normally succeed.           *
             ***************************************************/
            port_number = portselector_array[rc].port_number;

	    /* Store selected index in a local variable*/
	    idx = rc;	
            hp = hal_acquire(port_number);
            if (hp == NULL) {
                unpost_menu(selector);
                sprintf(tempbuf, "Hardware Error: Port %d not found.",
                        port_number);
                show_fault_dialog(minfo, tempbuf);
                show_title(minfo, titlebuf);
                post_menu(selector);
                show_info(minfo, portselectorhelp);
                break;
            }

            /*******************************************************
             * Call the originally intended function passed to us  *
             * passing the HAL pointer along. This could also be a *
             * submenu item that we are about to invoke.           *
             *******************************************************/
            unpost_menu(selector);

            minfo->minfo_userptr = hp;
            rc = (*func_ptr) (minfo);
            hal_release(hp);

            /* Exit request */
            if (rc == RC_EXIT) {
                done = TRUE;
                retval = RC_EXIT;
                break;
            }

            /********************************************************
             * Back from the intended function. In the meantime,   *
             * board types may have changed. Refresh and repost the *
             * selector.                                            *
             ********************************************************/
            show_title(minfo, titlebuf);
            numb_items = enumerate_ports(itemdef->menuitem_flags);
            refresh_selector(selector, numb_items, portselector_cb);
            set_selector_item(selector, idx);
            post_menu(selector);
            show_info(minfo, portselectorhelp);

            break;

        }

    }

    /************
     * Clean-up *
     ************/
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);
    if (portselector_array != NULL) {
        free(portselector_array);
        portselector_array = NULL;
    }

    return retval;

}
