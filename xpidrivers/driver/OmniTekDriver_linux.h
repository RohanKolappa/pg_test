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

/*!
 \brief				Get Device ID from pcie endpoint
 \param [in]	dev	pointer to pci_dev struct to request ID from
 \return			device ID
*/
static u16
OmniTekGetDeviceId(
  struct pci_dev * dev
  );

/*!
 \brief				Get Device ID from pcie endpoint
 \param [in]	dev	pointer to pci_dev struct to request ID from
 \return			device ID
*/
static u8
GetNumPciLanes(
  struct pci_dev * dev
  );

/*!
 \brief				Device add function (pci probe function)
 \param [in]	dev pci device struct that is being probed
 \param [in]  id device id that we are being called for
 \return			0 if probing successful or error code
*/
static int
OmniTekDeviceAdd(
    struct        pci_dev *    dev,
    const  struct pci_device_id * id
    );

/*!
 \brief				Device remove function (pci remove function)
 \param [in]	dev pci device struct that is being removed
*/
static void
OmniTekDeviceRemove(
  struct pci_dev * dev
    );

//Required Data structures


static struct pci_device_id ids[] = {
	{ PCI_DEVICE(0x1AA3, 0x0001), },		//AVDP
	{ PCI_DEVICE(0x1AA3, 0x2002), },		//DATALAB
	{ PCI_DEVICE(0x1AA3, 0x0010), },		//ARRIA
  { PCI_DEVICE(0x11A4, 0x0057), },    //NETVIZ BASE?
  { PCI_DEVICE(0x11A4, 0x0058), },    //NETVIZ DEBUG?
	{ 0, }
};



#ifndef IORESOURCE_MEM_64             //If this isn't defined in ioport.h (which it isn't in 2.6.29) define it here instead)
#define IORESOURCE_MEM_64	0x00100000
#endif



