

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "xmpp_agent_menu_gen.h"
#include "xmpp_agent_config.h"
#include "verify_license.h"
#include "vlog.h"

#define TEMPLATEDIR "/home/onair/templates"
#define TMPFILE "/tmp/xmppagent"
#define XMPP_LOGIN_TEMPLATE "/home/onair/templates/LoginRequest.xml"
#define XMPP_SETSERVICEDOMAIN_TEMPLATE "/home/onair/templates/SetServiceDomainRequest.xml"
#define XMPP_LOGIN_FILE "/etc/config/LoginRequest.xml"
#define XMPP_SETSERVICEDOMAIN_FILE "/etc/config/SetServiceDomainRequest.xml"

#define XMPP_AGENT_SCRIPTFILE "/tmp/xmppagent.sh"

int get_xmpp_agent_config(struct xmpp_agent_config *agent_cfg) {
    struct xmpp_agent_params params;
    retrieveXMPPAgentValues(&params);

    memset(agent_cfg, 0x00, sizeof(struct xmpp_agent_config));
    agent_cfg->enable = params.enable_value;
    strncpy(agent_cfg->serverip, params.serverip_value,
            XMPP_AGENT_CONFIG_SERVERIP_LEN);
    strncpy(agent_cfg->devicename, params.devicename_value,
            XMPP_AGENT_CONFIG_DEVICENAME_LEN);

    return 0;
}


static void RemoveFiles(void) {

    struct stat sbuf;


    if (stat("/etc/scripts/watchdog/watch_streamingserver.sh", &sbuf) == 0)
        remove("/etc/scripts/watchdog/watch_streamingserver.sh");

    if (stat("/home/onair/TV/bin/StreamingServer", &sbuf) == 0)
        remove("/home/onair/TV/bin/StreamingServer");

    if (stat("/etc/scripts/watchdog/watch_deviceapp.sh", &sbuf) == 0)
        remove("/etc/scripts/watchdog/watch_deviceapp.sh");

    if (stat("/etc/scripts/watch_managementserver.sh", &sbuf) == 0)
        remove("/etc/scripts/watch_managementserver.sh");

}

static  int validateXMPPAgent(struct xmpp_agent_params *params,
                            struct xmpp_agent_params *orig_params,
                            char *error_string)
{
#if 0
    if (params->enable_value) {
        if (strcmp(params->serverip_value, "127.0.0.1") == 0) {
            sprintf(error_string, "XMPP IP Address cannot be 127.0.0.1");
            return RC_INVALID_VALUE;
        }
    }
#endif
    if (!isHavingBaseLicense(0)) {
        RemoveFiles();
        sprintf(error_string, "Valid License is not found to set xmpp agent on this device");
        return RC_ERROR;
    }
    return RC_NORMAL;
}

int applyXMPPAgent(struct xmpp_agent_params *params, 
                   struct xmpp_agent_params *orig_params)
{
    FILE    *fp=NULL;
    if ((fp = fopen(XMPP_AGENT_SCRIPTFILE, "w")) == NULL) {
        return -1;
    }
    fprintf (fp, "#!/bin/sh\n");

    if (params->enable_value) {
        fprintf (fp, "sed -e 's/TEMPLATE_USER/%s/'", params->devicename_value);
        fprintf (fp, " %s ",XMPP_LOGIN_TEMPLATE);

        fprintf (fp, "|");
        fprintf (fp, "sed -e 's/TEMPLATE_PASSWORD/%s/'", params->password_value);
        fprintf (fp, "|");
        fprintf (fp, "sed -e 's/TEMPLATE_PORT/%d/'", params->port_value);
        fprintf (fp, "|");
        fprintf (fp, "sed -e 's/TEMPLATE_RESOURCE/%s/'", params->devicename_value);
        fprintf (fp, "|");
        fprintf (fp, "sed -e 's/TEMPLATE_SERVER/%s/'", params->serverip_value);
        fprintf (fp, "|");
        fprintf (fp, "sed -e 's/TEMPLATE_DOMAIN/%s/'", params->serverip_value);

        fprintf (fp, " >%s \n", TMPFILE);

        fprintf (fp, "cp %s %s\n", TMPFILE, XMPP_LOGIN_FILE);
        fprintf (fp, "cp %s %s\n", XMPP_SETSERVICEDOMAIN_TEMPLATE, TMPFILE);

        fprintf (fp, "sed -e 's/TEMPLATE_SERVICEDOMAIN/%s/'", 
                    params->servicedomain_value);
        fprintf (fp, " %s ", XMPP_SETSERVICEDOMAIN_TEMPLATE);
        fprintf (fp, "|");
        fprintf (fp, "sed -e 's/SYSTEM/%s/'", 
                    params->password_value);
        fprintf (fp, " >%s \n", TMPFILE);
        fprintf (fp, "cp %s %s\n", TMPFILE, XMPP_SETSERVICEDOMAIN_FILE);
        fprintf (fp, "/bin/rm %s\n", TMPFILE);
    }
    else {
        fprintf (fp, "/bin/rm %s\n", XMPP_LOGIN_FILE);
        fprintf (fp, "/bin/rm %s\n", XMPP_SETSERVICEDOMAIN_FILE);
        fprintf (fp, "/bin/touch %s\n", XMPP_LOGIN_FILE);
    }
    fclose(fp);
    chmod (XMPP_AGENT_SCRIPTFILE,00700 );
    system_command (XMPP_AGENT_SCRIPTFILE);
    system_command("/usr/bin/killall dclient");


    return RC_NORMAL;

}

void XMPPAgentInit(void)
{
    registerXMPPAgentValidateFunction(validateXMPPAgent);

}

