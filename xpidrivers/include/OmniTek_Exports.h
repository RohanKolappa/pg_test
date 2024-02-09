#pragma once 

/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific terms
 * and conditions.  Use of any of the software or derviatives thereof in any
 * product without a Image Processing Techniques is strictly prohibited. 
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
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF
 * USE, LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR 
 * CONSEQUENTIAL DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/
#ifndef CONFIG_MACH_NETVIZ
#define USE_IRQ_THREAD
#endif

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/pci.h>
#include <linux/workqueue.h>
#include <linux/types.h>	/* size_t */
#include <linux/errno.h>	/* error codes */
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/interrupt.h>

/*!
 \struct OmniTekDriver
 \brief  Used to keep track of the Extensions created by the driver
*/
typedef struct _OmniTekDriver
{
  struct list_head  Extensions;
  struct list_head  Devices;
  dev_t             dev;  
  u32               NextMinor;
  struct kmem_cache	*ResourcePool;  /*!< Lookaside buffer for resources */
  struct pci_driver pci_driver;
  struct kobject    kobject;        /*!< For sysfs entry */
  struct class      *omnitek_class;
  
} OmniTekDriver;

/*! 
 \struct _PCI_BAR_INFO
 \brief PCI BAR Space information 
*/
typedef struct _PCI_BAR_INFO
{
    u32            *pVa;        /*!< BAR Kernel Virtual Address */
    u32            *Physical;   /*!< BAR Physical Address */
    u32            Size;        /*!< BAR size */
    bool           IsIoMapped;  /*!< Memory or I/O mapped? */
    struct kobject kobject;     /*!< KObject for sysfs */
    dev_t          dev;         /*!< Device number for this BAR */
    struct device  *device;     /*!< device struct for sysfs */
} PCI_BAR_INFO;

#define PCI_NUM_BARS	    		        6
#define MAX_NUM_MEM_BARS			        4

#define OMNITEK_INTERRUPT_MASK		        0x3f0000

#define OMNITEK_DMACTRL_INTERRUPT_MASK		0xff
#define OMNITEK_INTERRUPT_STATUS_TEST       0x1
#define OMNITEK_INTERRUPT_STATUS_DMA        0x2
#define OMNITEK_INTERRUPT_STATUS_TX         0x4
#define OMNITEK_INTERRUPT_STATUS_TIMER      0x8

#define OMNITEK_DMACHANNEL_INTERRUPT_MASK   0x60
#define OMNITEK_DMACHANNEL_INTERRUPT_SG     0x20
#define OMNITEK_DMACHANNEL_INTERRUPT_EVT    0x40
#define OMNITEK_DMACHANNEL_SETTINGS_MASK    0xF00
#define OMNITEK_DMACHANNEL_CONTROL_MASK     0xF

#define OMNITEK_DMA_INTERRUPT		        0x2

//TODO: Share these with the user space API
// Definitions to encode DMA channel identifiers
#define DMA_CHANNEL_FDMA    0x8000000   // Bit to identify FDMA Dma channels
#define DMA_CHANNEL_WRITE	0x40000000	// Bit to identify FDMA direction
#define DMA_CHANNEL_READ	0x20000000	// Bit to identify FDMA direction

// NGS-D200 XPI MDIO 
#define XPI_MDIO_PHYAD      1           // PHY address
#define MAIN_OFFSET         0           // Main PHY offset
#define EXPANSION_OFFSET    0x400       // Expansion PHY offset

struct _OMNITEK_INTERFACE_EXTENSION;

typedef void (*ExtInterruptISR)(void * dev_id);
typedef void (*ExtInterruptHandler)(void * dev_id);

