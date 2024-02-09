/* bot.c
** libstrophe XMPP client library -- basic usage example
**
** Copyright (C) 2005-2009 Collecta, Inc. 
**
**  This software is provided AS-IS with no warranty, either express
**  or implied.
**
**  This software is distributed under license and may not be copied,
**  modified or distributed except as expressly authorized under the
**  terms of the license contained in the file LICENSE.txt in this
**  distribution.
*/

/* simple bot example
**  
** This example was provided by Matthew Wild <mwild1@gmail.com>.
**
** This bot responds to basic messages and iq version requests.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <strophe.h>
#include <expat.h>
#define XMPP_TRANSACTION_TIMEOUT    45
#define MAXXMLTEXTDATA  2048
#define AGENTMESSAGETYPE        "type"
#define AGENTMESSAGE            "AgentMessage"
#define AGENTMESSAGEREQUEST     "Request"
#define AGENTMESSAGERESPONSE    "Response"
#define AGENTMESSAGEEVENT       "Event"
#define AGENTMESSAGEHEADER      "Header"

#define STATUSFILE              "/tmp/status"

#define STARTREQUEST        "StartRequest"
#define SETUPREQUEST        "SetupRequest"
#define STATUSREQUEST       "GetDeviceStatusRequest"
#define GETCONFIGREQUEST    "GetDeviceConfigRequest"
#define SETCONFIGREQUEST    "SetDeviceConfigRequest"
#define LOGINREQUEST        "DeviceLoginRequest"

#define REBOOTDEVICEREQUEST             "RebootDeviceRequest"
#define REVERTTODEFAULTDEVICEREQUEST    "RevertToDefaultDeviceRequest"
#define SHUTDOWNDEVICEREQUEST           "ShutdownDeviceRequest"
#define SAVECONFIGDEVICEREQUEST         "SaveConfigDeviceRequest"
#define UPGRADEDEVICEREQUEST         	"UpgradeDeviceRequest"

#define SOURCEEVENT         "SourceEvent"
#define RELAYEVENT          "RelayEvent"
#define MEDIAPRESENCEEVENT  "MediaPresenceEvent"



#define LOGINHANDLER                "/etc/agent/scripts/deviceloginrequest.sh"
#define GETCONFIGHANDLER            "/etc/agent/scripts/getconfighandler.sh"
#define SETCONFIGHANDLER            "/etc/agent/scripts/setconfighandler.sh"
#define STATUSHANDLER               "/etc/agent/scripts/statusresponse.sh"
#define STARTHANDLER                "/etc/agent/scripts/startresponse.sh"
#define STARTRESPONSEHANDLER        "/etc/agent/scripts/startcomplete.sh"
#define SETUPHANDLER                "/etc/agent/scripts/setupresponse.sh"
#define LOGINRESPONSEHANDLER        "/etc/agent/scripts/loginresponse.sh"
#define RELAYEVENTHANDLER           "/etc/agent/scripts/handleRelayEvent.sh"
#define MEDIAPRESENCEEVENTHANDLER   "/etc/agent/scripts/handleMediaPresenceEvent.sh"
#define SOURCEEVENTHANDLER          "/etc/agent/scripts/handleSourceEvent.sh"
#define DEVICECMDHANDLER            "/etc/agent/scripts/handleDeviceCommand.sh"
#define DEVICEUPGRADEHANDLER		"/etc/agent/scripts/handleDeviceUpgradeCommand.sh"

#define MESSAGEINDIR   "/var/www/in/"
#define MESSAGEOUTDIR   "/var/www/out/"

#define MAXJIDLEN       80
#define FILENAMESIZE    80
#define CMDNAMESIZE     255
#define REQUESTNAMESIZE 80
#define EVENTNAMESIZE   80
#define SUCCESS         "8"

#define SYSLOG_INFO(...) syslog (LOG_LOCAL5 | LOG_INFO, __VA_ARGS__)

char *loginfile;
char *messagefile;
char *jid, *pass, *server;

int bConnected=0;
int bLoggedIn=0;
int bDisconnected=0;
int bProcessing=0;
int bSentLoginRequest=0;

typedef struct xml_parser_userdata
{
    xmpp_stanza_t *root;
    xmpp_stanza_t *current;
    char          *text;
    int depth;
} xml_parser_userdata_t;


xmpp_stanza_t* do_service_login(xmpp_ctx_t *);
int get_message (char *, char *,int);
int barco_xmpp_send_from_file (xmpp_conn_t *, char *);

char *tojid=NULL;


int version_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
	xmpp_stanza_t *reply, *query, *name, *version, *text;
	char *ns;
	xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;
	printf("Received version request from %s\n", xmpp_stanza_get_attribute(stanza, "from"));
	
	reply = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(reply, "iq");
	xmpp_stanza_set_type(reply, "result");
	xmpp_stanza_set_id(reply, xmpp_stanza_get_id(stanza));
	xmpp_stanza_set_attribute(reply, "to", xmpp_stanza_get_attribute(stanza, "from"));
	
	query = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(query, "query");
    ns = xmpp_stanza_get_ns(xmpp_stanza_get_children(stanza));
    if (ns) {
        xmpp_stanza_set_ns(query, ns);
    }

	name = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(name, "name");
	xmpp_stanza_add_child(query, name);
	
	text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, "libstrophe example bot");
	xmpp_stanza_add_child(name, text);
	
	version = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(version, "version");
	xmpp_stanza_add_child(query, version);
	
	text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, "1.0");
	xmpp_stanza_add_child(version, text);
	
	xmpp_stanza_add_child(reply, query);

	xmpp_send(conn, reply);
	xmpp_stanza_release(reply);
	return 1;
}


int presence_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    char *buf;
    unsigned int buflen=0;
    char *tojid, *type;
    xmpp_stanza_t *reply;
    xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;

    
    tojid = strdup (xmpp_stanza_get_attribute(stanza, "from"));
    type = strdup (xmpp_stanza_get_attribute(stanza, "type"));
	SYSLOG_INFO(" DEBUG: Got a Presence");

    if (tojid != NULL) {
	    fprintf(stderr, "From %s ", tojid);
    }
    if (type != NULL) {
	    fprintf(stderr, "From %s ", type);
    }
	fprintf(stderr, "\n");

    xmpp_stanza_to_text(stanza, &buf, &buflen);
    if (buflen > 0)
	    fprintf(stderr, "DEBUG: %s\n", buf);

    if (strcmp (type, "subscribe")==0) {
        reply = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(reply, "presence");
        xmpp_stanza_set_attribute(reply, "to", tojid);
        xmpp_stanza_set_attribute(reply, "type", "subscribed");
        xmpp_send(conn, reply);
        xmpp_stanza_release(reply);
    }

    free(tojid);
    free(type);
    free(buf);

	return 1;
}
int message_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
	xmpp_stanza_t *AgentMessage=NULL, *BarcoExtension=NULL, 
        *Request=NULL, *Response=NULL, *Header=NULL, *Event=NULL;
    xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;
    char *buf=NULL, *AgentMessageType=NULL;
    char *response=NULL;
    unsigned int buflen=0, len=0;
    int fd;
    char sInFileName[FILENAMESIZE+1],sOutFileName[FILENAMESIZE+1];
    static int  ctr=0;
    char sRequestName[REQUESTNAMESIZE+1];
    char sEventName[EVENTNAMESIZE+1];
    char sState[REQUESTNAMESIZE+1];
    char cmd[CMDNAMESIZE+1];
    FILE *FPStatus=NULL;

    bProcessing=1;

    BarcoExtension = xmpp_stanza_get_child_by_name(stanza, "x");
    if (BarcoExtension != NULL) {
        AgentMessage = xmpp_stanza_get_child_by_name(BarcoExtension, "AgentMessage");
        if (AgentMessage != NULL) {
            AgentMessageType = xmpp_stanza_get_attribute (
                    AgentMessage, AGENTMESSAGETYPE);
            if (AgentMessageType != NULL) {
                if (strcmp (AgentMessageType, AGENTMESSAGEREQUEST) ==0) {
                    Request = xmpp_stanza_get_child_by_name(
                            AgentMessage, AGENTMESSAGEREQUEST);
                }
                else if (strcmp (AgentMessageType, AGENTMESSAGERESPONSE) ==0) {
                    Response = xmpp_stanza_get_child_by_name(
                            AgentMessage, AGENTMESSAGERESPONSE);
                }
                else if (strcmp (AgentMessageType, AGENTMESSAGEEVENT) ==0) {
                    Event = xmpp_stanza_get_child_by_name(
                            AgentMessage, AGENTMESSAGEEVENT);
                }
                else {
	                SYSLOG_INFO("DEBUG: Agent Message %s", AgentMessageType);
                }
                if (Response != NULL) {
                    Header = xmpp_stanza_get_child_by_name(
                            Response, AGENTMESSAGEHEADER);
                    if (Header != NULL) {
                        strcpy (sState, 
                                xmpp_stanza_get_attribute(Header, "state"));
                        strcpy (sRequestName, 
                                xmpp_stanza_get_attribute(Header, "requestName"));
			
                    }
                    if (strcmp (sRequestName, LOGINREQUEST) ==0) {
                        // Now update the status file
	                    SYSLOG_INFO(" Response for  %s  State %s", 
                                sRequestName, sState);
                        if (strcmp(sState, SUCCESS) == 0) {
                            FPStatus = fopen (STATUSFILE, "w");
                            fprintf (FPStatus, "<serverIP>%s</serverIP>\n",
                                    server);
                            fclose(FPStatus);
                            bLoggedIn=1;
                        }
                    }
                    else if (strcmp (sRequestName, STARTREQUEST) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  State %s Length %d", 
                                AgentMessageType, sRequestName, sState, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s", 
                                STARTRESPONSEHANDLER,
                                sInFileName, sOutFileName);

	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            xmpp_free(ctx, buf);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else {
	                    SYSLOG_INFO(" Unhandled %s  %s  State %s", 
                                AgentMessageType, sRequestName, sState);
                    }
                }
                else if (Request != NULL) {
                    Header = xmpp_stanza_get_child_by_name(
                            Request, AGENTMESSAGEHEADER);
                    if (Header != NULL) {
                        strcpy (sRequestName, 
                                xmpp_stanza_get_attribute(Header, "requestName"));
                    }
                    if (strcmp (sRequestName, STARTREQUEST) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sRequestName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            xmpp_free(ctx, buf);

                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s", 
                                STARTHANDLER,
                                sInFileName, sOutFileName);

	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            barco_xmpp_send_from_file (conn, sOutFileName);

                            unlink(sInFileName);
                            sprintf (sInFileName, "%s", sOutFileName);

                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s", 
                                STARTRESPONSEHANDLER,
                                sInFileName, sOutFileName);
	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else if (strcmp (sRequestName, SETUPREQUEST) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sRequestName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            xmpp_free(ctx, buf);

                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s", 
                                SETUPHANDLER,
                                sInFileName, sOutFileName);
	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);

                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else if ( 
                    (strcmp (sRequestName, REBOOTDEVICEREQUEST)==0) || 
                    (strcmp (sRequestName, REVERTTODEFAULTDEVICEREQUEST)==0) ||
                    (strcmp (sRequestName, SHUTDOWNDEVICEREQUEST)==0) ||                    
                    (strcmp (sRequestName, SAVECONFIGDEVICEREQUEST)==0)) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sRequestName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s %s", 
                                DEVICECMDHANDLER,
                                sInFileName, sOutFileName, sRequestName);
	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            xmpp_free(ctx, buf);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else if (strcmp (sRequestName, STATUSREQUEST) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sRequestName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr);
                            sprintf (cmd, "%s %s %s", STATUSHANDLER,
                                sInFileName, sOutFileName);
	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            xmpp_free(ctx, buf);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else if (strcmp (sRequestName, GETCONFIGREQUEST) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sRequestName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr);
                            sprintf (cmd, "%s %s %s", GETCONFIGHANDLER,
                                sInFileName, sOutFileName);
	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            xmpp_free(ctx, buf);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else if (strcmp (sRequestName, SETCONFIGREQUEST) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sRequestName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr);
                            sprintf (cmd, "%s %s %s", SETCONFIGHANDLER,
                                sInFileName, sOutFileName);
	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            xmpp_free(ctx, buf);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
		else if (strcmp (sRequestName, UPGRADEDEVICEREQUEST) ==0) {			
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sRequestName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr);
                            sprintf (cmd, "%s %s %s", DEVICEUPGRADEHANDLER,
                                sInFileName, sOutFileName);
	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            xmpp_free(ctx, buf);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                }
                
                else if (Event != NULL) {
                    Header = xmpp_stanza_get_child_by_name(
                            Event, AGENTMESSAGEHEADER);
                    if (Header != NULL) {
                        strcpy (sEventName, 
                                xmpp_stanza_get_attribute(Header, "eventName"));
                    }
                    if (strcmp (sEventName, RELAYEVENT) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sEventName, buflen);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            xmpp_free(ctx, buf);
                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s", 
                                RELAYEVENTHANDLER,
                                sInFileName, sOutFileName);

	                        SYSLOG_INFO(" Command %s \n", cmd);
                            system (cmd);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else if (strcmp (sEventName, MEDIAPRESENCEEVENT) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sEventName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            xmpp_free(ctx, buf);

                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s", 
                                MEDIAPRESENCEEVENTHANDLER,
                                sInFileName, sOutFileName);

	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else if (strcmp (sEventName, SOURCEEVENT) ==0) {
                        xmpp_stanza_to_text(stanza, &buf, &buflen);
	                    SYSLOG_INFO(" %s  %s  Length %d", 
                                AgentMessageType, sEventName, buflen);
                        sprintf (sInFileName, "%s%d.xml", MESSAGEINDIR, ctr++);
                        fd = open (sInFileName, O_CREAT|O_WRONLY|O_TRUNC);
                        if (fd > 0) {
                            write (fd, buf, buflen);
                            close(fd);
                            xmpp_free(ctx, buf);

                            sprintf (sOutFileName, "%s%d.xml", 
                                MESSAGEOUTDIR, ctr++);
                            sprintf (cmd, "%s %s %s", 
                                SOURCEEVENTHANDLER,
                                sInFileName, sOutFileName);

	                        SYSLOG_INFO(" Command %s ", cmd);
                            system (cmd);
                            barco_xmpp_send_from_file (conn, sOutFileName);
                            unlink(sOutFileName);
                            unlink(sInFileName);
                        }
                        else {
	                        SYSLOG_INFO(" Could not write %s ", buf);
                        }
                    }
                    else {
	                    SYSLOG_INFO(" Unhandled %s  %s ", 
                                AgentMessageType, sEventName);
                    }
                }
            }
            else {
	            SYSLOG_INFO("DEBUG: Unknown Agent Message");
            }
        }
        else {
	        SYSLOG_INFO("DEBUG: Got an Barco Message %s\n",
                    xmpp_stanza_get_ns(BarcoExtension));
        }
    }
    else {
        xmpp_stanza_to_text(stanza, &buf, &buflen);
	    SYSLOG_INFO("DEBUG: Got an XMPP Message\n");
    }

	
	if(!xmpp_stanza_get_child_by_name(stanza, "x")) {
        bProcessing=0;
        return 1;
    }

	
    if (tojid != NULL) {
        free(tojid);
        tojid = NULL;
    }
    bProcessing=0;
	return 1;
}

/* define a handler for connection events */
void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status, 
		  const int error, xmpp_stream_error_t * const stream_error,
		  void * const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
    char *devicelogin=NULL;
    int len=0;

    if (status == XMPP_CONN_CONNECT) {
	    xmpp_stanza_t* pres;
	    SYSLOG_INFO("DEBUG: connected\n");
	    xmpp_handler_add(conn,version_handler, "jabber:iq:version", "iq", NULL, ctx);
	    xmpp_handler_add(conn,message_handler, NULL, "message", NULL, ctx);
    
	    xmpp_handler_add(conn,presence_handler, NULL, "presence", NULL, ctx);
	
	    /* Send initial <presence/> so that we appear online to contacts */
	    pres = xmpp_stanza_new(ctx);
	    xmpp_stanza_set_name(pres, "presence");
	    xmpp_send(conn, pres);
	    xmpp_stanza_release(pres);
        bConnected=1;
    }
    else {
        bLoggedIn=0;
        bSentLoginRequest=0;
        bConnected=0;
        bDisconnected=1;
	    SYSLOG_INFO("DEBUG: disconnected\n");
	    xmpp_stop(ctx);
    }
}
int bQuit = 0;
int bRestart = 0;

