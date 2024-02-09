/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        osd.h
*
*    DESCRIPTION:
*        1) Parse the OSD font library.
*		 2) Define and Implement OSD APIs
*
*    AUTHOR:Xiuli Guo
*	
*
*
******************************************************************************/


#ifndef __OSD_H__
#define __OSD_H__

#include "Struct.h"
typedef unsigned long HOSD;
#define OSD_STRING_LEN_MAX 96

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	unsigned char x;
	unsigned char y;
    char osd_string[OSD_STRING_LEN_MAX];
}osd_string_t;

/*	
typedef struct{

	OSD_STRING_t OSD_String[31];	

} OSD_FRAME_t;*/

typedef enum	
{ 
	
	OSD_NORMAL = 0,
	OSD_REFRESH,
}osd_display_mode_t;

	
typedef enum
	
{
	OSD_ERROR_OK = 0,
	OSD_ERROR_ADDR_OUTOF_RANGE,
	OSD_ERROR_BUS_WRITE_FAIL,
	OSD_ERROR_INVALID_ADDRESS,
	OSD_ERROR_INVALID_FONT,
	OSD_ERROR_OUTOFMEMORY,
}osd_error_t;
	

osd_error_t
write_osd(unsigned short int x, unsigned short int y, unsigned char *string_addr);
//write_osd(unsigned char x, char y, unsigned char *string_addr);


osd_error_t 
init_osd(void * pdxc,long video_frame_width, long video_frame_height);

osd_error_t 
get_osd_frame(unsigned char string_num, void *string_addr);

osd_error_t 
show_osd_frame(osd_display_mode_t  mode);

osd_error_t 
clean_osd();



#ifdef __cplusplus
}
#endif

#endif

/********************************* end of osd.h **************************/
