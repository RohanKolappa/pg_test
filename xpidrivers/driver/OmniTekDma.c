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

#include <linux/aio.h>
#include "OmniTekDma.h"
#include "OmniTekDmaOperations.h"
#include "OmniTekRequestQueue.h"
#include "OmniTekDMACore.h"
#include "OmniTekDMARequest.h"
#include "OmniTekResources_sysfs.h"

bool
DmaChannelBusy(
	PResource pChannel
	)
{
	u32 Val;

	ReadRegValue(pChannel->pExt, 
		pChannel->General.Bar, 
		pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
		&Val);

	return (Val & 0x10) == 0;
}

size_t DmaChannelGetBytesAvail(
  PResource pChannel
  )
{  
  return atomic_xchg(&(pChannel->u.DmaChannel.nBytesXferred),0);
}

EXPORT_SYMBOL(DmaChannelGetBytesAvail);


size_t DmaChannelReadBytesAvail(
  PResource pChannel
  )
{
  u32 Val;

  //OmniTekSpinLockCheck(&(pChannel->SpinLock), DMA, KERN_ERR, "Called without channel spinlock!\n");

	ReadRegValue(pChannel->pExt, 
		pChannel->General.Bar, 
		pChannel->General.RegisterOffset + DMA_CHANNEL_BYTES_XFER,
		&Val);

  //Write amount back to decrement counter.
  WriteRegValue(pChannel->pExt,
		pChannel->General.Bar, 
		pChannel->General.RegisterOffset + DMA_CHANNEL_BYTES_XFER,
		Val);


  return Val;
}
EXPORT_SYMBOL(DmaChannelReadBytesAvail);

void DmaChannelWriteBytesAvail(
  PResource pChannel,
  size_t amt
  )
{
	WriteRegValue(pChannel->pExt, 
		pChannel->General.Bar, 
		pChannel->General.RegisterOffset + DMA_CHANNEL_BYTES_XFER,
		amt);

}

EXPORT_SYMBOL(DmaChannelWriteBytesAvail);

static int
DmaChannelInit(
	PResource				pResource,
	u8						  Index
	)
{

	u32 Cap = ReadHWValue(pResource->pExt, 
										pResource->General.Bar, 
										pResource->General.RegisterOffset + DMA_CHANNEL_CSR);

  //char chname[32];

	OmniTekDebug(DMA,KERN_INFO,"channel %d : %p DMA Channel CSR %#x\n", Index, pResource, Cap);
	if(Cap & 0x40000){
		pResource->u.DmaChannel.FDMA_Enabled = true;
		switch((Cap >> 16) & 0x3){
		case 0:
			pResource->u.DmaChannel.FDMA_Write = false;
			pResource->u.DmaChannel.FDMA_Read = true;
			pResource->u.DmaChannel.Label = pResource->pExt->pDmaCtrl->u.DmaCtrl.nFDMARead++;
      snprintf(pResource->u.DmaChannel.Desc,sizeof(pResource->u.DmaChannel.Desc),"FDMA Read Channel %d", pResource->u.DmaChannel.Label);

			break;
		case 1:
			pResource->u.DmaChannel.FDMA_Write = true;
			pResource->u.DmaChannel.FDMA_Read = false;
			pResource->u.DmaChannel.Label = pResource->pExt->pDmaCtrl->u.DmaCtrl.nFDMAWrite++;
			snprintf(pResource->u.DmaChannel.Desc,sizeof(pResource->u.DmaChannel.Desc),"FDMA Write Channel %d", pResource->u.DmaChannel.Label);
      break;
		default:
			pResource->u.DmaChannel.FDMA_Write = true;
			pResource->u.DmaChannel.FDMA_Read = true;
			pResource->u.DmaChannel.Label = pResource->pExt->pDmaCtrl->u.DmaCtrl.nFDMABoth++;
			snprintf(pResource->u.DmaChannel.Desc,sizeof(pResource->u.DmaChannel.Desc),"FDMA Bidir Channel %d", pResource->u.DmaChannel.Label);
			break;
		}
	}
	else{
		pResource->u.DmaChannel.FDMA_Enabled = false;
		pResource->u.DmaChannel.Label = pResource->pExt->pDmaCtrl->u.DmaCtrl.nMDMA++;
		snprintf(pResource->u.DmaChannel.Desc,sizeof(pResource->u.DmaChannel.Desc),"MDMA Channel %d", pResource->u.DmaChannel.Label);

	}

  if (Cap & 0x1)
  {
    OmniTekDebug(DMA,KERN_ERR,"Dma Channel %d reports it is running?\n",Index);
  	WriteRegValue(pResource->pExt, 
		  pResource->General.Bar, 
		  pResource->General.RegisterOffset + DMA_CHANNEL_CSR,
		  (Cap | 0x4) & ~0x1    //Set abort, unset enable
    );

  }

  pResource->pExt->pDmaCtrl->u.DmaCtrl.nChannels++;

  pResource->u.DmaChannel.Running = false;	
  pResource->u.DmaChannel.Paused = false;	
  pResource->u.DmaChannel.FDMA_Throttling = false;
  pResource->u.DmaChannel.DMA_64BIT_PADR = pResource->pExt->pDmaCtrl->u.DmaCtrl.DMA_64BIT_PADR;		//Copy from controller
  pResource->u.DmaChannel.DMA_64BIT_LADR = pResource->pExt->pDmaCtrl->u.DmaCtrl.DMA_64BIT_LADR;  
  pResource->u.DmaChannel.FDMA_AutoStart = true;
  pResource->u.DmaChannel.EFDMA_Polling = false;
  atomic_set(&(pResource->u.DmaChannel.nSgIrqs),0);
  atomic_set(&(pResource->u.DmaChannel.nBytesXferred),0);
  atomic_set(&(pResource->u.DmaChannel.nBytesUnhandled),0);

//TODO: Currently we allocate coherent memory per transaction - need to check whether this is fast enough or if we should allocate for channel on init

#ifdef USE_FIXED_SGL_BUFFER

  pResource->u.DmaChannel.SglBuffer.Size = DMA_SGL_SIZE(MAXTRANSFER);
  pResource->u.DmaChannel.SglBuffer.CommonBuffer = NULL;
  //pResource->u.DmaChannel.SglBuffer.Allocated.Memory = NULL;

  pResource->u.DmaChannel.SglBuffer.CommonBuffer = dma_alloc_coherent(&(pResource->pExt->Device->dev),
                                                                      pResource->u.DmaChannel.SglBuffer.Size,
                                                                      &(pResource->u.DmaChannel.SglBuffer.DMA_Handle),
                                                                      GFP_KERNEL);

/*  pResource->u.DmaChannel.SglBuffer.CommonBuffer = pci_alloc_consistent(pResource->pExt->Device,
                                                                      pResource->u.DmaChannel.SglBuffer.Size,
                                                                      &(pResource->u.DmaChannel.SglBuffer.DMA_Handle));
*/

  if (!pResource->u.DmaChannel.SglBuffer.CommonBuffer)
  {
    OmniTekDebug(DMA,KERN_ERR,"DMA Consistent Buffer Allocate failed\n");
    return -ENOMEM;
  }
  else
  {
    OmniTekDebug(DMA,KERN_INFO,"DMA Consistent Buffer Allocated, size: %08zx, location: %p, DMA Addr: %p\n",
      pResource->u.DmaChannel.SglBuffer.Size,
      pResource->u.DmaChannel.SglBuffer.CommonBuffer,
      (void *)pResource->u.DmaChannel.SglBuffer.DMA_Handle);
  }
  
  // We need one bit per SGL item that can be held within the common buffer
#if 0  
  pResource->u.DmaChannel.SglBuffer.Allocated.Size = pResource->u.DmaChannel.SglBuffer.Size / (8*(SGL_ITEM_SIZE));
  pResource->u.DmaChannel.SglBuffer.Allocated.Free = kmalloc(pResource->u.DmaChannel.SglBuffer.Allocated.Size,GFP_KERNEL);

  if (!pResource->u.DmaChannel.SglBuffer.Allocated.Free)
  {
    OmniTekDebug(DMA,KERN_INFO,"DMA SGL Allocation Buffer Create failed\n");
    return -ENOMEM;
  }
  memset(pResource->u.DmaChannel.SglBuffer.Allocated.Free, 0, pResource->u.DmaChannel.SglBuffer.Allocated.Size);
#else

  pResource->u.DmaChannel.SglBuffer.Allocated.StartPos = pResource->u.DmaChannel.SglBuffer.CommonBuffer;
  pResource->u.DmaChannel.SglBuffer.Allocated.EndPos = pResource->u.DmaChannel.SglBuffer.CommonBuffer;
  pResource->u.DmaChannel.SglBuffer.Allocated.Start_cycle = false;
  pResource->u.DmaChannel.SglBuffer.Allocated.End_cycle = false;

#endif
#endif
  
//  sprintf(qname,"%s queue: Pending",chname);
//  OmniTekRequestQueueInit(&pResource->u.DmaChannel.Pending,qname);
//  sprintf(qname,"%s queue: Active",chname);
//  OmniTekRequestQueueInit(&pResource->u.DmaChannel.Active,qname);

  OmniTekRequestQueueInit(&pResource->u.DmaChannel.Pending,"Pending");
  OmniTekRequestQueueInit(&pResource->u.DmaChannel.Active,"Active");

  pResource->u.DmaChannel.Index = Index;
  pResource->NumRegisters = NUM_REGS_PER_DMA_CHANNEL;

  //Set the work item for ISR
  INIT_WORK(&(pResource->u.DmaChannel.ISRWork),&OmniTekDMAChannelCompleteWork);

  return 0;
}

