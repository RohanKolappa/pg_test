#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <dirent.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/in.h>
#include <signal.h>

#include "hal.h"
#include "hal_console.h"
#include "hal_hwinfo.h"
#include "kbm_control.h"
#include "vutils.h"
#include "loadnet.h"
#include "custmenu.h"
#include "configureSystem.h"
#include "configureVideo.h"
#include "commonutils.h"
#include "text_utils.h"
#include "system_utils.h"
#include "configfile_parser.h"
#include "serverutils.h"
#include "vlog.h"
#include "generatedHeaders.h"

#define TMPFILE "/tmp/hosts"
#define HOSTSFILE "/etc/hosts"

#define SSH_KEYGEN "/usr/bin/ssh-keygen"
#define SSH_SSHD "/usr/sbin/sshd"
#define SSH_SFTP "/usr/libexec/sftp-server"
#define SSH_CONFIG_DIR "/V2O/config"
#define RSA1_KEY_FILE "/V2O/openssh/etc/ssh_host_key"
#define RSA_KEY_FILE "/V2O/openssh/etc/ssh_host_rsa_key"
#define DSA_KEY_FILE "/V2O/openssh/etc/ssh_host_dsa_key"
#define LASTLOG_FILE "/var/log/lastlog"

#define S_DHCP          "DHCP"
#define S_ETHERIP       "ETHERIP"
#define S_ETHERMASK     "ETHERMASK"
#define S_DNSSERVIP     "DNSSERVIP"
#define S_LOCALDOMAIN   "LOCALDOMAIN"
#define S_FLANIP        "FLANIP"
#define S_FLANMASK      "FLANMASK"
#define S_HOSTNAME      "HOSTNAME"
#define S_TELNET        "TELNET"
#define S_HELLO         "HELLO"
#define S_DOMAIN        "domain"
#define S_NAMESERVER    "nameserver"

#define S_ROUTEID       "ROUTE_ID"
#define S_DESTINATION   "DESTINATION"
#define S_AUTHID        "AUTHID"
#define S_NETMASK       "NETMASK"
#define S_GATEWAY       "GATEWAY"

#define S_SSH           "SSH"
#define S_PRIVSEP       "UsePrivilegeSeparation"
#define S_ROOTLOGIN     "PermitRootLogin"
#define S_EMPTYPASS     "PermitEmptyPasswords"
#define S_KEYREGEN      "KeyRegenerationInterval"
#define S_CIPHERS       "Ciphers"
#define S_COMPRESSION   "Compression"
#define S_GRACETIME     "LoginGraceTime"
#define S_PKAUTH        "PubkeyAuthentication"
#define S_SUBSYSTEM     "Subsystem"

#define S_DESCRIPTOR_EDIDID      "DESCRIPTOR_EDIDID"
#define S_DESCRIPTOR_BLOCK       "DESCRIPTOR_BLOCK"
#define S_DESCRIPTOR_LUTID       "DESCRIPTOR_LUTID"
#define S_DESCRIPTOR_HSPOL       "DESCRIPTOR_HSPOL"
#define S_DESCRIPTOR_VSPOL       "DESCRIPTOR_VSPOL"


#define S_COMMENT       "#*"

char *rootloginlist[] = { "Yes",
    "Without-password",
    "Forced-commands-only",
    "No"
};
int rootloginlist_size = sizeof(rootloginlist) / sizeof(char *);

int verbose_errors = 0;         /* Used to prevent messages during menu
                                   operations */

static int wait_for_dns_config(void);
static int write_dns_config(struct net_entry *netentryp);

/***********************
 * Print help messages *
 ***********************/
static void print_help(void)
{
    printf("\nUsage: loadnet [-rpsD] [-f File]\n\n");
    printf("Loads or Displays Network or Route configuration.\n\n");
    printf("Options:\n");
    printf("\t-r\t Load/Display Route configuraion (else do Network)\n");
    printf("\t-k\t Displays Kernel IP Routes\n");
    printf("\t-v\t Load/Display Video configuraion (else do Network)\n");
    printf("\t-p\t Display only (do not Load)\n");
    printf("\t-s\t Save Network config to file again after parsing\n");
    printf("\t-f File\t Parse \"File\" instead of default file\n");
    printf("\t-x\t Start XMPP Agent \n");
    printf("\t-o\t Start XMPP Server\n");
    printf("\t-D\t Delete all routes in config file (dont change file)\n");
}


/*****************************************
 * Main entry point load network utility *
 *****************************************/
int loadnet_main(int argc, char **argv)
{
    int opt;
    int parse_only = 0;
    int do_routes = 0;
    int do_kernel_routes = 0;
    int do_xmpp_server = 0;
    int do_xmpp_agent = 0;
    int save_file = 0;
    int delete_routes = 0;
    const char *filename = NULL;
    int retcode, currentidx;
    struct net_entry netentry;

    struct route_entry *routeentryp;
    struct list_head route_list_head, *tmp;

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "rkpf:sDh")) > 0) {

        switch (opt) {
        case 'x':
            do_xmpp_agent = 1;
            break;
        case 'o':
            do_xmpp_server = 1;
            break;
        case 'p':
            parse_only = 1;
            break;
        case 'r':
            do_routes = 1;
            break;
        case 'k':
            do_kernel_routes = 1;
            break;
        case 'f':
            filename = optarg;
            break;
        case 's':
            save_file = 1;
            break;
        case 'D':
            delete_routes = 1;
            break;
        case '?':
        case 'h':
            print_help();
            return -1;
        }
    }

    if (do_kernel_routes) {
        int count;
        struct route_entry *routeentryp;
        struct list_head *tmp;

        /*****************************************
         * Print the kernel IP routing table     *
         *****************************************/
        verbose_errors = 1;
        count = parse_kernel_routes(&route_list_head);

        if (count < 0 && verbose_errors) {
            printf("Error processing kernel IP routes\n");
            return -1;
        }

        printf("  ID      Destination     Network Mask         Gateway\n");

        list_for_each(tmp, &route_list_head) {
            char destination[INET_ADDR_LEN + 1];
            char gateway[INET_ADDR_LEN + 1];
            routeentryp = list_entry(tmp, struct route_entry, route_list);
            if (strcasecmp(routeentryp->destination, "0.0.0.0") == 0)
                strcpy(destination, "default");
            else
                strcpy(destination, routeentryp->destination);
            if (strcasecmp(routeentryp->gateway, "0.0.0.0") == 0)
                strcpy(gateway, "*");
            else
                strcpy(gateway, routeentryp->gateway);
            printf("%-4d %-16s %-16s %-16s\n", routeentryp->routeid,
                   destination, routeentryp->netmask, gateway);
        }
    }
    else if (do_routes) {
        /*****************************************
         * Read and parse the routes config file *
         *****************************************/
        verbose_errors = 1;
        retcode = parse_routes(&route_list_head, filename);

        if (retcode < 0) {
            printf("Error processing network config file\n");
            return -1;
        }

        if (!parse_only) {

            list_for_each(tmp, &route_list_head) {
                routeentryp =
                    list_entry(tmp, struct route_entry, route_list);
                if (!delete_routes)
                    apply_route(routeentryp->destination,
                                routeentryp->netmask,
                                routeentryp->gateway);
                else
                    remove_route(routeentryp->destination,
                                 routeentryp->netmask,
                                 routeentryp->gateway);

            }
        }
        else {
            currentidx = 1;
            list_for_each(tmp, &route_list_head) {
                routeentryp =
                    list_entry(tmp, struct route_entry, route_list);
                print_route(routeentryp, currentidx, NULL);
                currentidx++;
            }
        }
    }

#ifdef TODO // review all the initXXX code below does get applied
    else if (do_video) {

        /***********************************************************
         * Find out who we are and start either dserver or dclient *
         ***********************************************************/
        {
            int board_type;
            if (get_board_type(&board_type) < 0)
                board_type = HARDWARE_UNKNOWN;

            system_command("/usr/bin/killall dserver");
            system_command("/usr/bin/killall dclient");

            switch (board_type) {

            case BOARDTYPE_RX:
                initPixelAdjust();
                kbtype = getRxKeyboardType();
                printf("Setting keyboard type to %s.\n",
                       (kbtype == KB_TYPE_SGI) ? "SGI" : "PC");
                initKeyboardType(kbtype);
                initGenlockMode();

                printf("Starting Client Daemon...\n");
                retcode = system_command("/V2O/bin/dclient");
                if (retcode != 0) {
                    printf("Could not start Client Daemon\n");
                }
                break;

            case HARDWARE_DUMMY:
            case BOARDTYPE_TX:

                initVideoCompression();
                initHorizPos();
                initVertPos();
                initPhaseShift();
                initCoarseNoise();
                initFineNoise();
                initFrameDivider();
                initAudioClock();
                initFpgaEdidDescriptor();
                initQualAdjust();

                printf("Starting Server Daemon...\n");
                retcode = system_command("/V2O/bin/dserver");
                if (retcode != 0) {
                    printf("Could not start Server Daemon\n");
                }
                break;

            case HARDWARE_UNKNOWN:
                printf
                    ("Unknown hardware. Not starting sever or client.\n");
                return -1;
            }
        }

    }
