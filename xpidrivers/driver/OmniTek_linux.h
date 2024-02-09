#pragma once
/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific 
 * terms and conditions.  Use of any of the software or derviatives thereof in 
 * any product without a Image Processing Techniques is strictly prohibited. 
 * 
 * This file is provided without any express or implied warranties, including, 
 * but not limited to, the implied warranties of merchantability and fitness 
 * for a particular purpose. It is not intended for use in life support 
 * appliances, devices, or systems. Use in such applications is expressly 
 * prohibited.
 *
 * Image Processing Techniques makes no guarantee or representations regarding
 * the use of, or the results of the use of, the software and documentation in 
 * terms of correctness, accuracy, reliability, currentness, or otherwise; and 
 * you rely on the software, documentation and results solely at your own risk.
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS 
 * OF USE, LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR 
 * CONSEQUENTIAL DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>	/* size_t */
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/errno.h>	/* error codes */
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/phy.h>
#include <asm/uaccess.h>	/* copy_*_user */
#include <asm/atomic.h>

#include "OmniTek_debug.h"

#include "OmniTekRequest_linux.h"
#include "../include/OmniTekTypes_linux.h"
#include "../include/OmniTek_Exports.h"
#include "OmniTekResources_linux.h"

#include "OmniTekFPGA_linux.h"
#include "OmniTekFops_linux.h"
#include "OmniTekDma.h"
#include "OmniTekDMACore.h"
#include "OmniTekDMARequest.h"

//Omnitek Specific Error Codes:
#define STATUS_OMNITEK_MEMORY_ERROR 256
#define STATUS_OMNITEK_RESOURCE_INVALID 257
#define STATUS_OMNITEK_RESOURCE_LOCKED 258
#define STATUS_OMNITEK_ILLEGAL_SESSION_ID 259
#define STATUS_OMNITEK_RESOURCE_COMMAND_ERROR 260
#define STATUS_INVALID_PARAMETER_1 261
#define STATUS_INVALID_PARAMETER_2 262
#define STATUS_INVALID_PARAMETER_3 263

//FPGA registers for S6/S3 access
#define FPGA_S3_CMD_REG     0x0340  /* S3 (Expansion) FPGA command register */
#define FPGA_S3_ADDR_REG    0x0344  /* S3 (Expansion) FPGA addr register */
#define FPGA_S3_DATAI_REG   0x0348  /* S3 (Expansion) FPGA datain register */
#define FPGA_S3_DATAO_REG   0x034c  /* S3 (Expansion) FPGA dataout register */
#define FPGA_S6_CMD_REG     0x0870  /* S6 (Fanout) FPGA command register */
#define FPGA_S6_ADDR_REG    0x0874  /* S6 (Fanout) FPGA addr register */
#define FPGA_S6_DATAI_REG   0x0878  /* S6 (Fanout) FPGA datain register */
#define FPGA_S6_DATAO_REG   0x087c  /* S6 (Fanout) FPGA dataout register */

/*! 
 \struct _InterruptData
 \brief data about interrupts
*/
typedef struct _InterruptData
{
	POMNITEK_INTERFACE_EXTENSION	pExt;    /*!< Pointer to device extension */
	u32						nInterruptStatus;/*!<  Interrupt status */
} InterruptData;

//WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(InterruptData, GetInterruptData)

// NOTE: Should NOT be used except for initialisation - no bounds checking is done


#ifndef CONFIG_MACH_NETVIZ

#define ReadHWValue(pExt,b,r) \
  ioread32((u32*)((pExt)->MemBar[(b)].pVa) + (r))

#define WriteHWValue(pExt,b,r,Val) \
  iowrite32((Val),(pExt)->MemBar[(b)].pVa + (r))

#define ReadHWValueByte(pExt,b,r) \
  ioread8((u8*)((pExt)->MemBar[(b)].pVa) + (r))

#define WriteHWValueByte(pExt,b,r,Val) \
  iowrite8((Val) & 0xFF,((u8*)(pExt)->MemBar[(b)].pVa) + (r))


#else

#define ReadHWValue(pExt,b,r) \
  ioread32((u32*)((pExt)->MemBar[(b)].Physical + (r)))

#define WriteHWValue(pExt,b,r,Val) \
  iowrite32((Val),(u32)((pExt)->MemBar[(b)].Physical + (r)))

#define ReadHWValueByte(pExt,b,r) \
  ioread8((u8*)((pExt)->MemBar[(b)].Physical) + (r))

#define WriteHWValueByte(pExt,b,r,Val) \
  iowrite8((Val) & 0xFF,(u8*)((pExt)->MemBar[(b)].Physical) + (r))

#endif 

