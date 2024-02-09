/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        pci_7008.h
*
*    DESCRIPTION:
*        
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#ifndef _PCI_7008_H
#define _PCI_7008_H

typedef ULONG DWORD;
// Header file for WIS 7007SB PCI controller.

/*********************************************************************************************/
// PBI Command Register
/*---------------------------------------------------------------------------------------------
// 31:8 	Reserved	For future use.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_PBI_COMMAND_REGISTER_RESERVED0                                    31:8

/*---------------------------------------------------------------------------------------------
7:6	Frame Skip Threshold
(WC & 656 mode only)	Frame Skip Threshold is a function of Number of Consecutive Page Skips.
00: 2 consecutive Page skip occurrence.
01: 3 consecutive Page skip occurrence.
10: 4 consecutive Page skip occurrence.
11: Rsvd
Under Frame Skip condition, DMA flushes the existing Fifo Data, keeps accepting subsequent 
incoming Data from Sensor Block but drops it only the floor until DMA is reprogrammed (Start 
Bit is Set again) , logs Frame Skip Occurrence in Status Register, optionally generates a 
Frame Skip Interrupt if Enabled, optionally notifies the Software by writing the contents of 
the Status Register at specified location in CPU (External) Memory.
DMA Reprogramming is required to commence Normal Operation after Frame Skip Threshold.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_PBI_COMMAND_REGISTER_FRAME_SKIP_THRESHOLD                          7:6
#define WIS_SBPCI_PBI_COMMAND_REGISTER_FRAME_SKIP_THRESHOLD_2_PAGE                   0x00
#define WIS_SBPCI_PBI_COMMAND_REGISTER_FRAME_SKIP_THRESHOLD_3_PAGE                   0x01
#define WIS_SBPCI_PBI_COMMAND_REGISTER_FRAME_SKIP_THRESHOLD_4_PAGE                   0x02
#define WIS_SBPCI_PBI_COMMAND_REGISTER_FRAME_SKIP_THRESHOLD_RSVD                     0x03

/*---------------------------------------------------------------------------------------------
5:4	Page Skip Threshold
(WC & 656 mode only)	Page Skip Threshold is a function of Write Channel Fifo Fill level.
00: 3/4 full
01: 1/2 full
10: 7/8 full
11: Rsvd.
Once the programmed threshold level of Fifo Fill is reached, DMA flushes the Fifo data, 
logs Page Skip Occurrence in Status Register, optionally generates an Page Skip Interrupt 
if enabled, optionally notifies the Software by writing the contents of Status Register at 
specified location in CPU (External) memory and Starts reading the subsequent descriptor 
corresponding to the Next Page of the Frame.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_PBI_COMMAND_REGISTER_PAGE_SKIP_THRESHOLD                           5:4
#define WIS_SBPCI_PBI_COMMAND_REGISTER_PAGE_SKIP_THRESHOLD_3_4THS_FULL               0x00
#define WIS_SBPCI_PBI_COMMAND_REGISTER_PAGE_SKIP_THRESHOLD_1_HALF_FULL               0x01
#define WIS_SBPCI_PBI_COMMAND_REGISTER_PAGE_SKIP_THRESHOLD_7_8THS_FULL               0x02
#define WIS_SBPCI_PBI_COMMAND_REGISTER_PAGE_SKIP_THRESHOLD_DISABLED                  0x03

/*---------------------------------------------------------------------------------------------
3	Terminate	1'b1: Kill or Terminate the current DMA transfer after the most recent 
descriptor read or Data Write/Read transfer is complete.
1'b0: Normal operation
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_PBI_COMMAND_REGISTER_TERMINATE                                     3:3
#define WIS_SBPCI_PBI_COMMAND_REGISTER_TERMINATE_NO                                  0x0
#define WIS_SBPCI_PBI_COMMAND_REGISTER_TERMINATE_YES                                 0x1

/*---------------------------------------------------------------------------------------------
2:1	MTU Selection	
00: 16 Words (64B)
01: 8   Words (32B)
10: 32 Words (128B)
11: Rsvd 
MTU Selection intended to assist PCI Bandwidth sharing depending on the Read & Write Channel 
traffic.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_PBI_COMMAND_REGISTER_MTU_SELECTION                                 2:1
#define WIS_SBPCI_PBI_COMMAND_REGISTER_MTU_SELECTION_16_WORDS                        0x00
#define WIS_SBPCI_PBI_COMMAND_REGISTER_MTU_SELECTION_08_WORDS                        0x01
#define WIS_SBPCI_PBI_COMMAND_REGISTER_MTU_SELECTION_32_WORDS                        0x02
#define WIS_SBPCI_PBI_COMMAND_REGISTER_MTU_SELECTION_RSVD                            0x03

/*---------------------------------------------------------------------------------------------
0	Run	1'b1: Commences DMA Operation from where it paused or stopped.
1'b0:  Pauses DMA Operation. DMA goes into Pause State after it completes the current transfer
i.e. either after most recent descriptor read is complete or most recent MTU Data Read is 
complete. 
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_PBI_COMMAND_REGISTER_RUN                                           0:0
#define WIS_SBPCI_PBI_COMMAND_REGISTER_RUN_GO                                        0x1
#define WIS_SBPCI_PBI_COMMAND_REGISTER_RUN_PAUSE                                     0x0
/*********************************************************************************************/


