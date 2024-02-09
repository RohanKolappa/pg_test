#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <regex.h>
#include <netdb.h>

#include "custmenu.h"
#include "menuutils.h"

#include "vlog.h"

#if 0
/* Any string */
char regexp_any[] = "^.*$";

/* Any non blank string */
char regexp_anynonempty[] = "^[^[:space:]].*$";

/* validation pattern for password: any printable character except space */
char regexp_password[] = "^[[:graph:]]* *$";

/* validation pattern for hostname */
char regexp_hostname[] = "^[[:alpha:]][[:alnum:]-]* *$";
#endif

int invoked_via_cli = 0;

/*******************************************************
 * Get IP address for 'hostname' and write it to 'buf' * 
 *******************************************************/
void print_ip_addr(char *buf, char *hostname)
{
    struct hostent *host;
    u_int8_t split[4];
    u_int32_t ip;
    u_int32_t *x;

    if ((host = gethostbyname(hostname)) != NULL) {
        x = (u_int32_t *) host->h_addr_list[0];
        ip = ntohl(*x);
        split[0] = (ip & 0xff000000) >> 24;
        split[1] = (ip & 0x00ff0000) >> 16;
        split[2] = (ip & 0x0000ff00) >> 8;
        split[3] = (ip & 0x000000ff);
        sprintf(buf, "%d.%d.%d.%d", split[0], split[1], split[2], split[3]);
    }
    else
        buf[0] = '\0';
}

/*************************************************
 * Scans string for decimal or hex integer value * 
 *************************************************/
int scanval(char *buf, int *val)
{
    int rc;
    if (buf == NULL)
        return RC_ERROR;
    /*********************************************
     * check if supplied value is in hexadecimal *
     *********************************************/
    if (buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) {
        rc = sscanf(buf, "0x%x", val);
        if (rc < 1)
            return RC_ERROR;
        return 0;
    }

    /********************************
     * Assume decimal integer value *
     ********************************/
    rc = sscanf(buf, "%d", val);
    if (rc < 1)
        return RC_ERROR;
    return 0;

}

/*************************************************
 * Create string representation of Bit positions *
 *************************************************/
void bitstring(char *buf, const char *bitpattern, int bitval)
{
    int i;
    char *bp = buf;

    if (buf == NULL || bitpattern == NULL)
        return;

    if ((i = strlen(bitpattern)) == 0) {
        buf[0] = '\0';
        return;
    }
    i--;
    for (; i >= 0; i--) {
        if (bitval & (1 << i)) {
            if (*bitpattern != ' ')
                *bp++ = *bitpattern;
        }
        bitpattern++;
    }
    *bp = '\0';
}

int get_persitent_int(char *name)
{

    FILE *fp;
    char fname[32];

    strcpy(fname, "/tmp/");
    strncat(fname, name, 24);

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(fname, "r")) == NULL) {
        return -1; /* Default value -1 */
    }

    fgets(fname, 32, fp);

    fclose(fp);

    return atoi(fname);

}

void set_persitent_int(char *name, int value)
{

    FILE *fp;
    char fname[32];

    strcpy(fname, "/tmp/");
    strncat(fname, name, 24);

    /********************************
     * Open config file for writing *
     ********************************/
    if ((fp = fopen(fname, "w+")) == NULL) {
        return;
    }

    fprintf(fp, "%d\n", value);

    fclose(fp);

}

