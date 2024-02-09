
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "loadnet.h"
#include "vlog.h"
#include "system_utils.h"
#include "v2d_streaming_profile_menu_gen.h"
#define AUTHCMD "/V2O/bin/add_auth_entry"
#define AUTHORIZATIONFILE "/V2O/config/authlistconf.txt"


static int validateV2DStreamingProfile(struct v2d_streaming_profile_params *params,
                      struct v2d_streaming_profile_params *orig_params,
                      char *error_string) 
{
    return RC_NORMAL;
}


int applyV2DStreamingProfile(struct v2d_streaming_profile_params *params,
                   struct v2d_streaming_profile_params *orig_params)
{
    char cmdbuf[256];
    if (!params->enable_auth_table_value) {
        unlink(AUTHORIZATIONFILE);
        sprintf(cmdbuf, "%s -ip 0.0.0.0 -mask 0.0.0.0 -maxBandwidth %s -lowCompression %d -highCompression %d", AUTHCMD,
                params->bandwidth, params->low_compression_value,
                params->high_compression_value);
        system_command(cmdbuf);
        printf ("%s\n", cmdbuf);

    }

    return RC_NORMAL;
}


void V2DStreamingProfileInit()
{
    registerV2DStreamingProfileValidateFunction(validateV2DStreamingProfile);
}
