#ifndef __CUSTMENU_H_
#define __CUSTMENU_H_

#ifdef CUSTOM_MENU_DEFS

#include <curses.h>
#include <panel.h>
#include <menu.h>
#include <form.h>
#include <cdk/cdk.h>

struct minf;

struct midef {
    char        *menuitem_name;         /* Name for menu item */
    char        *menuitem_help;         /* Help string for the menu */
    int         menuitem_type;          /* One of MTYPE_XXX */
    int         (*menuitem_func)(struct minf *);  /* Function to be invoked */
    struct midef        *menuitem_submenu;   /* Sub menu */
    char        *menuitem_title;        /* Title for submenu */
    unsigned long  menuitem_flags;              /* Logical or of MFLAG_XXX types */
};

typedef struct midef MENUITEMDEF;


typedef struct minf {
    char *minfo_title;
    MENUITEMDEF *minfo_itemdefs;
    CDKSCREEN *minfo_menuscreen;
    CDKSCREEN *minfo_infoscreen;
    CDKLABEL  *minfo_title_label;
    CDKLABEL  *minfo_info_label;
    void      *minfo_userptr;
} MENUINFO;

typedef struct {
    int         formitem_ident;          /* Identifier for the form field */
    const char  *formitem_name;          /* Label for the form item */
    const char  *formitem_help;          /* Help string for the item */
    int         formitem_len;            /* length of entry field */
    char *      (*formitem_initval)(char *, void *, int ident);  
                                         /* Routine to return 
                                            initial val of buffer*/
    int         (*formitem_val)(const char *); /* Validation routine */
    int         formitem_type;           /* Field validation types */
    int         formitem_min;            /* validation min range */
    int         formitem_max;            /* validation min range */
    int         formitem_options;        /* Extra options to turn off */
    int         formitem_flags;          /* Formatting flags */
} FORMITEMDEF;

#define _DIALOG_LINES_          19
typedef  struct {
    CDKLABEL    *label;
    char        *message[_DIALOG_LINES_];
}DIALOG;

#define MTYPE_EXIT              1
#define MTYPE_FUNCTION          2
#define MTYPE_SUBMENU           3

#define MFLAG_SYSTEM                    0x000001

#define MFLAG_PORTTYPE_TX               0x000002
#define MFLAG_PORTTYPE_RX               0x000004

#define MFLAG_PORTTYPE_BOTH    (MFLAG_PORTTYPE_TX | \
                                MFLAG_PORTTYPE_RX)

#define MFLAG_PORTTYPE_MASK    (MFLAG_PORTTYPE_TX | \
                                MFLAG_PORTTYPE_RX)

#define MFLAG_DEVICE_V2D_TX             0x000100
#define MFLAG_DEVICE_V2D_RX             0x000200
#define MFLAG_DEVICE_DMS                0x000400
#define MFLAG_DEVICE_MS                 0x000800
#define MFLAG_DEVICE_XP200              0x001000
#define MFLAG_DEVICE_XP100              0x002000
#define MFLAG_DEVICE_ORB                0x004000
#define MFLAG_DEVICE_XPI_TX		0x008000
#define MFLAG_DEVICE_XPI_RX		0x010000

#define MFLAG_DEVICES_ALL    (MFLAG_DEVICE_V2D_TX | \
                             MFLAG_DEVICE_V2D_RX | \
                             MFLAG_DEVICE_DMS | \
                             MFLAG_DEVICE_MS | \
                             MFLAG_DEVICE_XP200 | \
                             MFLAG_DEVICE_XP100 | \
                             MFLAG_DEVICE_ORB | \
			                 MFLAG_DEVICE_XPI_TX | \
                             MFLAG_DEVICE_XPI_RX)

#define MFLAG_DEVICES_XPI       (MFLAG_DEVICE_XPI_TX | MFLAG_DEVICE_XPI_RX)

#define MFLAG_DEVICES_NON_XPI	(MFLAG_DEVICE_V2D_TX | \
                                 MFLAG_DEVICE_V2D_RX | \
                                 MFLAG_DEVICE_DMS | \
                                 MFLAG_DEVICE_MS | \
                                 MFLAG_DEVICE_XP200 | \
                            	 MFLAG_DEVICE_XP100 | \
                             	 MFLAG_DEVICE_ORB) 

#define MFLAG_DEVICES_V2D    (MFLAG_DEVICE_V2D_TX | \
                             MFLAG_DEVICE_V2D_RX | \
                             MFLAG_DEVICE_XP200 | \
                             MFLAG_DEVICE_XP100 | \
                             MFLAG_DEVICE_XPI_TX | \
                             MFLAG_DEVICE_XPI_RX)

#define MFLAG_DEVICES_V2D_TX    (MFLAG_DEVICE_V2D_TX | \
                                MFLAG_DEVICE_XP200 | \
                                MFLAG_DEVICE_XP100 | \
                                MFLAG_DEVICE_XPI_TX)

#define MFLAG_DEVICES_V2D_RX    (MFLAG_DEVICE_V2D_RX | \
                                MFLAG_DEVICE_XP200 | \
                                MFLAG_DEVICE_XP100 | \
                                MFLAG_DEVICE_XPI_RX)

#define MFLAG_DEVICES_XP     (MFLAG_DEVICE_XP200 | MFLAG_DEVICE_XP100)

