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

#include "OmniTek_linux.h"
#if 0
/*!
 \struct OmniTek_BAR_dev
 \brief  This is container for an OmniTek BAR char device.
*/
struct OmniTek_BAR_dev {
};

/*!
 \struct  OmniTek_Resource_dev
 \brief   This device provides resource-based access to the register space
 \details Access to OmniTek 'resources' is performed through this device, so
          mainly IOCtl calls to specific resources etc.
*/
struct OmniTek_Resource_dev {
};
#endif

/*!
 \enum    _OMNITEK_DEVTYPES
 \brief   This enumeration lists the types of device that are used in the driver
*/ 
typedef enum _OMNITEK_DEVTYPES{
  OMNITEK_DEV_BAR,        /*!< BAR type device */
  OMNITEK_DEV_RESOURCE,   /*!< Resource type device */
  OMNITEK_DEV_MDMA        /*!< MDMA Type Device */
} OmniTek_DevTypes;

/*!  
  \struct OmniTek_dev 
  \brief  BAR Device data structure
*/
typedef struct _OmniTek_dev
{
  struct cdev                  cDev;             /*!< cdev struct for the BAR device */
  struct list_head             Object;           /*!< For adding to the driver list of devices */
  OmniTek_DevTypes             Type;             /*!< This device's type */
  struct _OMNITEK_INTERFACE_EXTENSION * pExt;    /*!< Pointer to the interface extension for this BAR */
  dev_t                        dev;              /*!< Device number (first device) assigned to this device type */
  int                          firstMinor;       /*!< If there are several devices with this type, then this is minor number of the first in the group */
  int                          nMinors;          /*!< If there are several devices with this type, then this is the total number of them */
/*  union {
    struct OmniTek_BAR_dev;
    struct OmniTek_Resource_dev;
  } u */
//  struct pci_dev *             ppci_dev;				 /*!< pci device associated with this OmniTek Device */
} OmniTek_dev;


static const struct file_operations OmniTek_dev_fops;

/*! \brief IOCtl call to BAR Device */
#ifdef MKERNEL
int OmniTek_BAR_dev_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#else
long OmniTek_BAR_dev_ioctl(struct file *, unsigned int, unsigned long);
#endif

/*! \brief BAR Device open */
int OmniTek_BAR_dev_open(struct inode *, struct file *);

/*! \brief BAR Device close/release */
int OmniTek_BAR_dev_release(struct inode * , struct file *);

/*!
 \brief					Setup BAR Device nodes
 \param [in]		pExt Pointer to device extension to set up BAR devices for
*/
void
OmniTekDeviceSetupDev(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
);

/*!
 \brief					Release BAR Device nodes
 \param [in]		pExt Pointer to device extension to release BAR devices for
*/
void 
OmniTekDeviceReleaseDev(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
);