void
DmaResourceInit(
	Resource *pResource
	)
{
	unsigned int i;
	u32 nChannels;
	u32 Val = ReadHWValue(pResource->pExt, 
										pResource->General.Bar, 
										pResource->General.RegisterOffset + 1);

	nChannels = (Val & 0x1f);

  OmniTekDebug(DMA,KERN_INFO,"Read Value of: %x Have %d DMA Channels\n",Val,nChannels);
  pResource->u.DmaCtrl.nChannels = 0;

  if(pResource->NumRegisters == 0)
    pResource->NumRegisters = (DMA_CHANNEL_OFFSET * (nChannels + 1));

  INIT_LIST_HEAD(&(pResource->u.DmaCtrl.Channels));

	if (Val & 0x100)
		pResource->u.DmaCtrl.DMA_64BIT_PADR = true;
	else
		pResource->u.DmaCtrl.DMA_64BIT_PADR = false;
	if (Val & 0x200)
    pResource->u.DmaCtrl.DMA_64BIT_LADR = true;
	else
		pResource->u.DmaCtrl.DMA_64BIT_LADR = false;

  init_waitqueue_head(&(pResource->u.DmaCtrl.DMA_Wait_Queue));

  omnitek_dma_control_sysfs_create(pResource);

	for(i = 0;i < nChannels;i++){
		PResource pChannel = NULL;

		if((pResource->General.RegisterOffset + ((i + 1) * DMA_CHANNEL_OFFSET) + NUM_REGS_PER_DMA_CHANNEL) < pResource->NumRegisters)
			pChannel = AddResource(pResource->pExt, OMNITEK_RESOURCE_DMA_CHANNEL, pResource->General.Bar, 
						(u16)(pResource->General.RegisterOffset + ((i + 1) * DMA_CHANNEL_OFFSET)));


		if(pChannel != NULL){
      INIT_LIST_HEAD(&(pChannel->u.DmaChannel.Object));
			pChannel->NumRegisters = NUM_REGS_PER_DMA_CHANNEL;
			//WdfCollectionAdd(pResource->u.DmaCtrl.Channels, pChannel->Object);
      list_add_tail(&(pChannel->u.DmaChannel.Object),&(pResource->u.DmaCtrl.Channels));
      OmniTekDebug(DMA,KERN_INFO,": ResourceInit : added channel %p no. %d to channels list\n",pChannel,pChannel->u.DmaChannel.Index);
		}
	}

}

int
DmaInit(
    POMNITEK_INTERFACE_EXTENSION pExt
	)
{
	u32 i = 0,Val;
  PResource pChannel, pChannel_temp;
  OmniTekDebug(DMA,KERN_INFO,"Initialising DMA Hardware for extension %p\n",pExt);
	if(pExt->pDmaCtrl == NULL)
  {
    OmniTekDebug(DMA,KERN_INFO,"Extension %p No DMA Controller Resource - Failed\n",pExt);
		return STATUS_INVALID_PARAMETER_1;
  }

	pExt->pDmaCtrl->u.DmaCtrl.nFDMABoth = 0;
	pExt->pDmaCtrl->u.DmaCtrl.nFDMARead = 0;
	pExt->pDmaCtrl->u.DmaCtrl.nFDMAWrite = 0;
	pExt->pDmaCtrl->u.DmaCtrl.nMDMA = 0;

  //Check if the DMA Controller is responsible for interrupts
  ReadRegValue(pExt,pExt->pDmaCtrl->General.Bar, pExt->pDmaCtrl->General.RegisterOffset + 1,&Val);
  if (Val & 0x400)
  {
    OmniTekDebug(DMA,KERN_INFO,"Interrupts handled by DMA Controller\n");
    pExt->pDmaCtrl->u.DmaCtrl.DmaInterrupts = true;
  }
  else
  {
    pExt->pDmaCtrl->u.DmaCtrl.DmaInterrupts = false;
  }

  pExt->pDmaCtrl->u.DmaCtrl.ChannelStatus = 0;

  pExt->pDmaCtrl->u.DmaCtrl.DMA_Work_Queue = create_workqueue("DMA Controller Work Queue");

  list_for_each_entry_safe(pChannel,pChannel_temp,&(pExt->pDmaCtrl->u.DmaCtrl.Channels),u.DmaChannel.Object)
  {
    OmniTekDebug(DMA,KERN_INFO,"Initialising DMA Channel %d\n",i);
    if (DmaChannelInit(pChannel,i++))
    {
      RemoveResource(pChannel);
      OmniTekDebug(DMA,KERN_ERR,"Weird - DMA Channel failed init\n");
      list_del(&(pChannel->u.DmaChannel.Object));
    }
    else
    {
      OmniTekDebug(DMA,KERN_INFO,"Initialised %s\n",pChannel->u.DmaChannel.Desc);
    }

  }

  
	return 0;
}

