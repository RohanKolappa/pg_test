//EFDMA Test Module:
//Create a device that we can use to test the EFDMA implementation.
#include "test_efdma.h"

struct test_efdma_struct test_efdma;

void test_efdma_freepackets(
  struct test_efdma_struct * pTest
)
{
  
  struct test_efdma_packet * pPacket, * pPacket_temp;
  spin_lock(&(pTest->SpinLock));
  
  list_for_each_entry_safe(pPacket,pPacket_temp,&(pTest->Packets),Object)
  {
    kfree(pPacket->buffer);
    kfree(pPacket);
    list_del(&(pPacket->Object));
  }

  spin_unlock(&(pTest->SpinLock));
}

DECLARE_WAIT_QUEUE_HEAD(test_efdma_packet_queue);


int test_efdma_receive_func(struct efdma_channel * pEFDMA_Channel,void * priv, ssize_t size)
{

  //On packet receive we are going to store the data temporarily until (hopefully) a nice IOCtl comes along and takes it away from us
  
  //Scan through the packets:
  
  //Allocate a new packet
  struct test_efdma_packet * pPacket = kmalloc(sizeof(struct test_efdma_packet),GFP_ATOMIC);  
  pPacket->buffer = kmalloc(size,GFP_ATOMIC);

  if (!pPacket->buffer)
    return -ERESTARTSYS;

  OmniTekDebug(TESTEFDMA, KERN_INFO, "packet length: %zd\n",size);

  INIT_LIST_HEAD(&(pPacket->Object));

  //Receive data into the packet
  pPacket->length = efdma_channel_receive_packet(pEFDMA_Channel,pPacket->buffer,size);


  //Add the packet to the list
  spin_lock(&(test_efdma.SpinLock));
  list_add_tail(&(pPacket->Object),&(test_efdma.Packets));
  test_efdma.nPackets++;
  OmniTekDebug(TESTEFDMA, KERN_INFO, "Received packet %d, length: %zd.  Number of packets in buffer: %d\n",test_efdma.nPackets_rxd, pPacket->length, test_efdma.nPackets);
  test_efdma.nPackets_rxd++;
  spin_unlock(&(test_efdma.SpinLock));


  //Re-enable interrupts...
  //efdma_channel_rx_interrupt_enable(test_efdma.pEFDMA_Channel, true);

  wake_up(&test_efdma_packet_queue);

  return 0;
}
efdma_receive_func test_efdma_receive_func_ptr = test_efdma_receive_func;

