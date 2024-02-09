#ifndef __CONFIGUREUSER_H__
#define __CONFIGUREUSER_H__

#ifdef  CUSTOM_MENU_DEFS
#include "mylist.h"

#define USER_NAME_LEN                   16
#define DESCRIPTION_LEN                 32

struct user_entry {
    char                login[USER_NAME_LEN + 1];
    char                description[DESCRIPTION_LEN + 1];
    struct list_head    user_list;
};

extern int configureUser(MENUINFO *minfo);
#endif

#define USER_ERR_NO_PERMISSION          -2
#define USER_ERR_INTERNAL_ERROR         -3
#define USER_ERR_UNKNOWN_USER           -4
#define USER_ERR_PASSWORD_LOCKED        -5
#define USER_ERR_SETUID_ERROR           -6
#define USER_ERR_INVALID_PASSWORD       -7
#define USER_ERR_INCORRECT_PASSWORD     -8
#define USER_ERR_INVALID_UID            -9      
#define USER_ERR_LOGIN_EXISTS           -10
#define USER_ERR_NO_GROUP               -11

#define PASSWD_FILE                     "/etc/passwd"
#define SHADOW_FILE                     "/etc/shadow"

#endif /* __CONFIGUREUSER_H__ */