void
DmaCtrlRemove(
	PResource pCtrl
	)
{ 
  PResource pChannel;
  int i = 0;
//  list_for_each_entry_safe(pChannel,pChannel_temp,&(pCtrl->u.DmaCtrl.Channels),u.DmaChannel.Object)
    
  i = 0;
  while (!list_empty(&(pCtrl->u.DmaCtrl.Channels)))
  {
    pChannel = list_entry(pCtrl->u.DmaCtrl.Channels.next,struct _Resource,u.DmaChannel.Object);
    OmniTekDebug(DMA,KERN_INFO,"Removing DMA channel from list\n");
    list_del(&(pChannel->u.DmaChannel.Object));
    OmniTekDebug(DMA,KERN_INFO,"Removing DMA channel %d at %p\n",i++,pChannel);

    OmniTekDMAChannelCancel(pChannel);    //Cancel anything left on the channel - do this here as the RemoveResource grabs the lock

    RemoveResource(pChannel);
    OmniTekDebug(DMA,KERN_INFO,"channel removed\n");
  }
  OmniTekDebug(DMA,KERN_INFO,"all channels removed\n");
  destroy_workqueue(pCtrl->u.DmaCtrl.DMA_Work_Queue);
  omnitek_driver_kobject_put(&(pCtrl->u.DmaCtrl.kobject));
  OmniTekDebug(DMA,KERN_INFO,"DMA Controller Work Queue Removed\n");

}

#ifdef USE_FIXED_SGL_BUFFER

static void DmaSglBufferDump(u8 *Base)
{
  //Dump the SGLBuffer out as a big lump of hex:
    
  u32 * base;
  for (base = (u32*)Base; base < ((u32*) (((u8*)Base) + DMA_SGL_SIZE(MAXTRANSFER)/8)); base+=8)
  {
    printk("%p : %08x %08x %08x %08x %08x %08x %08x %08x\n",
      base,base[0],base[1],base[2],base[3],base[4],base[5],base[6],base[7]);
  }
}   

unsigned int
DmaGetSglBufferOffset(
	PResource pChannel,
	unsigned int NumElements
	)
{

  unsigned int  result = 0;
  unsigned long space = 0;
  DmaSglEntry *  NewStartPos = 0;
  DmaSglEntry *  OldStartPos = 0;

  // * Check if space is available in buffer
  // * Move start pointer required amount
  // * return result

  //Calculate remaing space - use the 'cycle' flags to determine whether space between the pointers is used or unused.
  space = ((unsigned long)pChannel->u.DmaChannel.SglBuffer.Allocated.EndPos -
           (unsigned long)pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos) +
           ((pChannel->u.DmaChannel.SglBuffer.Allocated.Start_cycle == pChannel->u.DmaChannel.SglBuffer.Allocated.End_cycle) ? 
                pChannel->u.DmaChannel.SglBuffer.Size : 0);


  OmniTekDebug(DMA_CORE, KERN_INFO, "Allocating %u elements from buffer, available space: %lu\n",
    NumElements, space/sizeof(DmaSglEntry));

  if (space < NumElements * sizeof(DmaSglEntry))
  {
    OmniTekDebug(DMA_CORE, KERN_ERR, "Couldn't allocate enough SG Descriptor buffer for %u elements!\n",NumElements);
    return -1;
  }

  OldStartPos = pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos;
  NewStartPos = pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos + NumElements;
  if ((unsigned long)NewStartPos > 
                                  (unsigned long)pChannel->u.DmaChannel.SglBuffer.CommonBuffer + 
                                  (unsigned long)pChannel->u.DmaChannel.SglBuffer.Size - 
                                  sizeof(DmaSglEntry))
  {
    NewStartPos -= pChannel->u.DmaChannel.SglBuffer.Size/sizeof(DmaSglEntry);
    pChannel->u.DmaChannel.SglBuffer.Allocated.Start_cycle = ~pChannel->u.DmaChannel.SglBuffer.Allocated.Start_cycle;
  }

  pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos = NewStartPos;

  OmniTekDebug(DMA_CORE, KERN_INFO, "Allocated %d elements from buffer: descriptor pos: %p start pos: %p, end pos: %p\n", 
    NumElements,
    OldStartPos,
    pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos,
    pChannel->u.DmaChannel.SglBuffer.Allocated.EndPos);

  //Return the offset in bytes
  result = (unsigned long)OldStartPos - (unsigned long)pChannel->u.DmaChannel.SglBuffer.CommonBuffer;

  //TODO: This should never happen so remove it when it won't...
  if (result & 0x1F)
  {
    OmniTekDebug(DMA_CORE, KERN_ERR, "Offset %08x not 64 bit aligned\n",result);
    return -1;
  }

  return result;


}
void
DmaReleaseSglBuffer(
	struct _OmniTekDmaTransactionContext * pContext
	)
{

  DmaSglEntry *  NewEndPos = 0;
  DmaSglEntry *  OldEndPos = 0;

  OmniTekDebug(DMA_CORE, KERN_INFO, "Releasing %d elements from buffer.  Current end pos: %p\n",
    pContext->Sgl.NumElements,pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.EndPos);

  // * Check we don't underflow
 
  // Logic for this is a little bit fiddly.  Essentially we cannot cross the StartPos
  // So basically if the oldpos is < startpos and newpos > startpos we have a problem.
  // if oldpos < startpos and newpos < startpos we are ok,
  // if oldpos > startpos and newpos < starpos then we have looped but we are ok
  

  OldEndPos = pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.EndPos;
  NewEndPos = pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.EndPos + pContext->Sgl.NumElements;
  if ((unsigned long)NewEndPos > 
                                  (unsigned long)pContext->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + 
                                  (unsigned long)pContext->pChannel->u.DmaChannel.SglBuffer.Size - 
                                  sizeof(DmaSglEntry))
  {
    NewEndPos -= pContext->pChannel->u.DmaChannel.SglBuffer.Size/sizeof(DmaSglEntry);
    pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.End_cycle = ~pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.End_cycle;
  }

  if ((NewEndPos > pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos) && 
      (OldEndPos < pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos))
  {
    OmniTekDebug(DMA_CORE, KERN_ERR, "Underflow releasing SG Descriptor Buffer!\n");
  }

  if (pContext->Sgl.NumElements == 0)
  {
    OmniTekDebug(DMA_CORE, KERN_ERR, "Requested to release 0 elements from SG Descriptor buffer!\n");
  }


  // * Move end pointer required amount

  pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.EndPos = NewEndPos;

  
  OmniTekDebug(DMA_CORE, KERN_INFO, "Released %d elements from buffer: start pos: %p, end pos: %p\n", 
    pContext->Sgl.NumElements,
    pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.StartPos,
    pContext->pChannel->u.DmaChannel.SglBuffer.Allocated.EndPos);

}

#endif

void DmaChannelSetAutoStart(struct _Resource * pChannel, bool set)
{
  pChannel->u.DmaChannel.FDMA_AutoStart = set;  
  OmniTekDebug(DMA,KERN_INFO,"Set channel %d to %s start\n",pChannel->u.DmaChannel.Index, set ? "Auto" : "Manual");
}
EXPORT_SYMBOL(DmaChannelSetAutoStart);


void DmaChannelSetThrottling(struct _Resource * pChannel, bool set)
{
  pChannel->u.DmaChannel.FDMA_Throttling = set;  
  OmniTekDebug(DMA,KERN_INFO,"Set channel %d throttling %s\n",pChannel->u.DmaChannel.Index, set ? "Enabled" : "Disabled");
}
EXPORT_SYMBOL(DmaChannelSetThrottling);


/**************************************************************************************/
/************************** DMA Devices Setup *****************************************/
/**************************************************************************************/


