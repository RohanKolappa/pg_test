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
 * documentation and results solely at your own risk.e
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF USE, 
 * LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL
 * DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/

#include "OmniTekResources_sysfs.h"

/************************* DRIVER SYSFS *********************************************/

/*! Attribute structure for driver */
static struct attribute omnitek_driver_attributes[] =
{
  S_ATTR("Description",THIS_MODULE, S_IROTH),
  S_ATTR("nExts",THIS_MODULE, S_IROTH),
  S_ATTR(0,0,0)
};
static struct attribute ** omnitek_driver_attribute_ptrs;

ssize_t omnitek_driver_sysfs_show(struct kobject * kobj, struct attribute * attr, char * buffer)
{
  //Driver sysfs show
  OmniTekDriver * pOmniTek_Driver = container_of(kobj,OmniTekDriver,kobject);


  if (!strcmp(attr->name, omnitek_driver_attributes[0].name))
    sprintf(buffer,"OmniTek Driver Module\n");
  else if (!strcmp(attr->name, omnitek_driver_attributes[1].name))
    sprintf(buffer,"%d\n",list_count(&(pOmniTek_Driver->Extensions)));
  else
    return -EPERM;

  return strlen(buffer);
}

ssize_t omnitek_driver_sysfs_store(struct kobject * kobj, struct attribute * attr, const char * buffer, size_t size)
{
  OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver KObject Attribute store\n");
  return -EPERM;

}

static struct sysfs_ops omnitek_driver_sysfs_ops =
{
  .show = omnitek_driver_sysfs_show,
  .store  = omnitek_driver_sysfs_store
};

static struct kobj_type omnitek_driver_kobj_type =
{
  .release = omnitek_driver_kobject_release,
  .sysfs_ops = &omnitek_driver_sysfs_ops,
};

void omnitek_driver_sysfs_create(struct _OmniTekDriver * pDriver)
{

  OmniTekDebug(SYSFS,KERN_INFO,"Creating Driver Sysfs kobject\n");

  memset(&(pDriver->kobject),0,sizeof(struct kobject));

  //Setup kobject
  if (kobject_init_and_add(
      &(pDriver->kobject),
      &omnitek_driver_kobj_type,
      (void*)0,
      "OmniTek") < 0)
    kobject_put(&(pDriver->kobject));

  OmniTekDebug(SYSFS,KERN_INFO,"Done\n");

}


/************************* EXTENSION SYSFS *********************************************/


/*! Attribute structure for device extension */
static struct attribute omnitek_extension_attributes[] =
{
  S_ATTR("Default",THIS_MODULE, S_IROTH),
  S_ATTR("nResources",THIS_MODULE, S_IROTH),
  S_ATTR("nBARs",THIS_MODULE, S_IROTH),
  S_ATTR("pExt",THIS_MODULE, S_IROTH),
  S_ATTR(0,0,0)
};
static struct attribute ** omnitek_extension_attribute_ptrs;

ssize_t omnitek_extension_sysfs_show(struct kobject * kobj, struct attribute * attr, char * buffer)
{
  //Extension sysfs show

  POMNITEK_INTERFACE_EXTENSION pExt = container_of(kobj,OMNITEK_INTERFACE_EXTENSION,kobject);

  if (!strcmp(attr->name, omnitek_extension_attributes[0].name))
    sprintf(buffer,"OmniTek Device Extension\n");
  else if (!strcmp(attr->name, omnitek_extension_attributes[1].name))
    sprintf(buffer,"%d\n",list_count(&(pExt->Resources)));
  else if (!strcmp(attr->name, omnitek_extension_attributes[2].name))
    sprintf(buffer,"%d\n",pExt->nBars);  
  else if (!strcmp(attr->name, omnitek_extension_attributes[3].name))
    sprintf(buffer,"%p\n",pExt);
  else
    return -EPERM;

  return strlen(buffer);
}

ssize_t omnitek_extension_sysfs_store(struct kobject * kobj, struct attribute * attr, const char * buffer, size_t size)
{
  OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver KObject Attribute store\n");
  return -EPERM;
}

static struct sysfs_ops omnitek_extension_sysfs_ops =
{
  .show = omnitek_extension_sysfs_show,
  .store  = omnitek_extension_sysfs_store
};

