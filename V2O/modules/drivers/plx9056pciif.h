#ifndef __PLX9056_H_INCLUDED__
#define __PLX9056_H_INCLUDED__

/*******************************************************************************
 * Copyright (c) 2005 PLX Technology, Inc.
 * 
 * PLX Technology Inc. licenses this software under specific terms and
 * conditions.  Use of any of the software or derviatives thereof in any
 * product without a PLX Technology chip is strictly prohibited. 
 * 
 * PLX Technology, Inc. provides this software AS IS, WITHOUT ANY WARRANTY,
 * EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  PLX makes no guarantee
 * or representations regarding the use of, or the results of the use of,
 * the software and documentation in terms of correctness, accuracy,
 * reliability, currentness, or otherwise; and you rely on the software,
 * documentation and results solely at your own risk.
 * 
 * IN NO EVENT SHALL PLX BE LIABLE FOR ANY LOSS OF USE, LOSS OF BUSINESS,
 * LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES
 * OF ANY KIND.  IN NO EVENT SHALL PLX'S TOTAL LIABILITY EXCEED THE SUM
 * PAID TO PLX FOR THE PRODUCT LICENSED HEREUNDER.
 * 
 ******************************************************************************/

#define PCI_CODE

#if defined(PCI_CODE)
#define PCI9056_REG_BASE         0x000
#define PCI9056_REG_BASE_1       0x000
#define PCI9056_NEW_CAP_BASE     0x000
#elif defined(PLX_IOP_CODE)
#define PCI9056_REG_BASE         0x080
#define PCI9056_REG_BASE_1       0x0a0
#define PCI9056_NEW_CAP_BASE     0x140
#endif


// PCI offsets for Type 0 Header
#define CFG_VENDOR_ID           0x000
#define CFG_COMMAND             0x004
#define CFG_REV_ID              0x008
#define CFG_CACHE_SIZE          0x00C
#define CFG_BAR0                0x010
#define CFG_BAR1                0x014
#define CFG_BAR2                0x018
#define CFG_BAR3                0x01C
#define CFG_BAR4                0x020
#define CFG_BAR5                0x024
#define CFG_CIS_PTR             0x028
#define CFG_SUB_VENDOR_ID       0x02C
#define CFG_EXP_ROM_BASE        0x030
#define CFG_CAP_PTR             0x034
#define CFG_RESERVED1           0x038
#define CFG_INT_LINE            0x03C


// PCI Extended Capability IDs
#define CAP_ID_POWER_MAN        0x01
#define CAP_ID_AGP              0x02
#define CAP_ID_VPD              0x03
#define CAP_ID_SLOT_ID          0x04
#define CAP_ID_MSI              0x05
#define CAP_ID_HOT_SWAP         0x06
#define CAP_ID_PCIX             0x07
#define CAP_ID_HYPER_TRANSPORT  0x08
#define CAP_ID_VENDOR_SPECIFIC  0x09
#define CAP_ID_DEBUG_PORT       0x0A
#define CAP_ID_RESOURCE_CTRL    0x0B
#define CAP_ID_HOT_PLUG         0x0C
#define CAP_ID_BRIDGE_SUB_ID    0x0D
#define CAP_ID_AGP_8X           0x0E
#define CAP_ID_SECURE_DEVICE    0x0F
#define CAP_ID_PCI_EXPRESS      0x10
#define CAP_ID_MSI_X            0x11


// PCI Express Extended Capability IDs
#define PCIE_CAP_ID_ERROR_REPORTING         0x001
#define PCIE_CAP_ID_VIRTUAL_CHANNEL         0x002
#define PCIE_CAP_ID_DEV_SERIAL_NUMBER       0x003
#define PCIE_CAP_ID_POWER_BUDGETING         0x004
#define PCIE_CAP_ID_RC_LINK_DECLARATION     0x005
#define PCIE_CAP_ID_RC_INT_LINK_CONTROL     0x006
#define PCIE_CAP_ID_RC_EVENT_COLLECTOR      0x007
#define PCIE_CAP_ID_MF_VIRTUAL_CHANNEL      0x008
#define PCIE_CAP_ID_VC_WITH_MULTI_FN        0x009
#define PCIE_CAP_ID_RC_REG_BLOCK_HEADER     0x00A
#define PCIE_CAP_ID_VENDOR_SPECIFIC         0x00B


// PLX 9056 Additional Defintions
#define PCI9056_MAX_REG_OFFSET       (PCI9056_REG_BASE_1 + 0x108)
#define PCI9056_EEPROM_SIZE   0x064  // EEPROM size (bytes) used by PLX Chip
#define PCI9056_DMA_CHANNELS  2      // # DMA channels supported by PLX Chip