static struct _OmniTek_dev OmniTekMDMADev ;

const struct file_operations OmniTek_MDMA_dev_fops = {
  .owner = THIS_MODULE,
  .read = OmniTek_MDMA_dev_read,
  .write = OmniTek_MDMA_dev_write,
  .ioctl = OmniTek_MDMA_dev_ioctl,
  .open = OmniTek_MDMA_dev_open,
  .release = OmniTek_MDMA_dev_release,
  .aio_read = OmniTek_MDMA_dev_aio_read,
  .aio_write = OmniTek_MDMA_dev_aio_write
};



void
DmaChannelDelete(
	PResource pChannel
	)
{
	//DmaChannelStop(pChannel);		//Don't need to call stop as everything has already been cancelled/disabled at this stage?


#if 0
	if(!list_empty(&(pChannel->u.DmaChannel.WorkList))){
    struct _OmniTekDmaTransactionContext * pContext, *pContext_temp;    
    OmniTekDebug(DMA,KERN_INFO,"Emptying Worklist\n");
    list_for_each_entry_safe(pContext, pContext_temp, &(pChannel->u.DmaChannel.WorkList), Object)
    {
      OmniTekDebug(DMA,KERN_INFO,"Removing Entry\n");
      list_del(&(pContext->Object));

/*      if (OmniTekDMAChannelCancel(pChannel))
      {
        //TODO: We must wait until the requests have been cancelled here...
        OmniTekDebug(DMA,KERN_INFO,"Couldn't delete all requests for channel?\n");
      }
*/      
		DmaTransactionClean(pContext);
		}

		INIT_LIST_HEAD(&(pChannel->u.DmaChannel.WorkList));
	}
#endif

//TODO: Currently coherent buffer is allocated per transaction - possibly this is slower so we may go back to allocating for the channel on init
#ifdef USE_FIXED_SGL_BUFFER

  OmniTekDebug(DMA,KERN_INFO,"Releasing coherent buffer\n");

  if (pChannel->u.DmaChannel.SglBuffer.CommonBuffer)
  {
    dma_free_coherent (&(pChannel->pExt->Device->dev),
                      pChannel->u.DmaChannel.SglBuffer.Size,
                      pChannel->u.DmaChannel.SglBuffer.CommonBuffer,
                      pChannel->u.DmaChannel.SglBuffer.DMA_Handle                   
                      );

    pChannel->u.DmaChannel.SglBuffer.CommonBuffer = NULL;
  }
/*  pci_free_consistent(pChannel->pExt->Device,
                    pChannel->u.DmaChannel.SglBuffer.Size,
                    pChannel->u.DmaChannel.SglBuffer.CommonBuffer,
                    pChannel->u.DmaChannel.SglBuffer.DMA_Handle                   
                    );
*/
  
//	if(pChannel->u.DmaChannel.SglBuffer.Allocated.Memory != NULL)
//		pChannel->u.DmaChannel.SglBuffer.Allocated.Memory = 0;

#endif

  omnitek_driver_kobject_put(&(pChannel->u.DmaChannel.kobject));
  OmniTekDebug(DMA,KERN_INFO,"Channel deleted\n");
}


void
OmniTekDMASetupDev(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
)
{
  OmniTekDriver * omnitek_driver = pExt->pDriver;
  struct _OmniTek_dev * dev = &OmniTekMDMADev;

  //Many of the resources require a corresponding device for IO (e.g. the DMA channels).  These will be created here.
  //For now the BARs will be given devices so that we can read their registers...

  OmniTekDebug(DMA, KERN_INFO, "Setting up DMA devices, have omnitek_driver at %p\n",omnitek_driver);
   
  if (!omnitek_driver)
  {
    OmniTekDebug(DMA, KERN_ERR, "Setting up devices - omnitek_driver is NULL!!!\n");
    return;
  }

  if (!(pExt->pDmaCtrl))
  {
    OmniTekDebug(DMA, KERN_ERR, "Setting up devices - No DMA Controller resource found\n");
    return;
  }

  if (pExt->pDmaCtrl->u.DmaCtrl.nChannels)
  {
    int err;
  	int result = 0;
    int i = 0;
    int nDevs = pExt->pDmaCtrl->u.DmaCtrl.nChannels;

    PResource pChannel;
    
    alloc_chrdev_region(&(dev->dev),0,nDevs,"OmniTek DMA Driver");    

    omnitek_driver->NextMinor += nDevs;
  
    OmniTekDebug(DMA, KERN_INFO, "DMA Major %d\n",MAJOR(dev->dev));
    OmniTekDebug(DMA, KERN_INFO, "DMA allocated minors %d to %d\n",MINOR(dev->dev),MINOR(dev->dev)+nDevs-1);

    if (result)
    {
      OmniTekDebug(DMA, KERN_ERR, "couldn't allocate minor numbers!\n");
      return;
    }
      
  
    //Only need to do this once, we can get the minor number back later so we know which channel we are referring to...
    INIT_LIST_HEAD(&(dev->Object));
    dev->Type = OMNITEK_DEV_MDMA;
    cdev_init(&(dev->cDev),&(OmniTek_MDMA_dev_fops));
    dev->cDev.owner = THIS_MODULE;
    dev->cDev.ops = &(OmniTek_MDMA_dev_fops);
    dev->pExt = pExt;
    dev->firstMinor = MINOR(dev->dev);
    dev->nMinors = nDevs;
  
    //Add DMA Devices:
    err = cdev_add(&(dev->cDev),dev->dev,nDevs);
  	if (err)
  		OmniTekDebug(DMA, KERN_ERR, "Error %d adding cdev",err);
        
    list_add_tail(&(dev->Object),&(omnitek_driver->Devices));

    //Enable DMA interrupts:
    OmniTekDebug(DMA, KERN_INFO, "Enabling DMA interrupts\n");
    InterruptEnable(
      pExt,
	    0x2,
	    true
    );
    

    OmniTekDebug(DMA, KERN_INFO, "Creating sysfs entries\n");
    //Set device number in BAR resource
    list_for_each_entry(pChannel,&(pExt->pDmaCtrl->u.DmaCtrl.Channels),u.DmaChannel.Object)
    {
      pChannel->u.DmaChannel.dev = MKDEV(MAJOR(dev->dev),MINOR(dev->dev) + i++);
      omnitek_dma_channel_sysfs_create(pChannel);
    }    
    dev->cDev.ops = &(OmniTek_MDMA_dev_fops);


    OmniTekDebug(DMA, KERN_INFO, "DMA Device Setup done!\n");

  }



}

void 
OmniTekDMAReleaseDev(
  POMNITEK_INTERFACE_EXTENSION pExt
)
{
  struct _OmniTek_dev * dev,*dev_temp;

  OmniTekDriver * omnitek_driver = GetOmniTekDriver(pExt->Device->driver);
  OmniTekDebug(DMA,KERN_INFO,"DMA Device release for pExt @ %p, driver @ %p\n",pExt,omnitek_driver);

  //Run through all the items in the driver and remove the devices corresponding to this pExt  
  list_for_each_entry_safe(dev,dev_temp,&(omnitek_driver->Devices),Object)
  {
    if ((dev->pExt == pExt) && (dev->Type == OMNITEK_DEV_MDMA))
    {  
      OmniTekDebug(DMA,KERN_INFO,"pExt @ %p found, releasing\n",pExt);
      cdev_del(&(dev->cDev));
      list_del(&(dev->Object));
      OmniTekDebug(DMA,KERN_INFO,"unregistering minors %d to %d\n", dev->firstMinor,dev->firstMinor+dev->nMinors-1);
    	unregister_chrdev_region(dev->dev,dev->nMinors);
      break;
    }
  }
  if (dev->pExt != pExt)
  {
    OmniTekDebug(GENERAL,KERN_ERR,"pExt @ %p NOT found, didn't release DMA devices!\n",pExt);
  }
}

