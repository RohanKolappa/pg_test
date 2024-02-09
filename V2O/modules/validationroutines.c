#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <arpa/inet.h>

#include "vutils.h"
#include "vlog.h"
#include "validationroutines.h"
#include "custmenu.h"
#include "commonutils.h"
#include "cli_utils.h"


/* validation pattern for domain name */
static char regexp_domainname[] = "^[[:alpha:]][[:alnum:]-]*[[:alnum:]]"
    "\\(\\.[[:alpha:]][[:alnum:]-]*[[:alnum:]]\\)* *$";

/* valid pattern for IP address */
static char regexp_ipaddress[] =
        "^\\(25[0-5]\\|2[0-4][0-9]\\|[01]\\?[0-9][0-9]\\?\\)"
            "\\(\\.\\(25[0-5]\\|2[0-4][0-9]\\|[01]\\?[0-9][0-9]\\?\\)\\)\\{3\\} *$";

/***********************************************
 * Check if the string is a valid ip address.  *
 ***********************************************/
int check_ip(const char *str)
{

    regex_t re;
    int retval;

    if (strlen(str) <= 1)
        return RC_ERROR;
    else if (!isdigit(*str))
        return RC_ERROR;

    retval = regcomp(&re, regexp_ipaddress, 0);

    if (retval == 0) {
        retval = regexec(&re, str, 0, NULL, 0);
        regfree(&re);
        if (retval == 0)
            return RC_NORMAL;
        else
            return RC_ERROR;
    }
    else {
        return RC_ERROR;
    }

    return RC_NORMAL;
}

/*************************************************
 * Check the string is integerorany without range  *
 *************************************************/

int check_integerorany(const char *str)
{
    int len = strlen(str);
    int i;

    if (strcasecmp(str, "any") == 0 ) {
        return RC_NORMAL;
    }
    else {
        for(i=0; i<len; i++) {
            if (!isdigit(str[i]))
              return RC_ERROR;
        }
        return RC_NORMAL;
    }
    return RC_ERROR;
}

/**********************************************
 * Check the string is integerorany with range  *
 **********************************************/

int check_integeroranyrange(const char *str, int min, int max)
{
    int retval = RC_NORMAL;

    retval = check_integerorany(str);

    if (retval != RC_NORMAL)
      return RC_ERROR;

    if (strcasecmp(str, "any") != 0)  {

      if(atoi(str) < min || atoi(str) > max)
        return RC_ERROR;
    }

    return RC_NORMAL;
}

/*************************************************************
 * Check the string is integerlist with comma's and in range  *
* ************************************************************/

int check_integerlist_range(const char *str, int min, int max)
{
    int len = strlen(str);
    char *res = NULL;

    // if first and last string values is comma return error
    if (*str == ',' || *(str + len-1) == ',') {
        return RC_ERROR;
    }

    //if it is neither comma nor digit return error
    const char * cp = str;
    while (*cp != '\0') {
        if ((*cp != ',') && 
            !isdigit(*cp)) {
                return RC_ERROR;
        }
        cp++;
    }
    char *string = strdup(str);
    //Tokenize string into individual tokens
    res = strtok(string, ",");
        
    while(res != NULL) {
        if (atoi(res) < min || atoi(res) > max) 
            return RC_ERROR; 
 
        res = strtok(NULL, ",");
    }
    free(string);
    return RC_NORMAL;
}

/***********************************************
 *  * Check if the string is a valid ip address or blank.  *
 *   ***********************************************/
int check_ipaddorblank(const char *str)
{

      /* allow empty strings */
    if (strlen(str) == 0)
        return RC_NORMAL;
 
    return check_ip(str);
  
}

/**********************************************
 *  * Check if the string is a valid Hostname or blank *
 *   **********************************************/
int check_hostnameorblank(const char *str)
{
    regex_t re;
    int retval;

    /* allow empty strings */
    if (*str == '\0')
        return RC_NORMAL;

    retval = regcomp(&re, regexp_hostname, 0);

    if (retval == 0) {
        retval = regexec(&re, str, 0, NULL, 0);
        regfree(&re);
        if (retval == 0)
            return RC_NORMAL;
    }

    return RC_ERROR;
}

/*********************************************
 * Check the netmask string for proper range *
 *********************************************/