/*********************************************************************************************/
//Head Buffer Descriptor Address Register
/*---------------------------------------------------------------------------------------------
Bit	Name	Description
31:4	Head Descriptor Address	Physical Address of the very first Descriptor of the Linked 
List in CPU (External) Memory.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_HEAD_BUFFER_DESCRIPTOR_ADDRESS_REGISTER_PHYS_ADDR                  31:4

/*---------------------------------------------------------------------------------------------
3:0	4'h0	Descriptor Address should be 16B aligned.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_HEAD_BUFFER_DESCRIPTOR_ADDRESS_REGISTER_ALIGNMENT                  3:0
/*********************************************************************************************/


/*********************************************************************************************/
//Status (Notify) Register
/*---------------------------------------------------------------------------------------------
Bit	Name	Description
31:16	Number of Words (Word=4B)  Transferred	Total Number of Successfully Transferred Data 
Words (Word=4B). This field is set once MTU data is drained either into SB or on PCI bus.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_NUM_WORDS                                   31:16

/*---------------------------------------------------------------------------------------------
15:6	Page ID	ID of the Current Page for which DMA is executing Data Transfer.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_PAGE_ID                                     15:6

/*---------------------------------------------------------------------------------------------
5	Skip Frame (WC & 656 mode only)	Indicates Skip Frame Occurrence
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_SKIP_FRAME                                   5:5
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_SKIP_FRAME_NO                                0x00
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_SKIP_FRAME_YES                               0x01

/*---------------------------------------------------------------------------------------------
4	Skip Page (WC & 656 mode only)	Indicates Skip Page Occurrence
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_SKIP_PAGE                                    4:4
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_SKIP_PAGE_NO                                 0x00
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_SKIP_PAGE_YES                                0x01

/*---------------------------------------------------------------------------------------------
3	TimeOut Occurred	PCI Descriptor or Data Read Timed Out
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_TIMEOUT                                      3:3
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_TIMEOUT_NO                                   0x00
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_TIMEOUT_YES                                  0x01

/*---------------------------------------------------------------------------------------------
2	Transaction Aborted	Indicates that the Current DMA Transfer was Aborted because of either 
a Target Abort, SERR or PERR.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_ABORTED                                      2:2 
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_ABORTED_NO                                   0x00
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_ABORTED_YES                                  0x01

/*---------------------------------------------------------------------------------------------
1	Page Done	Indicates Transfer of Current Page Buffer is Complete.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_PAGE_DONE                                    1:1 
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_PAGE_DONE_NO                                 0x00
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_PAGE_DONE_YES                                0x01

/*---------------------------------------------------------------------------------------------
0	Busy	DMA is in the middle of Data Transfer.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_BUSY                                         0:0 
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_BUSY_NO                                      0x00
#define WIS_SBPCI_STATUS_NOTIFY_REGISTER_BUSY_YES                                     0x01
/*********************************************************************************************/