/*! \brief IOCtl call to MDMA Device */
#ifdef MKERNEL
int OmniTek_MDMA_dev_ioctl(
  struct inode * inode, 
  struct file *  filp, 
  unsigned int   cmd, 
  unsigned long  arg)
{
#else
long OmniTek_MDMA_dev_ioctl(
  struct file *  filp, 
  unsigned int   cmd, 
  unsigned long  arg)
{
  struct inode *inode = filp->f_dentry->d_inode;
#endif

  int retval = -ENOTTY;

  OmniTekDebug(DMA, KERN_INFO, "Got MDMA IOCtl\n");



  switch (cmd)
  {

    case OMNITEK_MDMA_READ:
    case OMNITEK_MDMA_WRITE:
    {
            
      OmniTekMDMAIoctlBuffer * buffer = (OmniTekMDMAIoctlBuffer *)arg;
      OmniTekMDMAIoctlBuffer   k_buffer;
	  int toDMA = 0;
	  int cntDMA = 0;
	  int maxDMA = (SG_MAX_SINGLE_ALLOC-1) * PAGE_SIZE;

      if (!arg)
      {
        OmniTekDebug(DMA,KERN_ERR,"MDMA IOCtl No DMA IOCtl Buffer provided!\n");
        retval = -EFAULT;
      }
          

      if (!access_ok(VERIFY_READ,arg,sizeof(OmniTekMDMAIoctlBuffer)))
      {
        OmniTekDebug(DMA,KERN_ERR,"MDMA IOCtl Xfer couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {
        if (copy_from_user(&k_buffer, buffer, sizeof(OmniTekMDMAIoctlBuffer)))
        {
          OmniTekDebug(DMA,KERN_ERR,"MDMA IOCtl Xfer couldn't access buffer!\n");
          retval = -EFAULT;
        }
  

        OmniTekDebug(DMA,KERN_INFO,"Transfer Device address: 0x%08llx, Host address: %p, length: 0x%08zx, direction: %s\n",
                                    k_buffer.f_pos,k_buffer.buf,k_buffer.count,cmd == OMNITEK_MDMA_WRITE ? "Write" : "Read");


#ifdef ARCH_HAS_SG_CHAIN
		 retval = OmniTek_MDMA_dev_transfer(filp,k_buffer.buf,0,k_buffer.count,&k_buffer.f_pos,cmd == OMNITEK_MDMA_WRITE ? true : false,false);
#else
		 // DMA loop with max size allowed in limited SG list
		 toDMA = k_buffer.count;
		 while (toDMA > 0)
		 {
			cntDMA = toDMA > maxDMA ? maxDMA : toDMA;
			retval = OmniTek_MDMA_dev_transfer(filp,k_buffer.buf,0,cntDMA,&k_buffer.f_pos,cmd == OMNITEK_MDMA_WRITE ? true : false,false);
			k_buffer.f_pos += cntDMA;
			k_buffer.buf += cntDMA;
			toDMA -= cntDMA;
		 }
#endif


      }
      break;
    }
    
    case OMNITEK_MDMA_READ_KERNEL:
    case OMNITEK_MDMA_WRITE_KERNEL:
    {

      OmniTekMDMAIoctlBuffer * buffer = (OmniTekMDMAIoctlBuffer *)arg;
      OmniTekMDMAIoctlBuffer   k_buffer;

      if (!arg)
      {
        OmniTekDebug(DMA,KERN_ERR,"MDMA IOCtl No DMA IOCtl Buffer provided!\n");
        retval = -EFAULT;
      }
      if (!access_ok(VERIFY_READ,arg,sizeof(OmniTekMDMAIoctlBuffer)))
      {
        OmniTekDebug(DMA,KERN_ERR,"MDMA IOCtl Xfer couldn't access buffer!\n");
        retval = -EFAULT;
      }
      else
      {
        char * kernel_buffer;
        PResource pChannel = filp->private_data;
        dma_addr_t  dma_mem_addr;

        if (copy_from_user(&k_buffer, buffer, sizeof(OmniTekMDMAIoctlBuffer)))
        {
          OmniTekDebug(DMA,KERN_ERR,"MDMA IOCtl Xfer couldn't access buffer!\n");
          retval = -EFAULT;
          break;
        }
  
        //Allocate a kernel buffer
        //kernel_buffer = kmalloc(k_buffer.count,GFP_KERNEL);
        //kernel_buffer = (char *)__get_free_pages(GFP_KERNEL, get_order(k_buffer.count));
        kernel_buffer = (char *)dma_alloc_coherent(&(pChannel->pExt->Device->dev),
                                                   k_buffer.count,
                                                   &dma_mem_addr,
                                                   GFP_KERNEL          
                                                   );

        if (!kernel_buffer)
        {
          OmniTekDebug(DMA,KERN_ERR,"Couldn't allocate kernel bounce buffer!\n");
          retval = -ENOMEM;
          break;
        }
        else
        { 
          OmniTekDebug(DMA,KERN_INFO,"Kernel Bounce-Buffer Transfer Device address: 0x%08llx, Kernel Address: %p,\
                                      DMA Addr %p, Host address: %p, length: 0x%08zx, direction: %s\n",
                                      k_buffer.f_pos,
                                      kernel_buffer,
                                      (void *)dma_mem_addr,
                                      k_buffer.buf,
                                      k_buffer.count,
                                      cmd == OMNITEK_MDMA_WRITE_KERNEL ? "Write" : "Read");

          retval = OmniTek_MDMA_dev_transfer(filp,kernel_buffer,dma_mem_addr,k_buffer.count,&k_buffer.f_pos,cmd == OMNITEK_MDMA_WRITE_KERNEL ? true : false, true);

          if (retval != k_buffer.count)
          {
            OmniTekDebug(DMA,KERN_ERR,"Kernel bounce buffer transfer failed!\n");
            //kfree(kernel_buffer);
            //free_pages(kernel_buffer, get_order(k_buffer.count));
            dma_free_coherent(&(pChannel->pExt->Device->dev),
                               k_buffer.count,
                               kernel_buffer,
                               dma_mem_addr);
            break;
          }

          //Copy from kernel Buffer to user space
          if (copy_to_user((void *)k_buffer.buf,kernel_buffer,k_buffer.count))
          {
            OmniTekDebug(DMA,KERN_ERR,"MDMA IOCtl Xfer couldn't access buffer!\n");
            //kfree(kernel_buffer);
            //free_pages(kernel_buffer, get_order(k_buffer.count));
            dma_free_coherent(&(pChannel->pExt->Device->dev),
                               k_buffer.count,
                               kernel_buffer,
                               dma_mem_addr);

            retval = -EFAULT;
            break;
          }
          //kfree(kernel_buffer);
          //free_pages(kernel_buffer, get_order(k_buffer.count));
          dma_free_coherent(&(pChannel->pExt->Device->dev),
                             k_buffer.count,
                             kernel_buffer,
                             dma_mem_addr);

        }
      }
      

      break;
    }
    //TODO: We should probably have a stop IOctl somewhere
    
    case OMNITEK_FDMA_START:
    {
      //Called to start an FDMA channel
      PResource pChannel = filp->private_data;
      OmniTekDebug(DMA,KERN_INFO,"Starting channel %d, no of outstanding requests: %d\n",
                    pChannel->u.DmaChannel.Index,OmniTekRequestQueueSize(&(pChannel->u.DmaChannel.Active)));

      retval = DMAChannelStart(pChannel);
      break;
    }
    
    case OMNITEK_FDMA_SET_AUTO:
    {
      //Called to set an FDMA channel auto-start
      PResource pChannel = filp->private_data;

      if (pChannel->u.DmaChannel.Running) 
      {
        OmniTekDebug(DMA,KERN_INFO,"Couldn't set channel automatic start - already running!\n");
        retval = -EBUSY;
      }
      else if (arg)
      {
        DmaChannelSetAutoStart(pChannel,true);
        retval = 0;
      }
      else
      {
        DmaChannelSetAutoStart(pChannel,false);
        retval = 0;
      }
            
      break;
    }
    default:
      retval = -ENOTTY;
      break;
  }

  return retval;
}

/*! \brief Device open call to MDMA Device */
int OmniTek_MDMA_dev_open(struct inode * inode, struct file * filp  )
{

  struct _OmniTek_dev * dev = container_of(inode->i_cdev,struct _OmniTek_dev,cDev);

  //TODO: Should we store the DMA Channel pointer here:  Might be more useful later on, and we can get back to the other data?
  //filp->private_data = dev; //Store a copy for other uses...

  //TODO: We should check the file directions (O_RDONLY etc.) against the channel and stop it from being opened if not valid

  POMNITEK_INTERFACE_EXTENSION pExt = dev->pExt;
  PResource pChannel = NULL;

  int chNum = MINOR(inode->i_rdev) - dev->firstMinor;
  bool found = false;
  
	OmniTekDebug(DMA, KERN_INFO, "Opening DMA device, current: %p\n",current);  

  //Get the relevant channel pointer  
  list_for_each_entry(pChannel,&(pExt->pDmaCtrl->u.DmaCtrl.Channels),u.DmaChannel.Object)
  {
    if (pChannel->u.DmaChannel.Index == chNum) 
    {
      found = true;
      break;
    }
  }

  //Check that we got a valid channel
  if (!found)
  {
    OmniTekDebug(DMA, KERN_ERR, "Couldn't find channel %d in Controller %p channel list\n",chNum,pExt->pDmaCtrl);  
    return -ENOTTY;
  }
  OmniTekDebug(DMA, KERN_INFO, "Opened channel %d\n",chNum);

	OmniTekDebug(DMA,KERN_INFO,"required mask %llx\n",dma_get_required_mask(&(pExt->Device->dev)));
	dma_set_mask(&(pExt->Device->dev), (-1 >> 2));  //TODO: Is this correct?
	OmniTekDebug(DMA,KERN_INFO,"dma mask set to %p\n",pExt->Device->dev.dma_mask);

  filp->private_data = pChannel;
    
  //Enable DMA interrupts:
  InterruptEnable(pExt,2,true);

  return 0;
}

/*! \brief Device release to MDMA Device */
int OmniTek_MDMA_dev_release(struct inode * inode , struct file * filp)
{
  PResource pChannel = filp->private_data;

  OmniTekDebug(DMA, KERN_INFO, "Releasing MDMA device, current: %p\n",current);
  //TODO: There's gotta be some cleanup required here...

  //Check if the channel still has work:


  if (pChannel->u.DmaChannel.FDMA_Enabled)
  {
    if ((!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Pending))) || (!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active))))
    {
      OmniTekDebug(DMA, KERN_ERR, "FDMA Channel still has transactions, cancelling!\n");
      OmniTekDMAChannelCancel(pChannel);    //Cancel anything left on the channel - do this here as the RemoveResource grabs the lock
    }
  }
    
      
      

	return 0;

}