#endif

    else if (do_xmpp_agent) {
        loadXMPPAgent();
    }
    else if (do_xmpp_server) {
        loadXMPPServer();
    }
    else {
        /******************************************
         * Read and parse the network config file *
         ******************************************/
        verbose_errors = 1;
        retcode = parse_net(&netentry, filename);

        if (retcode < 0) {
            printf("Error processing network config file\n");
            return -1;
        }

        if (!parse_only) {
            load_net(&netentry);
        }
        else {
            print_net(&netentry, NULL);
            printf("\nParsed %d parameters\n", retcode);

            if (save_file) {
                save_net(&netentry);
            }
        }
        /***********************************************
         * Add multicast route (this used to be in     *
         * custom_startup.sh but moved here since we   *
         * need to do it each time IP address changes. *
         ***********************************************/
        retcode =
            system_command
            ("/sbin/route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0");
        if (retcode < 0) {
            printf("Could not add multicast route\n");
        }

    }
    return 0;
}

void print_net(struct net_entry *netentryp, FILE * fp)
{

    int len;

    if (fp == NULL)
        fp = stdout;

    fprintf(fp, "######## Network Parameters ##########\n\n");
    len = fprintf(fp, "%s", S_DHCP);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            netentryp->enable_dhcp ? "Yes" : "No");
    len = fprintf(fp, "%s", S_ETHERIP);
    fprintf(fp, "%*s %s\n", 25 - len, " ", netentryp->ethip);
    len = fprintf(fp, "%s", S_ETHERMASK);
    fprintf(fp, "%*s %s\n", 25 - len, " ", netentryp->ethmask);
    len = fprintf(fp, "%s", S_DNSSERVIP);
    fprintf(fp, "%*s %s\n", 25 - len, " ", netentryp->dnsservip);
    len = fprintf(fp, "%s", S_LOCALDOMAIN);
    fprintf(fp, "%*s %s\n", 25 - len, " ", netentryp->localdomain);
    len = fprintf(fp, "%s", S_HOSTNAME);
    fprintf(fp, "%*s %s\n", 25 - len, " ", netentryp->hostname);
    len = fprintf(fp, "%s", S_TELNET);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            netentryp->enable_telnet ? "Yes" : "No");
    len = fprintf(fp, "%s", S_HELLO);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            netentryp->enable_hello ? "Yes" : "No");
}

/*****************************************
 * Save given network parameters to file *
 *****************************************/
int save_net(struct net_entry *netentryp)
{

    FILE *fp;

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(NET_CONFIG_FILE, "w+")) == NULL) {
        return -1;
    }

    print_net(netentryp, fp);

    fclose(fp);

    return 0;
}

/***********************************
 * Apply the network configuration *
 ***********************************/
int load_net(struct net_entry *netentryp)
{

    int retcode;
    char buff[128];
    struct in_addr in;
    int found;

    /* Start udhcpc if it's not currently running */
    found = find_proc_by_name("udhcpc");
    if (!found && netentryp->enable_dhcp) {
        // TODO: ensure we do not inherit currently open file handles.

        /* clear ip address etc they'll be set by DHCP */
        strcpy(netentryp->ethip, "0.0.0.0");
        strcpy(netentryp->ethmask, "0.0.0.0");
        strcpy(netentryp->dnsservip, "0.0.0.0");
        memset(netentryp->localdomain, '\0', DOMAINNAME_LEN + 1);

        if (verbose_errors)
            printf("Starting DHCP Client...\n");

        sprintf(buff, "/sbin/udhcpc -H %s ",
                netentryp->hostname);
        retcode = system_command(buff);
        if (retcode != 0) {
            if (verbose_errors)
                printf("Could not start udhcpc\n");
        }
        else {
            struct in_addr addr, mask;

            /************************************************
             * wait for dhcpc to create the DNS config file *
             ************************************************/
            if (verbose_errors)
                printf("Waiting for DNS config info...\n");

            retcode = wait_for_dns_config();

            if (retcode && get_if_info("eth0", &addr, &mask) >= 0) {
                strncpy(netentryp->ethip, inet_ntoa(addr), INET_ADDR_LEN);
                strncpy(netentryp->ethmask, inet_ntoa(mask),
                        INET_ADDR_LEN);
                get_dns_config(netentryp->dnsservip,
                               netentryp->localdomain);
            }
        }
    }
    else if (found && netentryp->enable_dhcp == 0) {
        /* If dhcp currently running, kill them */
        retcode = system_command("/usr/bin/killall udhcpc");
        if (retcode != 0 && verbose_errors)
            printf("Could not kill udhcpc\n");
    }

    /* Set Ether IP */
    in.s_addr = 0;
    retcode = inet_aton(netentryp->ethip, &in);
    if (retcode != 0 && in.s_addr != 0) {
        if (verbose_errors)
            printf("Setting Ethernet IP: %s mask: %s.\n",
                   netentryp->ethip, netentryp->ethmask);
        sprintf(buff, "/sbin/ifconfig eth0 %s netmask %s",
                netentryp->ethip, netentryp->ethmask);
        retcode = system_command(buff);
        if (retcode != 0 && verbose_errors) {
            printf("Could not set Ethernet IP.\n");
        }
    }

    /* Set DNS server IP & local domain name */
    if (netentryp->enable_dhcp == 0) {
        retcode = write_dns_config(netentryp);
        if (retcode != 0 && verbose_errors) {
            printf("Could not write DNS config file\n");
        }
    }

    /* Start telnetd if it's not currently running  */
    found = find_proc_by_name("telnetd");
    if (!found && netentryp->enable_telnet) {
        /* TODO: ensure we do not inherit open files handles here */
        if (verbose_errors)
            printf("Starting Telnet Daemon...\n");
        sprintf(buff, "/usr/sbin/telnetd");
        retcode = system_command(buff);
        if (retcode != 0 && verbose_errors) {
            printf("Could not start telnet daemon\n");
        }
    }
    else if (found && netentryp->enable_telnet == 0) {
        /* If telnetd currently running, kill it */
        retcode = system_command("/usr/bin/killall telnetd ");
        if (retcode != 0 && verbose_errors)
            printf("Could not kill telnetd.\n");
    }

    /* TODO - Is this the right thing for hostname */
    if (netentryp->hostname[0]) {
        retcode = sethostname(netentryp->hostname, HOSTNAME_LEN + 1);
        if (retcode != 0 && verbose_errors) {
            printf("Could not set hostname\n");
        }
    }

    /* Moved here from custom_startup.sh since multicast
       route should be added whenever IP address changes */
    retcode =
        system_command
        ("/sbin/route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0");
    if (retcode != 0 && verbose_errors)
        printf("Could not kill in.tftpd.\n");

    return 0;
}