/*********************************************************************************************/
// TimeOut Value Register
/*---------------------------------------------------------------------------------------------
Bit	Name	Description
31:0	Read TimeOut Value	Maximum Number of 33MHz PCI Clock Cycles allocated for a Descriptor or Data Read Request to complete on PCI. If Read on PCI doesn't complete within the specified value, RC & WC treats it as a Read TimeOut Error, logs this error in Status register and optionally generates an Interrupt. Reprogramming of DMA engines is required for such a timeout condition.
Default Value : 32'h0000ffff
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_TIMEOUT_REGISTER_VALUE                                              31:0
/*********************************************************************************************/

/*********************************************************************************************/
// ASIC Memory Address Register
/*---------------------------------------------------------------------------------------------
Bit	Name	Description
31:16	Burst Size	Burst Length in number of Words (Word=4B) used by HPI/VIP -> PCI Translator
 block (Neil's Block) to execute burst Read from or Write into SB.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_ASIC_MEMORY_REGISTER_BURSTSIZE                                       31:16
/*---------------------------------------------------------------------------------------------
15:0	Starting Address	Starting Address to Read from or Write into the Buffer in SB.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_ASIC_MEMORY_REGISTER_START_ADDRESS                                    15:0
/*********************************************************************************************/

/*********************************************************************************************/
// Frame Size Register
/*---------------------------------------------------------------------------------------------
Bit	Name	Description
31:0	Frame Size	Total Size of the Frame that is being Read from SB Memory and Written into 
PCI System Memory. This is the only mechanism by which the HPI/VIP -> PCI Translator block 
(Neil's Block) could determine an End of Frame during Read SB Memory and Write  System Memory 
transfer.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_FRAME_SIZE_REGISTER_VALUE                                             31:0
/*********************************************************************************************/

/*********************************************************************************************/
// Group A Interrupt Status Register
/*---------------------------------------------------------------------------------------------
Bit	Name	Description
31:6	Reserved	
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER_RSVD                                   31:6
/*---------------------------------------------------------------------------------------------
5	MA	Master Abort Response received on PCI bus
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER_MASTER_ABORT                            5:5
/*---------------------------------------------------------------------------------------------
4	TA	Target Abort Response received on PCI bus.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER_TARGET_ABORT                            4:4
/*---------------------------------------------------------------------------------------------
3	TRDYERR	Addressed target inserts more wait states than programmed TRDY COUNT.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER_TRDYERR                                 3:3
/*---------------------------------------------------------------------------------------------
2	RETRYERR	Number of Retries on PCI bus exceeded the RETRY COUNT Limit.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER_RETRYERR                                2:2
/*---------------------------------------------------------------------------------------------
1	SERR	Indicates that System Error occurred on PCI bus.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER_SERR                                    1:1
/*---------------------------------------------------------------------------------------------
0	PERR 	Indicates that Parity Error occurred on PCI bus.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER_PERR                                    0:0
/*********************************************************************************************/

