#ifndef __VALIDATIONROUTINE_H__
#define __VALIDATIONROUTINE_H__

#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif
extern int check_enum(const char *string, char *array[]);
extern int check_regex(const char *string, char *pattern);
extern int check_netmask(const char *str);
extern int check_integerorany(const char *str);
extern int check_ip(const char *str);
extern int check_integeroranyrange(const char *str, int min, int max);
extern int check_integerlist_range(const char *str, int min, int max);
extern int check_ipaddorblank(const char *str);
extern int check_hostnameorblank(const char *str);
extern int check_domainname(const char *str);
extern int check_multicastip(const char *str);
extern int check_inetname(const char *str);
extern int check_hexadecimal(const char *string);
extern int get_hexadecimal(const char *string);
extern int check_onlyhexadecimal(const char *string);
extern int get_onlyhexadecimal(const char *string);

#ifdef __cplusplus
}
#endif

#endif // __VALIDATIONROUTINE_H__
