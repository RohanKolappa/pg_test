#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <libaio.h>
#include <time.h>
#ifndef NO_UDEV
#include <libudev.h>
#endif
#include "../../include/OmniTekIoctl_linux.h"


#define DMA_INTERFACE_EXPORTS
#include "dma_interface.h"


//TODO: We don't support ASYNC mode properly for MDMA.  Either block calls to it (or create a separate FDMA DMA interface that enables them) or sort out the driver?
//      currently the behaviour is misleading!  

CDMAInterface::CDMAInterface(int instance)  :   
  m_instance(instance),
  m_p_io_ctx(0)
{
#ifndef NO_UDEV      
	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Couldn't create udev\n");
    return;
	}

  openBar();

  printf("DMA Interface Opened\n");
  printf("Enumerating Devices:\n");
  PrintDevices();
#endif

}

CDMAInterface::~CDMAInterface()
{
#ifndef NO_UDEV
  udev_unref(udev);
  printf("udev released\n");
#endif

  //Delete all dependent channels:
  while (!m_channel_list.empty())
  {
    IDMAChannel * pChannel = m_channel_list.front();
    delete pChannel;
    printf("Channel Deleted\n");
    m_channel_list.pop_back();
  }

  if (m_p_io_ctx)
  {
    printf("Destroy AIO Context\n");
    io_destroy(m_p_io_ctx);    //Let's just kill the lot of it...
  }

  printf("DMA Interface Closed\n");


}
void CDMAInterface::PrintDevices()
{
#ifndef NO_UDEV
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;
	
  char  board_index_char[4];
  snprintf(board_index_char,4,"%d",Instance());
  
  if (!udev) return;	

  printf("------------------------------------------------------------------------------\n");

	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "omnitek");
  udev_enumerate_add_match_property(enumerate, "DEVTYPE", "omnitek_dma");
  udev_enumerate_add_match_sysattr(enumerate, "board_index", board_index_char);

	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	/* For each item enumerated, print out its information.
	   udev_list_entry_foreach is a macro which expands to
	   a loop. The loop will be executed for each member in
	   devices, setting dev_list_entry to a list entry
	   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path;
		
		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);

		/* usb_device_get_devnode() returns the path to the device node
		   itself in /dev. */
		printf("Device Node Path: %s\n", udev_device_get_devnode(dev));

		printf("Board Index %s, ",
		        udev_device_get_sysattr_value(dev,"board_index"));

    printf("Channel Index %s, ",
		        udev_device_get_sysattr_value(dev,"chan_index"));

    if (atoi(udev_device_get_sysattr_value(dev,"fdma_enabled")))
  		printf("FDMA %s Channel\n",
        ((atoi(udev_device_get_sysattr_value(dev,"fdma_read")) == 1 ) ? "Read" : "Write")); //TODO: Bidir?
		else
      printf("MDMA Channel\n");

		udev_device_unref(dev);
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);
  printf("------------------------------------------------------------------------------\n");
#else
  printf("Device enumeration requires udev support\n");
#endif

}

struct io_context * CDMAInterface::GetIOContext()
{
  //If context does not exist then create one
  if (m_p_io_ctx == NULL)
  {
    int result;
	  printf("Creating AIO Context\n");
	  result = io_setup(20, &m_p_io_ctx);
	  if (result != 0)
	  {
		  printf("AIO Context setup Failed, error: %d - ",result);
		  printf("%s\n",strerror(-result));
		  return NULL;
	  }
	  printf("Created AIO Context\n");

  }
  return m_p_io_ctx;
} 

#define ATTR_TRUE "1"
#define ATTR_FALSE "0"

