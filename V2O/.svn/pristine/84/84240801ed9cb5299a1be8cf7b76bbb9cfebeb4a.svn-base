#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
//#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/mii.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"
#include "if_list_menu_gen.h"
#include "network_ports_status_menu_gen.h"

#define STAT1000_T_FD        0x0800       
#define STAT1000_T_HD        0x0400       
                                                                                
struct mii_data {
    unsigned short phy_id;
    unsigned short reg_num;
    unsigned short val_in;
    unsigned short val_out;
};
                                                                                
static int get_link_partner(char *devname, char *buff) {
    int skfd, lpa, stat1000;
    struct ifreq ifr;
    struct mii_data *mii = (struct mii_data *) &(ifr.ifr_data);
 
    /* Open control socket */
    if ((skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
        return -1;
    }

    /* Get the vitals from the interface. */
    strncpy(ifr.ifr_name, devname, IFNAMSIZ);
    if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0) {
        if (errno != ENODEV) {
            return -1;
        }
    } 

    /* Some bits in the BMSR are latched, but we can't rely on being
       the only reader, so only the current values are meaningful */
    mii->reg_num = MII_BMSR;
    if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0) {
        return -1;
    }

    mii->reg_num = MII_LPA;
    if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0) {
        return -1;
    }
    lpa = mii->val_out; 

    mii->reg_num = MII_STAT1000;
    if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0) {
        return -1;
    }
    stat1000 = mii->val_out; 

    *buff = '\0';

    if (lpa & (LPA_PAUSE_CAP|LPA_100BASE4|LPA_100FULL|LPA_100HALF|
                                    LPA_10FULL|LPA_10HALF)) {
        if (lpa & LPA_10HALF)
            strcat(buff, "10BaseT-HD ");
        if (lpa & LPA_10FULL)
            strcat(buff, "10BaseT-FD ");
        if (lpa & LPA_100HALF)
            strcat(buff, "100BaseT-HD ");
        if (lpa & LPA_100FULL)
            strcat(buff, "100BaseT-FD ");
        if (lpa & LPA_100BASE4)
            strcat(buff, "100BaseT4 ");
    }
    if ((stat1000 & (STAT1000_T_FD|STAT1000_T_HD))) {
        if (stat1000 & STAT1000_T_FD) 
            strcat(buff, "1000BaseT-FD ");
        if (stat1000 & STAT1000_T_HD) 
            strcat(buff, "1000BaseT-HD ");
    }
    if (lpa & LPA_PAUSE_CAP)
        strcat(buff, "flow-control");

    close(skfd);

    return 0;
}

char *constructHeaderNetworkPortsStatus(char *buffer)
{

    sprintf(buffer, "%-16s %-16s %-20s",
            "IPAddress", "Netmask", "Pckts In/Out");

    return buffer;
}


char *constructRowNetworkPortsStatus(char *buffer,
                                     struct network_ports_status_params *params)
{
    char buf[32];
    if (params->status_value == 0) {
        snprintf(buf, 21, "Disabled"); 
    }
    else {
        snprintf(buf, 21, "%ld/%ld",
                 params->rx_packets_value, 
                 params->tx_packets_value);
    }
    sprintf(buffer, "%-16s %-16s %s",
            params->ipaddress_value,
            params->netmask_value,
            buf);
    return buffer;
}