/*********************************************************************************************/
// Group B Interrupt Status Register
/*---------------------------------------------------------------------------------------------
Bit	Name	Description
31:22	RSVD	Reserved for WC2 & WC3.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RSVD0                                           31:22
/*---------------------------------------------------------------------------------------------
21	FRMSKIPINT	WC1 DMA Frame Skip Occurrence Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC1_DMA_FRAME_SKIP                              21:21
/*---------------------------------------------------------------------------------------------
20	PGSKIPINT	WC1 DMA Page Skip Occurrence Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC1_DMA_PAGE_SKIP                               20:20
/*---------------------------------------------------------------------------------------------
19	TOUTINT	WC1 DMA Descriptor or Data Read Timed Out on PCI bus Interrupt.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC1_DMA_DESC_TIMEOUT                            19:19
/*---------------------------------------------------------------------------------------------
18	ABTINT	WC1 DMA Abort or SERR Response Received Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC1_DMA_ABORT                                   18:18
/*---------------------------------------------------------------------------------------------
17	PGCMPINT	WC1 DMA Page Buffer Write Complete Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC1_DMA_PAGE_BUFFER_WRITE_COMPLETE              17:17
/*---------------------------------------------------------------------------------------------
16	FRMSKIPINT	WC0 DMA Frame Skip Occurrence Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC0_DMA_FRAME_SKIP                              16:16
/*---------------------------------------------------------------------------------------------
15	PGSKIPINT	WC0 DMA Page Skip Occurrence Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC0_DMA_PAGE_SKIP                               15:15
/*---------------------------------------------------------------------------------------------
14	TOUTINT	WC0 DMA Descriptor or Data Read Timed Out on PCI bus Interrupt.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC0_DMA_DESC_TIMEOUT                            14:14
/*---------------------------------------------------------------------------------------------
13	ABTINT	WC0 DMA Abort or SERR Response Received Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC0_DMA_ABORT                                   13:13
/*---------------------------------------------------------------------------------------------
12	PGCMPINT	WC0 DMA Page Buffer Write Complete Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC0_DMA_PAGE_BUFFER_WRITE_COMPLETE              12:12
/*---------------------------------------------------------------------------------------------
11:6	RSVD	Reserved for RC2 & RC3.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RSVD1                                           11:6
/*---------------------------------------------------------------------------------------------
5	TOUTINT	RC1 DMA Descriptor or Data Read Timed Out on PCI bus Interrupt.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RC1_DMA_DESC_TIMEOUT                             5:5
/*---------------------------------------------------------------------------------------------
4	ABTINT	RC1 DMA Abort or SERR Response Received Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RC1_DMA_ABORT                                    4:4
/*---------------------------------------------------------------------------------------------
3	PGCMPINT	RC1 DMA Page Buffer Read Complete Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RC1_DMA_PAGE_BUFFER_READ_COMPLETE                3:3
/*---------------------------------------------------------------------------------------------
2	TOUTINT	RC0 DMA Descriptor or Data Read Timed Out on PCI bus Interrupt.
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RC0_DMA_DESC_TIMEOUT                             2:2
/*---------------------------------------------------------------------------------------------
1	ABTINT	RC0 DMA Abort or SERR Response Received Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RC0_DMA_ABORT                                    1:1
/*---------------------------------------------------------------------------------------------
0	PGCMPINT	RC0 DMA Page Buffer Read Complete Interrupt
---------------------------------------------------------------------------------------------*/
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_RC0_DMA_PAGE_BUFFER_READ_COMPLETE                0:0
/*********************************************************************************************/

/*********************************************************************************************/
// DMA Descriptor

// Defines used for the control block.                                                                 
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_LAST_PAGE                              0:0 
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_LAST_PAGE_NO                           0x0
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_LAST_PAGE_YES                          0x1
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_INTERRUPT                              1:1 
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_INTERRUPT_DISABLE                      0x0
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_INTERRUPT_ENABLE                       0x1
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_NOTIFY                                 2:2 
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_NOTIFY_DISABLE                         0x0
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_NOTIFY_ENABLE                          0x1
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_STOP                                   3:3
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_STOP_NO                                0x0
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_STOP_YES                               0x1
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_RSVD                                   5:4
#define WIS_SBPCI_DMA_DESCRIPTOR_CONTROL_BLOCK_PAGE_ID                               15:6

