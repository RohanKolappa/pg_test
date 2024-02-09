#include <linux/module.h>
#include <linux/socket.h>
#include <linux/miscdevice.h>
#include <linux/list.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/delay.h>
#include <linux/usb.h>
#include <linux/smp_lock.h>
#include <linux/devfs_fs_kernel.h>

#include "WisGoDrv.h"
#include "platform.h"
#include "function.h"
#include "DeviceIO.h"
#include "DeviceCtl.h"
#include "Board.h"
#include "hw_base.h"


/* Device array and mutex */
static PDEVICE_EXTENSION	wisusb_dev_table[MAX_DEVICES];
static DECLARE_MUTEX		(wisusb_dev_table_mutex);
int buffers = 256;

void Config_Initialize(PDEVICE_EXTENSION_COMMON pdxc);


static ssize_t wisgo_read (struct file *file,
			   char *buf, size_t count, loff_t * ppos)
{
	PDEVICE_EXTENSION s;
	int r;
	int FrameSize = count;

//	PrintMsg(DEBUG_DEVICE, ("Enter read"));
	
	s = (PDEVICE_EXTENSION) file->private_data;

	r = ProcessReadRequest(&(s->Common), buf, &FrameSize);

	if(r == 0)
		r = FrameSize;
	else
		r = 0;

//	PrintMsg(DEBUG_DEVICE, ("Leave read : %d",r));
	return r;
}

static ssize_t wisgo_write(struct file *file,
			   const char *buf, size_t count, loff_t * ppos)
{
	PDEVICE_EXTENSION s;
	s = (PDEVICE_EXTENSION) file->private_data;
	return 0;
}

static int wisgo_ioctl (struct inode *inode,
			struct file *file,
			unsigned int cmd, unsigned long arg)
{
	PDEVICE_EXTENSION s;
	int ret_len;
	s = (PDEVICE_EXTENSION) file->private_data;
	
//	dbg("ioctl %x %x", (int)cmd, (int)arg);
	
	if(cmd != IOCTL_WISGO_COMMAND)
		return 0;
	return ProcessIOCTL(&(s->Common), (void *)arg, &ret_len);
}

static int wisgo_open (struct inode *inode, struct file *file)
{
	PDEVICE_EXTENSION s;
	int minor;
	int rv;

	PrintMsg(DEBUG_DEVICE, ("<DRV> Enter open"));
	rv = 0;
	minor = MINOR(inode->i_rdev);
	PrintMsg(DEBUG_DEVICE, ("<DRV> minor = %d", minor));

	if(minor < 0 || minor >= MAX_DEVICES)
		return -ENODEV;

	down(&wisusb_dev_table_mutex);

	s = wisusb_dev_table[minor];

	if(s == NULL)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("<DRV> device (%d) not exist", minor));
		up(&wisusb_dev_table_mutex);
		return -ENODEV;
	}

	if (s->Common.OpenHandleCount > 0)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("<DRV> ERROR: device already open"));
		rv = -EIO;
		goto exit;
	}
	/*
	 * When the driver is closed, the WIS chip is stopped and
	 * the kernel threads no longer process read requests for
	 * Audio and Video URBs. So, we need to make sure that any
	 * outstanding requests from the last run of the streams
	 * are cancelled. We do this by flushing the URB for
	 * the Audio and Video stream pipes.
	 */
	if (s->Common.video_urb)
	    usb_unlink_urb(s->Common.video_urb);

	if (s->Common.audio_urb)
	    usb_unlink_urb(s->Common.audio_urb);	    

	/* Resume setup ... */
	s->Common.ChipType = SpongeBob;
	s->Common.BusType = WisUsb;

	(s->Common.hw_funcs.InitializeBus)(&(s->Common));
	
	InitBoardInterface(&(s->Common));
	/*
	if (s->Common.BootupSuccess == 0)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("ERROR: device does not bootup"));
		rv = -EIO;
		goto exit;
	}
	*/
	if ( (s->Common.BootupSuccess == 0) || (s->Common.ChipState != DS_IDLE) )
	{
	  // normally done by first "probe" and last "release" 
	  // - try once in case of bad last exit (ie without proper shutdown)
	  if(BootupChip(&(s->Common)) != 0)
	    {
	      s->Common.BootupSuccess = 0;
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("ERROR: device is not idle (%d)",
			     s->Common.ChipState));
		rv = -EIO;
		goto exit;
	    }
	}

	file->f_pos = 0;
	file->private_data = s;

	s->Common.OpenHandleCount ++;
	s->Common.DeviceId = s->minor;
	s->Common.videoretries=0;
	s->Common.audioretries=0;
	PrintMsg(DEBUG_DEVICE, ("1111 OpenHandleCount %d",s->Common.OpenHandleCount));
