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
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF
 * USE, LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR 
 * CONSEQUENTIAL DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/
#include "OmniTek_linux.h"
#include "OmniTekResources_sysfs.h"
#include "OmniTekInterrupt.h" 
#include <mach/gpio.h>
#include <linux/delay.h>

u32 list_count(struct list_head * head)
{
  struct list_head * entry;
  u32 i = 0;
  list_for_each(entry,head)
    i++;
  return i;
}

int list_index(struct list_head * list, struct list_head * entry)
{
  struct list_head * iter;
  int i = 0;
  list_for_each(iter,list)
  {
    if (iter == entry)
      return i;
    
    i++;
  }

  return -1;
}


void
OmniTekExtInit
	(
    struct pci_dev  * Device,
    POMNITEK_INTERFACE_EXTENSION	pExt,
    struct _OmniTekDriver * pDriver
	)
{
	int i;
  char poolName[128];

  OmniTekDebug(GENERAL,KERN_INFO,"Initialising Extension at %p\n", pExt);
  spin_lock_init(&(pExt->IrqLock));


	for(i = 0; i < MAX_NUM_MEM_BARS;i++){
		pExt->RegValue[i].regs = NULL;
//		pExt->RegValue[i].Object = NULL;
		pExt->RegValue[i].num_regs = 0;
	}

  pExt->nBars = 0;
	pExt->Device = Device;
	pExt->pFPGA = NULL;
	pExt->pDmaCtrl = NULL;
  pExt->pDriver = pDriver;
  pExt->irq = -1;

  snprintf(poolName,128,"Extension: %p Resource Pool",pExt);

  INIT_LIST_HEAD(&(pExt->Resources));

#ifndef USE_IRQ_THREAD
  tasklet_init(&(pExt->irq_tasklet), OmniTekInterruptHandler, (unsigned long)pExt);
#endif

}

void
OmniTekExtShutdown
	(
	POMNITEK_INTERFACE_EXTENSION	pExt
	)
{
	int i;
	PResource pResource;

    OmniTekDebug(GENERAL,KERN_INFO,"Shutting down extension\n");


    OmniTekDebug(GENERAL,KERN_INFO,"Removing Resources\n");



    //list_for_each_entry_safe(pResource,pResource_temp,&(pExt->Resources),Object)
    while(!list_empty(&(pExt->Resources)))    //Have to do this with a while loop as virtual resources may be removed e.g. DMA channels
    {
      pResource = list_entry(pExt->Resources.next,Resource, Object);
      RemoveResource(pResource);
    }
	

	for(i = 0;i < pExt->nBars;i++)
    {
        OmniTekDebug(GENERAL,KERN_INFO,"Deleting BAR %d\n",i);
		if ((pExt->RegValue[i].num_regs != 0) && (pExt->RegValue[i].regs))
            kfree(pExt->RegValue[i].regs);  //TODO: Check this is correct

        omnitek_driver_kobject_put(&(pExt->MemBar[i].kobject));

    }
    
    OmniTekDebug(GENERAL,KERN_INFO,"OmniTekExtShutDown Completed\n");
}

int
GetRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32				nBar,
	u32				nReg,
	u32		   *pVal
	)
{
	if(nBar > 2)
		return -STATUS_INVALID_PARAMETER_1;

	// -1 register offset means get the number of registers supported
	if(nReg == 0xffffffff){
		*pVal = pExt->RegValue[nBar].num_regs;
		return 0;
	}

	if(nReg >= pExt->RegValue[nBar].num_regs)
		return -STATUS_INVALID_PARAMETER_2;

	*pVal = pExt->RegValue[nBar].regs[nReg];

	return 0;
}

int
ReadRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32				nBar,
	u32				nReg,
	u32		   *pVal
	)
{
	//u32 * reg_addr;
	if(nBar > 2)
		return STATUS_INVALID_PARAMETER_1;

	if(nReg >= pExt->RegValue[nBar].num_regs)
		return STATUS_INVALID_PARAMETER_2;


	//reg_addr = MmGetPhysicalAddress((pExt)->MemBar[(nBar)].pVa + (nReg));
	
	// if (reg_addr.HighPart != 0)
		// return STATUS_INVALID_PARAMETER_3;  

	*pVal = ReadHWValue(pExt, nBar, nReg);	

	return 0;
}
EXPORT_SYMBOL(ReadRegValue);