int parse_net(struct net_entry *netentryp, const char *filename)
{
    char linbuf[256], savebuf[256], *c, *cp, *name, *value;
    FILE *fp;
    int count = 0;

    /********************************
     * Open config file for reading *
     ********************************/
    if (filename == NULL)
        filename = NET_CONFIG_FILE;
    if ((fp = fopen(filename, "r")) == NULL) {
        if (verbose_errors)
            perror(filename);
        return -1;
    }

    /**************************************
     * Zero out the net_entry data struct *
     **************************************/
    memset(netentryp, 0x00, sizeof(struct net_entry));

    /****************************************
     * Read file a line at a time and parse *
     ****************************************/
    do {
        c = fgets(linbuf, 256, fp);


        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL || *c == '#')
            continue;

        if (strlen(c) <= 3) {
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) {        /* whole line */
            if (verbose_errors)
                printf("Parse error at line: %s file %s\n", savebuf,
                       filename);
            continue;
        }

        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        if (*cp == '\0') {
            if (!strcasecmp(name, S_LOCALDOMAIN) &&
                !strcasecmp(name, S_HOSTNAME)) {
                if (verbose_errors)
                    printf("Missing value at line: %s file %s\n", savebuf,
                           filename);
                continue;
            }
        }
        else {
            *cp = '\0';
            cp++;
        }

        if (strlen(name) <= 1) {        /* just the name */
            if (verbose_errors)
                printf("Bad parameter \"%s\" file %s\n", name, filename);
            continue;
        }

        while (*cp == ' ' || *cp == '\t')
            cp++;

        value = cp;

        if (strlen(value) < 1 && !strcasecmp(name, S_LOCALDOMAIN) &&
            !strcasecmp(name, S_HOSTNAME)) {
            if (verbose_errors)
                printf("Parse error at line: %s file %s\n", savebuf,
                       filename);
            continue;
        }

        if (strcasecmp(name, S_DHCP) == 0) {
            if (strncasecmp(value, "Yes", 3) == 0) {
                netentryp->enable_dhcp = 1;
            }
            else
                netentryp->enable_dhcp = 0;
        }
        else if (strcasecmp(name, S_ETHERIP) == 0) {
            strncpy(netentryp->ethip, value, INET_ADDR_LEN);
            count++;
        }
        else if (strcasecmp(name, S_ETHERMASK) == 0) {
            strncpy(netentryp->ethmask, value, INET_ADDR_LEN);
            count++;
        }
        else if (strcasecmp(name, S_DNSSERVIP) == 0) {
            strncpy(netentryp->dnsservip, value, INET_ADDR_LEN);
            count++;
        }
        else if (strcasecmp(name, S_LOCALDOMAIN) == 0) {
            strncpy(netentryp->localdomain, value, DOMAINNAME_LEN);
            count++;
        }
        else if (strcasecmp(name, S_HOSTNAME) == 0) {
            strncpy(netentryp->hostname, value, HOSTNAME_LEN);
            count++;
        }
        else if (strcasecmp(name, S_TELNET) == 0) {
            if (strncasecmp(value, "Yes", 3) == 0)
                netentryp->enable_telnet = 1;
            else
                netentryp->enable_telnet = 0;
        }
        else if (strcasecmp(name, S_HELLO) == 0) {
            if (strncasecmp(value, "Yes", 3) == 0)
                netentryp->enable_hello = 1;
            else
                netentryp->enable_hello = 0;
        }
        else if ((strcasecmp(name, S_FLANIP) != 0) &&
                 (strcasecmp(name, S_FLANMASK) != 0)) {
            if (verbose_errors)
                printf("Unknown parameter \"%s\" file %s\n", name,
                       filename);
        }

    } while (c != NULL);


    fclose(fp);

    return count;
}

