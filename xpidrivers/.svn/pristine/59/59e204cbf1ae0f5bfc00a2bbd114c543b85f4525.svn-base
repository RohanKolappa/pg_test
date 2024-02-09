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

#include "OmniTek_linux.h"
#include "OmniTekFops_linux.h"
#include "OmniTekDriver_linux.h"
#include "OmniTekResources_sysfs.h"
#include "OmniTekInterrupt.h"

#include <asm/atomic.h>

static OmniTekDriver omnitek_driver;
MODULE_DEVICE_TABLE(pci, ids);


/*!
 \brief Get the device ID
 \param [in] dev  PCI Device struct to check
 \return Device id or 0 on error.
*/
static u16
OmniTekGetDeviceId(
  struct pci_dev * dev
  )
{
  u32 Val;
  int result = 0;
  result = pci_read_config_dword(dev,PCI_EXP_TYPE_ENDPOINT,&Val);
  if (result)
  {
    OmniTekDebug(GENERAL,KERN_ERR,"ERROR: Couldn't read device ID from PCIE Config Space!\n");
    return 0;
  }
  return (u16)(Val >> 16);
}


/*!
 \brief   Get the number of PCIE lanes enabled for the device
 \details Reads the PCI Configuration space to determine the number of lanes available to the device
 \param [in] dev  PCI Device struct to check
 \return Number of active lanes or zero on error.
*/
static u8
GetNumPciLanes(
  struct pci_dev * dev
  )
{  
  //Presume PCI Device has already been enabled...
	int result = 0;
	u32 Val;
	u8 Count = 0;

	//ReadWriteConfigSpace(DriverObject->DeviceObject , 0, &Val, 0x34, 4);
  result = pci_read_config_dword(dev,PCI_CAPABILITY_LIST,&Val);
  if (result)
  {  //Failure
    OmniTekDebug(GENERAL,KERN_ERR,"ERROR: Couldn't read capability list config DWORD\n");
    return 0;
  }

	Val <<= 8;

	while((Val & 0xff00) != 0){
		u32 Offset = (Val >> 8 ) & 0xff;
		//ReadWriteConfigSpace(DriverObject->DeviceObject , 0, &Val, Offset, 4);
    OmniTekDebug(GENERAL,KERN_INFO,"PCIE Lanes - reading offset %d - 0x%x\n",Offset,Offset);
    result = pci_read_config_dword(dev,Offset,&Val);
    if (result)
    {  //Failure
      OmniTekDebug(GENERAL,KERN_ERR,"ERROR: Couldn't read PCI Config space DWORD at 0x%x\n",Offset);
      return 0;
    }
   

		if((Val & 0xff) == 0x10){
			u32 Lane = 0;
			//ReadWriteConfigSpace(DriverObject->DeviceObject , 0, &Lane, Offset + 0x12, 4);
      Offset += 12;
      OmniTekDebug(GENERAL,KERN_INFO,"PCIE Lanes - reading lane info at offset %d - 0x%x\n",Offset,Offset);
      result = pci_read_config_dword(dev,Offset, &Lane);    //NB: 0x3 since we are doing DWORDs?
      OmniTekDebug(GENERAL,KERN_INFO,"PCIE Lanes - read 0x%x from 0x%x\n",Lane,Offset);
      if (result)
      {  //Failure
        OmniTekDebug(GENERAL,KERN_ERR,"ERROR: Couldn't read lane info from PCI Config space at 0x%x.  Error: 0x%x, result:%d\n",Offset,result,Lane);
        return 0;
      }
			return (u8)((Lane >> 4) & 0x3f);
		}

		if(++Count > 10)
			break;
	}

	return 0;
}