int
WriteRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32				nBar,
	u32				nReg,
	u32		    nVal
	)
{
	if(nBar > 2)
		return STATUS_INVALID_PARAMETER_1;

	if(nReg >= pExt->RegValue[nBar].num_regs)
		return STATUS_INVALID_PARAMETER_2;

	WriteHWValue(pExt, nBar, nReg, nVal);
	pExt->RegValue[nBar].regs[nReg] = nVal;

	return 0;
}
EXPORT_SYMBOL(WriteRegValue);


// Scan through the BARs to find all the capability structures
// This will set-up capability control structures and discover maximum register offsets for each BAR
int
OmniTekScanHw(
	POMNITEK_INTERFACE_EXTENSION	pExt
	)
{
	u32 i;
	u32 RegOffset = 0;
	u32 Val = 0xfffffff;
	u32 RegVal;
	u8  CapID;
	PResource pResource;

    OmniTekDebug(GENERAL,KERN_INFO,"Scanning %d BARs\n",pExt->nBars);
  
	for(i = 0;i < pExt->nBars;i++)
	{
		Val = 0xfffffff;
		RegOffset = 0;

        OmniTekDebug(GENERAL,KERN_INFO,"Scanning BAR %d\n",i);
		while((Val & 0xffff0000) != 0)
		{
			u8 CapVer;

			if((RegOffset * sizeof(u32)) >= pExt->MemBar[i].Size)
			{
                OmniTekDebug(GENERAL,KERN_ERR,"ERROR: Register Offset %#x out of BAR range (%d, %#x)- ignore BAR\n", RegOffset, i, pExt->MemBar[i].Size);
				break;
			}

            OmniTekDebug(GENERAL, KERN_INFO, "Read First Value: pExt: %p, i: %d, offset: %d\n",pExt,i,RegOffset);

            OmniTekDebug(GENERAL, KERN_INFO, "(%p)->MemBar[(%d)].pVa + (%d)) = %p\n",pExt,i,RegOffset,((pExt)->MemBar[(i)].pVa + (RegOffset)));
			// Use the HW read so we don't bounds check the register offset  
			Val = ReadHWValue(pExt, i, RegOffset);
            OmniTekDebug(GENERAL, KERN_INFO, "Read %x\n",Val);
      

			// If we've read 0xffffffff chances are the FPGA has died - drop-out quietly
			if(Val == 0xffffffff)
			{
                 OmniTekDebug(GENERAL,KERN_ERR,"ERROR: Scan read 0xffffffff - ignore BAR\n");
                 break;
			}
            if (Val == 0)
            {
                OmniTekDebug(GENERAL,KERN_ERR,"ERROR: Scan read 0x00000000 - No Capability header\n");
                break;
            }

			pResource = NULL;
			CapVer = (u8)((Val >> 8) & 0xff);
			CapID = (u8)(Val & 0xff);

            OmniTekDebug(GENERAL, KERN_INFO, "Resource found, adding\n");

			pResource = AddResource(pExt, CapID, (u8)i, RegOffset);

			RegVal = ReadHWValue(pResource->pExt, 
									pResource->General.Bar, 
									pResource->General.RegisterOffset + 1);

			pResource->NumRegisters = ((RegVal >> 24) & 0xff);

			switch(CapID)
			{
			case OMNITEK_RESOURCE_FPGA:
				OmniTekDebug(GENERAL,KERN_INFO,"FPGA discovered at %d, %d\n", i, RegOffset);

				if(pResource != NULL)
				{
					FPGAInit(pResource);
				}

				if(pExt->pFPGA == NULL)
					pExt->pFPGA = pResource;
				else
					OmniTekDebug(GENERAL,KERN_ERR,"Duplicate FPGA!!!\n");

				break;

			case OMNITEK_RESOURCE_DMA_CTRL:		// DMA Controller
				OmniTekDebug(GENERAL,KERN_INFO,"DMA Ctrl discovered at %d, %d\n", i, RegOffset);

				if(pResource != NULL)
					DmaResourceInit(pResource);

				if(pExt->pDmaCtrl == NULL){
					pExt->pDmaCtrl = pResource;
				}
				else
				{
					OmniTekDebug(GENERAL,KERN_ERR,"Duplicate DmaCtrl!!!\n");
				}

				break;      

			default:
				OmniTekDebug(GENERAL,KERN_INFO,"Capability type %d\n", CapID);   //TODO: This is an error condition, but it's been shut up to keep the log quiet
				break;
			}

			if(pResource != NULL)
				pResource->CapVersion = CapVer;

			if((Val != 0) && (pResource != NULL))
			{
				if((pExt->RegValue[i].num_regs <= RegOffset) && (pResource->NumRegisters != 0))
				{
					pExt->RegValue[i].num_regs = RegOffset + pResource->NumRegisters;
				}

			}
			RegOffset = (Val >> 18) & 0x3fff;
		}

		if(pExt->RegValue[i].num_regs != 0)
		{
			OmniTekDebug(GENERAL,KERN_INFO,"BAR %d : %d Regs\n", i, pExt->RegValue[i].num_regs);
            pExt->RegValue[i].regs = kmalloc(sizeof(u32)*pExt->RegValue[i].num_regs,GFP_KERNEL);
            if (pExt->RegValue[i].regs != NULL)
            {
	    	    OmniTekDebug(GENERAL,KERN_INFO,"BAR - Reading %d regs\n",pExt->RegValue[i].num_regs);
#ifndef CONFIG_MACH_NETVIZ
                memcpy_fromio(pExt->RegValue[i].regs,pExt->MemBar[i].pVa,pExt->RegValue[i].num_regs*4);
#else
                memcpy_fromio(pExt->RegValue[i].regs,pExt->MemBar[i].Physical,pExt->RegValue[i].num_regs*4);
#endif
            }      
		    else
            {
				OmniTekDebug(GENERAL,KERN_INFO,"Failed to allocate %d regs for BAR %d\n", pExt->RegValue[i].num_regs, i);
				pExt->RegValue[i].num_regs = 0;
			}
		}
		else
		{
			pExt->RegValue[i].regs = NULL;
//			pExt->RegValue[i].Object = NULL;
		}
        OmniTekDebug(GENERAL,KERN_INFO,"BAR %d Scanned\n",i);
	}

    OmniTekDebug(GENERAL,KERN_INFO,"Hardware scan complete\n");
	return 0;
}

