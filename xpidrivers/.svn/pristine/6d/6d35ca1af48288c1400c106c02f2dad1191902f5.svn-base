#pragma once

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

#include <linux/dma-mapping.h>
#include <linux/pagemap.h>

#include <linux/scatterlist.h>	

#include <asm/current.h>	/* current */
#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */
#include <linux/pfn.h>    /* PFN_DOWN etc.*/

/*!
 \brief				Fault in and lock pages for buffer
 \param [in]	startPage	first page to reserve
 \param [in]	numPages number of pages to reserve
 \param [in]	pages pointer to pages data structure to store details of reserved pages
 \param [in]	vmas pointer to vmas data structure - may be left null
 \return 			number of pages locked or error code
*/
int
dma_get_user_pages
(
	unsigned long startPage, 
	unsigned long numPages, 
	struct page ** pages, 
	struct vm_area_struct ** vmas
);

/*!
 \brief				Map locked pages into scatterlist
 \param [in]	sg scatterlist struct pointer
 \param [in]  pages array pointer to pages struct
 \param [in] 	nents number of entries to map
 \param [in]  len	total length to allocate (size of transaction)
 \param [in]  offset into first page for beginning of transaction
 \return			total length of entries mapped in scatterlist
*/
int
dma_map_sg_pages
(
	struct scatterlist ** sg, 
	struct page ** pages, 
	size_t nents, 
	unsigned long len, 
	loff_t offset
);

/*!
 \brief				Initialise scatter gather table and chain entries
 \param	[in]	sgt Scatter gather table structure to map entries into
 \param [in]	num_pages to map
 \return			number of pages mapped or error code
*/
int
dma_map_sg_init_table_and_chain
(
	struct sg_table * sgt, 
	unsigned long num_pages
);

/*!
 \brief				Test a mapped scatter list structure
 \param	[in]	sg Scatterlist structure to test
 \param [in]	nents number of entries in scatter list
 \return			total size that would be transferred
*/
unsigned long
dma_map_test_scatterlist
(
	struct scatterlist ** sg, 
	int nents
);


