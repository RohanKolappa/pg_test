
#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "vlib.h"
#include "maint_commands.h"
#include "sample_pager_status_menu_gen.h"
#define FILENAME "/data/deepika/rawdata.txt"
#define EDID_DATA_LEN           128
#define EDID_TEXT_SIZE          4096
int getSamplePager(struct sample_pager_params *params) 
{
    FILE *fp;
    char s[1224];
    fp = fopen(FILENAME, "r");
    if (fp == NULL)
        return RC_ERROR;
    
    while (fgets(s, 1223, fp) != NULL) {
        strcat (params->textarea_value, s);
    }
    fclose(fp);

    return RC_NORMAL;

}
