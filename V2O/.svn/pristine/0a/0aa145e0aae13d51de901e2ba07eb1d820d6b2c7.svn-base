#ifndef __MENUUTILS_H__
#define __MENUUTILS_H__

#define CREATE_CONFIG_HANDLER(function_prefix, file_name, field_name, field_length, default_value) \
 \
static int get_##function_prefix ## _handler(char *name, char *value, void *ptr) { \
    char *buf = ptr; \
    if (strcasecmp(name, field_name) == 0) { \
        if (buf != NULL) \
            strncpy(buf, value, field_length); \
        return 1; \
    } \
    return 0; \
} \
 \
static char *get_##function_prefix(char *buf, void *usrptr, int ident) {  \
    char buffer[field_length + 1];  \
    strcpy(buf, default_value); \
    if (parse_config_file(file_name,  \
                          get_##function_prefix ## _handler, buffer) < 0) {  \
        VLOG_WARNING("Could not load configuration file %s", file_name); \
        return buf;  \
    } \
    strcpy(buf, buffer);  \
    return buf; \
}  \
 \
static int write_##function_prefix ## _handler(char *name, char *value, void *ptr, char flag) {  \
    char *buf = ptr;  \
    if (flag) {  \
        if (buf != NULL && strlen(buf) <= field_length) {  \
            if (strcasecmp(name, field_name) == 0) {  \
                sprintf(value, "%s", buf);  \
                return 1;  \
            }  \
        }  \
    }  \
    else {  \
        strcpy(name, field_name);  \
        sprintf(value, "%s", buf);  \
    }  \
    return 0;  \
}  \
 \
static int set_##function_prefix(char *buf) {  \
    if (rewrite_config_file(file_name,  \
                            write_##function_prefix ## _handler, buf) < 0) {  \
        VLOG_WARNING("Could not rewrite configuration file %s", file_name); \
        return RC_ERROR;  \
    }  \
    return RC_NORMAL;  \
}  

extern char *yesnolist[];
extern int invoked_via_cli;
/***********************
 * Function prototypes *
 ***********************/

extern void print_ip_addr(char *buf, char *hostname);
extern int scanval(char *buf, int *val);
extern void bitstring(char *buf, const char *bitpattern, int bitval);
extern int get_persitent_int(char *name);
extern void set_persitent_int(char *name, int value);

#endif /* __MENUUTILS_H__ */
