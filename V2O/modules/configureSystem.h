#ifndef __CONFIGURE_SYSTEM_H__
#define __CONFIGURE_SYSTEM_H__

#ifdef CUSTOM_MENU_DEFS
extern int configureEEPROM(MENUINFO *minfo);
extern int configureImage(MENUINFO *minfo);
extern int configureHorizontalPosition(MENUINFO *minfo);
extern int configureVerticalPosition(MENUINFO *minfo);
extern int configurePhaseAdjust(MENUINFO *minfo);
extern int configurePolarity(MENUINFO *minfo);
extern int configureLRPolarity(MENUINFO *minfo);
extern int configureKB(MENUINFO *minfo);
extern int configureDefaults(MENUINFO *minfo);
extern int configureXMPPParams(MENUINFO *minfo);
extern int configureXMPPServerParams(MENUINFO *minfo);
extern int upgradeSoftware(MENUINFO *minfo);
extern int rebootSystem(MENUINFO *minfo);
extern int shutdownSystem(MENUINFO *minfo);
extern int haTakeover(MENUINFO *minfo);
extern int haStandby(MENUINFO *minfo);
extern int setTime(MENUINFO *minfo);
#endif

extern char *get_kbms_enable(char *buf, void *usrptr, int ident);
extern char *get_kbms_ipaddress(char *buf, void *usrptr, int ident);
extern char *get_kbms_port(char *buf, void *usrptr, int ident);
extern char *get_kbms_password(char *buf, void *usrptr, int ident);
extern int set_kbms_enable(char *buf);
extern int set_kbms_ipaddress(char *buf);
extern int set_kbms_port(char *buf);
extern int set_kbms_password(char *buf);
int write_kbmsenable_handler(char *name, char *value, void *ptr, char flag); 
int write_kbmsip_handler(char *name, char *value, void *ptr, char flag);
int write_kbmsport_handler(char *name, char *value, void *ptr, char flag); 
int write_kbmspassword_handler(char *name, char *value, void *ptr, char flag); 
int get_kbmsenable_handler(char *name, char *value, void *ptr);
int get_kbmsip_handler(char *name, char *value, void *ptr);
int get_kbmsport_handler(char *name, char *value, void *ptr);
int get_kbmspassword_handler(char *name, char *value, void *ptr);

int apply_xmpp(void);
int apply_xmppserver(void);

int dump_xmpp_agent(void);
int dump_xmpp_server(void);

#endif /* __CONFIGURE_SYSTEM_H__ */
