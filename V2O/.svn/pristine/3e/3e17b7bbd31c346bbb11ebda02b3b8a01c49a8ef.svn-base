

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "nas_list_menu_gen.h"
#include "verify_license.h"

#define CONFIG_DIR "/etc/config"
#define NASSCRIPT  "/etc/scripts/watchdog/watch_nas.sh"

char * constructHeaderNASServerList(char * buffer) {
    sprintf(buffer, "%-10s %-16s %-20s",
            "Name", "Server", "Directory");
    return buffer;
}

char * constructRowNASServerList(char *buffer, 
                                 struct nas_list_params *params) {

    sprintf(buffer, "%-10.10s %-16.16s %-20.20s",
            params->name_value,
            params->server_value,
            params->remote_directory_value);
 
    return tempbuf;
}

static void RemoveFiles(void) {

    struct stat sbuf;

    if (stat("/etc/scripts/watch_nas.sh", &sbuf) == 0)
        remove("/etc/scripts/watch_nas.sh");

    if (stat("/bin/nasctl", &sbuf) == 0)
        remove("/bin/nasctl");

}



int validateNASServerList(int req_code,
                          struct nas_list_params *params,
                          struct nas_list_params *orig_params,
                          char * error_string) {

    struct nas_list_params *paramsp;
    
    
    if (!isHavingNASLicense()) {
        RemoveFiles();
        sprintf(error_string, "Valid License is not found to config  NAS  on this device");
//        return RC_ERROR;
    }

    // Validate delete request
    if (req_code == DEL_REQUEST) {

        return RC_NORMAL;
    }

    // Validate add request
    if (req_code == ADD_REQUEST) {
        if ((params->name_set == 0) || 
            (strcmp(params->name_value, "") == 0)) {
            sprintf(error_string, "Need to specify name.");
            return RC_ERROR;
        }

    }

#if 0
    // Should not allow change of name during modify
    if (req_code == MOD_REQUEST) {
        if ((params->name_set == 1) &&
            (strcmp(params->name_value, orig_params->name_value) != 0)) {
            sprintf(error_string, "Cannot modify name.");
            return RC_ERROR;
        }
    }
#endif
    
    // Check for duplicate
    foreach_nas_list_entry(paramsp) {
        if (params->name_set == 1) {
            // if modify request, skip self
            if (req_code == MOD_REQUEST && 
                paramsp->nas_list_id == params->nas_list_id) {
                continue; 
            }
            if (strcmp(paramsp->name_value, params->name_value) == 0) {
                sprintf(error_string, "Duplicate: Name \'%s\' conflicts.",
                    params->name_value);
                return RC_DUPLICATE;
            }
        }
    }

    return RC_NORMAL;
}

static FILE *open_nas_config_file(struct nas_list_params *params)
{
    char configfilename[256];
    FILE *fp;
    sprintf(configfilename, "%s/nas_%d.conf", 
            CONFIG_DIR, params->nas_list_id);
    if ((fp = fopen(configfilename, "w")) == NULL)
        return NULL;
    return fp;
}

static void write_nas_config_params(FILE * fp, struct nas_list_params *params)
{
    fprintf (fp, "NAME=\"%s\"\n", params->name_value);
    fprintf (fp, "ID=%d\n", params->nas_list_id);
    fprintf (fp, "ENABLE=%d\n", params->enable_value);
    fprintf (fp, "READONLY=%d\n", params->readonly_value);
    fprintf (fp, "NFS_SERVER=%s\n", params->server_value);
    fprintf (fp, "REMOTE_DIR=%s\n", params->remote_directory_value);
}

static void run_nas_script(void)
{
    char command[64];
    sprintf(command, "%s hup", NASSCRIPT);
    system_command(command);
}


// Apply for NAS lists
int applyNASServerList(int req_code,
                       struct nas_list_params *params,
                       struct nas_list_params *orig_params) {


    FILE *fp=NULL;

    if (req_code == DEL_REQUEST) {

        if ((fp = open_nas_config_file(orig_params)) == NULL)
            return RC_ERROR;

        fprintf (fp, "ACTION=delete\n");
        write_nas_config_params(fp, orig_params);
        fclose(fp);

        run_nas_script();

        return RC_NORMAL;
    }

    if (req_code == ADD_REQUEST) {

        /* to preserve entries when swicthing from valid license to non-valid license case 
         *  the table entry is getting deleted because the values are not applied */
        if (!isHavingNASLicense()) {
            RemoveFiles();
            return RC_NORMAL;
        }
                            
        if ((fp = open_nas_config_file(params)) == NULL)
            return RC_ERROR;

        fprintf (fp, "ACTION=add\n");
        write_nas_config_params(fp, params);
        fclose(fp);

        run_nas_script();

        return RC_NORMAL;
    }

    if (req_code == MOD_REQUEST) {

        if (!isHavingNASLicense()) {
            RemoveFiles();
            return RC_NORMAL;
        }

        if ((fp = open_nas_config_file(params)) == NULL)
            return RC_ERROR;

        fprintf (fp, "ACTION=modify\n");
        write_nas_config_params(fp, params);
        fclose(fp);

        run_nas_script();

        return RC_NORMAL;
    }
    

    return RC_NORMAL;
}

