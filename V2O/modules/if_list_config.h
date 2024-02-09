/*
 * if_list_config.h
 *
 *  Created on: Oct 6, 2009
 *      Author: rkale
 */

#ifndef IF_LIST_CONFIG_H_
#define IF_LIST_CONFIG_H_

#define MAX_NUMBER_OF_INTERFACES    8

struct if_list_interface {
    int port;
    int enable;
    int primary;
    int multicast;
    int usedhcp;
    char ipaddress[32];
    char netmask[32];

};

struct if_list_config {
    int number_of_interfaces;
    struct if_list_interface interface[MAX_NUMBER_OF_INTERFACES];
};

#ifdef __cplusplus
extern "C" {
#endif

extern int get_if_list_config(struct if_list_config *if_list_cfg);
extern int is_dhcp_enabled(void);
extern int is_primary_interface(char * interface);

#ifdef __cplusplus
}
#endif

#endif /* IF_LIST_CONFIG_H_ */