typedef struct _OMNITEK_INTERFACE_EXTENSION
{
  struct list_head  Object; /*!< For tracking the extension */
  struct pci_dev *Device;   /*!< Pointer to the underlying PCI device */
  struct _OmniTekDriver *pDriver;   
                            /*!< Pointer to OmniTekDriver struct for device */
  PCI_BAR_INFO MemBar[MAX_NUM_MEM_BARS];   /*!< Details of the device BARs */
  u8 nBars;                 /*!< Number of BARs available */
  spinlock_t SpinLock;	    /*!< Prevent simultaneous access to the context */
  spinlock_t IrqLock;       /*!< Used to disable interrupt (on curr processor)*/
  spinlock_t PhyLock[2];    /*!< Used to lock PHY page access for NGS-D200 */
  u32 nInterruptStatus;     /*!< Interrupt status for this device */
  struct _Resource *pFPGA;  /*!< Pointer to FPGA Resource */
  struct _Resource *pDmaCtrl;   /*!< Pointer to DMA resource */
  struct _Resource *pFlashProgrammer;   /*!< Flash programmer resource */
  struct list_head	Resources;  /*!< Linked list of resources in extension */
  int    irq;                   /*!< IRQ Number for device */
  /*!
    \struct _ext_interrupts
    \brief Callback functions etc. for external interrupt support
  */
  struct _ext_interrupts {
    u32 Mask;       /*!< Registered external interrupt handlers mask */
    ExtInterruptISR ISRs[12];   /*!< Registered interrupt service routines */
    ExtInterruptHandler Handlers[12];   /*!< Registered interrupt handlers */
  } ExtInts;

  /*!
    \struct _bar_registers
    \brief details of the registers in each BAR
  */
  struct _bar_registers {
	u32	*regs;          /*!< Pointer to registers */
	u32 num_regs;       /*!< Number of registers in this BAR */
  } RegValue[4];

  struct kobject    kobject;

#ifndef USE_IRQ_THREAD
  struct tasklet_struct irq_tasklet;
#endif

} OMNITEK_INTERFACE_EXTENSION, *POMNITEK_INTERFACE_EXTENSION;

/*! 
  \struct     _GeneralCtrl
  \brief      Data structure for PCIE BAR
   \details   This contains basic information about each of the PCIE BARs for the device
 */
typedef struct _GeneralCtrl
{
  u8   Bar;             /*!< BAR Number */
  u32  RegisterOffset;  /*!< Offset of registers into BAR */
  bool Initialised;     /*!< Has this BAR been initialised */
} GeneralCtrl;

/*!
  \struct   _ResourceVersion
  \brief    Version details of a device resource
  \details  This contains the major and minor version number of the device resource.  Can be used to determine whether certain features are available
 */
typedef struct _ResourceVersion
{
  u8	Major;          /*!< Resource Major Version */
  u8	Minor;          /*!< Resource Minor Version */
} ResourceVersion;

/*!
  \struct   _FPGACtrl
  \brief    FPGA Control Resource 
  \details  
 */
typedef struct _FPGACtrl
{
  ResourceVersion Version;  /*!< Resource Version information */
  u32      		  FPGAType; /*!< FPGA Identification */
} FPGACtrl;


typedef struct _DmaSglEntry
{
  u32 PADR;
  u32 PADR_HIGH;
  u32 LADR;
  u32 LADR_HIGH;
  u32 DPR;
  u32 DPR_HIGH;
  u32 SIZE;
  u32 SIZE_HIGH;
} DmaSglEntry;

/*!
  \struct   _DmaSglBuffer
  \brief    DMA Scatter Gather List buffer
  \details  This is the common buffer (accessible by both driver and DMA device) that stores scatter gather entries.
 */
typedef struct _DmaSglBuffer
{
  struct _DmaSglEntry *CommonBuffer;  
                        /*!< Pointer to coherent DMA Memory for SGL Buffer */
  dma_addr_t DMA_Handle;
  size_t Size;          /*!< buffer size */
	
  /*!< 
    \struct _Allocated
    \details Details for SGL Buffer allocated in commonbuffer 
    */
  struct _Allocated
  {
    struct _DmaSglEntry *StartPos;  /*!< Start of allocated space */
    struct _DmaSglEntry *EndPos;    /*!< End of allocated space */
    bool Start_cycle;               /*!< Flags to track buffer full/empty */
    bool End_cycle;                 /*!< Flags to track buffer full/empty */
	} Allocated;                        
	spinlock_t SpinLock;            /*!< Spin lock for this buffer */
} DmaSglBuffer;

/*! \struct _OmniTekRequestQueue */
typedef struct _OmniTekRequestQueue {
  struct list_head Entries;         /*!< Queue Entries List */
  spinlock_t SpinLock;              /*!< Queue Spinlock */
  char  *Name;                      /*!< Queue Name (for Debug) */
} OmniTekRequestQueue, *POmniTekRequestQueue;

/*!
  \struct   _DmaChannel
  \brief    Data structure containing details of a DMA Channel Resource
  \details  This data structure contains the details of each individual DMA channel. 
 */