// PLX 9056 PCI Configuration Registers
#define PCI9056_VENDOR_ID            CFG_VENDOR_ID
#define PCI9056_COMMAND              CFG_COMMAND
#define PCI9056_REV_ID               CFG_REV_ID
#define PCI9056_CACHE_SIZE           CFG_CACHE_SIZE
#define PCI9056_RTR_BASE             CFG_BAR0
#define PCI9056_RTR_IO_BASE          CFG_BAR1
#define PCI9056_LOCAL_BASE0          CFG_BAR2
#define PCI9056_LOCAL_BASE1          CFG_BAR3
#define PCI9056_UNUSED_BASE1         CFG_BAR4
#define PCI9056_UNUSED_BASE2         CFG_BAR5
#define PCI9056_CIS_PTR              CFG_CIS_PTR
#define PCI9056_SUB_ID               CFG_SUB_VENDOR_ID
#define PCI9056_EXP_ROM_BASE         CFG_EXP_ROM_BASE
#define PCI9056_CAP_PTR              CFG_CAP_PTR
#define PCI9056_RESERVED2            CFG_RESERVED1
#define PCI9056_INT_LINE             CFG_INT_LINE
#define PCI9056_PM_CAP_ID            (PCI9056_NEW_CAP_BASE + 0x040)
#define PCI9056_PM_CSR               (PCI9056_NEW_CAP_BASE + 0x044)
#define PCI9056_HS_CAP_ID            (PCI9056_NEW_CAP_BASE + 0x048)
#define PCI9056_VPD_CAP_ID           (PCI9056_NEW_CAP_BASE + 0x04c)
#define PCI9056_VPD_DATA             (PCI9056_NEW_CAP_BASE + 0x050)


// PLX 9056 Local Configuration Registers
#define PCI9056_SPACE0_RANGE         (PCI9056_REG_BASE   + 0x000)
#define PCI9056_SPACE0_REMAP         (PCI9056_REG_BASE   + 0x004)
#define PCI9056_LOCAL_DMA_ARBIT      (PCI9056_REG_BASE   + 0x008)
#define PCI9056_ENDIAN_DESC          (PCI9056_REG_BASE   + 0x00c)
#define PCI9056_EXP_ROM_RANGE        (PCI9056_REG_BASE   + 0x010)
#define PCI9056_EXP_ROM_REMAP        (PCI9056_REG_BASE   + 0x014)
#define PCI9056_SPACE0_ROM_DESC      (PCI9056_REG_BASE   + 0x018)
#define PCI9056_DM_RANGE             (PCI9056_REG_BASE   + 0x01c)
#define PCI9056_DM_MEM_BASE          (PCI9056_REG_BASE   + 0x020)
#define PCI9056_DM_IO_BASE           (PCI9056_REG_BASE   + 0x024)
#define PCI9056_DM_PCI_MEM_REMAP     (PCI9056_REG_BASE   + 0x028)
#define PCI9056_DM_PCI_IO_CONFIG     (PCI9056_REG_BASE   + 0x02c)
#define PCI9056_SPACE1_RANGE         (PCI9056_REG_BASE   + 0x0f0)
#define PCI9056_SPACE1_REMAP         (PCI9056_REG_BASE   + 0x0f4)
#define PCI9056_SPACE1_DESC          (PCI9056_REG_BASE   + 0x0f8)
#define PCI9056_DM_DAC               (PCI9056_REG_BASE   + 0x0fc)
#define PCI9056_ARBITER_CTRL         (PCI9056_REG_BASE_1 + 0x100)
#define PCI9056_ABORT_ADDRESS        (PCI9056_REG_BASE_1 + 0x104)


// PLX 9056 Runtime Registers
#if defined(PCI_CODE)
#define PCI9056_MAILBOX0         0x078
#define PCI9056_MAILBOX1         0x07c
#elif defined(PLX_IOP_CODE)
#define PCI9056_MAILBOX0         0x0c0
#define PCI9056_MAILBOX1         0x0c4
#endif