exit:
	up(&wisusb_dev_table_mutex);
	if (!rv)
		MOD_INC_USE_COUNT;

	PrintMsg(DEBUG_DEVICE, ("<DRV> Leave open : %d",rv));
	return rv;
}


static int wisgo_release (struct inode *inode, struct file *file)
{
	PDEVICE_EXTENSION s;
	int rv;

	PrintMsg(DEBUG_DEVICE, ("<DRV> Enter release"));	

	MOD_DEC_USE_COUNT;

	s = (PDEVICE_EXTENSION) file->private_data;
	
	/* Lock minor table and our device */
	down(&wisusb_dev_table_mutex);

	//	if(s->Common.ChipState == DS_RUNNING)
	if(s->Common.ChipState != DS_IDLE)
		UninitializeDevice(&(s->Common));


	if(s->Common.OpenHandleCount < 1 || s->Common.ChipState != DS_IDLE)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("Leave ChipState != DS_IDLE %d",s->Common.ChipState));
		rv = -ENODEV;
		goto exit;
	}

	if(s->Common.usbdev == NULL)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("Leave s->Common.usbdev == NULL"));
		goto exit;	
	}

	s->Common.OpenHandleCount --;
	s->Common.ChipState = DS_IDLE;
	
exit:
	up(&wisusb_dev_table_mutex);

	PrintMsg(DEBUG_DEVICE, ("Leave"));
	return 0;
}

static struct file_operations wisgo_fops =
{
	owner:		THIS_MODULE,
	read:		wisgo_read,
	write:		wisgo_write,
	ioctl:		wisgo_ioctl,
	open:		wisgo_open,
	release:	wisgo_release,
};

/* --------------------------------------------------------------------- */
static struct usb_driver wisgo_driver;

static void *wisgo_probe (struct usb_device *usbdev, unsigned int ifnum,
			   const struct usb_device_id *id)
{
	int i;
	int minor= 0;
//	struct usb_endpoint_descriptor *pEP;
	PDEVICE_EXTENSION s = NULL;
	int bcdDevice;
//	int rv;

	PrintMsg(DEBUG_DEVICE, ("Enter wisgo_probe"));
	
	PrintMsg (DEBUG_ALWAYS_DISPLAY|DEBUG_NOTICE_LEVEL, ("vendor id 0x%x, device id 0x%x ifnum:%d",
		      usbdev->descriptor.idVendor,
		      usbdev->descriptor.idProduct, ifnum));


	/* We don't handle multiple configurations */
	if (usbdev->descriptor.bNumConfigurations != 1)
		return NULL;

	down(&wisusb_dev_table_mutex);


	for(minor = 0; minor < MAX_DEVICES; minor++)
	{
		if(wisusb_dev_table[minor] == NULL)
			break;
	}
	
	if(minor < 0 || minor >= MAX_DEVICES) 
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("Invalid minor: %d", minor));
		goto failed;
	}

	/* Allocate dev data structure */
	s = kmalloc(sizeof(DEVICE_EXTENSION), GFP_KERNEL);
	if(s == NULL)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("Out of memory"));
		goto failed;
	}
	wisusb_dev_table[minor] = s;

	PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_NOTICE_LEVEL, ("Device %d on Bus %d Minor %d",
		  usbdev->devnum, usbdev->bus->busnum, minor));

	/* Initialization */
	memset (s, 0, sizeof (DEVICE_EXTENSION));

