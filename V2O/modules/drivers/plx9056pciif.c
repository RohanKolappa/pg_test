
#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "fpga.h"
#include "v4_fpga.h"
#include "fpga_ioctl.h"
#include "log_control.h"
#include "plx9056pciif.h"

/*******************************
 * Forward Declarations        *
 *******************************/
static void plx_dma_test_handler(unsigned long data);

/***************************************************************
 * Routines to modify a PLX register synchronized with the ISR *
 ***************************************************************/
static void plx_sync_modify(struct v2d_board *board,
                            unsigned short reg,
                            unsigned long bits_to_set, 
                            unsigned long bits_to_clear) {
    unsigned long flags;
    unsigned long regval;
/*     LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX %s -> %08x\n", __FUNCTION__, reg); */

    spin_lock_irqsave(&(board->v_plx_isr_lock), flags);

    regval = get_plx_val(board, reg);
    regval |= bits_to_set;
    regval &= ~bits_to_clear;
    set_plx_val(board, reg, regval);

    spin_unlock_irqrestore(&(board->v_plx_isr_lock), flags);
}

/*************************************************************
 * Routines to enable and disable PLX chip interrupts        *
 *************************************************************/
void plx_interrupt_enable(struct v2d_board *board, int intr) {
    unsigned long regval;
    unsigned long bits_to_set;

    if (!board->v_hardware_detected)
        return;
/*     LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX %s -> %08x\n", __FUNCTION__, intr); */


    bits_to_set = (unsigned long) 0;

    if (intr & PLX_INT_TYPE_PCI_MAIN) {
        bits_to_set |= (1 << 8);
    }

    if (intr & PLX_INT_TYPE_LOCAL) 
        bits_to_set |= (1 << 11);

    if (intr & PLX_INT_TYPE_DMA_0) {
        bits_to_set |= (1 << 18);
        /* Make sure DMA interrupt is routed to PCI */
        regval = get_plx_val(board, PCI9056_DMA0_MODE);
        set_plx_val(board, PCI9056_DMA0_MODE, regval | (1 << 17));
        regval = get_plx_val(board, PCI9056_DMA0_MODE);
/*         LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX DMA0 mode%s -> %08x\n", __FUNCTION__, regval); */
    }

    if (intr & PLX_INT_TYPE_DMA_1) {
        bits_to_set |= (1 << 19);
        /* Make sure DMA interrupt is routed to PCI */
        regval = get_plx_val(board, PCI9056_DMA1_MODE);
        set_plx_val(board, PCI9056_DMA1_MODE, regval | (1 << 17));
        regval = get_plx_val(board, PCI9056_DMA1_MODE);
/*         LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX DMA1 mode%s -> %08x\n", __FUNCTION__, regval); */
    }

    /* Write register value if they've changed */
    if (bits_to_set != (unsigned long) 0) {
        plx_sync_modify(board, PCI9056_INT_CTRL_STAT, bits_to_set, 0); 
    }
    regval = get_plx_val(board, PCI9056_INT_CTRL_STAT);
/*     LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX INTR CTRL STAT mode%s -> %08x\n", __FUNCTION__, regval); */
}

void plx_interrupt_disable(struct v2d_board *board, int intr) {
    unsigned long regval, bits_to_clear;

    if (!board->v_hardware_detected)
        return;
/*     LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX %s -> %08x\n", __FUNCTION__, intr); */

    bits_to_clear = (unsigned long) 0;

    if (intr & PLX_INT_TYPE_PCI_MAIN) {
        bits_to_clear |= (1 << 8);
    }

    if (intr & PLX_INT_TYPE_LOCAL) 
        bits_to_clear |= (1 << 11);

    if (intr & PLX_INT_TYPE_DMA_0) {
        bits_to_clear |= (1 << 18);    
        /* Make sure DMA interrupt is not routed to PCI */
        regval = get_plx_val(board, PCI9056_DMA0_MODE);
        set_plx_val(board, PCI9056_DMA0_MODE, regval & ~(1 << 17));
    }

    if (intr & PLX_INT_TYPE_DMA_1) {
        bits_to_clear |= (1 << 19);
        /* Make sure DMA interrupt is not routed to PCI */
        regval = get_plx_val(board, PCI9056_DMA1_MODE);
        set_plx_val(board, PCI9056_DMA1_MODE, regval & ~(1 << 17));
    }

    /* Write register value if they've changed */
    if (bits_to_clear != (unsigned long) 0) {
        plx_sync_modify(board, PCI9056_INT_CTRL_STAT, 0, bits_to_clear);
    }
}

