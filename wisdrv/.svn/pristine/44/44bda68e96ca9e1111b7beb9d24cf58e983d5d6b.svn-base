/******************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        board_module.c
*
*    DESCRIPTION:
*        
*   
*
*    AUTHOR:
*        
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#include <linux/module.h>   // Needed by all modules
#include <linux/kernel.h>   // Needed for KERN_ALERT
#include <linux/init.h>     // Needed for the macros
#include "debug.h"

static int board_init(void)
{
   PrintMsg(DEBUG_ALWAYS_DISPLAY, ("load board module"));
   return 0;
}


static void board_exit(void)
{
   PrintMsg(DEBUG_ALWAYS_DISPLAY, ("unload board module"));
}

MODULE_LICENSE("GPL");

module_init(board_init);
module_exit(board_exit);

/****************************** end of board_module.c ************************/
