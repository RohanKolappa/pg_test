
#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "vlib.h"
#include "vlog.h"
#include "maint_commands.h"
#include "var_log_status_menu_gen.h"
#define FILENAME "/var/log/messages"
#define STR_BLOCK_LENGTH 1024
int getVarLogMessages(struct var_log_messages_params *params) 
{
    FILE *fp;
    char str_block [STR_BLOCK_LENGTH];
    fp = fopen(FILENAME, "r");
    if (fp == NULL)
        return RC_ERROR;
    int prev_size = 0, size = 0;
    while (fgets(str_block, STR_BLOCK_LENGTH, fp) != NULL) {
        size += strlen(str_block);
        if ((strlen(str_block) + size) > FIELD_VAR_LOG_MESSAGES_TEXTAREA_LENGTH)
            break;
        memcpy (params->textarea_value + prev_size, str_block, strlen(str_block));
        prev_size += strlen(str_block);
    }
    fclose(fp);
    return RC_NORMAL;

}