static struct kobj_type omnitek_extension_kobj_type =
{
  .release = omnitek_driver_kobject_release,
  .sysfs_ops = &omnitek_extension_sysfs_ops
};

void omnitek_extension_sysfs_create(struct _OMNITEK_INTERFACE_EXTENSION * pExt)
{
  OmniTekDebug(SYSFS,KERN_INFO,"Creating Extension Sysfs kobject @ %p\n",&(pExt->kobject));

  memset(&(pExt->kobject),0,sizeof(struct kobject));


  //Setup kobject
  if (kobject_init_and_add(
      &(pExt->kobject),
      &omnitek_extension_kobj_type,
      &(pExt->pDriver->kobject),
      "%d",
      list_index(&(pExt->pDriver->Extensions),&(pExt->Object))) < 0)
      kobject_put(&(pExt->kobject));

  if (sysfs_create_link(&(pExt->kobject),&(pExt->Device->dev.kobj),"PCI Device"))
  {
    OmniTekDebug(SYSFS,KERN_INFO,"Couldn't create sysfs link pExt to PCI Device\n");
  }
    
}

/************************* BAR SYSFS *********************************************/

/*! Attribute structure for device BAR */
static struct attribute omnitek_BAR_attributes[] =
{
  S_ATTR("Default",THIS_MODULE, S_IROTH),
  S_ATTR("Size", THIS_MODULE, S_IROTH),
  S_ATTR("Virtual",THIS_MODULE, S_IROTH),
  S_ATTR("Physical", THIS_MODULE, S_IROTH),
  S_ATTR(0,0,0)
};
static struct attribute ** omnitek_BAR_attribute_ptrs;

ssize_t omnitek_BAR_sysfs_show(struct kobject * kobj, struct attribute * attr, char * buffer)
{
  //BAR sysfs show

  struct _PCI_BAR_INFO *  Bar = container_of(kobj,struct _PCI_BAR_INFO,kobject);

  if (!strcmp(attr->name, omnitek_BAR_attributes[0].name))       //Default
    sprintf(buffer,"OmniTek Device BAR\n");
  else if (!strcmp(attr->name, omnitek_BAR_attributes[1].name))  //Size
    sprintf(buffer,"%x\n", Bar->Size);
  else if (!strcmp(attr->name, omnitek_BAR_attributes[2].name))  //Virtual
    sprintf(buffer,"%p\n", Bar->pVa);
  else if (!strcmp(attr->name, omnitek_BAR_attributes[3].name))  //Physical
    sprintf(buffer,"%p\n", Bar->Physical);
  else
    return -EPERM;

  return strlen(buffer);
}

ssize_t omnitek_BAR_sysfs_store(struct kobject * kobj, struct attribute * attr, const char * buffer, size_t size)
{
  OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver KObject Attribute store\n");
  return -EPERM;
}

static struct sysfs_ops omnitek_BAR_sysfs_ops =
{
  .show = omnitek_BAR_sysfs_show,
  .store  = omnitek_BAR_sysfs_store
};

static struct kobj_type omnitek_BAR_kobj_type =
{
  .release = omnitek_driver_kobject_release,
  .sysfs_ops = &omnitek_BAR_sysfs_ops
};

void omnitek_BAR_sysfs_create(struct _OMNITEK_INTERFACE_EXTENSION * pExt,int index)
{
  OmniTekDebug(SYSFS,KERN_INFO,"Creating BAR %d Sysfs kobject. Parent pExt->kobject @ %p\n",index, &(pExt->kobject));

  memset(&(pExt->MemBar[index].kobject),0,sizeof(struct kobject));
 
  if (kobject_init_and_add(
      &(pExt->MemBar[index].kobject),
      &omnitek_BAR_kobj_type,
      &(pExt->kobject),
      "BAR%d",
      index) < 0 )
    kobject_put(&(pExt->MemBar[index].kobject));

  OmniTekDebug(SYSFS,KERN_INFO,"Bar object @ %p\n",&(pExt->MemBar[index]));
  OmniTekDebug(SYSFS,KERN_INFO,"Bar kobject @ %p\n",&(pExt->MemBar[index].kobject));
  OmniTekDebug(SYSFS,KERN_INFO,"parent kobject @ %p\n",pExt->MemBar[index].kobject.parent);

  //Create dev entry:
  OmniTekDebug(SYSFS,KERN_INFO,"Creating BAR %d device, devno %d:%d\n",index,MAJOR(pExt->MemBar[index].dev),MINOR(pExt->MemBar[index].dev));
	pExt->MemBar[index].device = device_create(pExt->pDriver->omnitek_class,&(pExt->Device->dev),pExt->MemBar[index].dev, NULL,"OmniTekBAR%d",index);
  if (!pExt->MemBar[index].device)
    OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create device for BAR\n");


}


