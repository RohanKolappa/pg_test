#include <stdlib.h>
#include <string.h>
#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "logging_level_menu_gen.h"


char * constructHeaderLoggingLevel(char * buffer) {
    sprintf(buffer, "%-40s %-4s",
            "Category Name", "Enable");
    return buffer;
}

char * constructRowLoggingLevel(char *buffer, 
                                    struct logging_level_params *params) {

    sprintf(buffer, "%-40s %-4s",
            params->name_value,
            params->enable);

    return tempbuf;
}



int validateLoggingLevel(int req_code,
                          struct logging_level_params *params,
                          struct logging_level_params *orig_params,
                          char * error_string) {

    if (req_code == ADD_REQUEST) {
        if ((params->name_set == 0) ||
            (strcmp(params->name_value, "") == 0)) {
            sprintf(error_string, "Need to specify category name.");
            return RC_ERROR;
        }
        
    }

    return RC_NORMAL;
}

#define LOG4CRC_FILE "/home/onair/log4crc"
#define LOG4CRC_TEMPLATE_FILE "/home/onair/templates/log4crc"

void WriteLoggingLevel(int req_code) {

    char buff[350];
    struct logging_level_params *paramsp;


    retrieveLoggingLevelValues();

    sprintf(buff, "/bin/cp %s %s", LOG4CRC_TEMPLATE_FILE, LOG4CRC_FILE);
    system_command(buff);

    char buffer [250];
    foreach_logging_level_entry(paramsp) {

        if (paramsp->enable_value == 1) {
            sprintf(buffer,"<category name=\"%s\" priority=\"debug\" appender=\"ipvs_syslog\" additivity=\"false\"/>",paramsp->name_value);

            sprintf(buff, "/bin/sed -i -e '$ i\\        %s' %s", buffer, LOG4CRC_FILE);
            system_command(buff);
        }

    }
    
    sprintf(buff, "killall -HUP StreamingServer");
    system_command(buff);
}
// Apply for Logging Level
int applyLoggingLevel(int req_code,
                       struct logging_level_params *params,
                       struct logging_level_params *orig_params) {
    return RC_NORMAL;
}

void LoggingLevelInit(){
        registerLoggingLevelSavedFunction(WriteLoggingLevel);
}

