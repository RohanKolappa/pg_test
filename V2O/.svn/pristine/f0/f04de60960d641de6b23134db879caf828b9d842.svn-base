#ifndef __VLOG_H__
#define __VLOG_H__

#include "log4c.h"

#define LOG_CATEGORY_DEFAULT "ipvs.v2o" 

#ifdef VLOG_CATEGORY_NAME
#define LOG_CATEGORY_NAME LOG_CATEGORY_DEFAULT "." VLOG_CATEGORY_NAME
#else
#define LOG_CATEGORY_NAME LOG_CATEGORY_DEFAULT 
#endif


#define VLOG_FATAL(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_FATAL, (format), ## args)

#define VLOG_FATAL_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_FATAL, (format), ## args)

#define VLOG_ALERT(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_ALERT, (format), ## args)

#define VLOG_ALERT_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_ALERT, (format), ## args)

#define VLOG_CRITICAL(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_CRIT, (format), ## args)

#define VLOG_CRITICAL_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_CRIT, (format), ## args)

#define VLOG_ERROR(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_ERROR, (format), ## args)

#define VLOG_ERROR_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_ERROR, (format), ## args)

#define VLOG_WARNING(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_WARN, (format), ## args)

#define VLOG_WARNING_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_WARN, (format), ## args)

#define VLOG_NOTICE(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_NOTICE, (format), ## args)

#define VLOG_NOTICE_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_NOTICE, (format), ## args)

#define VLOG_INFO(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_INFO, (format), ## args)

#define VLOG_INFO_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_INFO, (format), ## args)

#define VLOG_DEBUG(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_DEBUG, (format), ## args)

#define VLOG_DEBUG_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_DEBUG, (format), ## args)

#ifdef V2ODEBUG
#define VLOG_TRACE(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_TRACE, (format), ## args)

#define VLOG_TRACE_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_TRACE, (format), ## args)
#else
#define VLOG_TRACE(format, args...) \
        do {} while(0)

#define VLOG_TRACE_CAT(format, args...) \
        do {} while(0)
#endif

#define VLOG_NOTSET(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_NOTSET, (format), ## args)

#define VLOG_NOTSET_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_NOTSET, (format), ## args)

#define VLOG_UNKNOWN(format, args...) \
        vlog_message(LOG_CATEGORY_NAME, LOG4C_PRIORITY_UNKNOWN, (format), ## args)

#define VLOG_UNKNOWN_CAT(category, format, args...) \
        vlog_message(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_UNKNOWN, (format), ## args)

#define VLOG_INFO_HEXDATA(args...) \
        vlog_hexdata(LOG_CATEGORY_NAME, LOG4C_PRIORITY_INFO, ## args)

#define VLOG_INFO_HEXDATA_CAT(category, args...) \
        vlog_hexdata(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_INFO, ## args)

#define VLOG_DEBUG_HEXDATA(args...) \
        vlog_hexdata(LOG_CATEGORY_NAME, LOG4C_PRIORITY_DEBUG, ## args)

#define VLOG_DEBUG_HEXDATA_CAT(category, args...) \
        vlog_hexdata(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_DEBUG, ## args)

#define VLOG_TRACE_HEXDATA_CAT(category, args...) \
        vlog_hexdata(LOG_CATEGORY_DEFAULT "." category, LOG4C_PRIORITY_TRACE, ## args)

#define VLOG_SVR                     "svr"
#define VLOG_CLN                     "cln"
#define VLOG_HAL                     "hal"
#define VLOG_HALD                    "hald"
#define VLOG_VIF                     "VIF"
#define VLOG_VLIB                    "vlib"
#define VLOG_I2C                     "i2c"
#define VLOG_USB                     "usb"
#define VLOG_KBM                     "kbm"
#define VLOG_KBM_KB                  "kbm.kb"
#define VLOG_KBM_MS                  "kbm.ms"
#define VLOG_KBM_EV                  "kbm.ev"
#define VLOG_KBM_RFB                 "kbm.rfb"
#define VLOG_KBM_HID                 "kbm.hid"
#define VLOG_NETHDR 		         "nethdr"
#define VLOG_RTPFILTER 		         "rtpfilter"

#ifdef __cplusplus
extern "C" {
#endif

extern void vlog_message(char *catName, int a_priority, const char* a_format, ... ) 
    __attribute__ ((format (printf, 3, 4)));

extern void vlog_hexdata(char *catName, int a_priority, unsigned char *dat, int len);

extern void vlog_init(const char * a_name);

extern void vlog_finish(void);

extern void vlog_reinit(void);

extern void WriteLoggingLevel(int req_code);

#ifdef __cplusplus
}
#endif


#endif /* __VLOG_H__ */