/* porting to 2.6.39.1 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29))
#define ioctl unlocked_ioctl
#define S_ATTR(a,b,c) {a,c} //remove obsolete member '*owner' from 'struct attribute'
#undef MKERNEL
#else
#define MKERNEL 1 // Marvell Kernel
#define S_ATTR(a,b,c) {a,b,c}
#endif

/*! 
 \brief    Initialise the OmniTek device extension
 \details initial setup of the device extension data structure from the PCI device
 \param [in] device The device that this extension is being initialised for
 \param [in] pExt Pointer to OmniTek Extension struct to initialise
 \param [in] driver Pointer to OmniTek Driver struct
 */
void
OmniTekExtInit(
    struct pci_dev * device,
    POMNITEK_INTERFACE_EXTENSION	pExt,
    struct _OmniTekDriver * driver
	);
	
/*! 
 \brief    Cleanup for device extension
 \details Performs necessary cleanup for the device extension when it is shutdown
 \param [in] pExt Pointer to Omnitek Extension struct to shutdown
 */
void
OmniTekExtShutdown
	(
	POMNITEK_INTERFACE_EXTENSION	pExt
	);

/*!
 \brief Get a cached register value for the device
 \details This will return the cached register value for the device (e.g. last written) - no hardware access

 \param [in] pExt Pointer to device extension to read from
 \param [in] nBar PCIE Bar number
 \param [in] nReg Register offset
 \param [in] pVal Pointer to (32 bit) value to store result in
 \return 0 on success or error code
 */
int
GetRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32        				    nBar,
	u32			              nReg,
	u32                    *pVal
	);

/*!
 \brief Read a register value from the hardware
 \details This will perform a register read from the hardware.  The cached register values are not updated.
 \param [in] pExt Pointer to device extension to read from
 \param [in] nBar PCIE Bar number
 \param [in] nReg Register offset
 \param [in] pVal Pointer to (32 bit) value to store result in
 \return 0 on success or error code
 */
int
ReadRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32				nBar,
	u32				nReg,
	u32			   *pVal
	);

/*!
 \brief Write a register value to the hardware
 \details This performs a write to a hardware register.  The cached register values are updated.
 \param [in] pExt Pointer to device extension to write to
 \param [in] nBar PCIE Bar number
 \param [in] nReg Register offset
 \param [in] nVal value to write
 \return 0 on success or error code
 */
int
WriteRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32				nBar,
	u32				nReg,
	u32			  nVal
	);

/*!
 \brief Scan hardware for Resources
 \details This performs a scan of the register space in each BAR.  It uses the linked list structure to determine the maximum number of register in the BAR, and the resources present
 Resource types are initialised here (so virtual resources e.g. DMA channels are also elaborated here).
 \return 0 on success or negative error code
 */
int
OmniTekScanHw(
	POMNITEK_INTERFACE_EXTENSION	pExt
	);

/*!
 \brief Return a list of the available capabilities
 \details This routine returns the available capabilities from the device
 \param [in]  pExt Omnitek Device Extension to get capabilities from
 \param [in] pCapList Pointer to capability list struct
  \return 0 on success or error code
*/
int
OmniTekGetCapList(
  POMNITEK_INTERFACE_EXTENSION	pExt,
	CapabilityList *				pCapList
	);

/*! 
  \brief      Get the OmniTekDriver struct from the pci_driver
  \details    Gstatic u32 list_count(struct list_head * head)
et the main driver data structure back from a pci_driver struct
  \param [in] driver  Pointer to the pci_driver structure that is contained within the OmniTekDriver struct
  \return     A pointer to the OmniTekDriver struct
*/
struct _OmniTekDriver *
GetOmniTekDriver(
  struct pci_driver * driver
  );

u32 list_count(struct list_head * head);

int list_index(struct list_head * list, struct list_head * entry);

extern int xpi_get_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long *val);
extern int xpi_set_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt,
        unsigned long reg, unsigned long val);
extern int xpi_and_in_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt,
        unsigned long reg, unsigned long val);
extern int xpi_or_in_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val);

extern int xpi_get_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt,
        int bar, unsigned long reg, unsigned long *val);
extern int xpi_set_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int bar, unsigned long reg, unsigned long val);
extern int xpi_and_in_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int bar, unsigned long reg, unsigned long val);
extern int xpi_or_in_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int bar, unsigned long reg, unsigned long val);

extern int xpi_get_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long *val);
extern int xpi_set_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val);
extern int xpi_and_in_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val);
extern int xpi_or_in_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val);

extern int xpi_get_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long *val);
extern int xpi_set_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val);
extern int xpi_and_in_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt,
        unsigned long reg, unsigned long val);
extern int xpi_or_in_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val);

extern int xpi_paged_mdio_write(struct _OMNITEK_INTERFACE_EXTENSION *pExt,
        int mdio_offset, int offset, int phyad, u16 page, u16 addr, u16 val);
extern int xpi_paged_mdio_read(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int mdio_offset, int offset, int phyad, u16 page, u16 addr);
extern int xpi_mac_write(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int offset, int addr, int val);
extern int xpi_mac_read(struct _OMNITEK_INTERFACE_EXTENSION *pExt, int offset, 
        int addr);

extern void set_gpio_pin(int pin, char *name, int val);
