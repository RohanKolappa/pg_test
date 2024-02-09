/// define.h

#ifndef __DEFINE_H__
#define __DEFINE_H__

#include "common.h"

typedef struct
{
	/* Basic device information */
	int					minor;

	/* Wait queue */
	wait_queue_head_t	wq_stop_stream_thread;
	wait_queue_head_t	wq_stop_ratectrl_thread;

	/* Interrupt pipe */
	struct
	{
		short			ready;
		unsigned short	retval;
		unsigned short	retdata;
	} rd_irq_data;

	struct semaphore	mutex;
	wait_queue_head_t	wait;
	wait_queue_head_t	remove_ok;
	spinlock_t			lock;
	atomic_t			pending_io;
	int					remove_pending;
	int					got_mem;
	int					total_buffer_size;
	unsigned int		overruns;
	int					readptr;
	struct list_head	free_buff_list;
	struct list_head	rec_buff_list;
	
	STREAM_BUFFER		StreamBuffer;
	int					InterruptThreadState;
	int					StreamThreadState;
	int					RateCtrlThreadState;

	/* bulk transfer */
	int				bulk_out_size;

	unsigned char*	bulk_in_buffer;
	unsigned char*	bulk_out_buffer;

	DEVICE_EXTENSION_COMMON Common;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

/* Version Information */
#define	DRIVER_VERSION	"v1.0"
#define	DRIVER_DESC	"WIS GO7007SB USB Driver"

/*
 * Devices information
 */

#define	USB_WISGO_MAJOR		240
#define	USB_WISGO_MINOR_BASE	0
#define MAX_DEVICES		256

#endif
