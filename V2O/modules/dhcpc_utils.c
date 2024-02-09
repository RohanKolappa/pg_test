#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/stat.h>

#include    "vutils.h"
#include    "vlog.h"
#include    "commonutils.h"
#include    "system_utils.h"
#include    "loadnet.h"
#include    "if_list_config.h"

#define DHCPC_EVENT_BOUND       "bound"
#define DHCPC_EVENT_DECONFIG    "deconfig"
#define DHCPC_EVENT_NAK         "nak"
#define DHCPC_EVENT_RENEW       "renew"
#define DHCPC_EVENT_LEASEFAIL   "leasefail"
#define DEFAULT_LINK_LOCAL      "169.254.0.1"


#define DNS_CONFIG_FILE "/etc/resolv.conf"
#define DNS_CONFIG_FILE_STATIC "/etc/resolv.conf.static"
#define S_DOMAIN        "domain"
#define S_NAMESERVER    "nameserver"


int dhcpc_event_main(int argc, char **argv)
{
    char buff[512];
    int retcode=0;
    if (argc < 2) {
    	printf("Need at least one argument\n");
    	exit(1);
    }
    if (getenv("interface") == NULL) {
        printf("Environment variable \"interface\" is not defined. Aborting.\n");
        exit(1);
    }
    VLOG_INFO("DHCP Client Event %s, Interface %s", argv[1], getenv("interface"));

    if (strcmp (argv[1], DHCPC_EVENT_BOUND) == 0 ||
            strcmp (argv[1], DHCPC_EVENT_RENEW) == 0) {
    	// Apply IP address
        if (getenv("ip") != NULL) {
            // Kill avahi-autoipd if running
            sprintf(buff, "/sbin/avahi-autoipd -c %s", getenv("interface"));
            retcode = system_command(buff);
            if (retcode == 0) {
                sprintf(buff, "/sbin/avahi-autoipd --kill %s", 
                        getenv("interface"));
                retcode = system_command(buff);
                // Delay so avahi-autoipd can finish running its action script
                // (fix for TSM-0090-192 - network does not come up when 
                // network cable is plugged in after box boots).
                sleep(2);
            }
            sprintf(buff, "/sbin/ifconfig %s %s",
                    getenv("interface"), getenv("ip"));
            if (getenv("subnet") != NULL) {
                strcat (buff, " netmask ");
                strcat (buff, getenv("subnet"));
            }
            if (getenv("broadcast") != NULL) {
                strcat (buff, " broadcast ");
                strcat (buff, getenv("broadcast"));
            }
            retcode = system_command(buff);
        }

        // Apply DNS configuration (write out /etc/resolv.conf)
        // DNS configuration is only honored on primary interface
        // There could be multiple DNS servers published via DHCP
        if (is_primary_interface(getenv("interface")) &&
                getenv("dns") != NULL && getenv("domain") != NULL ) {
            FILE *fp;
            int len;
            if ((fp = fopen(DNS_CONFIG_FILE, "w+")) != NULL) {
                len = fprintf(fp, "%s", S_DOMAIN);
                fprintf(fp, "%*s %s\n", 25 - len, " ", getenv("domain"));
                char * dns_server_list = strdup(getenv("dns"));
                char * dns_server = strtok(dns_server_list, " ");
                while (dns_server != NULL) {
                    if (strlen(dns_server) == 0)
                        continue;
                    len = fprintf(fp, "%s", S_NAMESERVER);
                    fprintf(fp, "%*s %s\n", 25 - len, " ", dns_server);
                    dns_server = strtok(NULL, " ");
                }
                free(dns_server_list);
                fclose(fp);
            }
        }

        // Apply default route
        if (getenv("router") != NULL) {
        	// Delete all existing default routes
            do {
                sprintf(buff, "/sbin/route del default gw 0.0.0.0 dev %s",
                    getenv("interface"));
                retcode = system_command(buff);
            } while (retcode == 0);

            // Add the new default route
            sprintf(buff, "/sbin/route add default gw %s dev %s",
                getenv("router"), getenv("interface"));
            retcode = system_command(buff);
        }

        // Save off the current environment for debugging
        sprintf(buff, "/usr/bin/env > /var/status/dhcp_%s.status",
                getenv("interface"));
        retcode = system_direct_command(buff);
        // Rewrite the host file since IP address just changed
        WriteHostFile(0);

        // Need to restart services that are affected by IP address change
        if (!is_standalone_enabled() ) {
            system_command("/bin/devappctl hup");
        }
        VLOG_INFO("Handled  Event %s", argv[1] );
    }
    else if (strcmp (argv[1], DHCPC_EVENT_DECONFIG) == 0) {

        // Kill avahi-autoipd if running
        sprintf(buff, "/sbin/avahi-autoipd -c %s", getenv("interface"));
        retcode = system_command(buff);
        if (retcode == 0) {
            sprintf(buff, "/sbin/avahi-autoipd --kill %s", getenv("interface"));
            retcode = system_command(buff);
            // Delay so avahi-autoipd can finish running its action script
            // (fix for TSM0090-192 network doesn't come up if network cable 
            // is plugged in after the box boots).
            sleep(2);
        }
        // Reset IP address
        sprintf(buff, "/sbin/ifconfig %s 0.0.0.0", getenv("interface"));
        retcode = system_command(buff);
        sprintf(buff, "/bin/rm /var/status/dhcp_%s.status",
                getenv("interface"));
        retcode = system_command(buff);

        // Reset DNS configuration
        if (is_primary_interface(getenv("interface"))) {
            FILE *fp;
            int len;
            struct stat buf;

            // Copy the static dns config file if it exists
            if (stat(DNS_CONFIG_FILE_STATIC, &buf) == 0) {
                system_command("/bin/cp " DNS_CONFIG_FILE_STATIC " " DNS_CONFIG_FILE);
            }
            else {
                if ((fp = fopen(DNS_CONFIG_FILE, "w+")) != NULL) {
                    len = fprintf(fp, "%s", S_DOMAIN);
                    fprintf(fp, "%*s %s\n", 25 - len, " ", "");
                    len = fprintf(fp, "%s", S_NAMESERVER);
                    fprintf(fp, "%*s %s\n", 25 - len, " ", "0.0.0.0");
                    fclose(fp);
                }
            }
        }

        WriteHostFile(0);
        if (!is_standalone_enabled() ) {
            system_command("/bin/devappctl hup");
        }
        VLOG_INFO("Handled  Event %s", argv[1] );
    }
    else if (strcmp (argv[1], DHCPC_EVENT_LEASEFAIL) == 0 ||
            strcmp (argv[1], DHCPC_EVENT_NAK) == 0 ) {

        sprintf(buff, "/sbin/avahi-autoipd --no-chroot -wD %s", getenv("interface"));
        retcode = system_command(buff);
        VLOG_INFO("Handled  Event %s", argv[1] );
    }
    else {
        VLOG_INFO("Unhandled  Event %s", argv[1] );
    }
    return 0;
}