/*!
 \brief Register device extension for an interrupt
 \param [in] pExt Pointer to device extension to register interrupt for
*/
int
OmniTekRegisterIRQ(
    POMNITEK_INTERFACE_EXTENSION pExt
  )
{

  u8	irq = 0;
  int result = 0;
  //Check for MSI interrupt

  result = pci_enable_msi(pExt->Device);  
  if (!result)
  {
    irq = pExt->Device->irq;
    OmniTekDebug(GENERAL,KERN_INFO,"Using MSI Interrupt no. %d\n",irq);
  }
  else
  {
    OmniTekDebug(GENERAL,KERN_INFO,"pci_enable_msi returned %d, Using standard PCI interrupt\n",result);
    irq = pExt->Device->irq;

	  OmniTekDebug(GENERAL,KERN_INFO,"Device IRQ Line is: %d\n",irq);
  }

  #ifdef USE_IRQ_THREAD
  //TODO: Does this support multiple devices?
  result = request_threaded_irq(irq,&OmniTekInterrupt,&OmniTekInterruptHandler,IRQF_SHARED,"OmniTek Device",pExt);
  #else
  result = request_irq(irq,&OmniTekInterrupt,IRQF_SHARED,"OmniTek Device",pExt);
  #endif

  if (result)
  {
		OmniTekDebug(GENERAL,KERN_INFO,"Couldn't request IRQ line - error %d!\n",result);
    return -EINVAL;
  }
	else
		OmniTekDebug(GENERAL,KERN_INFO,"IRQ %d Registered!\n",irq);

  pExt->irq = irq;

  return 0;

}


/*!
 \brief Unregister device extension for an interrupt
 \param [in] pExt Pointer to device extension to unregister interrupt for
*/
void
OmniTekUnRegisterIRQ(
    POMNITEK_INTERFACE_EXTENSION pExt
  )
{
		OmniTekDebug(GENERAL,KERN_INFO,"Freeing IRQ\n");
    if (pExt->irq != -1)
    {
      free_irq(pExt->irq,pExt);
      //TODO: Not sure that this is correct anymore ... we get a 'no irq handler for vector' error.  Am I supposed to do this?  Or do       
      pci_disable_msi(pExt->Device);
    }
};


