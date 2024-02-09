

#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "xmpp_server_menu_gen.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "verify_license.h" 

#define MSSCRIPT  "/etc/scripts/watchdog/watch_managementserver.sh"
#define OPENFIREDBTEMPLATESCRIPT  "/home/onair/templates/openfire.script"
#define OPENFIRELOCALDBTEMPLATESCRIPT  "/home/onair/templates/openfire.script.localdb"
#define OPENFIREDBSCRIPT  "/data/xmppserver/embedded-db/openfire.script"
#define AUTOREGISTRATIONFILE "/etc/config/autoregistration.enabled"

static void RemoveFiles(void) {

    struct stat sbuf;

    if (stat("/etc/scripts/watch_managementserver.sh", &sbuf) == 0)
        remove("/etc/scripts/watch_managementserver.sh");

    if (stat("/bin/mserverctl", &sbuf) == 0)
        remove("/bin/mserverctl");

}


static int validateXMPPServerConfig(struct xmpp_server_params *params,
                                   struct xmpp_server_params
                                   *orig_params, char *error_string)
{
    if (is_linuxha_enabled() && invoked_via_cli == 0) {
        sprintf(error_string,
                "XMPP Server cannot be configured while\nHA is enabled");
        return RC_ERROR;
    }




    if (!isHavingDMSMSLicense(0) || !isHavingBaseLicense(0))  {
        RemoveFiles();
        sprintf(error_string,
                "Valid License to enable xmpp server on this device is not found");
        return RC_ERROR;
    }
    return RC_NORMAL;
}

int applyXMPPServer(struct xmpp_server_params *params,
                   struct xmpp_server_params *orig_params)
{
    FILE *fp=NULL;
    // Open it in append mode
    if (params->enableldap_value) {
        system_command("cp " OPENFIREDBTEMPLATESCRIPT " " OPENFIREDBSCRIPT  );
        fp = fopen ( OPENFIREDBSCRIPT, "a");
        fprintf (fp, "INSERT INTO OFPROPERTY VALUES('ldap.host','%s')\n",
                params->ldaphost_value);
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.port','636')\n");
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.sslEnabled','true')\n");
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.adminDN','%s')\n",
                params->admindn_value);
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.adminPassword','%s')\n",
                params->adminpassword);
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.baseDN','%s')\n",
                params->basedn_value);
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.debugEnabled','true')\n");
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.ldapDebugEnabled','true')\n");
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.usernameField','%s')\n",
                params->userfield_value);
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('ldap.nameField','displayName')\n");
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('provider.auth.className','org.jivesoftware.openfire.auth.HybridAuthProvider')\n");
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('hybridAuthProvider.primaryProvider.className','org.jivesoftware.openfire.ldap.LdapAuthProvider')\n");
        fprintf (fp,
                "INSERT INTO OFPROPERTY VALUES('hybridAuthProvider.secondaryProvider.className','org.jivesoftware.openfire.auth.IpvsAuthProvider')\n");
        fclose(fp);
    }
    else {
        system_command("cp " OPENFIRELOCALDBTEMPLATESCRIPT " " OPENFIREDBSCRIPT  );
    }
    fp = fopen (OPENFIREDBSCRIPT, "a");
    if (fp != NULL) {
        if (params->enablediscovery_value) {
            fprintf (fp, "INSERT INTO OFPROPERTY VALUES('register.inband','true')\n");
            system_command("/bin/touch " AUTOREGISTRATIONFILE);
        }
        else {
            fprintf (fp, "INSERT INTO OFPROPERTY VALUES('register.inband','false')\n");
            system_command("/bin/rm -f " AUTOREGISTRATIONFILE);
        }
        fclose(fp);
    }

    if (params->enable_value) {
        system_command(MSSCRIPT " on");
    } else {
        system_command(MSSCRIPT " off");
    }
    // Copy the template script file

    return RC_NORMAL;
}

void XMPPServerInit(void)
{
    registerXMPPServerValidateFunction(validateXMPPServerConfig);
}

