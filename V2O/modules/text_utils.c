#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <arpa/inet.h>

#include "text_utils.h"

int bits_to_bytes(const char *bitstring)
{
    char *cc, *br;
    double rate;
    int brate = 0;

    br = strdup(bitstring);

    if (((cc = strchr(br, 'k')) != NULL) || ((cc = strchr(br, 'K')) != NULL)) {
        *cc = '\0';
        sscanf(br, "%lf", &rate);
        brate = rint(rate * 1024.0 / 8.0);
    }
    else if (((cc = strchr(br, 'm')) != NULL) || ((cc = strchr(br, 'M'))
            != NULL)) {
        *cc = '\0';
        sscanf(br, "%lf", &rate);
        brate = rint(rate * 1024.0 * 1024.0 / 8.0);
    }
    else {
        sscanf(br, "%lf", &rate);
        brate = rint(rate / 8.0);
    }

    free(br);
    if (brate < 0) {
      return 0;
    } 
    return brate;
}

char *bytes_to_bits(int bytes, char *bitstring, int maxlen)
{
    if (bytes < 0) 
       bytes = 0;

    if (maxlen <= 0) //This returns the bitstring if negative or zero
       return bitstring;

    double bits = (double) (bytes * 8.0);
     
    if (bits < 1024.0)
        snprintf(bitstring, maxlen, "%.2f", bits);
    
    else if (bits < 1024.0 * 1024.0)
        snprintf(bitstring, maxlen, "%.2fK", bits / 1024.0);
    
    else 
        snprintf(bitstring, maxlen, "%.2fM", bits / (1024.0 * 1024.0));
    
    return bitstring;
}

char *bytes_to_string(char *buf, unsigned long bytes)
{
    if (bytes < 1024)
        sprintf(buf, "%lu", bytes);
    else if (bytes < 1024 * 1024)
        sprintf(buf, "%.2f KB", bytes / 1024.0);
    else if (bytes < 1024 * 1024 * 1024)
        sprintf(buf, "%.2f MB", bytes / (1024.0 * 1024.0));
    else
        sprintf(buf, "%.2f GB", bytes / (1024.0 * 1024.0 * 1024.0));

    return buf;
}

char *bytes_l_to_string(char *buf, unsigned long long bytes)
{

    if (bytes < 1024)
        sprintf(buf, "%llu", bytes);
    else if (bytes < 1024 * 1024)
        sprintf(buf, "%.2f KB", bytes / 1024.0);
    else if (bytes < 1024 * 1024 * 1024)
        sprintf(buf, "%.2f MB", bytes / (1024.0 * 1024.0));
    else
        sprintf(buf, "%.2f GB", bytes / (1024.0 * 1024.0 * 1024.0));

    return buf;
}

/***********************************************************
 * Trim string (remove white space) from beginning and end *
 ***********************************************************/
char *trim_string(char *str)
{

    char *cp = str;

    if (str == NULL)
        return NULL;

    while (*cp != '\0')
        cp++;
    cp--;

    while (*cp == ' ' || *cp == '\t' || *cp == '\r' || *cp == '\n')
        cp--;

    cp++;

    *cp = '\0';

    cp = str;
    while (*cp == ' ' || *cp == '\t')
        cp++;

    return cp;

}

// TODO quick hack, clean this
#define RC_NORMAL               0
#define RC_ERROR                -1

/*********************************************
 * Increments IP address by the given number *
 *********************************************/
int increment_ip(char *str, int increment)
{
    int retval = RC_NORMAL;
    u_long ipaddress;
    struct in_addr in;
    int rc;

    if (strlen(str) == 0)
        return RC_ERROR;

    rc = inet_aton(str, &in);

    if (rc == 0)
        return RC_ERROR;

    ipaddress = ntohl(in.s_addr);

    ipaddress = (ipaddress & 0xffffff00) | ((ipaddress & 0xff) + increment);

    in.s_addr = htonl(ipaddress);

    strcpy(str, inet_ntoa(in));
    return retval;
}

/* Parse name=value pair parameter from a line */
char *parse_status_line(char *buffer, char *parameter)
{
    int len = strlen(parameter);
    if (strncmp(buffer, parameter, len) != 0)
        return NULL;

    if ((*(buffer + len)) != '=')
        return NULL;
    char *cp = strchr(buffer + len, '\n');
    if (cp != NULL)
        *cp = '\0';

    if ((*(buffer + len + 1)) == '\"') {
        char *cp = strrchr(buffer + len + 1, '"');
        if (cp != NULL)
            *cp = '\0';

        return buffer + len + 2;
    }

    return buffer + len + 1;
}

char *strgrowcat(char *dest, const char *src)
{
    if (dest == NULL || src == NULL)
        return dest;

    char *newbuffer = realloc(dest, (strlen(dest) + strlen(src) + 1) * sizeof(char));
    if (newbuffer != NULL) {
        strcat(newbuffer, src);

    }
    return newbuffer;
}