#define PCI9056_MAILBOX2             (PCI9056_REG_BASE + 0x048)
#define PCI9056_MAILBOX3             (PCI9056_REG_BASE + 0x04c)
#define PCI9056_MAILBOX4             (PCI9056_REG_BASE + 0x050)
#define PCI9056_MAILBOX5             (PCI9056_REG_BASE + 0x054)
#define PCI9056_MAILBOX6             (PCI9056_REG_BASE + 0x058)
#define PCI9056_MAILBOX7             (PCI9056_REG_BASE + 0x05c)
#define PCI9056_LOCAL_DOORBELL       (PCI9056_REG_BASE + 0x060)
#define PCI9056_PCI_DOORBELL         (PCI9056_REG_BASE + 0x064)
#define PCI9056_INT_CTRL_STAT        (PCI9056_REG_BASE + 0x068)
#define PCI9056_EEPROM_CTRL_STAT     (PCI9056_REG_BASE + 0x06c)
#define PCI9056_PERM_VENDOR_ID       (PCI9056_REG_BASE + 0x070)
#define PCI9056_REVISION_ID          (PCI9056_REG_BASE + 0x074)


// PLX 9056 DMA Registers
#define PCI9056_DMA0_MODE            (PCI9056_REG_BASE + 0x080)
#define PCI9056_DMA0_PCI_ADDR        (PCI9056_REG_BASE + 0x084)
#define PCI9056_DMA0_LOCAL_ADDR      (PCI9056_REG_BASE + 0x088)
#define PCI9056_DMA0_COUNT           (PCI9056_REG_BASE + 0x08c)
#define PCI9056_DMA0_DESC_PTR        (PCI9056_REG_BASE + 0x090)
#define PCI9056_DMA1_MODE            (PCI9056_REG_BASE + 0x094)
#define PCI9056_DMA1_PCI_ADDR        (PCI9056_REG_BASE + 0x098)
#define PCI9056_DMA1_LOCAL_ADDR      (PCI9056_REG_BASE + 0x09c)
#define PCI9056_DMA1_COUNT           (PCI9056_REG_BASE + 0x0a0)
#define PCI9056_DMA1_DESC_PTR        (PCI9056_REG_BASE + 0x0a4)
#define PCI9056_DMA_COMMAND_STAT     (PCI9056_REG_BASE + 0x0a8)
#define PCI9056_DMA_ARBIT            (PCI9056_REG_BASE + 0x0ac)
#define PCI9056_DMA_THRESHOLD        (PCI9056_REG_BASE + 0x0b0)
#define PCI9056_DMA0_PCI_DAC         (PCI9056_REG_BASE + 0x0b4)
#define PCI9056_DMA1_PCI_DAC         (PCI9056_REG_BASE + 0x0b8)


// PLX 9056 Messaging Unit Registers
#define PCI9056_OUTPOST_INT_STAT     (PCI9056_REG_BASE + 0x030)
#define PCI9056_OUTPOST_INT_MASK     (PCI9056_REG_BASE + 0x034)
#define PCI9056_MU_CONFIG            (PCI9056_REG_BASE + 0x0c0)
#define PCI9056_FIFO_BASE_ADDR       (PCI9056_REG_BASE + 0x0c4)
#define PCI9056_INFREE_HEAD_PTR      (PCI9056_REG_BASE + 0x0c8)
#define PCI9056_INFREE_TAIL_PTR      (PCI9056_REG_BASE + 0x0cc)
#define PCI9056_INPOST_HEAD_PTR      (PCI9056_REG_BASE + 0x0d0)
#define PCI9056_INPOST_TAIL_PTR      (PCI9056_REG_BASE + 0x0d4)
#define PCI9056_OUTFREE_HEAD_PTR     (PCI9056_REG_BASE + 0x0d8)
#define PCI9056_OUTFREE_TAIL_PTR     (PCI9056_REG_BASE + 0x0dc)
#define PCI9056_OUTPOST_HEAD_PTR     (PCI9056_REG_BASE + 0x0e0)
#define PCI9056_OUTPOST_TAIL_PTR     (PCI9056_REG_BASE + 0x0e4)
#define PCI9056_FIFO_CTRL_STAT       (PCI9056_REG_BASE + 0x0e8)


// PLX 9000-series EEPROM definitions
#define EE_CMD_LEN      11      // EE56 bits in instructions 
#define EE_CMD_SHIFT    2       // EE56 command bit shift in instructions
#define EE_READ         0x0180  // 01 1000 0000 read instruction
#define EE_WRITE        0x0140  // 01 0100 0000 write instruction
#define EE_WREN         0x0130  // 01 0011 0000 write enable instruction
#define EE_WRALL        0x0110  // 01 0001 0000 write all registers
#define EE_PRREAD       0x0180  // 01 1000 0000 read address stored in Prot Reg
#define EE_PRWRITE      0x0140  // 01 0100 0000 write the address into PR
#define EE_WDS          0x0100  // 01 0000 0000 write disable instruction
#define EE_PREN         0x0130  // 01 0011 0000 protect enable instruction
#define EE_PRCLEAR      0x01FF  // 01 1111 1111 clear protect register instr
#define EE_PRDS         0x0100  // 01 0000 0000 ONE TIME ONLY, permenant 