/************************* RESOURCES SYSFS *********************************************/

/*! Attribute structure for resources */
static struct attribute omnitek_resource_attributes[] = 
{
  S_ATTR("Description", THIS_MODULE, S_IROTH),
  S_ATTR("Version", THIS_MODULE, S_IROTH),
  S_ATTR("NumRegisters", THIS_MODULE, S_IROTH),
  S_ATTR("RegisterOffset", THIS_MODULE, S_IROTH),
  S_ATTR("BarNo", THIS_MODULE, S_IROTH),
  S_ATTR(0,0,0)

};
static struct attribute ** omnitek_resource_attribute_ptrs;

ssize_t omnitek_resource_sysfs_show(struct kobject * kobj, struct attribute * attr, char * buffer)
{
  //Extension sysfs show

  Resource * pResource = container_of(kobj,Resource,kobject);

  if (!strcmp(attr->name, omnitek_resource_attributes[0].name))
  {
    switch (pResource->Type)
    {
	    
	    case OMNITEK_RESOURCE_FPGA        : sprintf(buffer,"Virtex FPGA\n");      break;
	    case OMNITEK_RESOURCE_DMA_CTRL    : sprintf(buffer,"DMA Controller\n");   break;
	    case OMNITEK_RESOURCE_DMA_CHANNEL : sprintf(buffer,"DMA Channel\n");      break;
      default                           : sprintf(buffer,"Unknown Resource\n"); break;
    }    
  }
  else if (!strcmp(attr->name, omnitek_resource_attributes[1].name))
    sprintf(buffer,"%d\n",pResource->CapVersion);
  else if (!strcmp(attr->name, omnitek_resource_attributes[2].name))
    sprintf(buffer,"%d\n",pResource->NumRegisters);
  else if (!strcmp(attr->name, omnitek_resource_attributes[3].name))
    sprintf(buffer,"%d\n",pResource->General.RegisterOffset);
  else if (!strcmp(attr->name, omnitek_resource_attributes[4].name))
    sprintf(buffer,"%d\n",pResource->General.Bar);
  else
    return -EPERM;
  
  return strlen(buffer);
}

ssize_t omnitek_resource_sysfs_store(struct kobject * kobj, struct attribute * attr, const char * buffer, size_t size)
{
  return -EPERM;
}

static struct sysfs_ops omnitek_resource_sysfs_ops =
{
  .show = omnitek_resource_sysfs_show,
  .store  = omnitek_resource_sysfs_store
};

static struct kobj_type omnitek_resource_kobj_type =
{
  .release = omnitek_driver_kobject_release,
  .sysfs_ops = &omnitek_resource_sysfs_ops
};

void omnitek_resource_sysfs_create(struct _Resource * pResource)
{
  OmniTekDebug(SYSFS,KERN_INFO,"Creating Resource Sysfs kobject\n");
  
  memset(&(pResource->kobject),0,sizeof(struct kobject));
  
  //Setup kobject
  if (kobject_init_and_add(
      &(pResource->kobject),
      &omnitek_resource_kobj_type,
      &(pResource->pExt->kobject),
      "RESOURCE%04x:%04x",
      pResource->Type,pResource->Identifier
      ) < 0 )
    kobject_put(&(pResource->kobject));

  //Link resource to BAR                    
  if (sysfs_create_link(&(pResource->kobject),&(pResource->pExt->MemBar[pResource->General.Bar].kobject),"BAR"))
  {
    OmniTekDebug(SYSFS,KERN_INFO,"Couldn't create sysfs link Resource to BAR\n");
  }
   


}


/************************* DMA CONTROL SYSFS *********************************************/