/*************************************************************
 * Resets the PLX chip using the software reset feature      *
 *************************************************************/
static void plx_board_reset(struct v2d_board *board) {
    unsigned char mu_enabled, eeprom_present;
    unsigned long regval, regintr, reghs, regpm; 

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO,
            "PLX %s -> %08x\n", __FUNCTION__, board->v_id);

    /* Clear PCI errors */
    regval = get_plx_config_val(board, CFG_COMMAND);
    
    if (regval & (0xf8 << 24)) {
        /* Write value back to clear aborts */
        set_plx_config_val(board, CFG_COMMAND, regval);
    }

    /* Set state of I2O decode enable */
    regval = get_plx_val(board, PCI9056_FIFO_CTRL_STAT);
    mu_enabled = (unsigned char) (regval & (1 << 0));

    /* Determine if EEPROM is present */
    regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT);
 
    /* Make sure s/w rset and EEPROM reload bits are clear */
    regval &= ~((1 << 30) | (1 << 29));

    /* Remember if EEPROM is present */
    eeprom_present = (unsigned char) ((regval >> 28) & (1 << 0));

    /* Save interrupt line */
    regintr = get_plx_config_val(board, CFG_INT_LINE);

    /* Save HS and PM registers */
    reghs = get_plx_config_val(board, PCI9056_HS_CAP_ID);
    regpm = get_plx_config_val(board, PCI9056_PM_CSR);

    /* Issue software reset */
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval | (1 << 30));
    mdelay(100);

    /* Bring chip out of reset */
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);
    
    /* Issue EEPROM reload in case not programmed */
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval | (1 << 29));
    mdelay(10);

    /* Clear EEPROM reload */
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);
    
    /* Restore I2O decode enable state */
    if (mu_enabled) {
        regval = get_plx_val(board, PCI9056_FIFO_CTRL_STAT);
        set_plx_val(board, PCI9056_FIFO_CTRL_STAT, regval | (1 << 0));
    }

    /* Restore interrupt line */
    set_plx_config_val(board, CFG_INT_LINE, regintr);

    /* If EEPROM present, restore registers */
    if (eeprom_present) {
        /* Mask out HS bits that can be cleared */
        reghs &= ~((1 << 23) | (1 << 22) | (1 << 17));
        set_plx_config_val(board, PCI9056_HS_CAP_ID, reghs);

        /* Mask out PM bits that can be cleared */
        regpm &= ~(1 << 15);
        set_plx_config_val(board, PCI9056_PM_CSR, regpm);
    }
}

/*************************************************************
 * Routines to setup, initiate and clean up DMA transfers    *
 * Should be called with board->v_curr_buff_lock held.         *
 *************************************************************/
int plx_block_dma_channel_open(struct v2d_board *board, int cno) {

/*     LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX %s -> %08x\n", __FUNCTION__, cno); */

    if (cno < 0 || cno > 2) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING,
                 "ERROR - Invalid board or channel number: %d.\n", cno); 
        return ERROR;
    }

    /* Open the DMA channel */
    if (board->dma_channel[cno].state != DMA_STATE_CLOSED) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING,
                 "ERROR - Board DMA channel %d already open.\n",  cno);
        return ERROR;
    }
    board->dma_channel[cno].state = DMA_STATE_BLOCK;

#if 0
    // Why do we need this??
    if (cno == 0) {
        /* Enable DMA channel 0 interrupt */
        plx_sync_modify(board, PCI9056_INT_CTRL_STAT, (1 << 18), 0);
    }
    else {
        /* Enable DMA channel 1 interrupt */
        plx_sync_modify(board, PCI9056_INT_CTRL_STAT, (1 << 19), 0);
    }
#endif

    return OK;
} 

int plx_block_dma_channel_close(struct v2d_board *board, int cno) {
/*     LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX %s -> %08x\n", __FUNCTION__, cno); */

    if (cno < 0 || cno > 2) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING,
                 "ERROR - Invalid board or channel number:  %d.\n", cno); 
        return ERROR;
    }

    /* Verify DMA channel was opened for block transfer */
    if (board->dma_channel[cno].state != DMA_STATE_BLOCK) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING,
                 "Board %p DMA channel %d not open for block xfer.\n", board, cno);
        return PLX_DMA_CHANNEL_UNAVAILABLE;
    }

    /* Close the channel */
    board->dma_channel[cno].state = DMA_STATE_CLOSED;

    return OK;
} 

