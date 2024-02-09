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
#include "../include/OmniTekIoctl_linux.h"
#include "OmniTekResources_sysfs.h"

int resource_count(
		struct _OMNITEK_INTERFACE_EXTENSION *	pExt,
		ResourceType							            Type
  )
{
  struct _Resource * pResource;
  int count = 0;
  list_for_each_entry(pResource,&(pExt->Resources),Object)
  {
    if (pResource->Type == Type) count++;
  }
  return count;
}

struct _Resource *
AddResource(
		struct _OMNITEK_INTERFACE_EXTENSION *	pExt,
		ResourceType							            Type,
		u8  								                  Bar,
		u32 								                  RegOffset

		)
{
	struct _Resource *pResource;
  

	if (pExt->pDriver->ResourcePool == NULL)
    return NULL;

    pResource = kmem_cache_alloc(pExt->pDriver->ResourcePool,GFP_KERNEL); //Allocate resource from pool;
    if (!pResource)
        return 0;
    

	pResource->pExt = pExt;
	pResource->General.Bar = Bar;
	pResource->General.RegisterOffset = RegOffset;
	pResource->General.Initialised = false;
	pResource->NumRegisters = 0;
	pResource->Type = Type;
  
    //Since we can have multiple resources with the same type, we need to be able to index them, so:  
	pResource->Identifier = resource_count(pExt,Type);

    INIT_LIST_HEAD(&(pResource->Object));
  
    OmniTekDebug(RESOURCES,KERN_INFO,"AddResource: Adding resource to list\n");
    list_add_tail(&(pResource->Object),&(pExt->Resources));
  
    omnitek_resource_sysfs_create(pResource);

    return pResource;
}

int
RemoveResource(
	struct _Resource * pResource
	)
{
	//DoLevelTrace(TRACE_LEVEL_INFORMATION, Resources, "%#p (type %#x)\n", pResource, pResource->Type);
    OmniTekDebug(RESOURCES,KERN_INFO,"resource @ %p (type %x)\n", pResource, pResource->Type);

    OmniTekDebug(RESOURCES,KERN_INFO,"Removing resource %p from list\n",pResource);
    list_del(&(pResource->Object));

	switch(pResource->Type){
	case OMNITEK_RESOURCE_FPGA:
    OmniTekDebug(RESOURCES,KERN_INFO,"Removing FPGA resource\n");
		FPGADelete(pResource);
		break;
/* TODO: Add these functions back in
	case OMNITEK_RESOURCE_FLASH_PROGRAMMER:
		FlashRemove(pResource);
		break;
*/
	case OMNITEK_RESOURCE_DMA_CTRL:
    OmniTekDebug(RESOURCES,KERN_INFO,"Removing DMA Control resource\n");
		DmaCtrlRemove(pResource);
		break;
	case OMNITEK_RESOURCE_DMA_CHANNEL:
    OmniTekDebug(RESOURCES,KERN_INFO,"Removing DMA Channel resource\n");
		DmaChannelDelete(pResource);
		break;
	default:
		break;
	}

    omnitek_driver_kobject_put(&(pResource->kobject));

    OmniTekDebug(RESOURCES,KERN_INFO,"Freeing resource from pool\n");
    kmem_cache_free(pResource->pExt->pDriver->ResourcePool,pResource);
    OmniTekDebug(RESOURCES,KERN_INFO,"Resource removed\n");
	return 0;
}

static int
ResourceItemRelease(
	u32 SessionId,
	struct _Resource * pResource
	)
{
	//DoLevelTrace(TRACE_LEVEL_INFORMATION, Resources, "Release type %#x (%#x %#x)\n", pResource->Type, pResource->LockedBy, SessionId);
    OmniTekDebug(RESOURCES,KERN_INFO,"Release type %#x (%#x %#x)\n", pResource->Type, pResource->LockedBy, SessionId);
	if(pResource->LockedBy != 0){
		if(pResource->LockedBy != SessionId)
			return -STATUS_OMNITEK_RESOURCE_LOCKED;
		pResource->LockedBy = 0;
	}
	pResource->ReferenceCount--;

	return 0;
}

int
ReleaseResource(
	struct _OMNITEK_INTERFACE_EXTENSION	*	pExt,
	u32								SessionId,
	struct _Resource *					pResource
	)
{
	int status = -STATUS_OMNITEK_RESOURCE_INVALID;

	//DoLevelTrace(TRACE_LEVEL_INFORMATION, Resources, "ResourceRelease type %#x\n", pResource->Type);
    OmniTekDebug(RESOURCES,KERN_INFO,"ResourceRelease type %#x\n", pResource->Type);

	switch(pResource->Type){
	case OMNITEK_RESOURCE_DMA_CHANNEL:
//		DmaStart(pExt, pResource, false); //TODO: Stop DMA Channel
  default:
		break;
	}

	status = ResourceItemRelease(SessionId, pResource);

	return status;
}