void sighandler (int sig)
{
    switch (sig) { 
        case SIGTERM:
            bQuit=1;
            break;
        case SIGHUP:
            bRestart=1;
            break;
        default:
            fprintf (stderr, "Process %d ignoring signal %d", 
                    getpid(), sig);
            break;
    }
}

int main(int argc, char **argv)
{
    xmpp_ctx_t *ctx;
    xmpp_conn_t *conn;
    xmpp_log_t *log;
    xmpp_stanza_t *space, *disco, *query;
    unsigned long timeout= 1000;
    struct stat fileStatNow, fileStatPrev;
    FILE *FPLogin=NULL;
    char cmd[CMDNAMESIZE+1];
    struct timeval tlogin, tnow, tprev;
    int ret;
    int reg;

    jid = (char *) malloc(MAXJIDLEN+1);
    pass = (char *) malloc(MAXJIDLEN+1);
    server = (char *) malloc(MAXJIDLEN+1);


    signal (SIGPIPE, sighandler);
/*
    signal (SIGPIPE, sighandler);
    signal (SIGTTOU, SIG_IGN);
    signal (SIGCHLD, SIG_IGN);
    signal (SIGINT, sighandler);
    signal (SIGTERM, sighandler);

    daemon(1,0);
*/
    signal (SIGQUIT, sighandler);
    signal (SIGHUP, sighandler);



    /* take a jid and password on the command line */
    if (argc != 3) {
	    fprintf( stderr, "Usage: agent <loginfile> <deviceloginfile>\n\n");
	    return 1;
    }
    loginfile = strdup(argv[2]);


    /* create a context */
    while(1) {
        bDisconnected=0;
        if (stat (argv[1], &fileStatPrev) < 0) {
            sleep(5);
            continue;
        }
        if (stat (argv[1], &fileStatPrev) < 0) {
            sleep(5);
            continue;
        }
        /* init library */
        xmpp_initialize();
        log = xmpp_get_default_logger(XMPP_LEVEL_INFO); /* pass NULL instead to silence output */
        sprintf (cmd, "%s %s", LOGINHANDLER, loginfile);
        FPLogin = popen (cmd, "r");
        if (FPLogin != NULL) {
            fscanf (FPLogin,"%s", jid);
            fscanf (FPLogin,"%s", pass);
            fscanf (FPLogin,"%s", server);
            fscanf (FPLogin,"%d", &reg);
            pclose(FPLogin);
        }

        ctx = xmpp_ctx_new(NULL, log);

        /* create a connection */
        conn = xmpp_conn_new(ctx);

        /* setup authentication information */
        xmpp_conn_set_jid(conn, jid);
        xmpp_conn_set_pass(conn, pass);
        if (reg) {
            printf ("Now invoking registration\n");
            xmpp_conn_set_registration(conn, 1);
        }

        /* initiate connection */
        xmpp_connect_client(conn, NULL, 0, conn_handler, ctx);

        /* enter the event loop - 
        our connect handler will trigger an exit */
        gettimeofday (&tprev, NULL);
        gettimeofday (&tnow, NULL);
        while(1) {
            if (stat (argv[1], &fileStatNow) < 0) {
                break;
            }
            if (fileStatNow.st_mtime > fileStatPrev.st_mtime) {
                SYSLOG_INFO ("Detected an update\n");
                bConnected=0;
                stat (argv[1], &fileStatPrev);
                break;
            }
            if (bDisconnected) break;
            ret = xmpp_run_once(ctx, timeout);
            if (ret > 0) {
                timeout=100;
            }
            else {
                timeout=1000;
            }

            if (!bProcessing) {
                if (bLoggedIn) {
                    gettimeofday (&tnow, NULL);
                    if (tnow.tv_sec - tprev.tv_sec >  5) {
                        space = xmpp_stanza_new (ctx);
                        xmpp_stanza_set_text(space, " ");
                        xmpp_send(conn, space);
                        xmpp_stanza_release(space);
                        gettimeofday (&tprev, NULL);
                    }
                }
                else if ((bConnected) && (!bSentLoginRequest)) {
                    //Wait 20 seconds before sending the next login request
                    barco_xmpp_send_from_file (conn, loginfile);
                    bSentLoginRequest=1;
	                SYSLOG_INFO("Sending Login Request\n");
                    gettimeofday (&tlogin, NULL);
                }
                else if (bConnected) {
                    // We have waited long enough now disconnect
                    if (!bLoggedIn) {
                        gettimeofday (&tnow, NULL);
                        if (tnow.tv_sec - tlogin.tv_sec > 
                                XMPP_TRANSACTION_TIMEOUT) {
                            SYSLOG_INFO ("Device Login Failed %d\n",
                                    tnow.tv_sec - tlogin.tv_sec);
                            bSentLoginRequest=0;
                        }
                    }
                }
            }
        }
        unlink (STATUSFILE);

	    xmpp_stop(ctx);

        /* release our connection and context */
        conn_disconnect(conn);
        xmpp_conn_release(conn);
        xmpp_ctx_free(ctx);
        if (bQuit) break;
        SYSLOG_INFO ("Now sleeping for 5 sec\n");
        sleep(5);
        xmpp_shutdown();
        SYSLOG_INFO ("Disconnected\n");
    }

    /* final shutdown of the library */

    return 0;
}
void handle_data (void *userData, const char *content, int length)
{
  xml_parser_userdata_t *udata; 
  udata = (xml_parser_userdata_t *) userData;

  if (length < MAXXMLTEXTDATA) {
      strncpy (udata->text, content,length); 
      udata->text[length] = '\0';
  }
  else {
      // flag an error message
      udata->text[0] = '\0';
  }
}

int barco_xmpp_send_from_file(xmpp_conn_t * conn, char *file) {
    int retval=0; 
    char *response=NULL;
    struct stat st;
    FILE *FPInput = NULL; 
    if (stat (file, &st) == 0) {
        response = (char *) malloc (st.st_size);
        FPInput = fopen (file, "r"); 
        if (FPInput) { 
            retval = fread (response, 1, st.st_size, FPInput); 
            fclose(FPInput); 
            xmpp_send_raw (conn, response, st.st_size);
            free(response);
            retval=0;
        }
    }
    return retval;
}
int get_message( char *loginfile, char *loginbuffer, int len) {
  int retval=0;
  FILE *FPInput = NULL; 
  FPInput = fopen (loginfile, "r"); 
  if (FPInput) {
    retval = fread (loginbuffer, 1, len, FPInput);
    fclose(FPInput);
  }
  return retval;
}
