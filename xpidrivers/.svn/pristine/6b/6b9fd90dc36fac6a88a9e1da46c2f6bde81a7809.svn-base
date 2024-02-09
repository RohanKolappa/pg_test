/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific terms and
 * conditions.  Use of any of the software or derviatives thereof in any
 * product without a Image Processing Techniques is strictly prohibited. 
 * 
 * This file is provided without any express or implied warranties, including, 
 * but not limited to, the implied warranties of merchantability and fitness 
 * for a particular purpose. It is not intended for use in life support 
 * appliances, devices, or systems. Use in such applications is expressly 
 * prohibited.
 *
 * Image Processing Techniques makes no guarantee or representations regarding the use of, 
 * or the results of the use of, the software and documentation in terms of correctness, 
 * accuracy, reliability, currentness, or otherwise; and you rely on the software,
 * documentation and results solely at your own risk.
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF USE, 
 * LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL
 * DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/

#include "OmniTekFops_linux.h"
#include "OmniTekResources_sysfs.h"


static struct _OmniTek_dev OmniTekBoardDev ;

int OmniTek_dev_open(struct inode * inode, struct file * filp)
{
  
  return -ENXIO;
}

static const struct file_operations OmniTek_dev_fops = {
  .open = OmniTek_dev_open
};

const struct file_operations OmniTek_BAR_dev_fops = {
  .owner = THIS_MODULE,
  .ioctl = OmniTek_BAR_dev_ioctl,
  .open = OmniTek_BAR_dev_open,
  .release = OmniTek_BAR_dev_release
};

/*!
 \brief   Create system /dev entries for our device
 \details Once the hardware is fully elaborated we create the system devices
          for our device.  
*/
void
OmniTekDeviceSetupDev(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
)
{
  OmniTekDriver * omnitek_driver = pExt->pDriver;
  struct _OmniTek_dev * dev = &OmniTekBoardDev;

  //Many of the resources require a corresponding device for IO (e.g. the DMA channels).  These will be created here.
  //For now the BARs will be given devices so that we can read their registers...

  OmniTekDebug(GENERAL,KERN_INFO,"Setting up devices, have omnitek_driver at %p\n",omnitek_driver);
   
  if (!omnitek_driver)
  {
    OmniTekDebug(GENERAL, KERN_ERR, "Setting up devices - omnitek_driver is NULL!!!\n");
    return;
  }

  if (pExt->nBars > 0)
  {
    int err;
    //int devno = 0;
  	int result = 0;
    int i = 0;

    alloc_chrdev_region(&(dev->dev),0,pExt->nBars,"OmniTek Driver");

    //Set device number in BAR resource
    for (i = 0; i < pExt->nBars;i++)
      pExt->MemBar[i].dev = MKDEV(MAJOR(dev->dev),MINOR(dev->dev)+i);  
  
    OmniTekDebug(GENERAL,KERN_INFO,"Got Major %d\n",MAJOR(dev->dev));
    OmniTekDebug(GENERAL,KERN_INFO,"allocated minors %d to %d\n",MINOR(dev->dev),MINOR(dev->dev)+pExt->nBars-1);

    if (result)
    {
      OmniTekDebug(GENERAL, KERN_ERR, "OmniTekDeviceSetupDev couldn't allocate minor numbers!\n");
      return;
    }
      
  
    //Only need to do this once, we can get the minor number back later so we know which channel we are referring to...
    INIT_LIST_HEAD(&(dev->Object));
    dev->Type = OMNITEK_DEV_BAR;
    cdev_init(&(dev->cDev),&(OmniTek_BAR_dev_fops));
    dev->cDev.owner = THIS_MODULE;
    dev->cDev.ops = &(OmniTek_BAR_dev_fops);
    dev->pExt = pExt;
    dev->firstMinor = MINOR(dev->dev);
    dev->nMinors = pExt->nBars;

  
    //Add BAR Devices:
    err = cdev_add(&(dev->cDev),dev->dev,pExt->nBars);
  	if (err)
  		OmniTekDebug(GENERAL, KERN_ERR, "General: Startup : OmniTekDeviceSetupDev Error %d adding cdev",err);    

    //Create BAR device entries now that we have device numbers...
    for (i=0; i<pExt->nBars; i++)
      omnitek_BAR_sysfs_create(pExt,i);

        
    list_add_tail(&(dev->Object),&(omnitek_driver->Devices));
    OmniTekDebug(GENERAL,KERN_INFO,"Startup : OmniTekDeviceSetupDev done!\n");
    dev->cDev.ops = &(OmniTek_BAR_dev_fops);

  }

}


