
#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#include <sys/wait.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"
#include "commonutils.h"
#include "text_utils.h"

#include "database_management_status_menu_gen.h"



int getDatabaseManagementStatus(struct db_management_status_params *params) {

    FILE *statusFile;
    char inbuf[256];
    char *cp;

    statusFile = fopen("/data/sedna_dbsnapshot/status/lastbackup.status", "r");
    
    if (statusFile != NULL) {
        while (fgets(inbuf, sizeof(inbuf), statusFile) != NULL) {
            cp = parse_status_line(inbuf, "BACKUPTIME"); 
            if (cp != NULL)
                strncpy(params->last_backup_time_value, cp,
                        FIELD_DB_MANAGEMENT_STATUS_LAST_BACKUP_TIME_LENGTH);
            cp = parse_status_line(inbuf, "BACKUPREASON"); 
            if (cp != NULL)
                strncpy(params->last_backup_reason_value, cp,
                        FIELD_DB_MANAGEMENT_STATUS_LAST_BACKUP_REASON_LENGTH);

        }
        fclose(statusFile);
    }
    
    statusFile = fopen("/data/sedna_dbsnapshot/status/lastrestore.status", "r");
    
    if (statusFile != NULL) {
        while (fgets(inbuf, sizeof(inbuf), statusFile) != NULL) {
            cp = parse_status_line(inbuf, "RESTORETIME"); 
            if (cp != NULL)
                strncpy(params->last_restore_time_value, cp,
                        FIELD_DB_MANAGEMENT_STATUS_LAST_RESTORE_TIME_LENGTH);
            cp = parse_status_line(inbuf, "BACKUPTIME"); 
            if (cp != NULL)
                strncpy(params->last_restore_created_value, cp,
                        FIELD_DB_MANAGEMENT_STATUS_LAST_RESTORE_CREATED_LENGTH);

            cp = parse_status_line(inbuf, "REASON"); 
            if (cp != NULL)
                strncpy(params->last_restore_reason_value, cp,
                        FIELD_DB_MANAGEMENT_STATUS_LAST_RESTORE_REASON_LENGTH);

            cp = parse_status_line(inbuf, "WAS_CORRUPTED"); 
            if (cp != NULL) {
                if (strcmp(cp, "true") == 0)
                    params->corrupted_value = 1;
                else
                    params->corrupted_value = 0;
            }
            
        }
        fclose(statusFile);
    }
    

    return RC_NORMAL;
}
