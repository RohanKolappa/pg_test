/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        osd.c
*
*    DESCRIPTION:
*        1) Parse the OSD font library.
*		 2) Implement OSD APIs
*
*    AUTHOR:Xiuli Guo
*	
*
*
******************************************************************************/

#include "platform.h"
#include "function.h"
#include "common.h"


#define MB_SIZE 16

#define OSD_STRING_BUFFER1_START 0x3A00
#define OSD_STRING_BUFFER1_END 0x3A5F

#define OSD_STRING_BUFFER2_START 0x3A60
#define OSD_STRING_BUFFER2_END 0x3ACF
#define OSD_END_OF_FRAME 0xAAAA
#define OSD_END_OF_STRING 0
#define OSD_SUPPORT_INDEX_MAX 256

static int cbus_safe_write (int val, UINT16 Addr, UINT16 Data);
static osd_error_t get_osd_data (unsigned short int x, unsigned short int y,
				 unsigned char *string_addr);

#define OSD_BUFFER_LOOP_REGISTER 0x3FE8



UINT16 osd_write_retry;

typedef struct
{
PDEVICE_EXTENSION_COMMON context;

UINT16 string_st_addr;
UINT16 index_addr;
UINT16 current_buff_addr;
UINT16 frame_index;
UINT16 frame_buff[96];
UINT16 last_frame_buff[96];
UINT16 frame_width;
UINT16 frame_height;
UINT16 end_of_string_x;
UINT16 end_of_string_y;
UINT16 write_retry;
struct semaphore mutex;
}osd_status_t;

osd_status_t *osd;


/*************************************************************************
*
*
* Init OSD to get the ubs handling for write the OSD command by USB
*
*
***************************************************************************/
osd_error_t
init_osd (void *pdxc, long video_frame_width, long video_frame_height)
{
    UINT16 temp;
    int Retry = 5;
	
	//check if the system has enough memmory
	osd = (osd_status_t*)PIAllocMem(sizeof(osd_status_t));
	
	if( osd == 0)
		return OSD_ERROR_OUTOFMEMORY;

    //Get the USB handling
    osd->context= (PDEVICE_EXTENSION_COMMON) pdxc;

    //Get initial frame buffer start address
    while ((ReadCBusRegFW (osd->context, OSD_BUFFER_LOOP_REGISTER, &temp) != 0)
	   && (Retry > 0))
    {
	Retry--;
    }

    if (Retry > 0)
    {
	osd->current_buff_addr = temp;
    }
    else
	osd->current_buff_addr = OSD_STRING_BUFFER1_START;

    // from current video setting get the OSD width and height 
    osd->frame_width = video_frame_width / MB_SIZE;
    osd->frame_height = video_frame_height / MB_SIZE;



    //initialization 
    osd->frame_index = 0;
    osd->end_of_string_x = 0;
    osd->end_of_string_y = 0;
    osd_write_retry = 2;
    PrintMsg (DEBUG_HARDWARE_MISC,
	      ("***************************** osd_mutex,%d Y=%d,X%d,Z%d",
	       osd->mutex, osd, osd->frame_width, osd->frame_height));
    init_MUTEX (&osd->mutex);
    PrintMsg (DEBUG_HARDWARE_MISC,
	      ("$$$$$$$$$$$$$$$$$Current osd->mutex; %d\n", osd->mutex));
    return OSD_ERROR_OK;

}

/*************************************************************************
*
*
* Clean the OSD display
*
*
***************************************************************************/
osd_error_t
clean_osd ()
{

    //write the EOF to the start address of current frame buffer
    if (cbus_safe_write (osd_write_retry, osd->current_buff_addr, OSD_END_OF_FRAME)
	!= 0)
	return OSD_ERROR_BUS_WRITE_FAIL;

    osd->end_of_string_x = 0;
    osd->end_of_string_y = 0;

    return OSD_ERROR_OK;

}

