#ifndef __CONFIGFILE_PARSER_H__
#define __CONFIGFILE_PARSER_H__

#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif
extern int parse_config_file(const char *filename, int(*nv_handler)(char *,
        char *, void *ptr), void *ptr);
extern int parse_configuration_file(const char *filename, int(*nv_handler)(
        char *, char *, void *ptr), void *ptr);
extern int save_config_file(const char *filename, int(*nv_handler)(char *,
        char *, void *ptr), void *ptr);
extern int rewrite_config_file(const char *filename, int(*nv_handler)(char *,
        char *, void *, char), void *ptr);
extern int rewrite_configuration_file(const char *filename, int(*nv_handler)(
        char *, char *, void *, char), void *ptr);
extern int parse_list_config_file(const char *filename, int(*nv_handler)(
        char *, char *, void *ptr, int index), void *ptr);
extern int acquire_config_lock(void);
extern int release_config_lock(void);
#ifdef __cplusplus
}
#endif

#endif 
