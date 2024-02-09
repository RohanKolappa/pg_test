
#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "sample_status_menu_gen.h"
#include "vlog.h"


static bool b = 0;
static unsigned long l = 0;
static int i = 0;
static unsigned int ui = 0;
static unsigned int uib = 0;

int getSampleStatus(struct sample_status_params *params) 
{

    HAL *hp = params->userptr;
    
    if (hp == NULL)
        return RC_ERROR;

    if(b == 0)
        b = 1;
    else
        b = 0;
    params->abool_value = b;

    l = l+2;
    if (l > 1000)
        l = 0;
    params->bunsignedlong_value = l;
   
    params->cinteger_value = ++i;
    if ( i > 5000)
        i = 0;

    params->dfloat_value = hal_getVerticalFrequency(hp);
    
    ui = ui+1;
    if (ui > 1000)
        ui = 0;
    params->eunsignedintnum_value = ui;
    uib = uib+100000;
    if (uib > 1000000000)
        uib = 0;
    params->funsignedintbytes_value = uib;
    return RC_NORMAL;
}