#if 0   //Unused
static struct _Resource *
ResourceFind(
	struct _OMNITEK_INTERFACE_EXTENSION	*pExt,
	ResourceType Type,
	u32 Identifier
	)
{
  struct _Resource * pResource;
  list_for_each_entry(pResource,&(pExt->Resources),Object)
  {
    if((pResource->Type == Type) && (pResource->Identifier == Identifier))
			return pResource;
  }

	return 0;
}

bool
ResourceCheck(
	struct _OMNITEK_INTERFACE_EXTENSION	*pExt,
	struct _Resource * pResource
	)
{
  struct _Resource * check;
  list_for_each_entry(check,&(pExt->Resources),Object)
  {
    if (check == pResource)
      return true;
  }

	return false;
}
#endif

struct _Resource *
DmaChannelFind(
	struct _OMNITEK_INTERFACE_EXTENSION * pExt,
	u32 Identifier
	)
{
	DmaChannel * pChannel = NULL;
    struct _Resource   * pResource;
    bool chan_found = false;
  
	if(pExt->pDmaCtrl == NULL)
		return NULL;

    OmniTekDebug(GENERAL, KERN_INFO, "Finding Dma channel type %s %s number: %d\n",
        (Identifier & DMA_CHANNEL_FDMA) ? "FDMA" : "MDMA",
        (Identifier & DMA_CHANNEL_WRITE) ? "WRITE" : "READ",
        DMA_CHANNEL(Identifier)
    );  

  list_for_each_entry(pChannel,&(pExt->pDmaCtrl->u.DmaCtrl.Channels),Object)
  {
    OmniTekDebug(GENERAL, KERN_INFO, "Channel %p has identifier: %d.  Description: %s\n",pChannel, pChannel->Label, pChannel->Desc);  
    
		if(Identifier & DMA_CHANNEL_FDMA){
			if((pChannel->FDMA_Enabled) &&
				(DMA_CHANNEL(Identifier) == pChannel->Label)){

				if(((Identifier & DMA_CHANNEL_WRITE) != 0) &&
					(pChannel->FDMA_Write == false))
					continue;
				if(((Identifier & DMA_CHANNEL_READ) != 0) &&
					(pChannel->FDMA_Read == false))
					continue;
  
        chan_found = true;
				break;
			}
		}
		else{
			if((DMA_CHANNEL(Identifier) == pChannel->Label) && 
				(pChannel->FDMA_Enabled == false))
        chan_found = true;
				break;
		}
	}

	if(chan_found == false)
  {
    OmniTekDebug(GENERAL, KERN_INFO, "Channel not found...\n");
    return NULL;
  }
  else
  { 
    pResource = container_of(pChannel,Resource,u.DmaChannel);
    OmniTekDebug(GENERAL, KERN_INFO, "Returning DMA Channel %p with pExt %p\n",pResource,pResource->pExt);
    OmniTekDebug(GENERAL, KERN_INFO, "sizeof Resource is: %d\n",(unsigned int)sizeof(struct _Resource));
    return pResource;
  }

}

EXPORT_SYMBOL(DmaChannelFind);

int
DmaChannelSetEventFunc
(
    struct _Resource * pResource,
    void (*event_func)(struct _Resource * pResource, void * arg),
    void * arg
  )
{
  if (pResource->Type != OMNITEK_RESOURCE_DMA_CHANNEL)
    return -EINVAL;

  pResource->u.DmaChannel.event_func = event_func;
  pResource->u.DmaChannel.event_priv = arg;


  if (event_func)
  {
    //Enable event interrupts for this channel...
    DMAChanEventInterruptEnable(pResource,true,pResource->u.DmaChannel.FDMA_Write); 
  }
  else
  {
    //Disable event interrupts for this channel...
    DMAChanEventInterruptEnable(pResource,false,pResource->u.DmaChannel.FDMA_Write);
  }
  return 0;
}

EXPORT_SYMBOL(DmaChannelSetEventFunc);

#ifdef USE_POLLED_EFDMA
int
DmaChannelSetPollFunc
(
    struct _Resource * pResource,
    void (*poll_func)(struct _Resource * pResource, void * arg),
    void * arg
  )
{
  if (pResource->Type != OMNITEK_RESOURCE_DMA_CHANNEL)
    return -EINVAL;

  pResource->u.DmaChannel.poll_func = poll_func;
  pResource->u.DmaChannel.poll_priv = arg;

  if (poll_func)
    pResource->u.DmaChannel.EFDMA_Polling = true;
  else
    pResource->u.DmaChannel.EFDMA_Polling = false;


#if 0
  //If we are polling then we need to ensure that the event interrupts are enabled when we are ready for them!
  if (poll_func)
  {
    //Enable event interrupts for this channel...
    DMAChanEventInterruptEnable(pResource,true,pResource->u.DmaChannel.FDMA_Write); 
  }
  else
  {
    //Disable event interrupts for this channel...
    DMAChanEventInterruptEnable(pResource,false,pResource->u.DmaChannel.FDMA_Write);
  }
#endif

  return 0;
}

EXPORT_SYMBOL(DmaChannelSetPollFunc);

#endif