int
OmniTekGetCapList(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	CapabilityList *				pCapList
	)
{
  u32 count = 0;
  PResource pResource;
  pCapList->nMaxCapabilities = list_count(&(pExt->Resources));
  //nOffset is always 0, so we should be able to go through the list:
  list_for_each_entry(pResource,&(pExt->Resources),Object)
  {
    pCapList->pList[count].Id = pResource;
		pCapList->pList[count].Type = pResource->Type;
		pCapList->pList[count].Version = pResource->CapVersion;
		pCapList->pList[count].Size = pResource->NumRegisters;

		switch(pResource->Type){
		case OMNITEK_RESOURCE_FPGA:
			pCapList->pList[count].Major = pResource->u.FPGACtrl.Version.Major;
			pCapList->pList[count].Minor = pResource->u.FPGACtrl.Version.Minor;
			break;
		default:
			pCapList->pList[count].Major = 0;
			pCapList->pList[count].Minor = 0;
			break;
    }
    
    count++;
  }

	pCapList->nCapabilities = count;

	return 0;
}

/* ---------------Routines to read/write XPi V6 FPGA registers--------------- */

#define FPGA_BARNO                  1

#define FPGA_WR_DATA_OFFSET         0x1a
#define FPGA_WR_ADDR_OFFSET         0x1b
#define FPGA_RD_DATA_OFFSET         0x1c
#define S3FPGA_RESET_OFFSET         0x34

#define S3FPGA_RESET_BIT            0x01

#define FLAG_READ                   0x01
#define FLAG_WRITE                  0x02

static int fpga_reg_write_data(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                               unsigned long value)
{
    unsigned long offset;

    if (pExt == NULL)
        return -EINVAL;

    offset = FPGA_WR_DATA_OFFSET;
    if (offset < (pExt->MemBar[FPGA_BARNO].Size >> 2)) {
        WriteHWValue(pExt, FPGA_BARNO, offset, value);
        return 0;
    } 
    return -EINVAL;
}