int check_netmask(const char *str)
{
    int retval = RC_NORMAL;
    u_long tmp;
    int i;
    struct in_addr in;

    if (strlen(str) == 0)
        return RC_ERROR;

    retval = check_ip(str);
    if (retval != RC_NORMAL)
        return RC_ERROR;

    tmp = inet_aton(str, &in);

    if (tmp == 0)
        return RC_ERROR;

    tmp = ntohl(in.s_addr);
    for (i = 0; i < 32; i++) {
        if (tmp & 0x01)
            break;
        tmp >>= 1;
    }

    for (; i < 32; i++) {
        if (!(tmp & 0x01)) {
            retval = RC_ERROR;
            break;
        }
        tmp >>= 1;
    }

    return retval;
}

/*******************************************
 * Check the multicast ip for proper range *
 *******************************************/
int check_multicastip(const char *str)
{
    int retval = RC_NORMAL;
    u_long tmp;
    struct in_addr in;

    if (strlen(str) == 0)
        return RC_ERROR;

    retval = check_ip(str);
    if (retval != RC_NORMAL)
        return RC_ERROR;

    tmp = inet_aton(str, &in);

    if (tmp == 0)
        return RC_ERROR;

    tmp = ntohl(in.s_addr);

    /* Check if address is between 224.0.0.0 and 239.255.255.255 */
    if (tmp < 0xe0000000 || tmp > 0xefffffff)
        return RC_ERROR;

    return retval;
}

/**********************************************
 * Check if the string is a valid domain name *
 **********************************************/
int check_domainname(const char *str)
{
    regex_t re;
    int retval;

    /* allow empty strings */
    if (*str == '\0')
        return RC_NORMAL;

    retval = regcomp(&re, regexp_domainname, 0);

    if (retval == 0) {
        retval = regexec(&re, str, 0, NULL, 0);
        regfree(&re);
        if (retval == 0)
            return RC_NORMAL;
    }

    return RC_ERROR;
}

int check_enum(const char *string, char *array[])
{
    int status = RC_ERROR, i;
    for (i = 0; array[i] != NULL; i++) {
        if (strcasecmp(string, array[i]) == 0) {
            status = RC_NORMAL;
            break;
        }
    }
    return status;
}

int check_regex(const char *string, char *pattern)
{
    int status;
    regex_t re;

    if ((status = regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB)) != 0) {
        VLOG_WARNING("Could not compile regular expression %s", pattern);
        return RC_ERROR; /* error */
    }
    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) {
        return RC_ERROR; /* error */
    }
    else {
        return RC_NORMAL; /* match */
    }
}

/**********************************************
 * Check if the string is a valid IP address  *
 * or fully qualified host name               *
 **********************************************/
int check_inetname(const char *str)
{
    int retval;

    if (*str == '\0')
        return RC_NORMAL;

    else if (!isdigit(*str))
        return check_domainname(str);

    retval = check_ip(str);
    if (retval != RC_NORMAL)
        return RC_ERROR;

    return RC_NORMAL;
}

int check_hexadecimal(const char *string)
{
    int len = strlen(string);
    int i = 0;
    if ((strncmp(string, "0x" , 2) == 0) || (strncmp(string, "0X" , 2) == 0)) { 
        for (i=2; i<len; i++) {
            if (!isxdigit(string[i])) 
                return RC_ERROR;
        }
        return RC_NORMAL;
    }
    else {
        for (i=0; i<len; i++) {
            if (!isdigit(string[i])) 
                return RC_ERROR;
        }
        return RC_NORMAL;
    }
}

int get_hexadecimal(const char *string)
{
    int value = 0;
    if ((strncmp(string, "0x" , 2) == 0) || (strncmp(string, "0X" , 2) == 0)) 
        sscanf(string, "%x", &value);
    else
        sscanf(string, "%d", &value);
    return value;
}

int check_onlyhexadecimal(const char *string)
{
    int len = strlen(string);
    int i = 0;
    if ((strncmp(string, "0x" , 2) == 0) || (strncmp(string, "0X" , 2) == 0)) {
        i = 2;
    }
    else {
        i = 0;
    }
    for (; i<len; i++) {
        if (!isxdigit(string[i])) 
            return RC_ERROR;
    }
    return RC_NORMAL;
}

int get_onlyhexadecimal(const char *string)
{
    int value = 0;
    sscanf(string, "%x", &value);
    return value;
}

