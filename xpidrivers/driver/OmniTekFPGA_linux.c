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
#include "../include/OmniTekIoctl_linux.h"

void
FPGAInit(
	Resource *pResource
	)
{
	u8  nExtras, i;
	u32 nExt[8];

	u32 VersionVal = ReadHWValue(pResource->pExt, 
										pResource->General.Bar, 
										pResource->General.RegisterOffset + 1);

	for(i = 0;i < 8;i++)
		nExt[i] = 0;

	pResource->u.FPGACtrl.Version.Major = (u8)((VersionVal >> 8) & 0xff);
	pResource->u.FPGACtrl.Version.Minor = (u8)(VersionVal & 0xff);
	nExtras = (u8)(VersionVal >> 16) & 0xf;

	pResource->u.FPGACtrl.FPGAType = UNKNOWN_TYPE;

	if(pResource->NumRegisters == 0)
		pResource->NumRegisters = 7;

	pResource->General.Initialised = true;

	for(i = 0;i < nExtras;i++){
		PResource pExtra;
        
    
		u32 Val = ReadHWValue(pResource->pExt, 
										pResource->General.Bar, 
										pResource->General.RegisterOffset + 16 + i);
		u8 Bar = (u8)((Val >> 17) & 0x7);
		u8 Ext = (u8)((Val >> 28) & 0xf);

		pExtra = AddResource(pResource->pExt, 
								OMNITEK_RESOURCE_FPGA_EXTENSION | Ext,
								Bar,
								(Val & 0x1ffff) >> 2);

		if(pExtra == NULL)
			break;

		pExtra->Identifier = nExt[Ext]++;

		pExtra->NumRegisters = ((Val >> 20) & 0xff);

		OmniTekDebug(GENERAL,KERN_INFO,"Add New FPGA Extension Resource (type %d) [%d regs]\n", pExtra->Type, pExtra->NumRegisters);

		if(pResource->pExt->RegValue[Bar].num_regs < (u32)(pExtra->General.RegisterOffset + pExtra->NumRegisters)){
			OmniTekDebug(GENERAL,KERN_INFO,"Extend Bar %d to %d regs\n", Bar, pExtra->General.RegisterOffset + pExtra->NumRegisters);
			pResource->pExt->RegValue[Bar].num_regs = pExtra->General.RegisterOffset + pExtra->NumRegisters;
		}
	}

}

void
FPGADelete(
	PResource pFPGA
	)
{
}

void
FPGAInterruptEnable(
  PResource pFPGA,
	u32       Interrupt,
	bool      Enable
	)
{
	u32  Val;

	if((pFPGA == NULL) || (pFPGA->Type != OMNITEK_RESOURCE_FPGA))
		return;

	GetRegValue(pFPGA->pExt, 
					pFPGA->General.Bar, 
					pFPGA->General.RegisterOffset + 4, 
					&Val);
	if(Enable)
		WriteRegValue(pFPGA->pExt, 
							pFPGA->General.Bar, 
							pFPGA->General.RegisterOffset + 4,
							Val | Interrupt);
	else
		WriteRegValue(pFPGA->pExt, 
							pFPGA->General.Bar, 
							pFPGA->General.RegisterOffset + 4,
							Val & (~Interrupt));

}


void
DMACtrlInterruptEnable(
  PResource pDmaCtrl,
	u32       Interrupt,
	bool      Enable
	)
{
	u32  Val;

	if((pDmaCtrl == NULL) || (pDmaCtrl->Type != OMNITEK_RESOURCE_DMA_CTRL))
		return;

	GetRegValue(pDmaCtrl->pExt, 
					pDmaCtrl->General.Bar, 
					pDmaCtrl->General.RegisterOffset + DMA_CTRL_ENH_INTERRUPT_CTRL, 
					&Val);
	if(Enable)
		WriteRegValue(pDmaCtrl->pExt, 
							pDmaCtrl->General.Bar, 
							pDmaCtrl->General.RegisterOffset + DMA_CTRL_ENH_INTERRUPT_CTRL,
							Val | Interrupt);
	else
		WriteRegValue(pDmaCtrl->pExt, 
							pDmaCtrl->General.Bar, 
							pDmaCtrl->General.RegisterOffset + DMA_CTRL_ENH_INTERRUPT_CTRL,
							Val & (~Interrupt));

}

void DMAChanEventInterruptEnable(
  struct _Resource * pChannel,
  bool               Enable,
  bool               Write
  )
{
  u32 Val;

	GetRegValue(pChannel->pExt, 
					pChannel->General.Bar, 
					pChannel->General.RegisterOffset + DMA_CHANNEL_CSR, 
					&Val);

  
  //Update bits as required
	if(Enable)
    Val |= 0x400;
  else
    Val &= ~0x400;

  if (Write)
    Val |= 0x200;
  else
    Val &= ~0x200;

  Val &= ~(OMNITEK_DMACHANNEL_INTERRUPT_MASK);    //Make sure we don't accidentally wipe any interrupt status



	WriteRegValue(pChannel->pExt, 
						pChannel->General.Bar, 
						pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
						Val);


  OmniTekDebug(IRQ, KERN_INFO, "Dma Channel: %d event interrupts %s\n",pChannel->u.DmaChannel.Index, (Enable ? "Enabled" : "Disabled"));
}
EXPORT_SYMBOL(DMAChanEventInterruptEnable);