static int fpga_reg_write_addr(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                               unsigned long addr,
                               int flag)
{
    unsigned long offset, value;

    if (pExt == NULL)
        return -EINVAL;

    offset = FPGA_WR_ADDR_OFFSET;
    value = 0;
    if (FPGA_WR_ADDR_OFFSET < (pExt->MemBar[FPGA_BARNO].Size >> 2)) {
        WriteHWValue(pExt, FPGA_BARNO, offset, value);
        value = (flag & FLAG_READ) ? ((1 << 17) | addr) : ((1 << 16) | addr); 
        WriteHWValue(pExt, FPGA_BARNO, offset, value);
        return 0;
    } 
    return -EINVAL;
}

static int fpga_reg_read_data(struct _OMNITEK_INTERFACE_EXTENSION *pExt,
                              unsigned long *value)
{
    unsigned long offset;

    if (pExt == NULL)
        return -EINVAL;
   
    offset = FPGA_RD_DATA_OFFSET;
    if (FPGA_RD_DATA_OFFSET < (pExt->MemBar[FPGA_BARNO].Size >> 2)) {
        *value = ReadHWValue(pExt, FPGA_BARNO, offset);
        return 0;
    }
    return -EINVAL;
}

int xpi_get_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long *val)
{
    unsigned long extflags, value;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (!fpga_reg_write_addr(pExt, reg, FLAG_READ)) {
        (void) fpga_reg_read_data(pExt, &value);
        *val = value;
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_get_fpga_val);

int xpi_set_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long extflags;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (!fpga_reg_write_data(pExt, val)) {
        (void) fpga_reg_write_addr(pExt, reg, FLAG_WRITE);
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_set_fpga_val);

int xpi_and_in_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long extflags, value;
    int rc = -EFAULT;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (fpga_reg_write_addr(pExt, reg, FLAG_READ) < 0) 
        goto out;
    if (fpga_reg_read_data(pExt, &value) < 0)
        goto out; 
    if (fpga_reg_write_data(pExt, value & val) < 0)
        goto out;
    (void) fpga_reg_write_addr(pExt, reg, FLAG_WRITE);
    rc = 0;
out:
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return rc;
}
EXPORT_SYMBOL(xpi_and_in_fpga_val);

int xpi_or_in_fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long extflags, value;
    int rc = -EFAULT;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (fpga_reg_write_addr(pExt, reg, FLAG_READ) < 0) 
        goto out;
    if (fpga_reg_read_data(pExt, &value) < 0)
        goto out; 
    if (fpga_reg_write_data(pExt, value | val) < 0)
        goto out;
    (void) fpga_reg_write_addr(pExt, reg, FLAG_WRITE);
    rc = 0;
out:
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return rc;
}
EXPORT_SYMBOL(xpi_or_in_fpga_val);

/* -------Routines to read/write Omnitek registers from the XPi driver------- */

int xpi_get_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int bar, unsigned long reg, unsigned long *val)
{
    unsigned long extflags;

    if (pExt == NULL)
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (reg < (pExt->MemBar[bar].Size >> 2)) {
        *val = ReadHWValue(pExt, bar, reg);  
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_get_omnitek_val);

int xpi_set_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int bar, unsigned long reg, unsigned long val)
{
    unsigned long extflags;

    if (pExt == NULL)
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (reg < (pExt->MemBar[bar].Size >> 2)) {
        WriteHWValue(pExt, bar, reg, val);  
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_set_omnitek_val);

int xpi_and_in_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int bar, unsigned long reg, unsigned long val)
{
    unsigned long extflags, value = 0;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (reg < (pExt->MemBar[bar].Size >> 2)) {
        value = ReadHWValue(pExt, bar, reg);  
        WriteHWValue(pExt, bar, reg, value & val);  
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_and_in_omnitek_val);

int xpi_or_in_omnitek_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        int bar, unsigned long reg, unsigned long val)
{
    unsigned long extflags, value = 0;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);
    if (reg < (pExt->MemBar[bar].Size >> 2)) {
        value = ReadHWValue(pExt, bar, reg);  
        WriteHWValue(pExt, bar, reg, value | val);  
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_or_in_omnitek_val);

/* --------------Routines to read/write XPi S6 FPGA registers-------------- */

#define MAX_ITER        1000
#define MAX_RETRIES     3

static void reset_s6fpga_access(struct _OMNITEK_INTERFACE_EXTENSION *pExt)
{
    unsigned long val;

    if (pExt == NULL) 
        return;

    if (!fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_READ)) {
        (void) fpga_reg_read_data(pExt, &val);
        if (val != 0) {
            if (!fpga_reg_write_data(pExt, 0)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_WRITE);
            }
        }
    }
}

