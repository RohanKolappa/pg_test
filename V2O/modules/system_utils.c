#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>

#include <net/if.h>

#include "v2d.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "text_utils.h"
#include "cli_utils.h"

char *get_cpu_usage(char *buf, void *usrptr, int ident)
{

    static FILE *fp = NULL;
    unsigned long userval, systemval, niceval, idleval, totalval;
    unsigned long userval_d, systemval_d, niceval_d, idleval_d;
    static unsigned long userval_sav = 0, systemval_sav = 0, niceval_sav = 0,
            idleval_sav = 0;
    float scale;

    // enough for a /proc/stat CPU line (not the intr line)
    char lbuf[256];

    /* by opening this file once, we'll avoid the hit on minor page faults
     (sorry Linux, but you'll have to close it for us) */
    if (!fp) {
        if (!(fp = fopen("/proc/stat", "r")))
            goto loc_error;
    }
    rewind(fp);
    fflush(fp);

    // first value the last slot with the cpu summary line
    if (!fgets(lbuf, sizeof(lbuf), fp))
        goto loc_error;

    if (4 > sscanf(lbuf, "cpu %lu %lu %lu %lu", &userval, &niceval, &systemval,
            &idleval))
        goto loc_error;

    userval_d = userval - userval_sav;
    systemval_d = systemval - systemval_sav;
    niceval_d = niceval - niceval_sav;
    idleval_d = idleval - idleval_sav;

    totalval = userval_d + systemval_d + niceval_d + idleval_d;
    if (1 > totalval)
        totalval = 1;
    scale = 100.0 / (float) totalval;

    sprintf(buf, "Usr %5.1f Sys %5.1f Idl %5.1f", userval_d * scale,
            systemval_d * scale, idleval_d * scale);

    userval_sav = userval;
    systemval_sav = systemval;
    niceval_sav = niceval;
    idleval_sav = idleval;

    return buf;

    loc_error: sprintf(buf, "Not Available.");
    return buf;

}

/*******************************************
 * Returns 1 if proc 'pidName' is running  *
 *******************************************/
int find_proc_by_name(char *pidName)
{
    static DIR *dir = NULL;
    struct dirent *entry;
    char *name;
    int pid;
    char status[32];
    char buf[1024];
    FILE *fp;
    struct stat sb;
    char short_cmd[16];

    if (!dir) {
        dir = opendir("/proc");
        if (!dir) {
            return 0;
        }
    }
    for (;;) {
        if ((entry = readdir(dir)) == NULL) {
            closedir(dir);
            dir = NULL;
            return 0;
        }

        name = entry->d_name;
        if (!(*name >= '0' && *name <= '9')) {
            continue;
        }
        pid = atoi(name);
        sprintf(status, "/proc/%d/stat", pid);

        if ((fp = fopen(status, "r")) == NULL)
            continue;

        if (fstat(fileno(fp), &sb))
            continue;

        name = fgets(buf, sizeof(buf), fp);
        fclose(fp);
        if (name) {
            name = strrchr(buf, ')'); /* split into "PID(cmd" and "<rest>" */
            if (name && name[1] == ' ') {
                *name = 0;
                memset(short_cmd, 0, sizeof(short_cmd));
                sscanf(buf, "%*s (%15c", short_cmd);
                if (strcmp(pidName, short_cmd) == 0) {
                    return 1;
                }
            }
        }
    }
}

/******************************************************
 * Get IP address & netmask for interface (IPv4 only).*
 ******************************************************/
int get_if_info(char *ifname, struct in_addr *ap, struct in_addr *mp)
{
    int sd;
    struct ifreq ifr;
    struct sockaddr_in *sin;

    /***********************************************************
     * open a socket. any socket will do so we choose UDP and  *
     * it with an ioctl call what devices are behind it.       *
     ***********************************************************/
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(sd, SIOCGIFADDR, &ifr) == 0) {
        sin = (struct sockaddr_in *) &ifr.ifr_addr;
        ap->s_addr = sin->sin_addr.s_addr;
    }
    else
        memset(ap, 0, sizeof(struct in_addr));

    strcpy(ifr.ifr_name, ifname);
    if (ioctl(sd, SIOCGIFNETMASK, &ifr) == 0) {
        sin = (struct sockaddr_in *) &ifr.ifr_netmask;
        mp->s_addr = sin->sin_addr.s_addr;
    }
    else
        memset(mp, 0, sizeof(struct in_addr));

    close(sd);

    return 0;
}

// Returns value for system-wide persistent variable by name
int get_persistent_int(char *name)
{
    FILE *fp;
    char fname[32], str[32];

    sprintf(fname, "/tmp/%s", name);
    if ((fp = fopen(fname, "r")) == NULL) {
        if (errno == ENOENT) {
            if ((fp = fopen(fname, "a")) == NULL) 
                return -1;
            fprintf(fp, "0\n");
            rewind(fp);
        }
        else {
            return -1;  /* Default value -1 */
        }
    }
    fgets(str, 32, fp);
    fclose(fp);
    return atoi(str);
}

// Sets value for system-wide persistent variable by name
void set_persistent_int(char *name, int val)
{
    FILE *fp;
    char fname[32];
    sprintf(fname, "/tmp/%s", name);
    if ((fp = fopen(fname, "w+")) == NULL) {
        return;
    }
    fprintf(fp, "%d\n", val);
    fclose(fp);
}