int plx_block_dma_inprogress(struct v2d_board *board, int cno) {
    unsigned long regval;
    LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO,
            "PLX %s -> %08x\n", __FUNCTION__, cno);

    if (cno < 0 || cno > 2) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING,
                 "ERROR - Invalid board or channel number: %d.\n", cno); 
        return ERROR;
    }

    regval = get_plx_val(board, PCI9056_DMA_COMMAND_STAT);

    if (cno == 1) 
        regval = regval >> 8;

    if ((regval & (1 << 4))  == 0) 
        return PLX_DMA_INPROGRESS;

    return PLX_DMA_PAUSED;
}


int plx_block_dma_transfer(struct v2d_board *board, 
                           int cno,  
                           unsigned long pci_addr_lo,
                           unsigned long local_addr,
                           unsigned long transfer_count,
                           unsigned char direction) {
    unsigned char shift;
    unsigned short offset_dma_base;
    unsigned long regval;
/*     LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, */
/*             "PLX %s -> %08x\n", __FUNCTION__, transfer_count); */

    if (cno == 0) {
        shift = 0;
        offset_dma_base = PCI9056_DMA0_MODE;
    }
    else if (cno == 1) {
        shift = 8;
        offset_dma_base = PCI9056_DMA1_MODE;
    }
    else 
        return PLX_DMA_CHANNEL_INVALID;

    /* Verify that DMA is not in progress */
    if (plx_block_dma_inprogress(board, cno) != PLX_DMA_PAUSED) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING,
                 "ERROR - DMA channel %d is currently active. regval = 0x%lx\n", cno, get_plx_val(board, PCI9056_DMA_COMMAND_STAT)); 
        return PLX_DMA_INPROGRESS;
    }

    /* Verify DMA channel was opened for block transfer */
    if (board->dma_channel[cno].state != DMA_STATE_BLOCK) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING,
                 "%s - Board DMA channel %d not open for block xfer.\n",
                 __FUNCTION__, cno);
        return PLX_DMA_CHANNEL_UNAVAILABLE;
    }

    /* Make sure DMA done interrupt is enabled */
    regval = get_plx_val(board, offset_dma_base);
    set_plx_val(board, offset_dma_base, regval | (1 << 10));

    /* Write low PCI address */
    set_plx_val(board, offset_dma_base + 0x4, pci_addr_lo);
    
    /* Write local address */
    set_plx_val(board, offset_dma_base + 0x8, local_addr);

    /* Write transfer count */
    set_plx_val(board, offset_dma_base + 0xc, transfer_count);
 
    /* Set transfer direction */
    set_plx_val(board, offset_dma_base + 0x10, (0 << 2) | (direction << 3));

    /* Enable DMA channel and start DMA transfer */
    regval = get_plx_val(board, PCI9056_DMA_COMMAND_STAT);
    set_plx_val(board, PCI9056_DMA_COMMAND_STAT, regval | 
                (((1 << 0) | (1 << 1)) << shift));

    return OK;
}


static void plx_dma_test_init(struct v2d_board *board) {

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, 
             "PLX DMA Test: allocate physical buffer\n");
    board->v_plx_test_nearvar = kmalloc(PLX_DMA_IO_SIZE, GFP_KERNEL);
    if (board->v_plx_test_nearvar == NULL) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, "%s: malloc failed!\n", 
                 __FUNCTION__);
        return;
    }

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, 
             "Initialize FPGA interrupt function.\n");
    fpga_interrupt_init(board);

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, 
             "Register DMA interrupt handler.\n");
    fpga_register_irq(board, INT_30_PLX_DMA_0, plx_dma_test_handler);

    fpga_register_irq(board, INT_31_PLX_DMA_1, plx_dma_test_handler);


}


static void plx_dma_test_exit(struct v2d_board *board) {

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, "Freeing memory.\n");
    if (board->v_plx_test_nearvar)
        kfree(board->v_plx_test_nearvar);

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, 
             "Unregister DMA interrupt handler.\n");
    fpga_unregister_irq(board, INT_30_PLX_DMA_0);
    fpga_unregister_irq(board, INT_31_PLX_DMA_1);
        
    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, "Exit FPGA interrupt function.\n");
    fpga_interrupt_exit(board);
}