int test_efdma_dev_ioctl(struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg)
{

  int retval = -ENOTTY;

  struct efdma_packet_iobuf * p_user_iobuf = (struct efdma_packet_iobuf *)arg;
  struct efdma_packet_iobuf   kern_iobuf;


  switch (cmd)
  {
  
    case OMNITEK_BAR_IOCEFDMAWRITEPACKET:
    {
      //We get a packet from user space, that we can then transmit out...
      char * buffer;
      ssize_t transmitted;
    
      OmniTekDebug(TESTEFDMA, KERN_INFO, "Test EFDMA Write Packet\n");

      if (copy_from_user(&kern_iobuf, p_user_iobuf, sizeof(struct efdma_packet_iobuf)))
      {
        OmniTekDebug(TESTEFDMA, KERN_ERR, "Couldn't copy user IO Buffer for packet write...\n");
        retval = -EINVAL;
        break;
      }
      
      //Allocate a buffer locally
      if (!(buffer = kmalloc(kern_iobuf.length,GFP_KERNEL)))
      {
        OmniTekDebug(TESTEFDMA, KERN_ERR, "Couldn't allocate kernel data Buffer for packet write...\n");
        retval = -EINVAL;
        break;
      }
    
      //Copy data to buffer
      memcpy(buffer,kern_iobuf.buffer,kern_iobuf.length);
      
      //Transmit the packet
      transmitted = efdma_channel_transmit_packet(test_efdma.pEFDMA_Channel, buffer, kern_iobuf.length, 0, 0); 

      if (transmitted != 0)
      {
        OmniTekDebug(TESTEFDMA, KERN_ERR, "Packet transmit returned length of %zd, expected %zd\n",transmitted, kern_iobuf.length);
        retval = -EINVAL;
      }
      else
      {
        OmniTekDebug(TESTEFDMA, KERN_INFO, "Transmitted packet %d, length: %zd\n",test_efdma.nPackets_txd, kern_iobuf.length);
        test_efdma.nPackets_txd++;
        retval = 0;
      }

      kfree(buffer);

      

      break;
    }
    case OMNITEK_BAR_IOCEFDMAREADPACKET:
    {
      //We are given a user space buffer with up to TEST_EFDMA_MAX_PACKET_SIZE length, we copy one of our received packets into it and release
      //it from our list...
      struct test_efdma_packet * pPacket;
      int result=  0;

      OmniTekDebug(TESTEFDMA, KERN_INFO, "Test EFDMA Read Packet\n");

      result = wait_event_interruptible_timeout(test_efdma_packet_queue,test_efdma.nPackets > 0,msecs_to_jiffies(1000));
       
      if ((result == 0) && (test_efdma.nPackets == 0))
      {
        OmniTekDebug(TESTEFDMA, KERN_ERR, "Timeout waiting for packet\n");
        retval = -EINVAL;
        break;
      }

      spin_lock(&(test_efdma.SpinLock));  
  
  

      //Get first packet in the list
      pPacket = list_entry(test_efdma.Packets.next,struct test_efdma_packet, Object);

      //Remove the packet from the list
      list_del(&(pPacket->Object));
      test_efdma.nPackets--;
    
      //Get the user IO Buffer packet
      if (copy_from_user(&kern_iobuf, p_user_iobuf, sizeof(struct efdma_packet_iobuf)))
      {
        OmniTekDebug(TESTEFDMA, KERN_ERR, "Couldn't copy user IO Buffer for packet read...\n");
        retval = -EINVAL;
        spin_unlock(&(test_efdma.SpinLock));
        break;
      }
      
      //kern_iobuf.buffer is the user space buffer that we can use:
      if (pPacket->length > kern_iobuf.length)
      {
        OmniTekDebug(TESTEFDMA, KERN_ERR, "Packet is too large for user buffer have %zd, need %zd\n", kern_iobuf.length, pPacket->length);
        retval = -EINVAL;
        spin_unlock(&(test_efdma.SpinLock));
        break;
      }

      //Copy data to the user buffer
      OmniTekDebug(TESTEFDMA, KERN_INFO, "Copying packet to buffer, length is: %zd\n",pPacket->length);
      copy_to_user(kern_iobuf.buffer,pPacket->buffer,pPacket->length);
      kern_iobuf.length = pPacket->length;
      //Copy the io buffer back
      copy_to_user(p_user_iobuf,&kern_iobuf,sizeof(struct efdma_packet_iobuf));

      //Free the packet
      kfree(pPacket->buffer); pPacket->buffer = NULL;
      kfree(pPacket); pPacket = NULL;

      retval = 0;      
    
      OmniTekDebug(TESTEFDMA, KERN_INFO, "Packet Received.  Remaining: %d\n", test_efdma.nPackets);

      spin_unlock(&(test_efdma.SpinLock));
      break;
    }

    default:
      retval = -ENOTTY;
      break;
  }

  return retval;
}

int test_efdma_dev_open(struct inode * inode, struct file * filp)
{
  return 0;
}

int test_efdma_dev_release(struct inode * inode, struct file * filp)
{
	return 0;
}

static const struct file_operations test_efdma_fops = {
  .owner = THIS_MODULE,
  .unlocked_ioctl = test_efdma_dev_ioctl,
  .open = test_efdma_dev_open,
  .release = test_efdma_dev_release
};


