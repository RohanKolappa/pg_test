#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "nas_status_menu_gen.h"
#include "nas_list_menu_gen.h"
#include "commonutils.h"
#include "text_utils.h"
#include "vlog.h"

char *constructHeaderNASStatus(char *buffer)
{

    sprintf(buffer, "%-10s %-8s %4s %-35s",
            "Name", "Status", "Used", "Server");

    return buffer;
}

char *constructRowNASStatus(char *buffer,
                            struct nas_status_params *params)
{
    sprintf(buffer, "%-10.10s %-8.8s %3.0f%% %-35.35s",
            params->name_value,
            params->status_value,
            params->percent_used_value,
            params->server_value);
    return buffer;
}

static void get_nfs_stat(char *mount_point, 
                         struct nas_status_params *params) {
    
    char line[512];
    char word[32];
    FILE *fp;
    int search_state;
    unsigned long long writeb;
    unsigned long long readb;

    sprintf(line, "/proc/self/mountstats"); 

    fp = fopen(line, "r");
    if (fp == NULL)
        return;

    search_state = 0;
    while (fgets(line, 512, fp) != NULL) {
        
        /* Find a matching mount_point */
        if (strncmp(line, "device ", 7) == 0) {
            search_state = 1;
            if (strstr(line , mount_point) != NULL) {
                search_state = 2;
            }
        }

        if (search_state == 2) {
            if (strstr(line, "bytes:") != NULL) {
                sscanf(line, "%s %llu %llu", word,
                       &readb, &writeb);
                params->rx_bytes_value = readb;
                params->tx_bytes_value = writeb;
            }
        }

    }
    fclose(fp);
}

static void get_disk_usage(int index, 
                          struct nas_status_params *params) {

    char buff[128];
    FILE *fp;
    char *buffer;
    
    sprintf(buff, "/var/status/nas_%d.df", index); 

    fp = fopen(buff, "r");
    if (fp != NULL) {
        while (1) {
            buffer = fgets(buff, 128, fp);
            if (buffer == NULL)
                break;

            if (strstr(buffer, "/data1/DSS/data/portal/site/NAS") == NULL) {
                continue;
            }
            
            
            buffer = strpbrk(buffer, " \t");
            if (buffer == NULL)
                break;
            buffer = strpbrk(buffer, "0123456789");
            if (buffer == NULL)
                break;
            
            sscanf(buffer, "%llu %llu %llu", 
                   &params->total_bytes_value,
                   &params->used_bytes_value,
                   &params->available_bytes_value);
            
            params->total_bytes_value *= 1024;
            params->used_bytes_value *= 1024;
            params->available_bytes_value *= 1024;
            
            /* We ignore the used value reported by the system and compute 
               our own based on the available value */
            params->used_bytes_value = 
                params->total_bytes_value - params->available_bytes_value;
            
            if (params->total_bytes_value > 0) {
                
                params->percent_used_value = 
                    ((params->total_bytes_value - 
                      params->available_bytes_value) * 100.0)/params->total_bytes_value;
            }
            else {
                params->percent_used_value = 0.0;
            }
            
            break;
        }
        
        fclose(fp);
    }
}

int getNASStatus(struct nas_status_params *params)
{
    FILE *statusFile;
    int idx;
    char buf[128];
    char mount_point[128];
    char inbuf[256];
    char *cp;

    struct nas_list_params *nas_config_params;

    // First get the NAS config
    retrieveNASServerListValues();

    // For each NAS config find the status
    foreach_nas_list_entry(nas_config_params) {

        params = getNASStatusParamsStruct(params);

        idx = nas_config_params->nas_list_id;
        params->index_value = idx;
        strncpy(params->name_value, nas_config_params->name_value, 
               FIELD_NAS_STATUS_NAME_LENGTH);
        sprintf(buf, "/var/status/nas_%d.status", idx);
        statusFile = fopen(buf, "r");

        if (statusFile == NULL) {
            sprintf(params->status_value, "Error?");
            continue;
        }

        
        while (fgets(inbuf, sizeof(inbuf), statusFile) != NULL) {
            cp = parse_status_line(inbuf, "STATUS"); 
            if (cp != NULL)
                strncpy(params->status_value, cp, FIELD_NAS_STATUS_STATUS_LENGTH);

            cp = parse_status_line(inbuf, "SERVER"); 
            if (cp != NULL)
                strncpy(params->server_value, cp, FIELD_NAS_STATUS_SERVER_LENGTH);

            cp = parse_status_line(inbuf, "INFO"); 
            if (cp != NULL) {
                strncpy(params->error_message_value, cp, 
                        FIELD_NAS_STATUS_ERROR_MESSAGE_LENGTH);
            }
        }

        fclose(statusFile);

        if (strcmp(params->status_value, "Ok") == 0) {
            get_disk_usage(idx, params);
        }

        sprintf(mount_point, "/data1/DSS/data/portal/site/NAS/%d", idx);
        
        get_nfs_stat(mount_point, params);
    }

    return RC_NORMAL;
    
}
