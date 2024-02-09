/*
 * check_device_command.c
 *
 *  Created on: Jan 29, 2010
 *      Author: rkale
 */

#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "custmenu.h"
#include "commonutils.h"
#include "vlog.h"
#include "vutils.h"
#include "cli_utils.h"

int check_device_main(int argc, char **argv) {

    char command[128];
    int i;

    if (view_only() != 0) {
        printf("You must have admin privileges to check device.\n");
        return RC_ERROR;
    }


    // Create command, appending command line args to the script
    sprintf(command, "/etc/scripts/check_device.sh");
    for (i = 1; i < argc; i++) {
        if (strlen(command) + strlen(argv[i]) >= 126) {
            printf("Warning: Command too long. Ignoring arg %s and beyond\n", argv[i]);
            break;
        }
        strcat(command, " ");
        strcat(command, argv[i]);
    }

    // Run the command directly: we want to see all the output
    system(command);

    return RC_NORMAL;
}