/*! 
 \brief PCI Device Probe function 
 \param [in] dev  Pointer to pci device that matches our vendor/device IDs
 \param [in] id   vendor/device ID that matches one of the ids we specify
 \return success if we can set up our driver for the device or an error code
*/
int
OmniTekDeviceAdd(
    struct        pci_dev *    dev,
    const  struct pci_device_id * id
    )
{
  POMNITEK_INTERFACE_EXTENSION		pExt;
  int i = 0 ;
  u8  BarIndex = 0;
  int status = 0;

  OmniTekDebug(GENERAL,KERN_INFO,"omnitek_driver struct at %p\n",&omnitek_driver);


  OmniTekDebug(GENERAL,KERN_INFO,"OmniTekEvtDeviceAdd called\n");

  /* ENABLE PCI DEVICE ETC. */

  //Enable the pci device
  status = pci_enable_device(dev);    
  if (status)
  {
    OmniTekDebug(GENERAL,KERN_ERR,"pci_enable device failed with status: %x\n",status);
    return status;
  }


  //Allocate a new Extension for this device
  pExt = kzalloc(sizeof(OMNITEK_INTERFACE_EXTENSION),GFP_KERNEL);  
  OmniTekDebug(GENERAL,KERN_INFO,"allocated pExt at %p\n",pExt);
  INIT_LIST_HEAD(&(pExt->Object));

  list_add_tail(&(pExt->Object),&(omnitek_driver.Extensions));   //Add it to the driver list of devices

	OmniTekExtInit(dev, pExt, &omnitek_driver);                      //Initialise this extension
  OmniTekDebug(GENERAL,KERN_INFO,"OmniTek Device Extension initialised\n");
	OmniTekDebug(GENERAL,KERN_INFO,"PCIe - x%d\n", GetNumPciLanes(dev));


  /* SETUP PCI BARS ETC. */

  OmniTekDebug(GENERAL,KERN_INFO,"Startup - reached Prepare Hardware stage\n");

	// Initialize PCI BAR variables
	for(i = 0; i < 4;i++){
		pExt->MemBar[i].pVa               = NULL;
		pExt->MemBar[i].Physical          = NULL;
		pExt->MemBar[i].Size              = 0;
		pExt->MemBar[i].IsIoMapped        = false;
//		pExt->MemBar[i].pMdl              = NULL;
	}

  status = pci_request_regions(dev,"OmniTek PCIE Driver");
  if (status != 0)
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Request PCI BAR Regions Failed, error: %x\n",status);
  }
  else
  {

  	pci_set_master(dev);
    status = pci_set_dma_mask(dev,DMA_BIT_MASK(64));
    if (status)
    {
      OmniTekDebug(GENERAL,KERN_INFO,"Couldn't set 64 bit DMA mask, attempting 32\n");
      status = pci_set_dma_mask(dev,DMA_BIT_MASK(32));
    }

    if (status)
      OmniTekDebug(GENERAL,KERN_INFO,"Couldn't set DMA mask\n");
    else   
      OmniTekDebug(GENERAL,KERN_INFO,"DMA mask set\n");

        



    for (i = 0; i < 4; i++)
    {
      unsigned long start = 0;
      unsigned long end   = 0;
      unsigned long flags = 0;
      unsigned long len   = 0;
      int status = -EBUSY;
      u32 * bar_addr = 0;
    /*    {
        int result = 0 ;

        result = pci_assign_resource(dev, 0);
        if (result) OmniTekDebug(GENERAL,KERN_INFO,"Experimental bus rescan: assign resource 0 returned %d\n",result);      
        result = pci_assign_resource(dev, 1);
        if (result) OmniTekDebug(GENERAL,KERN_INFO,"Experimental bus rescan: assign resource 0 returned %d\n",result);      
      }
    */    
      start = pci_resource_start(dev,i);
      end   = pci_resource_end(dev,i);
      flags = pci_resource_flags(dev,i);
      len   = pci_resource_len(dev,i);
      
#ifdef CONFIG_MACH_NETVIZ
      if (start == 0x10010000)
      {
        OmniTekDebug(GENERAL,KERN_ERR,"Skipping first (presumed erroneous) PCI Device\n");
        OmniTekExtShutdown(pExt);
        list_del(&(pExt->Object));
        return -EINVAL;
      }
#endif
      OmniTekDebug(GENERAL,KERN_INFO,"Bar %d, start: 0x%lX - end: 0x%lX, size: 0x%lX, flags: 0x%lX\n",i,start,end,end-start,flags);
      if (flags & IORESOURCE_IO)  OmniTekDebug(GENERAL,KERN_INFO,"Bar %d is IO Mapped\n",i);
      if (flags & IORESOURCE_MEM) OmniTekDebug(GENERAL,KERN_INFO,"Bar %d is MEM Mapped\n",i);
      if (flags & IORESOURCE_PREFETCH)  OmniTekDebug(GENERAL,KERN_INFO,"Bar %d is Prefetchable\n",i);
      if (flags & IORESOURCE_READONLY)  OmniTekDebug(GENERAL,KERN_INFO,"Bar %d is Read Only!\n",i);

      bar_addr  = ioremap_nocache(start,len); 
      OmniTekDebug(GENERAL,KERN_INFO,"Remapped BAR %d to %p\n",i,bar_addr);

      if (!bar_addr)
      {
        OmniTekDebug(GENERAL,KERN_ERR,"Request PCI BAR %d failed, got zero address - status: %d\n",i,status);
      }
      else
      {
        pExt->MemBar[BarIndex].Physical = (u32 *)start;
        pExt->MemBar[BarIndex].Size     = len;
        pExt->MemBar[BarIndex].pVa      = bar_addr;

        BarIndex++;
      }

      if (IORESOURCE_MEM_64 & flags)
      {
        OmniTekDebug(GENERAL,KERN_INFO,"PCI BAR is 64 bit, so skipping next BAR\n");
        i++;
      }
    }
  }   
  OmniTekDebug(GENERAL,KERN_INFO,"%d PCI BARs allocated\n",BarIndex); 

	pExt->nBars = BarIndex;
    
  //Create sysfs entry for extension:
  omnitek_extension_sysfs_create(pExt);
  

  //Setup the system devices for the BARs
  OmniTekDeviceSetupDev(pExt);

	// We now have the BARs mapped into memory - fill-in the capability structures
	OmniTekScanHw(pExt);

	// Add DMA Enablers
  DmaInit(pExt);  

  //Make sure interrupts are disabled
  InterruptEnable(pExt,-1,false);

