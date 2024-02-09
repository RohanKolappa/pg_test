#ifndef __CLEAR_EDID_LIST_MENU_H_
#define __CLEAR_EDID_LIST_MENU_H_

#ifdef CUSTOM_MENU_DEFS
#include "custmenu.h"
        /* Prototype for configureEdidDescriptorList command */
extern int clearEdidDescriptorList(MENUINFO * minfo);
#endif

/* Prototypes for CLI access entry point functions */
extern int clear_edid_list_main(int argc, char **argv);

#endif                          /* __CLEAR_EDID_LIST_MENU_H_ */
