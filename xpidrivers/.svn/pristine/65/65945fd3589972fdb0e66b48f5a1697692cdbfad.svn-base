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

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include "OmniTek_linux.h"


int allocate_all_attribute_ptrs(void);
int free_all_attribute_ptrs(void);

int allocate_and_init_attribute_ptrs(struct attribute *** pAttribute_ptrs, struct attribute * attributes);
void omnitek_driver_kobject_release(struct kobject * kobj);
void omnitek_driver_sysfs_create(struct _OmniTekDriver * pDriver);
void omnitek_extension_sysfs_create(struct _OMNITEK_INTERFACE_EXTENSION * pExt);
void omnitek_BAR_sysfs_create(struct _OMNITEK_INTERFACE_EXTENSION * pExt,int index);
void omnitek_resource_sysfs_create(struct _Resource * pResource);
void omnitek_dma_control_sysfs_create(struct _Resource * pResource);
void omnitek_dma_channel_sysfs_create(struct _Resource * pResource);
void omnitek_driver_kobject_put(struct kobject * kobj);