void 
OmniTekDeviceReleaseDev(
  POMNITEK_INTERFACE_EXTENSION pExt
)
{
  struct _OmniTek_dev * dev,*dev_temp;

  OmniTekDriver * omnitek_driver = GetOmniTekDriver(pExt->Device->driver);
  OmniTekDebug(GENERAL,KERN_INFO,"Device minors release for pExt @ %p, driver @ %p\n",pExt,omnitek_driver);

  //Run through all the items in the driver and remove the devices corresponding to this pExt  
  list_for_each_entry_safe(dev,dev_temp,&(omnitek_driver->Devices),Object)
  {
    
    if ((dev->pExt == pExt) && (dev->Type == OMNITEK_DEV_BAR))
    {  
      OmniTekDebug(GENERAL,KERN_INFO,"pExt @ %p found, releasing\n",pExt);
      cdev_del(&(dev->cDev));
      list_del(&(dev->Object));
      OmniTekDebug(GENERAL,KERN_INFO,"Shutdown : unregistering minors %d to %d\n", dev->firstMinor,dev->firstMinor+dev->nMinors-1);
    	unregister_chrdev_region(dev->dev,dev->nMinors);
      break;
    }
  }
  if (dev->pExt != pExt)
  {
    OmniTekDebug(GENERAL,KERN_ERR,"pExt @ %p NOT found, didn't release devices!\n",pExt);
  }

}

