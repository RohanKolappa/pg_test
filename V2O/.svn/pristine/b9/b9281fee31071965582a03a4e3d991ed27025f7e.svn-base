#ifndef __REMOTE_CONFIG_H__
#define __REMOTE_CONFIG_H__

#define REMOTE_CONFIG_OPERATION_MODIFY   1
#define REMOTE_CONFIG_OPERATION_ADD      2
#define REMOTE_CONFIG_OPERATION_DELETE   3

#define REMOTE_CONFIG_SOURCE_CLI         1
#define REMOTE_CONFIG_SOURCE_MENU        2

#define REMOTE_CONFIG_STATUS_FAIL        0
#define REMOTE_CONFIG_STATUS_PASS        1

#ifdef __cplusplus
extern "C" {
#endif

    extern int remote_config_start(void);
    extern int remote_config_start_dummy(void);
    extern void remote_config_set_config_name(const char *name);
    extern void remote_config_set_command_name(const char *command);
    extern void remote_config_set_status(int status);
    extern void remote_config_set_operation(int operation);
    extern void remote_config_set_source(int source);
    extern void remote_config_set_index(int index);
    extern void remote_config_set_board_number(int index);
    extern void remote_config_set_channel_number(int index);
    extern void remote_config_set_client_data(const char *client_data);
    extern int remote_config_add_parameter(const char *parameter, 
                                           const char *value);
    extern int remote_config_add_bool_parameter(const char *parameter, 
                                                int value);
    extern int remote_config_add_int_parameter(const char *parameter, 
                                               int value);
    extern int remote_config_add_float_parameter(const char *parameter, 
                                                 float value);
    extern void remote_config_send(void);
    extern void remote_config_send_error(const char *reason);

    extern int remote_notification_start(char * a_event_source, char *a_event_type);
    extern void remote_notification_add_data(char *a_name, char *a_value);
    extern void remote_notification_send(void);
    extern int remote_external_buffer_send(char *buffer, int length);

#ifdef __cplusplus
}
#endif

#endif /* __REMOTE_CONFIG_H__ */

