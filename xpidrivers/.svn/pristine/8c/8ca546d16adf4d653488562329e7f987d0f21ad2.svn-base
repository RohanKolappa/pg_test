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
#include "OmniTekDmaOperations.h"


int dma_get_user_pages(unsigned long startPage, unsigned long numPages, struct page ** pages, struct vm_area_struct ** vmas)
{
	int got_pages = 0;

	
	OmniTekDebug(DMA_OPS,KERN_NOTICE,"start page 0x%lx, number of pages: %ld\n",startPage,numPages);

	if (current == NULL)
	{
		OmniTekDebug(DMA_OPS,KERN_NOTICE,"current is null?\n");
		return 0;
	}
	if (current->mm == NULL)
	{
		OmniTekDebug(DMA_OPS,KERN_NOTICE,"current->mm is null?\n");
		return 0;
	}
	

	//Get the user pages
/*	down_read(&current->mm->mmap_sem);
	got_pages = get_user_pages(	current,
                  		       	current->mm,
                     		    	startPage,	//Is this correct?
                        			numPages,		//And this?
                        			1,
                        			0,
			                        pages,
                        			NULL
                       			);
	up_read(&current->mm->mmap_sem);
*/
  got_pages = get_user_pages_fast(startPage,numPages,1,pages);
	
	OmniTekDebug(DMA_OPS,KERN_NOTICE,"got %d pages\n",got_pages);
  
  if (got_pages <0)
    dump_stack();

/*	while (numPages)
	{
		OmniTekDebug(DMA_OPS,KERN_NOTICE," get user pages : page %d pages** %p page* %p\n",got_pages-numPages,
											     pages,pages[got_pages- numPages]); 
		numPages--;
	}
*/											     
  
  if (got_pages < numPages)
    return -EINVAL;
	return got_pages;
}

int dma_map_sg_pages(struct scatterlist ** sg, struct page ** pages, size_t nents, unsigned long len, loff_t offset)
{
	struct page * cur_page;
	struct scatterlist * cur_scatter = *sg;

	unsigned int len_remaining = nents;	
	ssize_t len_mapped	=0;

	int i = 0;

	OmniTekDebug(DMA_OPS,KERN_INFO,"mapping %ld pages, nents %d offset 0x%x \n",len,len_remaining,(unsigned int)offset);
//	for (int i = 0; i < len ; i++)
//	{
	OmniTekDebug(DMA_OPS,KERN_INFO,"sg = %p pages = %p\n",sg,pages);

  for_each_sg(*sg, cur_scatter, len,i) 
	{

		unsigned int cur_len = 0;
		cur_page = pages[i];

//		OmniTekDebug(DMA_OPS,KERN_INFO,"setting up sg page %d @ %p\n",i,pages[i]);		

		if (i ==0)
		{
			cur_len = (1<<PAGE_SHIFT) - offset;			/* Work out size for first page */
			if (cur_len > len_remaining) cur_len = len_remaining;	/* If it fits within this page we just need to transfer that amount */
		}
		else if (len_remaining > (1<<PAGE_SHIFT)) 	cur_len = 1<<PAGE_SHIFT;
		else 						cur_len = len_remaining;

//		OmniTekDebug(DMA_OPS,KERN_NOTICE," map_sg_pages : page length %d, length remaining %d\n",cur_len, len_remaining);		
//		OmniTekDebug(DMA_OPS,KERN_NOTICE," map_sg_pages : cur_scatter = %p, cur_page = %p\n",cur_scatter,cur_page);

		if (cur_scatter == NULL)
			OmniTekDebug(DMA_OPS,KERN_ERR,"CUR_SCATTER IS NULL!!!\n");
		if (cur_page == NULL)
			OmniTekDebug(DMA_OPS,KERN_ERR,"CUR_PAGE IS NULL!!!\n");

		sg_set_page(cur_scatter,cur_page,cur_len,(i == 0 ? offset : 0));
		len_remaining -= cur_len;		
		len_mapped += cur_len;
	} 
	OmniTekDebug(DMA_OPS,KERN_INFO,"%d scatter entries/pages mapped size %zd\n",i,len_mapped);
	if (i != len)
		OmniTekDebug(DMA_OPS,KERN_INFO,"only mapped %d entries, was expecting %zd\n",i,nents);

	return len_mapped;
}

#if 0
int dma_map_sg_init_table_and_chain(
  struct sg_table * sgt, 
  unsigned long num_pages)
{
	OmniTekDebug(DMA_OPS,KERN_INFO,"Calling sg_alloc_table\n");
  //TODO: Changed to atomic for ethernet test, can probably be changed back in future
	if (sg_alloc_table(sgt,num_pages,GFP_ATOMIC))
	{
		OmniTekDebug(DMA_OPS,KERN_ERR,"Sg alloc table didn't go very well, cleaning up.\n");
		sg_free_table(sgt);
		return -EINVAL;
	}
	return num_pages;
}
#endif

unsigned long dma_map_test_scatterlist(struct scatterlist ** sg, int nents)
{
	//We are going to scan the scatterlist, dumping each entry:
	int i;
	int transferred = 0;
	struct scatterlist * cur_entry;
	OmniTekDebug(DMA_OPS,KERN_INFO,"Dumping Scatter-gather list: entries %d\n",nents);

  OmniTekDebug(DMA_OPS,KERN_INFO,"Number:\tPADR\t\tSize:\n");
	for_each_sg(*sg,cur_entry, nents,i)
	{
//		printk(KERN_NOTICE "%d:\t0x%x\t%d\n",i,sg_dma_address(cur_entry),sg_dma_len(cur_entry));
		transferred += sg_dma_len(cur_entry);
	}
	OmniTekDebug(DMA_OPS,KERN_INFO,"Done, total to be transferred: %d\n",transferred);	
	return transferred;

}