static struct attribute omnitek_dma_control_attributes[] =
{  
  S_ATTR("nMDMA", THIS_MODULE, S_IROTH),
  S_ATTR("nFDMARead", THIS_MODULE, S_IROTH),
  S_ATTR("nFDMAWrite", THIS_MODULE, S_IROTH),
  S_ATTR("nFDMABoth", THIS_MODULE, S_IROTH),
  S_ATTR("nChannels", THIS_MODULE, S_IROTH),
  S_ATTR(0,0,0)
};
static struct attribute ** omnitek_dma_control_attribute_ptrs;

ssize_t omnitek_dma_control_sysfs_show(struct kobject * kobj, struct attribute * attr, char * buffer)
{

  Resource * pResource = container_of(kobj,Resource,u.DmaCtrl.kobject);

  if (!strcmp(attr->name, omnitek_dma_control_attributes[0].name))
    sprintf(buffer,"%d\n",pResource->u.DmaCtrl.nMDMA);
  else if (!strcmp(attr->name, omnitek_dma_control_attributes[1].name))
    sprintf(buffer,"%d\n",pResource->u.DmaCtrl.nFDMARead);
  else if (!strcmp(attr->name, omnitek_dma_control_attributes[2].name))
    sprintf(buffer,"%d\n",pResource->u.DmaCtrl.nFDMAWrite);
  else if (!strcmp(attr->name, omnitek_dma_control_attributes[3].name))
    sprintf(buffer,"%d\n",pResource->u.DmaCtrl.nFDMABoth);
  else if (!strcmp(attr->name, omnitek_dma_control_attributes[4].name))
    sprintf(buffer,"%d\n",pResource->u.DmaCtrl.nChannels);
  else
    return -EPERM;
  
  return strlen(buffer);
}

ssize_t omnitek_dma_control_sysfs_store(struct kobject * kobj, struct attribute * attr, const char * buffer, size_t size)
{
  return -EPERM;
}

static struct sysfs_ops omnitek_dma_control_sysfs_ops =
{
  .show = omnitek_dma_control_sysfs_show,
  .store  = omnitek_dma_control_sysfs_store
};

static struct kobj_type omnitek_dma_control_kobj_type =
{
  .release = omnitek_driver_kobject_release,
  .sysfs_ops = &omnitek_dma_control_sysfs_ops
};

void omnitek_dma_control_sysfs_create(struct _Resource * pResource)
{
  OmniTekDebug(SYSFS,KERN_INFO,"Creating DMA Control Sysfs kobject\n");
  

  memset(&(pResource->u.DmaCtrl.kobject),0,sizeof(struct kobject));


  //Setup kobject
  if (kobject_init_and_add(
      &(pResource->u.DmaCtrl.kobject),
      &omnitek_dma_control_kobj_type,
      &(pResource->kobject),
      "DMA_Controller"
      ) < 0 )
    kobject_put(&(pResource->u.DmaCtrl.kobject));

  //Symlink to pExt  
  if (sysfs_create_link(&(pResource->pExt->kobject),&(pResource->u.DmaCtrl.kobject),"Dma Controller"))
  {
    OmniTekDebug(SYSFS,KERN_INFO,"Couldn't create sysfs link DMA Controller to pExt\n");
  }



}




/************************* DMA CHANNEL DEVICE ATTRIBUTES *********************************************/


static ssize_t dma_channel_dev_attr_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t dma_channel_dev_attr_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

DEVICE_ATTR(board_index, S_IRUGO, dma_channel_dev_attr_show, dma_channel_dev_attr_store);
DEVICE_ATTR(chan_index, S_IRUGO, dma_channel_dev_attr_show, dma_channel_dev_attr_store);
DEVICE_ATTR(fdma_enabled, S_IRUGO, dma_channel_dev_attr_show, dma_channel_dev_attr_store);
DEVICE_ATTR(fdma_read, S_IRUGO, dma_channel_dev_attr_show, dma_channel_dev_attr_store);
DEVICE_ATTR(fdma_write, S_IRUGO, dma_channel_dev_attr_show, dma_channel_dev_attr_store);



