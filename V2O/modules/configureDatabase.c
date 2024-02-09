
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "custmenu.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vutils.h"
#include "cli_utils.h"

/****************************************************
 * Command Line Routines For Database Related Access*
 ***************************************************/


int export_database_main(int argc, char **argv)
{
    int retval, i = 0;
    char command[256];

    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    strcpy(command, "/etc/scripts/export_database.sh");
    for (i = 1; i < argc; ++i) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }
    retval = system_command(command);

    return retval;
}

int import_database_main(int argc, char **argv)
{
    int retval, i = 0;
    char command[256];

    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    strcpy(command, "/etc/scripts/import_database.sh");
    for (i = 1; i < argc; ++i) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }
    retval = system_command(command);

    return retval;
}

int save_database_main(int argc, char **argv)
{
    int retval, i = 0;
    char command[256];

    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    strcpy(command, "/etc/scripts/save_database.sh");
    for (i = 1; i < argc; ++i) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }
    retval = system_command(command);

    return retval;
}

int restore_database_main(int argc, char **argv)
{
    int retval, i = 0;
    char command[256];

    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    strcpy(command, "/etc/scripts/restore_database.sh");
    for (i = 1; i < argc; ++i) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }
    retval = system_command(command);

    return retval;
}

int delete_database_main(int argc, char **argv)
{
    int retval, i = 0;
    char command[256];

    if (view_only() != 0) {
        printf("You must have admin privileges to reboot device.\n");
        return RC_ERROR;
    }

    strcpy(command, "/etc/scripts/delete_database.sh");
    for (i = 1; i < argc; ++i) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }
    retval = system_command(command);

    return retval;
}