int xpi_get_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long *val)
{
    unsigned long extflags, value, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s6fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S6_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, 0x02)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_WRITE);
                rc = 0x02;
                while (iter--) {
                    if (!fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_READ)) 
                        (void) fpga_reg_read_data(pExt, &rc);
                    if (!rc)
                        break;
                }
                if (iter == 0) {
                    reset_s6fpga_access(pExt);
                    continue;
                }
                if (!fpga_reg_write_addr(pExt, FPGA_S6_DATAO_REG, FLAG_READ)) {
                    (void) fpga_reg_read_data(pExt, &value);
                    *val = value;
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_get_s6fpga_val);

int xpi_set_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long extflags, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s6fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S6_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, val)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S6_DATAI_REG, FLAG_WRITE);
                if (!fpga_reg_write_data(pExt, 0x01)) {
                    (void) fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, 
                            FLAG_WRITE);
                    rc = 0x01;
                    while (iter--) {
                        if (!fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, 
                                    FLAG_READ)) 
                            (void) fpga_reg_read_data(pExt, &rc);
                        if (!rc)
                            break;
                    }
                    if (iter == 0) { 
                        reset_s6fpga_access(pExt);
                        continue;
                    }
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_set_s6fpga_val);

int xpi_and_in_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long value = 0, extflags, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s6fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S6_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, 0x02)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_WRITE);
                rc = 0x02;
                while (iter--) {
                    if (!fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_READ)) 
                        (void) fpga_reg_read_data(pExt, &rc);
                    if (!rc)
                        break;
                }
                if (iter == 0) {
                    reset_s6fpga_access(pExt);
                    continue;
                }
                if (!fpga_reg_write_addr(pExt, FPGA_S6_DATAO_REG, FLAG_READ)) {
                    (void) fpga_reg_read_data(pExt, &value);
                }
            }
            value &= val;
            if (!fpga_reg_write_data(pExt, value)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S6_DATAI_REG, FLAG_WRITE);
                if (!fpga_reg_write_data(pExt, 0x01)) {
                    (void) fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, 
                            FLAG_WRITE);
                    rc = 0x01;
                    while (iter--) {
                        if (!fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, 
                                    FLAG_READ)) 
                            (void) fpga_reg_read_data(pExt, &rc);
                        if (!rc)
                            break;
                    }
                    if (iter == 0) {
                        reset_s6fpga_access(pExt);
                        continue;
                    }
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_and_in_s6fpga_val);

int xpi_or_in_s6fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long value = 0, extflags, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s6fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S6_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, 0x02)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_WRITE);
                rc = 0x02;
                while (iter--) {
                    if (!fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, FLAG_READ)) 
                        (void) fpga_reg_read_data(pExt, &rc);
                    if (!rc)
                        break;
                }
                if (iter == 0) {
                    reset_s6fpga_access(pExt);
                    continue;
                }
                if (!fpga_reg_write_addr(pExt, FPGA_S6_DATAO_REG, FLAG_READ)) {
                    (void) fpga_reg_read_data(pExt, &value);
                }
            }
            value |= val;
            if (!fpga_reg_write_data(pExt, value)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S6_DATAI_REG, FLAG_WRITE);
                if (!fpga_reg_write_data(pExt, 0x01)) {
                    (void) fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, 
                            FLAG_WRITE);
                    rc = 0x01;
                    while (iter--) {
                        if (!fpga_reg_write_addr(pExt, FPGA_S6_CMD_REG, 
                                    FLAG_READ)) 
                            (void) fpga_reg_read_data(pExt, &rc);
                        if (!rc)
                            break;
                    }
                    if (iter == 0) {
                        reset_s6fpga_access(pExt);
                        continue;
                    }
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_or_in_s6fpga_val);

/* --------------Routines to read/write XPi S3 FPGA registers-------------- */

static void reset_s3fpga_access(struct _OMNITEK_INTERFACE_EXTENSION *pExt)
{
    unsigned long val;

    if (pExt == NULL) 
        return;

    if (!fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_READ)) {
        (void) fpga_reg_read_data(pExt, &val);
        if (val != 0) {
            if (!fpga_reg_write_data(pExt, 0)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_WRITE);
            }
        }
    }
}

