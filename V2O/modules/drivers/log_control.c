
#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/timer.h>  
#include <linux/sched.h>  
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>

#include "fpga.h"
#include "log_control.h"

extern int consoleFd;

static int      show_time = FALSE;

static char *log_array[6] = {LOG_STR_NONE, 
                             LOG_STR_CRITICAL,
                             LOG_STR_WARNING,
                             LOG_STR_INFO,
                             LOG_STR_DEBUG,
                             LOG_STR_DETAIL};

/* Initial value for log level enables all modules for logging levels
   info and above */
unsigned long current_log_flags = \
    LOG_ENABLE | LOG_MOD_ALL | \
    LOG_LEV_CRITICAL | LOG_LEV_WARNING | LOG_LEV_INFO;

/* This function called for logging (printfs) to the console */
int log_print(unsigned long log_flags, 
              const char *fmt, ... )
{
    int rc = 0;

    /*******************************************
     * If module not specified, assume default *
     *******************************************/
    if ((log_flags & LOG_MASK_MOD) == 0)
        log_flags |= LOG_MOD_DEFAULT;
  
    if ( (current_log_flags & log_flags & LOG_MASK_MOD) &&
         (current_log_flags & log_flags & LOG_MASK_LEVEL) ) {

        va_list ap;
        char *logPtr;
        char level[LOG_STRLEN];
        char module[LOG_STRLEN];
        char logBuffer[LOG_MAX_MSG_LEN];
          
        /*********************
         * Set module prefix *
         *********************/
        switch (log_flags & LOG_MASK_MOD) {
        case LOG_MOD_TEST:
            strcpy(module, LOG_MODSTR_TEST);
            break;
        case LOG_MOD_INIT:
            strcpy(module, LOG_MODSTR_INIT);
            break;
        case LOG_MOD_FRAMER:
            strcpy(module, LOG_MODSTR_FRAMER);
            break;
        case LOG_MOD_FEI:
            strcpy(module, LOG_MODSTR_FEI);
            break;
        case LOG_MOD_FIBERLAN:
            strcpy(module, LOG_MODSTR_FIBERLAN);
            break;
        case LOG_MOD_FPGA:
            strcpy(module, LOG_MODSTR_FPGA);
            break;
        case LOG_MOD_CLOCK:
            strcpy(module, LOG_MODSTR_CLOCK);
            break;
        case LOG_MOD_FLASH:
            strcpy(module, LOG_MODSTR_FLASH);
            break;
        case LOG_MOD_DEFAULT:
            strcpy(module, LOG_MODSTR_DEFAULT);
            break;
        default:
            strcpy(module, "????");
            break;
        }
          
        /************************
         * Set log level prefix *
         ************************/
        switch (log_flags & LOG_MASK_LEVEL) {
        case LOG_LEV_CRITICAL:
            strcpy(level, "CRIT");
            break;
        case LOG_LEV_WARNING:
            strcpy(level, "WARN");
            break;
        case LOG_LEV_INFO:
            strcpy(level, "INFO");
            break;
        case LOG_LEV_DEBUG:
            strcpy(level, "DEBG");
            break;
        case LOG_LEV_DETAIL:
            strcpy(level, "DETL");
            break;
        default:
            strcpy(level, "????");
            break;
        }


        /****************
         * Print prefix *
         ****************/
        if (!(log_flags & LOG_NO_BANNER)) {
            if (show_time) {
                unsigned int days,hrs,mins,secs;
                unsigned int millisecs;
                unsigned int jif;
                jif = jiffies;
                millisecs = (jif - (jif/HZ)*HZ)*10;
                secs = jiffies/HZ;
                mins = secs/60;
                hrs = mins/60;
                days = hrs/24;
                secs = secs%60;
                mins = mins%60;
                hrs = hrs%24;
                sprintf(logBuffer , 
                        "%s[%d:%02d:%02d:%02d:%03d] %s %s -- ", KERN_DEBUG,
                        days, hrs, mins, secs, millisecs,
                        module, level);

            }
            else {
                sprintf(logBuffer , "%s%s: %s -- ", KERN_DEBUG,  
                        module, level);
            }
        }
        else 
            strcpy(logBuffer, KERN_DEBUG);

        logPtr = logBuffer + strlen (logBuffer);

        /*******************************************
         * Print the given message into the buffer *
         *******************************************/
        va_start(ap, fmt);
        rc = vsprintf(logPtr, fmt, ap);
        va_end(ap);

        /************************************
         * Print the log message to console *
         ************************************/
        printk(logBuffer);
          

    }
    return rc;
}

/*********************************************************************
 * Function for enabling/disabling logging. Used by OMS menu to curb *
 * messages to the console.                                          *
 *********************************************************************/
void enableLogging(void) {
    current_log_flags |= LOG_ENABLE;
}

void disableLogging(void) {
    current_log_flags &= ~LOG_ENABLE;
}

unsigned long getDefaultLogFlags() {
    return \
        LOG_ENABLE | LOG_MOD_ALL | \
        LOG_LEV_CRITICAL | LOG_LEV_WARNING | LOG_LEV_INFO;
}

unsigned long getCurrentLogFlags() {
    return current_log_flags;
}

void setCurrentLogFlags(unsigned long flag) {
    current_log_flags = (current_log_flags & LOG_ENABLE) | flag;
}

