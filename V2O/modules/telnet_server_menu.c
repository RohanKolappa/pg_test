#include <stdlib.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "telnet_server_menu_gen.h"



int applyTelnetServer(struct telnet_server_params *params,
                             struct telnet_server_params *orig_params)
{
    char buff[128];
    int retcode;
    int found;

    /* Start telnetd if it's not currently running  */
    found = find_proc_by_name("telnetd");
    if (!found && params->telnet_value) {

        // TODO: ensure we do not inherit currently open file handles.
        if (verbose_errors)
            printf("Starting Telnet Daemon...\n");
        sprintf(buff, "/usr/sbin/telnetd");
        retcode = system_command(buff);
        if (retcode != 0 && verbose_errors) {
            printf("Could not start telnet daemon\n");
        }

    }
    else if (found && params->telnet_value == 0) {
        /* If telnetd currently running, kill it */
        retcode = system_command("/usr/bin/killall telnetd");
        if (retcode != 0 && verbose_errors)
            printf("Could not kill telnetd.\n");
    }

    return RC_NORMAL;

}