static void plx_dma_test_handler(unsigned long data) 
{
    unsigned long regval;
    struct v2d_board *board = (struct v2d_board *) data;

    /* Get DMA control/status */
    regval = get_plx_val(board, PCI9056_DMA_COMMAND_STAT);

    /* Clear DMA interrupt */
    if (board->v_dma_test_chan == 0)
        set_plx_val(board, PCI9056_DMA_COMMAND_STAT, regval | (1 << 3));
    else
        set_plx_val(board, PCI9056_DMA_COMMAND_STAT, regval | (1 << 11));

    /* Re-enable PCI interrupt */
    plx_interrupt_enable(board, PLX_INT_TYPE_PCI_MAIN);

    /* Close DMA channel */
    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG,
             "Closing DMA channel %d\n", board->v_dma_test_chan);

    if (plx_block_dma_channel_close(board, board->v_dma_test_chan) < 0) 
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING, "Can't close DMA channel\n");
      
    if (board->dma_channel[board->v_dma_test_chan].flags | DMA_FLAG_READ) {
        pci_unmap_single(board->v_pci_dev, board->v_plx_test_nearvardma,
                         PLX_DMA_IO_SIZE, PCI_DMA_FROMDEVICE);
    }    
    else {
        pci_unmap_single(board->v_pci_dev, board->v_plx_test_nearvardma,
                         PLX_DMA_IO_SIZE, PCI_DMA_TODEVICE);
    }

    board->dma_channel[board->v_dma_test_chan].flags = 0;

}

static int plx_dma_test_transfer(struct v2d_board *board, 
                                 int chan,
                                 int dir,
                                 int numbytes,
                                 int wait_states,
                                 int burst_mode,
                                 unsigned long local_addr) {
    int rc = OK;
    unsigned long flags;

    /* Check board number */
    if (!board->v_hardware_detected) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING, 
                 "%s: invalid board number or board not detected!\n",
                 __FUNCTION__);
        return -EFAULT;
    }

    spin_lock_irqsave(&board->v_curr_buff_lock, flags);

    /* Check whether there's ongoing DMA */
    if (board->dma_channel[chan].flags) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING, 
                 "%s: board  DMA channel %d busy (flag = 0x%x)!\n",
                 __FUNCTION__,  chan,
                 board->dma_channel[chan].flags);
        spin_unlock_irqrestore(&board->v_curr_buff_lock, flags);
        return -EFAULT;
    }

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, 
             "Opening DMA channel %d...\n", chan);
    
    rc = plx_block_dma_channel_open(board, chan);
    if (rc != OK) {
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING, 
                 "Board failed to open DMA channel %d. rc = %d\n", 
                 chan, rc);
        spin_unlock_irqrestore(&board->v_curr_buff_lock, flags);
        return ERROR;
    } 

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, 
             "DMA channel %d opened.\n", chan);

    if (dir == DMA_DIR_TO_LOCAL) {
        board->v_plx_test_nearvardma = pci_map_single(board->v_pci_dev, 
                                                      board->v_plx_test_nearvar, 
                                                      PLX_DMA_IO_SIZE, 
                                                      PCI_DMA_TODEVICE);

        LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG,
                 "Starting DMA transfer to local on channel %d.\n", board->v_dma_test_chan);
        board->v_dma_numbytes = numbytes;
        board->v_dma_test_chan = chan;
        board->dma_channel[board->v_dma_test_chan].flags |= DMA_FLAG_WRITE;
        rc = plx_block_dma_transfer(board, 
                                    chan,                       /* channel */  
                                    (unsigned long) board->v_plx_test_nearvardma, 
                                    local_addr,         /* local addrss */
                                    numbytes,           /* bytes to transfer */
                                    DMA_DIR_TO_LOCAL);  /* direction */

        if (rc == OK)
            LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, "OK.\n");
        else if (rc != OK) { 
            LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING, "Error %d\n", rc);
            rc = ERROR;
        }
    }
    else {
        board->v_plx_test_nearvardma = pci_map_single(board->v_pci_dev, 
                                                      board->v_plx_test_nearvar, 
                                                      PLX_DMA_IO_SIZE, 
                                                      PCI_DMA_FROMDEVICE);
        LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG,
                 "Starting DMA transfer from local on channel %d.\n", board->v_dma_test_chan);
        board->dma_channel[board->v_dma_test_chan].flags |= DMA_FLAG_READ;
        board->v_dma_numbytes = numbytes;
        board->v_dma_test_chan = chan;
        rc = plx_block_dma_transfer(board, 
                                    chan,                       /* channel */  
                                    (unsigned long) board->v_plx_test_nearvardma, 
                                    local_addr,         /* local addrss */
                                    numbytes,           /* bytes to transfer */
                                    DMA_DIR_FROM_LOCAL);        /* direction */

        if (rc == OK)
            LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG, "OK.\n");
        else if (rc != OK) {
            LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING, "Error %d\n", rc);
            rc = ERROR;
        }
    }
    spin_unlock_irqrestore(&board->v_curr_buff_lock, flags);

    return rc;           
}