static ssize_t dma_channel_dev_attr_show(struct device *dev, struct device_attribute *attr,
                         char *buf)
{
  //so we should be able to get drvdata back?
  struct _Resource * pResource = (struct _Resource *)dev_get_drvdata(dev);

  if (!strcmp(attr->attr.name, dev_attr_board_index.attr.name))
    return snprintf(buf, PAGE_SIZE, "%d\n",  list_index(&(pResource->pExt->pDriver->Extensions),&(pResource->pExt->Object)));    
  else if (!strcmp(attr->attr.name, dev_attr_chan_index.attr.name))
    return snprintf(buf,PAGE_SIZE,"%d\n",pResource->u.DmaChannel.Index);
  else if (!strcmp(attr->attr.name, dev_attr_fdma_enabled.attr.name))
    return snprintf(buf,PAGE_SIZE,"%d\n",pResource->u.DmaChannel.FDMA_Enabled ? 1 : 0);
  else if (!strcmp(attr->attr.name, dev_attr_fdma_read.attr.name))
    return snprintf(buf,PAGE_SIZE,"%d\n",pResource->u.DmaChannel.FDMA_Read ? 1 : 0);
  else if (!strcmp(attr->attr.name, dev_attr_fdma_write.attr.name))
    return snprintf(buf,PAGE_SIZE,"%d\n",pResource->u.DmaChannel.FDMA_Write ? 1 : 0);
  else
    return -EPERM;

}

static ssize_t dma_channel_dev_attr_store(struct device *dev, struct device_attribute *attr,
                          const char *buf, size_t count)
{
  return -EPERM;
}



/************************* DMA CHANNEL SYSFS *********************************************/

static struct device_type omnitek_dma_type = {
  .name = "omnitek_dma",
};

static struct attribute omnitek_dma_channel_attributes[] =
{
  S_ATTR("Index", THIS_MODULE, S_IROTH),
  S_ATTR("FDMA_Enabled", THIS_MODULE, S_IROTH),
  S_ATTR("FDMA_Read", THIS_MODULE, S_IROTH),
  S_ATTR("FDMA_Write", THIS_MODULE, S_IROTH),
  S_ATTR("AutoStart", THIS_MODULE, S_IROTH | S_IWOTH),
  S_ATTR("Running", THIS_MODULE, S_IROTH),
  S_ATTR("Description", THIS_MODULE, S_IROTH),
  S_ATTR("Board_Index", THIS_MODULE, S_IROTH),
  S_ATTR(0,0,0)
};
static struct attribute ** omnitek_dma_channel_attribute_ptrs;

ssize_t omnitek_dma_channel_sysfs_show(struct kobject * kobj, struct attribute * attr, char * buffer)
{
  DmaChannel * pChannel  = container_of(kobj,DmaChannel,kobject);

  if (!strcmp(attr->name, omnitek_dma_channel_attributes[0].name))
    sprintf(buffer,"%d\n",pChannel->Index);
  else if (!strcmp(attr->name, omnitek_dma_channel_attributes[1].name))
    sprintf(buffer,"%d\n",pChannel->FDMA_Enabled ? 1 : 0);
  else if (!strcmp(attr->name, omnitek_dma_channel_attributes[2].name))
    sprintf(buffer,"%d\n",pChannel->FDMA_Read ? 1 : 0);
  else if (!strcmp(attr->name, omnitek_dma_channel_attributes[3].name))
    sprintf(buffer,"%d\n",pChannel->FDMA_Write ? 1 : 0);
  else if (!strcmp(attr->name, omnitek_dma_channel_attributes[4].name))
    sprintf(buffer,"%d\n",pChannel->FDMA_AutoStart ? 1 : 0);
  else if (!strcmp(attr->name, omnitek_dma_channel_attributes[5].name))
    sprintf(buffer,"%d\n",pChannel->Running ? 1 : 0);  
  else if (!strcmp(attr->name, omnitek_dma_channel_attributes[6].name))
    sprintf(buffer,"%s\n",pChannel->Desc);
  else if (!strcmp(attr->name, omnitek_dma_channel_attributes[7].name))
  {
    struct _Resource * pResource = container_of(pChannel,struct _Resource, u.DmaChannel);
    sprintf(buffer,"%d\n",  list_index(&(pResource->pExt->pDriver->Extensions),&(pResource->pExt->Object)));    
  }
  else
    return -EPERM;
  
  return strlen(buffer);

}