int xpi_get_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long *val)
{
    unsigned long extflags, value = 0, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s3fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S3_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, 0x02)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_WRITE);
                rc = 0x02;
                while (iter--) {
                    if (!fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_READ)) 
                        (void) fpga_reg_read_data(pExt, &rc);
                    if (!rc)
                        break;
                }
                if (iter == 0) {
                    reset_s3fpga_access(pExt);
                    break;
                }
                if (!fpga_reg_write_addr(pExt, FPGA_S3_DATAO_REG, FLAG_READ)) {
                    (void) fpga_reg_read_data(pExt, &value);
                    *val = value;
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_get_s3fpga_val);

int xpi_set_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long extflags, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s3fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S3_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, val)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S3_DATAI_REG, FLAG_WRITE);
                if (!fpga_reg_write_data(pExt, 0x01)) {
                    (void) fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, 
                            FLAG_WRITE);
                    rc = 0x01;
                    while (iter--) {
                        if (!fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, 
                                    FLAG_READ)) 
                            (void) fpga_reg_read_data(pExt, &rc);
                        if (!rc)
                            break;
                    }
                    if (iter == 0) {
                        reset_s3fpga_access(pExt);
                        continue;
                    }
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_set_s3fpga_val);

int xpi_and_in_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long value = 0, extflags, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s3fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S3_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, 0x02)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_WRITE);
                rc = 0x02;
                while (iter--) {
                    if (!fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_READ)) 
                        (void) fpga_reg_read_data(pExt, &rc);
                    if (!rc)
                        break;
                }
                if (iter == 0) {
                    reset_s3fpga_access(pExt);
                    continue;
                }
                if (!fpga_reg_write_addr(pExt, FPGA_S3_DATAO_REG, FLAG_READ)) {
                    (void) fpga_reg_read_data(pExt, &value);
                }
            }
            value &= val;
            if (!fpga_reg_write_data(pExt, value)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S3_DATAI_REG, FLAG_WRITE);
                if (!fpga_reg_write_data(pExt, 0x01)) {
                    (void) fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, 
                            FLAG_WRITE);
                    rc = 0x01;
                    while (iter--) {
                        if (!fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, 
                                    FLAG_READ)) 
                            (void) fpga_reg_read_data(pExt, &rc);
                        if (!rc)
                            break;
                    }
                    if (iter == 0) {
                        reset_s3fpga_access(pExt);
                        continue;
                    }
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_and_in_s3fpga_val);

int xpi_or_in_s3fpga_val(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
        unsigned long reg, unsigned long val)
{
    unsigned long value = 0, extflags, rc = 0;
    int iter = MAX_ITER, retries;

    if (pExt == NULL) 
        return -EFAULT;

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    reset_s3fpga_access(pExt);

    for (retries = 0; retries < MAX_RETRIES; retries++) {
        if (!fpga_reg_write_data(pExt, reg)) {
            (void) fpga_reg_write_addr(pExt, FPGA_S3_ADDR_REG, FLAG_WRITE);
            if (!fpga_reg_write_data(pExt, 0x02)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_WRITE);
                rc = 0x02;
                while (iter--) {
                    if (!fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, FLAG_READ)) 
                        (void) fpga_reg_read_data(pExt, &rc);
                    if (!rc)
                        break;
                }
                if (iter == 0) {
                    reset_s3fpga_access(pExt);
                    continue;
                }
                if (!fpga_reg_write_addr(pExt, FPGA_S3_DATAO_REG, FLAG_READ)) {
                    (void) fpga_reg_read_data(pExt, &value);
                }
            }
            value |= val;
            if (!fpga_reg_write_data(pExt, value)) {
                (void) fpga_reg_write_addr(pExt, FPGA_S3_DATAI_REG, FLAG_WRITE);
                if (!fpga_reg_write_data(pExt, 0x01)) {
                    (void) fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, 
                            FLAG_WRITE);
                    rc = 0x01;
                    while (iter--) {
                        if (!fpga_reg_write_addr(pExt, FPGA_S3_CMD_REG, 
                                    FLAG_READ)) 
                            (void) fpga_reg_read_data(pExt, &rc);
                        if (!rc)
                            break;
                    }
                    if (iter == 0) {
                        reset_s3fpga_access(pExt);
                        continue;
                    }
                    break;
                }
            }
        }
    }
    spin_unlock_irqrestore(&pExt->SpinLock, extflags);
    return 0;
}
EXPORT_SYMBOL(xpi_or_in_s3fpga_val);

