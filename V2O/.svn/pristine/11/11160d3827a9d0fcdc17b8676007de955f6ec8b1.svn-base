#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <syslog.h>

#include "vutils.h"
#include "vlog.h"
#include "hal.h"
#include "hal_console.h"

#define BUFLEN 256
#define IDENTSTRLEN 128

static int initialized = 0;

void vlog_message(char *catName, int a_priority, const char* a_format, ... )
{
    const log4c_category_t* a_category = log4c_category_get(catName);
    if (log4c_category_is_priority_enabled(a_category, a_priority)) {
        va_list va;
        va_start(va, a_format);
        log4c_category_vlog(a_category, a_priority, a_format, va);
        va_end(va);
    }
}
void vlog_hexdata(char *catName, int a_priority, unsigned char *dat, int len)
{

    int i = 0, j = 0;
    unsigned char c1, c2;
    char buff[BUFLEN];

    const log4c_category_t* a_category = log4c_category_get(catName);
    if (log4c_category_is_priority_enabled(a_category, a_priority)) {
        for (i = j = 0; i < len; i++) {
            c1 = dat[i] / 16;
            c2 = dat[i] % 16;
            buff[j++] = (c1 < 10) ? (c1 + '0') : (c1 - 10 + 'a');
            buff[j++] = (c2 < 10) ? (c2 + '0') : (c2 - 10 + 'a');
            if (j < 23)
                buff[j++] = ' ';
            else {
                buff[j] = '\0';
                log4c_category_log(log4c_category_get(catName), a_priority, buff);
                j = 0;
            }
        }
        if (j > 0) {
            buff[j] = '\0';
            log4c_category_log(log4c_category_get(catName), a_priority, buff);
        }
    }
}

static int log4c_to_syslog_priority(int a_priority)
{
    static int priorities[] = {
        LOG_EMERG,
        LOG_ALERT,
        LOG_CRIT,
        LOG_ERR,
        LOG_WARNING,
        LOG_NOTICE,
        LOG_INFO,
        LOG_DEBUG
    };
    int result;
    
    a_priority++;
    a_priority /= 100;
    if (a_priority < 0) {
        result = LOG_EMERG;
    } else if (a_priority > 7) {
        result = LOG_DEBUG;
    } else {
        result = priorities[a_priority];
    }
    
    return result;
}


static char ident_string[IDENTSTRLEN];


static int ipvs_syslog_open(log4c_appender_t* this)
{
    openlog(ident_string, LOG_PID, LOG_USER);
    return 0;
}

static int ipvs_syslog_append(log4c_appender_t* this,
                        const log4c_logging_event_t* a_event)
{
    
    syslog(log4c_to_syslog_priority(a_event->evt_priority),
                    "%s", a_event->evt_rendered_msg);
    
    return 0;
}

static int ipvs_syslog_close(log4c_appender_t*  this)
{
    return 0;
}

const log4c_appender_type_t log4c_appender_type_syslog_user = {
    "ipvs_syslog",
    ipvs_syslog_open,
    ipvs_syslog_append,
    ipvs_syslog_close,
};

void vlog_init(const char * a_name)
{
    initialized = 1;
    memset(ident_string, 0x00, IDENTSTRLEN);
    strncpy(ident_string, a_name, IDENTSTRLEN - 1);
    /* The following is needed for vlog to work in usbhotplug */
    setenv("LOG4C_RCPATH", "/home/onair", 1);
    log4c_appender_type_set(&log4c_appender_type_syslog_user);
    log4c_init();
}

void vlog_finish()
{
    log4c_fini();
}

void vlog_reinit()
{
    if (initialized == 1) {
        VLOG_DEBUG("Reinitializing V2O logging config.");
        vlog_finish();
        vlog_init(ident_string);
    }
}