/*************************************************************
 * Routine to check if the PLX power management, hot-swap or *
 * VPD capability is enabled.                                *
 *************************************************************/
int plx_capability_enabled(struct v2d_board *board, int capability) {
    unsigned long regval, pmval, hsval, vpdval;

    LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO,
            "PLX %s -> %08x\n", __FUNCTION__, capability);


    /* Check if new capabilites are enabled */
    regval = get_plx_config_val(board, PCI9056_COMMAND);
    if ((regval & (1 << 20)) == 0) {
        return 0;
    }

    /* Get capabilities pointer & mask the irrelevant bits */
    regval = get_plx_config_val(board, PCI9056_CAP_PTR) & 0xff;
    if (regval == 0) {
        return 0;
    }
    
    /* Get PM, HS and VPD capabilities pointers & mask irrelevant bits */
    pmval = (get_plx_config_val(board, PCI9056_PM_CAP_ID) >> 8) & 0xff;
    hsval = (get_plx_config_val(board, PCI9056_HS_CAP_ID) >> 8) & 0xff;
    vpdval = (get_plx_config_val(board, PCI9056_VPD_CAP_ID) >> 8) & 0xff;

    if (capability == PCI9056_CAPABILITY_PM) {
        if (regval != PCI9056_PM_CAP_ID && pmval != PCI9056_PM_CAP_ID &&
            hsval != PCI9056_PM_CAP_ID && vpdval != PCI9056_PM_CAP_ID) {
            return 0;
        }
    }
    if (capability == PCI9056_CAPABILITY_HOTSWAP) {
        if (regval != PCI9056_HS_CAP_ID && pmval != PCI9056_HS_CAP_ID &&
            hsval != PCI9056_HS_CAP_ID && vpdval != PCI9056_HS_CAP_ID) {
            return 0;
        }
    }
    if (capability == PCI9056_CAPABILITY_VPD) {
        if (regval != PCI9056_VPD_CAP_ID && pmval != PCI9056_VPD_CAP_ID &&
            hsval != PCI9056_VPD_CAP_ID && vpdval != PCI9056_VPD_CAP_ID) {
            return 0;
        }
    }

    return 1;
}

/*************************************************
 * Routines to read, write VPD registers         *
 *************************************************/
int plx_vpd_read(struct v2d_board *board, unsigned long offset, unsigned long *value) {
    unsigned long regval, vpdstatus;
    int retries, pollcount;

    /* EEDO input must be disabled for VPD access */
    regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT);
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval & ~(1 << 31));

    /* Prepare VPD command */
    regval = (offset << 16) | 0x3;
 
    for (retries = 0; retries < VPD_COMMAND_MAX_RETRIES; retries++) {
        /* Send VPD command */
        set_plx_config_val(board, PCI9056_VPD_CAP_ID, regval);

        /* Poll until VPD operation has completed */
        for (pollcount = 0; pollcount < VPD_STATUS_MAX_POLL; pollcount++) {
            mdelay(VPD_STATUS_POLL_DELAY);
            vpdstatus = get_plx_config_val(board, PCI9056_VPD_CAP_ID);
            if (vpdstatus & (1 << 31)) {
                /* The VPD successfully read the EEPROM. Get */
                /* the VPD data result and return success.   */
                *value = get_plx_config_val(board, PCI9056_VPD_DATA);
                return OK;
            }
        }
    } 
    /* VPD access failed if we reach here. Return error. */
    return ERROR; 
}

int plx_vpd_write(struct v2d_board * board, unsigned long offset, unsigned long vpddata) {
    unsigned long regval, vpdstatus;
    int retries, pollcount;

    /* EEDO input must be disabled for VPD access */
    regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT);
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval & ~(1 << 31));

    /* Put write value into VPD data register */
    set_plx_config_val(board, PCI9056_VPD_DATA, vpddata);

    /* Prepare VPD command */
    regval = (1 << 31) | (offset << 16) | 0x3;
 
    for (retries = 0; retries < VPD_COMMAND_MAX_RETRIES; retries++) {
        /* Send VPD command */
        set_plx_config_val(board, PCI9056_VPD_CAP_ID, regval);

        /* Poll until VPD operation has completed */
        for (pollcount = 0; pollcount < VPD_STATUS_MAX_POLL; pollcount++) {
            mdelay(VPD_STATUS_POLL_DELAY);
            vpdstatus = get_plx_config_val(board, PCI9056_VPD_CAP_ID);
            if (vpdstatus & (1 << 31)) {
                /* The VPD successfully wrote to the EEPROM. */
                return OK;
            }
        }
    } 
    /* VPD access failed if we reach here. Return error. */
    return ERROR; 
}