void
InterruptEnable(
  POMNITEK_INTERFACE_EXTENSION pExt,
  u32 Interrupt,
  bool Enable
  )
{
  OmniTekDebug(IRQ,KERN_INFO,"Interrupt: %d %s\n",Interrupt,Enable ? "Enabled" : "Disabled");

  if (pExt->irq == -1 && Enable)
    OmniTekDebug(IRQ,KERN_NOTICE,"Haven't registered IRQ Number, not enabling interrupts\n");
  else if ((pExt->pDmaCtrl) && (pExt->pDmaCtrl->u.DmaCtrl.DmaInterrupts))
    DMACtrlInterruptEnable(pExt->pDmaCtrl,Interrupt,Enable);
  else if (pExt->pFPGA)
    FPGAInterruptEnable(pExt->pFPGA,Interrupt,Enable);
  else
    OmniTekDebug(IRQ,KERN_NOTICE,"Couldn't alter interrupt status - no DMA Controller or FPGA\n");
}

static u32
FPGAGetInterruptStatus(
    PResource pFPGA
	)
{
	u32 Val;
	u32 Status;

	ReadRegValue(pFPGA->pExt, 
		pFPGA->General.Bar,
		pFPGA->General.RegisterOffset + 4,
		&Val);

	Status = (Val & OMNITEK_INTERRUPT_MASK) >> 16;
  
	if(Status){
		// Clear interrupt
		WriteRegValue(pFPGA->pExt, 
			pFPGA->General.Bar,
			pFPGA->General.RegisterOffset + 4,
			Val
			);
  }
	return Status;
}

static u32
DMACtrlGetInterruptStatus(
    PResource pDmaCtrl
  )
{
  u32 Val;
  u32 Status;
	ReadRegValue(pDmaCtrl->pExt, 
		pDmaCtrl->General.Bar,
		pDmaCtrl->General.RegisterOffset + DMA_CTRL_ENH_INTERRUPT_STATUS,
		&Val);

	Status = (Val & OMNITEK_DMACTRL_INTERRUPT_MASK);
  
	if(Status){
		// Clear interrupt
		WriteRegValue(pDmaCtrl->pExt, 
			pDmaCtrl->General.Bar,
			pDmaCtrl->General.RegisterOffset + DMA_CTRL_ENH_INTERRUPT_STATUS,
			Val
			);
  }
	return Status;
}

u32
GetInterruptStatus(
    POMNITEK_INTERFACE_EXTENSION pExt
  )
{
  if ((pExt->pDmaCtrl) && (pExt->pDmaCtrl->u.DmaCtrl.DmaInterrupts))
    return DMACtrlGetInterruptStatus(pExt->pDmaCtrl);
  else
    return FPGAGetInterruptStatus(pExt->pFPGA);
  
}


u32 GetDmaChannelInterruptStatus (
  PResource pChannel
  )
{
  u32 Val;
  u32 Status;
	ReadRegValue(pChannel->pExt, 
		pChannel->General.Bar,
		pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
		&Val);

	Status = (Val & OMNITEK_DMACHANNEL_INTERRUPT_MASK);
  
	if(Status){
		// Clear interrupt - NB Write back Val so we don't change any control bits...
		WriteRegValue(pChannel->pExt, 
			pChannel->General.Bar,
			pChannel->General.RegisterOffset + DMA_CHANNEL_CSR,
			Val
			);
  }
	return Status;

}

u32 GetDmaCtrlChannelStatus (
  PResource pDmaCtrl
  )
{
	return ReadHWValue(pDmaCtrl->pExt, pDmaCtrl->General.Bar, pDmaCtrl->General.RegisterOffset + DMA_CTRL_INTERRUPT_STATUS);
}
#if 0 //Unused

static u64
FPGAReadTime(
	PResource pFPGA
	)
{
	u32 Val1, Val2;

	Val1 = ReadHWValue(pFPGA->pExt, 
		pFPGA->General.Bar,
		pFPGA->General.RegisterOffset + 10);

	Val2 = ReadHWValue(pFPGA->pExt, 
		pFPGA->General.Bar,
		pFPGA->General.RegisterOffset + 11);

	return (u64)Val1 | (u64)Val2 << 32;
}

static void
FPGAGetTime(
	PResource pFPGA, 
	u64 *Count
	)
{
  unsigned long flags;
	if(pFPGA == NULL){
		*Count = 0;
		return;
	}

	//WdfInterruptAcquireLock(pFPGA->pExt->WdfInterrupt);
  
  //TODO: Confirm this is the correct behaviour - disable interrupts on current processor before reading timer.  Do we need to disable across all CPUs?
  spin_lock_irqsave(&(pFPGA->pExt->IrqLock),flags);
	*Count = FPGAReadTime(pFPGA);
	spin_unlock_irqrestore(&(pFPGA->pExt->IrqLock),flags);
	//WdfInterruptReleaseLock(pFPGA->pExt->WdfInterrupt);
}
#endif
