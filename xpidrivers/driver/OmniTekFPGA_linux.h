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
 \brief Initialise the FPGA resource 
*/
void
FPGAInit(
	Resource *pResource   /*! Pointer to (FPGA) resource to initialise */
);

/*! 
 \brief delete the FPGA resource
*/
void
FPGADelete(
	Resource *pResource   /*! Pointer to (FPGA) resource to delete */
);

/*!
 \brief  Enable interrupts for the device
 \param pExt      Pointer to the device extension
 \param Interrupt Bitmask for interrupts to enable 
 \param Enable    Enable or disable interrupts 
*/
void
InterruptEnable(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt,
	u32 Interrupt,
	bool Enable
);

/*!
 \brief Enable event interrupts for the DMA Channe;
 \param pChannel  Pointer to the DMA Channel resource
 \param Enable    Enable or disable event interrupts 
*/
void DMAChanEventInterruptEnable(
  struct _Resource * pChannel,
  bool               Enable,
  bool               Write
);


/*!
 \brief Get Interrupt status
 \param pExt      Pointer to the device extension
 \return the enabled interrupt bitmask
 */
u32
GetInterruptStatus(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
);

/*!
 \brief Get DMA Channel Interrupt status
 \param pChannel      Pointer to the DMA Channel Resource
 \return the enabled interrupt bitmask
 */
u32 GetDmaChannelInterruptStatus (
  PResource pChannel
);

/*!
 \brief Get DMA Controller Channels Interrupted
 \param pDmaCtrl Pointer to the Dma Controller resource
 \return the enabled interrupt bitmask
 */
u32 GetDmaCtrlChannelStatus (
  PResource pDmaCtrl
);



#if 0

/*!
 \brief Get FPGA video standard
 \return the current video standard (output)
 */
u32
FPGAGetStandard(
	struct _Resource * pResource          /*!< Pointer to the FPGA resource */
);

/*! 
 \brief deal with control messages for the FPGA resource
  \return  0 on success or error code
 */
int
FPGAControl(
  struct _Resource * pFPGA,             /*!< Pointer to the FPGA resource */
	struct _ResourceInfo *pResourceInfo   /*!< Additional information for command */
);

/*!
 \brief Read time from FGPA
 \return 64 bit time value from FPGA
 */
u64
FPGAReadTime(
  struct _Resource * pFPGA             /*!< Pointer to the FPGA resource */
);

/*!
 \brief Get time from FGPA
 */
void
FPGAGetTime(
  struct _Resource * pFPGA,            /*!< Pointer to the FPGA resource */
	u64 *Count                           /*!< Pointer to value to store time in */
);

#endif