#ifndef NO_UDEV
int CDMAInterface::OpenChannel(int num, bool fdma, bool write)
{
  //Open the specified DMA channel from our instance

  //Find the correct DMA channel type via udev

	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;
	
  char  board_index_char[4];
  snprintf(board_index_char,4,"%d",Instance());
  
  int index = 0;


  int fd = -ENOTSUP;

  if (!udev) return -EINVAL;	

	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "omnitek");
  udev_enumerate_add_match_property(enumerate, "DEVTYPE", "omnitek_dma");
  udev_enumerate_add_match_sysattr(enumerate, "board_index", board_index_char);
  udev_enumerate_add_match_sysattr(enumerate, "fdma_enabled", fdma ? ATTR_TRUE : ATTR_FALSE);
  if (fdma)
  {
    if (!write)
      udev_enumerate_add_match_sysattr(enumerate, "fdma_read", ATTR_TRUE);
    if (write)
      udev_enumerate_add_match_sysattr(enumerate, "fdma_write", ATTR_TRUE);
  }
  

  printf("Looking for channel: %s", fdma ? "FDMA" : "MDMA");
  if (fdma)
    printf(" %s",write ? "Write" : "Read");
  
  printf(" number %d\n",num);    
  

	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	/* For each item enumerated, print out its information.
	   udev_list_entry_foreach is a macro which expands to
	   a loop. The loop will be executed for each member in
	   devices, setting dev_list_entry to a list entry
	   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path;
		
		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);
  
    if (dev)
    {
      printf("Matching channel found, index %d\n",index);      
  		printf("Device Node Path: %s, ", udev_device_get_devnode(dev));

      printf("Board Index %s, ",
              udev_device_get_sysattr_value(dev,"board_index"));

      printf("Channel Index %s, ",
              udev_device_get_sysattr_value(dev,"chan_index"));

      if (atoi(udev_device_get_sysattr_value(dev,"fdma_enabled")))
    		printf("FDMA %s Channel\n",
          ((atoi(udev_device_get_sysattr_value(dev,"fdma_read")) == 1 ) ? "Read" : "Write")); //TODO: Bidir?
      else
        printf("MDMA Channel\n");

      //Keep getting matches until we find the correct oneint debug_cmd_rr(int argc, char * argv[])
      if (index == num)
      {

        int flags = 0;

        if (fdma && write)
          flags = O_WRONLY;
        else if (fdma)
          flags = O_RDONLY;
        else
          flags = O_RDWR;


        printf("Opening %s with flags %x\n",udev_device_get_devnode(dev), flags );

        fd = open(udev_device_get_devnode(dev), flags );

        if (fd > 0)
        {  		
          udev_device_unref(dev);
          
          printf("Channel Opened!\n");      

          break;
        }
        else
        {
       		perror("Couldn't open channel");

        }
      }

      index++;
    }
		udev_device_unref(dev);

	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);

  if (fd <= 0)
 		printf("Channel NOT opened\n");

  return fd;
}
#else
//For platforms without UDEV we are going to make assumptions - mainly that we are an embedded system with only one board

int CDMAInterface::OpenChannel(int num, bool fdma, bool write)
{
  char filename[32];
  int flags = 0;
  int fd = 0;

  if (fdma)
    sprintf(filename,"/dev/OmnitekFDMA_%s%02d",write ? "WRITE" : "READ", num);
  else
    sprintf(filename,"/dev/OmnitekMDMA%02d",num);

  if (fdma && write)
    flags = O_WRONLY;
  else if (fdma)
    flags = O_RDONLY;
  else
    flags = O_RDWR;

  fd = open(filename,flags);

  if (fd > 0)
  {  		
    udev_device_unref(dev);
    
    printf("Channel Opened!\n");      

    break;
  }
  else
  {
	  perror("Couldn't open channel");

  }
  
  return fd;

}

#endif


IDMAChannel * CDMAInterface::newDmaChannel(int num, bool fdma, bool write)
{
  IDMAChannel * channel = new CDMAChannel(num,fdma,write,this);
  if ((channel) && (channel->isValid()))
    m_channel_list.push_front(channel);
  else
  {
    delete(channel);
    channel = NULL;
  }
   
  return channel;

}


/****************************************
  HERE BE HACKS!
*****************************************/
//TODO: Implement a 'proper' resource
//      based system for reg access

void CDMAInterface::openBar()
{
  m_bar_fd[0] = open("/dev/OmniTekBAR0", O_RDWR );
  if (m_bar_fd[0] >0)
    printf("Opened BAR0 device\n");

  m_bar_fd[1] = open("/dev/OmniTekBAR1", O_RDWR );
  if (m_bar_fd[1] >0)
    printf("Opened BAR1 device\n");
}

__u32 CDMAInterface::ReadRegister(__u32 Bar, __u32 offset)
{
  if (Bar < 2 && m_bar_fd[Bar])
  {
	  OmniTekBARIoctlBuffer buffer;
    int result =0;
    
    buffer.Offset = offset;


    if ((result = ioctl(m_bar_fd[Bar], OMNITEK_BAR_IOCQREADREGISTER, &buffer)))
      printf("Read from bar: %d, offset: %d returned error: %d!\n",Bar,buffer.Offset,result);
    else
      printf("Read from bar: %d, offset: %d returned result: 0x%x\n",Bar,buffer.Offset,buffer.Value);
    
    return buffer.Value;            

  }  

  return -1;
}

