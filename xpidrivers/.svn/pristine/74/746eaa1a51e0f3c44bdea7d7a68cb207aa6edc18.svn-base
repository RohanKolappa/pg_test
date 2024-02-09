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
#include "OmniTekDMACore.h"
#include "OmniTekDmaOperations.h"

#include <linux/pagemap.h>
#include <linux/dma-mapping.h>
#include <asm/dma-mapping.h>

static void getNumPages(
  unsigned long    buffer,
  size_t           size,
  off_t *          offset,
  unsigned long *  num_pages,
  unsigned long *  first,
  unsigned long *  last
)
{
	*first = PFN_DOWN((unsigned long)buffer)        << PAGE_SHIFT;
	*last  = PFN_DOWN((unsigned long)buffer+size-1) << PAGE_SHIFT;
	*offset   = buffer & ~PAGE_MASK;
	*num_pages = (*last >> PAGE_SHIFT)-(*first >> PAGE_SHIFT)+1;
  
  OmniTekDebug(DMA_CORE,KERN_INFO,"GetNumPages for buffer @%lx, size:%d : first : 0x%lx, last : 0x%lx, numpages : %ld, offset : 0x%lx\n",
                buffer,(int)size,*first,*last,*num_pages,(unsigned long)*offset);

}

static void free_kernel_pages(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{ 
	kfree(pTransaction->DMACoreInfo.pages);
  pTransaction->DMACoreInfo.pages = NULL;
}

static void free_user_pages(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{
  int i = 0;
	OmniTekDebug(DMA_CORE,KERN_INFO,"freeing %ld user pages\n",pTransaction->DMACoreInfo.num_pages);  

	if (pTransaction->DMACoreInfo.pages != NULL)
	{
		for (i = 0; i < pTransaction->DMACoreInfo.num_pages; i++)
		{
			OmniTekDebug(DMA_PAGES,KERN_INFO,"setting page %d @ 0x%p dirty\n",i,pTransaction->DMACoreInfo.pages[i]);
			if (pTransaction->DMACoreInfo.pages[i] != NULL)
			{
				if (!PageReserved(pTransaction->DMACoreInfo.pages[i]))
        {

        	OmniTekDebug(DMA_PAGES,KERN_INFO,"page not reserved, setting dirty\n");
				//	SetPageDirty(pTransaction->DMACoreInfo.pages[i]);
					set_page_dirty(pTransaction->DMACoreInfo.pages[i]);
        }
      	OmniTekDebug(DMA_PAGES,KERN_INFO,"Releasing from page cache\n");

//				OmniTekDebug(DMA_CORE,KERN_INFO,"freeing user page\n");
				page_cache_release(pTransaction->DMACoreInfo.pages[i]);
			}
			else
			{
				OmniTekDebug(DMA_CORE,KERN_INFO,"pages entry %d is null!\n",i);
			}

		}
	}
	OmniTekDebug(DMA_CORE,KERN_INFO,"Freeing pages @ %p\n",&pTransaction->DMACoreInfo.pages);
	kfree(pTransaction->DMACoreInfo.pages);
  pTransaction->DMACoreInfo.pages = NULL;

}


static int DMAMapSg(
  //struct device   * dev,
  struct pci_dev  * dev,
  struct sg_table * sgt,
  struct page **    pages,
  size_t            size,
  int               num_pages,
  off_t             pageOffset,
  bool              write
)
{
  size_t len_mapped;
  int mapped_nents = 0;  

	OmniTekDebug(DMA_CORE,KERN_INFO,"mapping SG pages\n");
	len_mapped = dma_map_sg_pages(
			&(sgt->sgl),
			pages,
			size,
			num_pages,
			pageOffset
		     );	
	//We should see len_mapped == size
	if (len_mapped == size)
  {
		OmniTekDebug(DMA_CORE,KERN_INFO,"size of mapped scatterlist is same as user buffer, looking good!\n");
  }
	else
  {
		OmniTekDebug(DMA_CORE,KERN_ERR,"size of mapped scatterlist (%zd) is _NOT_ same as user buffer (%zd), looking bad!\n",
                        len_mapped,size);	
    return -EINVAL;
  }
	
	//Call dma_map_sg 
	OmniTekDebug(DMA_CORE,KERN_INFO,"calling dma_map_sg with dev: %p , sg: %p, nents: %d\n",
                     dev,sgt->sgl,num_pages);
//#ifdef CONFIG_MACH_NETVIZ
  //Can't map scatter-gather pages when using coherent buffers?  Something different on this platform.
  //Dump out some info about the SGT, hopefully might help
  
  {  
    int i = sgt->nents;
    OmniTekDebug(DMA_CORE, KERN_INFO, "SG_TABLE Debug: sgl @ %p, nents %d, orig_nents %d\n",sgt->sgl, sgt->nents, sgt->orig_nents);
  
    while (i--)
    {
      OmniTekDebug(DMA_CORE, KERN_INFO, "SGL Debug: entry %d, page_link %p, offset %d, dma_addr %p, length %d\n",i,(void*)sgt->sgl[i].page_link,sgt->sgl[i].offset,(void*)(sgt->sgl[i].dma_address),sgt->sgl[i].length);
    }
  }
//#endif
	

	mapped_nents = dma_map_sg(&(dev->dev),sgt->sgl,num_pages,write ? DMA_TO_DEVICE : DMA_FROM_DEVICE);	
  //mapped_nents = pci_map_sg(dev,sgt->sgl,num_pages,write ? DMA_TO_DEVICE : DMA_FROM_DEVICE);	

	OmniTekDebug(DMA_CORE,KERN_INFO,"dma_map_sg mapped %d entries\n",mapped_nents);
  return mapped_nents;

}

int DMAGetUserPages(
  struct _OmniTekDmaTransactionContext * pTransaction

)
{

	int gotPages=dma_get_user_pages(pTransaction->DMACoreInfo.first_page,
                                  pTransaction->DMACoreInfo.num_pages,
                                  pTransaction->DMACoreInfo.pages,NULL);

  if (gotPages < 0)
  {
		OmniTekDebug(DMA_CORE,KERN_ERR,"ERROR: get_user_pages returned: %d\n",gotPages);
    return gotPages;
	}

	if (gotPages != pTransaction->DMACoreInfo.num_pages)
	{
		OmniTekDebug(DMA_CORE,KERN_ERR,"ERROR: got %d user pages, expected %ld\n",gotPages,pTransaction->DMACoreInfo.num_pages);
		free_user_pages(pTransaction);
    return -EINVAL;
	}
	OmniTekDebug(DMA_CORE,KERN_INFO,"got %d user pages\n",gotPages);
  return 0;
}

int DMAGetKernelPages(
  struct _OmniTekDmaTransactionContext * pTransaction

)
{
  //Need to fill out the pages structure somehow...

  //Kernel memory is contiguous, so we should be able to allocate the pages structure quite easily:
  //Don't think we need this for the kernel version...
  return 0;
}


int DMAMapTable(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{
  int result = 0;
  // * Map SG Entries - init_table_and_chain() and map_sg()
	OmniTekDebug(DMA_CORE,KERN_INFO,"Calling sg_alloc_table for %lu pages on sgt @ %p\n",
    pTransaction->DMACoreInfo.num_pages,
    &(pTransaction->DMACoreInfo.sgt));

#ifndef ARCH_HAS_SG_CHAIN
  if (pTransaction->DMACoreInfo.num_pages > SG_MAX_SINGLE_ALLOC)
  {
    //This doesn't really do much except prevent a bug in sg_alloc_table.
    //if we want to support large (> 1MB) FDMA slices we'll have to do some manual
    //SG table chaining for the ARM - or hope someone provides a patch...
    //TODO: Fix this if necessary
    OmniTekDebug(DMA_CORE, KERN_ERR, "Buffer size too large - can't allocate sg table\n");
    return -EINVAL;
  }
#endif

  //TODO: Changed to atomic for ethernet test, can be changed back later
  result = sg_alloc_table(&(pTransaction->DMACoreInfo.sgt),
                          pTransaction->DMACoreInfo.num_pages,
                          GFP_ATOMIC);

	if (result)
	{
		OmniTekDebug(DMA_CORE,KERN_ERR,"sg_alloc_table didn't go very well error %d, cleaning up.\n",result);
		sg_free_table(&(pTransaction->DMACoreInfo.sgt));

    if (OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
      ; //free_kernel_pages(pTransaction);
    else
      free_user_pages(pTransaction);


    return -EINVAL;
	}
	OmniTekDebug(DMA_CORE,KERN_INFO,"sg table allocated.\n");
  

	OmniTekDebug(DMA_CORE,KERN_INFO,"Mapping sg table\n");

  if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
  {
    result = DMAMapSg(
                 pTransaction->pChannel->pExt->Device,
                 &(pTransaction->DMACoreInfo.sgt),
                 pTransaction->DMACoreInfo.pages,
                 pTransaction->Xfer.Size,
                 pTransaction->DMACoreInfo.num_pages,            
                 pTransaction->DMACoreInfo.offset,
                 pTransaction->Xfer.Write
              );
  }      

	if (result<0)
	{
		OmniTekDebug(DMA_CORE,KERN_ERR,"dma_map_sg didn't go very well error %d, cleaning up.\n",result);
		sg_free_table(&(pTransaction->DMACoreInfo.sgt));
    if (OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
      ; //free_kernel_pages(pTransaction);
    else
      free_user_pages(pTransaction);
    return -EINVAL;
	}
	OmniTekDebug(DMA_CORE,KERN_INFO,"sg table mapped for transaction %08x.\n",pTransaction->index);
  return 0;
}

void DMAUnMapSg(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{
  OmniTekDebug(DMA_CORE, KERN_INFO, "Unmapping SG Table for device %p, sgl @ %p, num pages: %lu, direction: %d\n",
    pTransaction->pChannel->pExt->Device,
    pTransaction->DMACoreInfo.sgt.sgl,
    pTransaction->DMACoreInfo.num_pages,
    pTransaction->Xfer.Write ? DMA_TO_DEVICE : DMA_FROM_DEVICE);	

  dma_unmap_sg(
    &(pTransaction->pChannel->pExt->Device->dev),
    pTransaction->DMACoreInfo.sgt.sgl,
    pTransaction->DMACoreInfo.num_pages,
    pTransaction->Xfer.Write ? DMA_TO_DEVICE : DMA_FROM_DEVICE);	

/*
  pci_unmap_sg(
    pTransaction->pChannel->pExt->Device,
    pTransaction->DMACoreInfo.sgt.sgl,
    pTransaction->DMACoreInfo.num_pages,
    pTransaction->Xfer.Write ? DMA_TO_DEVICE : DMA_FROM_DEVICE);	
*/
}

int DMAProgramSgl(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{ 
  int i = 0;

  bool wrap_desc = false;
  bool wrap_buff = false;

#ifdef USE_FIXED_SGL_BUFFER
  u32 * pBuffer = (u32*)pTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + (pTransaction->Sgl.Offset/sizeof(u32))  ;
  dma_addr_t physAddrNext = pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + 
                            pTransaction->Sgl.Offset/sizeof(u8) + 
                            sizeof(DmaSglEntry);
#else
  u32 * pBuffer = pTransaction->Sgl.CoherentMem;
  dma_addr_t physAddrNext = pTransaction->Sgl.DmaMem + SGL_ITEM_SIZE;
#endif
  u32   localAddr = cpu_to_le32(pTransaction->Xfer.LocalAddr);

  u32 transferred = 0;  


  u32 CtrlWord = ((pTransaction->Xfer.Write == false) ? DMA_DPR_BIT_DIRECTION_TO_PC : 0);
	struct scatterlist * cur_entry;

  //Program the entries:
	OmniTekDebug(DMA_CORE,KERN_INFO,"Programming SGL entries - transaction %08x\n",pTransaction->index);


  OmniTekDebug(DMA_SGL,KERN_INFO,"SGL TABLE DUMP - transaction %08x\n",pTransaction->index);
  OmniTekDebug(DMA_SGL,KERN_INFO,"pBuffer\t\t  PADR\t\t\t  LADR\t\t\t  DPR\t\t\t SIZE\n");

  if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
  {
	  for_each_sg(pTransaction->DMACoreInfo.sgt.sgl,cur_entry,pTransaction->DMACoreInfo.sgt.nents,i)
	  {


      //If our buffer pointer has exceeded the size of the commonbuffer then we need to wrap it back to the start...
      
      if (pBuffer == (u32*)pTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + 
                     pTransaction->pChannel->u.DmaChannel.SglBuffer.Size/sizeof(u32))
      {
        //pBuffer -= pTransaction->pChannel->u.DmaChannel.SglBuffer.Size / sizeof(u32);
        OmniTekDebug(DMA_SGL,KERN_INFO,"pBuffer reached %p Wrapping Descriptor pointer\n",pBuffer);
        pBuffer = (u32*)pTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer;
        OmniTekDebug(DMA_SGL,KERN_INFO,"pBuffer now %p\n",pBuffer);
        wrap_buff = true;
      
      }

      if (physAddrNext == pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + pTransaction->pChannel->u.DmaChannel.SglBuffer.Size )
      {
        //physAddrNext -= pTransaction->pChannel->u.DmaChannel.SglBuffer.Size;
        OmniTekDebug(DMA_CORE,KERN_INFO,"Physaddr reached %p Wrapping DPR pointer\n",(void*)physAddrNext);
        physAddrNext = pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle;
        OmniTekDebug(DMA_CORE,KERN_INFO,"Physaddr now %p\n",(void*)physAddrNext);
        wrap_desc = true;

      }


      if (pTransaction->pChannel->u.DmaChannel.FDMA_Enabled == true)
      {
  //Not supporting this (video frame sync) for now, we don't need it...
         if (i == 0)        
          pBuffer[DMA_CHANNEL_LADR] = cpu_to_le32(0x11111111);
        else if (i == pTransaction->DMACoreInfo.sgt.nents -1)
          pBuffer[DMA_CHANNEL_LADR] = cpu_to_le32(0x22222222);
        else
          pBuffer[DMA_CHANNEL_LADR] = cpu_to_le32(0x00000000);
      }
      else
      {
        pBuffer[DMA_CHANNEL_LADR] = cpu_to_le32(localAddr);
      }

		  if(i == pTransaction->DMACoreInfo.sgt.nents - 1)		
      {
			  CtrlWord |= DMA_DPR_BIT_END_OF_CHAIN | DMA_DPR_BIT_INTERRUPT;		// Last entry - mark as last and interrupt
      }

      pBuffer[DMA_CHANNEL_LADR_HIGH] = 0;

      pBuffer[DMA_CHANNEL_PADR] = cpu_to_le32(sg_dma_address(cur_entry) & 0xFFFFFFFF);
      pBuffer[DMA_CHANNEL_PADR_HIGH] = cpu_to_le32((u64)sg_dma_address(cur_entry) >> 32);

      pBuffer[DMA_CHANNEL_SIZE] = cpu_to_le32(sg_dma_len(cur_entry));
      pBuffer[DMA_CHANNEL_SIZE_HIGH] = 0;

      pBuffer[DMA_CHANNEL_DPR] = cpu_to_le32((physAddrNext & 0xFFFFFFF0) | CtrlWord);
      pBuffer[DMA_CHANNEL_DPR_HIGH] = cpu_to_le32((u64)physAddrNext >> 32);
      wmb();
    
      //Dump SGL buffer:

      if (i == 0)
      { 
        OmniTekDebug(DMA_SGL,KERN_INFO,"First SGL Descriptor\n");
        OmniTekDebug(DMA_SGL,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
            pBuffer,
            pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
            pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
            pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
            pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);
      }
		  else if(i == pTransaction->DMACoreInfo.sgt.nents - 1)		
      {

        OmniTekDebug(DMA_SGL,KERN_INFO,"Last SGL Descriptor\n");
        OmniTekDebug(DMA_SGL,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
            pBuffer,
            pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
            pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
            pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
            pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);

      } else if (wrap_buff == true)
      {
        OmniTekDebug(DMA_SGL,KERN_INFO,"Descriptor after wrapped buffer\n");
        OmniTekDebug(DMA_SGL,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
            pBuffer,
            pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
            pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
            pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
            pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);
        wrap_buff = false;

      } else if (wrap_desc == true)
      {
        OmniTekDebug(DMA_SGL,KERN_INFO,"Descriptor after wrapped physaddr\n");
        OmniTekDebug(DMA_SGL,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
            pBuffer,
            pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
            pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
            pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
            pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);
        wrap_desc = false;
      }

      OmniTekDebug(DMA_SGL,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
          pBuffer,
          pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
          pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
          pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
          pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);
  /*
          pBuffer,
          sg_dma_address(cur_entry) >> 32,sg_dma_address(cur_entry) & 0xFFFFFFFF,
          0,localAddr,
          (physAddrNext >> 32),(physAddrNext & 0xFFFFFFF0) | CtrlWord,
          0,sg_dma_len(cur_entry));
  */


      //Increment DPR address, loop back to start of buffer if necessary
      physAddrNext += SGL_ITEM_SIZE;
      pBuffer += SGL_ITEM_SIZE/sizeof(u32);

      localAddr += sg_dma_len(cur_entry);
      transferred += sg_dma_len(cur_entry);

      
  //TODO: This check isn't valid since we allocate the coherent buffer specifically for the required number of entries
  /* 
      if (pBuffer > (pTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + pTransaction->pChannel->u.DmaChannel.SglBuffer.Size))
      {
        OmniTekDebug(DMA_CORE,KERN_ERR,"Overran DMA SGL Entry coherent buffer at item %d!  Buffer size: %08x\n",
                                        i,
                                        pTransaction->pChannel->u.DmaChannel.SglBuffer.Size);

        pBuffer = pTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer;  //Loop around?
      }
  */
	  }
  }
  else
  {
    //For kernel buffers we only have one entry...
    if (pTransaction->pChannel->u.DmaChannel.FDMA_Enabled == true)
    {
        pBuffer[DMA_CHANNEL_LADR] = cpu_to_le32(0x00000000);
    }
    else
    {
      pBuffer[DMA_CHANNEL_LADR] = cpu_to_le32(localAddr);
    }

    CtrlWord |= DMA_DPR_BIT_END_OF_CHAIN | DMA_DPR_BIT_INTERRUPT;		// Always Last entry - mark as such and interrupt

    pBuffer[DMA_CHANNEL_LADR_HIGH] = 0;

    pBuffer[DMA_CHANNEL_PADR] = cpu_to_le32(pTransaction->Xfer.DMA_Addr);
    pBuffer[DMA_CHANNEL_PADR_HIGH] = cpu_to_le32((u64)pTransaction->Xfer.DMA_Addr >> 32);

    pBuffer[DMA_CHANNEL_SIZE] = cpu_to_le32(pTransaction->Xfer.Size);
    pBuffer[DMA_CHANNEL_SIZE_HIGH] = 0;

    pBuffer[DMA_CHANNEL_DPR] = cpu_to_le32((physAddrNext & 0xFFFFFFF0) | CtrlWord);
    pBuffer[DMA_CHANNEL_DPR_HIGH] = cpu_to_le32((u64)physAddrNext >> 32);

    OmniTekDebug(DMA_SGL,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
        pBuffer,
        pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
        pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
        pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
        pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);

    wmb();
  }
    

  return 0;

}

int DMAJoinTransactions(
  struct _OmniTekDmaTransactionContext * pTransaction,
  struct _OmniTekDmaTransactionContext * pPrevTransaction
)
{
  //Join two DMA transactions - essentially make the last DPR of pPrevTransaction point to the first descriptor of pTransaction

  int nents = OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type) ? 1 : (pPrevTransaction->DMACoreInfo.sgt.nents);

#ifdef USE_FIXED_SGL_BUFFER
  u32 * pBuffer = (u32*)pPrevTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + 
                  pPrevTransaction->Sgl.Offset/sizeof(u32) + 
                  ((nents-1) * sizeof(DmaSglEntry)/sizeof(u32));

//  dma_addr_t physAddrNext = pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + pTransaction->Sgl.Offset;
  dma_addr_t physAddrNext = pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + 
                            pTransaction->Sgl.Offset/sizeof(u8);

  u32 * pNewBuffer = (u32*)pTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + 
                  pTransaction->Sgl.Offset/sizeof(u32);


  //If our buffer pointer has exceeded the size of the commonbuffer then we need to wrap it back to the start...

  if (pBuffer >= (u32*)pPrevTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + 
                 pPrevTransaction->pChannel->u.DmaChannel.SglBuffer.Size/sizeof(u32))
  {
    pBuffer -= pPrevTransaction->pChannel->u.DmaChannel.SglBuffer.Size / sizeof(u32);
    OmniTekDebug(DMA_JOIN,KERN_INFO,"Wrapping Descriptor pointer\n");
  }

  if (physAddrNext >= pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + 
                      pTransaction->pChannel->u.DmaChannel.SglBuffer.Size)
  {
    physAddrNext -= pTransaction->pChannel->u.DmaChannel.SglBuffer.Size;
    OmniTekDebug(DMA_JOIN,KERN_INFO,"Wrapping DPR pointer\n");
  }

  if (pNewBuffer >= (u32*)pTransaction->pChannel->u.DmaChannel.SglBuffer.CommonBuffer + pTransaction->pChannel->u.DmaChannel.SglBuffer.Size/sizeof(u32))
  {
    pNewBuffer -= pTransaction->pChannel->u.DmaChannel.SglBuffer.Size / sizeof(u32);
  }





#else
  u32 * pBuffer = pPrevTransaction->Sgl.CoherentMem + (SGL_ITEM_SIZE * (pPrevTransaction->DMACoreInfo.sgt.nents-1));
  dma_addr_t physAddrNext = pTransaction->Sgl.DmaMem;   //Address of first descriptor

#endif

  OmniTekDebug(DMA_JOIN,KERN_INFO,"Joining DMA transaction SGLs. Transaction %08x to %08x\n",
  pPrevTransaction->index,
  pTransaction->index);

  OmniTekDebug(DMA_JOIN,KERN_INFO,"Last descriptor of first transaction is %p\n",
  pBuffer);

  OmniTekDebug(DMA_JOIN,KERN_INFO,"First descriptor of second transaction is %p\n",
#ifdef USE_FIXED_SGL_BUFFER
  //pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + pTransaction->Sgl.Offset);
  (void *)(pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + 
  pTransaction->Sgl.Offset/sizeof(u8)));

#else
  (void*)pTransaction->Sgl.DmaMem);
#endif

  OmniTekDebug(DMA_JOIN,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
      pBuffer,
      pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
      pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
      pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
      pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);


  //Keep the existing values for the control bits.
  pBuffer[DMA_CHANNEL_DPR] &= (0xF & ~DMA_DPR_BIT_END_OF_CHAIN);      //Remove the end of chain bit, keep interrupt and direction bit
  pBuffer[DMA_CHANNEL_DPR] |= cpu_to_le32((u64)physAddrNext) | DMA_DPR_BIT_INTERRUPT;   //Definitely want interrupt bit set...

  //Enable pause bit in each joined transaction for EFDMA RX Channel
  if (pTransaction->pChannel->u.DmaChannel.FDMA_Throttling == true)
    pBuffer[DMA_CHANNEL_DPR] |= DMA_DPR_BIT_PAUSE;

  pBuffer[DMA_CHANNEL_DPR_HIGH] = cpu_to_le32((u64)physAddrNext >> 32); //Set high part of address.

  wmb();

  OmniTekDebug(DMA_JOIN,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
      pBuffer,
      pBuffer[DMA_CHANNEL_PADR_HIGH] ,pBuffer[DMA_CHANNEL_PADR] , 
      pBuffer[DMA_CHANNEL_LADR_HIGH] ,pBuffer[DMA_CHANNEL_LADR] , 
      pBuffer[DMA_CHANNEL_DPR_HIGH]  ,pBuffer[DMA_CHANNEL_DPR]  , 
      pBuffer[DMA_CHANNEL_SIZE_HIGH] ,pBuffer[DMA_CHANNEL_SIZE]);

  OmniTekDebug(DMA_JOIN,KERN_INFO,"%p\t  0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t 0x%08x%08x\t\n",
      pNewBuffer,
      pNewBuffer[DMA_CHANNEL_PADR_HIGH] ,pNewBuffer[DMA_CHANNEL_PADR] , 
      pNewBuffer[DMA_CHANNEL_LADR_HIGH] ,pNewBuffer[DMA_CHANNEL_LADR] , 
      pNewBuffer[DMA_CHANNEL_DPR_HIGH]  ,pNewBuffer[DMA_CHANNEL_DPR]  , 
      pNewBuffer[DMA_CHANNEL_SIZE_HIGH] ,pNewBuffer[DMA_CHANNEL_SIZE]);


  return 0;


}

int DMAMapKernel(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{
  //For a kernel mapping we just need to have the DMA_ADDR_T address for the buffer, as it is already contiguous.
  //We could possibly create a 'fake' scatter-gather table for it, but it's a waste of effort really...
  
  OmniTekDebug(DMA_CORE, KERN_INFO, "Created DMA mapping for kernel buffer.  Physical address: %p\n",(void*)pTransaction->Xfer.DMA_Addr);

  return pTransaction->Xfer.DMA_Addr ? 0 : -ENOMEM;


}

int DMAPrepareTransaction(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{

  int result = 0 ;

  //Core processing steps:
  // * Allocate pages for DMA mappings (struct page ** buffer)

  OmniTekDebug(DMA_CORE,KERN_INFO,"Preparing transaction: %08x for channel %d, type %d\n",pTransaction->index,pTransaction->pChannel->u.DmaChannel.Index,pTransaction->type);
  getNumPages(
    (unsigned long)pTransaction->Xfer.Buffer,
    pTransaction->Xfer.Size,
    &(pTransaction->DMACoreInfo.offset),
    &(pTransaction->DMACoreInfo.num_pages),
    &(pTransaction->DMACoreInfo.first_page),
    &(pTransaction->DMACoreInfo.last_page)    
  );
  if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
  {
    pTransaction->DMACoreInfo.pages = kmalloc(pTransaction->DMACoreInfo.num_pages * sizeof(struct page *),GFP_KERNEL);
    if (!pTransaction->DMACoreInfo.pages)
    {
      OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't allocate pages ** for non-kernel DMA!");
      return -ENOMEM;
    }
  }
  else
    pTransaction->DMACoreInfo.pages = NULL;


  // * Lock User Memory Pages

  if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
  {
    result = DMAGetUserPages(pTransaction);
    if (result)
    {
		  OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't map user pages: %d\n",result);
      return result;
    }    

    // * Map SG Entries - init_table_and_chain() and map_sg()  
    result = DMAMapTable(pTransaction);
    if (result)
    {
		  OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't map sg table: %d\n",result);
      return result;
    }    


  }
  else
  {
    result = DMAMapKernel(pTransaction);
    if (result)
    {
		  OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't map kernel pages: %d\n",result);
      return result;
    }    
  }  



  return 0;

}



int DMAStartTransaction(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{
 
  int result = 0;                              
  OmniTekDebug(DMA_CORE,KERN_INFO,"Starting transaction: %08x\n",pTransaction->index);

  // * Allocate space in DMA coherent buffer for descriptors

#ifdef USE_FIXED_SGL_BUFFER

  if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
  {
    pTransaction->Sgl.Offset = DmaGetSglBufferOffset(
    	pTransaction->pChannel,
    	pTransaction->DMACoreInfo.sgt.nents
	  );
  }
  else
  {
    pTransaction->Sgl.Offset = DmaGetSglBufferOffset(pTransaction->pChannel,1);   //TODO: Kernel buffer transfers always 1 entry - but need to enforce limit on single transfer size?
  }

  if (pTransaction->Sgl.Offset == -1)
  {
		OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't get coherent buffer space for %d sgl entries\n",  	pTransaction->DMACoreInfo.sgt.nents);
  

    if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
    {

      DMAUnMapSg(pTransaction);
		  sg_free_table(&(pTransaction->DMACoreInfo.sgt));
//      if (OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
        free_kernel_pages(pTransaction);
//      else
        free_user_pages(pTransaction);
      return -EBUSY;    //Return busy so the next layer up can re-queue the request
    }
    else
    {
      //TODO: Don't need to do anything?  We don't 'allocate' anything for the kernel buffer method
      return -EBUSY;
    }
  }

  //TODO: Duplication here with the sgt.nents

  if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
    pTransaction->Sgl.NumElements = pTransaction->DMACoreInfo.sgt.nents;
  else
    pTransaction->Sgl.NumElements = 1;

  if (pTransaction->Sgl.NumElements == 0)
    OmniTekDebug(DMA_CORE, KERN_ERR, "Scatter gather table has 0 entries?\n");


#else

  //TODO: Cleanup or implement DMA Pool version

  if ((pTransaction->state != OMNITEK_TRANSACTION_PREPARED) && (pTransaction->state != OMNITEK_TRANSACTION_PENDING))
  {
		OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't start transaction %08x, it is not prepared\n",pTransaction->index);
    return -EINVAL;
  }


/*  pTransaction->Sgl.CoherentMem = dma_alloc_coherent(&(pTransaction->pChannel->pExt->Device->dev), 
                                                     pTransaction->DMACoreInfo.sgt.nents * 4 * sizeof(u64),
                                                     &(pTransaction->Sgl.DmaMem),
                                                     GFP_KERNEL);
*/
  pTransaction->Sgl.CoherentMem = pci_alloc_consistent(pTransaction->pChannel->pExt->Device, 
                                                     pTransaction->DMACoreInfo.sgt.nents * 4 * sizeof(u64),
                                                     &(pTransaction->Sgl.DmaMem));

  
  if (pTransaction->Sgl.CoherentMem == 0)
  {    
		OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't get coherent buffer space for %d sgl entries\n",  	pTransaction->DMACoreInfo.sgt.nents);
//    DMAUnMapSg(pTransaction);
//		sg_free_table(&(pTransaction->DMACoreInfo.sgt));
//    free_user_pages(pTransaction);
    return -EBUSY;    //Return busy so the next layer up can place the request in the pending queue
  }
  else
  {
		OmniTekDebug(DMA_CORE,KERN_INFO,"Allocated coherent buffer at %p, DMA Addr: %p\n",pTransaction->Sgl.CoherentMem, (void*)pTransaction->Sgl.DmaMem);
  }
#endif  

  
  // * Program SGL entries into buffers
  DMAProgramSgl(pTransaction);

  // * Join SGLs? (for FDMA)

  //If we're an FDMA channel and we have >= 3 outstanding requests then we join the requests together if we are not in automatic mode
  //if the channel is in auto mode we don't chain requests (effectively makes it a single transfer DMA channel)
  if ((pTransaction->pChannel->u.DmaChannel.FDMA_Enabled == true) && (pTransaction->pChannel->u.DmaChannel.FDMA_AutoStart == false))
  {
  
    
    if ((pTransaction->pChannel->u.DmaChannel.Running == true && OmniTekRequestQueueSize(&pTransaction->pChannel->u.DmaChannel.Active) >= 3) 
     || ((pTransaction->pChannel->u.DmaChannel.FDMA_Throttling == true) && OmniTekRequestQueueSize(&pTransaction->pChannel->u.DmaChannel.Active) >=2)
     || (pTransaction->pChannel->u.DmaChannel.Running == false && OmniTekRequestQueueSize(&pTransaction->pChannel->u.DmaChannel.Active) >=1) )
    {
      //We should join these transactions;
      struct _OmniTekRequestQueueObject      * pRequest;
      struct _OmniTekDmaTransactionContext * pPrevTransaction;

      int result = OmniTekRequestQueueLast(
        &(pTransaction->pChannel->u.DmaChannel.Active),
        &pRequest
      );
  		OmniTekDebug(DMA_CORE,KERN_INFO,"Chaining transaction %08x\n",pTransaction->index);

      if (result)
      {
    		OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't get a previous transaction to chain\n");
        return result;
      }

      pPrevTransaction = container_of(pRequest,struct _OmniTekDmaTransactionContext,QueueObject);

      if (pPrevTransaction->DMACoreInfo.Chained == true)
      {
    		OmniTekDebug(DMA_CORE,KERN_ERR,"Previous transaction %08x is already chained!\n",pPrevTransaction->index);
        return -EINVAL;
      }
      
      result = DMAJoinTransactions(pTransaction,pPrevTransaction);
      if (result)
      {
    		OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't chain transactions?\n");
        return result;
      }

  		OmniTekDebug(DMA_CORE,KERN_INFO,"Transaction %08x chained from transaction %08x\n",pTransaction->index,pPrevTransaction->index);
    }
    else
    {
      //If the channel is running then we have a problem (likely starved).  If it's not running then we are ok, presume this is the first transaction
      if (pTransaction->pChannel->u.DmaChannel.Running == true)
      { 
        OmniTekDebug(DMA_CORE,KERN_ERR,"Couldn't chain transactions, insufficient outstanding (%d) on channel %p no. %d\n",
          OmniTekRequestQueueSize(&pTransaction->pChannel->u.DmaChannel.Active),
          pTransaction->pChannel, pTransaction->pChannel->u.DmaChannel.Index);
          result = -EBUSY;
      }

      return result;
    }  

  }

  OmniTekDebug(DMA_CORE,KERN_INFO,"Transaction: %08x started...\n",pTransaction->index);

  return 0;  
}

void DMAFinishTransaction(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{
  //Should be called with the channel lock held
  //Core cleanup steps:
 
  //TODO: should we check transaction state here?

  // * dma_unmap_sg() - undo scatter gather mapping

  if (!OMNITEK_TRANSACTION_BUFFER_KERNEL(pTransaction->type))
  {

    DMAUnMapSg(pTransaction);  
   
    // * sg_free_table()
    sg_free_table(&(pTransaction->DMACoreInfo.sgt));

    // * Free user pages and mark dirty
      free_user_pages(pTransaction);

    // * free page ** buffer
    if (pTransaction->DMACoreInfo.pages)
      kfree(pTransaction->DMACoreInfo.pages);  
  }

  // * Free coherent buffer
#ifdef USE_FIXED_SGL_BUFFER
  DmaReleaseSglBuffer(pTransaction);
#else
/*  dma_free_coherent(&(pTransaction->pChannel->pExt->Device->dev), 
                   pTransaction->DMACoreInfo.sgt.nents * 4 * sizeof(u64),
                   pTransaction->Sgl.CoherentMem,
                   pTransaction->Sgl.DmaMem);
*/

  pci_free_consistent(pTransaction->pChannel->pExt->Device, 
                   pTransaction->DMACoreInfo.sgt.nents * 4 * sizeof(u64),
                   pTransaction->Sgl.CoherentMem,
                   pTransaction->Sgl.DmaMem);
#endif                                                     

  pTransaction->DMACoreInfo.pages = 0;
  OmniTekDebug(DMA_CORE,KERN_INFO,"Transaction %08x Finished\n",pTransaction->index);

}


int DMAChannelStart(
  PResource pChannel
)
{
  int result;
  int CtrlWord = 0;
  u32 CsrWord = 0;
  u32 * PhysAddr = 0;
  struct _OmniTekRequestQueueObject    * pRequest;
  struct _OmniTekDmaTransactionContext * pTransaction;

  unsigned long flags;

  //To start a channel do the following:
  // * If the channel is already running then return busy?
  // * Look to see if there are active transactions:  If there are (at least one) then we can proceed
  // * Possibly for FDMA channels we can check for the number of outstanding requests and if < 3 print warning
  // * Program the DPR for the first descriptor
  // * Enable and start DMA channel

  //If there are pending requests, but no active requests, something has gone wrong somewhere!  Possibly we program the next
  //pending transaction and schedule it...
  //TODO: Program next pending transaction if there are pending but no active...


  if (!OmniTekSpinLockCheck(&(pChannel->SpinLock),DMA,KERN_ERR,"Channel Start called without spinlock held!\n"))
	return -EPERM;


  spin_lock_irqsave(&(pChannel->pExt->IrqLock),flags);  //TODO: irqlock?

  if (pChannel->u.DmaChannel.Running == true)
  {
	OmniTekDebug(DMA_CORE,KERN_NOTICE,"Channel %d already started (status)!\n",pChannel->u.DmaChannel.Index);
	spin_unlock_irqrestore(&(pChannel->pExt->IrqLock),flags);  //TODO: irqlock?
	return -EBUSY;
  }

  ReadRegValue(pChannel->pExt,
				pChannel->General.Bar,
				pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
				&CsrWord);

  if (CsrWord & 0x1)
  {
	OmniTekDebug(DMA_CORE,KERN_NOTICE,"Channel %d already started (reg) 0x%08x!\n",pChannel->u.DmaChannel.Index,CsrWord);
	spin_unlock_irqrestore(&(pChannel->pExt->IrqLock),flags);  //TODO: irqlock?
	return -EBUSY;
  }

  if (CsrWord & OMNITEK_DMACHANNEL_INTERRUPT_SG)
  {
	OmniTekDebug(DMA_CORE,KERN_NOTICE,"Channel %d has pending SG interrupt\n",pChannel->u.DmaChannel.Index);
	return -EBUSY;
  }
  if (CsrWord & OMNITEK_DMACHANNEL_INTERRUPT_EVT)
  {
	OmniTekDebug(DMA_CORE,KERN_NOTICE,"Channel %d has pending Event interrupt\n",pChannel->u.DmaChannel.Index);
	return -EBUSY;
  }

  if (OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active)))
  {
	OmniTekDebug(DMA_CORE,KERN_ERR,"Channel %d started with no active requests!\n",pChannel->u.DmaChannel.Index);
	spin_unlock_irqrestore(&(pChannel->pExt->IrqLock),flags);  //TODO: irqlock?
	dump_stack();
	return -EINVAL;
  }

  result = OmniTekRequestQueueNext(&(pChannel->u.DmaChannel.Active),&pRequest);
  if (result)
  {
	OmniTekDebug(DMA_CORE,KERN_ERR,"Channel %d couldn't get next active request!\n",pChannel->u.DmaChannel.Index);
	spin_unlock_irqrestore(&(pChannel->pExt->IrqLock),flags);  //TODO: irqlock?
	return -EINVAL;
  }

  pTransaction = container_of(pRequest,struct _OmniTekDmaTransactionContext,QueueObject);

  CtrlWord |= ((pTransaction->Xfer.Write == false)? (1<<3) : 0 | 1);  //Set write bit, and set PCIE Scatter Gather bit
#ifdef USE_FIXED_SGL_BUFFER
  //TODO: Write some functions to calculate the DPR addresses so that there aren't many places in code to change them...
  PhysAddr = (u32*)pTransaction->pChannel->u.DmaChannel.SglBuffer.DMA_Handle + pTransaction->Sgl.Offset/sizeof(u32);

#else
  PhysAddr = pTransaction->Sgl.DmaMem;
#endif

  if ((unsigned long)PhysAddr & 0x1F)
  {
	OmniTekDebug(DMA_CORE, KERN_ERR, "Physaddr: %p is not 64 bit aligned!\n",PhysAddr);
	return -EINVAL;
  }

  if (pTransaction->DMACoreInfo.sgt.nents == 1)
	CtrlWord |= DMA_DPR_BIT_INTERRUPT | DMA_DPR_BIT_END_OF_CHAIN;   //If there's only one element set stop and interrupt after transfer?

  //Clear the bytes transferred register if this channel is being started
  DmaChannelReadBytesAvail(pChannel);
  atomic_set(&(pChannel->u.DmaChannel.nBytesXferred),0);
  atomic_set(&(pChannel->u.DmaChannel.nBytesUnhandled),0);
  atomic_set(&(pChannel->u.DmaChannel.nSgIrqs),0);


  OmniTekDebug(DMA_CORE,KERN_INFO,"Starting DMA Channel %d @ %p.  First SGL entry @ %p\n",pChannel->u.DmaChannel.Index,pChannel,(void*)PhysAddr);

  //Program DPR of first descriptor:
  WriteRegValue(pChannel->pExt,
				pChannel->General.Bar,
				pChannel->General.RegisterOffset + DMA_CHANNEL_DPR_HIGH,
				cpu_to_le32((u64)PhysAddr >> 32));

  WriteRegValue(pChannel->pExt,
				pChannel->General.Bar,
				pChannel->General.RegisterOffset + DMA_CHANNEL_DPR,
				cpu_to_le32((u64)PhysAddr & 0xFFFFFFE0) | CtrlWord);

  OmniTekDebug(DMA_CORE,KERN_INFO,"DPR Set to: %lx\n",(unsigned long)PhysAddr | CtrlWord);

  wmb();  //Probably unnecessary, but just to be safe...


  pChannel->u.DmaChannel.Running = true;

  //Clear any outstanding bytes xferred
  if (pChannel->u.DmaChannel.FDMA_Read == true)
	DmaChannelReadBytesAvail(pChannel);

  //Enable DMA and start (SGL Mode)

  ReadRegValue(pChannel->pExt,
				pChannel->General.Bar,
				pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
				&CsrWord);

  WriteRegValue(pChannel->pExt,
				pChannel->General.Bar,
				pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
				CsrWord | 0x103);

  spin_unlock_irqrestore(&(pChannel->pExt->IrqLock),flags);  //TODO: irqlock?

  if (pChannel->u.DmaChannel.Running == false)
	OmniTekDebug(DMA_CORE,KERN_ERR,"Channel not running!\n");

  OmniTekDebug(DMA_CORE,KERN_INFO,"Channel %d started... CSR %x\n",pChannel->u.DmaChannel.Index,CsrWord);

  return 0;

}

EXPORT_SYMBOL(DMAChannelStart);


void DMAChannelPause(
  PResource pChannel,
  bool pause
)
{
  u32 CsrWord = 0;

  if (pChannel->u.DmaChannel.Running == false)
  {
    OmniTekDebug(DMA_CORE,KERN_INFO,"Channel %d not running, so not pause/unpausing\n",pChannel->u.DmaChannel.Index);
    return;
  }

  ReadRegValue(pChannel->pExt,
                pChannel->General.Bar,
                pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
                &CsrWord);

  //We only want to affect the enable bit.  We do _not_ want to write back any of the interrupt bits!
  //Everything interesting is in the lower 3 nibbles.  So leave 11:8 as is, don't touch 7:4 and don't write 4:1

  CsrWord &= 0xFFFFFF00;
  CsrWord |= (pause ? 0x0 : 0x1);   //Set to 0 to pause, and 1 to resume

  WriteRegValue(pChannel->pExt,
                pChannel->General.Bar,
                pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
                CsrWord);

  if (pChannel->u.DmaChannel.Paused != pause)
  {
    pChannel->u.DmaChannel.Paused = pause;
    OmniTekDebug(DMA_CORE,KERN_INFO,"Channel %p no. %d %s\n", pChannel, pChannel->u.DmaChannel.Index, pause ? "Paused" : "Resumed");
  }

 

}
EXPORT_SYMBOL(DMAChannelPause);


void DMAChannelStop(
  PResource pChannel
)
{
  u32 Val;
  unsigned long flags;
  spin_lock_irqsave(&(pChannel->pExt->IrqLock),flags);

  OmniTekDebug(DMA_CORE,KERN_NOTICE,"Aborting DMA Channel %d\n",pChannel->u.DmaChannel.Index);

  ReadRegValue(pChannel->pExt,
               pChannel->General.Bar,
               pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
                &Val);

  //Set abort, unset enable.
  WriteRegValue(pChannel->pExt,
                pChannel->General.Bar,
                pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
                (Val | 0x4) & ~0x1);

  //Read value back to force PCIE Buffer flush
  ReadRegValue(pChannel->pExt,
               pChannel->General.Bar,
               pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
                &Val);
  
  OmniTekDebug(DMA_CORE,KERN_INFO,"Flagging channel %d as NOT running...\n",pChannel->u.DmaChannel.Index);
  pChannel->u.DmaChannel.Running = false;
  wmb();
  spin_unlock_irqrestore(&(pChannel->pExt->IrqLock),flags);
}

EXPORT_SYMBOL(DMAChannelStop);


#if 0
bool DMAChannelThrottle(
  PResource pChannel
)
{
  //Throttling control for DMA channels...

  //When called this makes the decision as to whether the channel needs throttling.
  //There are two criteria used: number of bytes unhandled and number of slices queued

  //If there are > a specified threshold number of bytes outstanding _or_ there are
  //less than a required number of DMA transfers queued for the channel, then the channel is throttled.

  //This routine is to be called by the interrupt handler to make a fast decision on whether the channel
  //needs throttling

  //If more than 4 slices data outstanding or less than 6 slices left active stall  

  if ( (atomic_read(&(pChannel->u.DmaChannel.nBytesUnhandled)) >= 0x100000) ||
       ((OmniTekRequestQueueSize(&(pChannel->u.DmaChannel.Active)) - atomic_read(&(pChannel->u.DmaChannel.nSgIrqs)) < 6)))
  {
    DMAChannelPause(pChannel,true);
    return true;
  }
 
  return false;
}
EXPORT_SYMBOL(DMAChannelThrottle);
#endif

bool DMAChannelUnThrottle(
  PResource pChannel,
  int threshold
)
{
  //Throttling control for DMA channels...
  
  //When called this makes the decision as to whether the channel needs un-throttling
  
  //The same criteria as throttling are used, but both must be false, so there must be < a threshold of bytes
  //unhandled and > a number of slices queued before the channel is un-throttled.
  
  size_t bytes_unhandled = atomic_read(&(pChannel->u.DmaChannel.nBytesUnhandled));
  int    slices_remaining = (OmniTekRequestQueueSize(&(pChannel->u.DmaChannel.Active)) - atomic_read(&(pChannel->u.DmaChannel.nSgIrqs)));

  static bool throttled = false;

  if (pChannel->u.DmaChannel.FDMA_Throttling == false)
  {
    OmniTekDebug(DMA_THROTTLE, KERN_ERR, "Unthrottle called on channel without throttling enabled %d\n",pChannel->u.DmaChannel.Index);
    return false;
  }

  //Unpause the channel if there are less than a threshold amount of bytes left to unhandled and there are enough DMA slices waiting 
  
  if ( (bytes_unhandled < 0x4000) && ((slices_remaining >= threshold)))   //Block with 16K outstanding...
  {
    
    if (throttled)
      OmniTekDebug(DMA_THROTTLE, KERN_INFO, "Resuming channel:  Bytes Unhandled %zd, no. active requests remaining %d\n",
                    bytes_unhandled,
                    slices_remaining
                );

    OmniTekDebug(DMA_THROTTLE, KERN_INFO, "Resuming channel:  Bytes Unhandled %zd, no. active requests remaining %d\n",
                    bytes_unhandled,
                    slices_remaining
                );

    DMAChannelPause(pChannel,false);
    throttled = false;
    return true;
  }
  else
  {
    if (!throttled)
      OmniTekDebug(DMA_THROTTLE, KERN_INFO, "Not Resuming channel:  Bytes Unhandled %zd, no. active requests remaining %d\n",
                    bytes_unhandled,
                    slices_remaining
                );
                
    OmniTekDebug(DMA_THROTTLE, KERN_INFO, "Not resuming channel:  Bytes Unhandled %zd, no. active requests remaining %d\n",
                    bytes_unhandled,
                    slices_remaining
                );
    throttled = true;
  }
 
  return false;
}
EXPORT_SYMBOL(DMAChannelUnThrottle);

