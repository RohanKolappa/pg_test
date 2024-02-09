#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "vlog.h"
#include "strmsrv_comm.h"
#include "vutils.h"
#include "get_license_pin.h"



static int get_mac_info(char *mac_buf) {

    FILE *fp;

    fp = fopen("/sys/class/net/eth0/address", "r");
    if (fp == NULL)
        return 0;

    fscanf(fp, "%s", mac_buf);
    fclose(fp);

    return 1;

}

static int calculate_license_pin(char *macbuf) 
{

    char *cp;
    char newstr[40] = "";
    unsigned int value;
    unsigned long long int value_ull;

    cp = strtok(macbuf, ":");

    while(cp != NULL) {

        strcat(newstr, cp);

        cp = strtok(NULL, ":");

    }
    value_ull = strtoull(newstr, NULL, 16);
    value = (unsigned int) value_ull;
    value = value & 0xFFFFF;
    value = value % 999999;


    return value;
}


int get_license_pin(void) {

    char mac_buf[40] = "";
    unsigned int value = 0;


    get_mac_info(mac_buf);
    value = calculate_license_pin(mac_buf);

    return value;

}


int get_license_pin_main(int argc, char **argv)
{

    char mac_buf[40] = "";
    unsigned int value = 0;


    get_mac_info(mac_buf);
    value = calculate_license_pin(mac_buf);

    printf("pin = %d\n", value);

    return 1;
}



