static DECLARE_WAIT_QUEUE_HEAD(dma_wq);

void OmniTek_MDMA_dev_complete(struct _OmniTekDmaTransactionContext * pTransaction, int status)
{

  OmniTekDebug(DMA,KERN_INFO,"Got completion callback for transaction %08x,status: %d\n",pTransaction->index,status);

  //Check for any weirdness...
  if ((pTransaction->state != OMNITEK_TRANSACTION_COMPLETED) && (pTransaction->state != OMNITEK_TRANSACTION_CANCELLED))
  {
    OmniTekDebug(DMA,KERN_ERR,"Got completion callback for transaction that hasn't been completed or cancelled %08x!\n",pTransaction->index);
  }

  if ((status == -ECANCELED) && (pTransaction->state != OMNITEK_TRANSACTION_CANCELLED))
  {
    OmniTekDebug(DMA,KERN_ERR,"Got completion callback for transaction with completed status but transaction isn't cancelled %08x!\n",pTransaction->index);
  }

  //Wake the queue up:
  wake_up(&dma_wq);
  
}

OmniTekTransactionCompleteCb OmniTek_MDMA_Complete_Func = OmniTek_MDMA_dev_complete;

/*! 
  \brief Perform MDMA Transfer 
  \param [in] filp  pointer to file struct for this transfer
  \param [in] buf   pointer to user space buffer for transfer
  \param [in] dma_addr dma address for kernel buffer transfers
  \param [in] count amount of data to transfer (bytes)
  \param [in] f_pos pointer to file position
  \param [in] write set to true if this is a write (to the device)
  \param [in] kernel_buffer set if the provided buffer is in kernel memory, default to false for user space memory
  \return     amount of data transferred or error code
*/
ssize_t OmniTek_MDMA_dev_transfer(struct file * filp, const char __user * buf, dma_addr_t dma_addr, size_t count, loff_t * f_pos, bool write, bool kernel_buffer)
{
//  struct _OmniTek_dev * dev = filp->private_data;
  PResource pChannel = filp->private_data;
  struct _OmniTekDmaTransactionContext * pTransaction = 0;
  int result = 0;  

  if (pChannel->u.DmaChannel.FDMA_Enabled == true)
  {
    if (((pChannel->u.DmaChannel.FDMA_Write != true) && (write == true)) ||
        ((pChannel->u.DmaChannel.FDMA_Read  != true) && (write == false)))
    {
      OmniTekDebug(DMA,KERN_ERR,"Invalid direction for channel!\n");
      return -EINVAL;
    }
  }

  OmniTekDebug(DMA,KERN_INFO,"Creating transaction\n");

  //Create a transaction
  result = OmniTekDMACreateRequest(
    pChannel, 
    buf,
    kernel_buffer ? dma_addr : 0,
    count,
    *f_pos,
    write,  
    kernel_buffer ? OMNITEK_TRANSACTION_SYNC_KERNEL : OMNITEK_TRANSACTION_SYNC_IO,
    NULL,
    &OmniTek_MDMA_Complete_Func,  
    &pTransaction 
  );

  if (result)
  {
    OmniTekDebug(DMA,KERN_ERR,"Couldn't create transaction!\n");
    return result;
  }

  if (!pTransaction)
  {
    OmniTekDebug(DMA,KERN_ERR,"Transaction created but returned pointer NULL!\n");
    return -EINVAL;
  }

  //Transaction is ready to go...start the channel

  if ((pChannel->u.DmaChannel.FDMA_Enabled == false)  || 
      ((pChannel->u.DmaChannel.FDMA_Enabled == true) && (pChannel->u.DmaChannel.FDMA_AutoStart == true)))
  {
    OmniTekDebug(DMA,KERN_INFO,"(Auto) Starting channel %d\n",pChannel->u.DmaChannel.Index);
    result = DMAChannelStart(pChannel);
  }

  if (result == -EBUSY)
  {
    OmniTekDebug(DMA,KERN_INFO,"Channel already started!\n");
  }
  if ((result) && (result != -EBUSY))
  {
    //If some other error occured give up
    OmniTekDebug(DMA,KERN_ERR,"Couldn't start channel %p no. %d, error: %x\n",pChannel,pChannel->u.DmaChannel.Index,result);
    //Cancel the request
    OmniTekDMARequestCancel(pTransaction);
    return result;
  }
  
#define TRANSACTION_WAIT_MSECS 5000
  
  result = 0;
  while (result == 0)
  {
    OmniTekDebug(DMA,KERN_INFO,"Current: %p Sync DMA waiting for transaction: %08x\n",current,pTransaction->index);
  
    result = wait_event_timeout(dma_wq, 
                                ((pTransaction->state == OMNITEK_TRANSACTION_COMPLETED) || (pTransaction->state == OMNITEK_TRANSACTION_CANCELLED)),
                                msecs_to_jiffies(TRANSACTION_WAIT_MSECS)
                               );
    OmniTekDebug(DMA,KERN_INFO,"Sync DMA woke for transaction: %08x, status: %x\n",pTransaction->index,result);
    
    if (result >= 0)
    {
      if ((pTransaction->state != OMNITEK_TRANSACTION_COMPLETED) && (pTransaction->state != OMNITEK_TRANSACTION_CANCELLED))
      {
        OmniTekDebug(DMA,KERN_ERR,"Sync DMA timeout transaction: %08x\n",pTransaction->index);
        //Cancel request  
        OmniTekDMARequestCancel(pTransaction);
        //Will go round loop and wait again...
      }
      else if (pTransaction->state == OMNITEK_TRANSACTION_COMPLETED)
      {
        OmniTekDebug(DMA,KERN_INFO,"Sync DMA transaction: %08x has completed!  Time elapsed: %d ms\n",
          pTransaction->index, TRANSACTION_WAIT_MSECS - jiffies_to_msecs(result));
        OmniTekDMAReleaseRequest(pTransaction);
        return count;
      
      }
      else if (pTransaction->state == OMNITEK_TRANSACTION_CANCELLED)
      {
#if 0
        u32 i = 0, Val;
       //This is to flush the chipscope buffers when debugging....
        while (i++<2048)
        {
          ReadRegValue( pChannel->pExt,
                        pChannel->General.Bar,
                        pChannel->General.RegisterOffset,
                        &Val );

        }
#endif
        OmniTekDebug(DMA,KERN_NOTICE,"Sync DMA transaction: %08x has been cancelled!\n",pTransaction->index);
        OmniTekDMAReleaseRequest(pTransaction);
        return -ECANCELED;
      }
    }
    else
    {
      OmniTekDebug(DMA,KERN_NOTICE,"Sync DMA transaction: %08x woken up with result: %x.  Transaction state:%d\n",pTransaction->index,result,pTransaction->state);
      OmniTekDMARequestCancel(pTransaction);      
      OmniTekDMAReleaseRequest(pTransaction);
      if (result == -EINVAL)
        DMAChannelStop(pChannel);
      return result;      
    }
    OmniTekDebug(DMA,KERN_INFO,"Current: %p Going round loop\n",current);

  }

  //Spooky...  
  OmniTekDebug(DMA,KERN_ERR,"Sync DMA current: %p transaction: %08x fell out of processing! Result: %d\n",current,pTransaction->index,result);
  return -ERESTARTSYS;

}