/*************************************************
 * Routines to read, write and get EEPROM status *
 *************************************************/
static void plx_eeprom_clock(struct v2d_board *board) {
    int i;
    unsigned long regval;

    /* Set EEPROM clock high */
    regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT);
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval | (1 << 24));

    /* Do dummy register reads so we have a small delay */
    for (i = 0; i < 20; i++) 
        regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT);

    /* Set EEPROM clock low */
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval & ~(1 << 24));
}

static void plx_eeprom_command(struct v2d_board *board, unsigned long cmd) {
    int bitpos;
    unsigned long regval;

    /* Clear all EEPROM bits */
    regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT) & ~(0xf << 24);

    /* Toggle EEPROM's chip select to get it out of shift register mode */
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);

    /* Enable EEPROM chip select */
    regval |= (1 << 25);
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);

    /* Send EEPROM command one bit at a time */
    for (bitpos = EE_CMD_LEN - 1; bitpos >= 0; bitpos--) {
        if (cmd & (1 << bitpos)) 
            set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval | (1 << 26));
        else
            set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);

        /* Trigger EEPROM clock */
        plx_eeprom_clock(board);
    }
}

static void set_plx_eeprom_val(struct v2d_board *board, int offset, unsigned long val) {

    int bitpos, i, timeout = 0;
    unsigned short eeprom_value;
    unsigned long regval, regsave;

    if (!board->v_hardware_detected ||
        offset & 0x03 || offset >= 0x200)
        return;

    /* Unprotect the EEPROM for write access */
    regsave = get_plx_val(board, PCI9056_ENDIAN_DESC);
    set_plx_val(board, PCI9056_ENDIAN_DESC, regsave & ~(0xff << 16));

    /* Write EEPROM 16 bits at a time */
    for (i = 0; i < 2; i++) {
        if (i == 1) {
            eeprom_value = (unsigned short) (val >> 16);
            offset += sizeof (unsigned short);
        }
        else {
            eeprom_value = (unsigned short) val;
        }

        /* Send write_enable command to EEPROM */
        plx_eeprom_command(board, (EE_WREN << EE_CMD_SHIFT));

        /* Send EEPROM write command and offset to EEPROM */
        plx_eeprom_command(board, (EE_WRITE << EE_CMD_SHIFT) | (offset / 2));

        /* Clear all EEPROM bits */
        regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT) & ~(0xf << 24);

        /* Make sure EEDO input disabled (required for some PLX chips) */
        regval &= ~(1 << 31);

        /* Enable EEPROM chip select */
        regval |= (1 << 25);

        /* Write 16 bit value to EEPROM one bit at at time */
        for (bitpos = 15; bitpos >= 0; bitpos--) {
            if (eeprom_value & (1 << bitpos))
                set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval | (1 << 26));
            else
                set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);

            /* Trigger EEPROM clock */
            plx_eeprom_clock(board);
        }

        /* De-select the chip */
        set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval & ~(1 << 25));

        /* Re-select the chip */
        set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval | (1 << 25));
    
        /* A small delay is needed after the last bit to let EEPROM complete */
        do {
            regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT);
            timeout++;
        } while ((regval & (1 << 27)) == 0 && timeout < 20000);

        /* Send write disable command to EEPROM */
        plx_eeprom_command(board, (EE_WDS << EE_CMD_SHIFT));
    
        /* Clear chip select and all other EEPROM bits */
        set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval & ~(0xf << 24));
    }

    /* Restore EEPROM write-protect address boundary */
    set_plx_val(board, PCI9056_ENDIAN_DESC, regsave);
}

static unsigned long get_plx_eeprom_val(struct v2d_board *board, int offset) {

    int bitpos, i;
    unsigned long regval, result = 0;

    if (!board->v_hardware_detected ||
        offset & 0x03 || offset >= 0x200)
        return 0;

    /* Send EEPROM read command and offset to EEPROM */
    plx_eeprom_command(board, (EE_READ << EE_CMD_SHIFT) | (offset / 2));

    /* Set EEPROM EEDO input enable (required for some chips) */
    regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT) | (1 << 31);
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);

    /* Get 32 bits from EEPROM - one bit at a time */
    for (bitpos = 0; bitpos < 32; bitpos++) {

        /* Trigger EEPROM clock */
        plx_eeprom_clock(board);

        /* Delay */
        for (i = 0; i < 20; i++) 
            regval = get_plx_val(board, PCI9056_EEPROM_CTRL_STAT);

        /* Get bit value and shift into result */
        if (regval & (1 << 27)) 
            result = (result << 1) | 1;
        else 
            result = (result << 1);
    }    

    /* Clear EEDO input enable, chip select and all other EEPROM bits */
    regval &= ~(0xf << 24) & ~(1 << 31); 
    set_plx_val(board, PCI9056_EEPROM_CTRL_STAT, regval);
    
    /* Swap the upper & lower 16 bits so the bytes are in big endian order */
    return (result << 16) | ((result & 0xffff0000) >> 16);
}

