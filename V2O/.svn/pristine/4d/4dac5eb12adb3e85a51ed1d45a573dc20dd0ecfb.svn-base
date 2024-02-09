#ifndef __LOAD_NET_H__
#define __LOAD_NET_H__

#include "mylist.h"

#define NET_CONFIG_FILE "/V2O/config/netconf.txt"
#define ROUTE_CONFIG_FILE "/V2O/config/routeconf.txt"
#define DESCRIPTOR_CONFIG_FILE  "/V2O/config/edidlistconf.txt"
#define SERVERLIST_CONFIG_FILE "/V2O/config/serverlistconf.txt"
#define AUTHLIST_CONFIG_FILE "/V2O/config/authlistconf.txt"
#define DHCP_CONFIG_FILE "/etc/udhcpc/resolv.conf"
#define DNS_CONFIG_FILE "/etc/resolv.conf"
#define RULELIST_CONFIG_FILE    "/V2O/config/rulelistconf.txt"
#define KERNEL_ROUTE_FILE       "/proc/net/route"

#define INET_ADDR_LEN           16
#define HOSTNAME_LEN            32
#define ROOTLOGIN_LEN           32
#define SERVERNAME_LEN          32
#define DOMAINNAME_LEN          32
#define INET_NAME_LEN           32
#define BANDWIDTH_LEN           8

struct net_entry {
    int                 enable_dhcp;
    char                ethip[INET_ADDR_LEN + 1];
    char                ethmask[INET_ADDR_LEN + 1];
    char                dnsservip[INET_ADDR_LEN + 1];
    char                localdomain[DOMAINNAME_LEN + 1];
    char                flanip[INET_ADDR_LEN + 1];
    char                flanmask[INET_ADDR_LEN + 1];
    char                hostname[HOSTNAME_LEN + 1];
    int                 enable_telnet;
    int                 enable_hello;
};

struct ssh_entry {
    int                 enable_ssh;
    int                 rootlogin;
    int                 empty_pass;
    int                 keyregen;
    int                 allow_compression;
    int                 gracetime;
    int                 allow_pkauth;
    int                 enable_sftp;
};

struct route_entry {
    int                 routeid;
    char                destination[INET_ADDR_LEN + 1];
    char                netmask[INET_ADDR_LEN + 1];
    char                gateway[INET_NAME_LEN + 1];
    struct list_head    route_list;
};


#define AVOPTION_VIDEO          1
#define AVOPTION_AUDIO          2
#define AVOPTION_AUDIO_VIDEO    3

#define PHP_BIN "/home/onair/apache/bin/php"

#define PHP_STRMSRVCMD "/etc/scripts/Start_StrmSrv.php"
#define PHP_STRMSRVCMDRX "/etc/scripts/Start_StrmSrvRx.php"


#define LOCAL_IP "127.0.0.1"
#define DECPORT_ID 1000
#define ENCPORT_ID 1001
#define SS_PORT 10001


struct serverlist_entry {
    int                 serverid;
    char                servername[SERVERNAME_LEN + 1];
    int                 serverport;
    char                bandwidth[BANDWIDTH_LEN + 1];
    int                 multicast;
    int                 avoption;
    int                 bidir;
    struct list_head    serverlist_list;
};

struct authlist_entry {
    int                 authid;
    char                network[INET_ADDR_LEN + 1];
    char                netmask[INET_ADDR_LEN + 1];
    int                 enable;
    char                max_bandwidth[BANDWIDTH_LEN + 1];
    int                 min_compression;
    int                 max_compression;
    float               burst_ratio;
    int                 burst_duration;
    int                 unicast;
    int                 multicast_init;
    int                 multicast_part;
    int                 unicast_preemptible;
    struct list_head    authlist_list;
};

struct edidlist_entry {
    int                 edidid;
    int                 block;
    int                 lutid;
    int                 hspol;
    int                 vspol;
    struct list_head    edidlist_list;
};


struct rulelist_entry {
    int                 rule_id;
    int                 rule_hwm;
    int                 rule_lwm;
    int                 low_comp_delta;
    int                 high_comp_delta;
    int                 divider_delta;
    int                 rule_fpga_drop;
    struct list_head    rulelist_list;
};


/***********************
 * Function prototpyes *
 ***********************/
extern int parse_net(struct net_entry *netentryp, const char *filename);
extern void print_net(struct  net_entry *netentryp, FILE *fp);
extern int save_net(struct net_entry *netentryp);
extern int load_net(struct net_entry *netentryp);
extern int get_dns_config(char *dns, char *domain);

extern int parse_ssh(struct ssh_entry *sshentryp, const char *filename);
extern void print_ssh(struct  ssh_entry *sshentryp, FILE *fp);
extern int save_ssh(struct ssh_entry *sshentryp);
extern int apply_ssh(struct ssh_entry *sshentryp);

extern int count_routes(struct list_head *route_head);
extern int parse_kernel_routes(struct list_head *route_head);
extern int parse_routes(struct list_head *route_head, const char *filename);
extern int save_routes(struct list_head *route_head);
extern void print_route(struct route_entry *routeentryp, int count, FILE *fp);
extern int check_route(struct route_entry *routeentryp,
                       struct list_head *route_head,
                       int route_index);
extern int apply_route(const char *destination,
                       const char *netmask,
                       const char *gateway);
extern int remove_route(const char *destination,
                        const char *netmask,
                        const char *gateway);
extern void free_route(struct list_head *route_head);



extern int count_serverlist(struct list_head *serverlist_head);
extern int parse_serverlist(struct list_head *serverlist_head);
extern int save_serverlist(struct list_head *serverlist_head);
extern void free_serverlist(struct list_head *serverlist_head);

extern int count_authlist(struct list_head *authlist_head);
extern int parse_authlist(struct list_head *authlist_head);
extern int save_authlist(struct list_head *authlist_head);
extern void free_authlist(struct list_head *authlist_head);

extern int count_rulelist(struct list_head *rulelist_head);
extern int parse_rulelist(struct list_head *rulelist_head);
extern int save_rulelist(struct list_head *rulelist_head);
extern void free_rulelist(struct list_head *rulelist_head);

extern int get_unique_authid(struct list_head *authlist_head);
extern int get_unique_serverid(struct list_head *serverlist_head);
extern int get_unique_routeid(struct list_head *route_head);
extern int get_unique_ruleid(struct list_head *rule_head);

extern int count_edidlist(struct list_head *edidlist_head);
extern int parse_edidlist(struct list_head *edidlist_head);
extern int save_edidlist(struct list_head *edidlist_head);
extern void free_edidlist(struct list_head *edidlist_head);
extern int find_block_in_edidlist(struct list_head *edidlist_head, int block);

extern int count_rulelist(struct list_head *rulelist_head);
extern int parse_rulelist(struct list_head *rulelist_head);
extern int save_rulelist(struct list_head *rulelist_head);
extern void free_rulelist(struct list_head *rulelist_head);
extern int get_unique_ruleid(struct list_head *rule_head);

extern int count_edidlist(struct list_head *edidlist_head);
extern int parse_edidlist(struct list_head *edidlist_head);
extern int save_edidlist(struct list_head *edidlist_head);
extern void free_edidlist(struct list_head *edidlist_head);
extern int find_block_in_edidlist(struct list_head *edidlist_head, int block);
extern int get_unique_edidid(struct list_head *edidlist_head);

void send_serverdecport_discallupdate(void);
int send_server_startall(void);
int send_server_stopall(void);

void WriteHostFile(int req_code);


#endif /* __LOAD_NET_H__ */