void print_route(struct route_entry *routeentryp, int count, FILE * fp)
{

    int len;

    if (routeentryp == NULL)
        return;

    if (fp == NULL)
        fp = stdout;

    fprintf(fp,
            "################################################################\n");
    len = fprintf(fp, "%s.%d", S_ROUTEID, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", routeentryp->routeid);
    len = fprintf(fp, "%s.%d", S_DESTINATION, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", routeentryp->destination);
    len = fprintf(fp, "%s.%d", S_NETMASK, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", routeentryp->netmask);
    len = fprintf(fp, "%s.%d", S_GATEWAY, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", routeentryp->gateway);


}

/***********************************
 * Configure given route in the OS *
 ***********************************/
int apply_route(const char *destination,
                const char *netmask, const char *gateway)
{

    int retcode;
    char buff[128];
    struct in_addr indst, inmask;

    if (inet_aton(destination, &indst) == 0) {
        return RC_ERROR;
    }
    if (inet_aton(netmask, &inmask) == 0) {
        return RC_ERROR;
    }

    if ((indst.s_addr != 0) && (inmask.s_addr == 0xffffffff)) {
        sprintf(buff, "/sbin/route add %s netmask %s gw %s "
                "> /dev/null 2>&1", destination, netmask, gateway);
    }
    else if ((indst.s_addr != 0) &&
             (indst.s_addr & inmask.s_addr) != indst.s_addr) {
        return RC_INVALID_VALUE;
    }
    else {
        indst.s_addr = htonl(ntohl(indst.s_addr) & ntohl(inmask.s_addr));
        sprintf(buff, "/sbin/route add -net %s netmask %s gw %s "
                , inet_ntoa(indst), netmask, gateway);
    }
    if (verbose_errors)
        printf("Adding Route dest=%s mask=%s gw=%s\n",
               destination, netmask, gateway);
    retcode = system_command(buff);
    if (retcode != 0) {
        if (verbose_errors)
            printf("Route Add Failed for dest=%s mask=%s gw=%s\n",
                   destination, netmask, gateway);
        return RC_ERROR;
    }

    return RC_NORMAL;
}

/**********************************
 * Remove given route from the OS *
 **********************************/
int remove_route(const char *destination,
                 const char *netmask, const char *gateway)
{

    int retcode;
    char buff[128];
    struct in_addr indst, inmask;

    if (inet_aton(destination, &indst) == 0)
        return RC_ERROR;
    if (inet_aton(netmask, &inmask) == 0)
        return RC_ERROR;

    if ((indst.s_addr != 0) && (inmask.s_addr == 0xffffffff)) {
        if (verbose_errors)
            printf("Deleting Route dest=%s mask=%s gw=%s\n",
                   destination, netmask, gateway);
        sprintf(buff, "/sbin/route del %s netmask %s gw %s "
                "> /dev/null 2>&1", destination, netmask, gateway);
    }
    else if ((indst.s_addr != 0) &&
             (indst.s_addr & inmask.s_addr) != indst.s_addr) {
        return RC_INVALID_VALUE;
    }
    else {
        indst.s_addr = htonl(ntohl(indst.s_addr) & ntohl(inmask.s_addr));

        if (verbose_errors)
            printf("Deleting route net=%s mask=%s gw=%s\n",
                   inet_ntoa(indst), netmask, gateway);

        sprintf(buff, "/sbin/route del -net %s netmask %s gw %s "
                , inet_ntoa(indst), netmask, gateway);
    }
    retcode = system_command(buff);
    if (retcode != 0) {
        if (verbose_errors)
            printf("Route Delete failed for dest=%s "
                   "mask=%s gw=%s\n", destination, netmask, gateway);
        return RC_ERROR;
    }

    return RC_NORMAL;
}


/***************************************************
 * Count the number of routes in given linked list *
 ***************************************************/
int count_routes(struct list_head *route_head)
{

    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, route_head)
        count++;

    return count;
}

/*************************************
 * Save given list of routes to file *
 *************************************/
int save_routes(struct list_head *route_head)
{

    struct list_head *tmp;
    struct route_entry *routeentryp;
    int count = 0;
    FILE *fp;

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(ROUTE_CONFIG_FILE, "w+")) == NULL) {
        return -1;
    }

    list_for_each(tmp, route_head) {
        struct in_addr gw;
        count++;
        routeentryp = list_entry(tmp, struct route_entry, route_list);
        inet_aton(routeentryp->gateway, &gw);
        print_route(routeentryp, count, fp);
    }

    fclose(fp);

    return count;
}

/***************************************
 * Check if given route is well formed *
 ***************************************/
int check_route(struct route_entry *routeentryp,
                struct list_head *route_head, int route_index)
{
    struct list_head *tmp;
    struct route_entry *tmprouteentryp;
    unsigned long ip, mask, cip, cmask;
    int route_id;

    if (routeentryp == NULL)
        return -1;

    /****************************
     * Check if route is unique *
     ****************************/
    ip = ntohl(inet_addr(routeentryp->destination));
    mask = ntohl(inet_addr(routeentryp->netmask));
    if (route_head != NULL) {
        list_for_each(tmp, route_head) {
            tmprouteentryp =
                list_entry(tmp, struct route_entry, route_list);
            route_id = tmprouteentryp->routeid;
            if (route_id != route_index) {
                cip = ntohl(inet_addr(tmprouteentryp->destination));
                cmask = ntohl(inet_addr(tmprouteentryp->netmask));
                if ((mask == cmask) && ((cip & cmask) == (ip & mask)))
                    return RC_ERROR;
            }
        }
    }

    return 0;
}


/*********************************************************************
 * Parse the kernel routing table and create a linked list of routes *
 * Returns number of routes in /proc/net/route                       *
 *********************************************************************/
int parse_kernel_routes(struct list_head *route_head)
{
    char linbuf[256];
    FILE *fp;
    int nl = 0, count = 0, i, flgs, ref, use, metric, mtu, win, ir;
    struct route_entry *routeentryp, routeentry;
    unsigned long d, g, m;
    struct in_addr dest, gw, mask;

    /********************************
     * Open config file for reading *
     ********************************/
    if ((fp = fopen(KERNEL_ROUTE_FILE, "r")) == NULL) {
        if (verbose_errors)
            perror(KERNEL_ROUTE_FILE);
        return -1;
    }

    /**********************************
     * Zero out the route linked list *
     **********************************/
    INIT_LIST_HEAD(route_head);

    /****************************************
     * Read file a line at a time and parse *
     ****************************************/
    while (fgets(linbuf, sizeof(linbuf), fp) != NULL) {

        i = 0;
        if (nl) {
            while (linbuf[i] && linbuf[i] != ' ' && linbuf[i] != '\t')
                i++;
            linbuf[i++] = 0;    /* interface */
            if (sscanf(linbuf + i, "%lx%lx%X%d%d%d%lx%d%d%d", &d, &g,
                       &flgs, &ref, &use, &metric, &m, &mtu, &win,
                       &ir) != 10) {
                VLOG_WARNING("Unsupported kernel route format");
            }
            if (flgs & RTF_UP) {
                dest.s_addr = d;
                gw.s_addr = g;
                mask.s_addr = m;
                VLOG_INFO("%s: d 0x%lx g 0x%lx , m 0x%lx", __FUNCTION__,
                           d, g, m);
                memset(&routeentry, 0, (sizeof(struct route_entry)));
                routeentry.routeid = nl;
                strncpy(routeentry.destination, inet_ntoa(dest),
                        INET_ADDR_LEN);
                strncpy(routeentry.netmask, inet_ntoa(mask),
                        INET_ADDR_LEN);
                strncpy(routeentry.gateway, inet_ntoa(gw), INET_ADDR_LEN);

                routeentryp = malloc(sizeof(struct route_entry));
                if (routeentryp != NULL) {
                    memcpy(routeentryp, &routeentry,
                           sizeof(struct route_entry));
                    list_add_tail(&(routeentryp->route_list), route_head);
                    count++;
                }
            }
        }
        nl++;
    }

    (void) fclose(fp);

    return count;
}

/*********************************************************************
 * Parse the route config file and create a linked list of routes in *
 * it.                                                               *
 *********************************************************************/
int parse_routes(struct list_head *route_head, const char *filename)
{

    char linbuf[256], savebuf[256], *c, *cp, *name, *value;
    FILE *fp;
    int count = 0, idx, shouldsave = 0;
    int currentidx = -1;
    struct list_head *tmp;
    struct route_entry *routeentryp, routeentry;


    /********************************
     * Open config file for reading *
     ********************************/
    if (filename == NULL)
        filename = ROUTE_CONFIG_FILE;
    if ((fp = fopen(filename, "r")) == NULL) {
        if (verbose_errors)
            perror(filename);
        return -1;
    }

    /**********************************
     * Zero out the route linked list *
     **********************************/
    INIT_LIST_HEAD(route_head);

    /****************************************
     * Read file a line at a time and parse *
     ****************************************/
    do {
        c = fgets(linbuf, 256, fp);


        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL || *c == '#')
            continue;


        if (strlen(c) <= 3) {
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) {        /* whole line */
            if (verbose_errors)
                printf("Parse error at line: %s file %s\n", savebuf,
                       filename);
            continue;
        }


        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        if (*cp == '\0') {
            if (verbose_errors)
                printf("Missing value at line: %s file %s\n", savebuf,
                       filename);
            continue;
        }
        *cp = '\0';
        cp++;

        if (strlen(name) <= 1) {        /* just the name */
            if (verbose_errors)
                printf("Bad parameter \"%s\" file %s\n", name, filename);
            continue;
        }

        while (*cp == ' ' || *cp == '\t')
            cp++;

        value = cp;

        if (strlen(value) < 1) {
            if (verbose_errors)
                printf("Parse error at line: %s\n", savebuf);
            continue;
        }

        cp = name;
        while (*cp != '.' && *cp != '\0')
            cp++;
        if (*cp == '.') {
            *cp = '\0';
            cp++;
            idx = atoi(cp);
        }
        else {
            if (verbose_errors)
                printf("Parse error at line %s\n", savebuf);
            continue;
        }


        /***********************************************************
         * Change in index begins new entry. Deal with the old one *
         ***********************************************************/
        if (currentidx == -1)
            currentidx = idx;

        if (idx != currentidx) {
            if (check_route(&routeentry, route_head, -1) != -1) {
                routeentryp = malloc(sizeof(struct route_entry));
                if (routeentryp != NULL) {
                    memcpy(routeentryp, &routeentry,
                           (sizeof(struct route_entry)));
                    list_add_tail(&(routeentryp->route_list), route_head);
                    count++;
                    memset(&routeentry, 0, (sizeof(struct route_entry)));
                }
            }
            currentidx = idx;
        }

        /* Check for parameters */
        if (strcasecmp(name, S_ROUTEID) == 0) {
            routeentry.routeid = atoi(value);
        }
        else if (strcasecmp(name, S_DESTINATION) == 0) {
            strncpy(routeentry.destination, value, INET_ADDR_LEN);
        }
        else if (strcasecmp(name, S_NETMASK) == 0) {
            strncpy(routeentry.netmask, value, INET_ADDR_LEN);
        }
        else if (strcasecmp(name, S_GATEWAY) == 0) {
            strncpy(routeentry.gateway, value, INET_ADDR_LEN);
        }
        else {
            if (verbose_errors)
                printf("Unknown parameter \"%s\" \n", name);
        }
    } while (c != NULL);

    /* Add the last entry */
    if (currentidx != -1) {
        if (check_route(&routeentry, route_head, -1) != -1) {
            routeentryp = malloc(sizeof(struct route_entry));
            if (routeentryp != NULL) {
                memcpy(routeentryp, &routeentry,
                       (sizeof(struct route_entry)));
                list_add_tail(&(routeentryp->route_list), route_head);
                count++;
            }
        }
    }

    fclose(fp);

    /**********************************
     * Fix routeid value if necessary *
     **********************************/
    if (count > 0) {
        list_for_each(tmp, route_head) {
            routeentryp = list_entry(tmp, struct route_entry, route_list);
            if (routeentryp->routeid <= 0) {
                routeentryp->routeid = get_unique_routeid(route_head);
                shouldsave = 1;
            }
        }
    }

    if (shouldsave)
        save_routes(route_head);

    return count;
}

void free_route(struct list_head *route_head)
{
    struct list_head *tmp;
    struct route_entry *routeentryp;

    if (route_head == NULL)
        return;

    list_for_each(tmp, route_head) {
        routeentryp = list_entry(tmp, struct route_entry, route_list);
        list_del(&(routeentryp->route_list));
        free(routeentryp);
    }
}

int get_unique_routeid(struct list_head *route_head)
{
    struct list_head *tmp;
    struct route_entry *routeentryp;
    int unique_routeid = 1;

  again:
    list_for_each(tmp, route_head) {
        routeentryp = list_entry(tmp, struct route_entry, route_list);
        if (routeentryp->routeid == unique_routeid) {
            unique_routeid++;
            goto again;
        }
    }

    return unique_routeid;
}


#define S_SERVERID              "SERVER_ID"
#define S_SERVERNAME    "SERVER_NAME"
#define S_SERVERPORT    "SERVER_PORT"
#define S_BANDWIDTH             "MAX_BANDWIDTH"
#define S_MULTICAST             "MULTICAST"
#define S_AVOPTION              "AVOPTION"

/***********************************************************
 * Count the number of server entries in given linked list *
 ***********************************************************/
int count_serverlist(struct list_head *serverlist_head)
{

    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, serverlist_head)
        count++;

    return count;
}


