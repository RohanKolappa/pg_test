#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <regex.h>
#include <linux/input.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/file.h>

#include "commonutils.h"
#include "stand_alone_mode_menu_gen.h"
#include "xmpp_server_menu_gen.h"
#include "linuxha_menu_gen.h"
#ifdef __XPI__
#include "netvizxpi/netvizxpi.h"
#endif

#define RC_NORMAL               0
#define RC_ERROR                -1

int get_time_diff(struct timeval *last_time)
{
    struct timeval now;
    gettimeofday(&now, (struct timezone *) 0);
    int timdiff = 1;
    int timediff = TIMEVAL_DIFF(now, (*last_time));
    if (timediff > 1000)
        timdiff = timediff;
    *last_time = now;

    return timdiff;
}

int is_standalone_enabled() {
    struct adminop_params adminop_params;
    retrieveStandAloneValues(&adminop_params);
    if (adminop_params.standalone_value == 1)
        return 1;
    return 0;
}

int is_xmppserver_enabled() {
    struct xmpp_server_params xmpp_server_params;
    retrieveXMPPServerValues(&xmpp_server_params);
    if (xmpp_server_params.enable_value == 1)
        return 1;
    return 0;
}

int is_linuxha_enabled() {
    struct ha_params ha_params;
    retrieveHAValues(&ha_params);
    return (ha_params.enable_value == 1);
}
