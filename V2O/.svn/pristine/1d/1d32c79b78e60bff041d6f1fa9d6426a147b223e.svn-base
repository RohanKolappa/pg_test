#ifndef __LOG_CONTROL_H_
#define __LOG_CONTROL_H_


extern unsigned long current_log_flags;

#define LOGPRINT(log_flags,  format, args...) \
        log_print(log_flags, format, ## args)

#define LOG_MOD_DEFAULT         ((unsigned long) 0x00000100)
#define LOG_MOD_TEST            ((unsigned long) 0x00000200)
#define LOG_MOD_INIT            ((unsigned long) 0x00000400)
#define LOG_MOD_FRAMER          ((unsigned long) 0x00000800)
#define LOG_MOD_FEI             ((unsigned long) 0x00001000)
#define LOG_MOD_FIBERLAN        ((unsigned long) 0x00002000)
#define LOG_MOD_FPGA            ((unsigned long) 0x00004000)
#define LOG_MOD_CLOCK           ((unsigned long) 0x00008000)
#define LOG_MOD_FLASH           ((unsigned long) 0x00010000)
#define LOG_MOD_PLX             ((unsigned long) 0x00020000)

#define LOG_MODSTR_TEST         "TEST"
#define LOG_MODSTR_INIT         "INIT"
#define LOG_MODSTR_FRAMER       "FRAM"
#define LOG_MODSTR_FEI          "ETHR"
#define LOG_MODSTR_FIBERLAN     "FIBR"
#define LOG_MODSTR_FPGA         "FPGA"
#define LOG_MODSTR_CLOCK        "CLCK"
#define LOG_MODSTR_FLASH        "FLSH"
#define LOG_MODSTR_DEFAULT      "DFLT"
                
#define LOG_MOD_ALL             (LOG_MOD_DEFAULT | \
                                LOG_MOD_TEST | \
                                LOG_MOD_INIT | \
                                LOG_MOD_FRAMER | \
                                LOG_MOD_FEI | \
                                LOG_MOD_FIBERLAN | \
                                LOG_MOD_FPGA | \
                                LOG_MOD_CLOCK | \
                                LOG_MOD_FLASH)

#define LOG_ENABLE              ((unsigned long) 0x80000000)
#define LOG_NO_BANNER           ((unsigned long) 0x40000000)

#define LOG_MASK_MOD            ((unsigned long) 0x3FFFFF00)
#define LOG_MASK_LEVEL          ((unsigned long) 0x000000FF)

#define LOG_LEV_CRITICAL        ((unsigned long) 0x01)
#define LOG_LEV_WARNING         ((unsigned long) 0x02)
#define LOG_LEV_INFO            ((unsigned long) 0x04)
#define LOG_LEV_DEBUG           ((unsigned long) 0x08)
#define LOG_LEV_DETAIL          ((unsigned long) 0x10)

#define LOG_STR_NONE            "None"
#define LOG_STR_CRITICAL        "Critical"
#define LOG_STR_WARNING         "Warning"
#define LOG_STR_INFO            "Info"
#define LOG_STR_DEBUG           "Debug"
#define LOG_STR_DETAIL          "Detail"

#define LOG_STRLEN              16

#define LOG_MAX_MSG_LEN         256
#define LOG_MAX_NUM_MSGS        64

#define DEFAULT_LOG_LEVEL       3

#ifdef __KERNEL__

/* Function Prototypes */
extern void enableLogging(void);
extern void disableLogging(void);

extern char *getCurrentLogLevel(void);
extern int setCurrentLogLevel(char * logstr);
extern unsigned long getDefaultLogFlags(void);
extern unsigned long getCurrentLogFlags(void);
extern void setCurrentLogFlags(unsigned long flag);
extern int setLogModule(char *modstr);
extern int unsetLogModule(char *modstr);

extern int log_print(unsigned long log_flags, 
                     const char *fmt, ... ) __attribute__ ((format (printf, 2, 3)));
extern int set_current_log_level(int level);

#endif /* __KERNEL__ */

#endif  /* __LOG_CONTROL_H_ */
