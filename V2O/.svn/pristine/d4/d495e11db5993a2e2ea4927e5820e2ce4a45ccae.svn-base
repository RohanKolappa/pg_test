
#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "hal_edid.h"
#include "edid_utils.h"
#include "monitor_edid_status_menu_gen.h"

int getMonitorEdid(struct monitor_edid_params *params) 
{
    unsigned char buf[EDID_DATA_LEN];
    char edidtxt[EDID_TEXT_SIZE];
    HAL *hp;
    int  rc = 0, extensions, textlen;

    hp = params->userptr;
    
    if (hp == NULL) {
        strcat (params->textarea_value, "Failed: Invalid  Channel number");
        return RC_NORMAL;
    }

    rc = hal_read_mon_edid(hp, buf, EDID_DATA_LEN);
    
    if (rc < 0) {
        strcat (params->textarea_value, 
                "\nFailed to read Monitor EDID. Please disconnect\n"
                "and re-connect the DVI cable to recover.\n");
        return RC_NORMAL;
    } 
    else {
        print_edid_to_buffer(buf, edidtxt, EDID_PAGER, 0);
        strcat (params->textarea_value, edidtxt);
    }

    extensions = buf[126];

    if (extensions != 0) {
        int i;    
        for (i = 0; i < extensions; i++) {
            textlen = strlen(params->textarea_value);
            rc = hal_read_mon_edid_extension(hp, i+1, buf, EDID_DATA_LEN);
            if (rc < 0) {
                strcat (params->textarea_value, 
                        "Failed to read Monitor EDID. Please disconnect\n"
                        "and re-connect the DVI cable to recover.\n");
                return RC_NORMAL;
            } 
            else {
                print_edid_extension_to_buffer(buf, edidtxt, EDID_PAGER, i+1);
                if (textlen + strlen(edidtxt) < 8196)
                    strcat (params->textarea_value, edidtxt);
            }
        }
    }

    return RC_NORMAL;
}
