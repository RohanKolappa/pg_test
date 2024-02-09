#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "vlib.h"
#include "vlog.h"
#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "commonutils.h"
#include "system_utils.h"
#include "rx_kbm_layout_menu_gen.h"
#include "strmsrv_comm.h"
#include "standalone.h"
#include "hal_hwinfo.h"

#define KBLAYOUT "/etc/config/rxkblayout_%d.txt"

int init_rx_keyboard_layout(HAL *hp);

typedef struct LangCode_Rx {
    unsigned int index;
    char *name;
} LANGCODE;

enum LangId { /* arrange this in alphabetic order */
    ENGLISH_US = 0,
    GERMAN, 
    SPANISH, 
    BELGIUM,
    JAPAN,
    FRENCH
} ;

// #TO DO Arrange array in exact order when list grows with county id & list in
// cli menu */
LANGCODE LangCode_Rx[] = { 
    {0, "US"},
    {1, "German"},
    {2, "French"},
	{3, "Belgian French"},
    {4, "Spanish"},
    {5, "Dutch"},
    {6, "Italian"},
    {7, "Portuguese"},
    {8, "Russian"},
};

static int set_rx_kb_layout_config_file(int value, HAL *hp)
{
    FILE *fp;
    char buf[256];
    sprintf(buf, "/etc/config/rxkblayout_%d.txt", hal_get_board_number(hp));
	
    fp = fopen(buf, "wb");

    if (fp == NULL)
		return RC_ERROR;

    fprintf(fp, "%s\n", LangCode_Rx[value].name);

	fclose(fp);

    return RC_NORMAL;
}

int init_rx_keyboard_layout(HAL *hp) {

	 VLOG_INFO_CAT(VLOG_KBM_KB, "init_rx_keyboard_layout Enter \n");

    if (hp == NULL)
        return RC_ERROR;

    struct rx_kbm_layout_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveRxKBMLayoutValues(&params);
   
    return set_rx_kb_layout_config_file(params.keyboard_layout_value, hp);
}


int applyRxKBMLayout(struct rx_kbm_layout_params *params,
        struct rx_kbm_layout_params *orig_params) {

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    struct strmsrv_rx_kb_layout_params rx_kb_layout;
    //int set_rx_kbm_layout = 0;

     memset(&rx_kb_layout, 0x00, sizeof(rx_kb_layout));
     
    if ((hp->hw_info.hardware_type != PLX_DEVICE) &&
            (hp->hw_info.hardware_type != XPI_DEVICE))
        return RC_ERROR;

    if (params->keyboard_layout_set == 1) {
        rx_kb_layout.kblayout_set = 1;
        set_rx_kb_layout_config_file(params->keyboard_layout_value, hp);
        strcpy(rx_kb_layout.kblanguage, LangCode_Rx[params->keyboard_layout_value].name);
        strmsrv_set_rx_kb_layout_params(hal_get_channel_number(hp), &rx_kb_layout);

    }

    return RC_NORMAL;

}