typedef struct _DmaChannel
{
#ifdef USE_FIXED_SGL_BUFFER
	DmaSglBuffer SglBuffer;           /*!< Buffer for scattergather entries */
#endif
  struct _OmniTekRequestQueue Active; /*!< Active Queue (max 1 entry for MDMA)*/
  struct _OmniTekRequestQueue Pending;/*!< Pending Queue */
  struct list_head Object;  /*!< Object for DmaCtrl channels */
  u8 Label;                 /*!< Channel label */
  u8 Index;                 /*!< Channel Index */
  char Desc[32];
  bool FDMA_Enabled;        /*!< Channel is FDMA capable */
  bool FDMA_Read;           /*!< Channel is FDMA Read */
  bool FDMA_Write;          /*!< Channel is FDMA Write */
  bool FDMA_Throttling;     /*!< Channel has FDMA throttling enabled */
  bool EFDMA_Polling;       /*!< Set if this FDMA Channel uses polling */
  bool Running;             /*!< Channel is Active*/
  bool Paused;
  bool DMA_64BIT_PADR;      /*!< Supports 64 bit PCIE addr (>4GB Host memory) */
  bool DMA_64BIT_LADR;      /*!< Supports 64 bit local addr (>4GB device memory) */
  bool FDMA_AutoStart;      /*!< If set channel will start automatically */
  struct work_struct ISRWork;   /*!< Work item for ISR */
  atomic_t nSgIrqs;         /*!< # outstanding completion interrupt requests */
  atomic_t nBytesXferred;   /*!< # bytes transferred for event interrupts*/
  atomic_t nBytesUnhandled; /*!< # bytes awaiting processing */
  atomic_t nEvents;         /*!< Events for this channel - for polling mode */

  dev_t dev;                /*!< Device number for this BAR */

  struct kobject kobject;   /*!< Kobject for sysfs */
  struct device *device;    /*!< Device entry in OmniTek class */

  void (*event_func)(struct _Resource * pResource, void * arg); 
                            /* Callback function for event handling */
  void *event_priv;         /* Pointer for event_func 'arg'*/

  void (*poll_func)(struct _Resource * pResource, void * arg);  
                            /* Callback for polled event mdoe */
  void *poll_priv;          /* Pointer for poll_func 'arg'*/

} DmaChannel, *PDmaChannel;

/*!
  \struct   _DmaCtrl
  \brief    DMA Control Resource
  \details  
 */
typedef struct _DmaCtrl
{
  struct list_head Channels;    /*!< channels associated with this controller */
  u8 nMDMA;                     /*!< Number of MDMA Channels */
  u8 nFDMARead;                 /*!< Number of FDMA read channels */
  u8 nFDMAWrite;                /*!< Number of FDMA write channels */
  u8 nFDMABoth;                 /*!< Number of FDMA bidir channels */
  u8 nChannels;                 /*!< Total number of channels */
  bool DMA_64BIT_PADR;          /*!< Channel supports 64 bit PCIE Addr (>4GB Host memory) */
  bool DMA_64BIT_LADR;          /*!< Channel supports 64 bit local Addr (>4GB device memory) */
  wait_queue_head_t DMA_Wait_Queue;
                                /*!< Wait queue for sleeping blocking operations */
  struct workqueue_struct *DMA_Work_Queue; /*!< Work queue for Completions */
  bool DmaInterrupts;           /*!< Set to indicate interrupt status is in DMA controller */

  struct kobject kobject;

  u32 ChannelStatus;            /*!< Interrupt status for channels */
} DmaCtrl, *PDmaCtrl;

/*!

  \struct   _Resource
  \brief    Generic resource Details
  \details  
 */
typedef struct _Resource
{
  ResourceType Type;
  GeneralCtrl General;
  spinlock_t SpinLock;
  struct list_head Object;
  u32 LockedBy;		        // 0 for unlocked
  u8 ReferenceCount;
  u8 CapVersion;
  u32 NumRegisters;
  u32 Identifier;
  /*!< 
    \union _ResourceExtension 
    The resource may be one of these types 
  */
  union _ResourceExtension    
  {
	DmaChannel		DmaChannel;
	DmaCtrl			DmaCtrl;
	FPGACtrl		FPGACtrl;
  } u;
  // Keep a copy of the parent to avoid having to pass it as a parameter 
  // all the time
  struct _OMNITEK_INTERFACE_EXTENSION *pExt;

  struct kobject  kobject;

} Resource, *PResource;

/*!
 \def SUCCESS
 \brief sucess macro: returns true when provided status is >= 0
*/
#define SUCCESS(status) ((status) >= 0)