//	init_MUTEX (&s->mutex);
	s->Common.usbdev		= usbdev;
	s->Common.unit                  = minor;
	s->minor			= minor;
	//alloc urb for audio video and interrupt
	//audio urb could be alloced at alloc audio buffer(function.c)
	s->Common.audio_urb = usb_alloc_urb(0);
	//video urb could be alloced at alloc video buffer(function.c)
	s->Common.video_urb = usb_alloc_urb(0);
	s->Common.downloadbuffer_urb = usb_alloc_urb(0);
	if ((!s->Common.downloadbuffer_urb) ||
	    (!s->Common.video_urb) || (!s->Common.audio_urb))
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("wisgo_probe: D:%x,v:%x,a:%x",
			     (int)s->Common.downloadbuffer_urb,
			     (int)s->Common.video_urb,
			     (int)s->Common.audio_urb));
		goto failed;
	
	}

	PIInitializeEvent(&s->Common.wq_control_pipe);
	PIInitializeEvent(&s->Common.wq_download_pipe);
	PIInitializeEvent(&s->Common.wq_stream_pipe);
	PIInitializeEvent(&s->Common.wq_audio_stream_pipe);
	PIInitializeEvent(&s->Common.wq_interrupt_pipe);
	PIInitializeEvent(&s->Common.wq_interrupt_event);
	
//	down (&s->mutex);

	PrintMsg(DEBUG_DEVICE, ("<DRV> ---------- usb device description"));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bLength            : 0x%02x",
		     usbdev->descriptor.bLength));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bDescriptorType    : 0x%02x",
		     usbdev->descriptor.bDescriptorType));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bcdUSB             : 0x%04x",
		     usbdev->descriptor.bcdUSB));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bDeviceClass       : 0x%02x",
		     usbdev->descriptor.bDeviceClass));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bDeviceSubClass    : 0x%02x",
		     usbdev->descriptor.bDeviceSubClass));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bDeviceProtocol    : 0x%02x",
		     usbdev->descriptor.bDeviceProtocol));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bMaxPacketSize0    : 0x%02x",
		     usbdev->descriptor.bMaxPacketSize0));
	PrintMsg(DEBUG_DEVICE, ("<DRV> idVendor           : 0x%04x",
		     usbdev->descriptor.idVendor));
	PrintMsg(DEBUG_DEVICE, ("<DRV> idProduct          : 0x%04x",
		     usbdev->descriptor.idProduct));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bcdDevice          : 0x%04x",
		     usbdev->descriptor.bcdDevice));
	PrintMsg(DEBUG_DEVICE, ("<DRV> iManufacturer      : 0x%02x",
		     usbdev->descriptor.iManufacturer));
	PrintMsg(DEBUG_DEVICE, ("<DRV> iProduct           : 0x%02x",
		     usbdev->descriptor.iProduct));
	PrintMsg(DEBUG_DEVICE, ("<DRV> iSerialNumber      : 0x%02x",
		     usbdev->descriptor.iSerialNumber));
	PrintMsg(DEBUG_DEVICE, ("<DRV> bNumConfigurations : 0x%02x",
		     usbdev->descriptor.bNumConfigurations));

	/* Enumerate endpoints */

	{
		int j,k,l;
		for (i = 0; i < usbdev->descriptor.bNumConfigurations; i++) 
		{
			struct usb_config_descriptor *cfg = &usbdev->config[i];
			PrintMsg(DEBUG_DEVICE, ("<DRV> %d: bNumInterfaces %d",i,cfg->bNumInterfaces));
			for (j = 0; j < cfg->bNumInterfaces; j++)
			{
				struct usb_interface *ifp = &cfg->interface[j];
				PrintMsg(DEBUG_DEVICE, ("<DRV>  %d: num_altsetting %d",j, ifp->num_altsetting));
				for (k = 0; k < ifp->num_altsetting; k++) 
				{
					struct usb_interface_descriptor *as = &ifp->altsetting[k];
					PrintMsg(DEBUG_DEVICE, ("<DRV>   %d: bNumEndpoints %d",k,as->bNumEndpoints));
					for(l = 0; l < as->bNumEndpoints; l++)
					{
						struct usb_endpoint_descriptor *ep=&as->endpoint[l];
						PrintMsg(DEBUG_DEVICE, ("<DRV>    %d ------- %d 0x%02x %02d %x %d",l,
							ep->bDescriptorType,
							ep->bEndpointAddress,
							ep->wMaxPacketSize,
							ep->bmAttributes, ep->bInterval));
					}	
				} 
			}
		}
	}


	bcdDevice = usbdev->descriptor.bcdDevice;

	s->Common.BoardVendorId    = usbdev->descriptor.idVendor;
	s->Common.BoardProductId   = usbdev->descriptor.idProduct;
	s->Common.BoardRevisionNum = usbdev->descriptor.bcdDevice;

	s->Common.ReadInterruptState = 0;
	