typedef struct _WIS_SBPCI_DMA_DESCRIPTOR {
    /*---------------------------------------------------------------------------------------------
    --Call this the Control block for now--
    0	LPoF	Indicates that Current Buffer contains Last Page of the Most Recent Frame.
    1	IE	Interrupt Enable: Generate an End of Page or End of Frame Interrupt once the current buffer transfer is complete.
    2	Notify Enable	Write the contents of Status Register to the specified Notifier Address in CPU (External) memory.
    3	Stop	Stop the DMA Transfer after Current Buffer Transfer is complete. Wait for Reprogramming of DMA to proceed.
    5:4	Reserved	
    15:6	Page ID	Identification number for this page.
    ---------------------------------------------------------------------------------------------*/
    USHORT dma_control_block;

    /*---------------------------------------------------------------------------------------------
    31:16	Page Size	Size of this page (in Words, Word=4B)
    ---------------------------------------------------------------------------------------------*/
    USHORT dma_page_size;

    /*---------------------------------------------------------------------------------------------
    63:32	Buffer Address	Physical Address of the Data Buffer in System (External) memory. 
    Address should be Word (4B) aligned ([33:32]=00)
    ---------------------------------------------------------------------------------------------*/
    ULONG  dma_buffer_address;

    /*---------------------------------------------------------------------------------------------
    95:64	Next Descriptor Address	Physical Address of the Next Descriptor in the Linked List in 
    System (External) memory. Address should be Word (4B) aligned ([65:64]=00)
    ---------------------------------------------------------------------------------------------*/
    ULONG  dma_next_descriptor_address;

    /*---------------------------------------------------------------------------------------------
    127:96	Notifier Address	Write the contents of the Status Register to this Address in 
    System (External) memory. Address should be Word (4B) aligned ([97:96]=00)
    ---------------------------------------------------------------------------------------------*/
    ULONG  dma_notifier_address;

}WIS_SBPCI_DMA_DESCRIPTOR, *PWIS_SBPCI_DMA_DESCRIPTOR;
/*********************************************************************************************/

typedef struct _WIS_SBPCI_DEVICE {
//0x00	group a interrupt status register(isr)
    ULONG wis_sbpci_grpa_interrupt_status_register   ;
//0x04	group a interrupt control register(icr)
    ULONG wis_sbpci_grpa_interrupt_control_register  ;
//0x08	group b isr                                                                      
    ULONG wis_sbpci_grpb_interrupt_status_register   ;
//0x0c	group b icr
    ULONG wis_sbpci_grpb_interrupt_control_register  ;
//0x10	rc0 asic memory address register 
    ULONG wis_sbpci_rc0_asic_memory_register         ;
//0x14	rc1 asic memory address register 
    ULONG wis_sbpci_rc1_asic_memory_register         ;
//0x18	wc0 asic memory address register 
    ULONG wis_sbpci_wc0_asic_memory_register         ;
//0x1C	wc0 frame size register
    ULONG wis_sbpci_wc0_frame_size_register          ;
//0x20	wc1 asic memory address register 
    ULONG wis_sbpci_wc1_asic_memory_register         ;
//0x24	wc1 frame size register
    ULONG wis_sbpci_wc1_frame_size_register          ;
//0x28-0x3c	reserved
    ULONG rsvd0[0x18/4]                              ;
//0x40	rc0 dma command register
    ULONG wis_sbpci_rc0_pbi_command_register         ;
//0x44	rc0 dma head descriptor address register
    ULONG wis_sbpci_rc0_head_buffer_descriptor_address_register  ;
//0x48	rc0 dma status register
    ULONG wis_sbpci_rc0_status_notify_register                   ;
//0x4c	rc0 dma timeout value register
    ULONG wis_sbpci_rc0_timeout_register                         ;
//0x50	rc0 dma descriptor[31:0]
    ULONG wis_sbpci_rc0_dma_descriptor0                          ;
//0x54	rc0 dma descriptor[63:32]
    ULONG wis_sbpci_rc0_dma_descriptor1                          ;
//0x58	rc0 dma descriptor[95:64]
    ULONG wis_sbpci_rc0_dma_descriptor2                          ;
//0x5c	rc0 dma descriptor[127:96]
    ULONG wis_sbpci_rc0_dma_descriptor3                          ;
//0x60	rc1 dma command register
    ULONG wis_sbpci_rc1_pbi_command_register                     ;
//0x64	rc1 dma head descriptor address register
    ULONG wis_sbpci_rc1_head_buffer_descriptor_address_register  ;
//0x68	rc1 dma status register
    ULONG wis_sbpci_rc1_status_notify_register                   ;
//0x6c	rc1 dma timeout value register
    ULONG wis_sbpci_rc1_timeout_register                         ;
//0x70	rc1 dma descriptor[31:0]
    ULONG wis_sbpci_rc1_dma_descriptor0                          ;
//0x74	rc1 dma descriptor[63:32]
    ULONG wis_sbpci_rc1_dma_descriptor1                          ;
//0x78	rc1 dma descriptor[95:64]
    ULONG wis_sbpci_rc1_dma_descriptor2                          ;
//0x7c	rc1 dma descriptor[127:96]
    ULONG wis_sbpci_rc1_dma_descriptor3                          ;
//0x80 - 0xbc	reserved for rc2 & rc3
    ULONG rsvd1[0x40/4]                                          ;
//0xc0	wc0 dma command register
    ULONG wis_sbpci_wc0_pbi_command_register                     ;
//0xc4	wc0 dma head descriptor address register
    ULONG wis_sbpci_wc0_head_buffer_descriptor_address_register  ;
//0xc8	wc0 dma status register
    ULONG wis_sbpci_wc0_status_notify_register                   ;
//0xcc	wc0 dma timeout value register
    ULONG wis_sbpci_wc0_timeout_register                         ;
//0xd0	wc0 dma descriptor[31:0]
    ULONG wis_sbpci_wc0_dma_descriptor0                          ;
//0xd4	wc0 dma descriptor[63:32]
    ULONG wis_sbpci_wc0_dma_descriptor1                          ;
//0xd8	wc0 dma descriptor[95:64]
    ULONG wis_sbpci_wc0_dma_descriptor2                          ;
//0xdc	wc0 dma descriptor[127:96]
    ULONG wis_sbpci_wc0_dma_descriptor3                          ;
//0xe0	wc1 dma command register
    ULONG wis_sbpci_wc1_pbi_command_register                     ;
//0xe4	wc1 dma head descriptor address register
    ULONG wis_sbpci_wc1_head_buffer_descriptor_address_register  ;
//0xe8	wc1 dma status register
    ULONG wis_sbpci_wc1_status_notify_register                   ;
//0xec	wc1 dma timeout value register
    ULONG wis_sbpci_wc1_timeout_register                         ;
//0xf0	wc1 dma descriptor[31:0]
    ULONG wis_sbpci_wc1_dma_descriptor0                          ;
//0xf4	wc1 dma descriptor[63:32]
    ULONG wis_sbpci_wc1_dma_descriptor1                          ;
//0xf8	wc1 dma descriptor[95:64]
    ULONG wis_sbpci_wc1_dma_descriptor2                          ;
//0xfc	wc1 dma descriptor[127:96]
    ULONG wis_sbpci_wc1_dma_descriptor3                          ;
//0x100-0x13c	reserved for wc2 & wc3
//0x140 - 0x7fc	spongebob specifc registers

}WIS_SBPCI_DEVICE, *PWIS_SBPCI_DEVICE;
/*********************************************************************************************/

