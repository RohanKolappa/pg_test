#ifndef __REMOTE_STATUS_H__
#define __REMOTE_STATUS_H__


#define REMOTE_STATUS_FAIL              0
#define REMOTE_STATUS_PASS              1

#define REMOTE_STATUS_TYPE_BOOLEAN      1
#define REMOTE_STATUS_TYPE_INT          2
#define REMOTE_STATUS_TYPE_UINT         3
#define REMOTE_STATUS_TYPE_LONG         4
#define REMOTE_STATUS_TYPE_ULONG        5
#define REMOTE_STATUS_TYPE_STRING       6

#ifdef __cplusplus
extern "C" {
#endif

    extern int remote_status_start(int isList);
    extern void remote_status_set_name(const char *name);
    extern void remote_status_set_command_name(const char *command);
    extern void remote_status_set_status(int status);
    extern void remote_status_set_channel_number(int channelNumber);
    extern void remote_status_print_error(void);
    extern void remote_status_print(void);
    extern void remote_status_print_notify(void);
    extern void remote_status_new_parameters(void);
    extern int remote_status_translate_parameters(const char *xml_string);
    extern int remote_status_add_string_parameter(const char *parameter, 
                                                  const char *value,
                                                  const char *units,
                                                  const char *source);
    extern int remote_status_add_boolean_parameter(const char *parameter, 
                                                   int value,
                                                   const char *units,
                                                   const char *source);
    extern int remote_status_add_integer_parameter(const char *parameter, 
                                                   int value,
                                                   const char *units,
                                                   const char *source);
    extern int remote_status_add_unsigned_integer_parameter(const char *parameter, 
                                                            unsigned long value,
                                                            const char *units,
                                                            const char *source);
    extern int remote_status_add_long_parameter(const char *parameter, 
                                                long long value,
                                                const char *units,
                                                const char *source);
    extern int remote_status_add_unsigned_long_parameter(const char *parameter, 
                                                         unsigned long long value,
                                                         const char *units,
                                                         const char *source);
    
    extern int remote_status_add_float_parameter(const char *parameter, 
                                                 double value,
                                                 const char *units,
                                                 const char *source,
                                                 int precision);

#ifdef __cplusplus
}
#endif

#endif /* __REMOTE_STATUS_H__ */