/*************************************************************************
*
*
* * Write OSD data to buffer, if the usb write failed try it again
* val:  USD send data retry number
* Addr: OSD buffer address
* Data: data to be written to OSD buffer 
*
*
***************************************************************************/
osd_error_t
write_osd (unsigned short int x, unsigned short int y,
	   unsigned char *string_ptr)
{
    UINT16 temp[OSD_STRING_LEN_MAX], i = 1, temp_i, temp_x, temp_y;

    if (strlen (string_ptr) > OSD_STRING_LEN_MAX)
	return OSD_ERROR_ADDR_OUTOF_RANGE;


    //PrintMsg (DEBUG_HARDWARE_MISC,("*****************************  x:%d, y=%d\n", x, y));


    //if the display fond start address(x,y) is out of range,return error
    if (x > osd->frame_width || y > osd->frame_height)
    {
	//PrintMsg (DEBUG_HARDWARE_MISC, ("$$$$$$$$$$$$$$$$$$$$$$$$$$x,y out of range   "));
	return OSD_ERROR_ADDR_OUTOF_RANGE;
    }

    //if the current new start address is less than the end of the previous address, return error
    if ((y < osd->end_of_string_y)
	|| ((y == osd->end_of_string_y) && (x < osd->end_of_string_x)))
    {
	PrintMsg (DEBUG_HARDWARE_MISC,
		  ("####################x, y over write the prious string  x:%d, y=%d, ", x, y));
	return OSD_ERROR_INVALID_ADDRESS;
    }

    temp_x = ((x + strlen (string_ptr)) % osd->frame_width);
    temp_y = (x + strlen (string_ptr)) / osd->frame_width + y;

    //if the display fond start address(x,y) is out of range,return error
    if (temp_y > osd->frame_height)
    {
	PrintMsg (DEBUG_HARDWARE_MISC,
		  ("$$$$$$$$$$$$$$$$$Current string will over the range  
            x:%d, y=%d, X=%d, Y=%d", temp_x, temp_y, osd->end_of_string_x, osd->end_of_string_y));
	return OSD_ERROR_ADDR_OUTOF_RANGE;
    }


    osd->end_of_string_x = temp_x;
    osd->end_of_string_y = temp_y;

    PrintMsg (DEBUG_HARDWARE_MISC,
	      ("*****************************  x:%d, y=%d, X=%d, Y=%d", temp_x,
	       temp_y, osd->end_of_string_x, osd->end_of_string_y));

    if ((strlen (string_ptr) + 2) > OSD_STRING_LEN_MAX)
	return OSD_ERROR_ADDR_OUTOF_RANGE;

    temp[0] = (UINT16) (x + (y << 8));

    //put the charactor index to a temp arry
    ////if the string length is larger than OSD_STRING_LEN_MAX, return error
    while (*string_ptr != 0)
    {
	temp[i++] = (UINT16) * string_ptr++;
	if (*string_ptr > OSD_SUPPORT_INDEX_MAX)
	    return OSD_ERROR_INVALID_FONT;
    }

    //put OSD_DOC 
    temp[i++] = 0;
    temp_i = i;

    //switch to the next OSD frame
    if (osd->current_buff_addr == OSD_STRING_BUFFER1_START)
    {
	osd->string_st_addr = OSD_STRING_BUFFER2_START;
    }
    else
    {
	osd->string_st_addr = OSD_STRING_BUFFER1_START;
    }

    i = 0;
    osd->index_addr = osd->string_st_addr;

	//semaphore take
	down (&osd->mutex);


    //write the frame structure to chip by USB 
    while (i <= temp_i)
    {
	if (cbus_safe_write (osd_write_retry, osd->index_addr, temp[i]) != 0)
	{    
		//semaphore give
		up (&osd->mutex);

	    return OSD_ERROR_BUS_WRITE_FAIL;
	}
	osd->index_addr++;
	temp[i++];
    }

    //Write EOF to the end of frame
    if (cbus_safe_write (osd_write_retry, osd->index_addr, 
		OSD_END_OF_FRAME) !=0)
	{    
		//semaphore give
		up (&osd->mutex);

	    return OSD_ERROR_BUS_WRITE_FAIL;
	}

    //Change display addr.

    if (cbus_safe_write
	(osd_write_retry, OSD_BUFFER_LOOP_REGISTER, osd->string_st_addr) != 0)
	{    
		//semaphore give
		up (&osd->mutex);

	    return OSD_ERROR_BUS_WRITE_FAIL;
	}

	//semaphore give
    up (&osd->mutex);


    //Write down current buffer address 
    osd->current_buff_addr = osd->string_st_addr;

    osd->end_of_string_x = 0;
    osd->end_of_string_y = 0;

    return OSD_ERROR_OK;
}


/*************************************************************************
*
*
* Write OSD data to buffer, if the usb write failed try it again
* val:  USD send data retry number
* Addr: OSD buffer address
* Data: data to be written to OSD buffer
*
*
***************************************************************************/
static int
cbus_safe_write (int val, UINT16 addr, UINT16 data)
{
    int i, result;
    //result = (pOSD->hw_funcs.WriteInterrupt)(pOSD,(Addr+0x8000),Data); 

    //write the OSD data by the USB buffer
    result = WriteCBusRegFW (osd->context, addr, data);
    if (result != 0)
    {
	for (i = 0; i < val; i++)
	{
	    if (WriteCBusRegFW (osd->context, addr, data) == 0)
		return 0;
	}
	return 1;
    }
    return 0;
}


/*************************************************************************
*
*
* Clean the OSD display
* (x,y) start display location of the string
*  *pString: start address of the string stored at
*
***************************************************************************/
static osd_error_t
get_osd_data (unsigned short int x, unsigned short int y,
	      unsigned char *string_ptr)
{
    UINT16 temp_x, temp_y;


    PrintMsg (DEBUG_HARDWARE_MISC,
	      ("*****************************  x:%d, y=%d\n", x, y));


    //if the display fond start address(x,y) is out of range,return error
    if (x > osd->frame_width || y > osd->frame_height)
    {
	PrintMsg (DEBUG_HARDWARE_MISC, ("$$$$$$$$$$$$$$$$$$$$$$$$$$"));
	return OSD_ERROR_ADDR_OUTOF_RANGE;
    }

    //if the current new start address is less than the end of the previous address, return error
    if ((y < osd->end_of_string_y)
	|| ((y == osd->end_of_string_y) && (x < osd->end_of_string_x)))
    {
	PrintMsg (DEBUG_HARDWARE_MISC,
		  ("####################x, y over write the prious string  "));
	return OSD_ERROR_INVALID_ADDRESS;
    }

    temp_x = ((x + strlen (string_ptr)) % osd->frame_width);
    temp_y = (x + strlen (string_ptr)) / osd->frame_width + y;

    //if the display fond start address(x,y) is out of range,return error
    if (temp_y > osd->frame_height)
    {
	PrintMsg (DEBUG_HARDWARE_MISC,
		  ("$$$$$$$$$$$$$$$$$Current string will over the range  
        x:%d, y=%d, X=%d, Y=%d", temp_x, temp_y, osd->end_of_string_x, osd->end_of_string_y));
	return OSD_ERROR_ADDR_OUTOF_RANGE;
    }


    osd->end_of_string_x = temp_x;
    osd->end_of_string_y = temp_y;

    PrintMsg (DEBUG_HARDWARE_MISC,
	      ("$$$$$$$$$$$$$$$$$Current string   x:%d, y=%d, X=%d, Y=%d", x, y,
	       osd->end_of_string_x, osd->end_of_string_y));

    if ((osd->frame_index + strlen (string_ptr) + 2) > OSD_STRING_LEN_MAX)
	return OSD_ERROR_ADDR_OUTOF_RANGE;


    osd->frame_buff[osd->frame_index++] = (UINT16) (x + (y << 8));

    //put the charactor index to a temp arry
    //if the string length is larger than OSD_STRING_LEN_MAX, return error
    while (*string_ptr != 0)
    {
	osd->frame_buff[osd->frame_index] = (UINT16) * string_ptr++;
	if (*string_ptr > OSD_SUPPORT_INDEX_MAX)
	    return OSD_ERROR_INVALID_FONT;
	osd->frame_index++;
    }

    osd->frame_index = osd->frame_index;

    //put OSD_EOC 
    osd->frame_buff[osd->frame_index++] = 0;
    return OSD_ERROR_OK;
}


/*************************************************************************
*
*
* Write the prepared OSD data to OSD buffer 
* mode : to update current buffer of write to a new buffer
*  
*
***************************************************************************/
osd_error_t
show_osd_frame (osd_display_mode_t mode)
{

    UINT16 i;			// temp;

    if (mode == 0)
    {
	//switch to the next OSD frame
	if (osd->current_buff_addr == OSD_STRING_BUFFER1_START)
	{
	    osd->string_st_addr = OSD_STRING_BUFFER2_START;
	}
	else
	{
	    osd->string_st_addr = OSD_STRING_BUFFER1_START;
	}
    }
    i = 0;

    //Get start address of the OSD Framebuffer;
    osd->index_addr = osd->string_st_addr;

    //add the EOF to the buffer
    //FrameBuff[FrameIndex++]=0;
    osd->frame_buff[osd->frame_index++] = 0xaaaa;

    down (&osd->mutex);
    //write the frame structure to chip by USB 
    while (i <= osd->frame_index)
    {
	if (mode == 1)
	{
	    if (osd->last_frame_buff[i] != osd->frame_buff[i])
	    {
		if (cbus_safe_write
		    (osd_write_retry, osd->index_addr, osd->frame_buff[i]) != 0)
		{
		    //semaphore give
		    up (&osd->mutex);
		    return OSD_ERROR_BUS_WRITE_FAIL;
		}
	    }
	}
	else
	{
	    if (cbus_safe_write
		(osd_write_retry, osd->index_addr, osd->frame_buff[i]) != 0)
	    {
		//semaphore give
		up (&osd->mutex);
		return OSD_ERROR_BUS_WRITE_FAIL;
	    }
	}
	osd->index_addr++;
	osd->last_frame_buff[i] = osd->frame_buff[i];
	i++;
    }

    //Change display addr.
    if (cbus_safe_write
	(osd->write_retry, OSD_BUFFER_LOOP_REGISTER, osd->string_st_addr) != 0)
    {
	up (&osd->mutex);
	return OSD_ERROR_BUS_WRITE_FAIL;
    }

    //semaphore give
    up (&osd->mutex);

    //Write down current buffer address 
    osd->frame_index = 0;
    osd->current_buff_addr = osd->string_st_addr;
    osd->end_of_string_x = 0;
    osd->end_of_string_y = 0;
    return OSD_ERROR_OK;

}


/*************************************************************************
*
*
* Get the OSD data to OSD buffer in the OSD frame format 
* StringNum : string number
* *pString strings start address
*  
*
***************************************************************************/
osd_error_t
get_osd_frame (unsigned char num, void *string_ptr)
{
    UINT16 number;
    osd_string_t *temp_ptr = (osd_string_t *) string_ptr;

    //Geetstring number
    number = num;

    //Get frame data
    while (number--)
    {
	PrintMsg (DEBUG_HARDWARE_MISC,
		  ("**************  x:%d, y=%d, string : %s,", temp_ptr->x,
		   temp_ptr->y, &temp_ptr->osd_string[0]));
	//Write a string to OSD frame buffer
	get_osd_data (temp_ptr->x, temp_ptr->y, &temp_ptr->osd_string[0]);
	temp_ptr++;
    }



    return OSD_ERROR_OK;
}