/********************************************
 * PLX 9056 control initialization function *
 ********************************************/
int plx_control_init(struct v2d_board *board) {
    int i;
    unsigned long regval;

    if (!board->v_hardware_detected)
        return -1;
    LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO, 
             "%s\n", __FUNCTION__);

    /********************************
     * Initialize PLX DMA resources *
     ********************************/
    for (i = 0; i < MAX_DMA_CHANNELS; i++) {
        board->dma_channel[i].state = DMA_STATE_CLOSED;
        board->dma_channel[i].flags = 0;
    }    
    
    /************************
     * Initialize spin lock *
     ************************/
    spin_lock_init(&board->v_plx_isr_lock);

    /*****************************
     * Initialize board revision *
     *****************************/
    board->v_hardware_version = -1;

    regval = get_plx_config_val(board, PCI9056_VPD_CAP_ID);

    if (plx_capability_enabled(board, PCI9056_CAPABILITY_VPD)) {
        unsigned long regsave;
        regsave = get_plx_config_val(board, PCI9056_VPD_CAP_ID);
        if (((regval >> 8) & 0xff) == ((regsave >> 8) & 0xff)) {
            if (plx_vpd_read(board, 0, &regval) == OK && regval == PCI9056_ID) {

                unsigned short writeprotect =
                    ((get_plx_val(board, PCI9056_ENDIAN_DESC) >> 16) & 0xff) * 4;

                if (plx_vpd_read(board, writeprotect+VPD_DATE_SIZE+
                                 VPD_SERIAL_SIZE, &regval) >= 0) {
                    board->v_hardware_version = 
                        HARDWARE_VERSION_PLX_MASK | VPD_MAJ_REV(regval);
                }
            }
        }
    }

    /*************************************************************
     * Set local addr space 0 and 1 bus region descriptor regs   *
     * for Rev. 2 hardware. (Rev. 1 uses default from EEPROM).   *
     *************************************************************/
    if (board->v_hardware_version == HARDWARE_VERSION_PLX_V2) {
        /* 32-bit, 3 wait states for local addr space 0 */
        set_plx_val(board, PCI9056_SPACE0_ROM_DESC,
                    (get_plx_val(board, PCI9056_SPACE0_ROM_DESC) | 0x0f));
        /* 32-bit, 2 wait states for local addr space 1 */
        set_plx_val(board, PCI9056_SPACE1_DESC, 0x0b);
    }

    /*****************************
     * Setup DMA thresholds to 0 *
     *****************************/
    set_plx_val(board, PCI9056_DMA_THRESHOLD, 0x0e0e0e0e);

    /*************************************************************
     * Write DMA mode (should be 0x205c3 for 32 bit bus, 0 wait  * 
     * state, enable ready# and continuous burst, hold local bus * 
     * address constant or 0x20504 for 32 bit bus, 1 wait state, *
     * disable ready# and continuous burst, enable local burst.  *
     *************************************************************/
    switch (board->v_hardware_version) {
    case HARDWARE_VERSION_PLX_V1:
        set_plx_val(board, PCI9056_DMA0_MODE, 0x20504); 
        set_plx_val(board, PCI9056_DMA1_MODE, 0x20504); 
    case HARDWARE_VERSION_PLX_V2:
    default:
        set_plx_val(board, PCI9056_DMA0_MODE, 0x20dc3); 
        set_plx_val(board, PCI9056_DMA1_MODE, 0x20dc3); 
        break;
    }

    /******************************
     * Write PCI DMA address high *
     ******************************/
    set_plx_val(board, PCI9056_DMA0_PCI_DAC, 0);
    set_plx_val(board, PCI9056_DMA1_PCI_DAC, 0);

    return 0;
}

/********************************************
 * PLX 9056 control finalization function *
 ********************************************/
void plx_control_exit(struct v2d_board *board) {

    return;
}

/********************************************
 * Ioctl for the PLX 9056 PCI Bus Interface *
 ********************************************/