void CDMAInterface::WriteRegister(__u32 Bar, __u32 offset,__u32 value)
{
  if (Bar < 2 && m_bar_fd[Bar])
  {
	  OmniTekBARIoctlBuffer buffer;
    int result =0;
    
    buffer.Offset = offset; 
    buffer.Value = value;


    if ((result = ioctl(m_bar_fd[Bar], OMNITEK_BAR_IOCQWRITEREGISTER, &buffer)))
      printf("Write to bar: %d, offset: %d returned error: %d!\n",Bar,buffer.Offset,result);
    else
      printf("Write to bar: %d, offset: %d returned result: 0x%x\n",Bar,buffer.Offset,buffer.Value);
    

  }  

}



/****************************************
  DMA Channel implementation
*****************************************/

CDMAChannel::CDMAChannel(int num, bool fdma, bool write, CDMAInterface * pDMAInterface)  : 
  m_fdma_channel(fdma), 
  m_num_channel(num), 
  m_write(write | !fdma), 
  m_read(!write | !fdma),
  m_valid(false),
  m_pDMAInterface(pDMAInterface)
{
  m_fd_channel = pDMAInterface->OpenChannel(num,fdma,write);
  printf("Got file descriptor: %d\n",m_fd_channel);

  if (m_fd_channel > 0)
  {
    m_valid = true;

    //Default async mode setting - true for FDMA false for MDMA
    SetAsync(fdma);
  }

}

CDMAChannel::~CDMAChannel()
{
  if (m_fd_channel > 0)
  {
    close(m_fd_channel);
    printf("File descriptor %d closed\n",m_fd_channel);
  }
    
}

ssize_t CDMAChannel::MemoryXFer(unsigned char * buffer, off_t offset, size_t size, bool dir_write)
{
  
  //Simple, blocking IO
  lseek(m_fd_channel, offset, SEEK_SET);

  if (dir_write && m_write)
    return write(m_fd_channel, buffer, size);
  
  if (!dir_write && m_read)
    return read(m_fd_channel, buffer, size);
  

  return -ENOTSUP;

}

int   CDMAChannel::MemoryXFerSchedule(DMAXfer * pXfer)
{

  //TODO:  This submits an already created Xfer.  We assume it is correctly configured and
  //       all necessary fields are filled in... maybe we should check...

  int result;

  //Submit the Xfer...
  printf("Submitting Xfer %p iocb %p\n",pXfer,&(pXfer->AsyncXfer.iocb));
  
  //TODO: should we keep a 'master' list of all the iocbs, so that we can clear up?
  struct iocb * pIocb = &(pXfer->AsyncXfer.iocb);

	result = io_submit(pXfer->AsyncXfer.io_ctx,1,&pIocb);
  if (result == 1) return 0;        //1 Request submitted OK
  if (result == 0) return -EINVAL;  //Failed, return error
  return result;                    //Return error code

}

  
int   CDMAChannel::MemoryXFerSchedule(unsigned char * buffer, off_t offset, size_t size, bool dir_write, DMAXfer * pXfer)
{

  //Store information in the Xfer



  pXfer->buffer = buffer;
  pXfer->size = size;
  pXfer->write = dir_write;
  pXfer->offset = offset;

  //Store a pointer to the context (TODO: is this actually needed?)
  pXfer->AsyncXfer.io_ctx = m_pDMAInterface->GetIOContext();
  
  if (pXfer->AsyncXfer.io_ctx == NULL)
  {
    printf("Error: No IO Context\n");
    return -EINVAL;
  }

  printf("Preparing IO %s, buffer @ %p, size: %zd.  Xfer %p\n", (dir_write ? "Write" : "Read"), buffer,size,pXfer);

  if (pXfer->write)
    io_prep_pwrite(&(pXfer->AsyncXfer.iocb),m_fd_channel,pXfer->buffer,size,pXfer->offset);		//Initialize iocb
  else
    io_prep_pread(&(pXfer->AsyncXfer.iocb),m_fd_channel,pXfer->buffer,size,pXfer->offset);		//Initialize iocb

  printf("Read Prepared\n");

  return MemoryXFerSchedule(pXfer);

}

