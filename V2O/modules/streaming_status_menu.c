#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>


#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "remoteStatus.h"
#include "strmsrv_comm.h"
#include "streaming_status_menu_gen.h"


char *constructHeaderStreamingStatus(char *buffer)
{


    return buffer;
}

char *constructRowStreamingStatus(char *buffer,
                                  struct streaming_status_params *params)
{

    return buffer;
}

static void show_streaming_parameters(char *command)
{
    char *xmlstr;
    struct strmsvr_result *ss_result;

    ss_result = strmsvr_command(command);
    if (ss_result != NULL) {
        if ((xmlstr = strmsvr_get_result_xml(ss_result)) != NULL)
            remote_status_translate_parameters(xmlstr);
        strmsvr_release_result(ss_result);
    }
}

int getStreamingStatus(struct streaming_status_params *params) 
{

    /* Show Sender stats */
    show_streaming_parameters("action=xlist dsttype=AnyDecoderPort");

    /* Show Receiver stats */
    show_streaming_parameters("action=xlist srctype=AnyNetworkPort");

    /* Show Reader stats */
    show_streaming_parameters("action=xlist srctype=AnyMediaPort");

    /* Show Writer stats */
    show_streaming_parameters("action=xlist dsttype=AnyRecorderPort");

    /* We have collected all the objects from StreamingServer. Print the stats */
    remote_status_print();

    return RC_NORMAL;
}
