

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "linuxha_menu_gen.h"
#include "if_list_menu_gen.h"
#include "vlog.h"

// The menu writes the new HA config to the *.new files, the 
// watch linuxha script copies the *.new files to the real HA
// config files.
#define TEMPLATEDIR "/home/onair/templates/"

#define HACONF_TEMPLATE_FILE TEMPLATEDIR"ha.cf"
#define HACONF_FILE "/etc/ha.d/ha.cf.new"

#define HARES_TEMPLATE_FILE TEMPLATEDIR"haresources"
#define HARES_FILE "/etc/ha.d/haresources.new"

#define AUTHKEYS_TEMPLATE_FILE TEMPLATEDIR"authkeys"
#define AUTHKEYS_FILE "/etc/ha.d/authkeys.new"

#define LINUXHASCRIPT  "/etc/scripts/watchdog/watch_linuxha.sh"

static int validateHAConfig(struct ha_params *params,
        struct ha_params *orig_params, char *error_string)
{

     struct if_list_params *paramsp;
     retrieveIFListValues();
     int interface_found = 0; 

     foreach_if_list_entry(paramsp)  {
        if (params->enable_value){
           if (paramsp->enable_value) {
               if (params->interface_value ==
                       (enum ha_interface)  paramsp->port_value)
                   interface_found = 1;
           }
            
        } else {   
              interface_found = 1;
        }
     }
      
     if (interface_found == 0) {
        sprintf (error_string, "Interface not found or is disabled");
        return RC_ERROR;  
     }   
 
     if (!is_xmppserver_enabled()) {
        sprintf(error_string,
            "High Availability cannot be configured while\nXMPP Server is disabled");
        return RC_ERROR;
     }

     if (params->enable_value) {             
         if ((strlen(params->peerip_value) == 0) || (strcmp(params->peerip_value,"0.0.0.0")) == 0) {
           sprintf (error_string, "Peer IP cannot be blank or 0.0.0.0 if HA is enabled ");
           return RC_ERROR;
         }
         if (strlen(params->peername_value) == 0) {
            sprintf (error_string, "Peer name cannot be blank if HA is enabled");
            return RC_ERROR;
         }
         if (params->enable_virtual_ip_value) {
            if ((strlen(params->virtualip_value) == 0) || (strcmp(params->virtualip_value, "0.0.0.0")) == 0) {
               sprintf (error_string, "Virtual IP cannot be blank or 0.0.0.0 if Virtual IP and HA is enabled ");
               return RC_ERROR;
            }
         }
     }              
     return RC_NORMAL;
}
int applyHA(struct ha_params *params, 
                   struct ha_params *orig_params)
{
    char buff[128];
    char ipbuff[255];
    if (params->enable_value) {

        char hostname[255];
        memset(hostname, 0, 255);
        char *interface;

        if (verbose_errors) {
            printf("Configuring HA...\n");
        }

        gethostname(hostname, 255);

        // ---------------- setup ha.cf ---------------- 
        sprintf(buff, "/bin/cp -f %s %s", 
                HACONF_TEMPLATE_FILE, HACONF_FILE);
        system_command(buff);
 
        interface = params->interface_value == HA_INTERFACE_1 ?
              "eth0" : "eth1";
        sprintf(buff, "/bin/sed -i -e 's/IFNAME/%s/' %s", 
            interface, HACONF_FILE);
        system_command(buff);
       
        sprintf(buff, "/bin/sed -i -e 's/PEERIPADDRESS/%s/' %s", 
                params->peerip_value, HACONF_FILE);
        system_command(buff);
        sprintf(buff, "/bin/sed -i -e 's/SELFNODENAME/%s/' %s", 
                hostname, HACONF_FILE);
        system_command(buff);
        sprintf(buff, "/bin/sed -i -e 's/PEERNODENAME/%s/' %s", 
                params->peername_value, HACONF_FILE);
        system_command(buff);
        
        // ---------------- setup haresources ---------------- 
        sprintf(buff, "/bin/cp -f %s %s", 
                HARES_TEMPLATE_FILE, HARES_FILE);
        system_command(buff);

        if(params->enable_as_primary_value) {
            sprintf(buff, "/bin/sed -i -e 's/PRIMARYNODE/%s/' %s", 
                    hostname, HARES_FILE);
        }
        else {
            sprintf(buff, "/bin/sed -i -e 's/PRIMARYNODE/%s/' %s", 
                    params->peername_value, HARES_FILE);
        }
        system_command(buff);

        if(params->enable_as_primary_value) {
            sprintf(buff, "/bin/sed -i -e 's/SECONDARYNODE/%s/' %s", 
                    params->peername_value, HARES_FILE);
        }
        else {
            sprintf(buff, "/bin/sed -i -e 's/SECONDARYNODE/%s/' %s", 
                    hostname, HARES_FILE);
        }
        system_command(buff);

        if(params->enable_virtual_ip_value) {
            if ((strlen(params->virtualip2_value) != 0) && (strcmp(params->virtualip2_value, "0.0.0.0")) != 0 ) { 
                sprintf(ipbuff,"%s %s",  params->virtualip_value, params->virtualip2_value);
                sprintf(buff, "/bin/sed -i -e 's/VIRTUALIP/%s/' %s",
                    ipbuff, HARES_FILE);       
            }
            else {
                sprintf(buff, "/bin/sed -i -e 's/VIRTUALIP/%s/' %s", 
                    params->virtualip_value, HARES_FILE);
            }
        }
        else {
            sprintf(buff, "/bin/sed -i -e 's/VIRTUALIP//' %s", 
                    HARES_FILE);
        }
        system_command(buff);

        // ---------------- setup authkeys ---------------- 
        sprintf(buff, "/bin/cp -f %s %s", 
                AUTHKEYS_TEMPLATE_FILE, AUTHKEYS_FILE);
        system_command(buff);
        
        system_command(LINUXHASCRIPT " hup");
        system_command(LINUXHASCRIPT " on");
    } else {
        if (verbose_errors) {
            printf("Disabling HA...\n");
        }

        // We cannot delete the HA configuration files because
        // the watchdog off command is asynchronous and the HA
        // scripts work only if the conf files exist.
        system_command(LINUXHASCRIPT " off");
    }
                   
    return RC_NORMAL;
}

void HAInit(void)
{
    registerHAValidateFunction(validateHAConfig);
}