static void print_serverlist(struct serverlist_entry *serverlistentryp,
                             int count, FILE * fp)
{

    int len;

    if (serverlistentryp == NULL)
        return;

    if (fp == NULL)
        fp = stdout;

    fprintf(fp,
            "################################################################\n");
    len = fprintf(fp, "%s.%d", S_SERVERID, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", serverlistentryp->serverid);
    len = fprintf(fp, "%s.%d", S_SERVERNAME, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", serverlistentryp->servername);
    len = fprintf(fp, "%s.%d", S_SERVERPORT, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", serverlistentryp->serverport);
    len = fprintf(fp, "%s.%d", S_BANDWIDTH, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", serverlistentryp->bandwidth);
    len = fprintf(fp, "%s.%d", S_MULTICAST, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            serverlistentryp->multicast ? "Yes" : "No");
    len = fprintf(fp, "%s.%d", S_AVOPTION, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            (serverlistentryp->avoption == AVOPTION_AUDIO_VIDEO) ? "Both" :
            ((serverlistentryp->avoption ==
              AVOPTION_AUDIO) ? "Audio" : "Video"));
    len = fprintf(fp, "%s.%d", S_AUDIO_BIDIR, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            serverlistentryp->bidir ? "Yes" : "No");
}

static int serverlist_config_handler(char *name, char *value, void *ptr,
                                     int idx)
{

    struct list_head *serverlist_head = (struct list_head *) ptr;
    struct serverlist_entry *serverlistentryp;
    static struct serverlist_entry serverlistentry;
    static int lastidx = 0;

    if (idx != lastidx) {
        if (lastidx == 0 && (idx > 1 || idx == 0))
            return -1;
        if (lastidx == 0 && idx == 1) {
            memset(&serverlistentry, 0x00, sizeof(serverlistentry));    /* prepare for new struct */
            serverlistentry.serverport = SERVER_PORT;
            lastidx = 1;
        }
        else {
            /* Add the struct built up so far to the linked list */
            serverlistentryp = malloc(sizeof(serverlistentry));
            if (serverlistentryp == NULL)
                return -1;

            memcpy(serverlistentryp, &serverlistentry,
                   sizeof(serverlistentry));

            list_add_tail(&serverlistentryp->serverlist_list,
                          serverlist_head);

            memset(&serverlistentry, 0x00, sizeof(serverlistentry));    /* prepare for new struct */
            serverlistentry.serverport = SERVER_PORT;
            lastidx = idx;
        }
    }

    if (strcasecmp(name, S_SERVERID) == 0) {
        serverlistentry.serverid = atoi(value);
    }
    else if (strcasecmp(name, S_SERVERNAME) == 0) {
        strncpy(serverlistentry.servername, value, SERVERNAME_LEN);
    }
    else if (strcasecmp(name, S_SERVERPORT) == 0) {
        serverlistentry.serverport = atoi(value);
    }
    else if (strcasecmp(name, S_BANDWIDTH) == 0) {
        strncpy(serverlistentry.bandwidth, value, BANDWIDTH_LEN);
    }
    else if (strcasecmp(name, S_MULTICAST) == 0) {
        if (strcasecmp(value, "yes") == 0)
            serverlistentry.multicast = 1;
        else
            serverlistentry.multicast = 0;
    }
    else if (strcasecmp(name, S_AVOPTION) == 0) {
        if (strcasecmp(value, "both") == 0)
            serverlistentry.avoption = AVOPTION_AUDIO_VIDEO;
        else if (strcasecmp(value, "audio") == 0)
            serverlistentry.avoption = AVOPTION_AUDIO;
        else
            serverlistentry.avoption = AVOPTION_VIDEO;
    }
    else if (strcasecmp(name, S_AUDIO_BIDIR) == 0) {
        if (strcasecmp(value, "yes") == 0)
            serverlistentry.bidir = 1;
        else
            serverlistentry.bidir = 0;
    }
    return 0;
}

int get_unique_serverid(struct list_head *serverlist_head)
{
    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;
    int unique_serverid = 1;

  again:
    list_for_each(tmp, serverlist_head) {
        serverlistentryp =
            list_entry(tmp, struct serverlist_entry, serverlist_list);
        if (serverlistentryp->serverid == unique_serverid) {
            unique_serverid++;
            goto again;
        }
    }
    return unique_serverid;
}

int parse_serverlist(struct list_head *serverlist_head)
{
    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;
    int count = 0;

    /***********************************
     * Zero out the server linked list *
     ***********************************/
    INIT_LIST_HEAD(serverlist_head);

    if (parse_list_config_file(SERVERLIST_CONFIG_FILE,
                               serverlist_config_handler,
                               serverlist_head) < 0) {
        VLOG_WARNING("Could not load server configuration file %s",
                      SERVERLIST_CONFIG_FILE);
    }
    else {
        count = count_serverlist(serverlist_head);

        /***********************************
         * Fix serverid value if necessary *
         ***********************************/
        if (count > 0) {
            list_for_each(tmp, serverlist_head) {
                serverlistentryp = list_entry(tmp, struct serverlist_entry,
                                              serverlist_list);
                if (serverlistentryp->serverid <= 0)
                    serverlistentryp->serverid =
                        get_unique_serverid(serverlist_head);
            }
        }
    }
    return count;
}

/******************************************
 * Save given list of serverlists to file *
 ******************************************/
int save_serverlist(struct list_head *serverlist_head)
{

    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;
    int count = 0;
    FILE *fp;

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(SERVERLIST_CONFIG_FILE, "w+")) == NULL) {
        return -1;
    }

    list_for_each(tmp, serverlist_head) {
        count++;
        serverlistentryp =
            list_entry(tmp, struct serverlist_entry, serverlist_list);
        print_serverlist(serverlistentryp, count, fp);
    }

    fclose(fp);

    return count;
}

/****************************************
 * Free memory used by serverlist list  *
 ****************************************/
void free_serverlist(struct list_head *serverlist_head)
{

    struct list_head *tmp;
    struct serverlist_entry *serverlistentryp;

    if (serverlist_head == NULL)
        return;

    if (!list_empty(serverlist_head)) {
        list_for_each(tmp, serverlist_head) {
            serverlistentryp =
                list_entry(tmp, struct serverlist_entry, serverlist_list);
            free(serverlistentryp);
        }
    }
}

#define S_NETWORK               "NETWORK"
#define S_NETMASK               "NETMASK"
#define S_ENABLE                "ENABLE"
#define S_MAX_RATE              "MAX_RATE"
#define S_MIN_COMPRESSION       "MIN_COMPRESSION"
#define S_MAX_COMPRESSION       "MAX_COMPRESSION"
#define S_BURST_RATIO           "BURST_RATIO"
#define S_BURST_DURATION        "BURST_DURATION"
#define S_UNICAST               "UNICAST"
#define S_MULTICAST_INIT        "MULTICAST_INIT"
#define S_MULTICAST_PART        "MULTICAST_PART"
#define S_UNICAST_PREEMPTIBLE   "UNICAST_PREEMPTIBLE"

/******************************************************************
 * Count the number of authorization entries in given linked list *
 ******************************************************************/
int count_authlist(struct list_head *authlist_head)
{

    struct list_head *tmp;
    int count = 0;


    list_for_each(tmp, authlist_head)
        count++;

    return count;
}


static void print_authlist(struct authlist_entry *authlistentryp,
                           int count, FILE * fp)
{

    int len;

    if (authlistentryp == NULL)
        return;

    if (fp == NULL)
        fp = stdout;

    fprintf(fp,
            "################################################################\n");
    len = fprintf(fp, "%s.%d", S_AUTHID, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", authlistentryp->authid);
    len = fprintf(fp, "%s.%d", S_NETWORK, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", authlistentryp->network);
    len = fprintf(fp, "%s.%d", S_NETMASK, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", authlistentryp->netmask);
    len = fprintf(fp, "%s.%d", S_ENABLE, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            authlistentryp->enable ? "Yes" : "No");
    len = fprintf(fp, "%s.%d", S_MAX_RATE, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", authlistentryp->max_bandwidth);
    len = fprintf(fp, "%s.%d", S_MIN_COMPRESSION, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ",
            authlistentryp->min_compression);
    len = fprintf(fp, "%s.%d", S_MAX_COMPRESSION, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ",
            authlistentryp->max_compression);
    len = fprintf(fp, "%s.%d", S_BURST_RATIO, count);
    fprintf(fp, "%*s %.2f\n", 25 - len, " ", authlistentryp->burst_ratio);
    len = fprintf(fp, "%s.%d", S_BURST_DURATION, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", authlistentryp->burst_duration);
    len = fprintf(fp, "%s.%d", S_UNICAST, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            authlistentryp->unicast ? "Yes" : "No");
    len = fprintf(fp, "%s.%d", S_MULTICAST_INIT, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            authlistentryp->multicast_init ? "Yes" : "No");
    len = fprintf(fp, "%s.%d", S_MULTICAST_PART, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            authlistentryp->multicast_part ? "Yes" : "No");
    len = fprintf(fp, "%s.%d", S_UNICAST_PREEMPTIBLE, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            authlistentryp->unicast_preemptible ? "Yes" : "No");

}

static int authlist_config_handler(char *name, char *value, void *ptr,
                                   int idx)
{

    struct list_head *authlist_head = (struct list_head *) ptr;
    struct authlist_entry *authlistentryp;
    static struct authlist_entry authlistentry;
    static int lastidx = 0;


    if (idx != lastidx) {
        if (lastidx == 0 && (idx > 1 || idx == 0))
            return -1;
        if (lastidx == 0 && idx == 1) {
            memset(&authlistentry, 0x00, sizeof(authlistentry));        /* prepare for new struct */
            lastidx = 1;
        }
        else {
            /* Add the struct built up so far to the linked list */
            authlistentryp = malloc(sizeof(authlistentry));
            if (authlistentryp == NULL)
                return -1;
            memcpy(authlistentryp, &authlistentry, sizeof(authlistentry));
            list_add_tail(&authlistentryp->authlist_list, authlist_head);

            memset(&authlistentry, 0x00, sizeof(authlistentry));        /* prepare for new struct */
            lastidx = idx;
        }
    }
    if (strcasecmp(name, S_AUTHID) == 0) {
        authlistentry.authid = atoi(value);
    }
    else if (strcasecmp(name, S_NETWORK) == 0) {
        strncpy(authlistentry.network, value, INET_ADDR_LEN);
    }
    else if (strcasecmp(name, S_NETMASK) == 0) {
        strncpy(authlistentry.netmask, value, INET_ADDR_LEN);
    }
    else if (strcasecmp(name, S_ENABLE) == 0) {
        if (strcasecmp(value, "yes") == 0)
            authlistentry.enable = 1;
        else
            authlistentry.enable = 0;
    }
    else if (strcasecmp(name, S_MAX_RATE) == 0) {
        strncpy(authlistentry.max_bandwidth, value, BANDWIDTH_LEN);
    }
    else if (strcasecmp(name, S_MIN_COMPRESSION) == 0) {
        authlistentry.min_compression = atoi(value);
    }
    else if (strcasecmp(name, S_MAX_COMPRESSION) == 0) {
        authlistentry.max_compression = atoi(value);
    }
    else if (strcasecmp(name, S_BURST_RATIO) == 0) {
        authlistentry.burst_ratio = atof(value);
    }
    else if (strcasecmp(name, S_BURST_DURATION) == 0) {
        authlistentry.burst_duration = atoi(value);
    }
    else if (strcasecmp(name, S_UNICAST) == 0) {
        if (strcasecmp(value, "yes") == 0)
            authlistentry.unicast = 1;
        else
            authlistentry.unicast = 0;
    }
    else if (strcasecmp(name, S_MULTICAST_INIT) == 0) {
        if (strcasecmp(value, "yes") == 0)
            authlistentry.multicast_init = 1;
        else
            authlistentry.multicast_init = 0;
    }
    else if (strcasecmp(name, S_MULTICAST_PART) == 0) {
        if (strcasecmp(value, "yes") == 0)
            authlistentry.multicast_part = 1;
        else
            authlistentry.multicast_part = 0;
    }
    else if (strcasecmp(name, S_UNICAST_PREEMPTIBLE) == 0) {
        if (strcasecmp(value, "yes") == 0)
            authlistentry.unicast_preemptible = 1;
        else
            authlistentry.unicast_preemptible = 0;
    }
    return 0;
}

int get_unique_authid(struct list_head *authlist_head)
{
    struct list_head *tmp;
    struct authlist_entry *authlistentryp;
    int unique_authid = 1;

  again:
    list_for_each(tmp, authlist_head) {
        authlistentryp =
            list_entry(tmp, struct authlist_entry, authlist_list);
        if (authlistentryp->authid == unique_authid) {
            unique_authid++;
            goto again;
        }
    }
    return unique_authid;
}

int parse_authlist(struct list_head *authlist_head)
{
    struct list_head *tmp;
    struct authlist_entry *authlistentryp;
    int count = 0;

    /***********************************
     * Zero out the auth linked list *
     ***********************************/
    INIT_LIST_HEAD(authlist_head);

    if (parse_list_config_file(AUTHLIST_CONFIG_FILE,
                               authlist_config_handler,
                               authlist_head) < 0) {
        VLOG_WARNING("Could not load auth configuration file %s",
                      AUTHLIST_CONFIG_FILE);
    }
    else {
        count = count_authlist(authlist_head);

        /***********************************
         * Fix authid value if necessary   *
         ***********************************/
        if (count > 0) {
            list_for_each(tmp, authlist_head) {
                authlistentryp = list_entry(tmp, struct authlist_entry,
                                            authlist_list);
                if (authlistentryp->authid <= 0)
                    authlistentryp->authid = get_unique_authid(authlist_head);
            }
        }
    }
    return count;
}


/**************************************
 * Free memory used by authlist list  *
 **************************************/
void free_authlist(struct list_head *authlist_head)
{

    struct list_head *tmp;
    struct authlist_entry *authlistentryp;

    if (authlist_head == NULL)
        return;

    if (!list_empty(authlist_head)) {
        list_for_each(tmp, authlist_head) {
            authlistentryp =
                list_entry(tmp, struct authlist_entry, authlist_list);
            free(authlistentryp);
        }
    }
}


/******************************************
 * Save given list of authlists to file *
 ******************************************/
int save_authlist(struct list_head *authlist_head)
{

    struct list_head *tmp;
    struct authlist_entry *authlistentryp;
    int count = 0;
    FILE *fp;

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(AUTHLIST_CONFIG_FILE, "w+")) == NULL) {
        return -1;
    }

    list_for_each(tmp, authlist_head) {
        count++;
        authlistentryp =
            list_entry(tmp, struct authlist_entry, authlist_list);
        print_authlist(authlistentryp, count, fp);
    }

    fclose(fp);

    return count;
}

#define S_RULE_ID                   "RULEID"
#define S_RULE_HWM                  "HWM"
#define S_RULE_LWM                  "LWM"
#define S_RULE_LOW_COMP             "LOW_COMP"
#define S_RULE_HIGH_COMP            "HIGH_COMP"
#define S_RULE_DIVIDER              "FRAME_DIVIDER"
#define S_RULE_FPGA_DROP            "FPGA_DROP"

/***************************************************
 * Count the number of rules in given linked list  *
 ***************************************************/
int count_rulelist(struct list_head *rulelist_head)
{

    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, rulelist_head)
        count++;

    return count;
}
static void print_rulelist(struct rulelist_entry *rulelistentryp,
                           int count, FILE * fp)
{

    int len;

    if (rulelistentryp == NULL)
        return;

    if (fp == NULL)
        fp = stdout;

    fprintf(fp,
            "################################################################\n");
    len = fprintf(fp, "%s.%d", S_RULE_ID, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", rulelistentryp->rule_id);

    len = fprintf(fp, "%s.%d", S_RULE_HWM, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", rulelistentryp->rule_hwm);

    len = fprintf(fp, "%s.%d", S_RULE_LWM, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", rulelistentryp->rule_lwm);

    len = fprintf(fp, "%s.%d", S_RULE_LOW_COMP, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", rulelistentryp->low_comp_delta);

    len = fprintf(fp, "%s.%d", S_RULE_HIGH_COMP, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ",
            rulelistentryp->high_comp_delta);

    len = fprintf(fp, "%s.%d", S_RULE_DIVIDER, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", rulelistentryp->divider_delta);

    len = fprintf(fp, "%s.%d", S_RULE_FPGA_DROP, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            rulelistentryp->rule_fpga_drop ? "Yes" : "No");
}

static int rulelist_config_handler(char *name, char *value, void *ptr,
                                   int idx)
{

    struct list_head *rulelist_head = (struct list_head *) ptr;
    struct rulelist_entry *rulelistentryp;
    static struct rulelist_entry rulelistentry;
    static int lastidx = 0;

    if (idx != lastidx) {
        if (lastidx == 0 && (idx > 1 || idx == 0))
            return -1;
        if (lastidx == 0 && idx == 1) {
            memset(&rulelistentry, 0x00, sizeof(rulelistentry));        /* prepare for new struct */
            lastidx = 1;
        }
        else {
            /* Add the struct built up so far to the linked list */
            rulelistentryp = malloc(sizeof(rulelistentry));
            if (rulelistentryp == NULL)
                return -1;
            memcpy(rulelistentryp, &rulelistentry, sizeof(rulelistentry));
            list_add_tail(&rulelistentryp->rulelist_list, rulelist_head);

            memset(&rulelistentry, 0x00, sizeof(rulelistentry));        /* prepare for new struct */
            lastidx = idx;
        }
    }
    if (strcasecmp(name, S_RULE_ID) == 0) {
        rulelistentry.rule_id = atoi(value);
    }
    else if (strcasecmp(name, S_RULE_HWM) == 0) {
        rulelistentry.rule_hwm = atoi(value);
    }
    else if (strcasecmp(name, S_RULE_LWM) == 0) {
        rulelistentry.rule_lwm = atoi(value);
    }
    else if (strcasecmp(name, S_RULE_LOW_COMP) == 0) {
        rulelistentry.low_comp_delta = atoi(value);
    }
    else if (strcasecmp(name, S_RULE_HIGH_COMP) == 0) {
        rulelistentry.high_comp_delta = atoi(value);
    }
    else if (strcasecmp(name, S_RULE_DIVIDER) == 0) {
        rulelistentry.divider_delta = atoi(value);
    }
    else if (strcasecmp(name, S_RULE_FPGA_DROP) == 0) {
        if (strcasecmp(value, "yes") == 0)
            rulelistentry.rule_fpga_drop = 1;
        else
            rulelistentry.rule_fpga_drop = 0;
    }

    return 0;
}

int get_unique_ruleid(struct list_head *rulelist_head)
{
    struct list_head *tmp;
    struct rulelist_entry *rulelistentryp;
    int unique_ruleid = 1;

  again:
    list_for_each(tmp, rulelist_head) {
        rulelistentryp =
            list_entry(tmp, struct rulelist_entry, rulelist_list);
        if (rulelistentryp->rule_id == unique_ruleid) {
            unique_ruleid++;
            goto again;
        }
    }
    return unique_ruleid;
}


int parse_rulelist(struct list_head *rulelist_head)
{
    struct list_head *tmp;
    struct rulelist_entry *rulelistentryp;
    int count = 0;

    /***********************************
     * Zero out the rule linked list   *
     ***********************************/
    INIT_LIST_HEAD(rulelist_head);

    if (parse_list_config_file(RULELIST_CONFIG_FILE,
                               rulelist_config_handler,
                               rulelist_head) < 0) {
        VLOG_WARNING("Could not load rule configuration file %s",
                      RULELIST_CONFIG_FILE);
    }
    else {
        count = count_rulelist(rulelist_head);

        /***********************************
         * Fix rule_id value if necessary  *
         ***********************************/
        if (count > 0) {
            list_for_each(tmp, rulelist_head) {
                rulelistentryp = list_entry(tmp, struct rulelist_entry,
                                            rulelist_list);
                if (rulelistentryp->rule_id <= 0)
                    rulelistentryp->rule_id = get_unique_ruleid(rulelist_head);

            }
        }
    }
    return count;
}

/************************************
 * Save given list of rules to file *
 ************************************/
int save_rulelist(struct list_head *rulelist_head)
{

    struct list_head *tmp;
    struct rulelist_entry *rulelistentryp;
    int count = 0;
    FILE *fp;

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(RULELIST_CONFIG_FILE, "w+")) == NULL) {
        return -1;
    }

    list_for_each(tmp, rulelist_head) {
        count++;
        rulelistentryp =
            list_entry(tmp, struct rulelist_entry, rulelist_list);
        print_rulelist(rulelistentryp, count, fp);
    }

    fclose(fp);

    return count;
}

/************************************
 * Free memory used by rule list    *
 ************************************/
void free_rulelist(struct list_head *rulelist_head)
{

    struct list_head *tmp;
    struct rulelist_entry *rulelistentryp;

    if (rulelist_head == NULL)
        return;

    if (!list_empty(rulelist_head)) {
        list_for_each(tmp, rulelist_head) {
            rulelistentryp =
                list_entry(tmp, struct rulelist_entry, rulelist_list);
            free(rulelistentryp);
        }
    }
}

#define S_DESCEDIDID     "DESCRIPTOR_EDIDID"
#define S_DESCBLOCK      "DESCRIPTOR_BLOCK"
#define S_DESCLUTID      "DESCRIPTOR_LUTID"
#define S_DESCHSPOL      "DESCRIPTOR_HSPOL"
#define S_DESCVSPOL      "DESCRIPTOR_VSPOL"

/********************************************************
 * Count the number of edidlists in given linked list *
 ********************************************************/
int count_edidlist(struct list_head *edidlist_head)
{

    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, edidlist_head)
        count++;

    return count;
}

int find_block_in_edidlist(struct list_head *edidlist_head, int block)
{
    struct list_head *tmp;
    struct edidlist_entry *edidlistp;

    list_for_each(tmp, edidlist_head) {
        edidlistp = list_entry(tmp, struct edidlist_entry, edidlist_list);
        if (edidlistp->block == block)
            return 1;
    }
    return 0;
}

static void print_edidlist(struct edidlist_entry *edidlistp,
                           int count, FILE * fp)
{

    int len;

    if (edidlistp == NULL)
        return;

    if (fp == NULL)
        fp = stdout;

    fprintf(fp,
            "################################################################\n");
    len = fprintf(fp, "%s.%d", S_DESCRIPTOR_EDIDID, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", edidlistp->edidid);
    len = fprintf(fp, "%s.%d", S_DESCRIPTOR_BLOCK, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", edidlistp->block);
    len = fprintf(fp, "%s.%d", S_DESCRIPTOR_LUTID, count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", edidlistp->lutid);
    len = fprintf(fp, "%s.%d", S_DESCRIPTOR_HSPOL, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            edidlistp->hspol ? "Pos" : "Neg");
    len = fprintf(fp, "%s.%d", S_DESCRIPTOR_VSPOL, count);
    fprintf(fp, "%*s %s\n", 25 - len, " ",
            edidlistp->vspol ? "Pos" : "Neg");
}

static int edidlist_config_handler(char *name, char *value, void *ptr,
                                   int idx)
{

    struct list_head *edidlist_head = (struct list_head *) ptr;
    struct edidlist_entry *edidlistp;
    static struct edidlist_entry edidlist;
    static int lastidx = 0;

    if (idx != lastidx) {
        if (lastidx == 0 && (idx > 1 || idx == 0))
            return -1;
        if (lastidx == 0 && idx == 1) {
            memset(&edidlist, 0x00, sizeof(edidlist));  /* prepare for new struct */
            edidlist.hspol = edidlist.vspol = 0;
            lastidx = 1;
        }
        else {
            /* Add the struct built up so far to the linked list */
            edidlistp = malloc(sizeof(edidlist));
            if (edidlistp == NULL)
                return -1;
            memcpy(edidlistp, &edidlist, sizeof(edidlist));
            list_add_tail(&edidlistp->edidlist_list, edidlist_head);
            memset(&edidlist, 0x00, sizeof(edidlist));  /* prepare for new struct */
            edidlist.hspol = edidlist.vspol = 0;
            lastidx = idx;
        }
    }

    if (strcasecmp(name, S_DESCRIPTOR_EDIDID) == 0) {
        edidlist.edidid = atoi(value);
    }
    else if (strcasecmp(name, S_DESCRIPTOR_BLOCK) == 0) {
        edidlist.block = atoi(value);
    }
    else if (strcasecmp(name, S_DESCRIPTOR_LUTID) == 0) {
        edidlist.lutid = atoi(value);
    }
    else if (strcasecmp(name, S_DESCRIPTOR_HSPOL) == 0) {
        if (strncasecmp(value, "pos", 3) == 0)
            edidlist.hspol = 1;
        else
            edidlist.hspol = 0;
    }
    else if (strcasecmp(name, S_DESCRIPTOR_VSPOL) == 0) {
        if (strncasecmp(value, "pos", 3) == 0)
            edidlist.vspol = 1;
    }
    return 0;
}


int get_unique_edidid(struct list_head *edidlist_head)
{
    struct list_head *tmp;
    struct edidlist_entry *edidlistp;
    int unique_edidid = 1;

  again:
    list_for_each(tmp, edidlist_head) {
        edidlistp = list_entry(tmp, struct edidlist_entry, edidlist_list);
        if (edidlistp->edidid == unique_edidid) {
            unique_edidid++;
            goto again;
        }
    }
    return unique_edidid;
}


int parse_edidlist(struct list_head *edidlist_head)
{
    struct list_head *tmp;
    struct edidlist_entry *edidlistp;
    int count = 0;

    /***************************************
     * Zero out the edidlist linked list *
     ***************************************/
    INIT_LIST_HEAD(edidlist_head);

    if (parse_list_config_file(DESCRIPTOR_CONFIG_FILE,
                               edidlist_config_handler,
                               edidlist_head) < 0) {
        VLOG_WARNING("Could not load EDID edidlist file %s",
                      DESCRIPTOR_CONFIG_FILE);
    }
    else {
        count = count_edidlist(edidlist_head);

        /***********************************
         * Fix edidid value if necessary  *
         ***********************************/
        if (count > 0) {
            list_for_each(tmp, edidlist_head) {
                edidlistp = list_entry(tmp, struct edidlist_entry,
                                       edidlist_list);
                if (edidlistp->edidid <= 0)
                    edidlistp->edidid = get_unique_edidid(edidlist_head);

            }
        }
    }
    return count;
}

/******************************************
 * Save given list of edidlists to file *
 ******************************************/
int save_edidlist(struct list_head *edidlist_head)
{

    struct list_head *tmp;
    struct edidlist_entry *edidlistp;
    int count = 0;
    FILE *fp;

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(DESCRIPTOR_CONFIG_FILE, "w+")) == NULL) {
        return -1;
    }

    list_for_each(tmp, edidlist_head) {
        count++;
        edidlistp = list_entry(tmp, struct edidlist_entry, edidlist_list);
        print_edidlist(edidlistp, count, fp);
    }

    fclose(fp);

    return count;
}

/************************************
 * Free memory used by edidlists  *
 ************************************/
void free_edidlist(struct list_head *edidlist_head)
{

    struct list_head *tmp;
    struct edidlist_entry *edidlistp;

    if (edidlist_head == NULL)
        return;

    if (!list_empty(edidlist_head)) {
        list_for_each(tmp, edidlist_head) {
            edidlistp =
                list_entry(tmp, struct edidlist_entry, edidlist_list);
            free(edidlistp);
        }
    }
}



int send_server_startall(void)
{

/*     char buffer[512]; */

/*     sprintf(buffer, "%s %s startall > /dev/null 2>&1", PHP_BIN, */
/*             PHP_STRMSRVCMD); */
/*     return system(buffer); */
    return 0;
}

int send_server_stopall(void)
{

/*     char buffer[512]; */

/*     sprintf(buffer, "%s %s stopall > /dev/null 2>&1", PHP_BIN, */
/*             PHP_STRMSRVCMD); */
/*     return system(buffer); */
    return 0;
}


void send_serverdecport_discallupdate(void)
{

/*     char buffer[512]; */

/*     sprintf(buffer, "%s %s disconnectall > /dev/null 2>&1", PHP_BIN, */
/*             PHP_STRMSRVCMD); */
/*     system(buffer); */

}



/*******************************************
 * Get DNS server IP and local domain name *
 *******************************************/
int get_dns_config(char *dns, char *domain)
{
    char linbuf[256], savebuf[256], *c, *cp, *name, *value;
    FILE *fp;
    int count = 0;

    /********************************
     * Open config file for reading *
     ********************************/
    if ((fp = fopen(DHCP_CONFIG_FILE, "r")) == NULL) {
        if (verbose_errors)
            printf("Couldn't open %s\n", DHCP_CONFIG_FILE);
        return -1;
    }

    /****************************************
     * Read file a line at a time and parse *
     ****************************************/
    do {
        c = fgets(linbuf, 256, fp);


        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL || *c == '#')
            continue;

        if (strlen(c) <= 3) {
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) {        /* whole line */
            if (verbose_errors)
                printf("Parse error at line: %s file %s\n", savebuf,
                       DHCP_CONFIG_FILE);
            continue;
        }

        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        if (*cp == '\0') {
            if (verbose_errors)
                printf("Missing value at line: %s file %s\n", savebuf,
                       DHCP_CONFIG_FILE);
            continue;
        }
        *cp = '\0';
        cp++;

        if (strlen(name) <= 1) {        /* just the name */
            if (verbose_errors)
                printf("Bad parameter \"%s\" file %s\n", name,
                       DHCP_CONFIG_FILE);
            continue;
        }

        while (*cp == ' ' || *cp == '\t')
            cp++;

        value = cp;

        if (strlen(value) < 1) {
            if (verbose_errors)
                printf("Parse error at line: %s file %s\n", savebuf,
                       DHCP_CONFIG_FILE);
            continue;
        }

        if (strcasecmp(name, S_DOMAIN) == 0) {
            strncpy(domain, value, DOMAINNAME_LEN);
        }
        else if (strcasecmp(name, S_NAMESERVER) == 0) {
            /* ignore all nameserver entries except the 1st */
            if (count == 0)
                strncpy(dns, value, INET_ADDR_LEN);
            count++;
        }
        else {
            if (verbose_errors)
                printf("Unknown parameter \"%s\" file %s\n", name,
                       DHCP_CONFIG_FILE);
        }
    } while (c != NULL);

    fclose(fp);

    return count;
}

/*******************************************
 * Wait for DHCP_CONFIG_FILE to be created *
 ******************************************h*/
static int wait_for_dns_config(void)
{
    int i, retcode;
    struct stat buf;

    for (i = 0; i < 15; i++) {
        retcode = stat(DHCP_CONFIG_FILE, &buf);
        if (retcode == 0 && S_ISREG(buf.st_mode)) {
            return 1;
        }
        sleep(1);
    }
    return 0;
}

/********************************
 * Save info to DNS config file *
 ********************************/
static int write_dns_config(struct net_entry *netentryp)
{

    FILE *fp;
    int len;

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(DNS_CONFIG_FILE, "w+")) == NULL)
        return -1;

    /****************************************
     * Write local domain and dns server IP *
     ****************************************/
    len = fprintf(fp, "%s", S_DOMAIN);
    fprintf(fp, "%*s %s\n", 25 - len, " ", netentryp->localdomain);
    len = fprintf(fp, "%s", S_NAMESERVER);
    fprintf(fp, "%*s %s\n", 25 - len, " ", netentryp->dnsservip);

    fclose(fp);

    return 0;
}
void WriteHostFile(int req_code) {
    FILE    *fp=NULL;
    struct host_list_params *paramsp;
    struct if_list_params *ifparamsp;
    struct dns_client_params dcparams;
    char buff[128];
    char *hostip=NULL;
    struct network_ports_status_params *network_ports_params,
                                       *network_ports_params_list;




    if ((fp = fopen(TMPFILE, "w")) == NULL) {
        return ;
    }
    fprintf (fp, "127.0.0.1\tlocalhost.localdomain\tlocalhost\n");
    retrieveHostListValues();
    retrieveIFListValues();
    retrieveDNSClientValues(&dcparams);
    if (strlen (dcparams.hostname_value) > 0) {
        network_ports_params_list = retrieveNetworkPortsStatusValues();
        foreach_if_list_entry(ifparamsp) {
            if (ifparamsp->primary_value == 1) {
                // get the ip address from the system
                foreach_network_ports_status_entry(
                        network_ports_params, network_ports_params_list)
                {
                    // Skip if the interface is not enabled
                    if (network_ports_params->status_value == 0) continue;
                    if (network_ports_params->port_value ==
                            ifparamsp->port_value + 1)
                    {
                        hostip = strdup(network_ports_params->ipaddress_value);
                        break;
                    }
                }
            }
        }
        if (!hostip) {
            // Just get the first enabled interface IP address
            // if none are primary Defect #28796
            foreach_network_ports_status_entry( network_ports_params,
                    network_ports_params_list)
            {
                if (network_ports_params->status_value == 0) continue;
                hostip = strdup(network_ports_params->ipaddress_value);
                break;
            }
        }
        if (hostip) {
            fprintf (fp, "%s\t%s.%s\t%s\n", hostip,
                dcparams.hostname_value, dcparams.localdomain_value,
                dcparams.hostname_value);
            free(hostip);
        }
        destroyNetworkPortsStatusValues(network_ports_params_list);
    }
    foreach_host_list_entry(paramsp) {
        fprintf (fp, "%s\t%s\n",
                paramsp->hostaddress_value, paramsp->hostname_value);
    }
    fclose(fp);
    sprintf (buff, "/bin/mv %s %s ", TMPFILE, HOSTSFILE);
    system_command (buff);
}