ssize_t   CDMAChannel::MemoryXFerWait(DMAXfer * pXfer)
{
  struct timespec timeout;
  timeout.tv_sec = 3;
  timeout.tv_nsec = 0;
  

  //TODO:  Currently this looks for a specific Xfer to come back.  Instead perhaps it should look
  //       for any Xfer and return it, the user application can then handle the situation where
  //       things come back in the wrong order (as presumably it caused the situation!)

  //Wait on the IOCb for the pXfer to come back, then return a result...
  struct io_event     event;

  printf("Waiting for iocb %p:",&(pXfer->AsyncXfer.iocb));
  int result = io_getevents(m_pDMAInterface->GetIOContext(), 1, 1, &(event),&timeout);

  if (result == 1)
  {
    struct iocb * cur_iocb = (struct iocb *)(event.obj);
    printf(" Got iocb %p\n",cur_iocb);
    if (&pXfer->AsyncXfer.iocb != cur_iocb)   //TODO: This correct?
    {
      printf("Unexpected iocb completed: buffers %p and %p\n",pXfer->buffer, cur_iocb->u.c.buf);
      result = io_cancel(pXfer->AsyncXfer.io_ctx, &(pXfer->AsyncXfer.iocb), &(event));
      if (result)
      {
        printf("Cancelling iocb failed\n");
        return -EINVAL;
      }
      else
      {
        printf("Returning Cancelled\n");
        return -ECANCELED;
      }
    }
      
    return 0;       //Success
  }
  else if (result == 0)
  {
    io_cancel(pXfer->AsyncXfer.io_ctx, &(pXfer->AsyncXfer.iocb), &(event));
     printf(" timed out!!!!!\n");
    return -ETIMEDOUT;    //TODO: If we get timeouts we should probably stop the channel etc.
  }
  else
  {
    printf("error %d\n",result);
    return result;
  }

}

ssize_t   CDMAChannel::MemoryXFerWait(DMAXfer ** ppXfers, int nXfers)
{
  
  return -ENOTSUP;  //TODO: Multiple transfer wait - useful for e.g. field based TX

}

int   CDMAChannel::Start()
{
  int result;
  
  result = ioctl(m_fd_channel,OMNITEK_FDMA_START,1);
  if (result)
    perror("Start channel failed\n");


  return result;

  
}
int   CDMAChannel::Stop()
{
  return ioctl(m_fd_channel,OMNITEK_FDMA_SET_AUTO,1);
}

bool  CDMAChannel::SetAsync(bool async)
{
  //By default MDMA channels are in sync mode, and FDMA in async.
  //In order to correctly do async transactions (with queueing) etc.
  //we need to be able to put the MDMA channels in async mode...
  int result =0 ;  

  if ((result = ioctl(m_fd_channel,OMNITEK_FDMA_SET_AUTO,async ? 0 : 1)))
  {
    perror("Set manual mode for channel failed\n");
    return false;
  }

  return true;
}

#ifdef DMA_INTERFACE_TEST

int main(int argc, char ** argv)
{

  printf("Test DMA Interface\n");

  IDMAInterface * pDmaInterface = newDMAInterface(0);

  IDMAChannel * pDmaChannel = pDmaInterface->newDmaChannel(0,false,false);

  unsigned char buffer[1024];
  ssize_t result = 0;
  
  if ((result = pDmaChannel->MemoryXFer(buffer, 0,1024, false)) != 1024)
    printf("Read Failed : %s\n",strerror(result));    
  else
    printf("Read succeeded!\n");
  if ((result = pDmaChannel->MemoryXFer(buffer, 0,1024, true)) != 1024)
    printf("Write Failed : %s\n",strerror(result));
  else
    printf("Write succeeded!\n");

  //Asynchronous test:

  pDmaChannel->SetAsync(true);

  struct _DMAXfer xfer;
  if ((result = pDmaChannel->MemoryXFerSchedule(buffer, 0,1024, false, &xfer)) != 0)
    printf("Error scheduling Read Xfer %d\n",result);

  //Start Channel
  if ((result = pDmaChannel->Start()) != 0)
    printf("Error starting channel %d\n",result);

  //Wait for transfer
  if ((result = pDmaChannel->MemoryXFerWait(&xfer)) != 0)
    printf("Error On Wait for Read Xfer %d\n",result);
  else
    printf("Async Read OK!\n");
    
  

  if ((result = pDmaChannel->MemoryXFerSchedule(buffer, 0,1024, true, &xfer)) != 0)
    printf("Error scheduling Write Xfer\n");
  
  //Start Channel
  if ((result = pDmaChannel->Start()) != 0)
    printf("Error starting channel %d\n",result);

  //Wait for transfer
  if ((result = pDmaChannel->MemoryXFerWait(&xfer)) != 0)
    printf("Error On Wait for Write Xfer %d\n",result);
  else
    printf("Async Read OK!\n");

  pDmaChannel->SetAsync(false);

  

  deleteDMAInterface( pDmaInterface );

}

#endif

IDMAInterface *   newDMAInterface(int instance)
{
  return new CDMAInterface(instance);
}

void              deleteDMAInterface(IDMAInterface * pDMAInterface)
{
  delete          (CDMAInterface *)pDMAInterface;
}