//	up (&s->mutex);

	s->Common.ChipState = DS_IDLE;
	s->Common.OpenHandleCount = 0;

	s->Common.DeviceConnected = 1;

	OnAddDevice(&(s->Common));
	up(&wisusb_dev_table_mutex);

	s->Common.ChipType = SpongeBob;
	s->Common.BusType = WisUsb;

	HW_BASE_SetupHwDispatch(&(s->Common));

	(s->Common.hw_funcs.InitializeBus)(&(s->Common));

	InitBoardInterface(&(s->Common));
	/*
	 * Kidnap USB Audio descriptor.
	 * On XMEN-III : we determine this is an XMEN with Audio and
	 * a new USB EEPROM endpoint configuration.
	 */
	/*if(s->Common.BoardInfo.BusMode == UM_USB_ONCHIP_MODE1 &&
	   s->Common.BoardInfo.RevisionNum == 0x0209)*/
	if(s->Common.BoardInfo.BusMode == UM_USB_ONCHIP_MODE1)
	{
	    usb_driver_claim_interface(&wisgo_driver,
				       &usbdev->actconfig->interface[1], s); 
	    usb_driver_claim_interface(&wisgo_driver,
				       &usbdev->actconfig->interface[2], s); 
	}
	
	if(BootupChip(&(s->Common)) != 0)
	{
		s->Common.BootupSuccess = 0;
		PrintMsg(DEBUG_DEVICE, ("Leave (successful)"));
		return s;
	}

	// not needed - done at end in bootupchip	
	//	s->Common.BootupSuccess = 1;
	s->Common.testFlag = 0;		
	Config_Initialize(&(s->Common));	

	SuspendDebugOutput();
	s->Common.BoardInterface->pIBoardVideo->
			InitializeSensor(
				s->Common.BoardContext,
				&(s->Common),
                TVStandard_Default,
				VS_DEFAULT,
				s->Common.config_manager._sensor_config);
		
	ResumeDebugOutput();

	PrintMsg(DEBUG_DEVICE, ("Leave (successful)"));
	return s;

failed:
	up(&wisusb_dev_table_mutex);

	PrintMsg(DEBUG_DEVICE, ("Leave (probe failed)"));
	return NULL;
}


static struct usb_device_id wisgo_ids [] = {
	{ USB_DEVICE(0x0EB1, 0x7007) },
	{ }						/* Terminating entry */
};