/* ------------------Routines to read/write MDIO registers------------------- */

#define MDIO_OP_SETADDR             (0x0)
#define MDIO_OP_WRITE               (0x1)
#define MDIO_OP_READ                (0x2)

#define BARCO_MDIO_OPCODE           (0x00)
#define BARCO_MDIO_ADDRESS          (0x02)
#define BARCO_MDIO_WRITEDATA        (0x04)
#define BARCO_MDIO_READDATA         (0x06)
#define BARCO_MDIO_MIIMSEL          (0x08)
#define BARCO_MDIO_REQUEST          (0x0A)
#define BARCO_MDIO_MIIMREADY        (0x0C)
#define BARCO_MDIO_DRP_ADDRESS      (0x0E)
#define BARCO_MDIO_DRP_ENABLE       (0x10)
#define BARCO_MDIO_DRP_WRITEDATA    (0x12)
#define BARCO_MDIO_DRP_WRITEENABLE  (0x14)
#define BARCO_MDIO_DRP_READDATA     (0x16)
#define BARCO_MDIO_DRP_READY        (0x18)
#define BARCO_MDIO_MAC_IFG          (0x1A)
#define BARCO_MDIO_MAC_10GSEL       (0x1C)

static int mdio_wait(struct _OMNITEK_INTERFACE_EXTENSION *pExt, int mdio_offset)
{
    int count = 30000;
    int result = 0;

    if (pExt == NULL)
        return -EINVAL;

    while (--count) {
        result = ReadHWValue(pExt, 1, mdio_offset + BARCO_MDIO_MIIMREADY);
        if ((result & 0x01) == 0x01) 
            break;
        udelay(25);
    }
    if (count == 0) {
        OmniTekDebug(GENERAL, KERN_ERR, "mdio_rdy never went high after %lu\n",
                jiffies);
        return -EINVAL;
    }
    OmniTekDebug(GENERAL, KERN_INFO, "%s, after wait %lu.\n", __func__, jiffies);
    return 0;
}

static int xpi_mdio_write(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                          int mdio_offset,
                          int offset, 
                          int phyad, 
                          u16 addr, 
                          u16 val)
{
    unsigned long extflags;

    if (pExt == NULL) 
        return -EFAULT;

    OmniTekDebug(GENERAL, KERN_INFO, 
        "MDIO write to mdio_offset %x offset %x phyad %x reg %x value %x\n", 
        mdio_offset, offset, phyad, addr, val);

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    //Write out the write control word
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x00);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 1);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_ADDRESS, 
        (phyad << 5) | (addr & 0x1f) | offset);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_WRITEDATA, val);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_WRITE);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x01);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x00);
    mdio_wait(pExt, mdio_offset);

    // Set to default
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 1);

    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}

static int xpi_mdio_read(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                         int mdio_offset,
                         int offset, 
                         int phyad,
                         u16 addr)
{
    unsigned long extflags;
    int result = 0;

    if (pExt == NULL) 
        return -EFAULT;

    OmniTekDebug(GENERAL, KERN_INFO, 
        "MDIO read from mdio_offset 0x%x offset %d phyad %d addr 0x%x\n", 
        mdio_offset, offset, phyad, addr);

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    //Write out the write control word
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x00);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 1);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_ADDRESS, 
        (phyad << 5) | (addr & 0x1f) | offset);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x01);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x00);
    mdio_wait(pExt, mdio_offset);
    result = ReadHWValue(pExt,1,mdio_offset + BARCO_MDIO_READDATA);

    // Set to default
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 1);

    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    OmniTekDebug(GENERAL, KERN_INFO, 
        "Value read from mdio_offset 0x%x offset %d phyad %d addr %d = 0x%x\n",
        mdio_offset, offset, phyad, addr, result);

    return result;
}

#define PHY_PAGESEL         0x16