int plx_control_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data) {

    struct reg_io regio, *regiop;
    struct plx_io  plxtestio, *plxtestiop;
    int num;

    if (!board->v_hardware_detected) 
        return -EFAULT;


    switch (cmd) {
    case FPGA_IOC_MOD_PLX:
        if (((regiop = (struct reg_io *) data) == NULL) ||
            (copy_from_user(&regio, regiop, sizeof(struct reg_io))))
            return -EFAULT;

        switch (regio.type) {

        case REGIO_TYPE_PLX_GET:
            regio.value = get_plx_val(board, regio.reg);
            LOGPRINT(LOG_MOD_PLX | LOG_LEV_INFO,
                "PLX %s 0x%08lx->0x%08lx\n", __FUNCTION__, regio.reg, regio.value);
            break;

        case REGIO_TYPE_PLX_SET:
            set_plx_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_PCICONFIG_GET:
            regio.value = get_plx_config_val(board, regio.reg);
            break;

        case REGIO_TYPE_PCICONFIG_SET:
            set_plx_config_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_EEPROM_GET:
            regio.value = get_plx_eeprom_val(board, regio.reg);
            break;
        
        case REGIO_TYPE_EEPROM_SET:
            set_plx_eeprom_val(board, regio.reg, regio.value);
            break;

        case REGIO_TYPE_VPD_READ:
            if (plx_vpd_read(board, regio.reg, &regio.value) < 0) {
                LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG,
                         "Failed to read VPD offset 0x%08lx\n", regio.reg);
                regio.value = 0xffffffff;
            }
            break;

        case REGIO_TYPE_VPD_WRITE:
            if (plx_vpd_write(board, regio.reg, regio.value) < 0) {
                LOGPRINT(LOG_MOD_PLX | LOG_LEV_DEBUG,
                         "Failed to read VPD offset 0x%08lx\n", regio.reg);
            }
            break;

        default:
            return -EFAULT;
        }

        if (copy_to_user((struct reg_io *)data, 
                         &regio, sizeof(struct reg_io)) != 0)
            return -EFAULT;

        return 0;

    case FPGA_IOC_GET_PLXCAP:
        if ((int *)data == NULL) {
            return -EFAULT;
        }

        num = 0;
        if (plx_capability_enabled(board, PCI9056_CAPABILITY_PM)) 
            num |= PCI9056_CAPABILITY_PM; 
        if (plx_capability_enabled(board, PCI9056_CAPABILITY_HOTSWAP)) 
            num |= PCI9056_CAPABILITY_HOTSWAP; 
        if (plx_capability_enabled(board, PCI9056_CAPABILITY_VPD)) 
            num |= PCI9056_CAPABILITY_VPD; 

        if (copy_to_user((int *) data, &num, sizeof(int)) != 0)
            return -EFAULT;

        return 0;

    case FPGA_IOC_RESET_PLX:
        printk("Reseting PLX chip on board %p\n", board);
        plx_board_reset(board);
        return 0;

    case FPGA_IOC_PLX_TEST:

        if (((plxtestiop = (struct plx_io *) data) == NULL) ||
            (copy_from_user(&plxtestio, plxtestiop, sizeof(struct plx_io))))
            return -EFAULT;

        switch (plxtestio.type) {

        case REGIO_TYPE_PLX_TEST_DMA:
        { 
            int i, j;
            unsigned long addr;

            addr = (unsigned long) plxtestio.addr_hi;
            addr = (addr << 16) | (unsigned long) plxtestio.addr_lo;

            plx_dma_test_init(board);
            for (i = 0; i < plxtestio.repeat; i++) {
                plx_dma_test_transfer(board, 
                                      plxtestio.chan, 
                                      plxtestio.dir, 
                                      plxtestio.numbytes, 
                                      plxtestio.wait_states, 
                                      plxtestio.burst_mode, 
                                      addr);
                /* wait till DMA done or time out */
                for (j = 0; j < 1000; j++) {
                    if (board->dma_channel[plxtestio.chan].flags) 
                        udelay(1);
                    else
                        break;
                }
                if (board->dma_channel[plxtestio.chan].flags) {
                    LOGPRINT(LOG_MOD_PLX | LOG_LEV_WARNING, 
                             "Timeout waiting for DMA done interrupt!\n");
                    break;
                }
            }
            if (plxtestio.dir == DMA_DIR_FROM_LOCAL)
                if (copy_to_user(plxtestiop->dat, board->v_plx_test_nearvar, 
                                 plxtestiop->numbytes) != 0) {
                    printk ("%s:%d failed\n", __FUNCTION__, __LINE__);
                }
            plx_dma_test_exit(board);

        }
        return 0;

        default:
            return -EFAULT;
        }

        return -EFAULT;

    default:
        break;
    }

    return -EINVAL;
}