#ifdef CONFIG_MACH_NETVIZ
  //Clear Marvell PCIE Interrupt status if set
  {
    u32 interrupt_status = __raw_readl(0xfee00000 + 0x1820);
    rmb();
    if (interrupt_status & 0x3)
    {
      OmniTekDebug(IRQ,KERN_INFO,"Legacy INTA assert/de-assert set, clearing assert and de-assert!\n");
      __raw_writel(interrupt_status & 0x3,0xfee00000 + 0x1820);
      wmb();
    }
  }
#endif

  /* SETUP INTERRUPT HANDLERS ETC. */  
  status = OmniTekRegisterIRQ(pExt);
  if (status)
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Register IRQ returned %d\n",status);
  }

  OmniTekDMASetupDev(pExt);  

	return 0;
}

/*!
 \brief   Called when a device is removed from the system
 \param [in] dev pointer to pci device struct that is being removed
*/
static void
OmniTekDeviceRemove(
  struct pci_dev * dev
  )
{
	int i = 0;
  bool found = false;
  
  //Get the device extension for this device
  POMNITEK_INTERFACE_EXTENSION pExt;
  OmniTekDebug(GENERAL,KERN_INFO,"Finding extension for device %p, id: %d\n",dev,OmniTekGetDeviceId(dev));

  list_for_each_entry(pExt,&(omnitek_driver.Extensions),Object)
  {
    if (pExt->Device == dev)
    {
      found = true;
      break;
    }
    i++;
  }

  if (!found)
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Don't seem to have any record of the PCI device!!!  Scanned: %d\n",i);
    return;
  }

  //Disable all interrupts before shutting down
  InterruptEnable(pExt,-1,false);

  OmniTekDebug(GENERAL,KERN_INFO,"Releasing irq\n");
  OmniTekUnRegisterIRQ(pExt );

  OmniTekDebug(GENERAL,KERN_INFO,"Releasing DMA devices\n");
  OmniTekDMAReleaseDev(pExt);


	OmniTekDebug(GENERAL,KERN_INFO,"Releasing devices\n");
  OmniTekDeviceReleaseDev(pExt);

  OmniTekDebug(GENERAL,KERN_INFO,"Releasing Extension at %p\n",pExt);
	OmniTekExtShutdown(pExt);

	for(i = 0; i < MAX_NUM_MEM_BARS; i++){
    pci_release_region(dev,i);    
	}

  list_del(&(pExt->Object));

  omnitek_driver_kobject_put(&(pExt->kobject));

  kfree(pExt);
	OmniTekDebug(GENERAL,KERN_INFO,"Device Removal complete\n");

  
}

static int OmniTekGetNumExts(void)
{
  return list_count(&(omnitek_driver.Extensions));
}
EXPORT_SYMBOL(OmniTekGetNumExts);

static struct _OMNITEK_INTERFACE_EXTENSION * OmniTekGetExtension(int extnum)
{
  struct _OMNITEK_INTERFACE_EXTENSION * pExt;
  int count = 0;

  if (extnum > OmniTekGetNumExts())
    return NULL;