#define FFLAG_FOLLOW            0x01
#define FFLAG_HIDE              0x02
#define FFLAG_DISABLE           0x04
#define FFLAG_DISPLAYONLY       0x08

/* Special form processing flags */
#define FORM_DISPLAY_ONLY       0x01
#define FORM_AUTO_REFRESH       0x02
#define FORM_EDITABLE           0x04
#define FORM_NONROOT_EDITABLE   0x08

#define FTYPE_NONE                      0
#define FTYPE_ALNUM                     1
#define FTYPE_ALPHA                     2
#define FTYPE_INTEGER                   3
#define FTYPE_NUMERIC                   4
#define FTYPE_IPV4                      5
#define FTYPE_ENUM                      6
#define FTYPE_REGEXP                    7
#define FTYPE_INTEGER_RANGE             8
#define FTYPE_REGEXP_IMMEDIATE          9
#define FTYPE_ENUM_IMMEDIATE            10
#define FTYPE_INTEGER_IMMEDIATE         11
#define FTYPE_ENUM_RANGE                12
#define FTYPE_EVALUATE                  13
#define FTYPE_EVALUATE_IMMEDIATE        14
#define FTYPE_NUMERIC_IMMEDIATE         15
#define FTYPE_NUMERIC_LOW               16
#define FTYPE_NUMERIC_LOW_IMMEDIATE     17

#define TITLE_HEIGHT            4
#define SUBWIN_PAD              1

#define YESNO_STR               " [</U>Y<!U>/</U>N<!U>] "
#define PRESSANYKEY_STR         "Press any key to continue... "

#define scroll_print(minfo, format, args...) {                   \
        sprintf(strbuf, format, ## args);                       \
        waddstr((minfo)->minfo_menuscreen->window, strbuf);     \
        wrefresh((minfo)->minfo_menuscreen->window);            \
        VLOG_INFO(format, ## args);                            \
    }

typedef char *evalfun(char *buffer, int dirention);

extern int verbose_errors;
extern char *formmesg;
extern char *formmesg2;
extern char tempbuf[];
extern void show_info(MENUINFO *minfo, char *infostring);
extern void clear_info(MENUINFO *minfo, char *infostring);
extern int show_title(MENUINFO *minfo, char *titlestring);
extern int get_title(MENUINFO *minfo, char *titlebuffer);
extern DIALOG *show_dialog(MENUINFO *minfo, int starty, int startx, 
                           char *dialogstring);
extern int print_dialog(MENUINFO *minfo, DIALOG *dialog, char *dialogstring );
extern void remove_dialog(DIALOG *dialog);
extern FORM *create_form(FORMITEMDEF *fitems, int starty, int startx, 
                         void *usrptr);
extern void delete_form(FORM *form);
extern int map_numpad(int ch);
extern int process_form(MENUINFO *minfo, FORM *form, 
                        char *default_mesg, int form_flags);
extern void refresh_form(FORM *form);
extern void refresh_form_disable(FORM *form);
extern int set_form_windows(MENUINFO *minfo, FORM *form);
extern void unset_form_windows(FORM *form);
extern char *get_field_buffer(FORM *form, int identifier, char *buf);
extern int get_form_status(FORM *form);
extern int get_prevnextquit(MENUINFO * minfo);
extern int get_yesno(MENUINFO *minfo);
extern int get_anykey(MENUINFO *minfo);
extern MENU *create_selector(int numb_items, 
                             char * (*itemselector_cb)(int ident));
extern int set_menu_windows(MENUINFO *minfo, MENU *menu);
extern int process_selector(MENUINFO *minfo, MENU *menu, char *keylist);
extern int refresh_selector(MENU *menu, int numb_items, 
                        char * (*selector_cb)(int ident));
extern void clear_selector_init(MENU * menu);
extern void set_selector_init(MENU * menu);
extern void delete_selector(MENU *menu);
void set_selector_item(MENU *menu, int idx);
extern void unset_menu_windows(MENU *menu);
extern void set_formitem_inactive(FORMITEMDEF form[], int size, int identifier);
extern void set_formitem_flags(FORMITEMDEF form[], int size, 
                        int identifier, int flag);
extern void set_formitem_options(FORM *form,  
                        int identifier, bool flag);
extern void restore_curs_inopts(void);
extern int show_fault_dialog(MENUINFO *minfo, char *message);

#endif /* CUSTOM_MENU_DEFS */

// Request codes
#define DISPLAY_REQUEST         1
#define ADD_REQUEST             2
#define MOD_REQUEST             3
#define DEL_REQUEST             4

// Control characters
#define CTLX                    0x1a

/***************
 * Error codes *
 ***************/
#define RC_NORMAL               0
#define RC_ERROR                -1
#define RC_EXIT                 -2
#define RC_QUIT                 -3
#define RC_EXCEPT_FIELD         -4
#define RC_SHOULD_SAVE          -5
#define RC_INVALID_VALUE        -6
#define RC_BAD_INDEX            -7
#define RC_DUPLICATE            -8
#define RC_WARNING              -9
#define RC_SYSTEM_BUSY          -10
#define RC_LOCKED_OUT           -11

/****************************
 * Exposed global variables *
 ****************************/
extern unsigned short ruid;

/*********************
 * Exposed functions *
 *********************/
extern int mainmenu(void);

#endif /* __CUSTMENU_H_ */