#ifdef MKERNEL
int OmniTek_BAR_dev_ioctl(struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
#else
long OmniTek_BAR_dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) //.unlocked_ioctl 
{
  struct inode *inode = filp->f_dentry->d_inode;
#endif

  struct _OmniTek_dev * dev = filp->private_data;
  POMNITEK_INTERFACE_EXTENSION pExt = dev->pExt;
  int barno = MINOR(inode->i_rdev) - dev->firstMinor;

  int retval = -ENOTTY;
  //OmniTekDebug(FOPS,KERN_INFO,"BAR IOctl got request: 0x%x, with buffer: 0x%lx",cmd,arg);

  if (barno >= pExt->nBars)
  {
    OmniTekDebug(FOPS,KERN_ERR,"BAR IOctl got request for bar number > pExt supported bars!\n");
    return -EINVAL;
  }
  if (barno >= dev->nMinors)    //Probably redundant as dev->nMinors = pExt->nBars
  {
    OmniTekDebug(FOPS,KERN_ERR,"BAR IOctl got request for bar number > device supported bars!\n");  
    return -EINVAL;
  }


 
  switch (cmd)
  {
    case OMNITEK_BAR_IOCQREADREGISTER:
    {
      u32 offset, val;
      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR ReadReg couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {
      
        get_user(offset,&(buffer->Offset));       
        retval = ReadRegValue(pExt,barno,offset,&val);
        put_user(val,&(buffer->Value));
        
      }
      break;
    }
    case OMNITEK_BAR_IOCQWRITEREGISTER:
    {
      u32 offset, val;
      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_READ,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR WriteReg couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {
        get_user(offset,&(buffer->Offset));
        get_user(val,&(buffer->Value));
        retval = WriteRegValue(pExt,barno,offset,val);
      }    

      break;
    } 
    case OMNITEK_BAR_IOCQGETREGISTER:
    {
      u32 offset, val;
      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR GetReg couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {
      
        get_user(offset,&(buffer->Offset));       
        retval = GetRegValue(pExt,barno,offset,&val);
        put_user(val,&(buffer->Value));
        
      }
      break;
    }
    case OMNITEK_BAR_IOCQRAWREADREGISTER:
    {
      u32 offset,val;
      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR Raw Read couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {      
        get_user(offset,&(buffer->Offset));       
        if (offset < (pExt->MemBar[barno].Size >> 2))    //Size/4 because we are 32 bit access and the size is number of 8 bits
        {
          val = ReadHWValue(pExt,barno,offset);
          put_user(val,&(buffer->Value));        
          return 0;
        }
        return -EINVAL;
      }
      break;
    }
    case OMNITEK_BAR_IOCQRAWWRITEREGISTER:
    {
      u32 offset,val;
      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR Raw Write couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {      
        get_user(offset,&(buffer->Offset));       
        if (offset < (pExt->MemBar[barno].Size >> 2)) 
        {
          get_user(val,&(buffer->Value));        
          WriteHWValue(pExt,barno,offset,val);
          return 0;
        }
        return -EINVAL;
      }
    }

    case OMNITEK_BAR_IOCQRAWREADBYTE:
    {
      u32 offset;
      u8  val;
      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR Raw Byte Read couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {      
        get_user(offset,&(buffer->Offset));       
        if (offset < (pExt->MemBar[barno].Size >> 2))    //Size/4 because we are 32 bit access and the size is number of 8 bits
        {
          val = ReadHWValueByte(pExt,barno,offset);
          put_user(val,&(buffer->Value));        
          return 0;
        }
        return -EINVAL;
      }
      break;
    }
    case OMNITEK_BAR_IOCQRAWWRITEBYTE:
    {
      u32 offset;
      u8  val;
      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR Raw Byte Write couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {      
        get_user(offset,&(buffer->Offset));       
        if (offset < (pExt->MemBar[barno].Size >> 2)) 
        {
          get_user(val,&(buffer->Value));        
          WriteHWValueByte(pExt,barno,offset,val);
          return 0;
        }
        return -EINVAL;
      }
    }

    case OMNITEK_BAR_IOCQRAWREADBYTERANGE:
    {
      u32 offset;
      u8  val;
      u32 size;

      char * kern_buffer;  
      void * user_buffer;

      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR Raw Byte Read couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {      
        get_user(offset,&(buffer->Offset));       
        get_user(size,&(buffer->Size));
        get_user(user_buffer,&(buffer->Buffer));

        kern_buffer = kmalloc(size,GFP_KERNEL);

        if ((offset+size) <= (pExt->MemBar[barno].Size))
        {
          //val = ReadHWValueByte(pExt,barno,offset);

//          OmniTekDebug(FOPS,KERN_INFO,"Copying from hardware\n");

#ifndef CONFIG_MACH_NETVIZ
          memcpy_fromio(kern_buffer,(u8*)(pExt->MemBar[barno].pVa)+offset,size);
#else
          memcpy_fromio(kern_buffer,(u8*)(pExt->MemBar[barno].Physical)+offset,size);
#endif        

//          OmniTekDebug(FOPS,KERN_INFO,"Copying to user buffer\n");
          size -= copy_to_user(user_buffer,kern_buffer,size);    //Returns number of bytes that couldn't be copied
          val = size;

          put_user(val,&(buffer->Value));
          kfree(kern_buffer);

          return 0;
        }
        OmniTekDebug(FOPS,KERN_ERR,"Buffer size out of range! Start: %x Length: %x Available size: %x \n",offset,size,pExt->MemBar[barno].Size);
        return -ERANGE;
      }
      break;
    }
    case OMNITEK_BAR_IOCQRAWWRITEBYTERANGE:
    {
      u32 offset;
      u8  val;
      u32 size;

      char * kern_buffer;  
      void * user_buffer;

      OmniTekBARIoctlBuffer * buffer = (OmniTekBARIoctlBuffer *)arg; 
      if (!access_ok(VERIFY_WRITE,arg,sizeof(OmniTekBARIoctlBuffer)))
      {
        OmniTekDebug(FOPS,KERN_ERR,"BAR Raw Byte Write couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {      
        get_user(offset,&(buffer->Offset));       
        get_user(size,&(buffer->Size));
        get_user(user_buffer,&(buffer->Buffer));

        kern_buffer = kmalloc(size,GFP_KERNEL);

        if ((offset+size) <= (pExt->MemBar[barno].Size))
        {
//          OmniTekDebug(FOPS,KERN_INFO,"Copying user buffer\n");
          size -= copy_from_user(kern_buffer,user_buffer,size);    //Returns number of bytes that couldn't be copied

//          OmniTekDebug(FOPS,KERN_INFO,"Copying to hardware\n");

#ifndef CONFIG_MACH_NETVIZ
          memcpy_toio((u8*)(pExt->MemBar[barno].pVa) + offset,kern_buffer,size);
#else
          memcpy_toio((u8*)(pExt->MemBar[barno].Physical)+offset,kern_buffer,size);
#endif        

          val=size;
          put_user(val,&(buffer->Value));        
          kfree(kern_buffer);
  
          return 0;
        }
        OmniTekDebug(FOPS,KERN_ERR,"Buffer size out of range! Start: %x Length: %x Available size: %x \n",offset,size,pExt->MemBar[barno].Size);
        return -ERANGE;
      }
    }




    default:
      retval = -ENOTTY;
      break;
  }
  return retval;
}

int OmniTek_BAR_dev_open(struct inode * inode, struct file * filp)
{
  //Provide direct access to device BARs
  struct _OmniTek_dev * dev;
	OmniTekDebug(FOPS,KERN_INFO,"Opening BAR device\n");  
  dev = container_of(inode->i_cdev,struct _OmniTek_dev,cDev);

  filp->private_data = dev; //Store a copy for other uses...
    
  return 0;
}

int OmniTek_BAR_dev_release(struct inode * inode, struct file * filp)
{
	OmniTekDebug(FOPS,KERN_INFO,"Releasing BAR device\n");
	return 0;


}