EXPORT_SYMBOL(OmniTek_MDMA_dev_transfer);


/*! \brief Read from MDMA Device */
ssize_t OmniTek_MDMA_dev_read(struct file * filp, char __user * buf, size_t count, loff_t *f_pos)
{
  OmniTekDebug(DMA,KERN_INFO,"Got DMA Sync Read, current: %p\n",current);
  return OmniTek_MDMA_dev_transfer(filp,buf,0,count,f_pos,false,false);
}

/*! \brief Write to MDMA Device */
ssize_t OmniTek_MDMA_dev_write(struct file * filp, const char __user * buf, size_t count, loff_t *f_pos)
{
  OmniTekDebug(DMA,KERN_INFO,"Got DMA Sync Write, current: %p\n",current);
  return OmniTek_MDMA_dev_transfer(filp,buf,0,count,f_pos,true,false);
}

int OmniTek_DMA_dev_async_cancel(struct kiocb * iocb, struct io_event * event)
{
  //Cancel a request
  struct _OmniTekDmaTransactionContext * pTransaction = (struct _OmniTekDmaTransactionContext * )(unsigned long)iocb->ki_user_data;
  int result = 0;

  OmniTekDebug(DMA,KERN_INFO,"Cancelling transaction %08x\n",pTransaction->index);

  if ((pTransaction->state == OMNITEK_TRANSACTION_COMPLETED) || (pTransaction->state == OMNITEK_TRANSACTION_CANCELLED))
  {
    OmniTekDebug(DMA,KERN_ERR,"Transaction Already %s!\n",pTransaction->state == OMNITEK_TRANSACTION_COMPLETED ? "COMPLETED" : "CANCELLED");
    dump_stack();
    aio_put_req(iocb);  //Try 'putting' it again to get rid of it?
    return -EINVAL;
  }
  

  if (pTransaction->typeInfo.kiocb != iocb)
    OmniTekDebug(DMA,KERN_ERR,"Transaction doesn't match kiocb!!!!\n");

  if (pTransaction->type != OMNITEK_TRANSACTION_ASYNC_IO)
    OmniTekDebug(DMA,KERN_ERR,"Transaction %08x is not an async transaction?\n",pTransaction->index);


  
  //There are two ways this can work:  One is that we cancel and call aio_put_req here, this clears the request from the AIO framework
  //and we return that the cancel succeeded.  When we call requestCancel we must _not_ call aio_complete because the iocb no longer
  //exists.
  //An alternative is to allow the complete function to finish the request, but this means we need to schedule the request in a separate
  //process or we deadlock on a spinlock in the iocb.  This also means the user application should wait on cancelled requests, which isn't
  //ideal (it could probably not, and the framework will get rid of them when the application quits).

  kiocbSetCancelled(iocb);
  result = aio_put_req(iocb); //Think we need to do this to 'finish' the request?
  if (result == 0)
    OmniTekDebug(DMA,KERN_ERR,"aio_put_req returned that we weren't the last user of this request\n");
  else
    OmniTekDebug(DMA,KERN_INFO,"aio_put_req ok\n");

  result = OmniTekDMARequestCancel(pTransaction);
  if (result)
    OmniTekDebug(DMA,KERN_ERR,"Request Cancel returned error: %d\n",result);
  else
    OmniTekDebug(DMA,KERN_INFO,"Request Cancelled\n");

  OmniTekDebug(DMA,KERN_INFO,"Transaction %08x cancelled\n",pTransaction->index);

  return 0;   //If result == 1 then aio_put_req succeeded so we return 0...
}

