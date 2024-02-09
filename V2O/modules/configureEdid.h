#ifndef __EDID_CONFIGURE_MENU_H_
#define __EDID_CONFIGURE_MENU_H_
#include <stdio.h>
#include "mylist.h"

#ifdef CUSTOM_MENU_DEFS

#include "custmenu.h"
        /* Prototype for configureEdidDescriptorList command */
extern int configureEdidVideoInput(MENUINFO * minfo);
extern int configureEdidFromMonitor(MENUINFO * minfo);
#endif
extern int describe_edid_videoinput_main(int argc, char **argv);
extern int get_edid_videoinput_main(int argc, char **argv);
extern int set_edid_videoinput_main(int argc, char **argv);

#endif      /* __EDID_CONFIGURE_MENU_H_ */