int getNetworkPortsStatus(struct network_ports_status_params *params) 
{
    FILE *inpipe;
    char inbuf[256];
    char buff[256];
    char dnsname[256];
    char *cp;
    int processing_line = 0;
    struct if_list_params *if_config_params;

    // First get the Network interface lists
    retrieveIFListValues();

    char *command = "/sbin/ifconfig -a";

    inpipe = popen("/bin/uname -n", "r");
    if (inpipe == NULL) {
        return RC_ERROR;
    }
    if (fgets(inbuf, sizeof(inbuf), inpipe)) {
        sscanf(inbuf, "%s", dnsname);
    }
    pclose(inpipe);

    inpipe = popen(command, "r");
    if (inpipe == NULL) {
        return RC_ERROR;
    }

    while (fgets(inbuf, sizeof(inbuf), inpipe)) {
        char devname[16];
        FILE *ethtoolpipe;
        char ethtoolcommand[32];
        char ethtoolbuf[1600];
        char *ep;
        int ethtool_line = 0;

        if (strncmp(inbuf, "eth", 3) == 0) {
            processing_line = 1;
            params = getNetworkPortsStatusParamsStruct(params);
            strcpy(params->name_value, dnsname);
        }

        if (processing_line == 1) {
            strcpy(buff, "");
            params->virtual_value = 0;
            cp = strstr(inbuf, "eth");
            if (cp != NULL) {
                if (strncmp(cp + 4, ":0", 2) == 0)
                    params->virtual_value = 1;
                sscanf(cp, "eth%s", buff);
            }
            params->port_value = atoi(buff) + 1;
            sprintf(devname, "eth%lu", params->port_value - 1);
            // For this IF find out if it is configured as primary
            foreach_if_list_entry(if_config_params) {
                if (if_config_params->port_value == params->port_value-1) {
                    params->isprimary_value = if_config_params->primary_value;
                }
            }
            strcpy(buff, "");
            cp = strstr(inbuf, "HWaddr");
            if (cp != NULL)
                sscanf(cp, "HWaddr %s", buff);
            strcpy(params->hwaddress_value, buff);
            processing_line = 2;
            continue;
        }

        if (processing_line == 2) {
            strcpy(buff, "");
            cp = strstr(inbuf, "inet addr");
            if (cp != NULL)
                sscanf(cp, "inet addr:%s", buff);
            strcpy(params->ipaddress_value, buff);
            
            strcpy(buff, "");
            cp = strstr(inbuf, "Bcast:");
            if (cp != NULL)
                sscanf(cp, "Bcast:%s", buff);
            strcpy(params->broadcast_value, buff);

            strcpy(buff, "");
            cp = strstr(inbuf, "Mask:");
            if (cp != NULL)
                sscanf(cp, "Mask:%s", buff);
            strcpy(params->netmask_value, buff);
            
            processing_line = 3;
            continue;
        }

        if (processing_line == 3) {
            if (strstr(inbuf, "UP ") != NULL) {
                params->status_value = 1;
            } else {
                params->status_value = 0;
            }
               
            strcpy(buff, "");
            cp = strstr(inbuf, "MTU");
            if (cp != NULL)
                sscanf(cp, "MTU:%d", &params->mtu_value);
            processing_line = 4;
            continue;
        }

        if (processing_line == 4) {

            cp = strstr(inbuf, "RX packets:");
            if (cp != NULL)
                sscanf(cp, "RX packets:%lu", 
                       &params->rx_packets_value);

            cp = strstr(inbuf, "errors:");
            if (cp != NULL)
                sscanf(cp, "errors:%lu", 
                       &params->rx_errors_value);

            processing_line = 5;
            continue;
        }

        if (processing_line == 5) {

            cp = strstr(inbuf, "TX packets:");
            if (cp != NULL)
                sscanf(cp, "TX packets:%lu", 
                       &params->tx_packets_value);

            cp = strstr(inbuf, "errors:");
            if (cp != NULL)
                sscanf(cp, "errors:%lu", 
                       &params->tx_errors_value);

            processing_line = 6;
            continue;
        }

        if (processing_line == 6) {

            processing_line = 7;
            continue;
        }

        if (processing_line == 7) {

            cp = strstr(inbuf, "RX bytes:");
            if (cp != NULL)
                sscanf(cp, "RX bytes:%lu", 
                       &params->rx_bytes_value);

            cp = strstr(inbuf, "TX bytes:");
            if (cp != NULL)
                sscanf(cp, "TX bytes:%lu", 
                       &params->tx_bytes_value);

            processing_line = 0;
        }

        sprintf(ethtoolcommand, "/sbin/ethtool %s", devname);
        ethtoolpipe = popen(ethtoolcommand, "r");
        if (ethtoolpipe == NULL) {
            continue;
        }
        while (fgets(ethtoolbuf, sizeof(ethtoolbuf), ethtoolpipe)) {
            if (ethtool_line == 0) {
                ep = strstr(ethtoolbuf, "Speed");
                if (ep != NULL) {
                    strcpy(buff, "");
                    sscanf(ep, "Speed: %s", buff);
                    strcpy(params->speed_value, buff);
                    ethtool_line = 1;
                }
            }
            if (ethtool_line == 1) {
                ep = strstr(ethtoolbuf, "Duplex");
                if (ep != NULL) {
                    strcpy(buff, "");
                    sscanf(ep, "Duplex: %s", buff);
                    if (buff[0] >= 'a' && buff[0] <= 'z') 
                        buff[0] += 'A' - 'a';
                    strcpy(params->duplex_value, buff);
                    ethtool_line = 2;
                }
            }
            if (ethtool_line == 2) {
                ep = strstr(ethtoolbuf, "Auto-negotiation");
                if (ep != NULL) {
                    strcpy(buff, "");
                    sscanf(ep, "Auto-negotiation: %s", buff);
                    if (buff[0] >= 'a' && buff[0] <= 'z') 
                        buff[0] += 'A' - 'a';
                    strcpy(params->autonegotiate_value, buff);
                    ethtool_line = 3;
                }
            }
            if (ethtool_line == 3) {
                ep = strstr(ethtoolbuf, "Link detected");
                if (ep != NULL) {
                    strcpy(buff, "");
                    sscanf(ep, "Link detected: %s", buff);
                    if (buff[0] >= 'a' && buff[0] <= 'z') 
                        buff[0] += 'A' - 'a';
                    strcpy(params->linkdetected_value, buff);
                    ethtool_line = 0;
                }
            }
        }
        pclose(ethtoolpipe);
        get_link_partner(devname, params->lpa_value);
    }
    pclose(inpipe);

    return RC_NORMAL;
}