MODULE_DEVICE_TABLE (usb, wisgo_ids);
static void wisgo_disconnect (struct usb_device *usbdev, void *ptr);
static struct usb_driver wisgo_driver =
{
	name:		        "wisgo",
	probe:		        wisgo_probe,
	disconnect:		wisgo_disconnect,
	id_table:		wisgo_ids,
};
static void wisgo_disconnect (struct usb_device *usbdev, void *ptr)
{
	int minor;
	PDEVICE_EXTENSION s = (PDEVICE_EXTENSION) ptr;
	PDEVICE_EXTENSION_COMMON pdxc = (PDEVICE_EXTENSION_COMMON)&(s->Common);


	PrintMsg(DEBUG_DEVICE, ("Enter disconnect"));

	minor = s->minor;

//	down (&s->mutex);

	OnRemoveDevice(&(s->Common));
	// free urb buffer I could free audio and video at
	// releasebuffer(function.c) but I don't want to use ifdef LINUX
	if (pdxc->audio_urb)
	{
	    usb_unlink_urb(pdxc->audio_urb);
	    usb_free_urb(pdxc->audio_urb);
	}
	if (pdxc->video_urb)
	{
	    usb_unlink_urb(pdxc->video_urb);
	    usb_free_urb(pdxc->video_urb);
	}
	
	if (pdxc->downloadbuffer_urb)
	{
	    usb_unlink_urb(pdxc->downloadbuffer_urb);
	    usb_free_urb(pdxc->downloadbuffer_urb);
	}
//	up(&s->mutex);
/*	if(s->Common.BoardInfo.BusMode == UM_USB_ONCHIP_MODE1 &&
	   s->Common.BoardInfo.RevisionNum == 0x0209)*/
	if(s->Common.BoardInfo.BusMode == UM_USB_ONCHIP_MODE1)
	{
		if (usbdev)
		{

			usb_driver_release_interface(&wisgo_driver, &usbdev->actconfig->interface[1]); 
			usb_driver_release_interface(&wisgo_driver, &usbdev->actconfig->interface[2]); 
		}

	}

	wisusb_dev_table[minor] = NULL;
	kfree(s);

	PrintMsg(DEBUG_DEVICE, ("WISGO7007 #%d now disconnected", minor));
}


/* --------------------------------------------------------------------- */

static int __init wisgo_init (void)
{
	int rv;
	unsigned i;

	/* initialize struct */
	for (i = 0; i < MAX_DEVICES; i++)
		wisusb_dev_table[i] = NULL;
	

	/* Register devfs device*/
	rv = devfs_register_chrdev(USB_WISGO_MAJOR,
				   "wisgo7007", &wisgo_fops);

	if(rv)
	{
	    PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL,
		     ("failed to register devfs device. Error number %d", rv));
	    return -1;
	}

	/* Register usb device */
	rv = usb_register(&wisgo_driver);
	if(rv)
	{
		PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_ERROR_LEVEL, ("usb_register failed. Error number %d", rv));
		return -1;
	}

	PrintMsg(DEBUG_DEVICE, ("wisusb_init: driver registered"));

	//PrintMsg(DEBUG_DEVICE, "%s %s",DRIVER_DESC,DRIVER_VERSION));
	return 0;
}

static void __exit wisgo_cleanup (void)
{
	PrintMsg(DEBUG_DEVICE, ("wisusb_cleanup"));

	usb_deregister (&wisgo_driver);
	devfs_unregister_chrdev(USB_WISGO_MAJOR, "wisgo7007");
}


/* --------------------------------------------------------------------- */

MODULE_AUTHOR ("JUN DING & HONGJUN YUAN & JAMES DOUGHERTY");
MODULE_DESCRIPTION ("WIS GO 7007SB Driver for Linux (c)2002-2003");
MODULE_PARM (buffers, "i");
MODULE_PARM_DESC (buffers, "Number of buffers (default=256)");
MODULE_LICENSE("GPL");

module_init (wisgo_init);
module_exit (wisgo_cleanup);

