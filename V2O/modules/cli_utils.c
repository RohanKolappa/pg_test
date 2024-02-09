#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "hal.h"
#include "vutils.h"
#include "vlog.h"
#include "commonutils.h"
#include "custmenu.h"
#include "cli_utils.h"

char *yesnochecklist[] = { "Yes", "No", NULL };

char regexp_bandwidth[] = "^[[:digit:].][[:digit:].]*[kKmM]* *$"; /* Valid flow rate */

/* Any string */
char regexp_any[] = "^.*$";

/* Any non blank string */
char regexp_anynonempty[] = "^[^[:space:]].*$";

/* validation pattern for password: any printable character except space */
char regexp_password[] = "^[[:graph:]]* *$";

/* validation pattern for hostname */
char regexp_hostname[] = "^[[:alpha:]][[:alnum:]-]* *$";

void print_cli_failure(const char *groupname, const char *reason)
{
    printf(XML_GROUP, groupname);
    printf("  " XML_RESPONSE);
    printf("    " XML_ATTRSTR, CLI_STATUS_NAME, CLI_STATUS_FAILURE);
    printf("    " XML_ATTRSTR, CLI_REASON_NAME, reason);
    printf("  " XML_RESPONSEEND);
    printf(XML_GROUPEND);
}

void print_cli_failure_with_detail(const char *groupname, const char *reason,
        const char *detail)
{
    printf(XML_GROUP, groupname);
    printf("  " XML_RESPONSE);
    printf("    " XML_ATTRSTR, CLI_STATUS_NAME, CLI_STATUS_FAILURE);
    printf("    " XML_ATTRSTR_DETAIL, CLI_REASON_NAME, reason, detail);
    printf("  " XML_RESPONSEEND);
    printf(XML_GROUPEND);
}

void print_cli_success(const char *groupname)
{
    if (groupname != NULL)
        printf(XML_GROUP, groupname);
    printf("  " XML_RESPONSE);
    printf("    " XML_ATTRSTR, CLI_STATUS_NAME, CLI_STATUS_SUCCESS);
    printf("    " XML_ATTRSTR, CLI_REASON_NAME, CLI_REASON_NA);
    printf("  " XML_RESPONSEEND);
    if (groupname != NULL)
        printf(XML_GROUPEND);
}

int view_only()
{

    int gid = getgid();
    if (gid == 0 || gid == ADMIN_USER_GROUP) {
        return 0;
    }
    return 1;
}
