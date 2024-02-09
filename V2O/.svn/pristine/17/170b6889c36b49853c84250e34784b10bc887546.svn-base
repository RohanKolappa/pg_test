#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "dns_menu_gen.h"
#include "if_list_config.h"
#include "host_list_menu_gen.h"
#include "loadnet.h"
#include "system_utils.h"

#define DNS_CONFIG_FILE "/etc/resolv.conf"
#define DNS_CONFIG_FILE_STATIC "/etc/resolv.conf.static"
#define S_DOMAIN        "domain"
#define S_NAMESERVER    "nameserver"

void UpdateHostFile(void);

int applyDNSClient(struct dns_client_params *params,
                          struct dns_client_params *orig_params)
{

    FILE *fp;
    int len;

    loadHostList();

    /* Set host name */
    if (params->hostname[0]) {
        int retcode = sethostname(params->hostname_value,
                                  strlen(params->hostname_value));
        if (retcode != 0 && verbose_errors) {
            printf("Could not set hostname\n");
        }
    }

    /* Write out resolve.conf */
    if ((fp = fopen(DNS_CONFIG_FILE_STATIC, "w+")) != NULL) {

        len = fprintf(fp, "%s", S_DOMAIN);
        fprintf(fp, "%*s %s\n", 25 - len, " ", params->localdomain_value);
        len = fprintf(fp, "%s", S_NAMESERVER);
        fprintf(fp, "%*s %s\n", 25 - len, " ", params->dnsservip_value);

        fclose(fp);
    }

    if (!is_dhcp_enabled()) {
        system_command("/bin/cp " DNS_CONFIG_FILE_STATIC " " DNS_CONFIG_FILE);
    }
    return RC_NORMAL;
}

void UpdateHostFile()
{
    WriteHostFile(0);
}

void DNSClientInit(){
    registerDNSClientSavedFunction(UpdateHostFile);
}