// Return IP address/netmask (as a strings) for a given 
// interface name (e.g. eth0)
int get_interface_info(char *ifname,  char *ipaddress, char *netmask)
{
    int rc;
    struct in_addr ip_addr;
    struct in_addr netmask_addr;
    rc = get_if_info(ifname, &ip_addr, &netmask_addr);
    if (rc == -1)
        return -1;
    strcpy(ipaddress, inet_ntoa(ip_addr));
    strcpy(netmask, inet_ntoa(netmask_addr));
    return 0;
}

/* Issue system command sending both stdout and stderr to /dev/null */
int system_command(char *command)
{
    char *buff;
    int rc;
    if ((buff = malloc(8196)) == NULL) {
        VLOG_WARNING_CAT("config","Unable to malloc");
        return -ENOMEM;
    }
    sprintf(buff, "%s > /dev/null 2>&1", command);
    rc = system(buff);
    if (rc != 0)
        VLOG_WARNING("Failed Executing Command %s. err %d.", buff, rc);
    free(buff);

    return rc;
}

/* Issue system command sending only stderr to /dev/null */
int system_direct_command(char *command)
{
    char buff[256];
    int rc;
    sprintf(buff, "%s  2>/dev/null", command);
    VLOG_DEBUG("Executing System Command %s", buff);
    rc = system(buff);
    if (rc != 0)
        VLOG_DEBUG("Failed Executing Command %s", buff);
    return rc;
}

/* Run a system command and grab its stdout (stderr goes to /dev/null). */
/* User is expected to "free" the returned buffer */
char * get_command_buffer(char *command)
{
    char *buf;
    FILE *inpipe;
    char inbuf[512];
    char cmdbuff[256];

    sprintf(cmdbuff, "%s 2>/dev/null", command);

    buf = strdup("\n");
    inpipe = popen(cmdbuff, "r");

    if (inpipe != NULL) {
        while (fgets(inbuf, sizeof(inbuf), inpipe)) {
            buf = strgrowcat(buf, inbuf);
        }
        pclose(inpipe);
    }

    return buf;
}

/* Routine to read hex data from file */

#define BUFLEN           256
#define MAX_LINE_SIZE    128

static unsigned long readhex(unsigned char **bufptr) {
    unsigned char *cptr = *bufptr;
    unsigned char ch = *cptr;
    unsigned long val = 0;
                                                                                
    if (isxdigit(ch)) {
        do {
            val <<= 4;
            val += (isdigit(ch)) ? (ch - '0') : (toupper(ch) - 'A' + 10);
            cptr++;
            ch = *cptr;
        } while (isxdigit(ch));
        *bufptr = cptr;
    }
    else if (!isspace(ch)) {
        *bufptr = NULL;
        VLOG_INFO("%s: bad char 0x%02x/%c in hex file", __FUNCTION__, ch, ch);
    }
    return val;
}

int read_hex_from_file(FILE *file,
                       unsigned char dat[],
                       int maxlen,
                       int longformat) {

    unsigned char buf[MAX_LINE_SIZE], *bufptr, ch;
    int n = 0;
                                                                                
    /* round up 'maxlen' to 4-byte boundary */
    if (longformat)
        maxlen = ((maxlen + 3) / 4) * 4;
                                                                                
    /* read from file 'file', if longformat = 1 convert to long values, */
    /* store at most 'maxlen' of the values into the array 'dat'        */
    rewind(file);
    while (fgets((char *)buf, MAX_LINE_SIZE, file) != NULL && n < maxlen) {
        buf[MAX_LINE_SIZE - 1] = '\0';
        if (strlen((char *)buf) == MAX_LINE_SIZE-1 && buf[MAX_LINE_SIZE-2] != '\n') {
             VLOG_INFO("%s: input file line too long", __FUNCTION__);
             return -1;
        }
        if (buf[0] == '\n') continue;
                                                                                
        bufptr = buf;
        while ((ch = *bufptr) && isspace(ch)) bufptr++;
                                                                                
        while ((ch = *bufptr) != 0 && ch != '\n') {
            if (longformat) {
                unsigned long result;
                if (n > maxlen - 4) break;
                result = readhex(&bufptr);
                dat[n++] = (unsigned char) (result & 0x000000ff);
                dat[n++] = (unsigned char) ((result & 0x0000ff00) >> 8);
                dat[n++] = (unsigned char) ((result & 0x00ff0000) >> 16);
                dat[n++] = (unsigned char) ((result & 0xff000000) >> 24);
            }
            else {
                dat[n++] = (unsigned char) readhex(&bufptr);
            }
                                                                                
            if (bufptr == NULL)
                return 0;
            while ((ch = *bufptr) && isspace(ch)) bufptr++;
        }
    }

    /* return number of values stored in 'dat' */
    return n;
}

/* Routine to read attribute values from Linux sysfs */
int get_sysfs_attribute(const char *filename, const char *scanfmt) {
    char str[128];
    int fd, rc;
    struct stat astat;

    if (lstat(filename, &astat) != 0) {
        return -1;
    }

    if (S_ISREG(astat.st_mode) != 1) {
        return -1;
    }

    if ((fd = open(filename, O_RDONLY)) < 0) {
        return -1;
    }

    rc = read(fd, str, sizeof(str));
    if (rc < 0) {
        close(fd);
        return -1;
    }
    close(fd);

    sscanf(str, scanfmt, &rc);

    return rc;
} 

