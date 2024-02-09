#include <stdlib.h>
#include <string.h>
#include <crypt.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "admin_menu_gen.h"
#include "vlog.h"

#define SHADOW_TEMPLATE_FILE "/home/onair/templates/shadow"
#define SHADOW_FILE "/etc/shadow"


/* Prototype for applyAdminAccount function */
int applyAdminAccount(struct admin_params *params, 
                             struct admin_params *orig_params) 
{
    char buff[128];
    char *pwd;
    if (params->password_set) {
        pwd = crypt(params->password_value, "$1$ZC$");

        if (pwd == NULL || strlen(pwd) == 0)
            return RC_ERROR;
        if (verbose_errors) {
            printf("Setting Admin Account Parameters\n");
        }
        sprintf(buff, "/bin/sed -e 's#PASSWORD#%s#' %s > %s", 
                pwd, SHADOW_TEMPLATE_FILE, SHADOW_FILE);
        system_direct_command(buff);

    }
    
    return RC_NORMAL;
}