/*********************************************************************************************/
// Bourne PCI Memory Mapped Registers:
// Address Offset	Register Description
//0x00	Group A Interrupt Status Register(ISR)
#define WIS_SBPCI_GRPA_INTERRUPT_STATUS_REGISTER                                         0x00
//0x04	Group A Interrupt Control Register(ICR)
#define WIS_SBPCI_GRPA_INTERRUPT_CONTROL_REGISTER                                        0x04
//0x08	Group B ISR                                                                      
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_REGISTER                                         0x08
//0x0c	Group B ICR
#define WIS_SBPCI_GRPB_INTERRUPT_CONTROL_REGISTER                                        0x0C
//0x10	RC0 ASIC Memory Address Register 
#define WIS_SBPCI_RC0_ASIC_MEMORY_REGISTER                                               0x10
//0x14	RC1 ASIC Memory Address Register 
#define WIS_SBPCI_RC1_ASIC_MEMORY_REGISTER                                               0x14
//0x18	WC0 ASIC Memory Address Register 
#define WIS_SBPCI_WC0_ASIC_MEMORY_REGISTER                                               0x18
//0x1C	WC0 Frame Size Register
#define WIS_SBPCI_WC0_FRAME_SIZE_REGISTER                                                0x1C
//0x20	WC1 ASIC Memory Address Register 
#define WIS_SBPCI_WC1_ASIC_MEMORY_REGISTER                                               0x20
//0x24	WC1 Frame Size Register
#define WIS_SBPCI_WC1_FRAME_SIZE_REGISTER                                                0x24
//0x30-0x3c	Reserved
//0x40	RC0 DMA Command Register
#define WIS_SBPCI_RC0_PBI_COMMAND_REGISTER                                               0x40
//0x44	RC0 DMA Head Descriptor Address Register
#define WIS_SBPCI_RC0_HEAD_BUFFER_DESCRIPTOR_ADDRESS_REGISTER                            0x44
//0x48	RC0 DMA Status Register
#define WIS_SBPCI_RC0_STATUS_NOTIFY_REGISTER                                             0x48
//0x4c	RC0 DMA TimeOut Value Register
#define WIS_SBPCI_RC0_TIMEOUT_REGISTER                                                   0x4C
//0x50	RC0 DMA Descriptor[31:0]
#define WIS_SBPCI_RC0_DMA_DESCRIPTOR0                                                    0x50
//0x54	RC0 DMA Descriptor[63:32]
#define WIS_SBPCI_RC0_DMA_DESCRIPTOR1                                                    0x54
//0x58	RC0 DMA Descriptor[95:64]
#define WIS_SBPCI_RC0_DMA_DESCRIPTOR2                                                    0x58
//0x5c	RC0 DMA Descriptor[127:96]
#define WIS_SBPCI_RC0_DMA_DESCRIPTOR3                                                    0x5C
//0x60	RC1 DMA Command Register
#define WIS_SBPCI_RC1_PBI_COMMAND_REGISTER                                               0x60
//0x64	RC1 DMA Head Descriptor Address Register
#define WIS_SBPCI_RC1_HEAD_BUFFER_DESCRIPTOR_ADDRESS_REGISTER                            0x64
//0x68	RC1 DMA Status Register
#define WIS_SBPCI_RC1_STATUS_NOTIFY_REGISTER                                             0x68
//0x6c	RC1 DMA TimeOut Value Register
#define WIS_SBPCI_RC1_TIMEOUT_REGISTER                                                   0x6C
//0x70	RC1 DMA Descriptor[31:0]
#define WIS_SBPCI_RC1_DMA_DESCRIPTOR0                                                    0x70
//0x74	RC1 DMA Descriptor[63:32]
#define WIS_SBPCI_RC1_DMA_DESCRIPTOR1                                                    0x74
//0x78	RC1 DMA Descriptor[95:64]
#define WIS_SBPCI_RC1_DMA_DESCRIPTOR2                                                    0x78
//0x7c	RC1 DMA Descriptor[127:96]
#define WIS_SBPCI_RC1_DMA_DESCRIPTOR3                                                    0x7C
//0x80 - 0xbc	Reserved for RC2 & RC3
//0xC0	WC0 DMA Command Register
#define WIS_SBPCI_WC0_PBI_COMMAND_REGISTER                                               0xC0
//0xC4	WC0 DMA Head Descriptor Address Register
#define WIS_SBPCI_WC0_HEAD_BUFFER_DESCRIPTOR_ADDRESS_REGISTER                            0xC4
//0xC8	WC0 DMA Status Register
#define WIS_SBPCI_WC0_STATUS_NOTIFY_REGISTER                                             0xC8
//0xCC	WC0 DMA TimeOut Value Register
#define WIS_SBPCI_WC0_TIMEOUT_REGISTER                                                   0xCC
//0xD0	WC0 DMA Descriptor[31:0]
#define WIS_SBPCI_WC0_DMA_DESCRIPTOR0                                                    0xD0
//0xD4	WC0 DMA Descriptor[63:32]
#define WIS_SBPCI_WC0_DMA_DESCRIPTOR1                                                    0xD4
//0xD8	WC0 DMA Descriptor[95:64]
#define WIS_SBPCI_WC0_DMA_DESCRIPTOR2                                                    0xD8
//0xDc	WC0 DMA Descriptor[127:96]
#define WIS_SBPCI_WC0_DMA_DESCRIPTOR3                                                    0xDC
//0xE0	WC1 DMA Command Register
#define WIS_SBPCI_WC1_PBI_COMMAND_REGISTER                                               0xE0
//0xE4	WC1 DMA Head Descriptor Address Register
#define WIS_SBPCI_WC1_HEAD_BUFFER_DESCRIPTOR_ADDRESS_REGISTER                            0xE4
//0xE8	WC1 DMA Status Register
#define WIS_SBPCI_WC1_STATUS_NOTIFY_REGISTER                                             0xE8
//0xEc	WC1 DMA TimeOut Value Register
#define WIS_SBPCI_WC1_TIMEOUT_REGISTER                                                   0xEC
//0xF0	WC1 DMA Descriptor[31:0]
#define WIS_SBPCI_WC1_DMA_DESCRIPTOR0                                                    0xF0
//0xF4	WC1 DMA Descriptor[63:32]
#define WIS_SBPCI_WC1_DMA_DESCRIPTOR1                                                    0xF4
//0xF8	WC1 DMA Descriptor[95:64]
#define WIS_SBPCI_WC1_DMA_DESCRIPTOR2                                                    0xF8
//0xFc	WC1 DMA Descriptor[127:96]
#define WIS_SBPCI_WC1_DMA_DESCRIPTOR3                                                    0xFC
//0x100-0x13c	Reserved for WC2 & WC3
//0x140 - 0x7fc	SpongeBob Specifc Registers