ssize_t omnitek_dma_channel_sysfs_store(struct kobject * kobj, struct attribute * attr, const char * buffer, size_t size)
{
  DmaChannel * pChannel  = container_of(kobj,DmaChannel,kobject);
  Resource   * pResource = container_of(pChannel,Resource,u.DmaChannel);
  if (!strcmp(attr->name, omnitek_dma_channel_attributes[4].name))
  {
    bool set = false;
    if ((strncmp(buffer,"0",1)) && (pChannel->Running == false))
    {
      OmniTekDebug(SYSFS, KERN_INFO, "Setting DMA Channel %d to manual start\n",pChannel->Label);
      set = false;
    }
    else
    {
      OmniTekDebug(SYSFS, KERN_INFO, "Setting DMA Channel %d to auto start\n",pChannel->Label);
      set = true;
    }
    DmaChannelSetAutoStart(pResource,set);
    return 0;
  }


  return -EPERM;
}

static struct sysfs_ops omnitek_dma_channel_sysfs_ops =
{
  .show = omnitek_dma_channel_sysfs_show,
  .store  = omnitek_dma_channel_sysfs_store
};

static struct kobj_type omnitek_dma_channel_kobj_type =
{
  .release = omnitek_driver_kobject_release,
  .sysfs_ops = &omnitek_dma_channel_sysfs_ops
};

void omnitek_dma_channel_sysfs_create(struct _Resource * pResource)
{

  char  label[32];

  OmniTekDebug(SYSFS,KERN_INFO,"Creating DMA Channel Sysfs kobject\n");
  
  memset(&(pResource->u.DmaChannel.kobject),0,sizeof(struct kobject));

  //Setup kobject - link back to underlying resource

  if (kobject_init_and_add(
      &(pResource->u.DmaChannel.kobject),
      &omnitek_dma_channel_kobj_type,
      //&(pResource->pExt->pDmaCtrl->u.DmaCtrl.kobject),
      &(pResource->kobject),    
      "DMA_CHANNEL%02d",
      pResource->u.DmaChannel.Index
      ) < 0 )
    kobject_put(&(pResource->u.DmaChannel.kobject));


  //Create symbolic links
  if (pResource->u.DmaChannel.FDMA_Enabled == true)
  {
    if ((pResource->u.DmaChannel.FDMA_Read) && (pResource->u.DmaChannel.FDMA_Write))
      sprintf(label,"FDMA_BIDIR%02d",pResource->u.DmaChannel.Label);
    else if (pResource->u.DmaChannel.FDMA_Read)
      sprintf(label,"FDMA_READ%02d",pResource->u.DmaChannel.Label);
    else
      sprintf(label,"FDMA_WRITE%02d",pResource->u.DmaChannel.Label);
  }
  else
  {
    sprintf(label,"MDMA%02d",pResource->u.DmaChannel.Label);
  }
  //Link to DMA channel in pExt
  if (sysfs_create_link(&(pResource->pExt->kobject),&(pResource->u.DmaChannel.kobject),label))
  {
    OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create sysfs link pExt to DMA Channel\n");
  }

  //Link to DMA channel in DMA Controller
  if (sysfs_create_link(&(pResource->pExt->pDmaCtrl->u.DmaCtrl.kobject),&(pResource->u.DmaChannel.kobject),label))
  {
    OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create sysfs link DMA Controller to Channel\n");
  }

  //Create dev entry:
  OmniTekDebug(SYSFS,KERN_INFO,"Creating DMA Channel %s device, devno %d:%d\n",label,MAJOR(pResource->u.DmaChannel.dev),MINOR(pResource->u.DmaChannel.dev));
	pResource->u.DmaChannel.device = device_create(pResource->pExt->pDriver->omnitek_class, &(pResource->pExt->Device->dev),pResource->u.DmaChannel.dev, pResource,"OmniTek%s",label);
  if (pResource->u.DmaChannel.device)
  {
    pResource->u.DmaChannel.device->type = &omnitek_dma_type;  //TODO: Is there some specific way this is supposed to be set?
    OmniTekDebug(SYSFS,KERN_INFO,"Device created, creating device files\n");

    
    if (device_create_file(pResource->u.DmaChannel.device, &(dev_attr_board_index)))  OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create board_index file\n");
    if (device_create_file(pResource->u.DmaChannel.device, &(dev_attr_chan_index)))   OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create chan_index file\n");
    if (device_create_file(pResource->u.DmaChannel.device, &(dev_attr_fdma_enabled))) OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create fdma_enabled file\n");
    if (device_create_file(pResource->u.DmaChannel.device, &(dev_attr_fdma_read)))    OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create fdma_read file\n");
    if (device_create_file(pResource->u.DmaChannel.device, &(dev_attr_fdma_write)))   OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create fdma_write file\n");

    //Create a link from the dev entry (udev) to the sysfs kobject and vice versa
    
    OmniTekDebug(SYSFS,KERN_INFO,"Files created, creating links\n");
    

    if (sysfs_create_link(&(pResource->u.DmaChannel.device->kobj), &(pResource->u.DmaChannel.kobject), "omnitek_resource"))
    {
      OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create sysfs link DMA Channel Device to DMA Channel Resource\n");
    }

    if (sysfs_create_link(&(pResource->u.DmaChannel.kobject), &(pResource->u.DmaChannel.device->kobj), "dev"))
    {
      OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create sysfs link DMA Channel Resource to DMA Channel Device\n");
    }
  }
  else
  {
    OmniTekDebug(SYSFS,KERN_ERR,"Couldn't create DMA Channel Device\n");
  }

  OmniTekDebug(SYSFS,KERN_INFO,"DMA Channel sysfs setup Done\n");

  
}