// PCI9056 Capabilities 
#define PCI9056_CAPABILITY_PM           0x01
#define PCI9056_CAPABILITY_HOTSWAP      0x02
#define PCI9056_CAPABILITY_VPD          0x04


// Used for VPD access
#if defined(PLX_IOP_CODE)
#define VPD_COMMAND_MAX_RETRIES     10
#define VPD_STATUS_MAX_POLL         10000
#define VPD_STATUS_POLL_DELAY       500
#else
#define VPD_COMMAND_MAX_RETRIES     5
#define VPD_STATUS_MAX_POLL         10
#define VPD_STATUS_POLL_DELAY       5
#endif


// PLX 9056 ID
#define PCI9056_ID                      0x560110b5              

// Burst modes
#define BURST_MODE_NOBURST              0x00
#define BURST_MODE_BURST4               0x01
#define BURST_MODE_CONTINUOUS           0x02

#ifdef  __KERNEL__

#ifndef  __FPGA_H_INCLUDED__
#include "fpga.h"
#endif

// PLX 9056 interrupt identifiers
#define PLX_INT_0_PCI_MAIN              10
#define PLX_INT_1_LOCAL                 11
#define PLX_INT_2_PCI_ABORT             12
#define PLX_INT_3_DOORBELL              13
#define PLX_INT_4_DMA_0                 14
#define PLX_INT_5_DMA_1                 15

#define PLX_INT_TYPE_NONE               0
#define PLX_INT_TYPE_PCI_MAIN           (1 << PLX_INT_0_PCI_MAIN)
#define PLX_INT_TYPE_LOCAL              (1 << PLX_INT_1_LOCAL)
#define PLX_INT_TYPE_PCI_ABORT          (1 << PLX_INT_2_PCI_ABORT)      
#define PLX_INT_TYPE_DOORBELL           (1 << PLX_INT_3_DOORBELL)
#define PLX_INT_TYPE_DMA_0              (1 << PLX_INT_4_DMA_0)
#define PLX_INT_TYPE_DMA_1              (1 << PLX_INT_5_DMA_1)

// PLX 9056 DMA channel priority definitions
#define CHANNEL0_HIGHEST                0
#define CHANNEL1_HIGHEST                1
#define ROTATIONAL                      2

// PLX 9056 DMA transfer mode
#define ASSERT_BLAST                    0
#define EOT_ASSERTED                    1


// PLX 9056 DMA call return status (in addition to OK and ERROR)
#define PLX_DMA_DONE                    1
#define PLX_DMA_INPROGRESS              2
#define PLX_DMA_PAUSED                  3
#define PLX_DMA_NOTPAUSED               4
#define PLX_DMA_CHANNEL_CLOSED          5
#define PLX_DMA_CHANNEL_UNAVAILABLE     6
#define PLX_DMA_CHANNEL_INVALID         7
#define PLX_DMA_CHANNEL_TYPE_ERROR      8
#define PLX_DMA_COMMAND_INVALID         9
#define PLX_DMA_INVALID_PRIORITY        10
#define PLX_DMA_SGL_INVALID             11
#define PLX_DMA_SGL_BUILD_FAILED        12
#define PLX_DMA_SGL_QUEUE_FULL          13
#define PLX_DMA_CHANNEL_ERROR           14

// Function prototypes
extern int plx_control_init(struct v2d_board *board);
extern void plx_control_exit(struct v2d_board *board);
extern int plx_control_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data);

extern void plx_interrupt_enable(struct v2d_board *board, int intr);
extern void plx_interrupt_disable(struct v2d_board *board, int intr);

#if 1
extern int plx_block_dma_channel_open(struct v2d_board *board, int cno);
#else
extern int plx_block_dma_channel_open(struct v2d_board *board, 
                                      int cno, 
                                      struct dma_desc *desc);
#endif
extern int plx_block_dma_channel_close(struct v2d_board *board, int cno);
extern int plx_block_dma_inprogress(struct v2d_board *board, int cno);
extern int plx_block_dma_transfer(struct v2d_board *board, 
                                  int cno, 
                                  unsigned long pci_addr_lo,
                                  unsigned long local_addr,
                                  unsigned long transfer_count,
                                  unsigned char direction);
extern int plx_capability_enabled(struct v2d_board *board, int capability);
extern int plx_vpd_read(struct v2d_board *board, unsigned long offset, unsigned long *value);
extern int plx_vpd_write(struct v2d_board *board, unsigned long offset, unsigned long value);



#endif /*__KERNEL__*/

#endif /*__PLX9056_H_INCLUDED__*/