  list_for_each_entry(pExt,&(omnitek_driver.Extensions),Object)
  {
    if (count++ == extnum)
      return pExt;
  }

  return NULL;
}
EXPORT_SYMBOL(OmniTekGetExtension);



/*!
 \brief Driver init function
 \details This sets up the initial driver data structures so we can keep track of devices we manage
 \return Sucess or error code
*/
static int __init OmniTekDriver_init(void)
{

  OmniTekDebug(GENERAL,KERN_INFO,"OmniTek Driver Loaded\n");
    
  /*
  omnitek_driver = kmalloc(sizeof(OmniTekDriver),GFP_KERNEL);
  if (omnitek_driver)
  {
    OmniTekDebug(IRQ,KERN_INFO,"Allocated omnitek_driver struct at %p\n",omnitek_driver);
  }
  else
  {
    OmniTekDebug(GENERAL,KERN_ERR,"Startup : couldn't allocate omnitek_driver struct!\n");
    return -ENOMEM;
  }
  */
  omnitek_driver.pci_driver.name = "OmniTek Driver";
  omnitek_driver.pci_driver.id_table = ids;
  omnitek_driver.pci_driver.probe = OmniTekDeviceAdd;
  omnitek_driver.pci_driver.remove = OmniTekDeviceRemove;
  
  INIT_LIST_HEAD(&(omnitek_driver.Extensions));
  INIT_LIST_HEAD(&(omnitek_driver.Devices));            
	omnitek_driver.ResourcePool = kmem_cache_create("OmniTek Driver Resource Pool",sizeof(Resource),0,0,0);
  omnitek_driver.NextMinor = 0;


  allocate_all_attribute_ptrs();

  //Create class
  omnitek_driver.omnitek_class = class_create(THIS_MODULE, "omnitek");

  //Setup sysfs entry for driver:
  omnitek_driver_sysfs_create(&omnitek_driver);

  return pci_register_driver(&(omnitek_driver.pci_driver));    

}
/*!
 \brief Driver exit function
 \details This shuts down and cleans up any device extensions that are being managed by this driver
*/
static void __exit OmniTekDriver_exit(void)
{
  OmniTekDebug(GENERAL,KERN_INFO,"OmniTek Driver UnLoading\n");
  //Check everything is shutdown ok?
  
  
  pci_unregister_driver(&(omnitek_driver.pci_driver));

  if (!list_empty(&(omnitek_driver.Extensions)))
  {
    POMNITEK_INTERFACE_EXTENSION pExt, pExt_temp;
    OmniTekDebug(GENERAL,KERN_ERR,"Exit called, but devices still active?\n");
    list_for_each_entry_safe(pExt,pExt_temp,&(omnitek_driver.Extensions),Object)
    {
      OmniTekDebug(GENERAL,KERN_INFO,"Extension @ %p remains!\n",pExt);  
//      OmniTekDebug(GENERAL,KERN_INFO,"Forcibly removing device!?!\n");
//      OmniTekDeviceRemove(pExt->Device);   //This will remove the pExt from the list and free it
    }

  }

  //kfree(omnitek_driver);
  OmniTekDebug(GENERAL,KERN_INFO,"OmniTek Driver UnLoaded OK\n");

  omnitek_driver_kobject_put(&(omnitek_driver.kobject));
	class_destroy(omnitek_driver.omnitek_class);

  free_all_attribute_ptrs();


}

/*!
 \brief  Get Driver pointer
 \details This returns a pointer to the OmniTek driver data structure given the pci_driver data structure
 \param [in] driver pointer to pci_driver struct
 \return  Pointer to OmniTek Driver data structure
*/
struct _OmniTekDriver *
GetOmniTekDriver(
  struct pci_driver * driver
  )
{
  return container_of(driver,OmniTekDriver,pci_driver);
}


MODULE_LICENSE("GPL");
module_init(OmniTekDriver_init);
module_exit(OmniTekDriver_exit);