/******************************** OTHER FUNCTIONS ******************************/

void omnitek_driver_kobject_release(struct kobject * kobj)
{
  OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver release KObject: %s\n",kobj->name);
  memset(kobj,0,sizeof(struct kobject));  //Clear the kobject struct
}

//Do some cleanup before removing kobjects
void omnitek_driver_kobject_put(struct kobject * kobj)
{

  if (kobj->ktype == &omnitek_dma_channel_kobj_type)
  {
    Resource * pResource = container_of(kobj,Resource,u.DmaChannel.kobject);
    char label[32];


    if (pResource->u.DmaChannel.FDMA_Enabled == true)
    {
      if ((pResource->u.DmaChannel.FDMA_Read) && (pResource->u.DmaChannel.FDMA_Write))
        sprintf(label,"FDMA_BIDIR%02d",pResource->u.DmaChannel.Label);
      else if (pResource->u.DmaChannel.FDMA_Read)
        sprintf(label,"FDMA_READ%02d",pResource->u.DmaChannel.Label);
      else
        sprintf(label,"FDMA_WRITE%02d",pResource->u.DmaChannel.Label);
    }
    else
    {
      sprintf(label,"MDMA%02d",pResource->u.DmaChannel.Label);
    }

    OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver removing DMAChannel-resource syfs link: %s\n",label);

    sysfs_remove_link(&(pResource->u.DmaChannel.kobject),label);
    sysfs_remove_link(&(pResource->u.DmaChannel.device->kobj), "omnitek_resource");
    sysfs_remove_link(&(pResource->u.DmaChannel.kobject), "dev");


    OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver removing DMAChannel Device node\n");
    device_remove_file(pResource->u.DmaChannel.device, &(dev_attr_board_index));
    device_remove_file(pResource->u.DmaChannel.device, &(dev_attr_chan_index));
    device_remove_file(pResource->u.DmaChannel.device, &(dev_attr_fdma_enabled));
    device_remove_file(pResource->u.DmaChannel.device, &(dev_attr_fdma_read));
    device_remove_file(pResource->u.DmaChannel.device, &(dev_attr_fdma_write));

  	device_destroy(pResource->pExt->pDriver->omnitek_class, pResource->u.DmaChannel.dev);

  }
  else if (kobj->ktype == &omnitek_resource_kobj_type)
  {
    Resource * pResource = container_of(kobj,Resource,kobject);    
    OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver removing resource-BAR syfs link\n");
    sysfs_remove_link(&(pResource->kobject),"BAR");
  }
  else if (kobj->ktype == &omnitek_extension_kobj_type)
  {
    POMNITEK_INTERFACE_EXTENSION pExt = container_of(kobj,OMNITEK_INTERFACE_EXTENSION,kobject);
    OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver removing pExt-Driver syfs link\n");
    sysfs_remove_link(&(pExt->kobject),"PCI Device");
  }
  else if (kobj->ktype == &omnitek_BAR_kobj_type)
  {
    struct _PCI_BAR_INFO *  Bar = container_of(kobj,struct _PCI_BAR_INFO,kobject);

    POMNITEK_INTERFACE_EXTENSION pExt = container_of(Bar->kobject.parent,OMNITEK_INTERFACE_EXTENSION,kobject);
    OmniTekDebug(SYSFS,KERN_INFO,"Bar object @ %p\n",Bar);
    OmniTekDebug(SYSFS,KERN_INFO,"Bar kobject @ %p\n",&(Bar->kobject));
    OmniTekDebug(SYSFS,KERN_INFO,"parent kobject @ %p\n",Bar->kobject.parent);

    OmniTekDebug(SYSFS,KERN_INFO,"OmniTek Driver removing BAR %d:%d Device node, pExt @ %p\n",MAJOR(Bar->dev),MINOR(Bar->dev),pExt);
	  device_destroy(pExt->pDriver->omnitek_class,Bar->dev);
  }

  kobject_put(kobj);

}