void OmniTek_DMA_dev_async_complete(struct _OmniTekDmaTransactionContext * pTransaction, int status)
{
  //So we have a completed Transaction:
  //This should be quite a simple procedure...
  //Get the kiocb from the transaction
  struct kiocb * iocb = pTransaction->typeInfo.kiocb;

  OmniTekDebug(DMA,KERN_INFO,"Transaction %08x completing with status %d\n",pTransaction->index,status);

  if (unlikely(pTransaction->type != OMNITEK_TRANSACTION_ASYNC_IO))
    OmniTekDebug(DMA,KERN_ERR,"Transaction %08x is not an async transaction?\n",pTransaction->index);
  

  //Quick sanity check:
  if (unlikely((struct _OmniTekDmaTransactionContext * )(unsigned long)iocb->ki_user_data != pTransaction))
    OmniTekDebug(DMA,KERN_ERR,"Transaction doesn't match kiocb!!!!\n");  

  if (unlikely((pTransaction->state != OMNITEK_TRANSACTION_COMPLETED) && (pTransaction->state != OMNITEK_TRANSACTION_CANCELLED)))
  {
    OmniTekDebug(DMA,KERN_ERR,"Got completion callback for transaction that hasn't been completed or cancelled %08x!\n",pTransaction->index);
  }

  if (likely(pTransaction->state == OMNITEK_TRANSACTION_COMPLETED))
  {
    aio_complete(iocb,status,0);
  }
  else if (pTransaction->state == OMNITEK_TRANSACTION_CANCELLED)
  {
    OmniTekDebug(DMA, KERN_INFO, "Calling aio_complete with status %08x\n",status);
		aio_complete(iocb,-ECANCELED,0);  //Complete the request with cancelled status
  }

  OmniTekDebug(DMA,KERN_INFO,"Transaction %08x completed with status %d\n",pTransaction->index,status);

  //Release the transaction
  OmniTekDMAReleaseRequest(pTransaction);
  //call aio_complete?  If the transaction has been cancelled then we've already called aio_put_req, so presumably this is a bad idea?
}

OmniTekTransactionCompleteCb OmniTek_DMA_Async_Complete_Func = OmniTek_DMA_dev_async_complete;

/*! \brief Asynchronous transfer function */
ssize_t OmniTek_DMA_dev_async_transfer(struct kiocb * iocb, const char * buf, size_t count, loff_t pos, bool write)
{
  //Asynchronous transfer...
  PResource pChannel = iocb->ki_filp->private_data;
  struct _OmniTekDmaTransactionContext * pTransaction = 0;
  int result = 0;  

  if (pChannel->u.DmaChannel.FDMA_Enabled == true)
  {
    if (((pChannel->u.DmaChannel.FDMA_Write != true) && (write == true)) ||
        ((pChannel->u.DmaChannel.FDMA_Read  != true) && (write == false)))
    {
      OmniTekDebug(DMA,KERN_ERR,"Invalid direction for channel!\n");
      return -EINVAL;
    }
  }

  OmniTekDebug(DMA,KERN_INFO,"Creating transaction\n");

  //Create a transaction
  result = OmniTekDMACreateRequest(
    pChannel, 
    buf,
    0, 
    count,
    pos,
    write,  
    OMNITEK_TRANSACTION_ASYNC_IO,
    iocb,
    &OmniTek_DMA_Async_Complete_Func,  
    &pTransaction 
  );

  //Set cancellation callback
  if (unlikely(iocb->ki_cancel))
  	OmniTekDebug(DMA,KERN_NOTICE, "iocb already has a cancel callback\n");
  else
    iocb->ki_cancel = OmniTek_DMA_dev_async_cancel;

  if (unlikely(iocb->ki_user_data))
  	OmniTekDebug(DMA,KERN_NOTICE, "iocb already has user data!\n");
  else
    iocb->ki_user_data = (__u64)(unsigned long)pTransaction;  //Store a pointer to the transaction for cancellation

  if (unlikely(result))
  {
    OmniTekDebug(DMA,KERN_ERR,"Couldn't create transaction!\n");
    return -EAGAIN;
  }

  if (unlikely(!pTransaction))
  {
    OmniTekDebug(DMA,KERN_ERR,"Transaction created but returned pointer NULL!\n");
    return -EINVAL;
  }
  
  //If this channel is set to autostart then we start it here...
  if ((pChannel->u.DmaChannel.FDMA_Enabled == false)  || 
      ((pChannel->u.DmaChannel.FDMA_Enabled == true) && (pChannel->u.DmaChannel.FDMA_AutoStart == true)))
  {
    OmniTekDebug(DMA,KERN_INFO,"(Auto) Starting channel %d\n",pChannel->u.DmaChannel.Index);
    result = DMAChannelStart(pChannel);
  }  

  if (unlikely((result != 0) && (result != -EBUSY)))
    return result;
  else
    return -EIOCBQUEUED;
    
}

//	ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
/*! \brief Asynchronous read*/
ssize_t OmniTek_MDMA_dev_aio_read(struct kiocb * iocb, const struct iovec * vec, unsigned long niov, loff_t pos)
{
  OmniTekDebug(DMA,KERN_INFO,"Got DMA Async Read, current: %p\n",current);

  //TODO: We should probably handle the vectored read/write?  Either we need to add in another layer here that can
  //handle it or add some code to the transaction creation to handle it.
  if (niov != 1)
  {
    OmniTekDebug(DMA,KERN_ERR,"DMA Async Read has multiple part vector transfer, can't process!\n");

    while(niov)
    {
      OmniTekDebug(DMA,KERN_ERR,"Base: %p, size: %zd\n",vec->iov_base,vec->iov_len);
      vec++;      
      niov--;
    }
    return -EINVAL;
  }

  if (is_sync_kiocb(iocb))
  {    //Unusual case that we are supposed to handle...
    OmniTekDebug(DMA,KERN_INFO,"DMA Async Read is actually synchronous...\n");
    return OmniTek_MDMA_dev_transfer(iocb->ki_filp,vec->iov_base,0,vec->iov_len,&pos,false,false);
  }

  return OmniTek_DMA_dev_async_transfer(iocb,vec->iov_base,vec->iov_len,pos,false);
}

//	ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
/*! \brief Asynchronous write*/
ssize_t OmniTek_MDMA_dev_aio_write(struct kiocb * iocb, const struct iovec * vec, unsigned long niov, loff_t pos)
{
  OmniTekDebug(DMA,KERN_INFO,"Got DMA Async Write, current: %p\n",current);

  //Check this iovec malarkey - hopefully we only get the one - if we get more then we can't handle it.
  if (unlikely(niov != 1))
  {
    OmniTekDebug(DMA,KERN_ERR,"DMA Async Write has multiple part vector transfer, can't process!\n");

    while(niov)
    {
      OmniTekDebug(DMA,KERN_ERR,"Base: %p, size: %zd\n",vec->iov_base,vec->iov_len);
      vec++;      
      niov--;
    }
    return -EINVAL;
  }

  if (unlikely(is_sync_kiocb(iocb)))    //Unusual case that we are supposed to handle...
  {
    OmniTekDebug(DMA,KERN_INFO,"DMA Async Write is actually synchronous...\n");
    return OmniTek_MDMA_dev_transfer(iocb->ki_filp,vec->iov_base,0,vec->iov_len,&pos,true,false);
  }

  return OmniTek_DMA_dev_async_transfer(iocb,vec->iov_base,vec->iov_len,pos,true);
}