int xpi_paged_mdio_write(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                         int mdio_offset,
                         int offset, 
                         int phyad, 
                         u16 page,
                         u16 addr, 
                         u16 val)
{
    unsigned long extflags;
    int index;

    if (pExt == NULL) 
        return -EFAULT;

    if (offset != MAIN_OFFSET && offset != EXPANSION_OFFSET)
        return -EFAULT;
    
    index = (offset == MAIN_OFFSET) ? 0 : 1;

    OmniTekDebug(GENERAL, KERN_INFO, 
        "MDIO write to mdio_offset %x off %x phy %x page %x reg %x value %x\n", 
        mdio_offset, offset, phyad, page, addr, val);

    spin_lock_irqsave(&pExt->PhyLock[index], extflags);
    
    //Write page select
    xpi_mdio_write(pExt, mdio_offset, offset, phyad, PHY_PAGESEL, page);

    //Write register
    xpi_mdio_write(pExt, mdio_offset, offset, phyad, addr, val);
    
    spin_unlock_irqrestore(&pExt->PhyLock[index], extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_paged_mdio_write);

int xpi_paged_mdio_read(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                        int mdio_offset,
                        int offset, 
                        int phyad,
                        u16 page,
                        u16 addr)
{
    unsigned long extflags;
    int index, result = 0;

    if (pExt == NULL) 
        return -EFAULT;

    if (offset != MAIN_OFFSET && offset != EXPANSION_OFFSET)
        return -EFAULT;

    OmniTekDebug(GENERAL, KERN_INFO, 
        "MDIO read from mdio_offset %x off %x phy %x page %x reg %x\n", 
        mdio_offset, offset, phyad, page, addr);

    index = (offset == MAIN_OFFSET) ? 0 : 1;

    spin_lock_irqsave(&pExt->PhyLock[index], extflags);

    //Write page select
    xpi_mdio_write(pExt, mdio_offset, offset, phyad, PHY_PAGESEL, page);

    //Read register
    result = xpi_mdio_read(pExt, mdio_offset, offset, phyad, addr);

    spin_unlock_irqrestore(&pExt->PhyLock[index], extflags);

    OmniTekDebug(GENERAL, KERN_INFO, 
        "Value read from mdio_offset %x off %x phy %x page %x reg %x = %x\n",
        mdio_offset, offset, phyad, page, addr, result);

    return result;
}
EXPORT_SYMBOL(xpi_paged_mdio_read);

int xpi_mac_read(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                 int mdio_offset,
                 int addr) 
{
    unsigned long extflags;
    int result = 0;

    if (pExt == NULL) 
        return -EFAULT;

    OmniTekDebug(GENERAL, KERN_INFO, 
        "MAC read from mdio_offset 0x%x address %x\n", mdio_offset, addr);

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_ADDRESS, addr);

    // Set to default
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x00);

    // Select host interface
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 0);

    //Read a value back
    result = ReadHWValue(pExt,1,mdio_offset + BARCO_MDIO_READDATA);

    // Set to default
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);

    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    OmniTekDebug(GENERAL, KERN_INFO, 
            "MAC read at mdio_offset 0x%04x, address 0x%04x - value=0x%08X\n", 
            mdio_offset, addr, result);

    return result;
}
EXPORT_SYMBOL(xpi_mac_read);

int xpi_mac_write(struct _OMNITEK_INTERFACE_EXTENSION *pExt, 
                  int mdio_offset,
                  int addr, 
                  int val)
{
    unsigned long extflags;

    if (pExt == NULL) 
        return -EFAULT;

    OmniTekDebug(GENERAL, KERN_INFO, 
        "MAC write to mdio_offset: 0x%x addr %x val 0x%x\n", 
        mdio_offset, addr, val);

    spin_lock_irqsave(&pExt->SpinLock, extflags);

    // Set to default
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_REQUEST, 0x00);

    // Write the value
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 0x00);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_ADDRESS, addr);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_WRITEDATA, val);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_WRITE);

    // Set to default
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_OPCODE, MDIO_OP_READ);
    WriteHWValue(pExt,1,mdio_offset + BARCO_MDIO_MIIMSEL, 0x01);

    spin_unlock_irqrestore(&pExt->SpinLock, extflags);

    return 0;
}
EXPORT_SYMBOL(xpi_mac_write);


/* ----------------------------------GPIO----------------------------------- */

void set_gpio_pin(int pin, char *name, int value)
{
    int rc = gpio_request(pin, name);
    if (rc == 0) { 
        rc = gpio_direction_output(pin, 0);
        if (rc == 0) {
            mdelay(1);
            gpio_set_value(pin, value);
        }
        gpio_free(pin);
        mdelay(100);
    }
}
EXPORT_SYMBOL(set_gpio_pin);