int allocate_all_attribute_ptrs(void)
{

  allocate_and_init_attribute_ptrs(&omnitek_driver_attribute_ptrs, omnitek_driver_attributes);
	omnitek_driver_kobj_type.default_attrs = omnitek_driver_attribute_ptrs;

  allocate_and_init_attribute_ptrs(&omnitek_extension_attribute_ptrs, omnitek_extension_attributes);
  omnitek_extension_kobj_type.default_attrs = omnitek_extension_attribute_ptrs  ;

  allocate_and_init_attribute_ptrs(&omnitek_BAR_attribute_ptrs, omnitek_BAR_attributes);
  omnitek_BAR_kobj_type.default_attrs = omnitek_BAR_attribute_ptrs;

  allocate_and_init_attribute_ptrs(&omnitek_resource_attribute_ptrs, omnitek_resource_attributes);
  omnitek_resource_kobj_type.default_attrs = omnitek_resource_attribute_ptrs;

  allocate_and_init_attribute_ptrs(&omnitek_dma_control_attribute_ptrs, omnitek_dma_control_attributes);
  omnitek_dma_control_kobj_type.default_attrs = omnitek_dma_control_attribute_ptrs;

  allocate_and_init_attribute_ptrs(&omnitek_dma_channel_attribute_ptrs, omnitek_dma_channel_attributes);
  omnitek_dma_channel_kobj_type.default_attrs = omnitek_dma_channel_attribute_ptrs;

  return 0;

}

int free_all_attribute_ptrs(void)
{
  
  OmniTekDebug(SYSFS,KERN_INFO,"Freeing attribute pointers arrays\n");
  kfree(omnitek_driver_attribute_ptrs);     
  //kfree(omnitek_extension_attribute_ptrs);  
  //kfree(omnitek_BAR_attribute_ptrs);
  //kfree(omnitek_resource_attribute_ptrs);
  //kfree(omnitek_dma_control_attribute_ptrs);
  //kfree(omnitek_dma_channel_attribute_ptrs);
  return 0;
}


int allocate_and_init_attribute_ptrs(struct attribute *** ppAttribute_ptrs, struct attribute * attributes)
{
  //First count the number of attributes in the array
  int i = 0, j=0;
  struct attribute * cur_attr = attributes;

  OmniTekDebug(SYSFS,KERN_INFO,"Creating attribute pointers array\n");

  while (cur_attr->name != 0)
  {
    cur_attr++;
    i++;
  }

  OmniTekDebug(SYSFS,KERN_INFO,"Detected %d attributes\n",i);

  //Allocate suitable space for the attribute pointers:
  *ppAttribute_ptrs = kzalloc(sizeof(struct attribute *) * (i+1),GFP_KERNEL); //Allocate and zero array of attributes

  //Fill out the attribute structure
  cur_attr = attributes;
  for (j=0; j<i; j++)
  {
    if ((attributes+j)->name)
      OmniTekDebug(SYSFS,KERN_INFO,"Adding attribute: %s @ %p\n",(attributes+j)->name, attributes+j);
    else
      OmniTekDebug(SYSFS,KERN_INFO,"Adding end attribute\n");
    
    (*ppAttribute_ptrs)[j] = attributes+j;

		
  }

  OmniTekDebug(SYSFS,KERN_INFO,"Attributes array created\n");

  return i;
}


