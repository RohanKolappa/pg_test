
#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#include <sys/wait.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "xmpp_agent_status_menu_gen.h"
#include "xmlUtils.h"

static void retrieveXMPPServerName(char * buffer, int bufferLength)
{
    /* The contents of the /tmp/status is an xml snippet that stores
     * server logged in to in element "serverIP"
     */
    strncpy(buffer, "", bufferLength);
    XMLParser * parser = createXMLParser("/tmp/status");
    const char * serverName = XMLParserGetElementText(parser, "serverIP");
    if (serverName != NULL)
        strncpy(buffer, serverName, bufferLength);
    destroyXMLParser(parser);
}

int getXMPPAgentStatus(struct xmpp_agent_status_params *params) {

    struct stat buf;

    /* If /tmp/status file exists, the xmpp agent is logged in */
    /* Warning: for changing this file name see DRY.2 */
    if (stat("/tmp/status", &buf) == 0) {
        params->status_value = 1;
        retrieveXMPPServerName(params->xmpp_server_value,
                FIELD_XMPP_AGENT_STATUS_XMPP_SERVER_LENGTH);
    }
    else {
        params->status_value = 0;
        strncpy(params->xmpp_server_value, "", FIELD_XMPP_AGENT_STATUS_XMPP_SERVER_LENGTH);
    }
    return RC_NORMAL;
}
