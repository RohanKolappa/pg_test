
#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "hal_edid.h"
#include "edid_utils.h"
#include "eeprom_edid_status_menu_gen.h"

int getEepromEdid(struct eeprom_edid_params *params) 
{
    unsigned char buf[EDID_DATA_LEN];
    char edidtxt[EDID_TEXT_SIZE];

    HAL *hp;
    int  rc = 0;
    
    hp = params->userptr;
    
    if (hp == NULL) {
        strcat (params->textarea_value, "Failed: Invalid  Channel number");
        return RC_NORMAL;
    }

    rc = hal_read_edid_eeprom(hp, buf, EDID_DATA_LEN);
    
    if (rc < 0) {
        strcat (params->textarea_value, "Failed to read from Main Board EDID EEPROM\n");
    } 
    else {
        (void)  print_edid_to_buffer(buf, edidtxt, EDID_PAGER, 0);
        strcat (params->textarea_value, edidtxt);
    }

    return RC_NORMAL;

}
