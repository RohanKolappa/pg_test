

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "database_management_menu_gen.h"
#include "validationroutines.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"

#define RECOVERYMODEFILE  "/etc/config/restore.factorydefault"
#define EXPORTURLFILE  "/etc/config/export.database.url"


static int validateDatabaseManagement(struct db_management_params *params,
                                      struct db_management_params *orig_params,
                                      char * error_string)
{  

    if (strlen(params->export_url_value) > 0) {
        if (strncmp(params->export_url_value, "ftp://", 6) != 0) {
            strcpy(error_string, "URL should start with ftp://");
            return RC_ERROR;
        }
    }

    if (params->enable_export_value == 1) {
        if (strlen(params->export_url_value) == 0) {
            strcpy(error_string, "URL cannot be empty if export is enabled");
            return RC_ERROR;
        }
    }

    if (params->daily_backup_value) {
        if (strlen(params->when_value) == 0) {
            strcpy(error_string, "Need to define list of backup times.");
            return RC_ERROR;
        }
        char * cp = params->when_value;
        if (check_integerlist_range(cp, 0, 23) != RC_NORMAL) {
            sprintf(error_string, "Out of range or integers need to be separated by commas");
            return RC_ERROR;
        } 

    }
    return RC_NORMAL;
}

int applyDatabaseManagement(struct db_management_params *params,
                            struct db_management_params *orig_params)
{
    char buff[256];
    if (params->daily_backup_value) {

        system_command("cp /home/onair/templates/crontab /etc/crontab");
        sprintf(buff, "echo \"0 %s * * * /etc/scripts/sednactl.sh  create_snapshot Scheduled\" > /etc/config/db.crontab",
                params->when_value);
        system_direct_command(buff);
        system_command("/usr/bin/crontab /etc/crontab");
    } else {
        system_command("/bin/rm -f /etc/config/db.crontab");
    }
    system_command("cp /home/onair/templates/crontab /etc/crontab");
    system_direct_command("cat /etc/config/*.crontab >> /etc/crontab");
    system_command("/usr/bin/crontab /etc/crontab");


    if (params->recovery_mode_value == DB_MANAGEMENT_RECOVERY_MODE_FACTORY) {
        system_command("/bin/touch " RECOVERYMODEFILE);
    } else {
        system_command("/bin/rm -f " RECOVERYMODEFILE);
    }

    if (params->enable_export_value == 1 &&
        strlen(params->export_url_value) > 0) {
        sprintf(buff, "echo \"%s\" > %s",
                params->export_url_value, EXPORTURLFILE);
        system_direct_command(buff);
    } else {
        system_command("/bin/rm -f " EXPORTURLFILE);
    }

    return RC_NORMAL;
}

void DatabaseManagementInit(void)
{
    registerDatabaseManagementValidateFunction(validateDatabaseManagement);
}
