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
#include "tx_kbm_layout_menu_gen.h"
#include "strmsrv_comm.h"
#include "standalone.h"
#include "hal_hwinfo.h"

typedef struct LangCode {
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

int init_keyboard_layout(HAL *hp);

// #TO DO Arrange array in exact order when list grows with country id & list in
// cli menu */
LANGCODE LangCode[] = { 
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

static int set_kb_layout_config_file(int value, HAL *hp)
{
    FILE *fp;
    char buf[256];
    sprintf(buf, "/etc/config/txkblayout_%d.txt", hal_get_board_number(hp));

    fp = fopen(buf, "wb");

    if (fp == NULL)
    return RC_ERROR;

    fprintf(fp, "%s\n", LangCode[value].name);

    fclose(fp);

    return RC_NORMAL;
}

int init_keyboard_layout(HAL *hp) {

    if (hp == NULL)
        return RC_ERROR;

    struct tx_kbm_layout_params params;
    memset(&params, 0x00, sizeof(params));
    params.userptr = hp;
    retrieveTxKBMLayoutValues(&params);
   
    return set_kb_layout_config_file(params.keyboard_layout_value, hp);
}


int applyTxKBMLayout(struct tx_kbm_layout_params *params,
        struct tx_kbm_layout_params *orig_params) {

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    struct strmsrv_tx_kb_layout_params tx_kb_layout;
    int set_tx_kbm_layout = 0;

    memset(&tx_kb_layout, 0x00, sizeof(tx_kb_layout));
     
     /* Enable this condition for xpi device */
    if (hp->hw_info.hardware_type != PLX_DEVICE &&
        hp->hw_info.hardware_type != XPI_DEVICE)
        return RC_ERROR;

    if (params->keyboard_layout_set == 1) {
        tx_kb_layout.kblayout_set = 1;
        set_kb_layout_config_file(params->keyboard_layout_value, hp);
        strcpy(tx_kb_layout.kblanguage, LangCode[params->keyboard_layout_value].name);
        strmsrv_set_tx_kb_layout_params(hal_get_channel_number(hp), &tx_kb_layout);
    }

    return RC_NORMAL;

}

#if 0

int applyTxKBMLayout(struct tx_kbm_layout_params *params,
                    struct tx_kbm_layout_params *orig_params) {

    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    return RC_NORMAL;
}
#endif