typedef struct _WIS_SBHPI_DEVICE {
    // HPI_BASE+0x00-0x03         ACTUAL 0x00-0x07
    DWORD rsvd0[2];

    // HPI_BASE+0x04              ACTUAL 0x08-0x09
    DWORD hpi_stream_buffer;
    
    // HPI_BASE+0x06-0x09         ACTUAL 0x0C-0x13
    DWORD rsvd1[2];
    
    // HPI_BASE+0x0A              ACTUAL 0x14-0x15
    DWORD hpi_init_buffer;
    
    // HPI_BASE+0x0C              ACTUAL 0x18-0x19
    DWORD hpi_int_return_data;
    
    // HPI_BASE+0x0E              ACTUAL 0x1C-0x1D
    DWORD hpi_int_return_value;
    
    // HPI_BASE+0x10-0x11         ACTUAL 0x20-0x21
    DWORD rsvd2[1];
    
    // HPI_BASE+0x12              ACTUAL 0x24-0x25
    DWORD hpi_audio_buffer;
    
    // HPI_BASE+0x14              ACTUAL 0x28-0x29
    DWORD hpi_status;
    
    // HPI_BASE+0x16              ACTUAL 0x2C-0x2D
    DWORD hpi_int_parameter;
    
    // HPI_BASE+0x18              ACTUAL 0x30-0x31
    DWORD hpi_int_index;

}WIS_SBHPI_DEVICE, *PWIS_SBHPI_DEVICE;
/*********************************************************************************************/
#define WIS_SBHPI_STATUS_RSVD                                                       15:7
#define WIS_SBHPI_STATUS_ABV                                                         6:6
#define WIS_SBHPI_STATUS_URE                                                         5:5
#define WIS_SBHPI_STATUS_INTI                                                        4:4
#define WIS_SBHPI_STATUS_INTE                                                        3:3
#define WIS_SBHPI_STATUS_BME                                                         2:2
#define WIS_SBHPI_STATUS_IBL                                                         1:1
#define WIS_SBHPI_STATUS_SBL                                                         0:0

#define WIS_SBHPI_TRANSFER_SIZE                                                      0x40
#define TO_WISCHIP																	 0
#define FROM_WISCHIP																 1

//#define READ_DEV_REG(reg)                   0x12345678
#define SHIFT_VAL(entry)                    ((0?entry) % 32)
#define MASK_VAL(entry)                     (0xFFFFFFFF>>(31-((1?entry) % 32)+((0?entry) % 32)))
//#define READ_REGISTER_FIELD(reg,field)      (((REG_RD32(NV ## d ## r))>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
//#define WRITE_REGISTER_FIELD(reg,field,value) 
//(((value)>>SHIFT_VAL(field))&DRF_MASK(NV ## d ## r ## f))
//

#endif  //_PCI_7008_H

/******************************** end of pci_7008.h **************************/






