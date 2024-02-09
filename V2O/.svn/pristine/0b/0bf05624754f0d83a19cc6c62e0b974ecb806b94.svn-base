/*
 * xmpp_agent_menu.h
 *
 *  Created on: Dec 15, 2009
 *      Author: rkale
 */

#ifndef XMPP_AGENT_MENU_H_
#define XMPP_AGENT_MENU_H_

#define XMPP_AGENT_CONFIG_SERVERIP_LEN   32
#define XMPP_AGENT_CONFIG_DEVICENAME_LEN   32

struct xmpp_agent_config {
    int enable;
    char serverip[XMPP_AGENT_CONFIG_SERVERIP_LEN + 1];
    char devicename[XMPP_AGENT_CONFIG_DEVICENAME_LEN + 1];
};

#ifdef __cplusplus
extern "C" {
#endif

extern int get_xmpp_agent_config(struct xmpp_agent_config *xmpp_agent_cfg);

#ifdef __cplusplus
}
#endif

#endif /* XMPP_AGENT_MENU_H_ */