/* Returns the current log level as a string */
char *getCurrentLogLevel() {
    static char log_str[LOG_STRLEN];
        
    if (current_log_flags & LOG_LEV_DETAIL)
        strcpy(log_str, LOG_STR_DETAIL);
    else if (current_log_flags & LOG_LEV_DEBUG)
        strcpy(log_str, LOG_STR_DEBUG);
    else if (current_log_flags & LOG_LEV_INFO)
        strcpy(log_str, LOG_STR_INFO);
    else if (current_log_flags & LOG_LEV_WARNING)
        strcpy(log_str, LOG_STR_WARNING);
    else if (current_log_flags & LOG_LEV_CRITICAL)
        strcpy(log_str, LOG_STR_CRITICAL);
    else
        strcpy(log_str, LOG_STR_NONE);
          
    return log_str;

}

/* Sets the current log level. Set bits for specified log level and all
   higher levels */
int setCurrentLogLevel(char * logstr) {

    if (strcmp(logstr, LOG_STR_NONE) == 0) {
        current_log_flags  = (current_log_flags & ~LOG_MASK_LEVEL);
    }
    else if (strcmp(logstr, LOG_STR_CRITICAL) == 0) {
        current_log_flags  = (current_log_flags & ~LOG_MASK_LEVEL) | \
            LOG_LEV_CRITICAL;
    }
    else if (strcmp(logstr, LOG_STR_WARNING) == 0) {
        current_log_flags  = (current_log_flags & ~LOG_MASK_LEVEL) | \
            LOG_LEV_CRITICAL | LOG_LEV_WARNING;
    }
    else if (strcmp(logstr, LOG_STR_INFO) == 0) {
        current_log_flags  = (current_log_flags & ~LOG_MASK_LEVEL) | \
            LOG_LEV_CRITICAL | LOG_LEV_WARNING | LOG_LEV_INFO;
    }
    else if (strcmp(logstr, LOG_STR_DEBUG) == 0) {
        current_log_flags  = (current_log_flags & ~LOG_MASK_LEVEL) | \
            LOG_LEV_CRITICAL | LOG_LEV_WARNING | LOG_LEV_INFO | \
            LOG_LEV_DEBUG;
    }
    else if (strcmp(logstr, LOG_STR_DETAIL) == 0) {
        current_log_flags  = (current_log_flags & ~LOG_MASK_LEVEL) | \
            LOG_LEV_CRITICAL | LOG_LEV_WARNING | LOG_LEV_INFO | \
            LOG_LEV_DEBUG | LOG_LEV_DETAIL;
    }
    else 
        return -1;

    return 0;
}

int set_current_log_level(int level) {

    if (level < 0 || level > 5)
        level = DEFAULT_LOG_LEVEL;

    return setCurrentLogLevel(log_array[level]);

}


/* Enables output for the given module */
int setLogModule(char *modstr) {

    if (strcmp(modstr, LOG_MODSTR_TEST) == 0) {
        current_log_flags |= LOG_MOD_TEST;
    }
    else if (strcmp(modstr, LOG_MODSTR_INIT) == 0) {
        current_log_flags |= LOG_MOD_INIT;
    }
    else if (strcmp(modstr, LOG_MODSTR_FRAMER) == 0) {
        current_log_flags |= LOG_MOD_FRAMER;
    }
    else if (strcmp(modstr, LOG_MODSTR_FEI) == 0) {
        current_log_flags |= LOG_MOD_FEI;
    }
    else if (strcmp(modstr, LOG_MODSTR_FIBERLAN) == 0) {
        current_log_flags |= LOG_MOD_FIBERLAN;
    }
    else if (strcmp(modstr, LOG_MODSTR_FPGA) == 0) {
        current_log_flags |= LOG_MOD_FPGA;
    }
    else if (strcmp(modstr, LOG_MODSTR_CLOCK) == 0) {
        current_log_flags |= LOG_MOD_CLOCK;
    }
    else if (strcmp(modstr, LOG_MODSTR_FLASH) == 0) {
        current_log_flags |= LOG_MOD_FLASH;
    }
    else if (strcmp(modstr, "ALL") == 0) {
        current_log_flags |= LOG_MOD_ALL;
    }
    else
        return -1;

    return 0;
}

/* Disables output for the given module */
int unsetLogModule(char *modstr) {

    if (strcmp(modstr, LOG_MODSTR_TEST) == 0) {
        current_log_flags &= ~LOG_MOD_TEST;
    }
    else if (strcmp(modstr, LOG_MODSTR_INIT) == 0) {
        current_log_flags &= ~LOG_MOD_INIT;
    }
    else if (strcmp(modstr, LOG_MODSTR_FRAMER) == 0) {
        current_log_flags &= ~LOG_MOD_FRAMER;
    }
    else if (strcmp(modstr, LOG_MODSTR_FEI) == 0) {
        current_log_flags &= ~LOG_MOD_FEI;
    }
    else if (strcmp(modstr, LOG_MODSTR_FIBERLAN) == 0) {
        current_log_flags &= ~LOG_MOD_FIBERLAN;
    }
    else if (strcmp(modstr, LOG_MODSTR_FPGA) == 0) {
        current_log_flags &= ~LOG_MOD_FPGA;
    }
    else if (strcmp(modstr, LOG_MODSTR_CLOCK) == 0) {
        current_log_flags &= ~LOG_MOD_CLOCK;
    }
    else if (strcmp(modstr, LOG_MODSTR_FLASH) == 0) {
        current_log_flags &= ~LOG_MOD_FLASH;
    }
    else if (strcmp(modstr, "ALL") == 0) {
        current_log_flags &= ~LOG_MOD_ALL;
    }
    else
        return -1;

    return 0;
}