int test_efdma_setupdev(struct test_efdma_struct * pTest)
{
  int result = 0;

  if ((result = alloc_chrdev_region(&(pTest->dev),0,1,"OmniTek EFDMA Test")))
  {
    OmniTekDebug(GENERAL, KERN_ERR, "Error allocating char device!\n");
    return result;
  }

  OmniTekDebug(GENERAL,KERN_INFO,"Got Major %d\n",MAJOR(pTest->dev));
  OmniTekDebug(GENERAL,KERN_INFO,"allocated minor %d\n",MINOR(pTest->dev));    

  //Only need to do this once, we can get the minor number back later so we know which channel we are referring to...
  cdev_init(&(pTest->cDev),&(test_efdma_fops));
  pTest->cDev.owner = THIS_MODULE;
  pTest->cDev.ops = &(test_efdma_fops);

  //Add Device
  result = cdev_add(&(pTest->cDev),pTest->dev,1);
	if (result)
  {
		OmniTekDebug(GENERAL, KERN_ERR, "General: Startup : OmniTekDeviceSetupDev Error %d adding cdev",result);    
    cdev_del(&(pTest->cDev));
    
    return result;
  }

  pTest->cDev.ops = &(test_efdma_fops);
  
  //Create /dev entry
	device_create(pTest->efdma_class,NULL,pTest->dev,pTest,"OmniTek_EFDMA_TEST");

  return 0;

}

int test_efdma_deletedev(struct test_efdma_struct * pTest)
{
  
	device_destroy(pTest->efdma_class,pTest->dev);

  cdev_del(&(pTest->cDev));
 	unregister_chrdev_region(pTest->dev,1);


  return 0;

}


static int __init test_efdma_init(void)
{

  //Get the OmniTek Extension
  if (!(test_efdma.pExt = OmniTekGetExtension(0)))
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Couldn't get OmniTek Extension\n");
    return -EINVAL;
  }

  //Get the efdma channel
  if (efdma_channel_acquire(test_efdma.pExt,2,&(test_efdma.pEFDMA_Channel),false))
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Couldn't get EFDMA Channel\n");
    return -EINVAL;
  }
   
  //Register our callback function with the EFDMA channel...
  if (efdma_channel_set_rx_callback(test_efdma.pEFDMA_Channel, &test_efdma_receive_func_ptr, NULL))
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Error setting callback\n");
    goto out_release;
  }


  //Initialise some other stuff
  INIT_LIST_HEAD(&(test_efdma.Packets));
  spin_lock_init(&(test_efdma.SpinLock));
  test_efdma.efdma_class = class_create(THIS_MODULE, "omnitek_efdma");
  test_efdma.nPackets = 0;
  test_efdma.nPackets_txd = 0;
  test_efdma.nPackets_rxd = 0;


  //Setup device
  if (test_efdma_setupdev(&test_efdma))
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Error creating EFDMA Test devices\n");
    goto out_release;
  }

  
  efdma_channel_rx_interrupt_enable(test_efdma.pEFDMA_Channel, true);
  efdma_channel_start(test_efdma.pEFDMA_Channel);

  
  OmniTekDebug(GENERAL,KERN_INFO,"EFDMA Test device loaded\n");
  
  return 0;


out_release:
  efdma_channel_release(test_efdma.pEFDMA_Channel);
  return -EINVAL;
  


}



static void __exit test_efdma_exit(void)
{
  //Unregister devices
  test_efdma_deletedev(&test_efdma);
  //Release any outstanding packets
  test_efdma_freepackets(&test_efdma);
  //Release the EFDMA channel
  efdma_channel_release(test_efdma.pEFDMA_Channel);

  class_destroy(test_efdma.efdma_class);


}

MODULE_LICENSE("GPL");
module_init(test_efdma_init);
module_exit(test_efdma_exit);

