#define CUSTOM_MENU_DEFS
#include "custmenu.h"
#include "hal.h"
#include "sample_log_gen.h"
#define VLOG_CATEGORY_NAME "hal"
#include "vlog.h"

//#include "log4c.h"

//static log4c_category_t* mycat = NULL;
#define FILENAME "/tmp/a"
static unsigned long l = 0;
int getSampleLog(struct sample_log_params *params)
{
    FILE *ifile = NULL;
    int len1;
    char *filename = NULL;
    unsigned char buf1[1024];
    
    l = l+2;
    if (l > 1000)
        l = 0;

    params->bunsignedlong_value = l;

    VLOG_INFO("LOG INFO Hello Debug: %lu", l);
    VLOG_INFO_CAT("a","LOG INFO Hello Debug: %lu", l);
    
    VLOG_WARNING("LOG WARINING Hello Debug: %lu", l);
    VLOG_WARNING_CAT("a","LOG WARNING Hello Debug: %lu", l);
    
    VLOG_DEBUG("LOG DEBUG Hello Debug: %lu", l);
    VLOG_DEBUG_CAT("a","LOG DEBUG Hello Debug: %lu", l);
    
    VLOG_TRACE("LOG DETAIL Hello Debug: %lu", l);
    VLOG_TRACE_CAT("a","LOG DETAIL Hello Debug: %lu", l);
    filename = FILENAME;
    if ((ifile = fopen(filename, "rb")) == NULL) {
        printf("Error opening file %s. (%m).\n", filename);
        return -1;
    }
    len1 = fread(buf1, 1, sizeof(int), ifile);
    VLOG_INFO_HEXDATA(buf1, len1);
    VLOG_DEBUG_HEXDATA(buf1, len1);

    //log4c_category_log(mycat, MYLOG_PRIORITY_INFO, "OLD Hello Debug: %d", l);

     return RC_NORMAL;

}


